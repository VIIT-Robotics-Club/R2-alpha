#include "esp_log.h"
#include <driver/gpio.h>
#include "lf_pub.hpp"

#define TAG "LSA_TEST"

gpio_num_t rx = GPIO_NUM_40;
gpio_num_t tx = GPIO_NUM_41;
gpio_num_t en = GPIO_NUM_39;
gpio_num_t trg = GPIO_NUM_38;

lsaHandler* lf_pub::uarthandle = 0;
lf_pub* lf_pub::lf = 0;

int lf_pub::junction_count = 0;

const char * topic_name = "/line_lsa";
const char * topic_name_junction = "/junction";

// message type support
const rosidl_message_type_support_t * lsa_type_support = ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32);

lf_pub::lf_pub() {
    if(!uarthandle) uarthandle = new lsaHandler(tx, rx, en, trg);
    lf = this;
}

void lf_pub::init() { 
    ESP_LOGI(TAG, "Initializing line following publisher...");
    rclc_publisher_init_default(&lsaPublisher, node, lsa_type_support, topic_name);
    rclc_publisher_init_default(&lsaJunctionPub, node, lsa_type_support, topic_name_junction);

    rclc_timer_init_default(&timer, support, RCL_MS_TO_NS(100), lf_pub::timer_callback);

    rclc_executor_add_timer(exec, &timer);
    ESP_LOGI(TAG, "publisher initialization successful");
}

void lf_pub::timer_callback(rcl_timer_s* time, int64_t num) {

    uarthandle->update();

    std_msgs__msg__Int32 msg;
    std_msgs__msg__Int32__init(&msg);
    msg.data = uarthandle->value;

    std_msgs__msg__Int32 junction_msg;
    std_msgs__msg__Int32__init(&junction_msg);
    
    if (uarthandle->trigger) {
        junction_count++;
    }

    junction_msg.data = junction_count;

    rcl_publish(&lf->lsaPublisher, &msg, NULL);
    rcl_publish(&lf->lsaJunctionPub, &junction_msg, NULL);
}
#include "lf_pub.hpp"

#include "esp_log.h"
#include <driver/gpio.h>


#include <std_msgs/msg/int32.h>


#include "pinMap.hpp"

#define TAG "LSA"


lsaHandler* lf_pub::uarthandle = 0;
lf_pub* lf_pub::lf = 0;


const char * topic_name = "/line_lsa";
const char * topic_name_junction = "/junction";

// message type support
const rosidl_message_type_support_t * lsa_type_support = ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32);


lf_pub::lf_pub(int hz) : frequency(hz) {
    if(!uarthandle) uarthandle = new lsaHandler(LSA_TX, LSA_RX, LSA_EN, LSA_TRG);
    lf = this;
}

void lf_pub::init() { 
    ESP_LOGI(TAG, "Initializing line following publisher...");
    rclc_publisher_init_default(&lsaPublisher, node, lsa_type_support, topic_name);
    rclc_publisher_init_default(&lsaJunctionPub, node, lsa_type_support, topic_name_junction);

    rclc_timer_init_default(&timer, support, RCL_MS_TO_NS(1000 / frequency), lf_pub::timer_callback);

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
        lf->junctionCount++;
        uarthandle->trigger = false;
    }

    junction_msg.data = lf->junctionCount;

    rcl_publish(&lf->lsaPublisher, &msg, NULL);
    rcl_publish(&lf->lsaJunctionPub, &junction_msg, NULL);
}
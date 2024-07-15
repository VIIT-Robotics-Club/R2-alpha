#include "lunaPub.hpp"
#include "lunaHandler.hpp"
#include <esp_log.h>
#include <rclc/publisher.h>
#include <rmw_microros/rmw_microros.h>
#include <std_msgs/msg/int32.h>

#include "pinMap.hpp"

#define LOG "luna"

uint8_t lunaAddresses[LUNA_COUNT] = {
    0x14,
    0x11,
    0x10,
    0x12,
    0x13,
    0x15
};

const rosidl_message_type_support_t * luna_type_support = ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32);

lunaHandler* lunaPub::luna_handler = 0;
lunaPub* lunaPub::luna_pub = 0;

lunaPub::lunaPub(int freq) {
    luna_pub = this;
    luna_handler =  new lunaHandler(A_LUNA_SCL, A_LUNA_SDA, lunaAddresses, 6);
    pubDelayMs = 1000.0f / (float) freq;
}

void lunaPub::init() {

    rclc_publisher_init_default(&lunaPublisher[0], node, luna_type_support, "luna_fr");
    rclc_publisher_init_default(&lunaPublisher[1], node, luna_type_support, "luna_fl");
    rclc_publisher_init_default(&lunaPublisher[2], node, luna_type_support, "luna_rf");
    rclc_publisher_init_default(&lunaPublisher[3], node, luna_type_support, "luna_rb");
    rclc_publisher_init_default(&lunaPublisher[4], node, luna_type_support, "luna_lf");
    rclc_publisher_init_default(&lunaPublisher[5], node, luna_type_support, "luna_lb");
    // rclc_publisher_init_best_effort(&lunaPublisher1, node, luna_type_support, "luna_fr");
    // rclc_publisher_init_best_effort(&lunaPublisher2, node, luna_type_support, "luna_fl");
    // rclc_publisher_init_best_effort(&lunaPublisher3, node, luna_type_support, "luna_rf");
    // rclc_publisher_init_best_effort(&lunaPublisher4, node, luna_type_support, "luna_rb");

    rclc_timer_init_default(&timer, support, RCL_MS_TO_NS(pubDelayMs), lunaPub::luna_callback);
    rclc_executor_add_timer(exec, &timer);
}

void lunaPub::luna_callback(rcl_timer_s* timer, int64_t num) {
    // Initialize the message 

    luna_handler->update();

    for(int i=0; i < LUNA_COUNT; i++){
        std_msgs__msg__Int32 luna_msg;
        std_msgs__msg__Int32__init(&luna_msg);
        luna_msg.data = luna_handler->distances[i];
        rcl_publish(&luna_pub->lunaPublisher[i], &luna_msg, NULL);
    }

    
    // Assign the first distance value to the message data
    // // For Testing purpose
    // luna_msg1.data = 10;
    // luna_msg2.data = 20;
    // luna_msg3.data = 30;
    // luna_msg4.data = 40;

    // Publish the message
}

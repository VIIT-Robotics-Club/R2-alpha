#include "lunaPub.hpp"
#include "lunaHandler.hpp"
#include <esp_log.h>
#include <rclc/publisher.h>
#include <rmw_microros/rmw_microros.h>
#include <std_msgs/msg/int32.h>

#define LOG "luna"

const rosidl_message_type_support_t * luna_type_support = ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32);

lunaHandler* lunaPub::luna_handler = 0;
lunaPub* lunaPub::luna_pub = 0;

lunaPub::lunaPub() {
    luna_pub = this;
    luna_handler =  new lunaHandler();
}

void lunaPub::init() {
    rclc_publisher_init_default(&lunaPublisher1, node, luna_type_support, "luna_fr");
    rclc_publisher_init_default(&lunaPublisher2, node, luna_type_support, "luna_fl");
    rclc_publisher_init_default(&lunaPublisher3, node, luna_type_support, "luna_rf");
    rclc_publisher_init_default(&lunaPublisher4, node, luna_type_support, "luna_rb");

    rclc_timer_init_default(&timer, support, RCL_MS_TO_NS(100), lunaPub::luna_callback);
    rclc_executor_add_timer(exec, &timer);
}

void lunaPub::luna_callback(rcl_timer_s* timer, int64_t num) {
    // Initialize the message 
    std_msgs__msg__Int32 luna_msg1;
    std_msgs__msg__Int32 luna_msg2;
    std_msgs__msg__Int32 luna_msg3;
    std_msgs__msg__Int32 luna_msg4;

    std_msgs__msg__Int32__init(&luna_msg1);
    std_msgs__msg__Int32__init(&luna_msg2);
    std_msgs__msg__Int32__init(&luna_msg3);
    std_msgs__msg__Int32__init(&luna_msg4);
    
    luna_handler->update();
    
    // Assign the first distance value to the message data
    luna_msg1.data = luna_handler->distances[0];
    luna_msg2.data = luna_handler->distances[1];
    luna_msg3.data = luna_handler->distances[2];
    luna_msg4.data = luna_handler->distances[3];    
    
    // // For Testing purpose
    // luna_msg1.data = 10;
    // luna_msg2.data = 20;
    // luna_msg3.data = 30;
    // luna_msg4.data = 40;

    // Publish the message
    rcl_ret_t ret;

    rcl_publish(&luna_pub->lunaPublisher1, &luna_msg1, NULL);
    rcl_publish(&luna_pub->lunaPublisher2, &luna_msg2, NULL);
    rcl_publish(&luna_pub->lunaPublisher3, &luna_msg3, NULL);
    rcl_publish(&luna_pub->lunaPublisher4, &luna_msg4, NULL);
}

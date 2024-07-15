#include "clawServer.hpp"
#include "esp_intr_alloc.h"
#include "esp_attr.h"
#include "esp_intr_types.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "std_srvs/srv/set_bool.h"
#include "std_msgs/msg/bool.h"
#include <rmw_microros/rmw_microros.h>

#include <inttypes.h>

#include <pinMap.hpp>


// GPIO & Constants Declarations
const char* TAG = "gripper service";
const char* CLAW_TAG = "Claw";
bool triggered = false;

qmd* clawServer::handler = 0;
clawServer* clawServer::current = 0;

const rosidl_service_type_support_t * lift_service_type_support = ROSIDL_GET_SRV_TYPE_SUPPORT(std_srvs, srv, SetBool);
const rosidl_service_type_support_t * claw_service_type_support = ROSIDL_GET_SRV_TYPE_SUPPORT(std_srvs, srv, SetBool);
const rosidl_message_type_support_t * gotcha_type_support = ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Bool);


static char resMsg = '\0';

clawServer::clawServer(qmd* handler, int freq){
    current = this;
    clawServer::handler = handler;

    // configure gpio pins for trigger
    gpio_config_t triggerConf {
        .pin_bit_mask = (uint64_t)((1 << LIFT_UPPER_INTR) | (1 << LIFT_LOWER_INTR) |  (1 << OBJECT_LIMIT_SW)),
        .mode = GPIO_MODE_INPUT,
        // .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .intr_type = GPIO_INTR_ANYEDGE,
    };

    gpio_config(&triggerConf);

    gpio_install_isr_service( ESP_INTR_FLAG_SHARED | ESP_INTR_FLAG_LOWMED);
    // gpio_install_isr_service(ESP_INTR_FLAG_LOWMED);

    std_srvs__srv__SetBool_Request__init(&req_lift);
    std_srvs__srv__SetBool_Response__init(&res_lift);
    std_srvs__srv__SetBool_Request__init(&req_claw);
    std_srvs__srv__SetBool_Response__init(&res_claw);


    res_claw.message.data = &resMsg;
    res_claw.message.size = 1;
    res_claw.message.capacity = 1;

    gpio_isr_handler_add(LIFT_UPPER_INTR, gpio_upper_isr_handler,  this);
    gpio_isr_handler_add(LIFT_LOWER_INTR, gpio_lower_isr_handler, this);

    // Enable the interrupts
    gpio_intr_enable(LIFT_UPPER_INTR);
    gpio_intr_enable(LIFT_LOWER_INTR);

    updateState();
    // setup task to handler interrupt processing
    xTaskCreate(stopClawTask, "clawServerStopTask", 4096, this, 1, &clawStopThreadHandle);

    pubDelayMs = 1000.0f / (float) freq;
}

void clawServer::init() {

    if (rclc_service_init_default(&lift_service, node, lift_service_type_support, "gripper_lift") != RCL_RET_OK) {
        ESP_LOGE(TAG, "Failed to initialize lift service");
    }

    if (rclc_service_init_default(&claw_service, node, claw_service_type_support, "gripper_grab") != RCL_RET_OK) {
        ESP_LOGE(TAG, "Failed to initialize claw service");
    }

    rclc_executor_add_service_with_request_id(exec, &claw_service, &req_claw, &res_claw, &service_callback_grab);
    rclc_executor_add_service_with_request_id(exec, &lift_service, &req_lift, &res_lift, &service_callback_lift);
    ESP_LOGI(TAG, "lift and grab Services has been  Initialized ");


    rclc_publisher_init_default(&gotchaPub, node, gotcha_type_support, "gotcha");
    rclc_timer_init_default(&timer, support, RCL_MS_TO_NS(pubDelayMs), clawServer::gotchaCallback);
    rclc_executor_add_timer(exec, &timer);

}



void clawServer::gotchaCallback(rcl_timer_s* timer, int64_t num) {
    // Initialize the message 

    std_msgs__msg__Bool gotcha_msg;
    std_msgs__msg__Bool__init(&gotcha_msg);
    gotcha_msg.data = gpio_get_level(OBJECT_LIMIT_SW);

    rcl_publish(&current->gotchaPub, &gotcha_msg, NULL);

    // Assign the first distance value to the message data
    // // For Testing purpose
    // luna_msg1.data = 10;
    // luna_msg2.data = 20;
    // luna_msg3.data = 30;
    // luna_msg4.data = 40;

    // Publish the message
}

// Service to handle lift movement
void clawServer::service_callback_lift(const void * req, rmw_request_id_t * reqId, void * res){

    std_srvs__srv__SetBool_Request * req_lift=(std_srvs__srv__SetBool_Request *) req;
    std_srvs__srv__SetBool_Response * res_lift=(std_srvs__srv__SetBool_Response *) res;

    current->updateState();

    if(req_lift->data != current->state){

        if((current->headingUp = req_lift->data)){
            // ESP_LOGI(TAG,"Lift Going Up last state %f",  handler->speeds[4]);
            ESP_LOGI(TAG,"Lift Going Up");
            handler->speeds[LIFT_QMD_INDEX] = LIFT_SPEED;
        }
        else {
            handler->speeds[LIFT_QMD_INDEX] = -LIFT_SPEED; // Start motor to move downward at max speed
            ESP_LOGI(TAG,"Lift Going Down");
        }

        handler->update();
    }
    res_lift->success = true;
    rcl_ret_t result = rcl_send_response(&current->lift_service, reqId, res_lift);
}

// Service to handle claw movement
void clawServer::service_callback_grab(const void * req1, rmw_request_id_t * reqId,  void * res1){
  
    std_srvs__srv__SetBool_Request * req_claw=(std_srvs__srv__SetBool_Request *) req1;
    std_srvs__srv__SetBool_Response * res_claw=(std_srvs__srv__SetBool_Response *) res1;
    
    if(req_claw->data){
        ESP_LOGI(CLAW_TAG,"closing claw");
        handler->speeds[GRAB_QMD_INDEX] = -GRAB_FORCE;
        handler->update();

        vTaskDelay(pdMS_TO_TICKS(GRAB_ACTIVE_TIME));
        handler->speeds[GRAB_QMD_INDEX] = 0.0f;
        handler->update();

    }
    else {

        ESP_LOGI(CLAW_TAG,"opening claw");
        handler->speeds[GRAB_QMD_INDEX] = GRAB_FORCE;
        handler->update();

        vTaskDelay(pdMS_TO_TICKS(GRAB_ACTIVE_TIME));
        handler->speeds[GRAB_QMD_INDEX] = 0.0f;
        handler->update();
    }

    // return result based on whether limit switch is hit

    res_claw->success = gpio_get_level(OBJECT_LIMIT_SW);
    rcl_ret_t result = rcl_send_response(&current->claw_service, reqId, res_claw);
    // if(result){
    //     ESP_LOGI(CLAW_TAG,"failed to send response %ld", result);
    // }
}

// int isrCount = 0;

void clawServer::stopClawTask(void *ptr){

    clawServer* obj = (clawServer*) ptr;

    while(1){
        xTaskNotifyWait(0, 0x00, nullptr, portMAX_DELAY);

        // maintain time difference between adjacent interrupts and filter based on threshold filter
        std::chrono::milliseconds now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
        obj->diff = now - obj->lastTick;
        obj->lastTick = now;

        if(obj->diff.count() > LIFT_ISR_TIME_DIFF_THRESH) continue; 

        // stop lift motor
        current->handler->speeds[LIFT_QMD_INDEX] = 0.0f;
        current->handler->update();
        // ESP_LOGI(TAG, "logical interrupt called, motor lift stopped");
        vTaskDelay(pdMS_TO_TICKS(10));
    };

    // un-init this thread
    vTaskDelete(NULL);

}


bool isrActive = false;
// Interrupt Service Routine to Stop Lift Motion
void clawServer::gpio_upper_isr_handler(void* arg) {
    if(isrActive) return;

    clawServer* srv = (clawServer*) arg;
    isrActive = true;
    // isrCount++;
    if(srv->headingUp) vTaskNotifyGiveFromISR(current->clawStopThreadHandle, NULL);

    isrActive = false;
}


// Interrupt Service Routine to Stop Lift Motion
void clawServer::gpio_lower_isr_handler(void* arg) {
    if(isrActive) return;

    clawServer* srv = (clawServer*) arg;
    isrActive = true;
    // isrCount++;

    if(!srv->headingUp) vTaskNotifyGiveFromISR(current->clawStopThreadHandle, NULL);

    isrActive = false;
}


void clawServer::updateState(){

    int upperHit, lowerHit;
    upperHit = gpio_get_level(LIFT_UPPER_INTR);
    lowerHit = gpio_get_level(LIFT_LOWER_INTR);

    state = (upperHit) ? 1 : ((lowerHit) ? 0 : -1);
};
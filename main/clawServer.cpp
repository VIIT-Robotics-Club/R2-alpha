#include "clawServer.hpp"
#include "esp_intr_alloc.h"
#include "esp_attr.h"
#include "esp_intr_types.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "std_srvs/srv/set_bool.h"
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

clawServer::clawServer(qmd* handler) {
    current = this;
    clawServer::handler = handler;

    // configure gpio pins for trigger
    gpio_config_t triggerConf {
        .pin_bit_mask = (uint64_t)((1 << LIFT_UPPER_INTR) | (1 << LIFT_LOWER_INTR)),
        .mode = GPIO_MODE_INPUT,
        // .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .intr_type = GPIO_INTR_POSEDGE,
    };

    gpio_config(&triggerConf);

    gpio_install_isr_service( ESP_INTR_FLAG_SHARED | ESP_INTR_FLAG_LOWMED);
    // gpio_install_isr_service(ESP_INTR_FLAG_LOWMED);

    std_srvs__srv__SetBool_Request__init(&req_lift);
    std_srvs__srv__SetBool_Response__init(&res_lift);
    std_srvs__srv__SetBool_Request__init(&req_claw);
    std_srvs__srv__SetBool_Response__init(&res_claw);

    gpio_isr_handler_add(LIFT_UPPER_INTR, gpio_upper_isr_handler,  this);
    gpio_isr_handler_add(LIFT_LOWER_INTR, gpio_lower_isr_handler, this);

    // Enable the interrupts
    gpio_intr_enable(LIFT_UPPER_INTR);
    gpio_intr_enable(LIFT_LOWER_INTR);


    // setup task to handler interrupt processing
    xTaskCreate(stopClawTask, "clawServerStopTask", 2048, this, 1, &clawStopThreadHandle);
}

void clawServer::init() {

    if (rclc_service_init_default(&lift_service, node, lift_service_type_support, "gripper_lift") != RCL_RET_OK) {
        ESP_LOGE(TAG, "Failed to initialize lift service");
    }

    if (rclc_service_init_default(&claw_service, node, claw_service_type_support, "gripper_grab") != RCL_RET_OK) {
        ESP_LOGE(TAG, "Failed to initialize claw service");
    }

    rclc_executor_add_service_with_request_id(exec, &claw_service, &req_claw, &res_claw, service_callback_grab);
    rclc_executor_add_service_with_request_id(exec, &lift_service, &req_lift, &res_lift, service_callback_lift);
    ESP_LOGI(TAG, "lift and grab Services has been  Initialized ");


}


// Service to handle lift movement
void clawServer::service_callback_lift(const void * req, rmw_request_id_t * reqId, void * res){

    std_srvs__srv__SetBool_Request * req_lift=(std_srvs__srv__SetBool_Request *) req;
    std_srvs__srv__SetBool_Response * res_lift=(std_srvs__srv__SetBool_Response *) res;

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
    res_lift->success = true;
    rcl_ret_t result = rcl_send_response(&current->lift_service, reqId, res_lift);
}

// Service to handle claw movement
void clawServer::service_callback_grab(const void * req1, rmw_request_id_t * reqId,  void * res1){
  
    std_srvs__srv__SetBool_Request * req_claw=(std_srvs__srv__SetBool_Request *) req1;
    std_srvs__srv__SetBool_Response * res_claw=(std_srvs__srv__SetBool_Response *) res1;

    if(req_claw->data){
        ESP_LOGI(CLAW_TAG,"closing claw");
        handler->speeds[GRAB_QMD_INDEX] = GRAB_FORCE;
        handler->update();

        vTaskDelay(pdMS_TO_TICKS(GRAB_ACTIVE_TIME));
        handler->speeds[GRAB_QMD_INDEX] = 0.0f;
        handler->update();

    }
    else {

        ESP_LOGI(CLAW_TAG,"opening claw");
        handler->speeds[GRAB_QMD_INDEX] = -GRAB_FORCE;
        handler->update();

        vTaskDelay(pdMS_TO_TICKS(GRAB_ACTIVE_TIME));
        handler->speeds[GRAB_QMD_INDEX] = 0.0f;
        handler->update();
    }

    res_claw->success = true;
    rcl_ret_t result = rcl_send_response(&current->claw_service, reqId, res_claw);
}

int isrCount = 0;

void clawServer::stopClawTask(void *ptr){

    while(1){
        xTaskNotifyWait(0, 0x00, nullptr, portMAX_DELAY);
        current->handler->speeds[LIFT_QMD_INDEX] = 0.0f;
        current->handler->update();
        ESP_LOGI(TAG, "logical interrupt called, motor lift stopped %d", isrCount);
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
    isrCount++;
    if(srv->headingUp) vTaskNotifyGiveFromISR(current->clawStopThreadHandle, NULL);

    isrActive = false;
}


// Interrupt Service Routine to Stop Lift Motion
void clawServer::gpio_lower_isr_handler(void* arg) {
    if(isrActive) return;

    clawServer* srv = (clawServer*) arg;
    isrActive = true;
    isrCount++;

    if(!srv->headingUp) vTaskNotifyGiveFromISR(current->clawStopThreadHandle, NULL);

    isrActive = false;
}

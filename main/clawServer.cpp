#include "clawServer.hpp"
#include "esp_intr_alloc.h"
#include "esp_attr.h"
#include "esp_intr_types.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "std_srvs/srv/set_bool.h"
#include <rmw_microros/rmw_microros.h>

#include <inttypes.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
// #include "qmd.hpp"

// GPIO & Constants Declarations
const char* TAG = "shared_interrupt";
const char* CLAW_TAG = "Claw";
bool triggered = false;



const rosidl_service_type_support_t * lift_service_type_support = ROSIDL_GET_SRV_TYPE_SUPPORT(std_srvs, srv, SetBool);
const rosidl_service_type_support_t * claw_service_type_support = ROSIDL_GET_SRV_TYPE_SUPPORT(std_srvs, srv, SetBool);

clawServer::clawServer() {
    
    // Configure GPIO_INPUT_PIN1
    gpio_config_t io_conf1 = configure_interrupt(GPIO_NUM_4);
    gpio_config(&io_conf1);

    // Configure GPIO_INPUT_PIN2
    gpio_config_t io_conf2 = configure_interrupt(GPIO_NUM_19);
    gpio_config(&io_conf2);


    // Install GPIO ISR service with shared interrupt flag
    gpio_install_isr_service(ESP_INTR_FLAG_SHARED);

    // Attach the shared interrupt service routine to GPIO pins
    gpio_isr_handler_add(GPIO_NUM_4, gpio_isr_handler, (void *) GPIO_NUM_4);
    gpio_isr_handler_add(GPIO_NUM_19, gpio_isr_handler,(void *) GPIO_NUM_19);

    // Enable the interrupts
    gpio_intr_enable(GPIO_NUM_4);
    gpio_intr_enable(GPIO_NUM_19);
     // Log
    ESP_LOGI(TAG, "Shared interrupt configured for GPIO %d and GPIO %d", GPIO_NUM_4, GPIO_NUM_19);
}

void clawServer::init() {
    // Initialise
    ESP_LOGI(TAG, "Initializing services...");

    rcl_ret_t rc;

    rc = rclc_service_init_default(&lift_service, node, lift_service_type_support, "service_lift");
    if (rc != RCL_RET_OK) {
        ESP_LOGE(TAG, "Failed to initialize lift service");
    }

    rc = rclc_service_init_default(&claw_service, node, claw_service_type_support, "service_claw");
    if (rc != RCL_RET_OK) {
        ESP_LOGE(TAG, "Failed to initialize claw service");
    }

    rclc_executor_add_service(exec, &claw_service, &req_claw, &res_claw, service_callback_claw);
    rclc_executor_add_service(exec, &lift_service, &req_lift, &res_lift, service_callback_lift);
    ESP_LOGI(TAG, "Service Init has been called");


}
// Service to handle lift movement
void clawServer::service_callback_lift(const void * req, void * res){

    std_srvs__srv__SetBool_Request * req_lift=(std_srvs__srv__SetBool_Request *) req;
    std_srvs__srv__SetBool_Response * res_lift=(std_srvs__srv__SetBool_Response *) res;
    ESP_LOGI(TAG,"Lift Service called...");

    if(req_lift->data){
        // qmd::speeds[4] = 1; // Start motor to move upwards at max speed
        ESP_LOGI(TAG,"Lift Going Upwards");
        res_lift->success = true;
    }
    else {
        // qmd::speeds[4] = -1; // Start motor to move downward at max speed
        ESP_LOGI(TAG,"Lift Going Down");
        res_lift->success = true;
    }
        // Debugging Purpose    
    // while(1) {
    // if (triggered) {
    //     ESP_LOGI(TAG, "Interrupt triggered");
    //     triggered = false;
    // }
    // vTaskDelay(pdMS_TO_TICKS(10));
    // }
}

// Service to handle claw movement
void clawServer::service_callback_claw(const void * req1, void * res1){
  
    std_srvs__srv__SetBool_Request * req_claw=(std_srvs__srv__SetBool_Request *) req1;
    std_srvs__srv__SetBool_Response * res_claw=(std_srvs__srv__SetBool_Response *) res1;

    if(req_claw->data){
        // qmd::speeds[4] = 1; // Start motor to move upwards at max speed
        ESP_LOGI(CLAW_TAG,"Lift Going Upwards");
        res_claw->success = true;
        // res_claw->message = "Lift Up";
    }
    else {
        // qmd::speeds[4] = -1; // Start motor to move downward at max speed
        ESP_LOGI(CLAW_TAG,"Lift Going Downwards");
        res_claw->success = true;
        // res_claw->message = "Lift Down";
    }
}


// Interrupt Service Routine to Stop Lift Motion
void clawServer::gpio_isr_handler(void* arg) {
    triggered = true;
    // qmd::speeds[4] = 0; // Set PWM = 0 to stop the Lift
}


gpio_config_t clawServer::configure_interrupt(int gpio_pin){
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_NEGEDGE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << gpio_pin);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    return io_conf;
}


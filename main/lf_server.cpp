// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
#include "esp_log.h"
#include <driver/gpio.h>

// #include <rmw_microros/rmw_microros.h>
// #include <rcl/error_handling.h>
#include "lf_server.hpp"

#define TAG "TEST"

gpio_num_t rx = GPIO_NUM_2;
gpio_num_t tx = GPIO_NUM_4;
gpio_num_t en = GPIO_NUM_12;
gpio_num_t trg = GPIO_NUM_38;

int nodeCount = 0;
bool state = true;
int lastData = 0;
float prev_err, area;
float kp = 0.7, kd = 0.3, ki = 0.0;
float divisor = kp*35 + kd + ki;

bool lf_server::m_reachedZone3 = false;
lsaHandler* lf_server::uarthandle = 0;
lf_server* lf_server::lf = 0;

// velDriver* vel_D


const char * m_service_name = "/lf_service";


// message type support
const rosidl_service_type_support_t * lf_service_type_support = ROSIDL_GET_SRV_TYPE_SUPPORT(std_srvs, srv, SetBool);

lf_server::lf_server(velDriver* vel_D) {
    if(!uarthandle) uarthandle = new lsaHandler(tx, rx, en, trg);
    lf = this;
    this->vel_D = vel_D;
}

void lf_server::init() { 
    ESP_LOGI(TAG, "Initializing line following service...");
    rcl_ret_t rc = rclc_service_init_default(&m_lf_service, node, lf_service_type_support, m_service_name);

    if (rc != RCL_RET_OK) {
        ESP_LOGE(TAG, "Failed to initialize line follower service");
    }

    rclc_executor_add_service(exec, &m_lf_service, &m_req_follow, &m_res_follow, line_follower_service_callback);
    ESP_LOGI(TAG, "Service initialization successful");
}

void lf_server::line_follower_service_callback(const void * request_msg, void * response_msg) {
    std_srvs__srv__SetBool_Request * req_in = (std_srvs__srv__SetBool_Request *) request_msg;
    std_srvs__srv__SetBool_Response * res_in = (std_srvs__srv__SetBool_Response *) response_msg;

    // request == true
    if (req_in->data) {
        while (!m_reachedZone3) {
            lf->lf_server::move_to_zone_3();
        }
        res_in->success = true;
    }
    
}

void lf_server::move_to_zone_3() {
    uarthandle->update();
    uint8_t data = uarthandle->value;
    ESP_LOGI(TAG, "data = %d", data);

    // if (nodeCount == 0 && data == 255) {
    //     if (!state)
    //     {
    //         lf->moveForward(3000);
    //         state = false;
    //     }

    //     if (lastData > 35) {
    //         // move right
    //         lf->wheelmapper(1.0,0,0); // change the value of x to be around 0.588235294 (test value)
    //     }

    //     else {
    //         lf->wheelmapper(-1.0,0,0); // change the value of x to be around 0.588235294 (test value)
    //     }
        
    // }

    // else if (nodeCount >= 2) {
    //     lf->wheelmapper(0,0,0);
    //     m_reachedZone3 = true;
    // }

    // else {
        lf->pid(data);
    // }

    lastData = data;
}

void lf_server::pid(uint8_t data) {
    float error = data - 35.0; // 35 is the mid value of lsa08

    float prop = kp*error;                  // propotional equation
    float deriv = kd*(error - prev_err);    // derivative equation
    float integral = ki*(area + error);     // integral equation
    
    float final_PID = prop + deriv + integral; // adding proportional, derivative and integral values
    float normalized = -final_PID/divisor;     // normalized value should be less than 1

    // lf->wheelmapper(normalized, 1.0, 0.0);
    vel_D->run(2.0, 0.0, normalized);
    ESP_LOGI(TAG, "yaw = %f",normalized);
    prev_err = error;
}

// move forward for a few seconds (2 or 3 sec)
void lf_server::moveForward(int delay) {
    // change the value of y to be around 0.588235294 (test value)
    lf->wheelmapper(0,1.0,0);
    // vTaskDelay(pdMS_TO_TICKS(delay));
    lf->wheelmapper(0,0,0);
}

void lf_server::wheelmapper(float x, float y, float w) {};

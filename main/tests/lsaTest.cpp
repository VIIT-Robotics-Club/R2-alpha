#include <stdio.h>
#include "lsaHandler.hpp"
#include "freertos/FreeRTOS.h"

#include "freertos/task.h"
#include "esp_log.h"
#include <driver/gpio.h>

#include "velDriver.hpp"
#include "pinMap.hpp"



int pwmPins[] = { A_PWM1, A_PWM2, A_PWM3, A_PWM4}; //  LIFT_PWM, GRAB_PWM};
int dirPins[] = { A_DIR1, A_DIR2, A_DIR3, A_DIR4}; //  LIFT_DIR, GRAB_DIR};

gpio_num_t rx = GPIO_NUM_35;
gpio_num_t tx = GPIO_NUM_36;
gpio_num_t en = GPIO_NUM_14; 
gpio_num_t trg = GPIO_NUM_13;

int lastData = 0;
float prev_err = 0, area;
float kp = 0.215, kd = 0.0, ki = 0.0;
float divisor = kp*45 + kd + ki;

// float base

#define configTICK_RATE_HZ 100
#define TAG "TEST"

int juncCount = 0;

// qmd* handler = new qmd(pwmPins, dirPins, 4);
// velDriver* vel_D = new velDriver(handler);

// extern "C" void app_main(void);

extern "C" void app_main(void) {
    lsaHandler uartHandle(tx, rx, en, trg);
    // handler->setRange(19990, 0);

    while (true) {
        uartHandle.update();
        uint8_t data = uartHandle.value;
        // uint8_t data = 10;

        ESP_LOGI(TAG, "data = %d", data);

        // float error = data - 35.0; // 35 is the mid value of lsa08

        // float prop = kp*error;                  // propotional equation
        // float deriv = kd*(error - prev_err);    // derivative equation
        // // float integral = ki*(area + error);     // integral equation
        
        // float final_PID = prop + deriv; // adding proportional, derivative and integral values
        // // float normalized = final_PID/divisor;     // normalized value should be less than 1
        // float normalized = std::min(1.0f, std::max(final_PID/9, -1.0f));
        // if (data == 255) {
        //     vel_D->run(0.0, 0.0, 0.0);
        // }

        // else {
        //     vel_D->run(-1.0, 0.0, normalized);
        // }
        // ESP_LOGI(TAG, "x = %f,  yaw = %f", 1.0, normalized);
        // prev_err = error;



        
        if(uartHandle.trigger)
        {
            juncCount++;
            ESP_LOGI(TAG, "junction count = %d", juncCount);
            uartHandle.trigger = false;
        }

        // Delay for a short while to avoid flooding the terminal with too many messages
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
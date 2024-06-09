#include <stdio.h>
#include "lsaHandler.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

gpio_num_t rx = GPIO_NUM_6;
gpio_num_t tx = GPIO_NUM_7;
gpio_num_t en = GPIO_NUM_5;
gpio_num_t trg = GPIO_NUM_4;

#define configTICK_RATE_HZ 1000
#define TAG "TEST"

int juncCount = 0;

extern "C" void app_main(void);

void app_main(void) {
    lsaHandler uartHandle(tx, rx, en, trg);

    while (true) {
        uartHandle.update();
        ESP_LOGI(TAG, "value = %d", uartHandle.value);
        
        if(uartHandle.trigger)
        {
            juncCount++;
            ESP_LOGI(TAG, "junction count = %d", juncCount);
        }

        // Delay for a short while to avoid flooding the terminal with too many messages
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
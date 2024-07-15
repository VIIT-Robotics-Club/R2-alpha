#include <stdio.h>
#include "lsaHandler.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"


#define LSA_RX  GPIO_NUM_35
#define LSA_TX  GPIO_NUM_36
#define LSA_EN  GPIO_NUM_14
#define LSA_TRG  GPIO_NUM_13

#define configTICK_RATE_HZ 1000
#define TAG "TEST"

int juncCount = 0;

// extern "C" void app_main(void);

extern "C" void app_main(void) {
    lsaHandler uartHandle(LSA_TX, LSA_RX, LSA_EN, LSA_TRG);

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
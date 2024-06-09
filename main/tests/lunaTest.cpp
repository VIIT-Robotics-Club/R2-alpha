

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <lunaHandler.hpp>

#define A_LUNA_SCL GPIO_NUM_3
#define A_LUNA_SDA GPIO_NUM_46

extern "C" void app_main(void){

    lunaHandler* hand = new lunaHandler(A_LUNA_SCL, A_LUNA_SDA);

    while(1){
        hand->update();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
};
#include <stdio.h>
#include <esp_log.h>

#define LOG "MAIN"

#include <urosHandler.hpp>
#include "icmPub.hpp"


urosHandler* uros = 0 ;
extern "C" void app_main(void)
{
    uros = new urosHandler();

    // create a new pro core executor and run given element
    uros->addThreadExecutor({
        new icmPub()
    });

    // create a new app core executor and run given element
    uros->addThreadExecutor({
        // new icmPub()
    }, APP_CPU_NUM);
    

    ESP_LOGI(LOG, "R2-Alpha is under development !!");
}

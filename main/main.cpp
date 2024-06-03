#include <stdio.h>
#include <esp_log.h>

#define LOG "MAIN"

#include <urosHandler.hpp>
#include "icmPub.hpp"
#include "velDriver.hpp"

#include "pinMap.hpp"

int pwmPins[] = { A_PWM1, A_PWM2, A_PWM3, A_PWM4};
int dirPins[] = { A_DIG1, A_DIG2, A_DIG3, A_DIG4};

urosHandler* uros = 0 ;
extern "C" void app_main(void)
{
    qmd* handler = new qmd(pwmPins, dirPins, 4);
    uros = new urosHandler();
    // qmd* handler = 0;

    // create a new pro core executor and run given element
    uros->addThreadExecutor({
        new icmPub(),
        new velDriver(handler),
    });

    // // create a new app core executor and run given element
    // uros->addThreadExecutor({
    //     // new icmPub()
    // }, APP_CPU_NUM);
    
    ESP_LOGI(LOG, "R2-Alpha is under development !!");
}

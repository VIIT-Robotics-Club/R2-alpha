#include <stdio.h>
#include <esp_log.h>

#define LOG "MAIN"

#include <urosHandler.hpp>
#include "icmPub.hpp"
#include "lunaPub.hpp"
#include "velDriver.hpp"

#include "pinMap.hpp"

int pwmPins[] = { A_PWM1, A_PWM2, A_PWM3, A_PWM4, A_PWM5, A_PWM6};
int dirPins[] = { A_DIR1, A_DIR2, A_DIR3, A_DIR4, A_DIR5, A_DIR6};

urosHandler* uros = 0 ;
extern "C" void app_main(void)
{
    uros = new urosHandler();

    // quad motor handler in hexa operation mode
    qmd* handler = new qmd(pwmPins, dirPins, 6);
    handler->setRange(19990, 0);


    // create a new pro core executor and run given element
    // uros->addThreadExecutor({
    //     // new icmPub(),
    //     // new lunaPub(),
    // });

    // // create a new app core executor and run given element
    uros->addThreadExecutor({
        new velDriver(handler),
    }, APP_CPU_NUM);

    ESP_LOGI(LOG, "R2-Alpha is under development !!");
}

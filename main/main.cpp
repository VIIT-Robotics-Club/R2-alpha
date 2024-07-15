#include <stdio.h>
#include <esp_log.h>

#define LOG "MAIN"

#include "clawServer.hpp"


#include <urosHandler.hpp>
#include "icmPub.hpp"
#include "lunaPub.hpp"
#include "velDriver.hpp"
#include "mpuPub.hpp"


#include "lf_pub.hpp"
#include "resetClient.cpp"

#include "pinMap.hpp"

int pwmPins[] = { A_PWM1, A_PWM2, A_PWM3, A_PWM4, LIFT_PWM, GRAB_PWM};
int dirPins[] = { A_DIR1, A_DIR2, A_DIR3, A_DIR4, LIFT_DIR, GRAB_DIR};




urosHandler* uros = 0 ;
extern "C" void app_main(void)
{
 
    uros = new urosHandler("esp_controller");

    // // quad motor handler in hexa operation mode
    qmd* handler = new qmd(pwmPins, dirPins, 6);
    handler->setRange(19990, 0);

    uros->addThreadExecutor({
        new clawServer(handler),
        new velDriver(handler),
        new lunaPub(50),
        // new mpuPub(50),
        new lf_pub(),
        new resetClient(),
        // new icmPub(50),
    }, PRO_CPU_NUM);

    ESP_LOGI(LOG, "R2-Alpha is under development !!");
}

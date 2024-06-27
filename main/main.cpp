#include <stdio.h>
#include <esp_log.h>

#define LOG "MAIN"

#include <urosHandler.hpp>
// #include "velDriver.hpp"
// #include "lf_server.hpp"
#include "lf_pub.hpp"

#include "pinMap.hpp"

int pwmPins[] = { A_PWM1, A_PWM2, A_PWM3, A_PWM4}; //  LIFT_PWM, GRAB_PWM};
int dirPins[] = { A_DIR1, A_DIR2, A_DIR3, A_DIR4}; //  LIFT_DIR, GRAB_DIR};

urosHandler* uros = 0;
extern "C" void app_main(void)
{

    // quad motor handler in hexa operation mode
    // qmd* handler = new qmd(pwmPins, dirPins, 4);
    // handler->setRange(19990, 0);

    uros = new urosHandler("esp_controller");

    // velDriver* vel_d = new velDriver(handler);
    // lf_server* lf = new lf_server(vel_d);

    
    
    uros->addThreadExecutor({
        // new clawServer(handler),
        // new lunaPub(20),
        // new icmPub(50),
        // new velDriver(handler),
        new lf_pub(),
    }, PRO_CPU_NUM);
    // }, PRO_CPU_NUM);
}


// urosHandler* uros2 = 0;

// extern "C" void app_main(void)
// {
//     uros = new urosHandler("esp_controller");
//     uros2 = new urosHandler("esp_controller2");

//     // quad motor handler in hexa operation mode
//     qmd* handler = new qmd(pwmPins, dirPins, 6);
//     handler->setRange(19990, 0);


//     // create a new pro core executor and run given element
//     uros->addThreadExecutor({
//         new icmPub(),
//         new lunaPub(),
//     }, PRO_CPU_NUM);


//     // uros->addThreadExecutor({
//     // }, APP_CPU_NUM);

//     // // create a new app core executor and run given element
//     uros2->addThreadExecutor({
//         new velDriver(handler),
//     }, APP_CPU_NUM);

//     // uros->addThreadExecutor({
//     // }, PRO_CPUID);

//     ESP_LOGI(LOG, "R2-Alpha is under development !!");
// }
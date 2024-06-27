#ifndef CLAW_SERVICE_HPP
#define CLAW_SERVICE_HPP

#include <chrono>

#include "esp_intr_alloc.h"
#include "esp_attr.h"
#include "esp_intr_types.h"
#include "driver/gpio.h"
#include "esp_log.h"

#include <qmd.hpp>
#include <urosElement.hpp>
#include "std_srvs/srv/set_bool.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>



#define LIFT_QMD_INDEX (4)   // qmd index for lift motor  
#define GRAB_QMD_INDEX (5)   // qmd index for grab motor  
#define LIFT_SPEED (0.5f)    // motor speed for lift 
#define GRAB_FORCE (0.5f)    // motor speed for grab
#define GRAB_ACTIVE_TIME (1000) // active time for grab motor to active in miliseconds

#define LIFT_ISR_TIME_DIFF_THRESH 10
// gpio_config_t io_conf;

// rcl_service_t lift_service;
// std_srvs__srv__SetBool_Response req;
// std_srvs__srv__SetBool_Response res;

// rcl_service_t claw_service;
// std_srvs__srv__SetBool_Response req1;
// std_srvs__srv__SetBool_Response res1;

class clawServer : public urosElement
{
public:
    clawServer(qmd* handler);
    void init();
    static void gpio_upper_isr_handler(void* arg);
    static void gpio_lower_isr_handler(void* arg);
    static void service_callback_lift(const void * req,  rmw_request_id_t *, void * res);
    static void service_callback_grab(const void * req,  rmw_request_id_t *, void * res);


    // direction of movement in last trasaction
    bool headingUp = false;
// private:
    
    static qmd* handler;
    static clawServer* current; 
    rcl_service_t lift_service;
    rcl_service_t claw_service;

    std_srvs__srv__SetBool_Request req_lift;
    std_srvs__srv__SetBool_Response res_lift;
    std_srvs__srv__SetBool_Request req_claw;
    std_srvs__srv__SetBool_Response res_claw;

    std::chrono::milliseconds lastTick, diff;


    TaskHandle_t clawStopThreadHandle;
    // idle task to stop motor when notify received from ISR
    static void stopClawTask(void* ptr);
};

#endif // CLAW_SERVICE_HPP
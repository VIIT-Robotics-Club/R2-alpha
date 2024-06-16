#ifndef CLAW_SERVICE_HPP
#define CLAW_SERVICE_HPP

#include "esp_intr_alloc.h"
#include "esp_attr.h"
#include "esp_intr_types.h"
#include "driver/gpio.h"
#include "esp_log.h"

#include <qmd.hpp>
#include <urosElement.hpp>
#include "std_srvs/srv/set_bool.h"


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
    static void gpio_isr_handler(void* arg);
    static void service_callback_lift(const void * req,  rmw_request_id_t *, void * res);
    static void service_callback_claw(const void * req,  rmw_request_id_t *, void * res);

private:
    static qmd* handler;
    static clawServer* current; 
    rcl_service_t lift_service;
    rcl_service_t claw_service;

    std_srvs__srv__SetBool_Request req_lift;
    std_srvs__srv__SetBool_Response res_lift;
    std_srvs__srv__SetBool_Request req_claw;
    std_srvs__srv__SetBool_Response res_claw;


};

#endif // CLAW_SERVICE_HPP
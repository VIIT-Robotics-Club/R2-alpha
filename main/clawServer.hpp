#ifndef CLAW_SERVICE_HPP
#define CLAW_SERVICE_HPP

#include "esp_intr_alloc.h"
#include "esp_attr.h"
#include "esp_intr_types.h"
#include "driver/gpio.h"
#include "esp_log.h"

#include <urosElement.hpp>
#include "std_srvs/srv/set_bool.h"

#define GPIO_NUM_1 4
#define GPIO_IN2 19

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
    clawServer();
    void init();
    static void gpio_isr_handler(void* arg);
    gpio_config_t configure_interrupt(int gpio_pin);
    static void service_callback_lift(const void * req, void * res);
    static void service_callback_claw(const void * req, void * res);

private:
    gpio_config_t io_conf;
    rcl_service_t lift_service;
    std_srvs__srv__SetBool_Request req_lift;
    std_srvs__srv__SetBool_Response res_lift;

    rcl_service_t claw_service;
    std_srvs__srv__SetBool_Request req_claw;
    std_srvs__srv__SetBool_Response res_claw;

};

#endif // CLAW_SERVICE_HPP
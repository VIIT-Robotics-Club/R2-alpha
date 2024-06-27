#ifndef LF_SERVER_HPP
#define LF_SERVER_HPP

#include "urosElement.hpp"
#include "lsaHandler.hpp"
#include "velDriver.hpp"

#include "std_srvs/srv/set_bool.h"


class lf_server : public urosElement {

    rcl_service_t m_lf_service;

    // Service message objects
    std_srvs__srv__SetBool_Request m_req_follow;
    std_srvs__srv__SetBool_Response m_res_follow;

    static bool m_reachedZone3;
    static lsaHandler* uarthandle;
    static lf_server* lf;

    velDriver* vel_D;

    // lf helper methods
    void move_to_zone_3();
    void moveForward(int delay);
    void pid(uint8_t data);

    // just for testing purpose
    void wheelmapper(float x, float y, float w);

    public:
        // uart driver initialization
        lf_server(velDriver* vel_D);

        // microros initialization
        void init();

        // service callback function
        static void line_follower_service_callback(const void * req, void * res);

};
#endif
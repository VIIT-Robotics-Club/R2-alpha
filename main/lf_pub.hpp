#ifndef LF_PUB_HPP
#define LF_PUB_HPP

#include "urosElement.hpp"
#include "lsaHandler.hpp"
// #include "velDriver.hpp"

#include <std_msgs/msg/int32.h>
#include <example_interfaces/msg/int16_multi_array.h>


class lf_pub : public urosElement {

    static lsaHandler* uarthandle;
    rcl_timer_t timer;
    rcl_publisher_t lsaPublisher;
    rcl_publisher_t lsaJunctionPub;
    static lf_pub* lf;
    static int junction_count;

    public:
        // uart driver initialization
        lf_pub();

        // microros initialization
        void init();

        // publisher callback functions
        static void timer_callback(rcl_timer_s* time, int64_t num);

};
#endif
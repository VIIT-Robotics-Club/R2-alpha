#ifndef LF_PUB_HPP
#define LF_PUB_HPP

#include "urosElement.hpp"
#include "lsaHandler.hpp"



#define LSA_DEF_FREQ 20
class lf_pub : public urosElement {

    public:
        // uart driver initialization
        lf_pub(int hz = LSA_DEF_FREQ);

        // microros initialization
        void init();

        // publisher callback functions
        static void timer_callback(rcl_timer_s* time, int64_t num);


    private:
        int frequency = LSA_DEF_FREQ;

        static lsaHandler* uarthandle;
        static lf_pub* lf;

        rcl_timer_t timer;
        rcl_publisher_t lsaPublisher;
        rcl_publisher_t lsaJunctionPub;
        int junctionCount = 0;
};


#endif
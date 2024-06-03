


#include <urosElement.hpp>
#include <icm.hpp>


class icmPub : public urosElement {

public:

    // initialize hardware interface here
    icmPub();

    // initialize micro-ros publisher / subscribers here
    void init();

    
    static void icm_callback(rcl_timer_s* time, int64_t num);


private:
    rcl_timer_t timer;
    rcl_publisher_t imuPublisher, magPublisher;
    static icm* handler;
    static icmPub* defaultPub;
};

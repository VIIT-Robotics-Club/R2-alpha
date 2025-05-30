


#include <urosElement.hpp>
#include <icm.hpp>

#define ICM_DEF_FREQ 20

class icmPub : public urosElement {

public:

    // initialize hardware interface here
    icmPub(int freq = ICM_DEF_FREQ);

    // initialize micro-ros publisher / subscribers here
    void init();

    
    static void icm_callback(rcl_timer_s* time, int64_t num);


private:

    float pubDelayMs = 1000 / ICM_DEF_FREQ;
    rcl_timer_t timer;
    rcl_publisher_t imuPublisher, magPublisher;
    static icm* handler;
    static icmPub* defaultPub;
};

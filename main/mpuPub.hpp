


#include <urosElement.hpp>
#include <MPU.hpp>

#define MPU_DEF_FREQ 20

class mpuPub : public urosElement {

public:

    // initialize hardware interface here
    mpuPub(int freq = MPU_DEF_FREQ);

    // initialize micro-ros publisher / subscribers here
    void init();

    
    static void mpu_callback(rcl_timer_s* time, int64_t num);


private:

    float pubDelayMs = 1000 / MPU_DEF_FREQ;
    rcl_timer_t timer;
    rcl_publisher_t imuPublisher, magPublisher;
    static mpuPub* defaultPub;
};

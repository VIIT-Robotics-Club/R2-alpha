#include <urosElement.hpp>
#include "lunaHandler.hpp"


#define LUNA_DEF_FREQ 10
#define LUNA_COUNT (6)
class lunaPub : public urosElement {

public:

    // initialize hardware interface here
    lunaPub(int freq = LUNA_DEF_FREQ);

    // initialize micro-ros publisher / subscribers here
    void init();

    
    static void luna_callback(rcl_timer_s* time, int64_t num);


private:
    float pubDelayMs = 1000 / LUNA_DEF_FREQ;
    rcl_timer_t timer;
    rcl_publisher_t lunaPublisher[LUNA_COUNT];
    static lunaHandler* luna_handler;
    static lunaPub* luna_pub;

    lunaHandler* handler;
};



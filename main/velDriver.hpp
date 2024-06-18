
#ifndef VEL_DRIVER_HPP
#define  VEL_DRIVER_HPP


#include <math.h>

#include <esp_log.h>

#include <qmd.hpp>
#include <rclc/rclc.h>
#include <urosElement.hpp>
#include <geometry_msgs/msg/twist.h>

/**
 * @brief wheelSpeed to represent normalized wheel speed
 */
struct wheelSpeed {

    float rawSpeed[4] = {0.0f};
    float &fl = rawSpeed[0];   // front left wheel  
    float &fr = rawSpeed[1];   // front right wheel
    float &bl = rawSpeed[2];   // back left wheel
    float &br = rawSpeed[3];   // back right wheel
};


class velDriver : public urosElement {

public:
    const rosidl_message_type_support_t * twistMsgType = ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, Twist);
    static velDriver* def;

    velDriver(qmd* drv);

    void init();

    /**
     * @brief  mapping function for cmd_vel to motor speed translation
     * 
     * @param x normalized speed in x direction 
     * @param y normalized speed in y direction
     * @param w normalized yaw
     * @return 
     */
    static wheelSpeed map(float x, float y, float w);

    static void cmdVelCallback(const void* msgIn);

    qmd* handler = 0;

private:
    rcl_subscription_t sub;
    geometry_msgs__msg__Twist msgAlloc;
};


#endif // VEL_DRIVER_HPP

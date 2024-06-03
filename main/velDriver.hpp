

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

    velDriver(qmd* drv) : handler(drv) {
        def = this;
    };

    void init() {
        rclc_subscription_init_default(&sub, node, twistMsgType, "/cmd_vel");
        geometry_msgs__msg__Twist__init(&msgAlloc);
        rclc_executor_add_subscription(exec, &sub, &msgAlloc, cmdVelCallback, ON_NEW_DATA);
    };



    /**
     * @brief  mapping function for cmd_vel to motor speed translation
     * 
     * @param x normalized speed in x direction 
     * @param y normalized speed in y direction
     * @param w normalized yaw
     * @return 
     */
    static wheelSpeed map(float x, float y, float w){
        float si, co, mag, theta;

        theta = atan2(y, x);
        si = sin(theta - M_PI/4);
        co = cos(theta - M_PI/4);
        mag = abs(si) > abs(co) ? abs(si) : abs(co);

        wheelSpeed ret;
        ret.fl = y * co / mag + w;
        ret.fr = y * si / mag - w;
        ret.bl = y * si / mag + w;
        ret.br = y * co / mag - w;

        return ret;
    }


    static void cmdVelCallback(const void* msgIn){
        const geometry_msgs__msg__Twist * msg = (const geometry_msgs__msg__Twist *)msgIn;

        wheelSpeed mapped = map(msg->linear.x, msg->linear.y, msg->angular.z / 4.0f);
        memcpy(def->handler->speeds, &mapped, sizeof(wheelSpeed));
        def->handler->update();

        ESP_LOGI("CMD_VEL_SUB", "x %f y %f w %f", 
            msg->linear.x,
            msg->linear.y,
            msg->angular.z
        );
    };

    qmd* handler;

private:
    rcl_subscription_t sub;
    geometry_msgs__msg__Twist msgAlloc;
};



velDriver* velDriver::def = 0;
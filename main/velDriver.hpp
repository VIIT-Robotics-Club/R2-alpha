
// #ifndef 
// #define 
// #endif // 
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



    void run(float x, float y, float w){
        wheelSpeed mapped = map(y, x, -w / 2.0f);
        memcpy(def->handler->speeds, &mapped, 4 * sizeof(float));
        def->handler->update();

        ESP_LOGI("CMD_VEL_SUB", " x %f y %f w %f fl %f fr %f bl %f br %f ",
            x,
            y,
            w,
            mapped.fl,
            mapped.fr,
            mapped.bl,
            mapped.br
        );
    }
    /**
     * @brief  mapping function for cmd_vel to motor speed translation
     * 
     * @param x normalized speed in x direction 
     * @param y normalized speed in y direction
     * @param w normalized yaw
     * @return 
     */
    static wheelSpeed map(float x, float y, float w){
        // invert w to correct for inverted axis
        w = -w;
        x = -x;
    
        float si, co, max, theta = atan2(y, x), power = sqrt( x * x + y * y);

        si = sin(theta - M_PI/4);
        co = cos(theta - M_PI/4);
        max = abs(si) > abs(co) ? abs(si) : abs(co);

        wheelSpeed ret;
        // ret.fl = power * co / max + w;
        // ret.fr = power * si / max - w;
        // ret.bl = power * si / max + w;
        // ret.br = power * co / max - w;

        ret.fl = power * co + w;
        ret.fr = power * si - w;
        ret.bl = power * si + w;
        ret.br = power * co - w;


        float overShootMax = 1.0f;
        for(int i = 0; i < 4; i++){
            overShootMax = (abs(ret.rawSpeed[i]) > overShootMax ) ? abs(ret.rawSpeed[i]) : overShootMax;
        }

        for(int i = 0; i < 4; i++){
            ret.rawSpeed[i] /= overShootMax;
        }


        // if((power + abs(w)) > 1){
        //     ret.fl /= power + w;
        //     ret.fr /= power + w;
        //     ret.bl /= power + w;
        //     ret.br /= power + w;
        // };

        return ret;
    }


    static void cmdVelCallback(const void* msgIn){
        const geometry_msgs__msg__Twist * msg = (const geometry_msgs__msg__Twist *)msgIn;

        wheelSpeed mapped = map(msg->linear.y, msg->linear.x, msg->angular.z / 2.0f);
        memcpy(def->handler->speeds, &mapped, 4 * sizeof(float));

        // def->handler->speeds[4] = msg->linear.x / 2.0f;
        // def->handler->speeds[5] = msg->linear.y / 2.0f;
        def->handler->update();

        // ESP_LOGI("CMD_VEL_SUB", " x %f y %f w %f fl %f fr %f bl %f br %f ",
        //     msg->linear.x,
        //     msg->linear.y,
        //     msg->angular.z,
        //     mapped.fl,
        //     mapped.fr,
        //     mapped.bl,
        //     mapped.br
        // );
    };

    qmd* handler;

private:
    rcl_subscription_t sub;
    geometry_msgs__msg__Twist msgAlloc;
};


velDriver* velDriver::def = 0;
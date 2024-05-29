#include "icmPub.hpp"

#include <esp_log.h>
#include <rclc/publisher.h>

#include <rmw_microros/rmw_microros.h>
#include <sensor_msgs/msg/imu.h>
#include <sensor_msgs/msg/magnetic_field.h>

#define LOG "icmPub"

const rosidl_message_type_support_t * imu_type_support = ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, Imu);
const rosidl_message_type_support_t * mag_type_support = ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, MagneticField);


icm*  icmPub::handler = 0;
icmPub*  icmPub::defaultPub = 0;


icmPub::icmPub(){
    handler = new icm();
    defaultPub = this;
}


void icmPub::init()
{
    rclc_publisher_init_default(&imuPublisher, node, imu_type_support, "imu/data_raw");
    // rclc_publisher_init_default(&magPublisher, &node, mag_type_support, "imu/mag");

    rclc_timer_init_default(&timer, support, RCL_MS_TO_NS(100), icmPub::icm_callback);

    rclc_executor_add_timer(exec, &timer);
}


void icmPub::icm_callback(rcl_timer_s* time, int64_t num){

    handler->update();
    // MPU.heading(&magRaw);
    // MPU.motion(&accelRaw, &gyroRaw, &magRaw);

    sensor_msgs__msg__Imu data;
    sensor_msgs__msg__Imu__init(&data);

    

    sensor_msgs__msg__MagneticField magData;
    sensor_msgs__msg__MagneticField__init(&magData);


    data.header.stamp.nanosec = rmw_uros_epoch_nanos();
    data.header.stamp.sec = rmw_uros_epoch_millis() / 1000;
    data.header.frame_id.data = "imu";
    data.header.frame_id.size = 3;
    data.header.frame_id.capacity = 4;

    magData.header = data.header;

    data.linear_acceleration.x = handler->data.ax * 9.8f; 
    data.linear_acceleration.y = handler->data.ay * 9.8f; 
    data.linear_acceleration.z = handler->data.az * 9.8f; 
    
    data.angular_velocity.x = handler->data.gx;
    data.angular_velocity.y = handler->data.gy; 
    data.angular_velocity.z = handler->data.gz;
    

    memset(data.angular_velocity_covariance, 0, sizeof(data.angular_velocity_covariance));
    memset(data.linear_acceleration_covariance, 0, sizeof(data.angular_velocity_covariance));

    
    // magData.magnetic_field.x = magRaw.x * 0.15f;
    // magData.magnetic_field.y = magRaw.y * 0.15f;
    // magData.magnetic_field.z = magRaw.z * 0.15f;
    rcl_publish(&defaultPub->imuPublisher, &data, NULL);
    // rcl_publish(&magPublisher, &magData, NULL);
        
    ESP_LOGI(LOG, " ax %d %f ay %d %f az %d %f  gx %d %f gy %d %f gz %d %f temp %f", 
    handler->rawData.ax, handler->data.ax,
    handler->rawData.ay, handler->data.ay,
    handler->rawData.az, handler->data.az, 
    handler->rawData.gx, handler->data.gx,
    handler->rawData.gy, handler->data.gy,
    handler->rawData.gz, handler->data.gz,
    handler->data.temp
    );
    // magData.magnetic_field.x, magData.magnetic_field.y, magData.magnetic_field.z);
};




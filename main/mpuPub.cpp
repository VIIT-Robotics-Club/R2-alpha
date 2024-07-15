#include "mpuPub.hpp"

#include <esp_log.h>
#include <rclc/publisher.h>

#include <rmw_microros/rmw_microros.h>
#include <sensor_msgs/msg/imu.h>
#include <sensor_msgs/msg/magnetic_field.h>

#include <pinMap.hpp>
#define LOG "MPUPUB"


#include "MPU.hpp"        // main file, provides the class itself
#include "mpu/math.hpp"   // math helper for dealing with MPU data
#include "mpu/types.hpp"  // MPU data types and definitions
#include "I2Cbus.hpp"

const rosidl_message_type_support_t * imu_type_support = ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, Imu);
const rosidl_message_type_support_t * mag_type_support = ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, MagneticField);

mpuPub*  mpuPub::defaultPub = 0;

MPU_t MPU;         // create an object

rcl_publisher_t imuPublisher, magPublisher;
int32_t sec;
int32_t ns;
mpud::raw_axes_t accelRaw;  
mpud::raw_axes_t gyroRaw;
mpud::raw_axes_t magRaw;


mpuPub::mpuPub(int freq){
    defaultPub = this;
    pubDelayMs = 1000.0f / (float) freq; 

    i2c1.begin(MPU_SDA, MPU_SCL, 100000);

    MPU.setBus(i2c1);  // set communication bus, for SPI -> pass 'hspi'
    MPU.setAddr(mpud::MPU_I2CADDRESS_AD0_LOW);  // set address or handle, for SPI -> pass 'mpu_spi_handle'

    if(MPU.testConnection()){
        ESP_LOGE("main", "device mpu not found !!");
    }

    MPU.initialize();
    // MPU.compassInit();


    mpud::types::raw_axes_t offsetAccel, offsetGyro;
    MPU.computeOffsets(&offsetAccel, &offsetGyro);
    MPU.setAccelOffset(offsetAccel);
    MPU.setGyroOffset(offsetGyro);

    MPU.setSampleRate(250);
    MPU.setAccelFullScale(mpud::ACCEL_FS_4G);
    MPU.setGyroFullScale(mpud::GYRO_FS_500DPS);
    MPU.setDigitalLowPassFilter(mpud::DLPF_98HZ);  // smoother data
    MPU.setInterruptEnabled(mpud::INT_EN_RAWDATA_READY);  // enable INT pin
    
}


void mpuPub::init()
{
    rclc_publisher_init_best_effort(&imuPublisher, node, imu_type_support, "imu/data_raw");
    rclc_publisher_init_best_effort(&magPublisher, node, mag_type_support, "imu/mag");

    rclc_timer_init_default(&timer, support, RCL_MS_TO_NS(pubDelayMs), mpuPub::mpu_callback);

    rclc_executor_add_timer(exec, &timer);
}


void mpuPub::mpu_callback(rcl_timer_s* time, int64_t num){


    MPU.acceleration(&accelRaw);
    MPU.rotation(&gyroRaw);
    // MPU.heading(&magRaw);
    // MPU.motion(&accelRaw, &gyroRaw, &magRaw);

    sensor_msgs__msg__Imu data;
    sensor_msgs__msg__Imu__init(&data);

    

    sensor_msgs__msg__MagneticField magData;
    sensor_msgs__msg__MagneticField__init(&magData);


    ns += num;
    if(ns > (1000 * 1000 * 1000)) {
        sec++;
        ns -= (1000 * 1000 * 1000);
    } 

    data.header.stamp.nanosec = ns;
    data.header.stamp.sec = sec;
    data.header.frame_id.data = "imu";
    data.header.frame_id.size = 3;
    data.header.frame_id.capacity = 4;


    magData.header = data.header;

    data.linear_acceleration.x = -(float) accelRaw.y / (8192.0f / 9.8f); 
    data.linear_acceleration.y = -(float) accelRaw.x / (8192.0f / 9.8f); 
    data.linear_acceleration.z = -(float) accelRaw.z / (8192.0f / 9.8f); 
    
    data.angular_velocity.x = -(float) (gyroRaw.y / (65.5f * 114.591f * 0.5f)) ;
    data.angular_velocity.y = -(float) (gyroRaw.x / (65.5f * 114.591f * 0.5f)) ; 
    data.angular_velocity.z = -(float) (gyroRaw.z / (65.5f * 114.591f * 0.5f)) ;
    

    memset(data.angular_velocity_covariance, INFINITY, sizeof(data.angular_velocity_covariance));
    memset(data.linear_acceleration_covariance, INFINITY, sizeof(data.angular_velocity_covariance));

    
    // magData.magnetic_field.x = magRaw.x * 0.15f;
    // magData.magnetic_field.y = magRaw.y * 0.15f;
    // magData.magnetic_field.z = magRaw.z * 0.15f;
    rcl_publish(&defaultPub->imuPublisher, &data, NULL);
    // rcl_publish(&magPublisher, &magData, NULL);

    // ESP_LOGI("data"," %lf %lf %lf %lf %lf %lf ", data.angular_velocity.x, data.angular_velocity.y, data.angular_velocity.z,
    // data.linear_acceleration.x, data.linear_acceleration.y, data.linear_acceleration.z);
    // magData.magnetic_field.x, magData.magnetic_field.y, magData.magnetic_field.z);
};




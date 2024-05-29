#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <rclc/rclc.h>
#include <esp_log.h>




#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <uros_network_interfaces.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/executor.h>
#include <rmw_microros/rmw_microros.h>

#include <sensor_msgs/msg/imu.h>
#include <sensor_msgs/msg/magnetic_field.h>

#include "icm.hpp"
#include "icmRegisterMap.hpp"

#define LOG "MAIN"


#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){printf("Failed status on line %d: %d. Aborting.\n",__LINE__,(int)temp_rc);vTaskDelete(NULL);}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){printf("Failed status on line %d: %d. Continuing.\n",__LINE__,(int)temp_rc);}}


const rosidl_message_type_support_t * imu_type_support = ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, Imu);
const rosidl_message_type_support_t * mag_type_support = ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, MagneticField);


icm* handler = 0;         // create an object

rcl_publisher_t imuPublisher, magPublisher;
rcl_node_t node;

int32_t sec;
int32_t ns;


void publish_callback(rcl_timer_s* time, int64_t num){

    handler->update();
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
    rcl_publish(&imuPublisher, &data, NULL);
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



void micro_ros_task(void * arg)
{
	rcl_allocator_t allocator = rcl_get_default_allocator();
	rclc_support_t support;

	rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
	RCCHECK(rcl_init_options_init(&init_options, allocator));

	rmw_init_options_t* rmw_options = rcl_init_options_get_rmw_init_options(&init_options);
    
	

    bool waitAutoDiscovery = true;
    while (waitAutoDiscovery)
    {   
        rmw_ret_t ret = rmw_uros_discover_agent(rmw_options);
        if(ret == RMW_RET_OK) waitAutoDiscovery = false;
        else if(ret == RMW_RET_TIMEOUT) waitAutoDiscovery = true;
        else vTaskDelete(NULL);

        vTaskDelay(pdMS_TO_TICKS(100));
    }
    
    rmw_uros_sync_session(1000);

	RCCHECK(rclc_support_init_with_options(&support, 0, NULL, &init_options, &allocator));

	// create node
	RCCHECK(rclc_node_init_default(&node, "imu_node", "", &support));


    rclc_publisher_init_default(&imuPublisher, &node, imu_type_support, "imu/data_raw");
    // rclc_publisher_init_default(&magPublisher, &node, mag_type_support, "imu/mag");

	// create executor
	rclc_executor_t executor = rclc_executor_get_zero_initialized_executor();
	RCCHECK(rclc_executor_init(&executor, &support.context, 2, &allocator));

    rcl_timer_t timer;
    rclc_timer_init_default(&timer, &support, RCL_MS_TO_NS(10), publish_callback);

    rclc_executor_add_timer(&executor, &timer);
  	rclc_executor_spin(&executor);
	
  	vTaskDelete(NULL);
}

TaskHandle_t urosTaskHandle;

extern "C" void app_main(void){
    // MPU.compassInit();
    handler = new icm();
    
    ESP_ERROR_CHECK(uros_network_interface_initialize());

    //pin micro-ros task in APP_CPU to make PRO_CPU to deal with wifi:
    xTaskCreate(micro_ros_task,
            "uros_task",
            4096,
            NULL,
            20, &urosTaskHandle);
};
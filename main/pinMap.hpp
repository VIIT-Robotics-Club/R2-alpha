#ifndef PINMAP_HPP
#define PINMAP_HPP

#include <esp_log.h>
#include <driver/gpio.h>


#define ESP32S3
#ifdef ESP32S3
//...............................ESP1.........................
#define A_LUNA_SCL GPIO_NUM_3
#define A_LUNA_SDA GPIO_NUM_9
// #define A_TX0 GPIO_NUM_43
// #define A_RX0 GPIO_NUM_44
// #define A_TX1 GPIO_NUM_17
// #define A_RX1 GPIO_NUM_18


// FL
#define A_DIR1 GPIO_NUM_18
#define A_PWM1 GPIO_NUM_8


// FR
#define A_DIR2 GPIO_NUM_7
#define A_PWM2 GPIO_NUM_15

// BL
#define A_DIR3 GPIO_NUM_16
#define A_PWM3 GPIO_NUM_17 

// BR
#define A_DIR4 GPIO_NUM_4
#define A_PWM4 GPIO_NUM_5

// lift
#define LIFT_DIR GPIO_NUM_21
#define LIFT_PWM GPIO_NUM_47

// GRAB
#define GRAB_DIR GPIO_NUM_48
#define GRAB_PWM GPIO_NUM_45



// lift interrupt pins
#define LIFT_UPPER_INTR GPIO_NUM_1
#define LIFT_LOWER_INTR GPIO_NUM_2


// #define MPU_SDA GPIO_NUM_13
// #define MPU_SCL GPIO_NUM_14
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      

// #define ICM_CS GPIO_NUM_10
// #define ICM_CLK GPIO_NUM_12
// #define ICM_MISO GPIO_NUM_11
// #define ICM_MOSI GPIO_NUM_13

#else


    #define A_LUNA_SCL GPIO_NUM_22
    #define A_LUNA_SDA GPIO_NUM_21

    // FL
    #define A_DIR1 GPIO_NUM_25
    #define A_PWM1 GPIO_NUM_32


    // FR
    #define A_DIR2 GPIO_NUM_4
    #define A_PWM2 GPIO_NUM_2

    // BL
    #define A_DIR3 GPIO_NUM_13
    #define A_PWM3 GPIO_NUM_12 

    // BR
    #define A_DIR4 GPIO_NUM_27
    #define A_PWM4 GPIO_NUM_26

#endif // ESP32S3



#endif // PINMAP_HPP
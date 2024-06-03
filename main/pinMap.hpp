#ifndef PINMAP_HPP
#define PINMAP_HPP

#include <esp_log.h>
#include <driver/gpio.h>
//...............................ESP1.........................
#define A_LUNA_SCL GPIO_NUM_3
#define A_LUNA_SDA GPIO_NUM_46
#define A_TX0 GPIO_NUM_43
#define A_RX0 GPIO_NUM_44
#define A_TX1 GPIO_NUM_17
#define A_RX1 GPIO_NUM_18
#define A_DIG1 GPIO_NUM_9
#define A_DIG2 GPIO_NUM_11
#define A_DIG3 GPIO_NUM_13
#define A_DIG4 GPIO_NUM_21
#define A_DIG5 GPIO_NUM_4
#define A_DIG6 GPIO_NUM_6
#define A_DIG7 GPIO_NUM_2
#define A_PWM1 GPIO_NUM_10  // * = PWM 
#define A_PWM2 GPIO_NUM_12
#define A_PWM3 GPIO_NUM_14
#define A_PWM4 GPIO_NUM_47
#define A_PWM5 GPIO_NUM_5
#define A_PWM6 GPIO_NUM_7
#define TEST_LED GPIO_NUM_8

//................................ESP2............................
#define B_MPU_SCL GPIO_NUM_4
#define B_MPU_SDA GPIO_NUM_5
#define B_TX1 GPIO_NUM_17
#define B_RX1 GPIO_NUM_18
#define B_DIG1 GPIO_NUM_47
#define B_DIG2 GPIO_NUM_21
#define B_DIG3 GPIO_NUM_9
#define B_DIG4 GPIO_NUM_46
#define B_DIG5 GPIO_NUM_40
#define B_DIG6 GPIO_NUM_39
#define B_DIG7 GPIO_NUM_38
#define B_DIG8 GPIO_NUM_37
#define B_DIG9 GPIO_NUM_45
#define B_PWM1 GPIO_NUM_48    // * MEANS PWM
#define B_PWM2 GPIO_NUM_41
#define TEST_LED0 GPIO_NUM_8

#define ICM_CS GPIO_NUM_10
#define ICM_CLK GPIO_NUM_12
#define ICM_MISO GPIO_NUM_11
#define ICM_MOSI GPIO_NUM_13


#endif // PINMAP_HPP

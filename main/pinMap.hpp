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


// FL
#define A_DIR1 GPIO_NUM_48
#define A_PWM1 GPIO_NUM_45


// FR
#define A_DIR2 GPIO_NUM_7
#define A_PWM2 GPIO_NUM_15

// BL
#define A_DIR3 GPIO_NUM_21
#define A_PWM3 GPIO_NUM_47

// BR
#define A_DIR4 GPIO_NUM_16
#define A_PWM4 GPIO_NUM_17 

// lift
#define A_DIR5 GPIO_NUM_35
#define A_PWM5 GPIO_NUM_36


// GRAB
#define A_DIR6 GPIO_NUM_18
#define A_PWM6 GPIO_NUM_8


#define ICM_CS GPIO_NUM_10
#define ICM_CLK GPIO_NUM_12
#define ICM_MISO GPIO_NUM_11
#define ICM_MOSI GPIO_NUM_13


#endif // PINMAP_HPP

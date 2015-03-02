/*
 * pwm12bit.h
 *
 *  Created on: Nov 15, 2014
 *      Author: sid
 */

#ifndef PWM12BIT_H_
#define PWM12BIT_H_


#define PCA9685_ADDR		0x80

#define PCA9685_SUBADR1 	0x02
#define PCA9685_SUBADR2 	0x03
#define PCA9685_SUBADR3 	0x04

#define PCA9685_MODE1 		0x00
#define 	PCA9685_RESTART		(1<<7)
#define 	PCA9685_EXTCLK		(1<<6)
#define 	PCA9685_AI		(1<<5)
#define 	PCA9685_SLEEP		(1<<4)
#define 	PCA9685_SUB1		(1<<3)
#define 	PCA9685_SUB2		(1<<2)
#define 	PCA9685_SUB3		(1<<1)
#define 	PCA9685_ALLCALL		(1<<0)
#define PCA9685_MODE2 		0x01
#define PCA9685_PRESCALE 	0xFE

#define PCA9685_LED0_ON_L	0x06
#define PCA9685_LED0_ON_H	0x07
#define PCA9685_LED0_OFF_L	0x08
#define PCA9685_LED0_OFF_H	0x09
#define 	PCA9685_PWMOFF		0x10

#define PCA9685_ALLLED_ON_L	0xFA
#define PCA9685_ALLLED_ON_H	0xFB
#define PCA9685_ALLLED_OFF_L	0xFC
#define PCA9685_ALLLED_OFF_H	0xFD


#define PCA_LEDS		4

#define LED0_RED_CH		0
#define LED0_GRN_CH		1
#define LED0_BLU_CH		2
#define LED0_WHITE		3

#define LED1_RED_CH		4
#define LED1_GRN_CH		5
#define LED1_BLU_CH		6
#define LED1_WHITE		7

#define LED2_RED_CH		8
#define LED2_GRN_CH		9
#define LED2_BLU_CH		10
#define LED2_WHITE		11

#define LED3_RED_CH		12
#define LED3_GRN_CH		13
#define LED3_BLU_CH		14
#define LED3_WHITE		15

#define PCA9685_PWM_MASK	0x0FFF

typedef struct __attribute__((packed))
{
	rgb16_t color;
	rgb8_t channel;
} ledCtl_t;

typedef struct __attribute__((packed))
{
	uint16_t color;
	uint8_t channel;
} ledWhiteCtl_t;

void pca9685_init(void);
void pca9685_deinit(void);
void pca9685_set32(uint32_t num, uint32_t red, uint32_t grn, uint32_t blu);

#endif /* PWM12BIT_H_ */

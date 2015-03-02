/*
 * pwm12bit.c
 *
 *  Created on: Nov 15, 2014
 *      Author: sid
 */

#include "c_types.h"
#include "osapi.h"

#include "led/led.h"
#include "pca9685.h"
#include "led_i2c_master.h"

#include "espmissingincludes.h"

//config
ledCtl_t led12b[PCA_LEDS] = {
		{{{0,0,0}}, {{LED0_RED_CH, LED0_GRN_CH, LED0_BLU_CH}}},		//LED0
		{{{0,0,0}}, {{LED1_RED_CH, LED1_GRN_CH, LED1_BLU_CH}}},		//LED1
		{{{0,0,0}}, {{LED2_RED_CH, LED2_GRN_CH, LED2_BLU_CH}}},		//LED2
		{{{0,0,0}}, {{LED3_RED_CH, LED3_GRN_CH, LED3_BLU_CH}}},		//LED3
};

ledWhiteCtl_t led12bWhite[PCA_LEDS] = {
		{0, LED0_WHITE},
		{0, LED1_WHITE},
		{0, LED2_WHITE},
		{0, LED3_WHITE},
};

uint16_t chOff;


bool read(uint8_t addr, uint8_t *buf, uint8_t length)
{
	i2c_master_start();
	i2c_master_writeByte(PCA9685_ADDR | I2C_WRITE);
	if (i2c_master_getAck())
	{
//		os_printf("addr not ack when tx write cmd \n");
		i2c_master_stop();
		return false;
	}

	i2c_master_writeByte(addr);
	if (i2c_master_getAck())
	{
//		os_printf("data not ack when tx write byte1 \n");
		i2c_master_stop();
		return false;
	}

	i2c_master_start();
	i2c_master_writeByte(PCA9685_ADDR | I2C_READ);
	if (i2c_master_getAck())
	{
//		os_printf("addr not ack when tx read cmd \n");
		i2c_master_stop();
		return false;
	}

	uint8_t i = 0;
	for(i=0; i<length; i++)
	{
		buf[i] = i2c_master_readByte();
		i2c_master_setAck(0);//(i == (length - 1)) ? 1 : 0);
	}

	i2c_master_stop();
	return true;
}

bool write(uint8_t addr, uint8_t *buf, uint8_t length)
{
	i2c_master_start();
	i2c_master_writeByte(PCA9685_ADDR | I2C_WRITE);
	if (i2c_master_getAck())
	{
//		os_printf("addr not ack when tx write cmd \n");
		i2c_master_stop();
		return false;
	}

	i2c_master_writeByte(addr);
	if (i2c_master_getAck())
	{
//		os_printf("data not ack when tx write byte1 \n");
		i2c_master_stop();
		return false;
	}

	uint8_t i;
	for(i=0; i<length; i++)
	{
		i2c_master_writeByte(buf[i]);
		if (i2c_master_getAck())
		{
//			os_printf("data not ack when tx write byte %d\n", i);
			i2c_master_stop();
			return false;
		}
	}
	i2c_master_stop();
	return true;
}



bool pca9685_setPWM(uint8_t num, uint16_t val)
{
	uint8_t buf[4];

	if(num > 15)
		return false;

	if(num & 1)
	{
		buf[0] = 0;
		buf[1] = 0;
		buf[2] = val;
		buf[3] = val>>8;
		if(chOff & (1<<num))
		{
			bool ret = write(PCA9685_LED0_ON_L + 4*num, buf, 4);
			if(ret == true)
				chOff &= ~(1<<num);
			return ret;
		}
		else
		{
			return write(PCA9685_LED0_ON_L + 4*num + 2, &buf[2], 2);
		}
	}
	else
	{
		uint16_t res = PCA9685_PWM_MASK - val;
		buf[0] = res;
		buf[1] = res>>8;
		buf[2] = 0xFF;
		buf[3] = 0x0F;
		if(chOff & (1<<num))
		{
			bool ret = write(PCA9685_LED0_ON_L + 4*num, buf, 4);
			if(ret == true)
				chOff &= ~(1<<num);
			return ret;
		}
		else
		{
			return write(PCA9685_LED0_ON_L + 4*num, buf, 2);
		}
	}
	return write(PCA9685_LED0_ON_L + 4*num, buf, 4);
}

bool pca9685_setOff(uint8_t num)
{
	if(num > 15)
		return false;
	uint8_t buf[2];
	buf[0] = 0;
	buf[1] = PCA9685_PWMOFF;
	chOff |= 1<<num;
	return write(num*4 + PCA9685_LED0_OFF_L, buf, 2);
}

////public
void pca9685_deinit(void)
{
	i2c_master_gpio_deinit();
}

void ICACHE_FLASH_ATTR pca9685_init(void)
{
	i2c_master_gpio_init();

	uint8_t reg = PCA9685_SLEEP;
	if(!write(PCA9685_MODE1, &reg, 1))
	{
		os_printf("pwm1 failed\n");
	}

	reg = 3;
	if(!write(PCA9685_PRESCALE, &reg, 1))
	{
		os_printf("prescaler faild\n");
	}

	reg = PCA9685_AI | PCA9685_ALLCALL;			// auto  increment, disable sleep mode
	if(!write(PCA9685_MODE1, &reg, 1))
	{
		os_printf("pwm3 failed\n");
	}

	//read(PCA9685_PRESCALE, &reg, 1);
	//os_printf("1: %d\n", reg);

	uint8_t i=0;
	for(i=0; i<16; i++)
	{
		if(!pca9685_setOff(i))
		{
			os_printf("set off %d failed\n", i);
		}
	}
}


void pca9685_set32(uint32_t num, uint32_t red, uint32_t grn, uint32_t blu)
{
	if(num >= PCA_LEDS)
		return;

	uint8_t i;
	for(i=0; i<3; i++)
	{
		if(i == LEDSTRPOS_RED)
		{
			red = (red>>20);
			if(led12b[num].color.red == red)
				continue;
			else
				led12b[num].color.red = red;
		}
		else if(i == LEDSTRPOS_GRN)
		{
			grn = (grn>>20);
			if(led12b[num].color.grn == grn)
				continue;
			else
				led12b[num].color.grn = grn;
		}
		else if(i == LEDSTRPOS_BLU)
		{
			blu = (blu>>20);
			if(led12b[num].color.blu == blu)
				continue;
			else
				led12b[num].color.blu = blu;
		}

		if(!led12b[num].color.raw[i])
		{
			if(!pca9685_setOff(led12b[num].channel.raw[i]))
			{
				os_printf("setOff %d failed\n", led12b[num].channel.raw[i]);
			}
		}
		else
		{
			if(led12b[num].color.raw[i] > PCA9685_PWM_MASK)
				led12b[num].color.raw[i] = PCA9685_PWM_MASK;
			if(!pca9685_setPWM(led12b[num].channel.raw[i], led12b[num].color.raw[i]))
			{
				os_printf("setPWM %d,%d failed\n", led12b[num].channel.raw[i], led12b[num].color.raw[i]);
			}
		}
	}
}

void pca9685_setWhite32(uint32_t num, uint32_t intens)
{
	if(num >= PCA_LEDS)
		return;

	intens = (intens>>20);
	if(led12bWhite[num].color == intens)
		return;

	led12bWhite[num].color = intens;

	if(!led12bWhite[num].color)
	{
		if(!pca9685_setOff(led12bWhite[num].channel))
		{
			os_printf("setOff %d failed\n", led12bWhite[num].channel);
		}
	}
	else
	{
		if(led12bWhite[num].color > PCA9685_PWM_MASK)
			led12bWhite[num].color = PCA9685_PWM_MASK;
		if(!pca9685_setPWM(led12bWhite[num].channel, led12bWhite[num].color))
		{
			os_printf("setPWM %d,%d failed\n", led12bWhite[num].channel, led12bWhite[num].color);
		}
	}
}


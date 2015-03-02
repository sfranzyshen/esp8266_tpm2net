/*
 * led.c
 *
 *  Created on: Dec 21, 2014
 *      Author: sid
 */
#include "os_type.h"
#include "user_interface.h"
#include "espmissingincludes.h"
#include "mem.h"

#include "led/led.h"
#include "pca9685.h"
#include "apa102.h"
#include "ws28xx.h"

led_t *led = NULL;
uint16_t leds = 0;
struct __attribute__((packed))
{
	uint8_t pcaWhiteBehav[PCA_LEDS];
	uint8_t apa102Init :1;
	uint8_t pca9685Init :1;
	uint8_t ws281xInit :1;
	uint8_t ws2801Init :1;
} state;

os_event_t procTaskQueue[1];



void inline led_refresh(void)
{
	uint32_t pcaCh = 0;
	uint32_t loop;
	uint8_t apaDirty = 0, ws28xxDirty = 0;

	for (loop = 0; loop < leds; loop++)
	{
		if (led[loop].steps)
		{
			led[loop].color.red += led[loop].colorStep.red;
			led[loop].color.grn += led[loop].colorStep.grn;
			led[loop].color.blu += led[loop].colorStep.blu;
			led[loop].steps--;

			if(led[loop].type == LED_WS2801 || led[loop].type == LED_WS281X)
			{
				ws28xxDirty = 1;
			}
			else if(led[loop].type == LED_APA102)
			{
				apaDirty = 1;
			}
			else if(led[loop].type == LED_PCA9685)
			{
				uint32_t red = led[loop].color.red;
				uint32_t grn = led[loop].color.grn;
				uint32_t blu = led[loop].color.blu;
				uint32_t white = 0;

				if (state.pcaWhiteBehav[pcaCh] != WHITE_NA && state.pcaWhiteBehav[pcaCh] != WHITE_EXTRA)
				{
					white = min(red, min(grn, blu));

					if (state.pcaWhiteBehav[pcaCh] == WHITE_ADJUST)
					{
						white = min(0x2AAAA000, white);
						red -= white;
						grn -= white;
						blu -= white;
						white *= 3;
					}
				}

				/* Gamma correction. Gamma = 2.0 */
				red = ((uint32_t) (((uint16_t) (red >> 15)) * ((uint16_t) (red >> 15)))) >> led[loop].dim;
				grn = ((uint32_t) (((uint16_t) (grn >> 15)) * ((uint16_t) (grn >> 15)))) >> led[loop].dim;
				blu = ((uint32_t) (((uint16_t) (blu >> 15)) * ((uint16_t) (blu >> 15)))) >> led[loop].dim;

				pca9685_set32(pcaCh, red, grn, blu);
				if (pcaCh < PCA_LEDS && (state.pcaWhiteBehav[pcaCh] == WHITE_ADD || state.pcaWhiteBehav[pcaCh] == WHITE_ADJUST))
					pca9685_setWhite32(pcaCh, ((uint32_t) (((uint16_t) (white >> 15)) * ((uint16_t) (white >> 15)))) >> led[loop].dim);
				else if (pcaCh >= PCA_LEDS && pcaCh < (PCA_LEDS << 1) && state.pcaWhiteBehav[pcaCh - PCA_LEDS] == WHITE_EXTRA)
					pca9685_setWhite32(pcaCh - PCA_LEDS, red);
			}
		}
		if(led[loop].type == LED_PCA9685)		//count even if idle
			pcaCh++;
	}

	if(ws28xxDirty)
		ws28xx_transmitt(led, leds);

	if (apaDirty)
	{
		apa102_start();
		for(loop=0; loop<leds; loop++)
			if(led[loop].type == LED_APA102)
				apa102_set32(led_To32Bit(led[loop], LEDSTRPOS_RED), led_To32Bit(led[loop], LEDSTRPOS_GRN), led_To32Bit(led[loop], LEDSTRPOS_BLU));
		apa102_stop(leds);
	}
}

//LOCAL void procTask(os_event_t *events)
some_timerfunc(void *param)
{
	if (leds)
		led_refresh();

}

LOCAL void tim1_intr_handler(void)
{
//	RTC_CLR_REG_MASK(FRC1_INT_ADDRESS, FRC1_INT_CLR_MASK);
//	if (leds)
//	{
//		RTC_REG_WRITE(FRC1_LOAD_ADDRESS, US_TO_RTC_TIMER_TICKS(LED_INTERVALL_MS*1000));
//		system_os_post(USER_TASK_PRIO_0, 0, '0');
//	}
}

//public

void ICACHE_FLASH_ATTR led_deinit(void)
{
	leds = 0;
	if (led != NULL)
		os_free(led);

	if (state.apa102Init)
	{
		//todo apa102_deinit();
		state.apa102Init = 0;
	}
	if (state.pca9685Init)
	{
		pca9685_deinit();
		state.pca9685Init = 0;
	}
	if (state.ws2801Init)
	{
		ws28xx_deinit();
		state.ws2801Init = 0;
	}
	if (state.ws281xInit)
	{
		ws28xx_deinit();
		state.ws281xInit = 0;
	}

}

static os_timer_t some_timer;

uint16_t ICACHE_FLASH_ATTR led_init(uint16_t newLeds)
{
	if (leds || led != NULL) led_deinit();

	led = os_malloc(sizeof(led_t) * newLeds);
	if (led == NULL) return 0;
	leds = newLeds;

	state.apa102Init = 0;
	state.pca9685Init = 0;
	state.ws281xInit = 0;
	state.ws2801Init = 0;

	os_memset(led, 0, sizeof(led_t) * leds);

//	system_os_task(procTask, USER_TASK_PRIO_1, procTaskQueue, 1);

//	ETS_FRC_TIMER1_INTR_ATTACH(tim1_intr_handler, NULL);
//	TM1_EDGE_INT_ENABLE();
//	ETS_FRC1_INTR_ENABLE();

//	RTC_REG_WRITE(FRC1_CTRL_ADDRESS, DIVDED_BY_16 | FRC1_ENABLE_TIMER | TM_EDGE_INT);
//	RTC_REG_WRITE(FRC1_LOAD_ADDRESS, US_TO_RTC_TIMER_TICKS(LED_INTERVALL_MS*1000));

//software test:
	os_timer_disarm(&some_timer);
	os_timer_setfn(&some_timer, (os_timer_func_t *) some_timerfunc, NULL);
	os_timer_arm(&some_timer, 20, 1);

	return leds;
}

uint16_t ICACHE_FLASH_ATTR led_inited(void)
{
	return leds;
}

uint8_t ICACHE_FLASH_ATTR led_setType(ledrange_t range, uint8_t type)
{
	if (type == LED_PCA9685)
	{
		if (state.pca9685Init == 0)
		{
			pca9685_init();
			state.pca9685Init = 1;
		}
	}
	else if (type == LED_APA102)
	{
		if (state.apa102Init == 0)
		{
			apa102_init(leds);
			state.apa102Init = 1;
		}
	}
	else if (type == LED_WS2801)
	{
		if (state.ws281xInit == 1) return false;	// ws281x and WS2801 can't be initialized at same time
		if (state.ws2801Init == 0) 
		{
			ws28xx_init(type);
			state.ws2801Init = 1;
		}
	}
	else if (type == LED_WS281X)
	{
		if (state.ws2801Init == 1) return false;	// ws281x and WS2801 can't be initialized at same time
		if (state.ws281xInit == 0)
		{
			ws28xx_init(type);
			state.ws281xInit = 1;
		}
	}
	else return false;	// Invalid type
	
	// This loop is used multiple times here in led.c
	// There may be a better implementation posssible with a for loop, but all thoughts result in one unhandled case.
	int16_t i = range.from -1;	
	do 
	{
		i++;
		if (i==leds) i = 0;

		led[i].type = type;
	} while (i != range.to);
	
	return true;
}

uint8_t ICACHE_FLASH_ATTR led_setWhiteBehaviour(uint8_t ch0, uint8_t ch1, uint8_t ch2, uint8_t ch3)
{
	if ((ch0 > WHITE_EXTRA) || (ch1 > WHITE_EXTRA) || (ch2 > WHITE_EXTRA) || (ch3 > WHITE_EXTRA)) return false;

	state.pcaWhiteBehav[0] = ch0;
	state.pcaWhiteBehav[1] = ch1;
	state.pcaWhiteBehav[2] = ch2;
	state.pcaWhiteBehav[3] = ch3;
	
	return true;
}

uint8_t ICACHE_FLASH_ATTR led_setDim(ledrange_t range, uint8_t dim)
{
	int16_t i = range.from -1;
	do 
	{
		i++;
		if (i==leds) i = 0;

		if (led[i].type != LED_PCA9685) return false; 
		led[i].dim = dim & 0x0F;
	} while (i != range.to);

	return true;
}

uint8_t ICACHE_FLASH_ATTR led_set(ledrange_t range, rgb8_t ledValue, uint32_t ms)
{
	sint32_t targetRed = ledValue.red << 23 | 0x00400000;
	sint32_t targetGrn = ledValue.grn << 23 | 0x00400000;
	sint32_t targetBlu = ledValue.blu << 23 | 0x00400000;
	
	int16_t i = range.from -1;
	do 
	{
		i++;
		if (i==leds) i = 0;
		if (led[i].type == LED_NONE) return false;

		led[i].steps = ms / LED_INTERVALL_MS;
		if (led[i].steps < 2)				//set immediately
		{
			led[i].color.red = targetRed;
			led[i].colorStep.red = 0;
			led[i].color.grn = targetGrn;
			led[i].colorStep.grn = 0;
			led[i].color.blu = targetBlu;
			led[i].colorStep.blu = 0;
			led[i].steps = 1;
		}
		else
		{
			led[i].colorStep.red = ((sint32_t) (targetRed - led[i].color.red)) / led[i].steps;
			led[i].colorStep.grn = ((sint32_t) (targetGrn - led[i].color.grn)) / led[i].steps;
			led[i].colorStep.blu = ((sint32_t) (targetBlu - led[i].color.blu)) / led[i].steps;

			//os_printf("led%d: stepCol=%d target=%d col=%d\n", (int)i, (int)led[i].colorStep.red,(int)targetRed, (int)led[i].color.red);
		}
	} while (i != range.to);

	return true;
}

void ICACHE_FLASH_ATTR led_get(ledrange_t range, rgb8_t* value)
{
	uint32_t r = 0;
        uint32_t g = 0;
	uint32_t b = 0;

	int16_t i = range.from -1;
	do 
	{
		i++;
		if (i==leds) i = 0;

		r += led[i].color.red >> 23;
		g += led[i].color.grn >> 23;
		b += led[i].color.blu >> 23;
	} while (i != range.to);
	
	value->red = r / range.count;
	value->grn = g / range.count;
	value->blu = b / range.count;
}
	

void ICACHE_FLASH_ATTR led_checkRange(int32_t from, int32_t to, ledrange_t *range)
{
	int32_t tmp;

	// Negative modulo gives negative results in C, so do a workarround	
	tmp = from % leds;
	if (tmp < 0) tmp += leds;
	range->from = (uint16_t)tmp;
	
	tmp = to % leds;
	if (tmp < 0) tmp += leds;
	range->to = (uint16_t)tmp;

	tmp = range->to - range->from +1;
	if (tmp < 0) tmp += leds;
	range->count = (uint16_t)tmp;
}




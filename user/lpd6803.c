#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "os_type.h"
#include "lpd6803.h"

static uint16_t lpd6803_pixels[maxLEDs];
static uint16_t numLEDs = maxLEDs;

static int lpd6803_SendMode; // Used in interrupt 0=start,1=header,2=data,3=data done
static uint32_t lpd6803_BitCount;   // Used in interrupt
static uint16_t lpd6803_LedIndex; // Used in interrupt - Which LED we are sending.
static uint32_t lpd6803_BlankCounter;  //Used in interrupt.

static uint32_t lpd6803_lastdata = 0;
static uint16_t lpd6803_swapAsap = 0; //flag to indicate that the colors need an update asap

static os_timer_t lpd6803_timer;

void ICACHE_FLASH_ATTR lpd6803_LedOut() {
	switch (lpd6803_SendMode) {
	case LPD6803_DONE:            //Done..just send clocks with zero data
		if (lpd6803_swapAsap > 0) {
			if (!lpd6803_BlankCounter) //AS SOON AS CURRENT pwm IS DONE. BlankCounter
			{
				lpd6803_BitCount = 0;
				lpd6803_LedIndex = lpd6803_swapAsap;  //set current led
				lpd6803_SendMode = LPD6803_HEADER;
				lpd6803_swapAsap = 0;
				lpd6803_BlankCounter = 0;
			}
		}
		break;

	case LPD6803_DATA:               //Sending Data
		if ((1 << (15 - lpd6803_BitCount)) & lpd6803_pixels[lpd6803_LedIndex]) {
			if (!lpd6803_lastdata) { // digitalwrites take a long time, avoid if possible
				// If not the first bit then output the next bits
				// (Starting with MSB bit 15 down.)
				GPIO_OUTPUT_SET(0, 1);
				lpd6803_lastdata = 1;
			}
		} else {
			if (lpd6803_lastdata) { // digitalwrites take a long time, avoid if possible
				GPIO_OUTPUT_SET(0, 0);
				lpd6803_lastdata = 0;
			}
		}
		lpd6803_BitCount++;

		if (lpd6803_BitCount == 16)    //Last bit?
				{
			lpd6803_LedIndex++;        //Move to next LED
			if (lpd6803_LedIndex < numLEDs) //Still more leds to go or are we done?
			{
				lpd6803_BitCount = 0;  //Start from the fist bit of the next LED
			} else {
				// no longer sending data, set the data pin low
				GPIO_OUTPUT_SET(0, 0);
				lpd6803_lastdata = 0;      // this is a lite optimization
				lpd6803_SendMode = LPD6803_DONE; //No more LEDs to go, we are done!
			}
		}
		break;
	case LPD6803_HEADER:            //Header
		if (lpd6803_BitCount < 32) {
			GPIO_OUTPUT_SET(0, 0);
			lpd6803_lastdata = 0;
			lpd6803_BitCount++;
			if (lpd6803_BitCount == 32) {
				lpd6803_SendMode = LPD6803_DATA; //If this was the last bit of header then move on to data.
				lpd6803_LedIndex = 0;
				lpd6803_BitCount = 0;
			}
		}
		break;
	case LPD6803_START:            //Start
		if (!lpd6803_BlankCounter) //AS SOON AS CURRENT pwm IS DONE. BlankCounter
		{
			lpd6803_BitCount = 0;
			lpd6803_LedIndex = 0;
			lpd6803_SendMode = LPD6803_HEADER;
		}
		break;
	}

	// Clock out data (or clock LEDs)
	GPIO_OUTPUT_SET(2, 1);            //High
	GPIO_OUTPUT_SET(2, 0);            //Low

	//Keep track of where the LEDs are at in their pwm cycle.
	lpd6803_BlankCounter++;
}

void ICACHE_FLASH_ATTR lpd6803_setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b) {
	uint16_t data;

	if (n > numLEDs)
		return;

	data = (int) ((g / 255.0f) * 31);
	data <<= 5;
	data |= (int) ((r / 255.0f) * 31);
	data <<= 5;
	data |= (int) ((b / 255.0f) * 31);
	data |= 0x8000;

	lpd6803_pixels[n] = data;
}

void ICACHE_FLASH_ATTR lpd6803_setAllPixelColor(uint8_t r, uint8_t g, uint8_t b) {
	uint16_t i;
	for (i = 0; i < numLEDs; i++) {
		lpd6803_setPixelColor(i, r, g, b);
	}

}

void ICACHE_FLASH_ATTR lpd6803_init() {
	gpio_init();
	//Set GPIO2 to output mode for CLCK
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2);
	GPIO_OUTPUT_SET(2, 0);

	//Set GPIO0 to output mode for DATA
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0);
	GPIO_OUTPUT_SET(0, 0);

	lpd6803_setAllPixelColor(0, 0, 0);

	os_timer_disarm(&lpd6803_timer);
	os_timer_setfn(&lpd6803_timer, (os_timer_func_t *) lpd6803_LedOut, NULL);
	os_timer_arm_us(&lpd6803_timer, 40, 1);

	lpd6803_show();
}

void ICACHE_FLASH_ATTR lpd6803_show(void) {
	lpd6803_BitCount = lpd6803_LedIndex = lpd6803_BlankCounter = 0;
	lpd6803_SendMode = LPD6803_START;
}

void ICACHE_FLASH_ATTR lpd6803_strip(uint8_t * buffer, uint16_t length) {
	if (lpd6803_SendMode != LPD6803_DONE) { // wait until data is done?
		return; //drop frame
	}
	numLEDs = length/3;

	uint16_t i;
	for( i = 0; i < numLEDs; i++ ) {
		uint16_t cled = i * 3; // current led array location
		lpd6803_setPixelColor(i, buffer[cled], buffer[cled+1], buffer[cled+2]); // load up lpd6803's data array
	}

	lpd6803_show();
}


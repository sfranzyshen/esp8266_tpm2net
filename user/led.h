/*
 * led.h
 *
 *  Created on: Dec 21, 2014
 *      Author: sid
 */

#ifndef LED_H_
#define LED_H_

#define LED_INTERVALL_MS	20			//min 2

enum
{
	LED_NONE,
	LED_PCA9685,
	LED_APA102,
	LED_WS281X,
	LED_WS2801
}LED_TYPE;

enum
{
	WHITE_NA,					//white gets ignored
	WHITE_ADJUST,					//some rgb color gets replaced by white
	WHITE_ADD,					//some rgb color gets added by white channel
	WHITE_EXTRA					//r channel of 4th-7th PCA led
}PCA_BEHAV;

#define LEDSTRPOS_RED	0
#define LEDSTRPOS_GRN	1
#define LEDSTRPOS_BLU	2


#define led_To32Bit(l, ch) (((uint32_t) (((uint16_t) (l.color.raw[ch] >> 15)) * ((uint16_t) (l.color.raw[ch] >> 15)))) >> l.dim)

typedef union __attribute__((packed))
{
	struct
	{
		uint8_t red;
		uint8_t grn;
		uint8_t blu;
	};
	uint8_t raw[3];
} rgb8_t;

typedef union __attribute__((packed))
{
	struct
	{
		uint16_t red;
		uint16_t grn;
		uint16_t blu;
	};
	uint16_t raw[3];
} rgb16_t;

typedef union __attribute__((packed))
{
	struct
	{
		sint16_t red;
		sint16_t grn;
		sint16_t blu;
	};
	sint16_t raw[3];
} srgb16_t;

typedef union __attribute__((packed))
{
	struct
	{
		uint32_t red;
		uint32_t grn;
		uint32_t blu;
	};
	uint32_t raw[3];
} rgb32_t;

typedef union __attribute__((packed))
{
	struct
	{
		sint32_t red;
		sint32_t grn;
		sint32_t blu;
	};
	sint32_t raw[3];
} srgb32_t;

typedef struct __attribute__((packed))
{
	srgb32_t color;
	srgb32_t colorStep;
	sint32_t steps;
	uint8_t	type:4;
	uint8_t dim:4;
	union __attribute__((packed))
	{
		uint8_t channel;
	} misc;
} led_t;

typedef struct
{
	uint16_t from;
	uint16_t to;
	uint16_t count;
} ledrange_t;

#define min(a,b) ((a)<(b)?(a):(b))  /**< Find the minimum of 2 numbers. */


//XXX: 0xffffffff/(80000000/16)=35A
#define US_TO_RTC_TIMER_TICKS(t)          \
    ((t) ?                                   \
     (((t) > 0x35A) ?                   \
      (((t)>>2) * ((APB_CLK_FREQ>>4)/250000) + ((t)&0x3) * ((APB_CLK_FREQ>>4)/1000000))  :    \
      (((t) *(APB_CLK_FREQ>>4)) / 1000000)) :    \
     0)

#define FRC1_ENABLE_TIMER  BIT7

typedef enum {
    DIVDED_BY_1 = 0,
    DIVDED_BY_16 = 4,
    DIVDED_BY_256 = 8,
} TIMER_PREDIVED_MODE;

typedef enum {
    TM_LEVEL_INT = 1,
    TM_EDGE_INT   = 0,
} TIMER_INT_MODE;


void led_deinit(void);
uint16_t led_init(uint16_t newLeds);
uint16_t led_inited(void);

uint8_t led_set(ledrange_t range, rgb8_t ledValue, uint32_t ms);
void led_get(ledrange_t range, rgb8_t* value);

uint8_t led_setType(ledrange_t range, uint8_t type);
uint8_t led_setWhiteBehaviour(uint8_t ch0, uint8_t ch1, uint8_t ch2, uint8_t ch3);
uint8_t led_setDim(ledrange_t range, uint8_t value);

void led_checkRange(int32_t from, int32_t to, ledrange_t *range);

#endif /* LED_H_ */

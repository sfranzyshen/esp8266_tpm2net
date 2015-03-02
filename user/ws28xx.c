/*
 * ws28xx.c
 *
 *  Created on: Jan 30, 2015
 *      Author: sid
 */

#include "c_types.h"
#include "osapi.h"
#include "espmissingincludes.h"
#include "driver/uart.h"

#include "led/led.h"
#include "ws28xx.h"

led_t *ws28xx_led = NULL;
unsigned int leds_total;
unsigned int current_led;

// UartDev is defined and initialized in rom code.
extern UartDevice UartDev;

void ICACHE_RAM_ATTR ws28xx_color(uint8_t b0, uint8_t b1, uint8_t b2)
{
	WRITE_PERI_REG(UART_FIFO(UART1), (b0&0x80?0:0x01) | (b0&0x40?0:0x08) | (b0&0x20?0:0x40) | 0x12);
	WRITE_PERI_REG(UART_FIFO(UART1), (b0&0x10?0:0x01) | (b0&0x08?0:0x08) | (b0&0x04?0:0x40) | 0x12);
	WRITE_PERI_REG(UART_FIFO(UART1), (b0&0x02?0:0x01) | (b0&0x01?0:0x08) | (b1&0x80?0:0x40) | 0x12);
	WRITE_PERI_REG(UART_FIFO(UART1), (b1&0x40?0:0x01) | (b1&0x20?0:0x08) | (b1&0x10?0:0x40) | 0x12);
	WRITE_PERI_REG(UART_FIFO(UART1), (b1&0x08?0:0x01) | (b1&0x04?0:0x08) | (b1&0x02?0:0x40) | 0x12);
	WRITE_PERI_REG(UART_FIFO(UART1), (b1&0x01?0:0x01) | (b2&0x80?0:0x08) | (b2&0x40?0:0x40) | 0x12);
	WRITE_PERI_REG(UART_FIFO(UART1), (b2&0x20?0:0x01) | (b2&0x10?0:0x08) | (b2&0x08?0:0x40) | 0x12);
	WRITE_PERI_REG(UART_FIFO(UART1), (b2&0x04?0:0x01) | (b2&0x02?0:0x08) | (b2&0x01?0:0x40) | 0x12);
}

LOCAL void inline ICACHE_RAM_ATTR fill_fifo(void)
{
	if((WS28XX_FIFO_CNT >= 126 && !current_led) || ws28xx_led == NULL)
	{
		//uart_tx_one_char(UART1, '.');

		//we were too slow
		if(!current_led && ws28xx_led != NULL)
		{
			for(current_led=0; current_led<leds_total; current_led++)
			{
				if(ws28xx_led[current_led].type==LED_WS2801 || ws28xx_led[current_led].type==LED_WS281X)
				{
					if(ws28xx_led[current_led].steps == 0)
					{
						ws28xx_led[current_led].steps = 1;			//ensure dirty ws28xx buffer next time
						ws28xx_led[current_led].colorStep.red = 0;
						ws28xx_led[current_led].colorStep.grn = 0;
						ws28xx_led[current_led].colorStep.blu = 0;
					}
					break;
				}
			}
		}
		ws28xx_led = NULL;

		//disable tx_interrupt
		CLEAR_PERI_REG_MASK(UART_INT_ENA(UART1), UART_TXFIFO_EMPTY_INT_ENA);

		return;
	}

	while(WS28XX_FIFO_CNT < 119)	//126-7 (more)
	{
		if(ws28xx_led[current_led].type == LED_WS2801 || ws28xx_led[current_led].type == LED_WS281X)
		{

			/* Gamma correction. Gamma = 2.0 */
			uint32_t red = led_To32Bit(ws28xx_led[current_led], LEDSTRPOS_RED);
			uint32_t grn = led_To32Bit(ws28xx_led[current_led], LEDSTRPOS_GRN);
			uint32_t blu = led_To32Bit(ws28xx_led[current_led], LEDSTRPOS_BLU);

			if(ws28xx_led[current_led].type == LED_WS2801)
			{
//				if(!current_led)
//					uart_tx_one_char(UART0, red>>24);

				ws28xx_color(red>>24, grn>>24, blu>>24);
			}
			else if(ws28xx_led[current_led].type == LED_WS281X)
			{
				ws28xx_color(grn>>24, red>>24, blu>>24);
			}
		}

		if(++current_led >= leds_total)
		{
			//we are done
			ws28xx_led = NULL;

			//disable tx_interrupt
			CLEAR_PERI_REG_MASK(UART_INT_ENA(UART1), UART_TXFIFO_EMPTY_INT_ENA);
			break;
		}
	}


}

LOCAL void ICACHE_RAM_ATTR intr_handler(void *para)
{
	/* uart0 and uart1 intr combine together, when interrupt occur, see reg 0x3ff20020, bit2, bit0 represents
	 * uart1 and uart0 respectively
	 */

	if(READ_PERI_REG(UART_INT_ST(UART1)))	//any UART1 stuff
	{

		if(UART_TXFIFO_EMPTY_INT_ST == (READ_PERI_REG(UART_INT_ST(UART1)) & UART_TXFIFO_EMPTY_INT_ST))
			fill_fifo();

		//clr int
//		WRITE_PERI_REG(UART_INT_CLR(UART1), UART_TXFIFO_EMPTY_INT_CLR);
		WRITE_PERI_REG(UART_INT_CLR(UART1), 0xffff);	//just to be sure :)
	}

	if(READ_PERI_REG(UART_INT_ST(UART0)))	//any UART0 stuff
	{
		uart0_rx_intr_handler(&(UartDev.rcv_buff));
	}
}

void ICACHE_FLASH_ATTR ws28xx_init(uint8_t type)
{
	PIN_FUNC_SELECT(WS28XX_MUX, WS28XX_FUNC);

	if(type == LED_WS281X)
		gpio_output_set(1<<WS28XX_GPIO, 0, 1<<WS28XX_GPIO, 0);
	else if(type == LED_WS2801)
		gpio_output_set(0, 1<<WS28XX_GPIO, 1<<WS28XX_GPIO, 0);
	else
		return;

	//todo: give the device 25ms to sattle i.e. delay timer


	ETS_UART_INTR_DISABLE();

	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_U1TXD_BK);

	uart_div_modify(1, UART_CLK_FREQ / (2400000));

	//7n1
	WRITE_PERI_REG(UART_CONF0(UART1), (ONE_STOP_BIT << UART_STOP_BIT_NUM_S) | (SEVEN_BITS << UART_BIT_NUM_S));

	//clear rx and tx fifo,not ready
	SET_PERI_REG_MASK(UART_CONF0(UART1), UART_RXFIFO_RST | UART_TXFIFO_RST);
	CLEAR_PERI_REG_MASK(UART_CONF0(UART1), UART_RXFIFO_RST | UART_TXFIFO_RST);

	//overload uart int handler
	ETS_UART_INTR_ATTACH(intr_handler,  NULL);

	//disable rx_interrupt. gets previously enables by uart.c
	CLEAR_PERI_REG_MASK(UART_INT_ENA(UART1), UART_RXFIFO_FULL_INT_ENA);

	//set tx fifo trigger
	WRITE_PERI_REG(UART_CONF1(UART1), 54 << UART_TXFIFO_EMPTY_THRHD_S); //gives us 200uS to reakt

	//disable tx_interrupt
	CLEAR_PERI_REG_MASK(UART_INT_ENA(UART1), UART_TXFIFO_EMPTY_INT_ENA);

	//clear all interrupt
	WRITE_PERI_REG(UART_INT_CLR(UART1), 0xffff);

	ETS_UART_INTR_ENABLE();
}

void ICACHE_FLASH_ATTR ws28xx_deinit(void)
{
	gpio_output_set(0, 0, 0, 1<<WS28XX_GPIO);
	CLEAR_PERI_REG_MASK(UART_INT_ENA(UART1), UART_TXFIFO_EMPTY_INT_ENA);

	//disable tx_interrupt
	CLEAR_PERI_REG_MASK(UART_INT_ENA(UART1), UART_TXFIFO_EMPTY_INT_ENA);
}

void ICACHE_RAM_ATTR ws28xx_transmitt(led_t *leds, unsigned int total)
{
	if(ws28xx_led != NULL)
	{
		//not quite done here
		return;
	}

	ws28xx_led = leds;
	leds_total = total;
	current_led = 0;

	//enable tx_interrupt
	SET_PERI_REG_MASK(UART_INT_ENA(1), UART_TXFIFO_EMPTY_INT_ENA);
}

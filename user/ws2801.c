#include <c_types.h>
#include <eagle_soc.h>
#include <gpio.h>

void ICACHE_FLASH_ATTR ws2801_byte(uint8_t n) {
	uint8_t bitmask;
	for (bitmask=0x80; bitmask!=0; bitmask>>=1) {
		if (n & bitmask) GPIO_REG_WRITE(GPIO_OUT_W1TS_ADDRESS, BIT2);
		else GPIO_REG_WRITE(GPIO_OUT_W1TC_ADDRESS, BIT2);
		GPIO_REG_WRITE(GPIO_OUT_W1TS_ADDRESS, BIT0);
		GPIO_REG_WRITE(GPIO_OUT_W1TC_ADDRESS, BIT0);
	}
}

void ICACHE_FLASH_ATTR ws2801_strip(uint8_t * data, uint16_t len) {
	while (len--) {
		ws2801_byte(*(data++));
	}
	GPIO_REG_WRITE(GPIO_OUT_W1TC_ADDRESS, BIT2);
}

void ICACHE_FLASH_ATTR ws2801_init() {
    //Set GPIO2 and GPIO0 to output mode
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0);

    //Set both GPIOs low low
    gpio_output_set(0, BIT0|BIT2, BIT0|BIT2, 0);
}

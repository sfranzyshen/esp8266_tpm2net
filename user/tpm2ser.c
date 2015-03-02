#include "osapi.h"
#include "ets_sys.h"
#include "driver/uart.h"

void ICACHE_FLASH_ATTR tpm2ser_strip(uint8_t * buffer, uint16_t length) {
	uart0_tx_buffer(0xC9, 1); // start frame 
	uart0_tx_buffer(0xDA, 1); // data command 
	uart0_tx_buffer((length >> 8 & 0xFF), 1); // frame length (High Byte)
	uart0_tx_buffer((length & 0xFF), 1); // frame length (Low Byte)
	uart0_tx_buffer(buffer, length); // rgb data
	uart0_tx_buffer(0x36, 1); // data command 
}

void ICACHE_FLASH_ATTR tpm2ser_init() {
	uart_init(BIT_RATE_115200, BIT_RATE_115200);
}

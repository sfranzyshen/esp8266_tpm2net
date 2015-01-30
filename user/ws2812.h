#ifndef _WS2812_H
#define _WS2812_H

#define WSGPIO 0

#include "ets_sys.h"

void ICACHE_FLASH_ATTR ws2812_out( uint8_t * buffer, uint16_t length );
void ICACHE_FLASH_ATTR ws2812_init();

#endif


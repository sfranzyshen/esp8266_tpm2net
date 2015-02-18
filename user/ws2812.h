#ifndef _WS2812_H
#define _WS2812_H

#include "ets_sys.h"

void ICACHE_FLASH_ATTR ws2812_strip( uint8_t * buffer, uint16_t length );
void ICACHE_FLASH_ATTR ws2812_init();

#endif


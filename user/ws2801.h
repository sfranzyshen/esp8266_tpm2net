#ifndef _WS2801_H_
#define _WS2801_H_

#include "ets_sys.h"

void ICACHE_FLASH_ATTR ws2801_init();
void ICACHE_FLASH_ATTR ws2801_strip(uint8_t * data, uint16_t len);

#endif

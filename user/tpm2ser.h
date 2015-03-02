#ifndef _TPM2SER_H
#define _TPM2SER_H

#include "ets_sys.h"

void ICACHE_FLASH_ATTR tpm2ser_strip( uint8_t * buffer, uint16_t length );
void ICACHE_FLASH_ATTR tpm2ser_init();

#endif

#define maxLEDs 256
#define LPD6803_START 0
#define LPD6803_HEADER 1
#define LPD6803_DATA 2
#define LPD6803_DONE 3

void ICACHE_FLASH_ATTR lpd6803_setPixelColor(uint16_t n, uint8_t b, uint8_t r, uint8_t g);
void ICACHE_FLASH_ATTR lpd6803_setAllPixelColor(uint8_t r, uint8_t g, uint8_t b);
void ICACHE_FLASH_ATTR lpd6803_strip(uint8_t * buffer, uint16_t length);
void ICACHE_FLASH_ATTR lpd6803_init();
void ICACHE_FLASH_ATTR lpd6803_show(void);



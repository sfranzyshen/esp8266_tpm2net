/*
 * spi_master.h
 *
 *  Created on: Nov 30, 2014
 *      Author: sid
 */

#ifndef SPI_MASTER_H_
#define SPI_MASTER_H_

#include "led_spi_register.h"

/*SPI number define*/
#define SPI         0
#define HSPI        1

void led_spi_master_init(uint8 spi_no);
//void spi_master_8bit_write(uint8 spi_no, uint8 byte);
//void spi_master_9bit_write(uint8 spi_no, uint8 high_bit, uint8 low_8bit);
void led_spi_master_16bit_write(uint8 spi_no, uint8_t lowFirstByte, uint8 highLastByte);
//void spi_master_WSnibble_write(uint8 spi_no, uint8_t nibble);



#endif /* SPI_MASTER_H_ */

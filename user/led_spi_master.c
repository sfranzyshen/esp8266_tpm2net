/*
 * spi_master.c
 *
 *  Created on: Nov 30, 2014
 *      Author: sid
 */

#include "ets_sys.h"
#include "osapi.h"
#include "led_spi_master.h"

void ICACHE_FLASH_ATTR led_spi_master_init(uint8 spi_no)
{
	uint32 regvalue;

	if (spi_no > 1)
	{
		return;
	}

	WRITE_PERI_REG(PERIPHS_IO_MUX, 0x105); //clear bit9

	if (spi_no == SPI)
	{
		PIN_FUNC_SELECT(PERIPHS_IO_MUX_SD_CLK_U, 1); //configure io to spi mode
		PIN_FUNC_SELECT(PERIPHS_IO_MUX_SD_CMD_U, 1); //configure io to spi mode
		PIN_FUNC_SELECT(PERIPHS_IO_MUX_SD_DATA0_U, 1); //configure io to spi mode
		PIN_FUNC_SELECT(PERIPHS_IO_MUX_SD_DATA1_U, 1); //configure io to spi mode
	}
	else if (spi_no == HSPI)
	{
		//PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, 2); //configure io to spi mode
		PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, 2); //configure io to spi mode			//do
		PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, 2); //configure io to spi mode			//clk
		//PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDO_U, 2); //configure io to spi mode
	}

	regvalue = READ_PERI_REG(SPI_FLASH_USER(spi_no));
	regvalue = regvalue | SPI_USR_COMMAND;
	regvalue = regvalue & (~BIT2); //clear bit 2, undocumented
	WRITE_PERI_REG(SPI_FLASH_USER(spi_no), regvalue);
	WRITE_PERI_REG(SPI_FLASH_CTRL1(spi_no), 0); //reduces time between transmissions by 1us
	//fastest:    WRITE_PERI_REG(SPI_FLASH_CLOCK(spi_no), (0<<SPI_CLKDIV_PRE_S) | (5<<SPI_CLKCNT_N_S) | (0<<SPI_CLKCNT_H_S) | (4<<SPI_CLKCNT_L_S)); //clear bit 31,set SPI clock div
	WRITE_PERI_REG(SPI_FLASH_CLOCK(spi_no), (0<<SPI_CLKDIV_PRE_S) | (10<<SPI_CLKCNT_N_S) | (0<<SPI_CLKCNT_H_S) | (5<<SPI_CLKCNT_L_S)); //clear bit 31,set SPI clock div
}

void ICACHE_FLASH_ATTR led_spi_master_16bit_write(uint8 spi_no, uint8_t lowFirstByte, uint8 highLastByte)
{
	uint32 regvalue;
	if (spi_no > 1)
	{
		return;
	}

	regvalue = 0xF0000000 | ((uint32) (highLastByte << 8) | lowFirstByte);		//configure transmission variable,9bit transmission length and first 8 command bit

	while (READ_PERI_REG(SPI_FLASH_CMD(spi_no)) & SPI_FLASH_USR)
		;		//waiting for spi module available

	WRITE_PERI_REG(SPI_FLASH_USER2(spi_no), regvalue);				//write  command and command length into spi reg
	SET_PERI_REG_MASK(SPI_FLASH_CMD(spi_no), SPI_FLASH_USR);		//transmission start
}


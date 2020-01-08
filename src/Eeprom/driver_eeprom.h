/**************************************************************************
*   Copyright Glenn Self 2015
*
*	This file lcEeprom.h is part of spiEEEPROM.
*
*   spiEEEPROM is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   spiEEEPROM is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with spiEEEPROM.  If not, see <http://www.gnu.org/licenses/>.
***************************************************************************/
/*
 * eeprom8k.h
 *
 *  Created on: 9 Dec 2015
 *      Author: glenn
 */

#ifndef _EEPROM_DRIVER_H_
#define _EEPROM_DRIVER_H_

#include "stdint.h"
#include "stdbool.h"


//#define USING_25LC640
#undef	USING_25LC640


#define USING_25LC512
//#undef	USING_25LC512

//#define USING_25LC256
#undef	USING_25LC256


#define USING_MEMORY_PROTECT
//#undef  USING_MEMORY_PROTECT

#if defined USING_25LC640
/*
• 8192 x 8 Bit Organization
• 32 Byte Page
• Write Cycle Time: 5 ms max
*/
#define EEPROM_SIZE					8192
	#define EEPROM_PAGE_SIZE_BYTES		32
	#define SPI_SPEED_MHZ				10
#endif


#if defined USING_25LC512
/*
64K x 8-bit (512 Kbit)
- 128-byte page
- 5 ms max.
- No page or sector erase required

Supports SPI modes 0 and 3
Up to 20 MHz clock frequency
Self-Timed Erase and Write Cycles (5 ms max.)
Read current: 10 mA (Max) at 20 MHz
Write current: 5 mA (Max) at 20 MHz

 */
	#define EEPROM_SIZE					65536
	#define EEPROM_PAGE_SIZE_BYTES		128
	#define SPI_SPEED_MHZ				20

#endif


#if defined USING_25LC256
	#define EEPROM_SIZE					32768
	#define EEPROM_PAGE_SIZE_BYTES		64
	#define SPI_SPEED_MHZ				10
#endif


#define EEPROM_READ 	0x3				//Read data from memory array beginning at selected address
#define EEPROM_WRITE 	0x2 			//Write data to memory array beginning at selected address
#define EEPROM_WREN 	0x6		 		//Set the write enable latch (enable write operations)
#define EEPROM_WRDI 	0x4 			//Reset the write enable latch (disable write operations)
#define EEPROM_RDSR 	0x5		 		//Read STATUS register
#define EEPROM_WRSR 	0x1		 		//Write STATUS register
#if defined USING_25LC512
#define EEPROM_PE 		0x42			// Page Erase – erase one page in memory array
#define EEPROM_SE 		0xd0			//11011000 Sector Erase – erase one sector in memory array
#define EEPROM_CE 		0c7				//1100 0111 Chip Erase – erase all sectors in memory array
#endif



#define EEPROM_NUM_PAGES		    	EEPROM_SIZE/EEPROM_PAGE_SIZE_BYTES
#define EEPROM_FIRST_QUARTER_START 		(0)
#define EEPROM_SECOND_QUARTER_START		(EEPROM_SIZE/4)
#define EEPROM_THIRD_QUARTER_START		(EEPROM_SIZE/2)
#define EEPROM_TOP_HALF_START			EEPROM_THIRD_QUARTER_START
#define EEPROM_FOURTH_QUARTER_START		(EEPROM_SIZE/4 *3)
#define EEPROM_TOP_QUARTER_START		EEPROM_FOURTH_QUARTER_START


//#define DEBUG_PORT		GPIOB
//#define DEBUG_PIN		GPIO_PIN_1

//#define TFT_SET_CS 	GPIOC->BSRR = GPIO_Pin_8;
//#define TFT_CLR_CS 	GPIOC->BRR = GPIO_Pin_8;

//#define DEBUG_PIN_HIGH()	DEBUG_PORT->BSRR =DEBUG_PIN;
//#define DEBUG_PIN_LOW()		DEBUG_PORT->BRR  =DEBUG_PIN;

#define SPI_EEPROM		SPI1		//on APB2 higher speed than spi2 unless we change it, which should lower power use

#define EEPROM_CS_PIN		GPIO_PIN_1
#define EEPROM_CS_PORT		GPIOC
#define EEPROM_SCK_PIN		GPIO_PIN_10
#define EEPROM_MISO_PIN		GPIO_PIN_2
#define EEPROM_MOSI_PIN		GPIO_PIN_3
//#define EEPROM_WP_PIN		GPIO_Pin3
//#define EEPROM_HOLD_PIN	GPIO_Pin2

#define   EEPROM_CS_GPIO_CLK_ENABLE()  __GPIOC_CLK_ENABLE()



#define FLAG_WIP				0
#define FLAG_WEL				1
#define FLAG_BP0				2
#define FLAG_BP1				3
#define FLAG_WPEN				7

struct status_reg_s
{
	uint8_t	wip:1;
	uint8_t wel:1;
	uint8_t bp0:1;
	uint8_t bp1:1;
	uint8_t na0:1;
	uint8_t na1:1;
	uint8_t na2:1;
	uint8_t wpen:1;
};

typedef union status_reg_u
{
	struct status_reg_s flags;
	uint8_t				byte;
}statusReg_t;


typedef enum error_enum
{
	errNone=0,
	errFail=1,
	errRead,
	errWrite,
	errBounds,
	errProtected,
}error_t;


typedef enum protect_enum
{
	EEPROM_ProtectNone,
	EEPROM_ProtectUpperQuarter,
	EEPROM_ProtectHalf,
	EEPROM_ProtectAll,
}eepromProtect_t;







extern void EEPROM_Protect(eepromProtect_t p);
uint8_t EEPROM_ReadByte(uint32_t addr);
error_t EEPROM_ReadBuffer(uint8_t *buff,uint32_t addr,uint16_t num );
error_t EEPROM_WriteByte(uint32_t addr,uint8_t byte);
error_t EEPROM_WriteByte_irqDisable(uint32_t addr, uint8_t byte);
error_t EEPROM_WritePage(uint8_t *array,uint16_t page);
uint8_t EEPROM_ReadStatusReg(void);
void EEPROM_WriteStatusReg(uint8_t val);
void EEPROM_WaitForWIP(void);
error_t EEPROM_WriteBuffer(uint8_t *array,uint32_t address, uint16_t num_bytes);

#if defined USING_MEMORY_PROTECT
error_t EEPROM_IsProtected(uint16_t low, uint16_t high);
void EEPROM_Protect(eepromProtect_t p);
error_t EEPROM_ProtectionTest(void);

extern void EPROM_CS_Init(void);

#endif //defined USING_MEMORY_PROTECT


#define GPIO_WriteBit(PORT, PIN, ACTION)		HAL_GPIO_WritePin(PORT, PIN, ACTION)
#define EEPROM_MilliSecDelay(DELAY)				HAL_Delay(DELAY)

#endif /* EEPROM8K_H_ */

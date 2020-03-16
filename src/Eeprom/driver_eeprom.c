/**************************************************************************
*   Copyright Glenn Self 2015
*
*	This file lcEeprom.c is part of spiEEEPROM.
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
* eeprom8k.c
 *  Created on: 9 Dec 2015
 *      Author: glenn
 *
 *      Version 1.0
 */
#include "driver_eeprom.h"
//#include "stm32f10x.h"
#include "stm32f0xx_hal.h"
#include "spi.h"


/****************************************************************************
 *! \brief
 *25LC512 has 128 byte page
 *25LC640A has 32 byte page
 *NOTE that HOLD pin needs to be H to enable the chip
 *
 *Instruction Name Instruction Format Description
 *8k LC640 and 64k LC512 commands
 *READ  0000 0011 Read data from memory array beginning at selected addr
 *WRITE 0000 0010 Write data to memory array beginning at selected addr
 *WREN  0000 0110 Set the write enable latch (enable write operations)
 *WRDI  0000 0100 Reset the write enable latch (disable write operations)
 *RDSR  0000 0101 Read STATUS register
 *WRSR  0000 0001 Write STATUS register
 *
 *Following are 64k LC512 commands only
 *PE 0100 0010 Page Erase – erase one page in memory array
 *SE 1101 1000 Sector Erase – erase one sector in memory array
 *CE 1100 0111 Chip Erase – erase all sectors in memory array
 *RDID 1010 1011 Release from Deep power-down and read electronic signature
 *DPD 1011 1001 Deep Power-Down mode
 *
 *Read sequence:
 * !CS =L
 * tx READ
 * tx 16 bit addr
 * now send SCK's of multiples of 8 to obtain sequential data at selected addr
 *
 * Write sequence:
 * !CS=L
 * tx WREN
 * !CS=H
 * Implicit internal erase cycle is performed. Presumably over before next
 * spi data is clocked in as no mention of pause reqd
 * !CS=L
 * clock out data (MSB first). Up to 128 bytes before new Write is reqd. All data
 * must be in same page.
 * !CS=H for data to be written.
 *
 *Read Status Reg.
 *RDSR can be read any time, even during a write cycle.
 *WIP bit =1 write in progress
 *WIP bit =0 write not in progress
 *WEL bit =1 Write Enable Latch allows writes
 *WEL bit =0 Write Enable Latch disallows writes
 *BP0/BP1 bits  00 = no protection
 *BP0/BP1 bits  01 = sectors 0,1,2 unprotected
 *BP0/BP1 bits  10 = sectors 0,1 unprotected
 *BP0/BP1 bits  11 = sectors ALL PROTECTED
 *
 *WEL is set :
 *on power up
 *after byte, page or STATUS Reg write
 *
 *PAGE ERASE
 * !CS =L
 * WREN
 * !CS=H
 * !CS=L
 * PE
 * addr byte 0
 * addr byte 1
 * !CS=H
 *
 * All memory inside the PAGE will be erased. The addr sent can be
 * any addr within the page. Doesn't have to be first/last etc.
 *
 * SECTOR ERASE
 *
 * !CS =L
 * WREN
 * !CS=H
 *
 * !CS=L
 * SE
 * addr byte 0
 * addr byte 1
 * !CS=L
 *
 * Read STATUS reg and test WIP bit to determine when write completed
 *
 *CHIP ERASE
  *!CS =L
 * WREN
 * !CS=H
 * !CS=L
 * CE
 * !CS=H
 * Read STATUS reg and test WIP bit to determine when write completed
 *
 *DEEP POWER DOWN
 *!CS=L
 *DPD
 *!CS=H
 *
 *After DPD chip will only respond to RDID
 *
 *RDID (Read Electronic Signature command)
 *!CS =L
 *RDID
 *Read 16 bits, the signature of the chip
 *!CS=H
 *Device now in standy mode ready to accept commands
 *
 ***************************************************************************/

/****************************************************************************
 *! \brief
 *Note that the internal write time for the LC640 is 5mS
 *So we must use the status reg to tell us when it is complete
 *Writing individual bytes is an inefficient mechanism as a write of
 *a single byte takes as long as a page write.
 ***************************************************************************/

/*NOTA:
 * The CPOL (clock polarity) bit controls the idle state value of
 * the clock when no data is being transferred. This bit affects both master and slave modes. If
 * CPOL is reset, the SCK pin has a low-level idle state. If CPOL is set, the SCK pin has a
 * high-level idle state.
 *
 *
 * */

 error_t EEPROM_WriteByte(uint32_t addr, uint8_t byte)
 {
 /*
 	 * !CS=L
 	 * tx WREN
 	 * !CS=H
 	 * Implicit internal erase cycle is performed. Presumably over before next
 	 * spi data is clocked in as no mention of pause reqd
 	 * !CS=L
 	 * clock out data (MSB first). Up to 32/128 bytes before new Write is reqd. All data
 	 * must be in same page.
 	 * !CS=H for data to be written.
 */
	 uint8_t status;
	 uint8_t* status_ptr;
     uint8_t error;
     status_ptr = &status;
     error=0;
#if defined 	USING_MEMORY_PROTECT
 	 if(EEPROM_IsProtected(addr,addr))
 	 {
 		 //return errProtected;
 		error=1;
 		//return error;
 	 }
#endif
 	//Set CS low for the write sequence
  	 GPIO_WriteBit(EEPROM_CS_PORT,EEPROM_CS_PIN, RESET);		//CS==L
 // EEPROM_MilliSecDelay(1);
  	 SpiTxRxByte(EEPROM_WREN, status_ptr);							//Send the write sequence
  	 GPIO_WriteBit(EEPROM_CS_PORT,EEPROM_CS_PIN, SET);			//CS==H
 // EEPROM_MilliSecDelay(1);
  	 //end of write sequence. Write latch is enabled

      //Set CS low for the command sequence
      GPIO_WriteBit(EEPROM_CS_PORT,EEPROM_CS_PIN, RESET);		//CS==L
 //EEPROM_MilliSecDelay(1);
      status = 7;
      SpiTxRxByte(EEPROM_WRITE, status_ptr);							//send the WRITE command
      if(status !=errorNONE){
   		error=2;
   		return error;
      }
      SpiTxRxByte((uint8_t)(addr >> 8), status_ptr);					//send H 8 bits of addr
      if(status !=errorNONE){
   		error=3;
   		return error;
      }
      SpiTxRxByte((uint8_t)(addr &255), status_ptr);					//send low 8 bits of addr
      if(status !=errorNONE){
   		error=4;
   		return error;
      }

      SpiTxRxByte(byte, status_ptr);									//send the byte DATA to the eeprom
      if(status !=errorNONE){
   		error=5;
   		return error;
      }

      //byte written. Page writes are more efficient as the write time
      //for a page and a byte are the same at about 5mS
      GPIO_WriteBit(EEPROM_CS_PORT,EEPROM_CS_PIN, SET);			//CS==H
 //  EEPROM_MilliSecDelay(1);
      //we have a minimum of 3 mS delay here so rather than generate a lot
      //of useless sck's and perhaps noise, lets do a software delay
      EEPROM_MilliSecDelay(4);		//this could be as little as 3mS and no greater than 4mS
      //EEPROM_MilliSecDelay(16);		//this could be as little as 3mS and no greater than 4mS
      EEPROM_WaitForWIP();
      return errNone;


 }


 error_t EEPROM_WriteByte_irqDisable(uint32_t addr, uint8_t byte)
  {
  /*
  	 * !CS=L
  	 * tx WREN
  	 * !CS=H
  	 * Implicit internal erase cycle is performed. Presumably over before next
  	 * spi data is clocked in as no mention of pause reqd
  	 * !CS=L
  	 * clock out data (MSB first). Up to 32/128 bytes before new Write is reqd. All data
  	 * must be in same page.
  	 * !CS=H for data to be written.
  */
 	 uint8_t status;
 	 uint8_t* status_ptr;
 	error_t error;


 	 uint32_t prim = __get_PRIMASK();
	 __disable_irq();

      status_ptr = &status;
      error=errNone;

 #if defined 	USING_MEMORY_PROTECT
  	 if(EEPROM_IsProtected(addr,addr))
  	 {
  		 //return errProtected;
  		error = errProtected;
  		//return error;
  	 }
 #endif
  	//Set CS low for the write sequence
   	 GPIO_WriteBit(EEPROM_CS_PORT,EEPROM_CS_PIN, RESET);		//CS==L
  // EEPROM_MilliSecDelay(1);
   	 SpiTxRxByte(EEPROM_WREN, status_ptr);							//Send the write sequence
   	 GPIO_WriteBit(EEPROM_CS_PORT,EEPROM_CS_PIN, SET);			//CS==H
  // EEPROM_MilliSecDelay(1);
   	 //end of write sequence. Write latch is enabled

       //Set CS low for the command sequence
       GPIO_WriteBit(EEPROM_CS_PORT,EEPROM_CS_PIN, RESET);		//CS==L
  //EEPROM_MilliSecDelay(1);
       status = 7;
       SpiTxRxByte(EEPROM_WRITE, status_ptr);							//send the WRITE command
       if(status !=errorNONE){
    		//error=2;
    		error=errFail;
    		//return error;
       }
       SpiTxRxByte((uint8_t)(addr >> 8), status_ptr);					//send H 8 bits of addr
       if(status !=errorNONE){
    		//error=3;
    		error=errFail;
    		//return error;
       }
       SpiTxRxByte((uint8_t)(addr &255), status_ptr);					//send low 8 bits of addr
       if(status !=errorNONE){
    		//error=4;
    	   error=errFail;
    		//return error;
       }

       SpiTxRxByte(byte, status_ptr);									//send the byte DATA to the eeprom
       if(status !=errorNONE){
    		//error=5;
    		error=errFail;
    		//return error;
       }

       //byte written. Page writes are more efficient as the write time
       //for a page and a byte are the same at about 5mS
       GPIO_WriteBit(EEPROM_CS_PORT,EEPROM_CS_PIN, SET);			//CS==H
  //  EEPROM_MilliSecDelay(1);
       //we have a minimum of 3 mS delay here so rather than generate a lot
       //of useless sck's and perhaps noise, lets do a software delay

		 if (!prim) {
			__enable_irq();
		}


       EEPROM_MilliSecDelay(4);		//this could be as little as 3mS and no greater than 4mS
       //EEPROM_MilliSecDelay(16);		//this could be as little as 3mS and no greater than 4mS
       EEPROM_WaitForWIP();


       return error ;


  }
/****************************************************************************
 *! \brief
 *Write a page of data. For the lc640 a page is 32 bytes
 *for the lc512 it is 128 bytes
 *This is essentially a byte write except that we always start at a page
 *boundary and always write a full page
 *Caller is responsible for allocating the array with
 *of size EEPROM_PAGE_SIZE_BYTES
 * ***************************************************************************/
error_t EEPROM_WritePage(uint8_t *array, uint16_t page)
{
	uint16_t addr,i;
	 byte status;

	if(page >= EEPROM_NUM_PAGES){
		return errBounds;
	}
	addr=(uint16_t) (page* EEPROM_PAGE_SIZE_BYTES);

#if defined 	USING_MEMORY_PROTECT
 	 if(EEPROM_IsProtected(addr,addr))
 	 {
 		 return errProtected;
 	 }
#endif


	GPIO_WriteBit(EEPROM_CS_PORT,EEPROM_CS_PIN, RESET);		//CS==L
	SpiTxRxByte(EEPROM_WREN, (uint8_t*)&status);						//Send the write sequence
    GPIO_WriteBit(EEPROM_CS_PORT,EEPROM_CS_PIN, SET);		//CS==H
   //write sequence finished, the CS->H is needed to set the Write enable latch.

    //now select the Chip again for the command sequence
    GPIO_WriteBit(EEPROM_CS_PORT,EEPROM_CS_PIN, RESET);		//CS==L
    SpiTxRxByte(EEPROM_WRITE, (uint8_t*)&status);						//now send the command
    SpiTxRxByte((uint8_t)(addr >> 8), (uint8_t*)&status);				//send H 8 bits of addr
    SpiTxRxByte((uint8_t)(addr &255), (uint8_t*)&status);				//send low 8 bits of addr
    //now send the page to the eeprom
 	for(i=0; i < EEPROM_PAGE_SIZE_BYTES; i++)
 	{
 		SpiTxRxByte(array[i], (uint8_t*)&status);
 	}
    GPIO_WriteBit(EEPROM_CS_PORT,EEPROM_CS_PIN, SET);		//CS==H

    //we have a minimum of 3 mS delay here so rather than generate a lot
    //of useless sck's lets do a software delay
    EEPROM_MilliSecDelay(4);		//this could be as little as 3mS and no greater than 4mS
    EEPROM_WaitForWIP();

     return errNone;
}

/****************************************************************************
 *! \brief
 *Write a buffer of data
 *For the lc640 a page is 32 bytes
 *for the lc512 it is 128 bytes
 * ***************************************************************************/
error_t EEPROM_WriteBuffer(uint8_t* array, uint32_t addr, uint16_t num)
{
	//uint16_t page;
	uint16_t offsetInPage;
	uint16_t cursor=0;
	byte status;


	//first find the page the write is directed to.
	if(addr > EEPROM_SIZE || addr+num > EEPROM_SIZE )
		return errBounds;


#if defined 	USING_MEMORY_PROTECT
	if(EEPROM_IsProtected( addr,(uint16_t)(addr+num)))
 	 {
 		 return errProtected;
 	 }
#endif



	while(num) //si num es cero no hace nada !
	{

		//Set the Write Enable Latch
		offsetInPage=addr % EEPROM_PAGE_SIZE_BYTES;
		//now write to the end of the page
		GPIO_WriteBit(EEPROM_CS_PORT,EEPROM_CS_PIN, RESET);		//CS==L

		SpiTxRxByte(EEPROM_WREN, (uint8_t*)&status);						//Send the write sequence
		GPIO_WriteBit(EEPROM_CS_PORT,EEPROM_CS_PIN, SET);		//CS==H
		//write sequence finished, the CS->H is needed to set the Write enable latch.

		//now select the Chip again for the command sequence
		GPIO_WriteBit(EEPROM_CS_PORT,EEPROM_CS_PIN, RESET);		//CS==L
		SpiTxRxByte(EEPROM_WRITE, (uint8_t*)&status);						//now send the WRITE command
		SpiTxRxByte((uint8_t)(addr >> 8), (uint8_t*)&status);				//send H 8 bits of addr
		SpiTxRxByte((uint8_t)(addr &255), (uint8_t*)&status);				//send low 8 bits of addr

		while ((offsetInPage < EEPROM_PAGE_SIZE_BYTES) && (num >0) )
		{
			//send the byte
			SpiTxRxByte(array[cursor], (uint8_t*)&status);
			cursor++;
			addr++;
			offsetInPage++;
			num--;
		}
		GPIO_WriteBit(EEPROM_CS_PORT,EEPROM_CS_PIN, SET);		//CS==H
		EEPROM_MilliSecDelay(4);		//this could be as little as 3mS and no greater than 4mS
		EEPROM_WaitForWIP();
		offsetInPage=0;
	}

	return errNone;
}


#define dim_eepromBUFFER 128

uint8_t Tx_eepromBUFFER[dim_eepromBUFFER];
uint8_t Rx_eepromBUFFER[dim_eepromBUFFER];

error_t WriteBuffer_EEPROM(uint8_t *array, uint32_t addr, uint16_t num)
{
	//uint16_t page;
	uint16_t offsetInPage;
	uint16_t cursor=0;
	byte status;
	uint16_t sizeBUFF;
	//error_t error;

	//__disable_irq();

		//if (!prim) {
		//	__enable_irq();
		//}



	sizeBUFF = num+3;

	//first find the page the write is directed to.
	if(addr > EEPROM_SIZE || addr+num > EEPROM_SIZE )
		return errBounds;


#if defined 	USING_MEMORY_PROTECT

	if(EEPROM_IsProtected( addr,(uint16_t)(addr+num)))
 	 {
 		 return errProtected;
 	 }
#endif



   	while(num) //si num es cero no hace nada !
	{
   		GPIO_WriteBit(EEPROM_CS_PORT,EEPROM_CS_PIN, RESET);		//CS==L

		Tx_eepromBUFFER[0]=EEPROM_WREN;							//write sequence
   		status_spi_transfer = TRANSFER_WAIT;
		if(HAL_SPI_TransmitReceive_IT(&SpiHandle, (uint8_t*)Tx_eepromBUFFER, (uint8_t *)Rx_eepromBUFFER, 1) == HAL_OK){
			//espera finalizacion de transmision
			while(status_spi_transfer == TRANSFER_WAIT){
				if(status_spi_transfer ==TRANSFER_ERROR){
					status =errorFAIL;
				}
			};
			status =errorNONE;
		}else{
			status =errorFAIL;
		}
   		GPIO_WriteBit(EEPROM_CS_PORT,EEPROM_CS_PIN, SET);		//CS==L

		//Set the Write Enable Latch
		offsetInPage=addr % EEPROM_PAGE_SIZE_BYTES;
		//now write to the end of the page
		GPIO_WriteBit(EEPROM_CS_PORT,EEPROM_CS_PIN, RESET);		//CS==L

		Tx_eepromBUFFER[0]=EEPROM_WRITE;						//WRITE command
		Tx_eepromBUFFER[1]=(uint8_t)(addr >> 8);				//High 8 bits of addr
		Tx_eepromBUFFER[2]=(uint8_t)(addr &255);				//low 8 bits of addr

		while ((offsetInPage < EEPROM_PAGE_SIZE_BYTES) && (num >0) )
		{
			//send the byte
			Tx_eepromBUFFER[cursor+3] = array[cursor];
			cursor++;
			addr++;
			offsetInPage++;
			num--;
		}

		status_spi_transfer = TRANSFER_WAIT;
		if(HAL_SPI_TransmitReceive_IT(&SpiHandle, (uint8_t*)Tx_eepromBUFFER, (uint8_t *)Rx_eepromBUFFER, sizeBUFF) == HAL_OK){
			//espera finalizacion de transmision
			while(status_spi_transfer == TRANSFER_WAIT){
				if(status_spi_transfer ==TRANSFER_ERROR){
					status =errorFAIL;
				}
			};
			status =errorNONE;
		}else{
			status =errorFAIL;
		}
		GPIO_WriteBit(EEPROM_CS_PORT,EEPROM_CS_PIN, SET);		//CS==H
	    //espera a grabacion en eeprom
		EEPROM_MilliSecDelay(4);		//this could be as little as 3mS and no greater than 4mS
		EEPROM_WaitForWIP();
		offsetInPage=0;
	}


	return errNone;
}

/****************************************************************************
 *! \brief
 *Read a buffer of data
 ***************************************************************************/
error_t EEPROM_ReadBuffer(uint8_t* buff, uint32_t addr, uint16_t num)
	{
	/*
	 !CS =L
	 tx READ
	 tx 16 bit addr
	 now send SCK's of multiples of 8 to obtain sequential data at selected addr
	 EEPROM_CS
	 */
uint8_t tb;
uint16_t i;
byte status;


//Note although we check for EEPROM_SIZE overflow its not required
//Writing or reading beyond the limit of the eeprom will cause a wrap to
//offset 0. This might be useful behaviour in which case the following
//test can be removed
	if(addr > EEPROM_SIZE || num > EEPROM_SIZE || addr+num > EEPROM_SIZE )
		return errBounds;

	GPIO_WriteBit(EEPROM_CS_PORT, EEPROM_CS_PIN, RESET);		//CS==L
	tb=SpiTxRxByte(EEPROM_READ, (uint8_t*)&status);				//send the command to read the eeprom
	tb= (uint8_t)(addr>>8);
	tb=SpiTxRxByte(tb, (uint8_t*)&status);						//send the addr from which to read data (LSB)
	tb=(uint8_t)(addr &255);
	tb=SpiTxRxByte(tb, (uint8_t*)&status);						//send the addr from which to read data (MSB)
	//now clock in the data bytes
	for (i = 0; i < num; i++) {
		buff[i]=SpiTxRxByte(0, (uint8_t*)&status);
	}
	GPIO_WriteBit(EEPROM_CS_PORT, EEPROM_CS_PIN, SET);		//CS==H
	return (tb);
}




/****************************************************************************
 *! \brief
 *Read a single byte from the eeprom
  ***************************************************************************/

uint8_t EEPROM_ReadByte(uint32_t addr) {
	/*
	 !CS =L
	 tx READ
	 tx 16 bit addr
	 now send SCK's of multiples of 8 to obtain sequential data at selected addr
	 EEPROM_CS
	 */
uint8_t tb;
byte status;
	if(addr > EEPROM_SIZE)
		return errBounds;

	//drop CS
	GPIO_WriteBit(EEPROM_CS_PORT, EEPROM_CS_PIN, RESET);		//CS==L
	tb=SpiTxRxByte(EEPROM_READ, (uint8_t*)&status);				//send the command to read the eeprom

	tb= (uint8_t)(addr>>8);
	tb=SpiTxRxByte(tb, (uint8_t*)&status);						//send the addr from which to read data (LSB)
	tb=(uint8_t)(addr &255);
	tb=SpiTxRxByte(tb, (uint8_t*)&status);						//send the addr from which to read data (MSB)
	tb=SpiTxRxByte(0, (uint8_t*)&status);						//now clock in the data byte
	GPIO_WriteBit(EEPROM_CS_PORT, EEPROM_CS_PIN, SET);		//CS==H
	return (tb);
}



/****************************************************************************
 *! \brief
 *Read the status register:
 *RDSR
 ***************************************************************************/
uint8_t EEPROM_ReadStatusReg(void)
{
	statusReg_t sr;
	 byte status;

	GPIO_WriteBit(EEPROM_CS_PORT,EEPROM_CS_PIN, RESET);		//CS==L
	SpiTxRxByte(EEPROM_RDSR, (uint8_t*)&status);						//Send the Read Status Reg command
	sr.byte =SpiTxRxByte(0, (uint8_t*)&status);							//now clock in the status reg byte
 	GPIO_WriteBit(EEPROM_CS_PORT,EEPROM_CS_PIN, SET);		//CS==H
 	return sr.byte;
}


/****************************************************************************
 *! \brief
 *Write the Status reg:
 *WRSR
 ***************************************************************************/
void EEPROM_WriteStatusReg(uint8_t val)
{
	//enable WEL
	 byte status;

	GPIO_WriteBit(EEPROM_CS_PORT,EEPROM_CS_PIN, RESET);		//CS==L
	SpiTxRxByte(EEPROM_WREN, (uint8_t*)&status);						//Send the write sequence
	GPIO_WriteBit(EEPROM_CS_PORT,EEPROM_CS_PIN, SET);		//CS==H

	//write sequence finished, the CS->H is needed to set the Write enable latch.
	//WEL enabled


	//now write the status reg
	GPIO_WriteBit(EEPROM_CS_PORT,EEPROM_CS_PIN, RESET);		//CS==L
	SpiTxRxByte(EEPROM_WRSR, (uint8_t*)&status);						// Send the Read Status Reg command

	//BSP_LED_Toggle(LED3);	//PA5

	SpiTxRxByte(val, (uint8_t*)&status);								//now clock out the status reg byte
 	GPIO_WriteBit(EEPROM_CS_PORT,EEPROM_CS_PIN, SET);		//CS==H

	//an internal write cycle is generated when we write the status register
	//so we need to wait till it is complete
 	//BSP_LED_Toggle(LED3);	//PA5

 	if(spi_type ==TRANSFER_BY_IRQ){
 		//this could be as little as 3mS and no greater than 4mS
 	 	EEPROM_MilliSecDelay(4);
 	}

 	//BSP_LED_Toggle(LED3);	//PA5

 	EEPROM_WaitForWIP();
}


/****************************************************************************
 *! \brief
 *Poll the WIP (Write In Progress) flag till it is cleared
 ***************************************************************************/
void EEPROM_WaitForWIP()
{
	volatile statusReg_t sr;
	do
	{
		sr.byte=EEPROM_ReadStatusReg();
	}while(sr.flags.wip);

}




#if defined 	USING_MEMORY_PROTECT
/****************************************************************************
 *! \brief
 *
 * bp0:bp1
 * 0 no protection
 * 1 upper quarter protected
 * 2 upper half protected
 * 3 all protected
 *
 ***************************************************************************/
error_t EEPROM_IsProtected(uint16_t low, uint16_t high)
{

	statusReg_t sr;
	uint16_t p=0;
	sr.byte=EEPROM_ReadStatusReg();
	if(sr.flags.bp0)
	{
		p |= 1;
	}
	if(sr.flags.bp1)
	{
		p |=2;
	}

	switch (p)
	{
	case EEPROM_ProtectNone:
		return errNone;
		break;
	case EEPROM_ProtectUpperQuarter:
		//top Q protected
		if (low > EEPROM_TOP_QUARTER_START-1 || high > EEPROM_TOP_QUARTER_START-1 )
			return errProtected;
		break;
	case EEPROM_ProtectHalf:
		if (low > EEPROM_TOP_HALF_START-1 || high > EEPROM_TOP_HALF_START-1 )
			return errProtected;

		break;
	case 3:
			return errProtected;
		break;
	}
	return errNone;		//will never reach here but keep compiler from complaining
}


/****************************************************************************
 *! \brief
 *Protect the request block(s).
 ***************************************************************************/
void EEPROM_Protect(eepromProtect_t p)
{
	statusReg_t sr;
	sr.byte=EEPROM_ReadStatusReg();
	//BSP_LED_Toggle(LED3);	//PA5
	switch(p)
	{
	case EEPROM_ProtectNone:
		sr.flags.bp0=0;
		sr.flags.bp1=0;
		break;
	case EEPROM_ProtectUpperQuarter:
		sr.flags.bp0=1;
		sr.flags.bp1=0;
		break;
	case EEPROM_ProtectHalf:
		sr.flags.bp0=0;
		sr.flags.bp1=1;
		break;
	case EEPROM_ProtectAll:
		sr.flags.bp0=1;
		sr.flags.bp1=1;
		break;
	}
	//BSP_LED_Toggle(LED3);				//PA5
	EEPROM_WriteStatusReg(sr.byte);
	//BSP_LED_Toggle(LED3);				//PA5
}


/****************************************************************************
 *! \brief
 *Test the Protection status.
 *If we manage to write to a protected quadrant then return errorProtected
 ***************************************************************************/
error_t EEPROM_ProtectionTest(void)
{

	EEPROM_Protect(EEPROM_ProtectAll);

	//write to bottom quadrant
	if(EEPROM_WriteByte(0,15) != errProtected)
	{
		return errFail;
	}
	//write to 2nd quadrant
	if(EEPROM_WriteByte(EEPROM_TOP_HALF_START-1,15)  != errProtected)
	{
		return errFail;
	}

	//write to 3rd quadrant
	if(EEPROM_WriteByte(EEPROM_TOP_QUARTER_START-1,15)  != errProtected)
	{
		return errFail;
	}
	//write to 4th quadrant
	if(EEPROM_WriteByte(EEPROM_TOP_QUARTER_START,15)  != errProtected)
	{
		return errFail;
	}
	//----------finished protect all check----------------------


	EEPROM_Protect(EEPROM_ProtectUpperQuarter);

	//write to bottom quadrant
	if(EEPROM_WriteByte(0,15))
	{
		return errProtected;
	}
	//write to 2nd quadrant
	if(EEPROM_WriteByte(EEPROM_TOP_HALF_START-1,15))
	{
		return errProtected;
	}

	//write to 3rd quadrant
	if(EEPROM_WriteByte(EEPROM_TOP_QUARTER_START-1,15))
	{
		return errProtected;
	}
	//write to 4th quadrant
	if(EEPROM_WriteByte(EEPROM_TOP_QUARTER_START,15)  != errProtected)
	{
		return errFail;
	}
	//----------finished protect upper Q check----------------------


	EEPROM_Protect(EEPROM_ProtectHalf);
	//write to bottom quadrant
	if(EEPROM_WriteByte(0,15))
	{
		return errProtected;
	}
	//write to 2nd quadrant
	if(EEPROM_WriteByte(EEPROM_TOP_HALF_START-1,15))
	{
		return errProtected;
	}

	//write to 3rd quadrant
	if(EEPROM_WriteByte(EEPROM_TOP_QUARTER_START-1,15)  != errProtected)
	{
		return errFail;
	}
	//write to 4th quadrant
	if(EEPROM_WriteByte(EEPROM_TOP_QUARTER_START,15)  != errProtected)
	{
		return  errFail;
	}
	//----------finished upper half check----------------------


	EEPROM_Protect(EEPROM_ProtectNone);

	//write to bottom quadrant
	if(EEPROM_WriteByte(0,15))
	{
		return errProtected;
	}
	//write to 2nd quadrant
	if(EEPROM_WriteByte(EEPROM_TOP_HALF_START,15))
	{
		return errProtected;
	}

	//write to 3rd quadrant
	if(EEPROM_WriteByte(EEPROM_TOP_QUARTER_START-1,15))
	{
		return errProtected;
	}
	//write to 4th quadrant
	if(EEPROM_WriteByte(EEPROM_TOP_QUARTER_START,15))
	{
		return errProtected;
	}
	//----------finished protect all check----------------------

	return errNone;
}


void EPROM_CS_Init(void)
{
  GPIO_InitTypeDef  gpioinitstruct;

  /* Enable the GPIO_LED Clock */
  EEPROM_CS_GPIO_CLK_ENABLE();

  /* Configure the GPIO_LED pin */
  gpioinitstruct.Pin = EEPROM_CS_PIN;
  gpioinitstruct.Mode = GPIO_MODE_OUTPUT_PP;
  gpioinitstruct.Pull = GPIO_NOPULL;
  gpioinitstruct.Speed = GPIO_SPEED_FREQ_HIGH;

  HAL_GPIO_Init(EEPROM_CS_PORT, &gpioinitstruct);

  GPIO_WriteBit(EEPROM_CS_PORT,EEPROM_CS_PIN, SET);		//CS==H
}
#endif



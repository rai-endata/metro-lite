/*
 * spi.c
 *
 *  Created on: 26/4/2018
 *      Author: Rai
 */

#include "stm32f0xx_hal_spi.h"
#include "spi.h"



// Buffer used for transmission
uint8_t aTxBuffer[] = "****SPI - Two Boards communication based on Interrupt **** SPI Message ******** SPI Message ******** SPI Message ****";

// Buffer used for reception
uint8_t aRxBuffer[BUFFERSIZE];

// transfer state
uint8_t status_spi_transfer = TRANSFER_WAIT;

static uint16_t Buffercmp(uint8_t *pBuffer1, uint8_t *pBuffer2, uint16_t BufferLength);

/* SPI handler declaration */
SPI_HandleTypeDef SpiHandle;

spiMODE_en spi_type;

	void MX_SPIx_Init(void){
		SpiHandle.Instance               = SPIx;
		//SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
		SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
		SpiHandle.Init.Direction         = SPI_DIRECTION_2LINES;
		SpiHandle.Init.CLKPhase          = SPI_PHASE_1EDGE;
		SpiHandle.Init.CLKPolarity       = SPI_POLARITY_LOW; //clock en reposo
		//SpiHandle.Init.CLKPolarity       = SPI_POLARITY_HIGH;

		SpiHandle.Init.DataSize          = SPI_DATASIZE_8BIT;
		SpiHandle.Init.FirstBit          = SPI_FIRSTBIT_MSB;
		SpiHandle.Init.TIMode            = SPI_TIMODE_DISABLE;
		SpiHandle.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
		SpiHandle.Init.CRCPolynomial     = 7;
		SpiHandle.Init.CRCLength         = SPI_CRC_LENGTH_8BIT;
		SpiHandle.Init.NSS               = SPI_NSS_SOFT;
		SpiHandle.Init.NSSPMode          = SPI_NSS_PULSE_DISABLE;

		SpiHandle.Init.Mode = SPI_MODE_MASTER;

		if(HAL_SPI_Init(&SpiHandle) != HAL_OK)
		{
		// Initialization Error
			Error_Handler();
		}
	}

	void ENABLE_SPI_byPOLLING (void){
	  spi_type = TRANSFER_BY_POLL;
	  HAL_NVIC_DisableIRQ(SPIx_IRQn);
	}


	void ENABLE_SPI_byIRQ (void){
	  spi_type = TRANSFER_BY_IRQ;
	  HAL_NVIC_EnableIRQ(SPIx_IRQn);
	}


void test_spi(void){

	uint16_t ERROR;

	if(HAL_SPI_TransmitReceive_IT(&SpiHandle, (uint8_t*)aTxBuffer, (uint8_t *)aRxBuffer, BUFFERSIZE) != HAL_OK)
	  {
	    //error
		Error_Handler();
	  }

	//espera finalizacion de transmision
	while(status_spi_transfer != TRANSFER_COMPLETE){};

	ERROR = Buffercmp((uint8_t*)aTxBuffer, (uint8_t*)aRxBuffer, BUFFERSIZE);
	if(!ERROR){
		//recepcion correcta
		__NOP();
	}else{
		//error de recepcion
		__NOP();
		__NOP();
	}
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
  /* Turn LED on: Transfer in transmission/reception process is correct */
 // BSP_LED_On(LED2);
	status_spi_transfer = TRANSFER_COMPLETE;
}

void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
	status_spi_transfer = TRANSFER_ERROR;
}

static uint16_t Buffercmp(uint8_t *pBuffer1, uint8_t *pBuffer2, uint16_t BufferLength)
{
 uint16_t BUFFER_SIZE;

 BUFFER_SIZE = BufferLength;

  while (BufferLength--)
  {
	if ((*pBuffer1) != *pBuffer2)
	{
	  return (BUFFER_SIZE-BufferLength);
	}
	pBuffer1++;
	pBuffer2++;
  }

  return 0;
}


uint8_t SpiTxRxByte(uint8_t byte, uint8_t* status_ptr)
{
	uint8_t aRxBuffer[1];
	uint8_t aTxBuffer[1];
	uint16_t sizeBUFF;

	aTxBuffer[0]= byte;
	sizeBUFF = 1;

	if(spi_type == TRANSFER_BY_IRQ){
		//TX BY IRQ

		status_spi_transfer = TRANSFER_WAIT;
		if(HAL_SPI_TransmitReceive_IT(&SpiHandle, (uint8_t*)aTxBuffer, (uint8_t *)aRxBuffer, sizeBUFF) == HAL_OK){
			//espera finalizacion de transmision
			*status_ptr =errorNONE;
			while(status_spi_transfer == TRANSFER_WAIT){
				if(status_spi_transfer ==TRANSFER_ERROR){
					*status_ptr =errorFAIL;
					break;
				}
			};
		}else{
			*status_ptr =errorFAIL;
		}

	}else{
		//TX BY POLLING

		//if(HAL_SPI_TransmitReceive(&SpiHandle, (uint8_t*)aTxBuffer, (uint8_t *)aRxBuffer, sizeBUFF, 5000) == HAL_OK){
		if(HAL_SPI_TransmitReceive(&SpiHandle, (uint8_t*)aTxBuffer, (uint8_t *)aRxBuffer, sizeBUFF, 40000) == HAL_OK){
			*status_ptr =errorNONE;
			//*status_ptr = 0;
		}else{
			*status_ptr =errorFAIL;
			//*status_ptr = 9;
		}
	}
	return(aRxBuffer[0]);
	}




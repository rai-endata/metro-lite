/*
 * spi.h
 *
 *  Created on: 26/4/2018
 *      Author: Rai
 */

#ifndef _SPI_H_
#define _SPI_H_

#include "stm32f091xc.h"
#include "main.h"

enum {
	TRANSFER_WAIT,
	TRANSFER_COMPLETE,
	TRANSFER_ERROR
};


typedef enum
{
	TRANSFER_BY_POLL,
	TRANSFER_BY_IRQ
}spiMODE_en;


typedef enum ERROR_spiTxRx
{
	errorNONE=0,
	errorFAIL=1
}error_SPI_TxRx;

extern spiMODE_en spi_type;
extern  uint8_t status_spi_transfer;

#define SPIx                             SPI2
#define SPIx_CLK_ENABLE()                __HAL_RCC_SPI2_CLK_ENABLE()
#define SPIx_SCK_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOB_CLK_ENABLE()
#define SPIx_MISO_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOC_CLK_ENABLE()
#define SPIx_MOSI_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOC_CLK_ENABLE()

#define SPIx_FORCE_RESET()               __HAL_RCC_SPI2_FORCE_RESET()
#define SPIx_RELEASE_RESET()             __HAL_RCC_SPI2_RELEASE_RESET()

// Definition for SPIx Pins
#define SPIx_SCK_PIN                     GPIO_PIN_10
#define SPIx_SCK_GPIO_PORT               GPIOB
#define SPIx_SCK_AF                      GPIO_AF5_SPI2
#define SPIx_MISO_PIN                    GPIO_PIN_2
#define SPIx_MISO_GPIO_PORT              GPIOC
#define SPIx_MISO_AF                     GPIO_AF5_SPI2
#define SPIx_MOSI_PIN                    GPIO_PIN_3
#define SPIx_MOSI_GPIO_PORT              GPIOC
#define SPIx_MOSI_AF                     GPIO_AF5_SPI2


// Definition for SPIx's NVIC
#define SPIx_IRQn                        SPI2_IRQn
#define SPIx_IRQHandler                  SPI2_IRQHandler

// Size of buffer
#define BUFFERSIZE                       (COUNTOF(aTxBuffer) - 1)

// Exported macro ------------------------------------------------------------
#define COUNTOF(__BUFFER__)   (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))


void ENABLE_SPI_byIRQ (void);
void ENABLE_SPI_byPOLLING (void);

extern void MX_SPIx_Init(void);
extern uint8_t SpiTxRxByte(uint8_t byte, uint8_t* status);
extern void test_spi(void);

extern SPI_HandleTypeDef SpiHandle;

#endif /* SPI_SPI_H_ */

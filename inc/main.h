/**
  ******************************************************************************
  * @file    UART/UART_TwoBoards_ComIT/Inc/main.h 
  * @author  MCD Application Team
  * @version V1.2.4
  * @date    13-November-2015 
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"
#include "stm32f0xx_hal_uart.h"
#include "stm32f091xc.h"
#include "stm32f0xx_hal_rtc.h"
#include "Definicion de tipos.h"

/* ESTRUCTURAS */
/***************/

// BANDERAS GENERALES 1
    typedef union {
      byte Byte;
      struct{
        byte hab_Login        :1;       // Indica si esta o no habilitado el Login al sistema
        byte dispachOFF       :1;       // Indica si se encuentra o no en despacho suspendido
        byte logueado         :1;       // Indica si esta logueado o no
        byte reset            :1;       // Indica que se dio la orden de resetear el equipo
        byte turnOFF          :1;       // Indica que se dio la orden de apagar el equipo
        byte powerOFF         :1;       // Indica que el equipo se encuentra apagado
        byte     :1;
        byte firstBoot        :1;       // Indica que ocurrio un PRIMER ENCENDIDO
      }Bits;
    }mainFLAGS1;


typedef enum
{
  LED3 = 0,
  LED4 = 1
}Led_Type;

/*
typedef enum
{
  BUTTON_KEY = 0,
}Button_Type;

typedef enum
{
  BUTTON_MODE_GPIO = 0,
  BUTTON_MODE_EXTI = 1
}ButtonMode_Type;
*/

/* VARIABLES */
/*************/
  extern byte MOTIVO_RESET;

// TIPO DE DATOS DE MOTIVO DE RESET
 // El valor 0xFF en EEPROM (MOTIVO = 0xF0) es solo para el PRIMER ENCENDIDO del equipo,
 // luego de haberle cargado el software
 typedef enum{
   RST_POR,                          // Power On Reset
   RST_COP,                          // COP Watchdog Reset
   RST_XTAL_FAIL,                    // Crystal Monitor Failure Reset
   RST_MANUAL,                       // Manual Reset
   RST_TURN_ON,                      // Reset porque encendio el Equipo
   RST_PROG,                         // Programacion
   RST_TEST,                         // Modo Test
   RST_TURN_ON_byASIENTO,            // Encendido por sensor de asiento
   RST_REMOTO,                        // Reset desde la Central
   RST_DESBLOQUEO_EQUIPO
 }tRESET_CAUSE;

 typedef enum {
	NO_HUBO_CORTE,
	HUBO_CORTE
 };

	#define TIPO_DE_EQUIPO		0x01
  // BANDERAS GENERALES 1
  extern mainFLAGS1             _VISOR_F1;
    #define VISOR_F1            _VISOR_F1.Byte
    #define VISOR_habLogin      _VISOR_F1.Bits.hab_Login
    #define VISOR_despachoOFF   _VISOR_F1.Bits.dispachOFF
    #define VISOR_logueado      _VISOR_F1.Bits.logueado
    #define VISOR_reset         _VISOR_F1.Bits.reset
    #define VISOR_turnOFF       _VISOR_F1.Bits.turnOFF
    #define VISOR_powerOFF      _VISOR_F1.Bits.powerOFF
    #define VISOR_firstBoot     _VISOR_F1.Bits.firstBoot

    // MACROS
    #define desloguearVISOR       (VISOR_logueado = 0)
    #define loguearVISOR          (VISOR_logueado = 1)
    #define resetVISOR            (VISOR_reset = 1)
    #define chkVISOR_logueado     ((const) VISOR_logueado)
    #define chkVISOR_habLogin     ((const) VISOR_habLogin)
    #define chkVISOR_despachoOFF  ((const) VISOR_despachoOFF)
    #define turnOffVISOR          (VISOR_turnOFF = 1)
    #define chkVISOR_powerOFF     ((const)VISOR_powerOFF)
    #define chkVISOR_firstBoot    ((const)VISOR_firstBoot)


 extern  byte* ptr1;
 extern  byte* ptr2;



/* Exported constants --------------------------------------------------------*/
/* User can use this section to tailor USARTx/UARTx instance used and associated 
   resources */
/*
#define B1_Pin 				GPIO_PIN_13
#define B1_GPIO_Port		GPIOC
*/

#define USART_TX_Pin 		GPIO_PIN_2
#define USART_TX_GPIO_Port	GPIOA
#define USART_RX_Pin 		GPIO_PIN_3
#define USART_RX_GPIO_Port	GPIOA

//#define LD2_Pin				GPIO_PIN_5
//#define LD2_GPIO_Port		GPIOA

  #define TMS_Pin				GPIO_PIN_13
#define TMS_GPIO_Port		GPIOA
#define TCK_Pin				GPIO_PIN_14
#define TCK_GPIO_Port		GPIOA


//#define USARTx                           USART3
//#define USARTx_CLK_ENABLE()              __HAL_RCC_USART3_CLK_ENABLE();
//#define USARTx_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
//#define USARTx_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()

//#define USARTx_FORCE_RESET()             __HAL_RCC_USART3_FORCE_RESET()
//#define USARTx_RELEASE_RESET()           __HAL_RCC_USART3_RELEASE_RESET()

//#define USARTx_TX_PIN                    GPIO_PIN_10
//#define USARTx_TX_GPIO_PORT              GPIOB
//#define USARTx_TX_AF                     GPIO_AF7_USART3
//#define USARTx_RX_PIN                    GPIO_PIN_11
//#define USARTx_RX_GPIO_PORT              GPIOB
//#define USARTx_RX_AF                     GPIO_AF7_USART3
#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#define MANEJO_DE_ERRORES() _Error_Handler(__FILE__, __LINE__)

//DEFINICION DE OSCOLADOR INTERNO O EXTERNO
#define RTC_CLOCK_SOURCE_LSE
//#define RTC_CLOCK_SOURCE_LSI

/* Definition for USART1 clock resources */
//#define USART1                            USART1
#define USART1_CLK_ENABLE()              __HAL_RCC_USART1_CLK_ENABLE();
#define USART1_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USART1_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USART1_FORCE_RESET()             __HAL_RCC_USART1_FORCE_RESET()
#define USART1_RELEASE_RESET()           __HAL_RCC_USART1_RELEASE_RESET()
/* Definition for USARTx Pins */
#define USART1_TX_PIN                    GPIO_PIN_9
#define USART1_TX_GPIO_PORT              GPIOA
#define USART1_TX_AF                     GPIO_AF1_USART1
#define USART1_RX_PIN                    GPIO_PIN_10
#define USART1_RX_GPIO_PORT              GPIOA
#define USART1_RX_AF                     GPIO_AF1_USART1

/* Definition for USARTx's NVIC */
#define USART1_IRQn                      USART1_IRQn
#define USART1_IRQHandler                USART1_IRQHandler


/* Definition for USART3 clock resources */
//#define USART3                           USART3
#define USART7_CLK_ENABLE()              __HAL_RCC_USART7_CLK_ENABLE();
#define USART7_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOC_CLK_ENABLE()
#define USART7_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOC_CLK_ENABLE()
#define USART7_FORCE_RESET()             __HAL_RCC_USART7_FORCE_RESET()
#define USART7_RELEASE_RESET()           __HAL_RCC_USART7_RELEASE_RESET()
/* Definition for USARTx Pins */
#define USART7_TX_PIN                    GPIO_PIN_6
#define USART7_TX_GPIO_PORT              GPIOC
//#define USART7_TX_AF                     GPIO_AF7_USART7
#define USART7_RX_PIN                    GPIO_PIN_7
#define USART7_RX_GPIO_PORT              GPIOC
//#define USART7_RX_AF                     GPIO_AF7_USART7
/* Definition for USARTx's NVIC */
//#define USART7_IRQn                      USART7_IRQn
#define USART7_IRQHandler                USART7_IRQHandler


/* Size of Transmission buffer */
#define TXBUFFERSIZE                      (COUNTOF(aTxBuffer) - 1)
/* Size of Reception buffer */
#define RXBUFFERSIZE                      TXBUFFERSIZE

/* Exported macro ------------------------------------------------------------*/
#define COUNTOF(__BUFFER__)   (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))

/* Exported functions ------------------------------------------------------- */
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart7;

extern RTC_HandleTypeDef  hrtc;

//#define BUTTONn                                1

/**
 * @brief Wakeup push-button
 */
/*
#define KEY_BUTTON_PIN                         GPIO_PIN_0
#define KEY_BUTTON_GPIO_PORT                   GPIOA
#define KEY_BUTTON_GPIO_CLK_ENABLE()           __GPIOA_CLK_ENABLE()
#define KEY_BUTTON_GPIO_CLK_DISABLE()          __GPIOA_CLK_DISABLE()
#define KEY_BUTTON_EXTI_IRQn                   EXTI0_IRQn

#define BUTTONx_GPIO_CLK_ENABLE(__INDEX__)     do{if((__INDEX__) == 0) KEY_BUTTON_GPIO_CLK_ENABLE(); \
                                                 }while(0)
#define BUTTONx_GPIO_CLK_DISABLE(__INDEX__)    do{if((__INDEX__) == 0) KEY_BUTTON_GPIO_CLK_DISABLE(); \
                                                 }while(0)
*/

extern void _Error_Handler(char * file, int line);
extern void USART_Ini(UART_HandleTypeDef	*huart, uint16_t baudrate);
extern void checkUART_ERROR (void);
extern void MX_RTC_Init(void);

extern uint32_t cntIC_anterior;
extern byte estaOCUPADO(void );


#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

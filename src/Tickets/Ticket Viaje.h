#ifndef __IMPRESORA__
 #define __IMPRESORA__

#include "Common Ticket.h"
//#include "stm32f0xx_hal_gpio.h"
//#include "stm32f091xc.h"

/*
//define PIN PULSADOR_IMPRESION
#define PULSADOR_IMPRESION_PIN                 		   GPIO_PIN_11
#define PULSADOR_IMPRESION_GPIO_PORT                   GPIOB
#define PULSADOR_IMPRESION_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOB_CLK_ENABLE()
#define PULSADOR_IMPRESION_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOB_CLK_DISABLE()
#define PULSADOR_IMPRESION_EXTI_LINE                   GPIO_PIN_11
#define PULSADOR_IMPRESION_EXTI_IRQn                   EXTI4_15_IRQn
*/

extern uint32_t nroTICKET;


#define TO_PRINTING_VIAJE		3

#define NO_PRINTING			(to_print_cnt==0)

typedef enum
{
	PULSADOR_IMPRESION = 0,
  // Alias
	PULSADOR_IMPRESION_KEY  = PULSADOR_IMPRESION
} Button_TypeDef;

typedef enum
{
  PULSADOR_IMPRESION_MODE_GPIO = 0,
  PULSADOR_IMPRESION_MODE_EXTI = 1
} ButtonMode_TypeDef;


extern void print_ticket_viaje(void);
extern void ini_pulsador_impresion (void);
extern void inc_nroTICKET (void);


#endif

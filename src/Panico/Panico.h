#ifndef __PANICO_H
#define __PANICO_H
#include "Definicion de tipos.h"

#define PANICO_PIN                         GPIO_PIN_2
#define PANICO_GPIO_PORT                   GPIOD
#define PANICO_GPIO_CLK_ENABLE()           __GPIOD_CLK_ENABLE()
#define PANICO_GPIO_CLK_DISABLE()          __GPIOD_CLK_DISABLE()
#define PANICO_EXTI_IRQn	                EXTI2_3_IRQn




	typedef union {
	      byte Byte;
	      struct{
			  byte panicoDETECTED		:1;
			  byte :1;
			  byte :1;
			  byte :1;
			  byte :1;
			  byte :1;
			  byte :1;
			  byte :1;
	      }Bits;
	 }tPANICO_FLAGS;


extern tPANICO_FLAGS			_panicoFLAGS;

#define panicoDETECTADO		_panicoFLAGS.Bits.panicoDETECTED



extern void Panico_Ini(void);
extern void check_pressPANICO(void);
extern void check_pressPANICO(void);

//extern GPIO_PinState Buetooth_Status_GetState(void);

#endif

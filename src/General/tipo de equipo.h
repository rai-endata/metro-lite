/*
 * tipo de equipo.h
 *
 *  Created on: 23/11/2018
 *      Author: Rai
 */

#ifndef TIPO_DE_EQUIPO_H_
 #define TIPO_DE_EQUIPO_H_

#include "Definicion de tipos.h"

#define TIPO_EQUIPO_PIN0                       GPIO_PIN_6
#define TIPO_EQUIPO_PIN0_GPIO_PORT             GPIOB
#define TIPO_EQUIPO_PIN0_GPIO_CLK_ENABLE()     __GPIOB_CLK_ENABLE()
#define TIPO_EQUIPO_PIN0_GPIO_CLK_DISABLE()    __GPIOB_CLK_DISABLE()
#define TIPO_EQUIPO_PIN0_EXTI_IRQn 		       EXTI4_15_IRQn

#define TIPO_EQUIPO_PIN1                       GPIO_PIN_7
#define TIPO_EQUIPO_PIN1_GPIO_PORT             GPIOB
#define TIPO_EQUIPO_PIN1_GPIO_CLK_ENABLE()     __GPIOB_CLK_ENABLE()
#define TIPO_EQUIPO_PIN1_GPIO_CLK_DISABLE()    __GPIOB_CLK_DISABLE()
#define TIPO_EQUIPO_PIN1_EXTI_IRQn 		       EXTI4_15_IRQn


	typedef union {
	      byte Byte;
	      struct{
			  byte pin7		:1;
			  byte pin6		:1;
			  byte :1;
			  byte :1;
			  byte :1;
			  byte :1;
			  byte :1;
			  byte :1;
	      }Bits;
	 }tTIPO_EQUIPO_FLAGS;

		// TIPO DE EQUIPO
		typedef enum{
			METRO_BLUE,
			METRO_LITE,
			EQUIPO_NO_DEFINIDO
		}tEQUIPO;

extern tTIPO_EQUIPO_FLAGS			_tipoEquipoFLAGS;
extern tEQUIPO tipo_de_equipo;

#define pin0_tipoEQUIPO		_tipoEquipoFLAGS.Bits.pin6
#define pin1_tipoEQUIPO		_tipoEquipoFLAGS.Bits.pin7


#define EQUIPO_METRO_BLUE					(tipo_de_equipo == METRO_BLUE)
#define EQUIPO_METRO_LITE					(tipo_de_equipo == METRO_LITE)
#define EQUIPO_METRO_LITE_RELOJ_INTERNO		((tipo_de_equipo == METRO_LITE) && RELOJ_INTERNO)
#define EQUIPO_METRO_LITE_RELOJ_BANDERITA	((tipo_de_equipo == METRO_LITE) && RELOJ_BANDERITA)

#define RELOJ_INTERNO       (TIPO_RELOJ == INTERNO)
#define RELOJ_EXTERNO       (RELOJ_BANDERITA || RELOJ_PROTOCOLO)
#define RELOJ_BANDERITA     ((TIPO_RELOJ == BANDERITA_LIBRE_ALTO) || (TIPO_RELOJ == BANDERITA_OCUPADO_ALTO))
#define RELOJ_PROTOCOLO     (TIPO_RELOJ == PROTOCOLO)

#define RELOJ_BANDERITA_conPulsos     (RELOJ_BANDERITA && (chkSIN_PULSOS == 0))

void set_tipo_de_equipo (void);

//extern GPIO_PinState Buetooth_Status_GetState(void);



#endif /* TIPO_DE_EQUIPO_H_ */

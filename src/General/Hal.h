/* File <HAL.h> */

#ifndef _HAL_
	#define _HAL_

	#include "stm32f0xx_hal.h"



	/* WATCHDOG */
	  /************/
	#define  pin_WATCHDOG               GPIO_PIN_13
	#define  port_WATCHDOG              GPIOG


	 //Odometro



   // #define PULSE_ACCUM_CNT			__HAL_TIM_GET_COUNTER(&htim2)
	#define setPULSE_ACCUM_CNT(__HANDLE__, __COUNTER__) 	((__HANDLE__)->Instance->CNT = (__COUNTER__))
	#define getPULSE_ACCUM_CNT(__HANDLE__) 					((__HANDLE__)->Instance->CNT)




 //#define  PULSOS_x_KM				1000   //uint16_t
 //#define  CARRERA_BANDERA			 100   //uint16_t









#endif

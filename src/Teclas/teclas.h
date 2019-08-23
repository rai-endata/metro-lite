/*
 * teclas.h
 *
 *  Created on: 26/3/2018
 *      Author: Rai
 */

#ifndef _TECLAS_H_
	#define _TECLAS_H_

	#define TECLA1_PIN						GPIO_PIN_5
	#define TECLA1_PORT						GPIOA
	#define TECLA1_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOA_CLK_ENABLE()
	#define TECLA1_GPIO_CLK_DISABLE()       __HAL_RCC_GPIOA_CLK_DISABLE()
	#define TECLA1_EXTI_LINE                 GPIO_PIN_5
	#define TECLA1_EXTI_IRQn                 EXTI4_15_IRQn

	#define TECLA2_PIN						GPIO_PIN_11
	#define TECLA2_PORT						GPIOB
	#define TECLA2_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOB_CLK_ENABLE()
	#define TECLA2_GPIO_CLK_DISABLE()       __HAL_RCC_GPIOB_CLK_DISABLE()
	#define TECLA2_EXTI_LINE                 GPIO_PIN_11
	#define TECLA2_EXTI_IRQn                 EXTI4_15_IRQn

	#define	TIEMPO_TECLA_LARGA			200		//2 seg (200*10mseg = 2000 mseg = 2 seg)


	typedef union{

		byte Byte;
		struct{
		  byte contarTiempoPulsadoT1   :1;
		  byte contarTiempoPulsadoT2   :1;
		  byte pressTC1				   :1;
		  byte pressTC2				   :1;
		  byte pressTL1				   :1;
		  byte pressTL2				   :1;
		  byte :1;
		  byte :1;     // Pedido de actualizacion
		}Bits;
	}t_teclasFLAGS;

	  extern t_teclasFLAGS                       	_teclasFLAGS;

		#define teclasFLAGS						_teclasFLAGS.Byte
		#define contarTiempoPulsadoT1_F 		    _teclasFLAGS.Bits.contarTiempoPulsadoT1
		#define contarTiempoPulsadoT2_F 		    _teclasFLAGS.Bits.contarTiempoPulsadoT2
		#define pressTECLA1_CORTA					_teclasFLAGS.Bits.pressTC1
		#define pressTECLA2_CORTA					_teclasFLAGS.Bits.pressTC2
		#define pressTECLA1_LARGA					_teclasFLAGS.Bits.pressTL1
		#define pressTECLA2_LARGA					_teclasFLAGS.Bits.pressTL2

	  extern byte indexMenu_IniTurno;

	void  ini_tecla1(void);
	void  ini_tecla2(void);

	extern void teclaT1_CORTA(void);
	extern void teclaT1_LARGA(void);
	extern void teclaT2_CORTA(void);
	extern void teclaT2_LARGA(void);


	extern void startCONTADOR_tiempoPULSADO_T1(void);
	extern void stopCONTADOR_tiempoPULSADO_T1(void);
	extern void CONTADOR_tiempoPULSADO_timer_T1 (void);

	extern void startCONTADOR_tiempoPULSADO_T2(void);
	extern void stopCONTADOR_tiempoPULSADO_T2(void);
	extern void CONTADOR_tiempoPULSADO_timer_T2 (void);

	extern void check_pressTECLA(void);
	extern void check_pressTECLA_CORTA1(void);
	extern void check_pressTECLA_CORTA2(void);
	extern void check_pressT1(void);
	extern void check_pressT2(void);

	extern uint16_t CONTADOR_tiempoPULSADO_T1;
	extern uint16_t CONTADOR_tiempoPULSADO_T2;

#endif /* _TECLAS_H_ */

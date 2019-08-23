/*
 * display-7seg.h
 *
 *  Created on: 22/3/2018
 *      Author: Rai
 */

#ifndef _DISPLAY_7SEG_H_
#define _DISPLAY_7SEG_H_

#include "stdint.h"

/*
//define PIN CATODO DIGITO 0
#define PIN_CATODO_DIGITO_0                  GPIO_PIN_12
#define PUERTO_CATODO_DIGITO_0               GPIOB
#define CATODO_DIGITO_0_CLK_ENABLE()       __GPIOB_CLK_ENABLE()
#define CATODO_DIGITO_0_CLK_DISABLE()      __GPIOB_CLK_DISABLE()

//define PIN CATODO DIGITO 1
#define PIN_CATODO_DIGITO_1                  GPIO_PIN_13
#define PUERTO_CATODO_DIGITO_1               GPIOB
#define CATODO_DIGITO_1_CLK_ENABLE()       __GPIOB_CLK_ENABLE()
#define CATODO_DIGITO_1_CLK_DISABLE()      __GPIOB_CLK_DISABLE()

//define PIN CATODO DIGITO 2
#define PIN_CATODO_DIGITO_2                  GPIO_PIN_14
#define PUERTO_CATODO_DIGITO_2               GPIOB
#define CATODO_DIGITO_2_CLK_ENABLE()       __GPIOB_CLK_ENABLE()
#define CATODO_DIGITO_2_CLK_DISABLE()      __GPIOB_CLK_DISABLE()

//define PIN CATODO DIGITO 3
#define PIN_CATODO_DIGITO_3                  GPIO_PIN_8
#define PUERTO_CATODO_DIGITO_3               GPIOC
#define CATODO_DIGITO_3_CLK_ENABLE()       __GPIOC_CLK_ENABLE()
#define CATODO_DIGITO_3_CLK_DISABLE()      __GPIOC_CLK_DISABLE()
*/

//DEFINICION DE ERRORES
//***********************
/*
 * Err01: (error_eepromDATA)	No esta programado los parametros de reloj o hay error de checksum
 *
 *
 *
 *
 */






//define PIN CATODO DIGITO 0
#define PIN_CATODO_DIGITO_0                  GPIO_PIN_1
#define PUERTO_CATODO_DIGITO_0               GPIOB
#define CATODO_DIGITO_0_CLK_ENABLE()       __GPIOB_CLK_ENABLE()
#define CATODO_DIGITO_0_CLK_DISABLE()      __GPIOB_CLK_DISABLE()

//define PIN CATODO DIGITO 1
#define PIN_CATODO_DIGITO_1                  GPIO_PIN_0
#define PUERTO_CATODO_DIGITO_1               GPIOB
#define CATODO_DIGITO_1_CLK_ENABLE()       __GPIOB_CLK_ENABLE()
#define CATODO_DIGITO_1_CLK_DISABLE()      __GPIOB_CLK_DISABLE()

//define PIN CATODO DIGITO 2
#define PIN_CATODO_DIGITO_2                  GPIO_PIN_7
#define PUERTO_CATODO_DIGITO_2               GPIOA
#define CATODO_DIGITO_2_CLK_ENABLE()       __GPIOA_CLK_ENABLE()
#define CATODO_DIGITO_2_CLK_DISABLE()      __GPIOA_CLK_DISABLE()

//define PIN CATODO DIGITO 3
#define PIN_CATODO_DIGITO_3                  GPIO_PIN_6
#define PUERTO_CATODO_DIGITO_3               GPIOA
#define CATODO_DIGITO_3_CLK_ENABLE()       __GPIOA_CLK_ENABLE()
#define CATODO_DIGITO_3_CLK_DISABLE()      __GPIOA_CLK_DISABLE()

//define PIN CATODO DIGITO 4
#define PIN_CATODO_DIGITO_4                  GPIO_PIN_9
#define PUERTO_CATODO_DIGITO_4               GPIOC
#define CATODO_DIGITO_4_CLK_ENABLE()       __GPIOC_CLK_ENABLE()
#define CATODO_DIGITO_4_CLK_DISABLE()      __GPIOC_CLK_DISABLE()

//define PIN CATODO DIGITO 5
#define PIN_CATODO_DIGITO_5                  GPIO_PIN_8
#define PUERTO_CATODO_DIGITO_5               GPIOA
#define CATODO_DIGITO_5_CLK_ENABLE()       __GPIOA_CLK_ENABLE()
#define CATODO_DIGITO_5_CLK_DISABLE()      __GPIOA_CLK_DISABLE()


//define PIN SEGMENTO A
#define PIN_SEGMENTO_A                  GPIO_PIN_12
#define PUERTO_SEGMENTO_A               GPIOB
#define SEGMENTO_A_CLK_ENABLE()       __GPIOB_CLK_ENABLE()
#define SEGMENTO_A_CLK_DISABLE()      __GPIOB_CLK_DISABLE()

//define PIN SEGMENTO B
#define PIN_SEGMENTO_B                  GPIO_PIN_2
#define PUERTO_SEGMENTO_B               GPIOB
#define SEGMENTO_B_CLK_ENABLE()       __GPIOB_CLK_ENABLE()
#define SEGMENTO_B_CLK_DISABLE()      __GPIOB_CLK_DISABLE()

//define PIN SEGMENTO C
#define PIN_SEGMENTO_C                  GPIO_PIN_15
#define PUERTO_SEGMENTO_C               GPIOB
#define SEGMENTO_C_CLK_ENABLE()       __GPIOB_CLK_ENABLE()
#define SEGMENTO_C_CLK_DISABLE()      __GPIOB_CLK_DISABLE()

//define PIN SEGMENTO D
#define PIN_SEGMENTO_D                  GPIO_PIN_11
#define PUERTO_SEGMENTO_D               GPIOA
#define SEGMENTO_D_CLK_ENABLE()       __GPIOA_CLK_ENABLE()
#define SEGMENTO_D_CLK_DISABLE()      __GPIOA_CLK_DISABLE()

//define PIN SEGMENTO E
#define PIN_SEGMENTO_E                 GPIO_PIN_12
#define PUERTO_SEGMENTO_E              GPIOA
#define SEGMENTO_E_CLK_ENABLE()       __GPIOA_CLK_ENABLE()
#define SEGMENTO_E_CLK_DISABLE()      __GPIOA_CLK_DISABLE()

//define PIN SEGMENTO F
#define PIN_SEGMENTO_F              	GPIO_PIN_8
#define PUERTO_SEGMENTO_F               GPIOC
#define SEGMENTO_F_CLK_ENABLE()       __GPIOC_CLK_ENABLE()
#define SEGMENTO_F_CLK_DISABLE()      __GPIOC_CLK_DISABLE()

//define PIN SEGMENTO G
#define PIN_SEGMENTO_G                  GPIO_PIN_14
#define PUERTO_SEGMENTO_G               GPIOB
#define SEGMENTO_G_CLK_ENABLE()       __GPIOB_CLK_ENABLE()
#define SEGMENTO_G_CLK_DISABLE()      __GPIOB_CLK_DISABLE()

//define PIN SEGMENTO PUNTO
#define PIN_SEGMENTO_PUNTO                  GPIO_PIN_13
#define PUERTO_SEGMENTO_PUNTO               GPIOB
#define SEGMENTO_PUNTO_CLK_ENABLE()       __GPIOB_CLK_ENABLE()
#define SEGMENTO_PUNTO_CLK_DISABLE()      __GPIOB_CLK_DISABLE()


	typedef union{

		uint8_t Byte;
		struct{
			uint8_t mostrar_ini_I		:1;
			uint8_t mostrar_conF_I		:1;
			uint8_t mostrar_L_T			:1;
			uint8_t mostrar_P_T			:1;
			uint8_t mostrar_t_T			:1;
			uint8_t mostrar_numero_T	:1;
			uint8_t mostrar_turn_I		:1;
			uint8_t :1;
		}Bits;
	}t_displayFLAGS;

	  extern t_displayFLAGS _displayFLAGS;

	      #define displayFLAGS			 _displayFLAGS.Byte
	      #define mostrar_t_dsplyT 	     _displayFLAGS.Bits.mostrar_t_T
		  #define mostrar_L_dsplyT 	     _displayFLAGS.Bits.mostrar_L_T
		  #define mostrar_P_dsplyT	     _displayFLAGS.Bits.mostrar_P_T
		  #define mostrar_numero_dsplyT  _displayFLAGS.Bits.mostrar_numero_T

	      #define mostrar_ini_dsplyIMPORTE 		_displayFLAGS.Bits.mostrar_ini_I
		  #define mostrar_conF_dsplyIMPORTE		_displayFLAGS.Bits.mostrar_conF_I
		  #define mostrar_turn_dsplyIMPORTE		_displayFLAGS.Bits.mostrar_turn_I


extern void ini_display_7seg (void);
extern void print_display (void);
extern void print_display_off (uint8_t digito);
extern void update_valor_display (uint32_t valor);
extern void to_display_on (void );
extern void off_display (void );
extern void on_display_importe (void );
extern void on_display_tarifa (void );
extern void on_display_all (void );
extern void toggle_display (void );
extern void to_toggle_display (void );
extern void to_toggleTARIFA_display (void );
extern void update_valor_tarifa (uint8_t valor);
extern void display_off_toggle (void );
extern void display_set_toggle (void );


extern uint8_t status_display;
	#define DISPLAY_NORMAL		0x00
	#define DISPLAY_CHK			0xff

extern void TEST_DISPLAY_7SEG(void);
extern void TST_segmentos_display_7seg (void);
extern void to_test_display (void );
extern void setTEST_DISPLAY(void);

extern uint8_t cnt_1;
extern uint8_t cnt_2;
extern uint8_t cnt_3;
extern uint8_t statusDISPLAY;;

#endif /* DISPLAY_7SEG_DISPLAY_7SEG_H_ */

/*
 * buzzer.h
 *
 *  Created on: 25/6/2018
 *      Author: Rai
 */

#ifndef BUZZER_BUZZER_H_
#define BUZZER_BUZZER_H_

#include "stdint.h"

/*
#define BUZZER_TECLA_T1_CORTA				10
#define BUZZER_TECLA_T2_CORTA				10
#define BUZZER_HABILITA_PASE_LIBRE			20
#define VEHICULO_EN_MOVIMIENTO				60
*/


extern uint8_t tabRING_chgRELOJ[8];
extern uint8_t tabRING_teclaCORTA[4];
extern uint8_t tabRING_teclaLARGA[8];
extern uint8_t tabRING_check[8];
extern uint8_t tabRING_progOK[4];
extern uint8_t tabRING_progERROR[8];

/*
#define BUZZER_TECLA_T1_CORTA				&tabRING_chgRELOJ
#define BUZZER_TECLA_T2_CORTA				&tabRING_chgRELOJ
#define BUZZER_HABILITA_PASE_LIBRE			&tabRING_chgRELOJ
#define VEHICULO_EN_MOVIMIENTO				&tabRING_chgRELOJ
*/

#define BEEP_TECLA_CORTA					&tabRING_teclaCORTA
#define BEEP_TECLA_LARGA					&tabRING_teclaLARGA


#define BEEP_HABILITA_PASE_LIBRE			&tabRING_teclaCORTA
#define BEEP_VEHICULO_EN_MOVIMIENTO			&tabRING_teclaCORTA

#define BEEP_PROGRAMCION_OK						&tabRING_progOK
#define BEEP_PROGRAMCION_ERROR					&tabRING_progERROR



#define BUZZER_PIN                GPIO_PIN_4
#define BUZZER_PORT               GPIOA
#define BUZZER_CLK_ENABLE()       __GPIOA_CLK_ENABLE()
#define BUZZER_CLK_DISABLE()      __GPIOA_CLK_DISABLE()

extern void buzzer_ini(void);
extern void BuzzerPIN_On(void);
extern void BuzzerPIN_Off(void);
extern void buzzer_TIMEOUT (void);
extern void buzzer_TIMEOUT_toOFF (void);

extern void Buzzer_On(uint8_t* tabRING);
extern void pauseTONE_TIMEOUT (void);


#endif /* BUZZER_BUZZER_H_ */

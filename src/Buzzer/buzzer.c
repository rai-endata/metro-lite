/*
 * buzzer.c
 *
 *  Created on: 25/6/2018
 *      Author: Rai
 */

#include "buzzer.h"
#include "stm32f0xx_hal_gpio.h"
#include "- metroBLUE Config -.h"

uint8_t buzzer_to_cnt;
uint8_t cntBits_buzzer;
uint8_t buzzer_to_pause;
uint8_t buzzer_to_off;
uint8_t n_pulsos_buzzer;
uint8_t pause_tone,PAUSE_TONE;
uint8_t indexRING;
uint8_t* ptr_tabRING_AUX;

						//tON,  tOFF,  n, tTONE
uint8_t tabRING_check[] = {4,   4,    10, 80,
						   10,  10,   2, 0xff};


						      //tON,  tOFF,  n, tPAUSE
uint8_t tabRING_progERROR[] = {10,    4,     1, 10,
						 	   50,    1,     1, 0xff};

uint8_t tabRING_progOK[]     =  { 6,   6,     1, 0xff};
uint8_t tabRING_teclaCORTA[] =  {10,   1,     1, 0xff};


uint8_t tabRING_teclaLARGA[] = {10,    4,     1, 10,
						 	    10,    1,     1, 0xff};


#define DELAY_tOFF 	3
#define N_PULSOS    3

void buzzer_ini(void)
	{
		GPIO_InitTypeDef  GPIO_InitStruct;

		/* Enable the BANDERA OUT Clock */
		BUZZER_CLK_ENABLE();

		/* Configure the GPIO_LED pin */
		GPIO_InitStruct.Pin = BUZZER_PIN;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

		HAL_GPIO_Init(BUZZER_PORT, &GPIO_InitStruct);

		HAL_GPIO_WritePin(BUZZER_PORT, BUZZER_PIN, GPIO_PIN_RESET);
}


void BuzzerPIN_On(void){
	HAL_GPIO_WritePin(BUZZER_PORT, BUZZER_PIN, GPIO_PIN_SET);
}

/* RESET Puerto de Salida de Bandera */
/*************************************/

void BuzzerPIN_Off(void){
	HAL_GPIO_WritePin(BUZZER_PORT, BUZZER_PIN, GPIO_PIN_RESET);
}

void Buzzer_On(uint8_t* tabRING){

	uint8_t* ptr_tabRING;

	ptr_tabRING = tabRING;
	indexRING = 0;

	BuzzerPIN_On();
#ifdef RELOJ_DEBUG
	BuzzerPIN_Off();
#endif

	buzzer_to_cnt 	= ptr_tabRING[indexRING];
	n_pulsos_buzzer = ptr_tabRING[indexRING+2];
	PAUSE_TONE 		= ptr_tabRING[indexRING+3];
	ptr_tabRING_AUX = ptr_tabRING;

}


void buzzer_TIMEOUT (void){
// Ejecutada cada 10mseg
	if (buzzer_to_cnt != 0){
		buzzer_to_cnt--;
		if (buzzer_to_cnt == 0){
			BuzzerPIN_Off();
			buzzer_to_off = ptr_tabRING_AUX[indexRING+1];
		}
	}
}


void buzzer_TIMEOUT_toOFF (void){
     //Ejecutada cada 1mseg
     if (buzzer_to_off != 0){
    	 buzzer_to_off--;
       if (buzzer_to_off == 0){
			n_pulsos_buzzer--;
			if(n_pulsos_buzzer > 0){
				buzzer_to_cnt = ptr_tabRING_AUX[indexRING];
				BuzzerPIN_On();
			}else{
				//check pause
				if(PAUSE_TONE!=0xff){
					indexRING = indexRING + 4;
					pause_tone = PAUSE_TONE;
				}else{
					//fin beep

				}
			}

       }
     }
}

void pauseTONE_TIMEOUT (void){
// Ejecutada cada 10mseg
	if (pause_tone != 0){
		pause_tone--;
		if (pause_tone == 0){
			BuzzerPIN_On();
			buzzer_to_cnt 	= ptr_tabRING_AUX[indexRING];
			n_pulsos_buzzer = ptr_tabRING_AUX[indexRING+2];
			PAUSE_TONE 		= ptr_tabRING_AUX[indexRING+3];
		}
	}
}

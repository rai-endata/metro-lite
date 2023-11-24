/*
 * tipo de equipo.c
 *
 *  Created on: 23/11/2018
 *      Author: Rai
 */

#include "tipo de equipo.h"
#include "stm32f0xx_hal_gpio.h"
#include "Parametros Movil.h"

/* HAY DOS EQUIPOS (DOS PLACAS DISTINTAS)
 *
 * El FIRMWARE DIFERENCIA ESTAS DOS PLACAS ATRAVEZ DE DOS JUMPERS
 * SITUADOS EN EL LOS PUERTOS PB6 Y PB7
 *
 *------------------------
 * PB7 | PB6 | EQUIPO
 * -----------------------
 *  0  |  0  | METRO BLUE
 * -----------------------
 *  0  |  1  | NO DEFINIDO (METRO LITE con jumper)
 * ----------------------
 *  1  |  0  | NO DEFINIDO (METRO BLUE sin boton ... siempre programable ...)
 * -----------------------
 *  1  |  1  | METRO LITE
 *  ----------------------
 *
 * 1 - METRO BLUE: 	RELOJ HOMOLOGADO
 *     				USA JUMPER PARA PROGRAMACION
 *     				El dispositivo android no puede cambiar el
 *     				estado del reloj atravez de comandos. Ni ningun otro comando que
 *     				afecte el funcionamiento del reloj.
 *
 *
 * 2 - METROLITE (TRABAJA DE TRES FORMAS DE ACUERDO A LA PROGRAMACION)
 *
 * 		1 - METROLITE BANDERA LIBRE EN ALTO
 * 		2 - METROLITE BANDERA OCUPADO EN ALTO
 *					El dispositivo android no puede cambiar el
 *     				estado del reloj atravez de comandos. Ni ningun otro comando que
 *     				afecte el funcionamiento del reloj.
 *

 *
 * 		3 - METROLITE RELOJ: NO HOMOLOGADO
 * 							 SIN JUMPER DE PROGRAMACION
 * 							 SE MANEJA CON LA APP. ANDROID
 * 							 (se debe pasar a ocupado tambien por sensor de asiento)
 *
 *
 * */



static void check_pressTIPO_EQUIPO_PIN0(void);
static void check_pressTIPO_EQUIPO_PIN1(void);
static void ini_portTIPO_EQUIPO_PIN0(void);
static void ini_portTIPO_EQUIPO_PIN1(void);

tTIPO_EQUIPO_FLAGS			_tipoEquipoFLAGS;

tEQUIPO tipo_de_equipo;

static void ini_portTIPO_EQUIPO_PIN0(void){
	GPIO_InitTypeDef gpioinitstruct;

	TIPO_EQUIPO_PIN0_GPIO_CLK_ENABLE();

	gpioinitstruct.Pin = TIPO_EQUIPO_PIN0;
	//gpioinitstruct.Pull = GPIO_NOPULL;
	gpioinitstruct.Pull = GPIO_PULLUP;
	gpioinitstruct.Speed = GPIO_SPEED_FREQ_HIGH;

	// Configura pin como entrada como entrada
	//gpioinitstruct.Mode = GPIO_MODE_IT_RISING_FALLING;
	gpioinitstruct.Mode = GPIO_MODE_INPUT;
	HAL_GPIO_Init(TIPO_EQUIPO_PIN0_GPIO_PORT, &gpioinitstruct);

	//HAL_NVIC_SetPriority((IRQn_Type)(TIPO_EQUIPO_PIN0_EXTI_IRQn), 0x03, 0x00);
	HAL_NVIC_DisableIRQ((IRQn_Type)(TIPO_EQUIPO_PIN0_EXTI_IRQn));
}

static void ini_portTIPO_EQUIPO_PIN1(void){
	GPIO_InitTypeDef gpioinitstruct;

	TIPO_EQUIPO_PIN1_GPIO_CLK_ENABLE();

	gpioinitstruct.Pin = TIPO_EQUIPO_PIN1;
	//gpioinitstruct.Pull = GPIO_NOPULL;
	gpioinitstruct.Pull = GPIO_PULLUP;
	gpioinitstruct.Speed = GPIO_SPEED_FREQ_HIGH;

	// Configura pin como entrada como entrada
	//gpioinitstruct.Mode = GPIO_MODE_IT_RISING_FALLING;
	gpioinitstruct.Mode = GPIO_MODE_INPUT;
	HAL_GPIO_Init(TIPO_EQUIPO_PIN1_GPIO_PORT, &gpioinitstruct);

	//habilita y setea set PULSADOR IMPRESAION EXTI Interrupt a la mas baja prioridad
	//HAL_NVIC_SetPriority((IRQn_Type)(TIPO_EQUIPO_PIN1_EXTI_IRQn), 0x03, 0x00);
	HAL_NVIC_DisableIRQ((IRQn_Type)(TIPO_EQUIPO_PIN1_EXTI_IRQn));
}


static void check_pressTIPO_EQUIPO_PIN0(void){

	GPIO_PinState pinSTATE;

	pinSTATE = HAL_GPIO_ReadPin(TIPO_EQUIPO_PIN0_GPIO_PORT, TIPO_EQUIPO_PIN0);
	if(pinSTATE){
		pin0_tipoEQUIPO = 1;
	}else{
		pin0_tipoEQUIPO = 0;
	}
}


static void check_pressTIPO_EQUIPO_PIN1(void){

	GPIO_PinState pinSTATE;

	pinSTATE = HAL_GPIO_ReadPin(TIPO_EQUIPO_PIN1_GPIO_PORT, TIPO_EQUIPO_PIN1);
	if(pinSTATE){
		pin1_tipoEQUIPO = 1;

	}else{
		pin1_tipoEQUIPO = 0;
	}
}



void set_tipo_de_equipo (void){

	ini_portTIPO_EQUIPO_PIN0();
	ini_portTIPO_EQUIPO_PIN1();

	check_pressTIPO_EQUIPO_PIN0();
	check_pressTIPO_EQUIPO_PIN1();

	if((pin0_tipoEQUIPO == 0) && (pin1_tipoEQUIPO == 0)){
	   tipo_de_equipo = METRO_BLUE;
	}else if((pin0_tipoEQUIPO == 1) && (pin1_tipoEQUIPO == 1)){
	   tipo_de_equipo = METRO_LITE;
	}else if((pin0_tipoEQUIPO == 0) && (pin1_tipoEQUIPO == 1)){
		   tipo_de_equipo = MINI_BLUE;
	}else{
	   tipo_de_equipo = EQUIPO_NO_DEFINIDO;
	}

}

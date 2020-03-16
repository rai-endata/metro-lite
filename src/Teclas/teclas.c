/*
 * teclas.c
 *
 *  Created on: 26/3/2018
 *      Author: Rai
 */
#include "Reloj.h"
#include "teclas.h"
#include "buzzer.h"
#include "display-7seg.h"
#include "Programacion Parametros.h"
#include "Parametros Reloj.h"
#include "DA Define Commands.h"
#include "Reportes.h"
#include "tipo de equipo.h"

#ifdef VISOR_REPORTES
 static void mostrar_menu_ini (void);
#endif


uint16_t		CONTADOR_tiempoPULSADO_T1;
uint16_t		CONTADOR_tiempoPULSADO_T2;
t_teclasFLAGS	_teclasFLAGS;
byte indexMenu_IniTurno;

void  ini_tecla1(void){
	GPIO_InitTypeDef gpioinitstruct;

	if(EQUIPO_METRO_LITE){
		// Configura pin como salida para que no meta ruido
		TECLA1_GPIO_CLK_ENABLE();
		gpioinitstruct.Pin = TECLA1_PIN;
		gpioinitstruct.Pull = GPIO_NOPULL;
		gpioinitstruct.Speed = GPIO_SPEED_FREQ_HIGH;
		gpioinitstruct.Mode = GPIO_MODE_OUTPUT_OD;
		HAL_GPIO_Init(TECLA1_PORT, &gpioinitstruct);
	}else{
		TECLA1_GPIO_CLK_ENABLE();
		gpioinitstruct.Pin = TECLA1_PIN;
		gpioinitstruct.Pull = GPIO_NOPULL;
		gpioinitstruct.Speed = GPIO_SPEED_FREQ_HIGH;
		// Configura pin como entrada con interrupcion externa
		gpioinitstruct.Mode = GPIO_MODE_IT_RISING_FALLING;
		HAL_GPIO_Init(TECLA1_PORT, &gpioinitstruct);
		//habilita y setea set PULSADOR IMPRESAION EXTI Interrupt a la mas baja prioridad
		HAL_NVIC_SetPriority((IRQn_Type)(TECLA1_EXTI_IRQn), 0x03, 0x00);
		HAL_NVIC_EnableIRQ((IRQn_Type)(TECLA1_EXTI_IRQn));
		indexMenu_IniTurno = 1;
	}

}

void  ini_tecla2(void){
	GPIO_InitTypeDef gpioinitstruct;

	if(EQUIPO_METRO_LITE){
		TECLA2_GPIO_CLK_ENABLE();
		gpioinitstruct.Pin = TECLA2_PIN;
		gpioinitstruct.Pull = GPIO_NOPULL;
		gpioinitstruct.Speed = GPIO_SPEED_FREQ_HIGH;
		// Configura pin como entrada con interrupcion externa
		gpioinitstruct.Mode = GPIO_MODE_OUTPUT_OD;
		HAL_GPIO_Init(TECLA2_PORT, &gpioinitstruct);
	}else{
		TECLA2_GPIO_CLK_ENABLE();
		gpioinitstruct.Pin = TECLA2_PIN;
		gpioinitstruct.Pull = GPIO_NOPULL;
		gpioinitstruct.Speed = GPIO_SPEED_FREQ_HIGH;
		// Configura pin como entrada con interrupcion externa
		gpioinitstruct.Mode = GPIO_MODE_IT_RISING_FALLING;
		HAL_GPIO_Init(TECLA2_PORT, &gpioinitstruct);
		//habilita y setea set PULSADOR IMPRESAION EXTI Interrupt a la mas baja prioridad
		HAL_NVIC_SetPriority((IRQn_Type)(TECLA2_EXTI_IRQn), 0x03, 0x00);
		HAL_NVIC_EnableIRQ((IRQn_Type)(TECLA2_EXTI_IRQn));
	}
}


//se invoca desde la irq
void check_pressT1(void){

	GPIO_PinState pinSTATE;
	if(!RELOJ_BANDERITA){
		pinSTATE = HAL_GPIO_ReadPin(TECLA1_PORT, TECLA1_PIN);
		if(!pinSTATE){
			//flanco descendente (pulso la tecla)
			startCONTADOR_tiempoPULSADO_T1();
		}else{
			//flanco ascendente (solto la tecla)
			if(contarTiempoPulsadoT1_F){
			  //es tecla corta (si fuera tecla larga ya estaria reseteado el contador)
				pressTECLA1_CORTA=1;
			}
			stopCONTADOR_tiempoPULSADO_T1();
		}
	}
}

//se invoca desde la irq
void check_pressT2(void){

	GPIO_PinState pinSTATE;

	if(!RELOJ_BANDERITA){
		pinSTATE = HAL_GPIO_ReadPin(TECLA2_PORT, TECLA2_PIN);
		if(!pinSTATE){
			//flanco descendente (pulso la tecla)
			startCONTADOR_tiempoPULSADO_T2();
		}else{
			//flanco ascendente (solto la tecla)
			if(contarTiempoPulsadoT2_F){
			  //es tecla corta (si fuera tecla larga ya estaria reseteado el contador)
				pressTECLA2_CORTA=1;
			}
			stopCONTADOR_tiempoPULSADO_T2();
		}
	}
}


//se invoca desde el loop
void check_pressTECLA(void){
	check_pressTECLA1_CORTA();
	check_pressTECLA2_CORTA();
	check_pressTECLA1_LARGA();
	check_pressTECLA2_LARGA();
}


void check_pressTECLA1_CORTA(void){
	if(pressTECLA1_CORTA){
		pressTECLA1_CORTA=0;
		teclaT1_CORTA();
	}
}

void check_pressTECLA2_CORTA(void){
	if(pressTECLA2_CORTA){
		pressTECLA2_CORTA=0;
		teclaT2_CORTA();
	}
}

void check_pressTECLA1_LARGA(void){
	if(pressTECLA1_LARGA){
		pressTECLA1_LARGA=0;
		teclaT1_LARGA();
	}
}

void check_pressTECLA2_LARGA(void){
	if(pressTECLA2_LARGA){
		pressTECLA2_LARGA=0;
		teclaT2_LARGA();
	}
}


void teclaT1_CORTA(void){
	byte exit;
	byte numeroCHOFERES;
	numeroCHOFERES = cantCHOFERES + 1;

	exit = 0;
	if(prog_mode){
		exit = 1;
    	NVIC_SystemReset();
    }
#ifdef VISOR_REPORTES
	if(REPORTES_HABILITADOS){
		if(mostrar_turn_dsplyIMPORTE && !exit){
						//cancelar
						//dejo de mostar turn y vuelvo a mostrar ini
						mostrar_turn_dsplyIMPORTE=0;
						//vuelvo a muestro menu ini
						mostrar_menu_ini();
						indexMenu_IniTurno = 0;
		}
		if(mostrar_ini_dsplyIMPORTE && !exit){
			//mostrarMENU_IniTurno = 1;
			indexMenu_IniTurno++;
			if(indexMenu_IniTurno == (numeroCHOFERES+1)){
				mostrar_ini_dsplyIMPORTE = 0;
				mostrar_turn_dsplyIMPORTE = 1;
				mostrar_t_dsplyT=1;
				on_display_tarifa();

			}
			if(indexMenu_IniTurno > (numeroCHOFERES+2)){  //+2 PORQUE EN +1 MUESTRA turn
				indexMenu_IniTurno =1;
			}
			exit = 1;
			Buzzer_On(BEEP_TECLA_CORTA);
		}
		if(mostrar_conF_dsplyIMPORTE){
						//cancelar
						//dejo de mostar conF y vuelvo a mostrar ini
						mostrar_conF_dsplyIMPORTE=0;
						//vuelvo a muestro menu ini
						mostrar_menu_ini();
						exit = 1;
		}
	}
#endif

	if(!exit){
		if(((ESTADO_RELOJ!=FUERA_SERVICIO) && !prog_mode) ){
			cambio_de_reloj_x_pulsador();
		}else{
			Buzzer_On(BEEP_TECLA_CORTA);
		}
	}
}

void teclaT2_CORTA(void){
	 if(prog_mode){
		NVIC_SystemReset();
	 }

	 Buzzer_On(BEEP_TECLA_CORTA);
	 if(!prog_mode){
		if(ESTADO_RELOJ==LIBRE){
			//Pase_a_FUERA_SERVICIO();
			Pase_a_FUERA_SERVICIO();
		}else if(ESTADO_RELOJ==COBRANDO){
			//IMPRESION DE TICKET DE VIAJE
			print_ticket_viaje();
		}else if(ESTADO_RELOJ==FUERA_SERVICIO){
  	 #ifdef VISOR_REPORTES
			if(REPORTES_HABILITADOS){
				if(mostrar_ini_dsplyIMPORTE){
					mostrar_ini_dsplyIMPORTE=0;
					mostrar_conF_dsplyIMPORTE=1;
					mostrar_numero_dsplyT=1;
				}else if(mostrar_conF_dsplyIMPORTE){
					mostrar_conF_dsplyIMPORTE=0;
					mostrar_t_dsplyT=0;
					nroChofer = indexMenu_IniTurno;
					off_display();
					RELOJ_INTERNO_newSesion(nroChofer);
					Tx_Conf_inicioTURNO(okINICIO);
				}else if(mostrar_turn_dsplyIMPORTE){
					BanderaOut_On();
					print_ticket_turno(0);
				}else{
					Pase_a_LIBRE();
				}
			}
	#else
			Pase_a_LIBRE();
	#endif

	  }
  }
}


void teclaT1_LARGA(void){

	if(prog_mode){
		NVIC_SystemReset();
    }

	if(ESTADO_RELOJ==FUERA_SERVICIO){
 #ifdef VISOR_REPORTES
		if(REPORTES_HABILITADOS){
			if(mostrar_turn_dsplyIMPORTE || mostrar_ini_dsplyIMPORTE){
				//cancelar
				//dejo de mostrar ini y apago todo (fuera de servicio)
				mostrar_ini_dsplyIMPORTE = 0;
				mostrar_turn_dsplyIMPORTE = 0;
				mostrar_t_dsplyT=0;
				off_display();
			}else{
				//muestro menu ini
				mostrar_menu_ini();
				indexMenu_IniTurno = nroChofer;
			}
		}
#endif

  }
  //Tx_Comando_TRANSPARENTE_prueba();
}


void teclaT2_LARGA(void){

	if(prog_mode){
    	NVIC_SystemReset();
    }
	//Tx_Boton_EMERGENCIA();
}







#ifdef VISOR_REPORTES
	static void mostrar_menu_ini (void){

		mostrar_ini_dsplyIMPORTE=1;
		mostrar_t_dsplyT=1;
		on_display_tarifa();

	}
#endif

void startCONTADOR_tiempoPULSADO_T1(void){
	contarTiempoPulsadoT1_F=1;
}

void stopCONTADOR_tiempoPULSADO_T1(void){
	contarTiempoPulsadoT1_F = 0;
	CONTADOR_tiempoPULSADO_T1=0;
}

void startCONTADOR_tiempoPULSADO_T2(void){
	contarTiempoPulsadoT2_F=1;
}

void stopCONTADOR_tiempoPULSADO_T2(void){
	contarTiempoPulsadoT2_F = 0;
	CONTADOR_tiempoPULSADO_T2=0;
}


/* TIMERS */
/**********/


  void CONTADOR_tiempoPULSADO_timerT1 (void){
    // Ejecutada cada 1 10 mseg no preciso.
    // Este timer se dispara cuando al iniciar el pulsado de tecla1
    if (contarTiempoPulsadoT1_F){
    	CONTADOR_tiempoPULSADO_T1++;
    	if(CONTADOR_tiempoPULSADO_T1 == TIEMPO_TECLA_LARGA){
    		stopCONTADOR_tiempoPULSADO_T1();
	#ifdef VISOR_REPORTES
			if (REPORTES_HABILITADOS){
				Buzzer_On(BEEP_TECLA_LARGA);
				pressTECLA1_LARGA=1;
			}
	#endif
    	}
    }
  }

  void CONTADOR_tiempoPULSADO_timerT2 (void){
    // Ejecutada cada 1 10 mseg no preciso.
    // Este timer se dispara cuando al iniciar el pulsado de tecla1
    if (contarTiempoPulsadoT2_F){
    	CONTADOR_tiempoPULSADO_T2++;
    	if(CONTADOR_tiempoPULSADO_T2 == TIEMPO_TECLA_LARGA){
			stopCONTADOR_tiempoPULSADO_T2();
    		if(prog_mode){
				Buzzer_On(BEEP_TECLA_LARGA);
				pressTECLA2_LARGA=1;
        	}
    	}
    }
  }


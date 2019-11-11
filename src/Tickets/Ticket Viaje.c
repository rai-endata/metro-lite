
#include "Tarifacion Reloj.h"
#include "Reloj.h"
#include "odometro.h"
#include "Calculo Velocidad.h"
#include "Common Ticket.h"
#include "Ticket Viaje.h"
#include "teclas.h"
#include "eeprom.h"


uint32_t nroTICKET;

void print_ticket_viaje(void){

	uint8_t* ptrSimple;
	uint8_t** ptrDouble;
	uint8_t buffer_nro_correlativo_viaje[11];
	uint8_t buffer_nroTICKET[11];
	uint8_t buffer_ValorViaje[11];
	uint8_t buffer_fichas_Total[11];
	uint8_t buffer_Bajada[11];
	uint8_t buffer_ValorFichaD[11];
	uint8_t buffer_ValorFichaT[11];
	uint8_t buffer_TarifaN[11];
	uint8_t buffer_Distancia[11];
	uint8_t buffer_VelMax[11];
	uint8_t buffer_MinEspera[11];
	uint8_t buffer_Date_cobrando[20];
	uint8_t buffer_Date_ocupado[20];
	uint8_t buffer_FichasD[20];
	uint8_t buffer_FichasT[20];
	uint32_t fichas_x_Pulsos;
	uint32_t fichas_x_Tiempo;
	uint16_t N_print;
  if(ESTADO_RELOJ==COBRANDO){
    if(statusPRINT==NO_HAY_IMPRESION_EN_PROCESO){
    		statusPRINT = IMPRESION_EN_PROCESO;
    		to_print_cnt = TO_PRINTING_VIAJE;
			//preparar_print (nroTICKET, 0, &buffer_nro_correlativo_viaje, 0 );
    		preparar_print_nroTICKET (nroTICKET, &buffer_nroTICKET);
    		//preparar_print (nroCorrelativo_INTERNO, 0, &buffer_nro_correlativo_viaje, 0 );

			ptrSimple = &print_buffer;
			ptrDouble = &ptrSimple;

			preparar_print (tarifa_1_4, 0, &buffer_TarifaN, 0);
			if(TARIFA_PESOS){
				//valor en pesos
				preparar_print (TARIFA.bajadaBandera, PUNTO_DECIMAL, &buffer_Bajada, 1);
				preparar_print (TARIFA.valorFicha, PUNTO_DECIMAL, &buffer_ValorFichaD, 1);
				preparar_print (TARIFA.valorFichaTiempo, PUNTO_DECIMAL, &buffer_ValorFichaT, 1);
				preparar_print (VALOR_VIAJE, PUNTO_DECIMAL, &buffer_ValorViaje, 1);
				preparar_print (fichas_Total, 0, &buffer_fichas_Total, 0);
			}else{
				if(EqPESOS_hab){
					preparar_print (fichas_Total, 0, &buffer_fichas_Total, 0);
					//valor en fichas
					if(TARIFA.diaNoche==0){
						//tarifa diurna
						preparar_print (EqPESOS_DIURNA.bajadaBandera, 3, &buffer_Bajada, 1);
						preparar_print (VALOR_VIAJE/1000, 0, &buffer_ValorViaje, 1);
						preparar_print (EqPESOS_DIURNA.valorFicha, 3, &buffer_ValorFichaD, 1);
						preparar_print (EqPESOS_DIURNA.valorFichaTiempo, 3, &buffer_ValorFichaT, 1);

					}else{
						//tarifa Nocturna
						preparar_print (EqPESOS_NOCTURNA.bajadaBandera, 3, &buffer_Bajada, 1);
						preparar_print (VALOR_VIAJE/1000, 0, &buffer_ValorViaje, 1);
						preparar_print (EqPESOS_NOCTURNA.valorFicha, 3, &buffer_ValorFichaD, 1);
						preparar_print (EqPESOS_NOCTURNA.valorFichaTiempo, 3, &buffer_ValorFichaT, 1);
					}
				}else{
					//valor en pesos
					preparar_print (TARIFA.bajadaBandera, PUNTO_DECIMAL, &buffer_Bajada, 0);
					preparar_print (TARIFA.valorFicha, PUNTO_DECIMAL, &buffer_ValorFichaD, 0);
					preparar_print (TARIFA.valorFichaTiempo, PUNTO_DECIMAL, &buffer_ValorFichaT, 1);
					preparar_print (VALOR_VIAJE, PUNTO_DECIMAL, &buffer_ValorViaje, 1);
					preparar_print (fichas_Total, 0, &buffer_fichas_Total, 0);
				}
			}
			preparar_print (minutosEspera, 0, &buffer_MinEspera, 0 );
			preparar_print (VELOCIDAD_MAX, 0, &buffer_VelMax, 0 );

			fichas_x_Pulsos = TARIFACION_getFichasD();
			fichas_x_Tiempo = TARIFACION_getFichasT();

			preparar_print (fichas_x_Pulsos, 0, &buffer_FichasD, 0 );
			preparar_print (fichas_x_Tiempo, 0, &buffer_FichasT, 0 );

			//prepara hora y fecha de inicio y fin de viaje
			date_to_string(buffer_Date_cobrando, cobrandoDATE);
			//date_to_string(buffer_Date_cobrando, HoraApagado);

			date_to_string(buffer_Date_ocupado, ocupadoDATE);

			if(DISTANCIAm_OCUPADO<1001){
				//distancia en metros
				preparar_print (DISTANCIAm_OCUPADO, 0, &buffer_Distancia, 0 );
			}else{
				//distancia en km con 2 decimales
				preparar_print (DISTANCIAm_OCUPADO/10, 2, &buffer_Distancia, 0 );

			}

			//en print_buffer se guarda todos los datos a enviar a la impresora
			//se guarda como un solo string.
			//cada linea de impresion finaliza con 0x0A ya que este es el comando
			//que imprime una linea y sube el carro de impresion una linea.


			IniPRINT(ptrDouble);	//Normal Mode (inicializa impresora MPT II)
			//deshabilitar_negrita(ptrDouble);
			//printFONT_B(ptrDouble);
			Ajuste_Interlinea(ptrDouble, 28);
			add_LF(ptrDouble);
			printLINE(ptrDouble);
			//disableBOLD(ptrDouble);
			//georgianFONT(ptrDouble);
			//habilitar_negrita(ptrDouble);
			//enableFONT_ITALIC(ptrDouble);
			//setCharCode_spain(ptrDouble);
			//enableBOLD(ptrDouble);
			//setBOLD(ptrDouble);
			//string_copy_incDest(ptrDouble,"         TICKET DE VIAJE");
			string_copy_incDest(ptrDouble,"TICKET DE VIAJE");
			string_copy_incDest(ptrDouble,"      ");
			//string_copy_incDest(ptrDouble,&buffer_nro_correlativo_viaje);
			string_copy_incDest(ptrDouble,&buffer_nroTICKET);
			//string_copy_incDest(ptrDouble,"         ÑANDU");
			//disableBOLD(ptrDouble);
			//clearBOLD(ptrDouble);
			add_LF(ptrDouble);
			//deshabilitar_negrita(ptrDouble);
			//disableFONT_ITALIC(ptrDouble);
			printLINE(ptrDouble);
			//add_LF(ptrDouble);
			//print nombre de empresa
			printEMPRESA(ptrDouble);
			//print datos del titular
			//printCABECERA(ptrDouble,1);
			printCABECERA(ptrDouble,0);
			string_copy_incDest(ptrDouble,"Tarifa: ");
			if(TARIFA_PESOS){
				string_copy_incDest(ptrDouble,"               ");
			}else{
				if(EqPESOS_hab){
					string_copy_incDest(ptrDouble,"               ");
				}else{
					string_copy_incDest(ptrDouble,"               ");
				}
			}
			string_copy_incDest(ptrDouble,&buffer_TarifaN);
			if(TARIFA.diaNoche==0){
			  //TARIFA DIURNA
			  string_copy_incDest(ptrDouble,"D");
			}else{
			  //TARIFA NOCTURNA
			  string_copy_incDest(ptrDouble,"N");
			}

			add_LF(ptrDouble);

			string_copy_incDest(ptrDouble,"Bajada: ");

			if(TARIFA_PESOS){
				if(PUNTO_DECIMAL == 0){
					string_copy_incDest(ptrDouble,"               ");
				}else{
					string_copy_incDest(ptrDouble,"              ");
				}
			}else{
				if(EqPESOS_hab){
					string_copy_incDest(ptrDouble,"              ");
				}else{
					string_copy_incDest(ptrDouble,"                ");
				}
			}

			string_copy_incDest(ptrDouble,&buffer_Bajada);
			add_LF(ptrDouble);

			if(SERVICIO_TAXI){
				string_copy_incDest(ptrDouble,"Valor Ficha:");
				if(TARIFA_PESOS){
					if(PUNTO_DECIMAL == 0){
						string_copy_incDest(ptrDouble,"           ");
					}else{
						string_copy_incDest(ptrDouble,"          ");
					}
				}else{
					if(EqPESOS_hab){
						string_copy_incDest(ptrDouble,"          ");
					}else{
						string_copy_incDest(ptrDouble,"            ");
					}
				}
				string_copy_incDest(ptrDouble,&buffer_ValorFichaD);
				add_LF(ptrDouble);
			}else{
				string_copy_incDest(ptrDouble,"Valor F. Dist:");
				if(TARIFA_PESOS){
					if(PUNTO_DECIMAL == 0){
						string_copy_incDest(ptrDouble,"         ");
					}else{
						string_copy_incDest(ptrDouble,"        ");
					}
				}else{
					if(EqPESOS_hab){
						string_copy_incDest(ptrDouble,"        ");
					}else{
						string_copy_incDest(ptrDouble,"          ");
					}
				}
				string_copy_incDest(ptrDouble,&buffer_ValorFichaD);
				add_LF(ptrDouble);

				string_copy_incDest(ptrDouble,"Valor F. Tiempo:");
				string_copy_incDest(ptrDouble,"       ");
				string_copy_incDest(ptrDouble,&buffer_ValorFichaT);
				add_LF(ptrDouble);
			}


			printLINE(ptrDouble);

			string_copy_incDest(ptrDouble,"INICIO: ");
			if(TARIFA_PESOS){
				string_copy_incDest(ptrDouble,"   ");
			}else{
				if(EqPESOS_hab){
					string_copy_incDest(ptrDouble,"    ");
				}else{
					string_copy_incDest(ptrDouble,"    ");
				}
			}
			string_copy_incDest(ptrDouble,&buffer_Date_ocupado);
			add_LF(ptrDouble);
			string_copy_incDest(ptrDouble,"FIN:");
			if(TARIFA_PESOS){
				string_copy_incDest(ptrDouble,"       ");
			}else{
				string_copy_incDest(ptrDouble,"        ");
			}
			string_copy_incDest(ptrDouble,&buffer_Date_cobrando);
			add_LF(ptrDouble);

			string_copy_incDest(ptrDouble,"Distancia:");
			if(DISTANCIAm_OCUPADO<1001){
				//distancia en metros
				string_copy_incDest(ptrDouble,"            ");
			}else{
				//distancia en km con 2 decimales
				string_copy_incDest(ptrDouble,"          ");
			}
			string_copy_incDest(ptrDouble,&buffer_Distancia);
			if(DISTANCIAm_OCUPADO<1001){
				//distancia en metros
				string_copy_incDest(ptrDouble," m");
			}else{
				//distancia en km con 2 decimales
				string_copy_incDest(ptrDouble," Km");
			}
			add_LF(ptrDouble);

	/*
			string_copy_incDest(ptrDouble,"Espera:");
			string_copy_incDest(ptrDouble,"             ");
			string_copy_incDest(ptrDouble,&buffer_MinEspera);
			string_copy_incDest(ptrDouble," min");
			add_LF(ptrDouble);
	*/
			string_copy_incDest(ptrDouble,"Fichas Dist:");
			string_copy_incDest(ptrDouble,"            ");
			string_copy_incDest(ptrDouble,&buffer_FichasD);
			//string_copy_incDest(ptrDouble,&buffer_MinEspera);
			add_LF(ptrDouble);

			string_copy_incDest(ptrDouble,"Fichas Tiempo:");
			string_copy_incDest(ptrDouble,"          ");
			string_copy_incDest(ptrDouble,&buffer_FichasT);
			add_LF(ptrDouble);

			string_copy_incDest(ptrDouble,"Vel. Max:");
			string_copy_incDest(ptrDouble,"          ");
			string_copy_incDest(ptrDouble,&buffer_VelMax);
			string_copy_incDest(ptrDouble," km/h");
			add_LF(ptrDouble);
			printLINE(ptrDouble);

			//habilitar_negrita(ptrDouble);
			//enableBOLD(ptrDouble);
			//setBOLD(ptrDouble);
			//enableFONT_ITALIC(ptrDouble);

			if(TARIFA_FICHAS){
				string_copy_incDest(ptrDouble,"FICHAS:");
				string_copy_incDest(ptrDouble,"                 ");
				string_copy_incDest(ptrDouble,&buffer_fichas_Total);
			}
			if(TARIFA_PESOS){
				//add_LF(ptrDouble);
				string_copy_incDest(ptrDouble,"A PAGAR: ");
				if(PUNTO_DECIMAL == 0){
					string_copy_incDest(ptrDouble,"              ");
				}else{
					string_copy_incDest(ptrDouble,"             ");
				}
				string_copy_incDest(ptrDouble,&buffer_ValorViaje);
			}else{
				if(EqPESOS_hab){
					add_LF(ptrDouble);
					string_copy_incDest(ptrDouble,"A PAGAR: ");
					string_copy_incDest(ptrDouble,"              ");
					string_copy_incDest(ptrDouble,&buffer_ValorViaje);
				}
			}
			add_LF(ptrDouble);
			//disableBOLD(ptrDouble);
			//clearBOLD(ptrDouble);
			//deshabilitar_negrita(ptrDouble);
			//disableFONT_ITALIC(ptrDouble);
			printLINE(ptrDouble);

			string_copy_incDest(ptrDouble,"           Gracias ");
			//string_copy_incDest(ptrDouble,"     ");
			//string_copy_incDest(ptrDouble,&buffer_nro_correlativo_viaje);

			//retorno
			add_LF(ptrDouble);
			//sube el papel
			add_LF(ptrDouble);
			add_LF(ptrDouble);
			add_LF(ptrDouble);
			//marco fin de string
			**ptrDouble = 0x00;

			N_print = ptrSimple - (uint16_t)&print_buffer;
			//inicio envio por el puerto
			PRINT_send(print_buffer, N_print);
    }
  }
}
void  ini_pulsador_impresion(void){
	ini_tecla2();
	/*
	GPIO_InitTypeDef gpioinitstruct;
	PULSADOR_IMPRESION_GPIO_CLK_ENABLE();
	gpioinitstruct.Pin = PULSADOR_IMPRESION_PIN;
	gpioinitstruct.Pull = GPIO_NOPULL;
	gpioinitstruct.Speed = GPIO_SPEED_FREQ_HIGH;
	// Configura pin como entrada con interrupcion externa
	gpioinitstruct.Mode = GPIO_MODE_IT_FALLING;
	HAL_GPIO_Init(PULSADOR_IMPRESION_GPIO_PORT, &gpioinitstruct);
	//habilita y setea set PULSADOR IMPRESAION EXTI Interrupt a la mas baja prioridad
	HAL_NVIC_SetPriority((IRQn_Type)(PULSADOR_IMPRESION_EXTI_IRQn), 0x03, 0x00);
	HAL_NVIC_EnableIRQ((IRQn_Type)(PULSADOR_IMPRESION_EXTI_IRQn));
	*/
}


void inc_nroTICKET (void){
	nroTICKET++;
	if(nroTICKET==0){
		nroTICKET=1;
	}
	EEPROM_writeDouble ((uint32_t*)EEPROM_NRO_TICKET, nroTICKET);
}

/*
 * Ticket Turno.c

 *
 *  Created on: 14/3/2018
 *      Author: Rai
 */
#include "- metroBLUE Config -.h"

#ifdef VISOR_PRINT_TICKET
	#include "Ticket Parcial.h"
	#include "Ticket Turno.h"
	#include "stdint.h"
	#include "calculos.h"
	#include "DA Tx.h"
	#include "Parametros Reloj.h"
	#include "Parametros Ticket.h"
	#include "Reloj.h"
	#include "Variables en flash.h"
	#include "Common Ticket.h"
	#include "Tarifacion Reloj.h"
	#include "Reportes.h"
	#include <stdio.h>
	#include <stdlib.h>

	uint32_t getFichasDistancia_tarifa(byte nroTarifa);
	uint32_t getFichasTiempo_tarifa(byte nroTarifa);


		//static tREG_SESION iniTURNO;
	//static tREG_SESION finTURNO;
	//static tREG_SESION* iniTURNO_ptr;      // Puntero a inicio de Turno del tipo SESION
	//static tREG_SESION* finTURNO_ptr;      // Puntero a final de Turno del tipo SESION


	void print_ticket_parcial(void){

		uint8_t* ptrSimple;
		uint8_t** ptrDouble;
		uint8_t buffer_aux[20];
		uint16_t aux16;
		uint32_t aux32;
		uint16_t N_print;
		tREG_GENERIC* REPORTE_PUTptr_aux;
		uint8_t puntoDECIMAL;

		if(ESTADO_RELOJ==FUERA_SERVICIO){
			if(statusPRINT==NO_HAY_IMPRESION_EN_PROCESO){
				statusPRINT = IMPRESION_EN_PROCESO;

			if(TARIFA_PESOS){
				//muestra importe
				puntoDECIMAL = PUNTO_DECIMAL;
			}else{
				//muestra ficha
				puntoDECIMAL = 3;
			}

				//printf("STATUS = %c\n",statusPRINT);
				//fflush(stdout);

				//iniTURNO_aux_ptr = &iniTURNO_aux;
				//finTURNO_aux_ptr = &finTURNO_aux;
			   // BuzzerPIN_Off();
				REPORTE_PUTptr_aux = REPORTE_PUTptr;
				ptrSimple = (uint8_t*)&print_buffer;
				ptrDouble = (uint8_t**)&ptrSimple;
				// El primer puntero a sesion (sesion_ptrs[0]) corresponde al ultimo turno
				// abierto, por lo tanto no esta cerrado => Los turnos comienzan a partir
				// de sesion_ptrs[1]. Es por eso que si el indice vale X:
				//  - INCIO -> sesion_ptrs[X+1]
				//  - FIN ---> sesion_ptrs[X]
				iniTURNO_ptr = &iniTURNO;
				finTURNO_ptr = &finTURNO;


				//Normal Mode (inicializa impresora MPT II)
				IniPRINT(ptrDouble);
				printLINE(ptrDouble);
				//print reporte de turno
				string_copy_incDest(ptrDouble,"       RECAUDACION PARCIAL");
				add_LF(ptrDouble);
				printLINE(ptrDouble);
				//add_LF(ptrDouble);
				//print nombre de empresa
				printEMPRESA(ptrDouble);
				REPORTES_getSesions(sesion_ptrs, max_turnosReporte);        				// Obtengo punteros a todos las sesiones
				//inicializo punteros a sesiones
				//TICKET_PARCIAL_setIni(sesion_ptrs[MENU_REPORTE_TURNO_index + 1], iniTURNO_ptr);   // Puntero a inicio de turno seleccionado
				TICKET_PARCIAL_setFin(sesion_ptrs[MENU_REPORTE_TURNO_index], finTURNO_ptr);   // Puntero a inicio de turno seleccionado
				//iniTURNO_ptr_aux = sesion_ptrs[MENU_REPORTE_TURNO_index + 1];
				finTURNO_ptr_aux = sesion_ptrs[MENU_REPORTE_TURNO_index];
				//print numero de turno
				string_copy_incDest(ptrDouble,"Nro Turno               ");
				//preparar_print (iniTURNO_ptr->nroTurno, 0, &buffer_aux, 0);
				preparar_print (finTURNO_ptr->nroTurno, 0, &buffer_aux, 0);
				string_copy_incDest(ptrDouble,&buffer_aux);
				add_LF(ptrDouble);

				//print chofer
				string_copy_incDest(ptrDouble,"Chofer                  ");
				preparar_print (finTURNO_ptr->chofer, 0, &buffer_aux, 0);
				string_copy_incDest(ptrDouble,&buffer_aux);
				add_LF(ptrDouble);

				//print date inicio de turno
				string_copy_incDest(ptrDouble,"Inicio     ");
				//date_to_string(buffer_aux, iniTURNO_ptr->date);
				date_to_string(buffer_aux, finTURNO_ptr->date);
				string_copy_incDest(ptrDouble,&buffer_aux);
				add_LF(ptrDouble);

				//print date impresion
				string_copy_incDest(ptrDouble,"Impreso el ");
				date_to_string(buffer_aux, getDate());
				string_copy_incDest(ptrDouble,&buffer_aux);
				add_LF(ptrDouble);

				printLINE(ptrDouble);

				//print cantidad de viajes
				string_copy_incDest(ptrDouble,"Viajes:                 ");
				aux16 = getViajes_Parcial();
				preparar_print (aux16, 0, &buffer_aux, 0);
				string_copy_incDest(ptrDouble,&buffer_aux);
				add_LF(ptrDouble);

				//print recaudacion
				string_copy_incDest(ptrDouble,"RECAUDACION:            ");
				aux32 = getRecaudacion_Parcial();
				//preparar_print (aux32, 0, &buffer_aux, 0);
				preparar_print_poneCOMA (aux32, puntoDECIMAL, &buffer_aux, 0 );
				string_copy_incDest(ptrDouble,&buffer_aux);
				add_LF(ptrDouble);

				//GRACIAS
				printLINE(ptrDouble);
				string_copy_incDest(ptrDouble,"           Gracias ");
				add_LF(ptrDouble);
				printLINE(ptrDouble);

				//subo el carro varias lineas
				add_LF(ptrDouble);
				add_LF(ptrDouble);
				add_LF(ptrDouble);
				add_LF(ptrDouble);
				add_LF(ptrDouble);
				add_LF(ptrDouble);
				//marco fin de string
				**ptrDouble = 0x00;
				//inicio envio por el puerto
				N_print = ptrSimple - (uint16_t)&print_buffer;
				//inicio envio por el puerto
				PRINT_send(print_buffer, N_print);


			}
		}
	}

	/* SETEAR PUNTERO DE INICIO DE TURNO */
	/*************************************/

	void TICKET_PARCIAL_setIni (tREG_SESION* addr_iniSESION, tREG_SESION* reg_iniSESION ){
		uint32_t addr;
		addr = (uint32_t)addr_iniSESION;

		EEPROM_ReadBuffer(reg_iniSESION,(uint32_t)addr_iniSESION,sizeof(tREG_SESION));
	}


	void TICKET_PARCIAL_setFin (tREG_SESION* addr_finSESION, tREG_SESION* reg_finSESION ){
		uint32_t addr;
		addr = (uint32_t)addr_finSESION;

		EEPROM_ReadBuffer(reg_finSESION,(uint32_t)addr_finSESION,sizeof(tREG_SESION));
	}



	/* EXTRAER RECAUDACION PARCIAL */
	/*******************************/
	uint32_t getRecaudacion_Parcial(void){
		// La recaudacion parcial es el total de todos los viajes, desde el ultimo inicio
		// de turno (sesion mas reciente), hasta ahora.
		//
		// EDIT 17/10/12
		//  Dejo de ser static, xq se llama por fuera, en la pantalla RECAUDACION
		tREG_SESION*far SESION_ptr;
		tREG_GENERIC*far ACTUAL_ptr;
		byte nroTarifa;
		uint32_t recaudacion;

		SESION_ptr = (tREG_SESION*far) REPORTES_getPrevRegister_byType(REG_sesion);   // Extraigo puntero SESION anterior
		ACTUAL_ptr = REPORTES_getPUT_ptr();           // Extraigo puntero actual

		if (SESION_ptr == ACTUAL_ptr){
		  // Si se diera el caso de que el puntero de inicio de sesion es el mismo que
		  // el actual, es porque (ya se dio al meno una vuelta) y el puntero PUT esta
		  // apuntando justamente al registro de sesion de muuuuucho tiempo atras, como
		  // para pisarlo en una proxima escritura.
		  // => Para extraer los datos necesito que estos punteros sean distintos, por
		  // eso avanzo el de sesion
		  incFlashRep_ptr(&SESION_ptr);               // Avanzo puntero
		}
		recaudacion = 0;                              // Reseteo Variable
		nroTarifa = 0;                                // Reseteo Variable
		while (nroTarifa <= cantidadTarifasProgramables){
		  recaudacion += REPORTES_getImporte_byRango((tREG_GENERIC*far)SESION_ptr, ACTUAL_ptr, REG_apagar, nroTarifa); // Total IMPORTE en A PAGAR en Rango

		  nroTarifa++;
		}
		return(recaudacion);
	}


	/* EXTRAER CANTIDAD DE VIAJES */
	/******************************/



	uint16_t getViajes_Parcial(void){
		// Determina cantidad de viajes desde el ultimo inicio de turno (sesion mas reciente),
		// hasta ahora.
		tREG_SESION*far SESION_ptr;
		tREG_GENERIC*far ACTUAL_ptr;
		byte nroTarifa;
		uint16_t viajes;

		SESION_ptr = (tREG_SESION*far) REPORTES_getPrevRegister_byType(REG_sesion);   // Extraigo puntero SESION anterior
		ACTUAL_ptr = REPORTES_getPUT_ptr();           // Extraigo puntero actual

		if (SESION_ptr == ACTUAL_ptr){
		  // Si se diera el caso de que el puntero de inicio de sesion es el mismo que
		  // el actual, es porque (ya se dio al menos una vuelta) y el puntero PUT esta
		  // apuntando justamente al registro de sesion de muuuuucho tiempo atras, como
		  // para pisarlo en una proxima escritura.
		  // => Para extraer los datos necesito que estos punteros sean distintos, por
		  // eso avanzo el de sesion
		  incFlashRep_ptr(&SESION_ptr);               // Avanzo puntero
		}

		nroTarifa = 0;                                // Reseteo Variable
		viajes = 0;
		while (nroTarifa <= cantidadTarifasProgramables){
		  viajes += REPORTES_getCantidadViajes_byRango((tREG_GENERIC*far)SESION_ptr, ACTUAL_ptr, REG_apagar, nroTarifa); // Total de cantidad de viajes en A PAGAR
		  nroTarifa++;
		}

		return(viajes);
	}

	void REPORTES_cal_kml_kmo (uint32_t* datosMOV){

		uint32_t kmLIBRE, kmOCUPADO;
		byte TO_F;
		tREG_GENERIC aux_INI;
		tREG_GENERIC* aux_INI_ptr;
		byte pointer_ok;

		tREG_SESION*far SESION_ptr;
		tREG_GENERIC*far ACTUAL_ptr;

		SESION_ptr = (tREG_SESION*far) REPORTES_getPrevRegister_byType(REG_sesion);   // Extraigo puntero SESION anterior
		ACTUAL_ptr = REPORTES_getPUT_ptr();           // Extraigo puntero actual

		if (SESION_ptr == ACTUAL_ptr){
		  // Si se diera el caso de que el puntero de inicio de sesion es el mismo que
		  // el actual, es porque (ya se dio al menos una vuelta) y el puntero PUT esta
		  // apuntando justamente al registro de sesion de muuuuucho tiempo atras, como
		  // para pisarlo en una proxima escritura.
		  // => Para extraer los datos necesito que estos punteros sean distintos, por
		  // eso avanzo el de sesion
		  incFlashRep_ptr(&SESION_ptr);               // Avanzo puntero
		}

		kmLIBRE = 0;
		kmOCUPADO = 0;
		aux_INI_ptr = &aux_INI;
		pointer_ok = REPORTE_chkPointer(SESION_ptr);   // Verifico que el puntero de inicio sea correcto
		if(pointer_ok){
			 pointer_ok = REPORTE_chkPointer(ACTUAL_ptr); // Verifico que el puntero de fin sea correcto
		}

		TO_F = 0;
		dispararTO_lazo();
		while ((SESION_ptr != ACTUAL_ptr)){

			TO_F = chkTO_lazo_F();		//Chequeo bandera de time out de lazo
			//CHECK RANGO PUNTEROS
			if(checkRANGE(SESION_ptr, FIN_TABLA_REPORTE, TO_F )){break;}		//cuando INI_ptr > FIN_TABLA_REPORTE sale del while
			//READ EEPROM
			EEPROM_ReadBuffer(&aux_INI,SESION_ptr,sizeof(tREG_GENERIC));

			//CALCULOS LIBRE
			if (aux_INI.tipo == REG_libre){
			  //libre,ocupado,fuera de servicio
			  kmLIBRE += ((tREG_LIBRE*) aux_INI_ptr)->km;
			}

			//CALCULOS OCUPADO
			if (aux_INI.tipo == REG_ocupado){
			  //libre,ocupado,fuera de servicio
			  kmOCUPADO += ((tREG_OCUPADO*) aux_INI_ptr)->km;
			}
			//Avanzo puntero
			incFlashRep_ptr(&SESION_ptr);
		}
		detenerTO_lazo();
		datosMOV[0] = kmLIBRE;
		datosMOV[1] = kmOCUPADO;
	}


#endif

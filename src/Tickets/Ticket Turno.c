/*
 * Ticket Turno.c

 *
 *  Created on: 14/3/2018
 *      Author: Rai
 */
#include "- metroBLUE Config -.h"

#ifdef VISOR_REPORTES
#include "Ticket Turno.h"
#include "stdint.h"
#include "calculos.h"
#include "DA Tx.h"
//#include "Param Reloj.h"
#include "Parametros Reloj.h"

#include "Parametros Ticket.h"
#include "Reloj.h"
#include "Variables en flash.h"
#include "Common Ticket.h"
#include "Tarifacion Reloj.h"
#include "Reportes.h"




  // Tabla de estados de reloj a imprimir.
  // Contiene los tipos de registros a buscar, y el orden en el que se imprimen
  const byte TICKET_TURNO_estados[3]={
    REG_libre,
    REG_ocupado,
    REG_fserv
  };

  uint32_t getFichasDistancia_tarifa(byte nroTarifa);
  uint32_t getFichasTiempo_tarifa(byte nroTarifa);


byte estadosTurno;

    #define estadoTICKET            TICKET_TURNO_estados[estadosTurno]






tREG_SESION iniTURNO;
tREG_SESION finTURNO;
tREG_SESION* iniTURNO_ptr;      // Puntero a inicio de Turno del tipo SESION
tREG_SESION* finTURNO_ptr;      // Puntero a final de Turno del tipo SESION


tREG_SESION* sesion_ptrs[max_turnosReporte];

void print_ticket_turno(uint8_t nroTurno){

	uint8_t* ptrSimple;
	uint8_t** ptrDouble;
    uint8_t buffer_aux[20];
    uint16_t aux16;
    uint32_t aux32;
    uint16_t N_print;
    byte cantidad_de_sesion;
    tREG_GENERIC* REPORTE_PUTptr_aux;
    byte statusSESION;
    uint32_t datosMOV[4][3];
    uint32_t kmTotal, velMaxTotal, tMarchaTotal,tParadoTotal;
    byte puntoDECIMAL;

    if(TARIFA_PESOS){
		//muestra importe
		puntoDECIMAL = PUNTO_DECIMAL;
   	}else{
		//muestra ficha
   		puntoDECIMAL = 3;
   	}

  //if(ESTADO_RELOJ==FUERA_SERVICIO){
    if(statusPRINT==NO_HAY_IMPRESION_EN_PROCESO){
		statusPRINT = IMPRESION_EN_PROCESO;

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
		string_copy_incDest(ptrDouble,"       REPORTE DE TURNO");
		add_LF(ptrDouble);
		printLINE(ptrDouble);
		//add_LF(ptrDouble);
		//print nombre de empresa
		printEMPRESA(ptrDouble);

		//cantidad_de_sesion = REPORTES_getSesions(sesion_ptrs, 2);        				// Obtengo punteros a todos las sesiones
		cantidad_de_sesion = REPORTES_getTurno(sesion_ptrs, nroTurno, max_turnosReporte);
		if(cantidad_de_sesion ==0xff){
			//no hay viajes
			statusSESION = 1;
		}else if(cantidad_de_sesion ==0xfe){
			//ERROR
			statusSESION = 2;
		}else{
			statusSESION = 0;
		}
		if(REPORTE_NRO_TURNO>=1 && !statusSESION){

			iniTURNO_ptr_aux = sesion_ptrs[MENU_REPORTE_TURNO_index + 1];		 // Puntero a inicio de turno seleccionado
	        finTURNO_ptr_aux = sesion_ptrs[MENU_REPORTE_TURNO_index];			// Puntero a final de turno seleccionado

	        //toma datos desde eeprom del inicio y fin de sesion
			TICKET_TURNO_setIni(iniTURNO_ptr_aux, iniTURNO_ptr);
			TICKET_TURNO_setFin(finTURNO_ptr_aux, finTURNO_ptr);

			//print numero de turno
			string_copy_incDest(ptrDouble,"Nro Turno               ");
			preparar_print (iniTURNO_ptr->nroTurno, 0, &buffer_aux, 0);
			string_copy_incDest(ptrDouble,&buffer_aux);
			add_LF(ptrDouble);

			//print chofer
			string_copy_incDest(ptrDouble,"Chofer                  ");
			preparar_print (iniTURNO_ptr->chofer, 0, &buffer_aux, 0);
			string_copy_incDest(ptrDouble,&buffer_aux);
			add_LF(ptrDouble);

			//print date inicio de turno
			string_copy_incDest(ptrDouble,"Inicio     ");
			date_to_string(buffer_aux, iniTURNO_ptr->date);
			string_copy_incDest(ptrDouble,&buffer_aux);
			add_LF(ptrDouble);

			//print date fin de turno
			string_copy_incDest(ptrDouble,"Fin        ");
			date_to_string(buffer_aux, finTURNO_ptr->date);
			string_copy_incDest(ptrDouble,&buffer_aux);
			add_LF(ptrDouble);

			printLINE(ptrDouble);

			//printCABECERA(ptrDouble,0);

			//print datos

			(void)REPORTES_calVarios (iniTURNO_ptr_aux, finTURNO_ptr_aux, datosMOV);
			estadosTurno = 0;
			while(estadosTurno < (sizeof(TICKET_TURNO_estados))){
				if (TICKET_TURNO_estados[estadosTurno] == REG_libre){
					string_copy_incDest(ptrDouble,"LIBRE        ");
					add_LF(ptrDouble);
					kmTotal     = datosMOV[0][0];	velMaxTotal = datosMOV[1][0];
					tMarchaTotal= datosMOV[2][0];	tParadoTotal= datosMOV[3][0];

				}else if (TICKET_TURNO_estados[estadosTurno] == REG_ocupado){
					string_copy_incDest(ptrDouble,"OCUPADO      ");
					kmTotal     = datosMOV[0][1];	velMaxTotal = datosMOV[1][1];
					tMarchaTotal= datosMOV[2][1];	tParadoTotal= datosMOV[3][1];
					add_LF(ptrDouble);

				}else if (TICKET_TURNO_estados[estadosTurno] == REG_fserv){
					string_copy_incDest(ptrDouble,"FUERA DE SERVICIO      ");
					kmTotal     = datosMOV[0][2];	velMaxTotal = datosMOV[1][2];
					tMarchaTotal= datosMOV[2][2];	tParadoTotal= datosMOV[3][2];
					add_LF(ptrDouble);
				}

				//print DISTANCIA
				string_copy_incDest(ptrDouble,"  Distancia [Km]       ");
				//preparar_print (TURNO_getKM_estado(estadoTICKET), 1, &buffer_aux, 0);
				preparar_print (kmTotal, 1, &buffer_aux, 0);
				string_copy_incDest(ptrDouble,&buffer_aux);
				//string_copy_incDest(ptrDouble," Km");
				add_LF(ptrDouble);
				//print VELOCIDAD MAXIMA
				if(velMaxTotal >= 255){
					string_copy_incDest(ptrDouble,"  Vel: Max  [Km/h]     ");
					preparar_print (velMaxTotal, 0, &buffer_aux, 0);
					string_copy_incDest(ptrDouble,&buffer_aux);
					string_copy_incDest(ptrDouble,"+");
				}else{
					string_copy_incDest(ptrDouble,"  Vel: Max  [Km/h]      ");
					preparar_print (velMaxTotal, 0, &buffer_aux, 0);
					string_copy_incDest(ptrDouble,&buffer_aux);
				}
				add_LF(ptrDouble);

				//print TIEMPO MARCHA
				string_copy_incDest(ptrDouble,"  T. Marcha [hh:mm]      ");
				seconds_to_HMS(buffer_aux,tMarchaTotal);          // Convierto a H-M-S
				buffer_aux[5] = 0;						// Como solo me interesa imprimir HH:MM, pongo un fin de cadena
				string_copy_incDest(ptrDouble,&buffer_aux);
				add_LF(ptrDouble);

				//print TIEMPO Parado
				string_copy_incDest(ptrDouble,"  T. Parado [hh:mm]      ");
				seconds_to_HMS(buffer_aux,tParadoTotal);          // Convierto a H-M-S
				buffer_aux[5] = 0;						// Como solo me interesa imprimir HH:MM, pongo un fin de cadena
				string_copy_incDest(ptrDouble,&buffer_aux);
				add_LF(ptrDouble);

				add_LF(ptrDouble);

				estadosTurno++;
			}

			//print LINE
			printLINE(ptrDouble);
			tarifasTurno = tarifa1D;

			do{
				uint32_t cant_viajes = TURNO_getCantViajes_tarifa(tarifasTurno);
				if(cant_viajes){
					//print tarifas
					string_copy_incDest(ptrDouble,"TARIFA ");
					if (tarifasTurno < tarifa1N){
						convert_to_string(buffer_aux, tarifasTurno, 0xFF, base_DECIMAL);
						//preparar_print (tarifasTurno, 0, &buffer_aux, 0);
					}else{
						convert_to_string(buffer_aux, tarifasTurno - tarifa1N + 1, 0xFF, base_DECIMAL);
						//preparar_print (tarifasTurno - tarifa1N + 1, 0, &buffer_aux, 0);
					}
					string_copy_incDest(ptrDouble,&buffer_aux);

					if (tarifasTurno < tarifa1N){
						string_copy_incDest(ptrDouble,"D");
					}else{
						string_copy_incDest(ptrDouble,"N");
					}
					add_LF(ptrDouble);
					//print  Viajes
					string_copy_incDest(ptrDouble,"  Viajes                ");
					preparar_print (cant_viajes, 0, &buffer_aux, 0);
					string_copy_incDest(ptrDouble,&buffer_aux);
					add_LF(ptrDouble);
				//	if (TARIFA_FICHAS){
						//print Fichas
						string_copy_incDest(ptrDouble,"  Fichas                ");
						preparar_print (getFichasTiempo_tarifa(tarifasTurno) + getFichasDistancia_tarifa(tarifasTurno), 0, &buffer_aux, 0);
						string_copy_incDest(ptrDouble,&buffer_aux);
						add_LF(ptrDouble);
				//	}else{
						//print LINE
						//printLINE(ptrDouble);
				//	}

					//print Recaudacion
					if(puntoDECIMAL == 0){
						string_copy_incDest(ptrDouble,"  Recaudado [$]         ");
					}else{
						string_copy_incDest(ptrDouble,"  Recaudado [$]        ");
					}
					aux32 = TURNO_getRecaudacion_tarifa(&buffer_aux, tarifasTurno);
					//preparar_print (aux32, 3, &buffer_aux, 0 );
					//preparar_print (aux32, puntoDECIMAL, &buffer_aux, 0 );
					preparar_print_new (aux32, puntoDECIMAL, &buffer_aux, 0, ptrDouble);

					string_copy_incDest(ptrDouble,&buffer_aux);
					add_LF(ptrDouble);

				}
			}while(TICKET_pasarSiguienteTarifa(&tarifasTurno));

			//print LINE
			printLINE(ptrDouble);

			//print TOTALES
			string_copy_incDest(ptrDouble,"TOTALES");
			add_LF(ptrDouble);

			//print Viajes
			string_copy_incDest(ptrDouble,"  Viajes                ");
			aux32 = (uint32_t)TURNO_getCantViajes_turno();
			preparar_print (aux32, 0, &buffer_aux, 0 );
			string_copy_incDest(ptrDouble,&buffer_aux);

			add_LF(ptrDouble);


			//print viajes por asiento
			if (SENSOR_ASIENTO_NORM || SENSOR_ASIENTO_INTEL){
				string_copy_incDest(ptrDouble,"  Viajes asiento        ");
				aux32 = (uint32_t)TURNO_getCantViajesAsiento_turno();
				preparar_print (aux32, 0, &buffer_aux, 0 );
				string_copy_incDest(ptrDouble,&buffer_aux);
				add_LF(ptrDouble);
			}/*else{
				string_copy_incDest(ptrDouble,"  Viajes asiento        ");
				string_copy_incDest(ptrDouble,"N/A");
				add_LF(ptrDouble);
			}*/


			//print errores por asiento
			if (SENSOR_ASIENTO_INTEL){
				string_copy_incDest(ptrDouble,"  Errores asiento          ");
				aux32 = (uint32_t)TURNO_getCantErroresAsiento_turno();
				preparar_print (aux32, 0, &buffer_aux, 0 );
				string_copy_incDest(ptrDouble,&buffer_aux);
				add_LF(ptrDouble);
			}/*else{
				string_copy_incDest(ptrDouble,"  Errores asiento          ");
				string_copy_incDest(ptrDouble,"N/A");
				add_LF(ptrDouble);
			}*/

			//print recaudacion
			if(puntoDECIMAL == 0){
				string_copy_incDest(ptrDouble,"  Recaudado [$]         ");
			}else{
				string_copy_incDest(ptrDouble,"  Recaudado [$]        ");
			}
			aux32 = (uint32_t)TURNO_getRecaudacion_turno(&buffer_aux);
			//preparar_print (aux32, 3, &buffer_aux, 0 );
			preparar_print_new (aux32, puntoDECIMAL, &buffer_aux, 0, ptrDouble);
			string_copy_incDest(ptrDouble,&buffer_aux);
			add_LF(ptrDouble);

			//print Km totales
			string_copy_incDest(ptrDouble,"  Km Totales           ");
			aux32 = (uint32_t)TURNO_calcKmTotales_turno();
			preparar_print (aux32, 1, &buffer_aux, 0 );
			string_copy_incDest(ptrDouble,&buffer_aux);
			//string_copy_incDest(ptrDouble," Km");
			add_LF(ptrDouble);

			//print porcentaje chofer
			if (PROG_TICKET_percentChofer > 0){
				convert_to_string(buffer_aux, PROG_TICKET_percentChofer, 0, base_DECIMAL);
				//string_concat(buffer_aux, "% ");
				//string_FAR_concat(PRINT_BUFFER, _Chofer);

				//aux32 = (uint32_t)PROG_TICKET_percentChofer;
				//preparar_print (aux32, 0, &buffer_aux, 0 );
				string_copy_incDest(ptrDouble,"  ");
				string_copy_incDest(ptrDouble,&buffer_aux);
				string_copy_incDest(ptrDouble,"%");
				if(puntoDECIMAL == 0){
					string_copy_incDest(ptrDouble," Chofer [$]      ");
				}else{
					string_copy_incDest(ptrDouble," Chofer [$]     ");
				}
				string_copy_incDest(ptrDouble,"  ");
				aux32 = (uint32_t)TURNO_calcRecaudacionChofer(buffer_aux);
				//preparar_print (aux32, puntoDECIMAL, &buffer_aux, 0 );
				preparar_print_new (aux32, puntoDECIMAL, &buffer_aux, 0, ptrDouble);
				//convert_to_string(buffer_aux, aux32, 0xFF, base_DECIMAL);
				string_copy_incDest(ptrDouble,&buffer_aux);
				add_LF(ptrDouble);
			}/*else{
				string_copy_incDest(ptrDouble," %");
				string_copy_incDest(ptrDouble," Chofer:                 ");
				string_copy_incDest(ptrDouble,"N/A");
				add_LF(ptrDouble);
			}*/

			//print LINE
			printLINE(ptrDouble);

			//print recaudacion por km
			string_copy_incDest(ptrDouble,"$ x km              ");
			string_copy_incDest(ptrDouble,"   ");
			aux32 = (uint32_t)TURNO_calcRecaudacionPorKm_turno(buffer_aux);
			preparar_print_new (aux32, puntoDECIMAL, &buffer_aux, 0, ptrDouble );
			//convert_to_string(buffer_aux, aux32, 0xFF, base_DECIMAL);
			string_copy_incDest(ptrDouble,&buffer_aux);
			add_LF(ptrDouble);

			//print porcentaje de  km en ocupado
			string_copy_incDest(ptrDouble,"%km Ocupado             ");
			aux32 = (uint32_t)TURNO_calcPercentKmOcupado_turno(buffer_aux);
			aux32 = aux32/100;
			preparar_print (aux32, 0, &buffer_aux, 0 );
			string_copy_incDest(ptrDouble,&buffer_aux);
			//string_copy_incDest(ptrDouble,"%");
			add_LF(ptrDouble);

			//print line
			printLINE(ptrDouble);

			//print cantidad de desconexiones de alimentacion
			string_copy_incDest(ptrDouble,"Desconex. alim.         ");
			aux32 = (uint32_t)TURNO_getCantDesconexionesAlim_turno();
			preparar_print (aux32, 0, &buffer_aux, 0 );
			string_copy_incDest(ptrDouble,&buffer_aux);
			add_LF(ptrDouble);

			//print tiempo de desconexiones de alimentacion
			string_copy_incDest(ptrDouble,"Tiempo [hh:mm:ss]     ");
			//aux32 = (uint32_t)TURNO_calcTiempoDesconexionAlim_turno(&buffer_aux);
			//preparar_print (aux32, EqPesosD.puntoDECIMAL, &buffer_aux, 0 );
			(void)TURNO_calcTiempoDesconexionAlim_turno(buffer_aux);
			string_copy_incDest(ptrDouble,&buffer_aux);
			add_LF(ptrDouble);

			//print importe perdido
			if(puntoDECIMAL == 0){
				string_copy_incDest(ptrDouble,"Perdidos [$]          ");
			}else{
				string_copy_incDest(ptrDouble,"$ Perdidos             ");
			}
			aux32 = (uint32_t)TURNO_calcImportePerdido_turno(&buffer_aux);
			//preparar_print (aux32, puntoDECIMAL, &buffer_aux, 0 );
			preparar_print_new (aux32, puntoDECIMAL, &buffer_aux, 0, ptrDouble);
			string_copy_incDest(ptrDouble,&buffer_aux);
			add_LF(ptrDouble);

			/*
			//print Movimiento sin pulsos
			string_copy_incDest(ptrDouble,"Mov s/Pulsos                 ");
			convert_to_string(buffer_aux, TURNO_getCantMovSinPulsos_turno(), 0xFF, base_DECIMAL);
			string_copy_incDest(ptrDouble,&buffer_aux);
			add_LF(ptrDouble);
            */
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

		}else{
			if(statusSESION == 2){
				string_copy_incDest(ptrDouble,"   ERROR EN REPORTE DE TURNO");
				add_LF(ptrDouble);
				printLINE(ptrDouble);
			}else{
				string_copy_incDest(ptrDouble,"TODAVIA NO FINALIZO NINGUN TURNO");
				add_LF(ptrDouble);
				printLINE(ptrDouble);
			}

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
 // }
 }

/* SETEAR PUNTERO DE INICIO DE TURNO */
/*************************************/
 /*
  void TICKET_TURNO_setIni (tREG_SESION* ptr){
    iniTURNO_ptr = ptr;
  }*/
  void TICKET_TURNO_setIni (tREG_SESION* addr_iniSESION, tREG_SESION* reg_iniSESION ){
	  uint32_t addr;
	  addr = (uint32_t)addr_iniSESION;

	  EEPROM_ReadBuffer(reg_iniSESION,(uint32_t)addr_iniSESION,sizeof(tREG_SESION));
  }


/* SETEAR PUNTERO DE FIN DE TURNO */
/**********************************/
/*
 void TICKET_TURNO_setFin (tREG_SESION* ptr){
   finTURNO_ptr = ptr;
 }*/

 void TICKET_TURNO_setFin (tREG_SESION* addr_finSESION, tREG_SESION* reg_finSESION ){
	 uint32_t addr;
	 addr = (uint32_t)addr_finSESION;

	 EEPROM_ReadBuffer(reg_finSESION,(uint32_t)addr_finSESION,sizeof(tREG_SESION));
  }


  /* EXTRAER KM RECORRIDOS EN ESTADO DE RELOJ */
  /********************************************/
    uint32_t TURNO_getKM_estado(byte tipo){
      uint32_t km;

      //tREG_SESION* sesion_ptrs[max_turnosReporte];

      tREG_SESION* iniTURNO_ptr_aux;      // Puntero a inicio de Turno del tipo SESION
      tREG_SESION* finTURNO_ptr_aux;      // Puntero a final de Turno del tipo SESION

      //(void)REPORTES_getSesions(sesion_ptrs, max_turnosReporte);        				// Obtengo punteros a todos las sesiones
      iniTURNO_ptr_aux = sesion_ptrs[MENU_REPORTE_TURNO_index + 1];
      finTURNO_ptr_aux = sesion_ptrs[MENU_REPORTE_TURNO_index];

		//inicializo punteros a sesiones

		TICKET_TURNO_setIni(sesion_ptrs[MENU_REPORTE_TURNO_index + 1], iniTURNO_ptr);   // Puntero a inicio de turno seleccionado
		TICKET_TURNO_setFin(sesion_ptrs[MENU_REPORTE_TURNO_index], finTURNO_ptr);       // Puntero a final de turno seleccionado

		km = REPORTES_getKmRecorridos_byRango((tREG_GENERIC*)iniTURNO_ptr_aux, (tREG_GENERIC*)finTURNO_ptr_aux, tipo);

		return(km);
    }


  /* EXTRAER VELOCIDAD MAXIMA DEL VIAJE */
  /**************************************/
    byte TURNO_getVelMax_estado(byte tipo){

    	byte vel;

		//tREG_SESION* sesion_ptrs[max_turnosReporte];
		tREG_SESION* iniTURNO_ptr_aux;      // Puntero a inicio de Turno del tipo SESION
		tREG_SESION* finTURNO_ptr_aux;      // Puntero a final de Turno del tipo SESION

           // (void)REPORTES_getSesions(sesion_ptrs, max_turnosReporte);        				// Obtengo punteros a todos las sesiones
            iniTURNO_ptr_aux = sesion_ptrs[MENU_REPORTE_TURNO_index + 1];
            finTURNO_ptr_aux = sesion_ptrs[MENU_REPORTE_TURNO_index];

      vel = REPORTES_getVelMax_byRango((tREG_GENERIC*)iniTURNO_ptr_aux, (tREG_GENERIC*)finTURNO_ptr_aux, tipo);

      return(vel);
    }


  /* EXTRAER TIEMPO MARCHA EN EL TURNO */
  /*************************************/
    void TURNO_getTiempoMarcha_estado(byte* buffer, byte tipo){
	uint32_t tiempo;

	//tREG_SESION* sesion_ptrs[max_turnosReporte];
	tREG_SESION* iniTURNO_ptr_aux;      // Puntero a inicio de Turno del tipo SESION
	tREG_SESION* finTURNO_ptr_aux;      // Puntero a final de Turno del tipo SESION

	//(void)REPORTES_getSesions(sesion_ptrs, max_turnosReporte);        				// Obtengo punteros a todos las sesiones
	iniTURNO_ptr_aux = sesion_ptrs[MENU_REPORTE_TURNO_index + 1];
	finTURNO_ptr_aux = sesion_ptrs[MENU_REPORTE_TURNO_index];

	tiempo = REPORTES_getTiempoMarcha_byRango((tREG_GENERIC*)iniTURNO_ptr_aux, (tREG_GENERIC*)finTURNO_ptr_aux, tipo);

	seconds_to_HMS(buffer,tiempo);          // Convierto a H-M-S

	// Como solo me interesa imprimir HH:MM, pongo un fin de cadena
	buffer[5] = 0;
    }


  /* EXTRAER TIEMPO PARADO EN EL TURNO */
  /*************************************/
    void TURNO_getTiempoParado_estado(byte* buffer, byte tipo){
    uint32_t tiempo;
  	//tREG_SESION* sesion_ptrs[max_turnosReporte];
  	tREG_SESION* iniTURNO_ptr_aux;      // Puntero a inicio de Turno del tipo SESION
  	tREG_SESION* finTURNO_ptr_aux;      // Puntero a final de Turno del tipo SESION

  	//(void)REPORTES_getSesions(sesion_ptrs, max_turnosReporte);        				// Obtengo punteros a todos las sesiones
  	iniTURNO_ptr_aux = sesion_ptrs[MENU_REPORTE_TURNO_index + 1];
  	finTURNO_ptr_aux = sesion_ptrs[MENU_REPORTE_TURNO_index];

      tiempo = REPORTES_getTiempoParado_byRango((tREG_GENERIC*)iniTURNO_ptr_aux, (tREG_GENERIC*)finTURNO_ptr_aux, tipo);

      seconds_to_HMS(buffer,tiempo);          // Convierto a H-M-S

      buffer[5] = 0;
    }


  /* EXTRAER CANTIDAD DE VIAJES EN TARIFA */
  /****************************************/
    uint16_t TURNO_getCantViajes_tarifa(byte nroTarifa){
    uint16_t viajes;
	//tREG_SESION* sesion_ptrs[max_turnosReporte];
	tREG_SESION* iniTURNO_ptr_aux;      // Puntero a inicio de Turno del tipo SESION
	tREG_SESION* finTURNO_ptr_aux;      // Puntero a final de Turno del tipo SESION

      //(void)REPORTES_getSesions(sesion_ptrs, max_turnosReporte);        				// Obtengo punteros a todos las sesiones
       iniTURNO_ptr_aux = sesion_ptrs[MENU_REPORTE_TURNO_index + 1];
       finTURNO_ptr_aux = sesion_ptrs[MENU_REPORTE_TURNO_index];

      viajes = REPORTES_getCantidadViajes_byRango((tREG_GENERIC*)iniTURNO_ptr_aux, (tREG_GENERIC*)finTURNO_ptr_aux, REG_apagar, nroTarifa); // Total de cantidad de viajes en A PAGAR

      return(viajes);
    }


  /* EXTRAER CANTIDAD DE FICHAS DE TIEMPO EN TARIFA */
  /**************************************************/
    uint32_t getFichasTiempo_tarifa(byte nroTarifa){
    uint32_t fichas;
  	//tREG_SESION* sesion_ptrs[max_turnosReporte];
  	tREG_SESION* iniTURNO_ptr_aux;      // Puntero a inicio de Turno del tipo SESION
  	tREG_SESION* finTURNO_ptr_aux;      // Puntero a final de Turno del tipo SESION

  	//(void)REPORTES_getSesions(sesion_ptrs, max_turnosReporte);        				// Obtengo punteros a todos las sesiones
  	iniTURNO_ptr_aux = sesion_ptrs[MENU_REPORTE_TURNO_index + 1];
  	finTURNO_ptr_aux = sesion_ptrs[MENU_REPORTE_TURNO_index];


      fichas = REPORTES_getFichasTime_byRango((tREG_GENERIC*)iniTURNO_ptr_aux, (tREG_GENERIC*)finTURNO_ptr_aux, REG_apagar, nroTarifa); // Total de cantidad de viajes en A PAGAR

      return(fichas);
    }


  /* EXTRAER CANTIDAD DE FICHAS DE DISTANCIA EN TARIFA */
  /*****************************************************/
    uint32_t getFichasDistancia_tarifa(byte nroTarifa){
    uint32_t fichas;
  	//tREG_SESION* sesion_ptrs[max_turnosReporte];
  	tREG_SESION* iniTURNO_ptr_aux;      // Puntero a inicio de Turno del tipo SESION
  	tREG_SESION* finTURNO_ptr_aux;      // Puntero a final de Turno del tipo SESION

  	//(void)REPORTES_getSesions(sesion_ptrs, max_turnosReporte);        				// Obtengo punteros a todos las sesiones
  	iniTURNO_ptr_aux = sesion_ptrs[MENU_REPORTE_TURNO_index + 1];
  	finTURNO_ptr_aux = sesion_ptrs[MENU_REPORTE_TURNO_index];


      fichas = REPORTES_getFichasDist_byRango((tREG_GENERIC*)iniTURNO_ptr_aux, (tREG_GENERIC*)finTURNO_ptr_aux, REG_apagar, nroTarifa); // Total de cantidad de viajes en A PAGAR

      return(fichas);
    }


  /* EXTRAER RECAUDACION EN TARIFA */
  /*********************************/
    uint32_t TURNO_getRecaudacion_tarifa(byte* buffer, byte nroTarifa){
      // El calculo de la recaudacion depende de si trabaja con PESOS o con FICHAS.
      // Para el caso con fichas, sólo tiene sentido si tiene habilitada la Eq en Pesos
      //
      // Si el buffer es NULL, no lo convierte a string ni lo modifica.
      //
      // Devuelve la recaudacion de la tarifa.Si no tiene programada la Eq Pesos devuelve 0
    uint32_t recaudacion;
  //	tREG_SESION* sesion_ptrs[max_turnosReporte];
  	tREG_SESION* iniTURNO_ptr_aux;      											// Puntero a inicio de Turno del tipo SESION
  	tREG_SESION* finTURNO_ptr_aux;      											// Puntero a final de Turno del tipo SESION

  	//(void)REPORTES_getSesions(sesion_ptrs, max_turnosReporte);        				// Obtengo punteros a todos las sesiones
  	iniTURNO_ptr_aux = sesion_ptrs[MENU_REPORTE_TURNO_index + 1];
  	finTURNO_ptr_aux = sesion_ptrs[MENU_REPORTE_TURNO_index];

      recaudacion = REPORTES_getImporte_byRango((tREG_GENERIC*)iniTURNO_ptr_aux, (tREG_GENERIC*)finTURNO_ptr_aux, REG_apagar, nroTarifa); // Total IMPORTE en A PAGAR en Rango

      TICKET_importeToString(recaudacion, buffer);

      return(recaudacion);
    }


  /* EXTRAER CANTIDAD DE VIAJES DEL TURNO */
  /****************************************/
    uint16_t TURNO_getCantViajes_turno(void){
    uint16_t viajes;
    byte i;
  //	tREG_SESION* sesion_ptrs[max_turnosReporte];
  	tREG_SESION* iniTURNO_ptr_aux;      // Puntero a inicio de Turno del tipo SESION
  	tREG_SESION* finTURNO_ptr_aux;      // Puntero a final de Turno del tipo SESION

  	//(void)REPORTES_getSesions(sesion_ptrs, max_turnosReporte);        				// Obtengo punteros a todos las sesiones
  	iniTURNO_ptr_aux = sesion_ptrs[MENU_REPORTE_TURNO_index + 1];
  	finTURNO_ptr_aux = sesion_ptrs[MENU_REPORTE_TURNO_index];


      viajes = 0;
      i = 1;
      while (i <= cantidadTarifasProgramables){
        viajes += REPORTES_getCantidadViajes_byRango((tREG_GENERIC*)iniTURNO_ptr_aux, (tREG_GENERIC*)finTURNO_ptr_aux, REG_apagar, i);
        i++;
      }

      return(viajes);
    }


  /* EXTRAER CANTIDAD DE VIAJES (POR SENSOR DE ASIENTO) DEL TURNO */
  /****************************************************************/
    uint16_t TURNO_getCantViajesAsiento_turno(void){
    uint16_t viajes;
    uint16_t turnos;
  //	tREG_SESION* sesion_ptrs[max_turnosReporte];
  	tREG_SESION* iniTURNO_ptr_aux;      // Puntero a inicio de Turno del tipo SESION
  	tREG_SESION* finTURNO_ptr_aux;      // Puntero a final de Turno del tipo SESION

  //	(void)REPORTES_getSesions(sesion_ptrs, max_turnosReporte);        				// Obtengo punteros a todos las sesiones
  	iniTURNO_ptr_aux = sesion_ptrs[MENU_REPORTE_TURNO_index + 1];
  	finTURNO_ptr_aux = sesion_ptrs[MENU_REPORTE_TURNO_index];

      viajes = REPORTES_getCantidadViajesAsiento_byRango_Value((tREG_GENERIC*)iniTURNO_ptr_aux, (tREG_GENERIC*)finTURNO_ptr_aux, sensorAsiento_pressed);
      turnos = TURNO_getCantErroresAsiento_turno();                // Agrego viajes por error
      viajes += turnos;
      return(viajes);
    }


  /* EXTRAER CANTIDAD DE ERRORES DE SENSOR DE ASIENTO DEL TURNO */
  /**************************************************************/
    uint16_t TURNO_getCantErroresAsiento_turno(void){
      uint16_t viajes;

      viajes = REPORTES_getCantidadViajesAsiento_byRango_Value((tREG_GENERIC*)iniTURNO_ptr_aux, (tREG_GENERIC*)finTURNO_ptr_aux, sensorAsiento_error);

      return(viajes);
    }


  /* EXTRAER RECAUDACION EN TURNO */
  /********************************/
    uint32_t TURNO_getRecaudacion_turno(byte* buffer){
      // El calculo de la recaudacion depende de si trabaja con PESOS o con FICHAS.
      // Para el caso con fichas, sólo tiene sentido si tiene habilitada la Eq en Pesos
      //
      // Si el buffer es NULL, no lo convierte a string ni lo modifica.
      //
      // Devuelve la recaudacion de la tarifa. 0 es el valor de error si no
      // tiene programada la Eq Pesos
      uint32_t recaudacion;
      byte i;

      recaudacion = 0;
      i = 1;
      while(i <= cantidadTarifasProgramables){
        recaudacion += TURNO_getRecaudacion_tarifa(NULL,i);
        i++;
      }

       TICKET_importeToString(recaudacion, buffer);

      return(recaudacion);
    }


  /* CALCULAR KM TOTALES DEL TURNO */
  /*********************************/
    uint32_t TURNO_calcKmTotales_turno(void){
      // Calcula el total de KM recorridos
      uint32_t km;

      km = TURNO_getKM_estado(REG_libre);
      km += TURNO_getKM_estado(REG_ocupado);
      km += TURNO_getKM_estado(REG_fserv);

      return(km);
    }


  /* CALCULAR RECAUDACION CHOFER */
  /*******************************/
    uint32_t TURNO_calcRecaudacionChofer(byte* buffer){
      // Calcula la recaudacion que le corresponde al chofer, la convierte a string y la devuelve
      // en el buffer.
      // Devuelve la recaudacion. 0 es el valor de error si no tiene programada la Eq Pesos
      uint32_t recaudacion;
      uint32_t recaudChofer;

      recaudacion = TURNO_getRecaudacion_turno(NULL);
      recaudChofer = (recaudacion * PROG_TICKET_percentChofer) / 100;

      TICKET_importeToString(recaudChofer, buffer);

      return(recaudChofer);
    }


  /* CALCULAR RECAUDACION POR KM TURNO */
  /*************************************/
    uint32_t TURNO_calcRecaudacionPorKm_turno(byte* buffer){
      // Calcula la recaudacion por KM recorrido, la convierte a string y la devuelve
      // en el buffer.
      // Devuelve la recaudacion. 0 es el valor de error si no tiene programada la Eq Pesos
      uint32_t recaudacion;
      uint32_t km;
      uint32_t pesosXkm;

      recaudacion = TURNO_getRecaudacion_turno(NULL);
      km = TURNO_getKM_estado(REG_libre);
      km += TURNO_getKM_estado(REG_ocupado);
      km += TURNO_getKM_estado(REG_fserv);
      pesosXkm = recaudacion*10;
      pesosXkm /= km;                             // Como los KM tiene 1 decimal, multiplico x10

      TICKET_importeToString(pesosXkm, buffer);

      return(pesosXkm);
    }


  /* CALCULAR PROCENTAJE DE KM OCUPADO EN TURNO */
  /**********************************************/
    uint32_t TURNO_calcPercentKmOcupado_turno(byte* buffer){
      // Calcula el porncentaje de KM ocupados, la convierte a string y la devuelve
      // en el buffer.
      // Devuelve el porcentaje
      uint32_t km_ocupado;
      uint32_t km;
      uint32_t percent;

      km_ocupado = TURNO_getKM_estado(REG_ocupado);
      km = TURNO_getKM_estado(REG_libre);
      km += km_ocupado;
      km += TURNO_getKM_estado(REG_fserv);
      percent = ((km_ocupado * 100)*100) / km;

      if (buffer != NULL){
        convert_to_string_with_decimals(buffer, percent, 0xFF, 2, base_DECIMAL);
        string_concat(buffer, "%");
      }

      return(percent);
    }


  /* EXTRAER CANTIDAD DE DESCONEXIONES DE ALIMENTACION DEL TURNO */
  /***************************************************************/
    uint16_t TURNO_getCantDesconexionesAlim_turno(void){
    uint16_t desconexAlim;
    byte i;
  //	tREG_SESION* sesion_ptrs[max_turnosReporte];
  	tREG_SESION* iniTURNO_ptr_aux;      // Puntero a inicio de Turno del tipo SESION
  	tREG_SESION* finTURNO_ptr_aux;      // Puntero a final de Turno del tipo SESION

  	//(void)REPORTES_getSesions(sesion_ptrs, max_turnosReporte);        				// Obtengo punteros a todos las sesiones
  	iniTURNO_ptr_aux = sesion_ptrs[MENU_REPORTE_TURNO_index + 1];
  	finTURNO_ptr_aux = sesion_ptrs[MENU_REPORTE_TURNO_index];


      desconexAlim = 0;
      // EDIT 14/11/12
      //  i empieza de CERO, xq barre las tarifas => Antes de que pase a OCUPADO por 1ra vez, la tarifa
      // es CERO. Si no pongo esto => no cuentas esas desconexiones
      i = 1;
      while (i <= cantidadTarifasProgramables){
        desconexAlim += REPORTES_getCantidadViajes_byRango((tREG_GENERIC*)iniTURNO_ptr_aux, (tREG_GENERIC*)finTURNO_ptr_aux, REG_desconexAlim, i);
        i++;
      }

      return(desconexAlim);
    }



  /* CALCULAR TIEMPO DESCONEXION ALIMENTACION EN TURNO */
  /*****************************************************/
    uint32_t TURNO_calcTiempoDesconexionAlim_turno(byte* buffer){
      // Calcula el tiempo que la alimentacion estuvo deconectada en el turno, la convierte a string y la devuelve
      // en el buffer.
    uint32_t tiempoDesconectado;
  	//tREG_SESION* sesion_ptrs[max_turnosReporte];
  	tREG_SESION* iniTURNO_ptr_aux;      // Puntero a inicio de Turno del tipo SESION
  	tREG_SESION* finTURNO_ptr_aux;      // Puntero a final de Turno del tipo SESION

  	//(void)REPORTES_getSesions(sesion_ptrs, max_turnosReporte);        				// Obtengo punteros a todos las sesiones
  	iniTURNO_ptr_aux = sesion_ptrs[MENU_REPORTE_TURNO_index + 1];
  	finTURNO_ptr_aux = sesion_ptrs[MENU_REPORTE_TURNO_index];

      tiempoDesconectado = REPORTES_getTiempoDesconexionAlim_byRango((tREG_GENERIC*)iniTURNO_ptr_aux, (tREG_GENERIC*)finTURNO_ptr_aux); // Total TIEMPO en DESCONEXIONES ALIM

      if (buffer != NULL){
        seconds_to_HMS(buffer, tiempoDesconectado);
      }

      return(tiempoDesconectado);
    }


    uint32_t TURNO_calcTiempoDesconexionAlim_turno_HEX(byte* buffer){
      // Calcula el tiempo que la alimentacion estuvo deconectada en el turno, la convierte a string y la devuelve
      // en el buffer.
    uint32_t tiempoDesconectado;
  	//tREG_SESION* sesion_ptrs[max_turnosReporte];
  	tREG_SESION* iniTURNO_ptr_aux;      // Puntero a inicio de Turno del tipo SESION
  	tREG_SESION* finTURNO_ptr_aux;      // Puntero a final de Turno del tipo SESION

  	//(void)REPORTES_getSesions(sesion_ptrs, max_turnosReporte);        				// Obtengo punteros a todos las sesiones
  	iniTURNO_ptr_aux = sesion_ptrs[MENU_REPORTE_TURNO_index + 1];
  	finTURNO_ptr_aux = sesion_ptrs[MENU_REPORTE_TURNO_index];

      tiempoDesconectado = REPORTES_getTiempoDesconexionAlim_byRango((tREG_GENERIC*)iniTURNO_ptr_aux, (tREG_GENERIC*)finTURNO_ptr_aux); // Total TIEMPO en DESCONEXIONES ALIM

      if (buffer != NULL){
        seconds_to_HMS_HEX(buffer, tiempoDesconectado);
      }

      return(tiempoDesconectado);
    }



  /* EXTRAER IMPORTE PERDIDO EN TARIFA */
  /*************************************/
    uint32_t TURNO_getImportePerdido_tarifa(byte* buffer, byte nroTarifa){
      // El calculo del importe perdido depende de si trabaja con PESOS o con FICHAS.
      // Para el caso con fichas, sólo tiene sentido si tiene habilitada la Eq en Pesos
      //
      // Si el buffer es NULL, no lo convierte a string ni lo modifica.
      //
      // Devuelve el importe perdido en la tarifa. Si no tiene programada la Eq Pesos devuelve 0
      uint32_t importePerdido;
  //	tREG_SESION* sesion_ptrs[max_turnosReporte];
  	tREG_SESION* iniTURNO_ptr_aux;      // Puntero a inicio de Turno del tipo SESION
  	tREG_SESION* finTURNO_ptr_aux;      // Puntero a final de Turno del tipo SESION

  	//(void)REPORTES_getSesions(sesion_ptrs, max_turnosReporte);        				// Obtengo punteros a todos las sesiones
  	iniTURNO_ptr_aux = sesion_ptrs[MENU_REPORTE_TURNO_index + 1];
  	finTURNO_ptr_aux = sesion_ptrs[MENU_REPORTE_TURNO_index];

      importePerdido = REPORTES_getImporte_byRango((tREG_GENERIC*)iniTURNO_ptr_aux, (tREG_GENERIC*)finTURNO_ptr_aux, REG_desconexAlim, nroTarifa); // Total IMPORTE en DESCONEXIONES ALIM

      return(importePerdido);
    }





  /* CALCULAR IMPORTE PERDIDO EN TURNO */
  /*************************************/
    uint32_t TURNO_calcImportePerdido_turno(byte* buffer){
      // Calcula el importe perdido del turno, la convierte a string y la devuelve
      // en el buffer.
      // Devuelve este importe perdido. 0 es el valor de error si no tiene programada la Eq Pesos
      uint32_t importePerdido;
      byte i;

      importePerdido = 0;
      i = 1;
      while(i <= cantidadTarifasProgramables){
        importePerdido += TURNO_getImportePerdido_tarifa(NULL,i);
        i++;
      }

      TICKET_importeToString(importePerdido, buffer);

      return(importePerdido);
    }



  /* EXTRAER CANTIDAD DE DESCONEXIONES DE ACCESORIO DEL TURNO */
  /************************************************************/
    uint16_t TURNO_getCantDesconexionesAcc_turno(void){
      uint16_t desconexAcc;
  	//tREG_SESION* sesion_ptrs[max_turnosReporte];
  	tREG_SESION* iniTURNO_ptr_aux;      // Puntero a inicio de Turno del tipo SESION
  	tREG_SESION* finTURNO_ptr_aux;      // Puntero a final de Turno del tipo SESION

  //	(void)REPORTES_getSesions(sesion_ptrs, max_turnosReporte);        				// Obtengo punteros a todos las sesiones
  	iniTURNO_ptr_aux = sesion_ptrs[MENU_REPORTE_TURNO_index + 1];
  	finTURNO_ptr_aux = sesion_ptrs[MENU_REPORTE_TURNO_index];

      desconexAcc = REPORTES_getCantidadDesconexionesAcc_byRango((tREG_GENERIC*)iniTURNO_ptr_aux, (tREG_GENERIC*)finTURNO_ptr_aux);

      return(desconexAcc);
    }




  /* CALCULAR TIEMPO DESCONEXION ACCESORIO EN TURNO */
  /**************************************************/
    uint32_t TURNO_calcTiempoDesconexionAcc_turno(byte* buffer){
      // Calcula el tiempo que el accesorio estuvo deconectado en el turno, la convierte a string y la devuelve
      // en el buffer.
    uint32_t tiempoDesconectado;
  	//tREG_SESION* sesion_ptrs[max_turnosReporte];
  	tREG_SESION* iniTURNO_ptr_aux;      // Puntero a inicio de Turno del tipo SESION
  	tREG_SESION* finTURNO_ptr_aux;      // Puntero a final de Turno del tipo SESION

  	//(void)REPORTES_getSesions(sesion_ptrs, max_turnosReporte);        				// Obtengo punteros a todos las sesiones
  	iniTURNO_ptr_aux = sesion_ptrs[MENU_REPORTE_TURNO_index + 1];
  	finTURNO_ptr_aux = sesion_ptrs[MENU_REPORTE_TURNO_index];

      tiempoDesconectado = REPORTES_getTiempoDesconexionAcc_byRango((tREG_GENERIC*)iniTURNO_ptr_aux, (tREG_GENERIC*)finTURNO_ptr_aux); // Total TIEMPO en DESCONEXIONES ALIM

      if (buffer != NULL){
        seconds_to_HMS(buffer, tiempoDesconectado);
      }

      return(tiempoDesconectado);
    }




  /* EXTRAER IMPORTE PERDIDO POR DESCONEXION DE ACCESORIO EN TARIFA */
  /******************************************************************/
    uint32_t TURNO_getImportePerdidoAcc_tarifa(byte* buffer, byte nroTarifa){
      // El calculo del importe perdido por desconexion del accesorio depende de si trabaja
      // con PESOS o con FICHAS.
      // Para el caso con fichas, sólo tiene sentido si tiene habilitada la Eq en Pesos
      //
      // Si el buffer es NULL, no lo convierte a string ni lo modifica.
      //
      // Devuelve el importe perdido en la tarifa. Si no tiene programada la Eq Pesos devuelve 0
    uint32_t importePerdido;

  //tREG_SESION* sesion_ptrs[max_turnosReporte];
  	tREG_SESION* iniTURNO_ptr_aux;      // Puntero a inicio de Turno del tipo SESION
  	tREG_SESION* finTURNO_ptr_aux;      // Puntero a final de Turno del tipo SESION

  //	(void)REPORTES_getSesions(sesion_ptrs, max_turnosReporte);        				// Obtengo punteros a todos las sesiones
  	iniTURNO_ptr_aux = sesion_ptrs[MENU_REPORTE_TURNO_index + 1];
  	finTURNO_ptr_aux = sesion_ptrs[MENU_REPORTE_TURNO_index];

      importePerdido = REPORTES_getImporte_byRango((tREG_GENERIC*)iniTURNO_ptr_aux, (tREG_GENERIC*)finTURNO_ptr_aux, REG_desconexAcc, nroTarifa); // Total IMPORTE en DESCONEXIONES ACC

      return(importePerdido);
    }


  /* EXTRAER CANTIDAD DE MOVIMIENTO SIN PULSOS DEL TURNO */
  /*******************************************************/
    uint16_t TURNO_getCantMovSinPulsos_turno(void){
		uint16_t veces;
		//tREG_SESION* sesion_ptrs[max_turnosReporte];
		tREG_SESION* iniTURNO_ptr_aux;      // Puntero a inicio de Turno del tipo SESION
		tREG_SESION* finTURNO_ptr_aux;      // Puntero a final de Turno del tipo SESION

		//(void)REPORTES_getSesions(sesion_ptrs, max_turnosReporte);        				// Obtengo punteros a todos las sesiones
		iniTURNO_ptr_aux = sesion_ptrs[MENU_REPORTE_TURNO_index + 1];
		finTURNO_ptr_aux = sesion_ptrs[MENU_REPORTE_TURNO_index];

		veces = REPORTES_getCantidadMovSinPulsos_byRango((tREG_GENERIC*)iniTURNO_ptr_aux, (tREG_GENERIC*)finTURNO_ptr_aux);

		return(veces);
    }

  /* CALCULAR IMPORTE PERDIDO POR DESCONEXION DE ACCESORIO EN TURNO */
  /******************************************************************/
    uint32_t TURNO_calcImportePerdidoAcc_turno(byte* buffer){
      // Calcula el importe perdido por desconexion de accesorio del turno, la convierte a string y la devuelve
      // en el buffer.
      // Devuelve este importe perdido. 0 es el valor de error si no tiene programada la Eq Pesos
      uint32_t importePerdido;
      byte i;

      importePerdido = 0;
      i = 1;
      while(i <= cantidadTarifasProgramables){
        importePerdido += TURNO_getImportePerdidoAcc_tarifa(NULL,i);
        i++;
      }

      TICKET_importeToString(importePerdido, buffer);

      return(importePerdido);
    }

#ifdef    DESCONEXIONES_TOTALES_EN_TICKET_TURNO

  /* EXTRAER CANTIDAD DE DESCONEXIONES DE ALIMENTACION TOTAL */
  /*********************************************************/
    uint16_t TURNO_getCantDesconexionesAlim_TOTAL(void){
      uint16_t desconexAlimTOTAL;
      byte i;

      tREG_SESION* lastSesion[1];
      tREG_GENERIC* lastSesion_ptr;
      tREG_GENERIC* iniTable_ptr;

      iniTable_ptr = get_iniFlashTable_ptr();


      (void)REPORTES_getSesions(lastSesion, 1);       // Obtengo ultimo puntero de sesion
      lastSesion_ptr = lastSesion[0];                 // ultimo puntero de sesion


      desconexAlimTOTAL = 0;
      // EDIT 14/11/12
      //  i empieza de CERO, xq barre las tarifas => Antes de que pase a OCUPADO por 1ra vez, la tarifa
      // es CERO. Si no pongo esto => no cuentas esas desconexiones
      i = 0;
      while (i <= cantidadTarifasProgramables){
        desconexAlimTOTAL += REPORTES_getCantidadViajes_byRango((tREG_GENERIC*)iniTable_ptr, (tREG_GENERIC*)lastSesion_ptr, REG_desconexAlim, i);
        i++;
      }

      return(desconexAlimTOTAL);
    }



  /* CALCULAR TIEMPO DESCONEXION ALIMENTACION TOTAL */
  /*****************************************************/

    uint32_t TURNO_calcTiempoDesconexionAlim_TOTAL(byte* buffer){
      // Calcula el tiempo que la alimentacion estuvo deconectada , la convierte a string y la devuelve
      // en el buffer.
      uint32_t tiempoDesconectadoTOTAL;
      tREG_SESION* lastSesion[1];
      tREG_GENERIC* lastSesion_ptr;
      tREG_GENERIC* iniTable_ptr;

      iniTable_ptr = get_iniFlashTable_ptr();


      (void)REPORTES_getSesions(lastSesion, 1);       // Obtengo ultimo puntero de sesion
      lastSesion_ptr = (tREG_GENERIC*)lastSesion[0];                 // ultimo puntero de sesion


      //tiempoDesconectado = REPORTES_getTiempoDesconexionAlim_byRango((tREG_GENERIC*)(&EEPROM_REPORTE), (tREG_GENERIC*)REPORTE_PUTptr); // Total TIEMPO en DESCONEXIONES ALIM
      tiempoDesconectadoTOTAL = REPORTES_getTiempoDesconexionAlim_byRango((tREG_GENERIC*)iniTable_ptr, (tREG_GENERIC*)lastSesion_ptr); // Total TIEMPO en DESCONEXIONES ALIM

      if (buffer != NULL){
        seconds_to_HMS(buffer, tiempoDesconectadoTOTAL);
      }

      return(tiempoDesconectadoTOTAL);
    }


  /* EXTRAER IMPORTE PERDIDO TOTAL EN TARIFA */
  /*******************************************/
    uint32_t TURNO_getImportePerdido_tarifa_TOTAL(byte* buffer, byte nroTarifa){
      // El calculo del importe perdido depende de si trabaja con PESOS o con FICHAS.
      // Para el caso con fichas, sólo tiene sentido si tiene habilitada la Eq en Pesos
      //
      // Si el buffer es NULL, no lo convierte a string ni lo modifica.
      //
      // Devuelve el importe perdido en la tarifa. Si no tiene programada la Eq Pesos devuelve 0
      uint32_t importePerdidoTOTAL;

      tREG_SESION* lastSesion[1];
      tREG_GENERIC* lastSesion_ptr;
      tREG_GENERIC* iniTable_ptr;

      iniTable_ptr = get_iniFlashTable_ptr();


      (void)REPORTES_getSesions(lastSesion, 1);                         // Obtengo ultimo puntero de sesion
      lastSesion_ptr = (tREG_GENERIC*)lastSesion[0];                 // ultimo puntero de sesion


      importePerdidoTOTAL = REPORTES_getImporte_byRango((tREG_GENERIC*)iniTable_ptr, (tREG_GENERIC*)lastSesion_ptr, REG_desconexAlim, nroTarifa); // Total IMPORTE en DESCONEXIONES ALIM

      return(importePerdidoTOTAL);
    }


  /* CALCULAR IMPORTE PERDIDO TOTAL */
  /**********************************/
    uint32_t TURNO_calcImportePerdido_TOTAL(byte* buffer){
      // Calcula el importe perdido del turno, la convierte a string y la devuelve
      // en el buffer.
      // Devuelve este importe perdido. 0 es el valor de error si no tiene programada la Eq Pesos
      uint32_t importePerdido;
      byte i;

      importePerdido = 0;
      i = 1;
      while(i <= cantidadTarifasProgramables){
        importePerdido += TURNO_getImportePerdido_tarifa_TOTAL(NULL,i);
        i++;
      }

      TICKET_importeToString(importePerdido, buffer);

      return(importePerdido);
    }



  /* EXTRAER CANTIDAD DE DESCONEXIONES DE ACCESORIO TOTAL */
  /********************************************************/
    uint16_t TURNO_getCantDesconexionesAcc_TOTAL(void){
      uint16_t desconexAccTOTAL;

      tREG_GENERIC* iniTable_ptr;
      tREG_SESION* lastSesion[1];
      tREG_GENERIC* lastSesion_ptr;

      iniTable_ptr = get_iniFlashTable_ptr();
      (void)REPORTES_getSesions(lastSesion, 1);                         // Obtengo ultimo puntero de sesion
      lastSesion_ptr = (tREG_GENERIC*)lastSesion[0];                 // ultimo puntero de sesion

      desconexAccTOTAL = REPORTES_getCantidadDesconexionesAcc_byRango((tREG_GENERIC*)iniTable_ptr, (tREG_GENERIC*)lastSesion_ptr);

      return(desconexAccTOTAL);
    }



  /* CALCULAR TIEMPO DESCONEXION ACCESORIO EN TURNO */
  /**************************************************/
    uint32_t TURNO_calcTiempoDesconexionAcc_TOTAL(byte* buffer){
      // Calcula el tiempo que el accesorio estuvo deconectado en el turno, la convierte a string y la devuelve
      // en el buffer.
      uint32_t tiempoDesconectadoTOTAL;

      tREG_GENERIC* iniTable_ptr;
      tREG_SESION* lastSesion[1];
      tREG_GENERIC* lastSesion_ptr;

      iniTable_ptr = get_iniFlashTable_ptr();
      (void)REPORTES_getSesions(lastSesion, 1);                         // Obtengo ultimo puntero de sesion
      lastSesion_ptr = (tREG_GENERIC*)lastSesion[0];                 // ultimo puntero de sesion

      tiempoDesconectadoTOTAL = REPORTES_getTiempoDesconexionAcc_byRango((tREG_GENERIC*)iniTable_ptr, (tREG_GENERIC*)lastSesion_ptr); // Total TIEMPO en DESCONEXIONES ALIM

      if (buffer != NULL){
        seconds_to_HMS(buffer, tiempoDesconectadoTOTAL);
      }

      return(tiempoDesconectadoTOTAL);
    }

 /* EXTRAER IMPORTE PERDIDO POR DESCONEXION DE ACCESORIO TOTAL EN TARIFA */
  /************************************************************************/


    uint32_t TURNO_getImportePerdidoAcc_TOTAL_tarifa(byte* buffer, byte nroTarifa){
      // El calculo del importe perdido por desconexion del accesorio depende de si trabaja
      // con PESOS o con FICHAS.
      // Para el caso con fichas, sólo tiene sentido si tiene habilitada la Eq en Pesos
      //
      // Si el buffer es NULL, no lo convierte a string ni lo modifica.
      //
      // Devuelve el importe perdido en la tarifa. Si no tiene programada la Eq Pesos devuelve 0
      uint32_t importePerdidoTOTAL;

      tREG_GENERIC* iniTable_ptr;
      tREG_SESION* lastSesion[1];
      tREG_GENERIC* lastSesion_ptr;

      iniTable_ptr = get_iniFlashTable_ptr();
      (void)REPORTES_getSesions(lastSesion, 1);                         // Obtengo ultimo puntero de sesion
      lastSesion_ptr = (tREG_GENERIC*)lastSesion[0];                 // ultimo puntero de sesion

      importePerdidoTOTAL = REPORTES_getImporte_byRango((tREG_GENERIC*)iniTable_ptr, (tREG_GENERIC*)lastSesion_ptr, REG_desconexAcc, nroTarifa); // Total IMPORTE en DESCONEXIONES ACC

      return(importePerdidoTOTAL);
    }


/* CALCULAR IMPORTE PERDIDO POR DESCONEXION DE ACCESORIO EN TURNO */
  /******************************************************************/

    uint32_t TURNO_calcImportePerdidoAcc_TOTAL(byte* buffer){
      // Calcula el importe perdido por desconexion de accesorio del turno, la convierte a string y la devuelve
      // en el buffer.
      // Devuelve este importe perdido. 0 es el valor de error si no tiene programada la Eq Pesos
      uint32_t importePerdidoTOTAL;
      byte i;

      importePerdidoTOTAL = 0;
      i = 1;
      while(i <= cantidadTarifasProgramables){
        importePerdidoTOTAL += TURNO_getImportePerdidoAcc_TOTAL_tarifa(NULL,i);
        i++;
      }

      TICKET_importeToString(importePerdidoTOTAL, buffer);

      return(importePerdidoTOTAL);
    }



  /* EXTRAER CANTIDAD DE MOVIMIENTO SIN PULSOS TOTAL */
  /***************************************************/

    uint16_t TURNO_getCantMovSinPulsos_TOTAL(void){
      uint16_t veces;

      tREG_GENERIC* iniTable_ptr;
      tREG_SESION* lastSesion[1];
      tREG_GENERIC* lastSesion_ptr;

      iniTable_ptr = get_iniFlashTable_ptr();
      (void)REPORTES_getSesions(lastSesion, 1);                      // Obtengo ultimo puntero de sesion
      lastSesion_ptr = (tREG_GENERIC*)lastSesion[0];                 // ultimo puntero de sesion

      veces = REPORTES_getCantidadMovSinPulsos_byRango((tREG_GENERIC*)iniTable_ptr, (tREG_GENERIC*)lastSesion_ptr);

      return(veces);
    }

    void REPORTES_calVarios (tREG_GENERIC* INI_ptr, tREG_GENERIC* FIN_ptr, uint32_t* datosMOV){



     	uint32_t kmLIBRE, kmOCUPADO, kmFSERV;
     	uint32_t velMaxLIBRE, velMaxOCUPADO, velMaxFSERV;
     	uint32_t tiempoMarchaLIBRE, tiempoMarchaOCUPADO, tiempoMarchaFSERV;
     	uint32_t tiempoParadoLIBRE, tiempoParadoOCUPADO, tiempoParadoFSERV;


         byte TO_F;
         tREG_GENERIC aux_INI;
         tREG_GENERIC* aux_INI_ptr;
         byte pointer_ok;



        kmLIBRE = 0; 			kmOCUPADO = 0; 			 	kmFSERV = 0;
		velMaxLIBRE = 0; 		velMaxOCUPADO = 0;			velMaxFSERV = 0;
		tiempoMarchaLIBRE = 0;	tiempoMarchaOCUPADO = 0;	tiempoMarchaFSERV = 0;
		tiempoParadoLIBRE = 0;	tiempoParadoOCUPADO = 0;	tiempoParadoFSERV = 0;

		aux_INI_ptr = &aux_INI;

		pointer_ok = REPORTE_chkPointer(INI_ptr);   // Verifico que el puntero de inicio sea correcto

        if(pointer_ok){
             pointer_ok = REPORTE_chkPointer(FIN_ptr); // Verifico que el puntero de fin sea correcto
        }

		TO_F = 0;
		dispararTO_lazo();
		while ((INI_ptr != FIN_ptr)){

			TO_F = chkTO_lazo_F();		//Chequeo bandera de time out de lazo
			//CHECK RANGO PUNTEROS
			if(checkRANGE(INI_ptr, FIN_TABLA_REPORTE, TO_F )){break;}		//cuando INI_ptr > FIN_TABLA_REPORTE sale del while
			//READ EEPROM
			EEPROM_ReadBuffer(&aux_INI,INI_ptr,sizeof(tREG_GENERIC));

			//CALCULOS LIBRE
			if (aux_INI.tipo == REG_libre){
			  //libre,ocupado,fuera de servicio
			  kmLIBRE += ((tREG_LIBRE*) aux_INI_ptr)->km;

			  if (((tREG_LIBRE*) aux_INI_ptr)->velMax > velMaxLIBRE){
				velMaxLIBRE = ((tREG_LIBRE*) aux_INI_ptr)->velMax;  // Actualizo valor de velocidad maxima
			  }

			  //tiempo marcha y parado
			  //MARCHA
			  tiempoMarchaLIBRE += ((tREG_LIBRE*) aux_INI_ptr)->segMarcha;
			  tiempoParadoLIBRE += ((tREG_LIBRE*) aux_INI_ptr)->segParado;
			}

			//CALCULOS OCUPADO
			if (aux_INI.tipo == REG_ocupado){
			  //libre,ocupado,fuera de servicio
			  kmOCUPADO += ((tREG_OCUPADO*) aux_INI_ptr)->km;

			  if (((tREG_OCUPADO*) aux_INI_ptr)->velMax > velMaxOCUPADO){
				velMaxOCUPADO = ((tREG_OCUPADO*) aux_INI_ptr)->velMax;  // Actualizo valor de velocidad maxima
			  }

			  //tiempo marcha y parado
			  //MARCHA
			  tiempoMarchaOCUPADO += ((tREG_OCUPADO*) aux_INI_ptr)->segMarcha;
			  tiempoParadoOCUPADO += ((tREG_OCUPADO*) aux_INI_ptr)->segParado;
			}

			//CALCULOS FUERA DE SERVICIO
			if (aux_INI.tipo == REG_fserv){
			  //libre,ocupado,fuera de servicio
			  kmFSERV += ((tREG_FSERV*) aux_INI_ptr)->km;

			  if (((tREG_FSERV*) aux_INI_ptr)->velMax > velMaxFSERV){
				velMaxFSERV = ((tREG_FSERV*) aux_INI_ptr)->velMax;  // Actualizo valor de velocidad maxima
			  }

			  //tiempo marcha y parado
			  //MARCHA
			  tiempoMarchaFSERV += ((tREG_FSERV*) aux_INI_ptr)->segMarcha;
			  tiempoParadoFSERV += ((tREG_FSERV*) aux_INI_ptr)->segParado;
			}
			//Avanzo puntero
			incFlashRep_ptr(&INI_ptr);
		}
		detenerTO_lazo();
		datosMOV[0] = kmLIBRE;
		datosMOV[1] = kmOCUPADO;
		datosMOV[2] = kmFSERV;
		datosMOV[3] = velMaxLIBRE;
		datosMOV[4] = velMaxOCUPADO;
		datosMOV[5] = velMaxFSERV;
		datosMOV[6] = tiempoMarchaLIBRE;
		datosMOV[7] = tiempoMarchaOCUPADO;
		datosMOV[8] = tiempoMarchaFSERV;
		datosMOV[9] = tiempoParadoLIBRE;
		datosMOV[10] = tiempoParadoOCUPADO;
		datosMOV[11] = tiempoParadoFSERV;
    	}




#endif
#endif

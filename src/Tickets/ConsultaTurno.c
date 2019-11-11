
#include "Common Ticket.h"
#include "Ticket Turno.h"


uint16_t consulta_ticket_turno(uint8_t* buffer, uint16_t nroTurno){

	byte i;
	uint8_t* ptrSimple;
	uint8_t** ptrDouble;
    uint8_t buffer_aux[20];
    uint16_t aux16;
    uint32_t aux32;
    uint16_t N;
    byte cantidad_de_sesion;
    tREG_GENERIC* REPORTE_PUTptr_aux;
    byte statusSESION;
    uint32_t datosMOV[4][3];
    uint32_t kmTotal, velMaxTotal, tMarchaTotal,tParadoTotal;

			REPORTE_PUTptr_aux = REPORTE_PUTptr;

			// El primer puntero a sesion (sesion_ptrs[0]) corresponde al ultimo turno
			// abierto, por lo tanto no esta cerrado => Los turnos comienzan a partir
			// de sesion_ptrs[1]. Es por eso que si el indice vale X:
			//  - INCIO -> sesion_ptrs[X+1]
			//  - FIN ---> sesion_ptrs[X]
			iniTURNO_ptr = &iniTURNO;
			finTURNO_ptr = &finTURNO;


			//cantidad_de_sesion = REPORTES_getSesions(sesion_ptrs, max_turnosReporte);        				// Obtengo punteros a todos las sesiones
			(void)REPORTES_getTurno(sesion_ptrs, nroTurno, max_turnosReporte);
			//inicializo punteros a sesiones
			TICKET_TURNO_setIni(sesion_ptrs[MENU_REPORTE_TURNO_index + 1], iniTURNO_ptr);   // Puntero a inicio de turno seleccionado
			TICKET_TURNO_setFin(sesion_ptrs[MENU_REPORTE_TURNO_index], finTURNO_ptr);       // Puntero a final de turno seleccionado

			iniTURNO_ptr_aux = sesion_ptrs[MENU_REPORTE_TURNO_index + 1];
	        finTURNO_ptr_aux = sesion_ptrs[MENU_REPORTE_TURNO_index];

			ptrSimple = buffer;
			ptrDouble = (uint8_t**)&ptrSimple;

//doy vuelta los bytes
/*
convert_bigINDIAN_to_litleINDIAN (&regVIAJE.fichasDist, 3);
convert_bigINDIAN_to_litleINDIAN (&regVIAJE.fichasTime, 3);
convert_bigINDIAN_to_litleINDIAN (&regVIAJE.importe, 4);
convert_bigINDIAN_to_litleINDIAN (&regVIAJE.kmLIBRE			, 2);
convert_bigINDIAN_to_litleINDIAN (&regVIAJE.kmOCUPADO		, 2);
convert_bigINDIAN_to_litleINDIAN (&regVIAJE.segMarchaLIBRE	, 2);
convert_bigINDIAN_to_litleINDIAN (&regVIAJE.segMarchaOCUPADO, 2);
convert_bigINDIAN_to_litleINDIAN (&regVIAJE.segParadoLIBRE	, 2);
convert_bigINDIAN_to_litleINDIAN (&regVIAJE.segParadoOCUPADO, 2);
*/
			//NUMERO DE TURNO
			string_copy_incDest(ptrDouble,"NT");									//comando a buffer
            aux32 = iniTURNO_ptr->nroTurno;
            convert_bigINDIAN_to_litleINDIAN (&aux32, 4);
            bufferNcopy_incDst(ptrDouble,(byte*)&aux32+2,2);
            //CHOFER
			string_copy_incDest(ptrDouble,"CH");									//comando a buffer
            aux32 = iniTURNO_ptr->chofer;
            convert_bigINDIAN_to_litleINDIAN (&aux32, 4);
            bufferNcopy_incDst(ptrDouble,(byte*)&aux32+3,1);
            //FICHAS O PESOS
			string_copy_incDest(ptrDouble,"FP");									//comando a buffer
            aux32 = iniTURNO_ptr->fichaPesos;
            convert_bigINDIAN_to_litleINDIAN (&aux32, 4);
            bufferNcopy_incDst(ptrDouble,(byte*)&aux32+3,1);
            //CHOFER
			string_copy_incDest(ptrDouble,"PD");									//comando a buffer
            aux32 = iniTURNO_ptr->punto_decimal;
            convert_bigINDIAN_to_litleINDIAN (&aux32, 4);
            bufferNcopy_incDst(ptrDouble,(byte*)&aux32+3,1);

			//INICIO DE TURNO
			string_copy_incDest(ptrDouble,"IT");									//comando a buffer
            bufferNcopy_incDst(ptrDouble,(byte*)&iniTURNO_ptr->date.hora[0],1);		//Hora
            bufferNcopy_incDst(ptrDouble,(byte*)&iniTURNO_ptr->date.hora[1],1);		//minutos
            bufferNcopy_incDst(ptrDouble,(byte*)&iniTURNO_ptr->date.hora[2],1);		//segundos
            bufferNcopy_incDst(ptrDouble,(byte*)&iniTURNO_ptr->date.fecha[0],1);	//dia
			bufferNcopy_incDst(ptrDouble,(byte*)&iniTURNO_ptr->date.fecha[1],1);	//mes
			bufferNcopy_incDst(ptrDouble,(byte*)&iniTURNO_ptr->date.fecha[2],1);	//año

			//FIN DE TURNO
			string_copy_incDest(ptrDouble,"FT");									//comando a buffer
            bufferNcopy_incDst(ptrDouble,(byte*)&finTURNO_ptr->date.hora[0],1);		//Hora
            bufferNcopy_incDst(ptrDouble,(byte*)&finTURNO_ptr->date.hora[1],1);		//minutos
            bufferNcopy_incDst(ptrDouble,(byte*)&finTURNO_ptr->date.hora[2],1);		//segundos
            bufferNcopy_incDst(ptrDouble,(byte*)&finTURNO_ptr->date.fecha[0],1);	//dia
			bufferNcopy_incDst(ptrDouble,(byte*)&finTURNO_ptr->date.fecha[1],1);	//mes
			bufferNcopy_incDst(ptrDouble,(byte*)&finTURNO_ptr->date.fecha[2],1);	//año


			(void)REPORTES_calVarios (iniTURNO_ptr_aux, finTURNO_ptr_aux, datosMOV);
			estadosTurno = 0;
			while(estadosTurno < sizeof(TICKET_TURNO_estados)){
			//while(estadosTurno < 8){
				if (TICKET_TURNO_estados[estadosTurno] == REG_libre){
					//DATOS DE LIBRE
					string_copy_incDest(ptrDouble,"LI");									//comando a buffer
					kmTotal     = datosMOV[0][0];	velMaxTotal = datosMOV[1][0];
					tMarchaTotal= datosMOV[2][0];	tParadoTotal= datosMOV[3][0];
				}else if (TICKET_TURNO_estados[estadosTurno] == REG_ocupado){
					//DATOS DE OCUPADO
					string_copy_incDest(ptrDouble,"OC");									//comando a buffer
					kmTotal     = datosMOV[0][1];	velMaxTotal = datosMOV[1][1];
					tMarchaTotal= datosMOV[2][1];	tParadoTotal= datosMOV[3][1];
				}else if (TICKET_TURNO_estados[estadosTurno] == REG_fserv){
					//DATOS DE FUERA DE SERVICIO
					string_copy_incDest(ptrDouble,"FS");									//comando a buffer
					kmTotal     = datosMOV[0][2];	velMaxTotal = datosMOV[1][2];
					tMarchaTotal= datosMOV[2][2];	tParadoTotal= datosMOV[3][2];
				}

				//DISTANCIA
				string_copy_incDest(ptrDouble,"DI");									//comando a buffer
				aux32 = kmTotal;
				convert_bigINDIAN_to_litleINDIAN (&aux32, 4);
				bufferNcopy_incDst(ptrDouble,(byte*)&aux32,4);

				//VELOCIDAD MAXIMA EN KM/H
				string_copy_incDest(ptrDouble,"VE");									//comando a buffer
				if(velMaxTotal > 255){
					velMaxTotal = 255;
				}
				aux32 = velMaxTotal;
	            convert_bigINDIAN_to_litleINDIAN (&aux32, 4);
	            bufferNcopy_incDst(ptrDouble,(byte*)&aux32+3,1);


				//TIEMPO MARCHA
				string_copy_incDest(ptrDouble,"TM");									//comando a buffer
				seconds_to_HMS_HEX(buffer_aux,tMarchaTotal);          		//Convierto a H-M-S
	            bufferNcopy_incDst(ptrDouble,(byte*)&buffer_aux[0],1);		//Hora
	            bufferNcopy_incDst(ptrDouble,(byte*)&buffer_aux[1],1);		//minutos

				//TIEMPO PARADO
				string_copy_incDest(ptrDouble,"TP");
				seconds_to_HMS_HEX(buffer_aux,tParadoTotal);          		//Convierto a H-M-S
	            bufferNcopy_incDst(ptrDouble,(byte*)&buffer_aux[0],1);		//Hora
	            bufferNcopy_incDst(ptrDouble,(byte*)&buffer_aux[1],1);		//minutos
				estadosTurno++;

			}

			tarifasTurno = tarifa1D;
			do{
				//TARIFA n
				string_copy_incDest(ptrDouble,"T");									//comando a buffer
				convert_to_string(buffer_aux, tarifasTurno, 0xff, base_DECIMAL);
				string_copy_incDest(ptrDouble,&buffer_aux);

				//CANTIDAD DE VIAJES
				string_copy_incDest(ptrDouble,"CV");
				aux32 = TURNO_getCantViajes_tarifa(tarifasTurno);
				convert_bigINDIAN_to_litleINDIAN (&aux32, 4);
				bufferNcopy_incDst(ptrDouble,(byte*)&aux32+3,1);

				//CANTIDAD DE FICHAS
				string_copy_incDest(ptrDouble,"CF");
				aux32 = getFichasTiempo_tarifa(tarifasTurno) + getFichasDistancia_tarifa(tarifasTurno);
				convert_bigINDIAN_to_litleINDIAN (&aux32, 4);
				bufferNcopy_incDst(ptrDouble,(byte*)&aux32+2,2);

				//RECAUDACION
				string_copy_incDest(ptrDouble,"RE");
				aux32 = TURNO_getRecaudacion_tarifa(&buffer_aux, tarifasTurno);
				convert_bigINDIAN_to_litleINDIAN (&aux32, 4);
				bufferNcopy_incDst(ptrDouble,(byte*)&aux32,4);

			}while(TICKET_pasarSiguienteTarifa(&tarifasTurno));


			//TOTALES
			string_copy_incDest(ptrDouble,"TT");

			//CANTIDAD TOTAL DE VIAJES
			string_copy_incDest(ptrDouble,"TV");
			aux32 = (uint32_t)TURNO_getCantViajes_turno();
            convert_bigINDIAN_to_litleINDIAN (&aux32, 4);
            bufferNcopy_incDst(ptrDouble,(byte*)&aux32+3,1);


			//CANTIDAD TOTAL DE VIAJES POR ASIENTO
			if (SENSOR_ASIENTO_NORM || SENSOR_ASIENTO_INTEL){
				string_copy_incDest(ptrDouble,"TA");
				aux32 = (uint32_t)TURNO_getCantViajesAsiento_turno();
	            convert_bigINDIAN_to_litleINDIAN (&aux32, 4);
	            bufferNcopy_incDst(ptrDouble,(byte*)&aux32+3,1);
			}


			//ERRORES POR ASIENTO
			if (SENSOR_ASIENTO_INTEL){
				string_copy_incDest(ptrDouble,"EA");
				aux32 = (uint32_t)TURNO_getCantErroresAsiento_turno();
	            convert_bigINDIAN_to_litleINDIAN (&aux32, 4);
	            bufferNcopy_incDst(ptrDouble,(byte*)&aux32+3,1);
			}

			//TOTAL RECAUDACION
			string_copy_incDest(ptrDouble,"TR");
			aux32 = (uint32_t)TURNO_getRecaudacion_turno(&buffer_aux);
			convert_bigINDIAN_to_litleINDIAN (&aux32, 4);
			bufferNcopy_incDst(ptrDouble,(byte*)&aux32,4);

			//TOTAL KILOMETROS
			string_copy_incDest(ptrDouble,"TK");
			aux32 = (uint32_t)TURNO_calcKmTotales_turno();
			convert_bigINDIAN_to_litleINDIAN (&aux32, 4);
			bufferNcopy_incDst(ptrDouble,(byte*)&aux32,4);

			//PORCENTAJE CHOFER
			if (PROG_TICKET_percentChofer > 0){
				string_copy_incDest(ptrDouble,"PC");
				aux32 = (uint32_t)PROG_TICKET_percentChofer;
		        convert_bigINDIAN_to_litleINDIAN (&aux32, 4);
		        bufferNcopy_incDst(ptrDouble,(byte*)&aux32+3,1);

				aux32 = (uint32_t)TURNO_calcRecaudacionChofer(buffer_aux);
		        convert_bigINDIAN_to_litleINDIAN (&aux32, 4);
		        bufferNcopy_incDst(ptrDouble,(byte*)&aux32+1,3);
			}

			//PESOS POR KILOMETRO
			string_copy_incDest(ptrDouble,"PK");
			aux32 = (uint32_t)TURNO_calcRecaudacionPorKm_turno(buffer_aux);
	        convert_bigINDIAN_to_litleINDIAN (&aux32, 4);
	        bufferNcopy_incDst(ptrDouble,(byte*)&aux32+1,3);

			//PORCENTAJE DE KILOMETROS OCUPADOS
			string_copy_incDest(ptrDouble,"KO");
			aux32 = (uint32_t)TURNO_calcPercentKmOcupado_turno(buffer_aux);
			aux32 = aux32/100;
	        convert_bigINDIAN_to_litleINDIAN (&aux32, 4);
	        bufferNcopy_incDst(ptrDouble,(byte*)&aux32+3,1);

			//CANTIDAD DE DESCONEXIONES DE ALIMENTACION
			string_copy_incDest(ptrDouble,"CD");
			aux32 = (uint32_t)TURNO_getCantDesconexionesAlim_turno();
			if(aux32>255){
				aux32=255;
			}
			convert_bigINDIAN_to_litleINDIAN (&aux32, 4);
	        bufferNcopy_incDst(ptrDouble,(byte*)&aux32+3,1);

			//TIEMPO DE DESCONEXIONES DE ALIMENTACION
			string_copy_incDest(ptrDouble,"TD");
			(void)TURNO_calcTiempoDesconexionAlim_turno_HEX(buffer_aux);
            bufferNcopy_incDst(ptrDouble,(byte*)&buffer_aux[0],1);		//Hora
            bufferNcopy_incDst(ptrDouble,(byte*)&buffer_aux[1],1);		//minutos
            bufferNcopy_incDst(ptrDouble,(byte*)&buffer_aux[2],1);		//minutos

			//IMPORTE PERDIDOS
			string_copy_incDest(ptrDouble,"IP");
			aux32 = (uint32_t)TURNO_calcImportePerdido_turno(&buffer_aux);
	        convert_bigINDIAN_to_litleINDIAN (&aux32, 4);
	        bufferNcopy_incDst(ptrDouble,(byte*)&aux32,4);

			//inicio envio por el puerto
			N = ptrSimple - buffer;

			return(N);

 }

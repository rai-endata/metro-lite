/*
 * file: Comandos sin conexion a central.c
 *
 *  Created on: 21/11/2019
 *      Author: Rai
 */

#include "Comandos sin conexion a central.h"
#include "Lista Comandos.h"
#include "rtc.h"
#include "Reportes.h"
#include "DA Define Commands.h"
#include "DA Rx.h"

buffcirTYPE	 datosSC;					//estructura de datos sin conexion para acumular  comandos sin condexion
byte 		 bufferSC[dim_bufferSC];    //buffer circular que acumula datos sin conexion
byte timeoutTx_datosSC;

void ini_acumular_cmdSC (void){
	datosSC_iniBUFFER   = bufferSC;
	datosSC_GETptr      = bufferSC;
	datosSC_PUTptr      = bufferSC;
	datosSC_SizeBUFFER  = dim_bufferSC;
	datosSC_cntBYTE    	= 0;
	datosSC_cntDATOS   	= 0;
	Tx_datosSC = 0;
}

void acumular_cmdSC(byte nro_viaje){
	putBUFCIR(nro_viaje, &datosSC);
}

void procesar_datosSC(void){

	byte nroVIAJE,i, N;
	byte buff_aux[400];
	byte cantidad_de_sesion;
	tREG_GENERIC* ptrLIBRE;
	tREG_GENERIC* ptrOCUPADO;
	tREG_GENERIC* ptrREG_APAGAR;

	byte* 	get_BACK;
    byte cntBYTE_BACK;

	if( !sinCONEXION_CENTRAL && datosSC_cntBYTE > 0 && !Tx_datosSC){
		Tx_datosSC = 1;
        timeoutTx_datosSC = TIMEOUT_datosSC;

        //guardo puntero get y cntBYTE
		get_BACK = datosSC.get;
		cntBYTE_BACK = datosSC.cntBYTE;

		//tomo numero de viaje
		nroVIAJE = getBUFCIR(&datosSC);

		//restauro puntero get y cntBYTE (solo avanzo cuando recibo respuesta)
		datosSC.get = get_BACK;
		datosSC.cntBYTE = cntBYTE_BACK;

		i=0;
		buff_aux[i++] = subCMD_datosSC;   		//subcomando
		buff_aux[i++] = 0;   					//fuente de hora
		getDate();
		buff_aux[i++] = RTC_Date.hora[0];   	//HORA
		buff_aux[i++] = RTC_Date.hora[1];   	//MINUTOS
		buff_aux[i++] = RTC_Date.hora[2];   	//SEGUNDOS
		buff_aux[i++] = RTC_Date.fecha[0];  	//DIA
		buff_aux[i++] = RTC_Date.fecha[1];  	//MES
		buff_aux[i++] = RTC_Date.fecha[2];  	//AÑO

		ptrREG_APAGAR = get_regAPAGAR_byNUMERO_VIAJE(nroVIAJE);
		if(ptrREG_APAGAR != NULL){
			ptrOCUPADO  = get_regOCUPADO_by_ptrREG_APAGAR (ptrREG_APAGAR, nroVIAJE);
			ptrLIBRE    = get_regLIBRE_by_ptrREG_APAGAR (ptrREG_APAGAR, nroVIAJE);
			i = armar_buff_viaje((uint8_t*)&buff_aux, i);
			N = i+1;
			Tx_cmdTRANSPARENTE(N, buff_aux );
		}
	}
}


/* TIMER REINTENTOS DATOS SIN CONEXION */
/***************************************/



void reTx_datosSC (void){
         // Ejecutada cada 1seg de forma no precisa

		if (timeoutTx_datosSC != 0){
			timeoutTx_datosSC--;
			if (timeoutTx_datosSC == 0){
				Tx_datosSC = 0;
		        timeoutTx_datosSC = TIMEOUT_datosSC;
			}
		}
}

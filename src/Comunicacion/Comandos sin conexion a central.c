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
#include "Reloj.h"

#include "DTE - Tx.h"

buffWordTYPE	 datosSC;					//estructura de datos sin conexion para acumular  comandos sin condexion
word bufferSC[dim_bufferSC];    			//buffer circular que acumula datos sin conexion
byte timeoutTx_datosSC;

static byte load_regViaje(byte nro_viaje);

void ini_acumular_cmdSC (void){
	datosSC_iniBUFFER   = bufferSC;
	datosSC_GETptr      = bufferSC;
	datosSC_PUTptr      = bufferSC;
	datosSC_SizeBUFFER  = dim_bufferSC;
	datosSC_cntWORD    	= 0;
	datosSC_cntDATOS   	= 0;
	Tx_datosSC = 0;
	esperarRespuesta_cmdReloj = 0;
}

/*  ARMA Y TRANSMITE DATOS SIN CONEXION *
 ****************************************/


void check_datosSC(void){
	sc_data dataSC;
	byte status;
	tREG_LIBRE*		ptrREG_LIBRE;

	if(datosSC_cntWORD > 0 && appConectada_ACentral && !DA_Txing && !esperarRespuesta_cmdReloj){
		dataSC = pull_dataSC();

		//levantar datos de la tabla
		status = load_regViaje(dataSC.nro_viaje);

		//rearmar y transmitir comandos de reloj
		if(dataSC.estado_reloj == LIBRE){

			//rearmar y transmitir libre
			esperarRespuesta_cmdReloj = 1;
			ptrREG_LIBRE = get_regLIBRE_date(REPORTE_PUTptr, dataSC.nro_viaje);
			rearmar_y_TX_cmdLibre(dataSC.nro_viaje);
			if(datosSC_cntWORD == 0 && ESTADO_RELOJ == LIBRE){
				guardar_dateLIBRE();
			}

		}else if(dataSC.estado_reloj == OCUPADO){
			if(datosSC_cntWORD == 0 && ESTADO_RELOJ == OCUPADO){
					Tx_Pase_a_OCUPADO(CON_CONEXION_CENTRAL);
			}else{
				//rearmar y transmitir ocupado
				esperarRespuesta_cmdReloj = 1;
				rearmar_y_TX_cmdOcupado(dataSC.nro_viaje, status);
				if(datosSC_cntWORD == 0 && ESTADO_RELOJ == COBRANDO){
						Tx_Pase_a_COBRANDO(CON_CONEXION_CENTRAL);
				}
			}
		}
	}
}

static byte load_regViaje(byte nro_viaje){

	tREG_LIBRE*		ptrREG_LIBRE;
	tREG_OCUPADO*	ptrREG_OCUPADO;
	tREG_A_PAGAR*	ptrREG_APAGAR;
    byte status;

    status =0;
	//Levanto los datos de tabla en regViaje
	if(nro_viaje == 0){
		ptrREG_LIBRE = get_regLIBRE_by_anyPTR(REPORTE_PUTptr, 0xff);
 	}else{
 		ptrREG_LIBRE = get_regLIBRE_by_anyPTR(REPORTE_PUTptr, nro_viaje-1);
 	}
	ptrREG_OCUPADO = get_regOCUPADO_by_anyPTR(REPORTE_PUTptr, nro_viaje);
	ptrREG_APAGAR = get_regAPAGAR_by_anyPTR(REPORTE_PUTptr, nro_viaje);

	if( ptrREG_APAGAR == NULL ){
		status = 1;
	}
	return(status);

}

/*  GUARDO DATOS SIN CONEXION *
 ***************************** */

void push_dataSC(sc_data data){

	//guardo nroCorrelativo_INTERNO y estado de reloj
	//cuando hay un pase de reloj sin conexion, para poder rearmar el
	//comando cuando recupere la conexion

	word data_in;

	data_in = create_dataWord(data.nro_viaje, data.estado_reloj);

	putWord_BUFCIR(data_in, &datosSC);
}

/*  TOMO DATOS SIN CONEXION *
 ***************************** */

sc_data pull_dataSC(){

	//Tomo nroCorrelativo_INTERNO y estado de reloj
	//para rearmar comandos de reloj no enviados cuando
	//estaba sin conexion

	word data_out;
	sc_data data;

	data_out = getWord_BUFCIR(&datosSC);
    data.nro_viaje = (byte)(data_out>>8);
    data.estado_reloj = (byte)(data_out & 0x00ff);

    return(data);
}


/*
void procesar_datosSC(void){

	byte nroVIAJE,i, N;
	byte buff_aux[400];
	byte cantidad_de_sesion;
	tREG_GENERIC* ptrLIBRE;
	tREG_GENERIC* ptrOCUPADO;
	tREG_GENERIC* ptrREG_LIBRE;

	word* 	get_BACK;
    byte cntBYTE_BACK;

	//if( !sinCONEXION_CENTRAL && datosSC_cntBYTE > 0 && !Tx_datosSC){
	if(appConectada_ACentral && (datosSC_cntWORD > 0) && (getColaReportes() == 0) && !Tx_datosSC){
		Tx_datosSC = 1;
        timeoutTx_datosSC = TIMEOUT_datosSC;

        //guardo puntero get y cntBYTE
		get_BACK = datosSC.get;
		cntBYTE_BACK = datosSC.cntWORD;

		//tomo numero de viaje
		nroVIAJE = getBUFCIR(&datosSC);

		//restauro puntero get y cntBYTE (solo avanzo cuando recibo respuesta)
		datosSC.get = get_BACK;
		datosSC.cntWORD = cntBYTE_BACK;

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

		//ptrREG_APAGAR = get_regAPAGAR_byNUMERO_VIAJE(nroVIAJE);
		ptrREG_LIBRE = get_regLIBRE_byNUMERO_VIAJE(nroVIAJE);
		//if(ptrREG_APAGAR != NULL){
		if(ptrREG_LIBRE != NULL){
			ptrOCUPADO  = get_regOCUPADO_by_ptrREG_LIBRE (ptrREG_LIBRE, nroVIAJE);
			//ptrLIBRE    = get_regLIBRE_by_ptrREG_APAGAR (ptrREG_APAGAR, nroVIAJE);
			i = armar_buff_viajeSC((uint8_t*)&buff_aux, i);
			N = i+1;
			Tx_cmdTRANSPARENTE(N, buff_aux );
			Tx_Comando_MENSAJE(SINCRONIZANDO_CON_CENTRAL);
		}
	}
}

*/

/* TIMER REINTENTOS DATOS SIN CONEXION */
/***************************************/

/*

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

*/

/*
void pruebaSC(){

	sc_data A, B, C, D, E;

     sc_data a = {0x01, 0x18};
     sc_data b = {0x02, 0x19};
     sc_data c = {0x03, 0x1A};
     sc_data d = {0x05, 0x1B};
     sc_data e = {0x06, 0x1C};

     push_dataSC(a);
     push_dataSC(b);
     push_dataSC(c);
     push_dataSC(d);
     push_dataSC(e);

     A = pull_dataSC();
     B = pull_dataSC();
     C = pull_dataSC();
     D = pull_dataSC();
     E = pull_dataSC();

}

*/

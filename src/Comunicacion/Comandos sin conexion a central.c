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
#include "Eeprom.h"
#include "Timer.h"
#include "main.h"
#include "Inicio.h"
#include "Tarifacion Reloj.h"

static void inc_scGet(void);
static void inc_scPut(void);
static void dec_scGet(void);
static void dec_scPut(void);


buffWordTYPE	 datosSC;					//estructura de datos sin conexion para acumular  comandos sin condexion
word bufferSC[dim_bufferSC];    			//buffer circular que acumula datos sin conexion
byte timeoutTx_datosSC;


static word* scGet_ptr;
static word* scPut_ptr;

	void ini_acumular_cmdSC (void){
		byte TO_F;
		word dataOut;
		word* dataOut_ptr;
		word* auxGet;
		word** auxAddr;

		sc_data data;
		byte cntData;


		datosSC_iniBUFFER   = bufferSC;
		datosSC_GETptr      = bufferSC;
		datosSC_PUTptr      = bufferSC;
		datosSC_SizeBUFFER  = dim_bufferSC;
		datosSC_cntWORD    	= 0;
		datosSC_cntDATOS   	= 0;
		Tx_datosSC = 0;
		esperarRespuesta_cmdReloj = 0;

		firstIniSc();
		//leer puntero get en eeprom
		EEPROM_ReadBuffer((uint8_t*)&scGet_ptr, ADDR_DATOS_SC_GET_PTR, SIZE_DATOS_SC_GET_PTR);
		//leer puntero put en eeprom
		EEPROM_ReadBuffer((uint8_t*)&scPut_ptr, ADDR_DATOS_SC_PUT_PTR, SIZE_DATOS_SC_PUT_PTR);
		//calculo cantidad de comandos de reloj no transmitidos
		if(scPut_ptr > scGet_ptr){
			cntData = scPut_ptr - scGet_ptr;
		}else if(scPut_ptr < scGet_ptr){
			cntData = scGet_ptr - scPut_ptr;
		}else{
			cntData = 0;
		}

		//saco datos no transmitidos de la eeprom y los pongo en cola en ram para que sean transmitidos
		toLazoCnt = 20;			//2 segundos de timeout
		dataOut_ptr = (word*)&dataOut;

		//uso la variable auxGet para sacar los datos de la eeprom sin modificar el puntero scGet_ptr
		//el cual sera modificado al sacar los datos de la cola en ram para ser transmitidos
		auxGet = scGet_ptr;
		auxAddr = &auxGet;

		while((cntData > 0) && (toLazoCnt != 0) && (datosSC_cntWORD < dim_bufferSC)){
			//saca datos de cola en eeprom
			EEPROM_ReadBuffer(dataOut_ptr, *auxAddr, SIZE_DATOS_SC_GET_PTR);
			//pone datos sc en cola de ram para ser transmitidos
			data.nro_viaje = (byte)(dataOut>>8);
			data.estado_reloj = (byte)(dataOut & 0x00ff);
			push_dataSC(data);
			cntData--;
			incWord_ptr(auxAddr, (word*)ADDR_DATOS_SC, (uint16_t)SIZE_DATOS_SC);
		}
	}


	void firstIniSc(void){
		 if (VISOR_firstBoot){
			scPut_ptr = ADDR_DATOS_SC;
			scGet_ptr = ADDR_DATOS_SC;
			EEPROM_write(ADDR_DATOS_SC_GET_PTR, (byte*)scGet_ptr);
			EEPROM_write(ADDR_DATOS_SC_PUT_PTR, (byte*)scPut_ptr);
		 }
	}


/*  ARMA Y TRANSMITE DATOS SIN CONEXION *
 ****************************************/


void check_datosSC(void){
	sc_data dataSC;
	byte status;
	tREG_LIBRE*		ptrREG_LIBRE;
	byte error_inesperado;

	if(datosSC_cntWORD > 0 && appConectada_ACentral && !DA_Txing && !esperarRespuesta_cmdReloj){
		dataSC = pull_dataSC();
	    //actualiza puntero get sc en cola de eeprom
		inc_scGet();
		error_inesperado = 0;
		//check si es el ultimo comando a transmitir
		//si es el ultimo comando, check si se esta tr
		//rearmar y transmitir comandos de reloj
 		if(dataSC.estado_reloj == LIBRE){
			//levantar datos de la tabla
			status = load_regViaje(dataSC.nro_viaje);

			//rearmar y transmitir libre
			esperarRespuesta_cmdReloj = 1;
			ptrREG_LIBRE = get_regLIBRE_date(REPORTE_PUTptr, dataSC.nro_viaje);
			rearmar_y_TX_cmdLibre(dataSC.nro_viaje);
			if(datosSC_cntWORD == 0 && ESTADO_RELOJ == LIBRE){
				corregir_dateLIBRE();
			}
		}else if(dataSC.estado_reloj == OCUPADO){
			//transmitir el ultimo comando ?
			if(datosSC_cntWORD == 0 && ESTADO_RELOJ == OCUPADO){
				if(paseOCUPADO_PACTADO){
					VALOR_VIAJE = VALOR_VIAJE_PACTADO;
				}
				Tx_Pase_a_OCUPADO(CON_CONEXION_CENTRAL);
			}else{
				//levantar datos de la tabla
				status = load_regViaje(dataSC.nro_viaje);
				if(!status){
					//rearmar y transmitir ocupado
					esperarRespuesta_cmdReloj = 1;
					rearmar_y_TX_cmdOcupado(dataSC.nro_viaje, status);
					if(datosSC_cntWORD == 0 && ESTADO_RELOJ == COBRANDO){
						if(paseOCUPADO_PACTADO){
							VALOR_VIAJE = VALOR_VIAJE_PACTADO;
						}
						Tx_Pase_a_COBRANDO(CON_CONEXION_CENTRAL);
					}
				}else{
					error_inesperado = 1;
				}
			}
		}else{
			error_inesperado = 1;
		}
		if(error_inesperado){
			 // se produjo un error inesperado reinicio todo datosSC
				datosSC_iniBUFFER   = bufferSC;
				datosSC_GETptr      = bufferSC;
				datosSC_PUTptr      = bufferSC;
				datosSC_SizeBUFFER  = dim_bufferSC;
				datosSC_cntWORD    	= 0;
				datosSC_cntDATOS   	= 0;
				Tx_datosSC = 0;
				esperarRespuesta_cmdReloj = 0;
				//inicializa puntero get sin conexion
				EEPROM_write(ADDR_DATOS_SC_GET_PTR, ADDR_DATOS_SC);
				//inicializa puntero put sin conexion
				EEPROM_write(ADDR_DATOS_SC_PUT_PTR, ADDR_DATOS_SC);
				scPut_ptr = ADDR_DATOS_SC;
				scGet_ptr = ADDR_DATOS_SC;
		}
		// en el inicio, si datosSC_cntWORD != 0, no llama a check_corte_alimentacion(), por eso lo hago aca
		// entonces lo debe llamar cuando termina de transmitir los comandos que tenia guardado antes que se
		// apagara el equipo, luego de llamarlo una vez no lo debe volver a llamar hasta un nuevo encendido

		//if(datosSC_cntWORD == 0 && ((corteALIMENTACION | 0x1 == 0x1) | (corteALIMENTACION | 0x2 == 0x02)) ){
		if(datosSC_cntWORD == 0 && (((corteALIMENTACION & 0x1) == 0x1)|((corteALIMENTACION & 0x2) == 0x02))){
			INICIO_microCorte = diferenciaHoraria(horaEncendido.hora, HoraApagado.hora, 16);
			if(!INICIO_microCorte){
				// si hubo micro corte ya llamo a check_corte_alimentacion();
				// antes de transmitir comandos guardados
				check_corte_alimentacion();
			}
		}
	}
}


byte load_regViaje(byte nro_viaje){

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

	if( (ptrREG_LIBRE == NULL) | (ptrREG_OCUPADO == NULL) | (ptrREG_APAGAR == NULL) ){
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

	uint16_t data_in;

	data_in = create_dataWord(data.nro_viaje, data.estado_reloj);
	putWord_BUFCIR(data_in, &datosSC);	//pone datos sc en cola de ram
}


void push_dataSC_inEeprom(sc_data data){

	uint16_t data_in;

	//pone datos sc en cola de eeprom
	data_in = create_dataWord(data.nro_viaje, data.estado_reloj);
	EEPROM_write(scPut_ptr, data_in);
	inc_scPut();
}

/*  TOMO DATOS SIN CONEXION *
 ***************************** */

sc_data pull_dataSC(){

	//Tomo nroCorrelativo_INTERNO y estado de reloj
	//para rearmar comandos de reloj no enviados cuando
	//estaba sin conexion

	word data_out;
	sc_data data;
	//saca datos de cola
	data_out = getWord_BUFCIR(&datosSC);
    data.nro_viaje = (byte)(data_out>>8);
    data.estado_reloj = (byte)(data_out & 0x00ff);
    return(data);
}

void inc_scGet(void){
	//word* pepe;
	word** auxAddr;

	incWord_ptr(&scGet_ptr, (word*)ADDR_DATOS_SC, (uint16_t)SIZE_DATOS_SC);
	auxAddr = scGet_ptr;
	EEPROM_write(ADDR_DATOS_SC_GET_PTR, auxAddr);
}


void inc_scPut(void){
	//word* pepe;
	word** auxAddr;

	incWord_ptr(&scPut_ptr, (word*)ADDR_DATOS_SC, (uint16_t)SIZE_DATOS_SC);
	auxAddr = scPut_ptr;
	EEPROM_write(ADDR_DATOS_SC_PUT_PTR, auxAddr);
}


void dec_scGet(void){
	word** auxAddr;

	decWord_ptr(&scGet_ptr, (word*)ADDR_DATOS_SC, (uint16_t)SIZE_DATOS_SC);
	auxAddr = scGet_ptr;
	EEPROM_write(ADDR_DATOS_SC_GET_PTR, auxAddr);

}

void dec_scPut(void){
	word** auxAddr;

	decWord_ptr(&scPut_ptr, (word*)ADDR_DATOS_SC, (uint16_t)SIZE_DATOS_SC);
	auxAddr = scPut_ptr;
	EEPROM_write(ADDR_DATOS_SC_PUT_PTR, auxAddr);
}



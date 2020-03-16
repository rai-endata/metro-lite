/*
 * Rx-VA.c
 *
 *  Created on: 15/9/2017
 *      Author: Rai
 */
#include "Rx-VA.h"
#include "usart7.h"
#include "Protocolo bax.h"

void rxVA_chk_baxFORMAT(byte dato);

static void va_txDATA_ini (void);
static void va_rxDATA_ini (void);
static void rxVA_baxFORMAT_ini (void);
static byte VA_copyRxDAChain (void);


/**************************
* DEFINICION DE VARIABLES *
***************************/

		buffcirTYPE		va_rxDATA;				//buffer de recepcion serie del visor android
		buffcirTYPE		va_txDATA;				//buffer de transmision serie del visor android
		byte 			va_txDATA_buffer[dim_va_txDATA];
		byte 			va_rxDATA_buffer[dim_va_rxDATA];			//buffer de recepción serie del visor android
static tBAX 			rxVA_protBAX_STAGE; 			 // Etapa del protocolo BAX

static tBAX_SCI 		rxVA_baxFORMAT;              // Datos de SCI segun protocolo BAX

void VA_UART_ini (void){
	va_rxDATA_ini();

	va_txDATA_ini();
	rxVA_baxFORMAT_ini();
}

/* INICIALIZACION DE va_rxDATA *
********************************/

static void va_rxDATA_ini (void){
	va_iniRxBuffer   = va_rxDATA_buffer;
	va_rxGETptr      = va_rxDATA_buffer;
	va_rxPUTptr      = va_rxDATA_buffer;
	va_rxSizeBUFFER  = dim_va_rxDATA;
	va_rx_cntBYTE    = 0;
	va_rx_cntDATOS   = 0;
}


/* INICIALIZACION DE rxVA_baxFORMAT *
************************************/

static void rxVA_baxFORMAT_ini (void){
	rxVA_protBAX_STAGE = BAX_START;
	rxVA_baxFORMAT.start = BAX_start;
	rxVA_baxFORMAT.RxGPA_GETidx = 0;
	rxVA_baxFORMAT.RxGPA_PUTidx = 1;
	rxVA_baxFORMAT.RxPUT = rxVA_baxFORMAT.RxBuffer;
	rxVA_baxFORMAT.RxGPA[0] = rxVA_baxFORMAT.RxBuffer;
	rxVA_baxFORMAT.Rx_TO_cnt = 0;
	rxVA_baxFORMAT.Rx_cnt = 0;
}


/* INICIALIZACION DE va_rxDATA *
********************************/

static void va_txDATA_ini (void){
	va_iniTxBuffer   = va_txDATA_buffer;
	va_txGETptr      = va_txDATA_buffer;
	va_txPUTptr      = va_txDATA_buffer;
	va_txSizeBUFFER  = dim_va_txDATA;
	va_tx_cntBYTE    = 0;
	va_tx_cntDATOS   = 0;
}


void guardarRxDA_BaxFORMAT (void){
	byte rxBYTE;

	if( va_rx_cntBYTE > 0){
		//toma datos del buffer de sci (va_rxDATA)
		rxBYTE = getBUFCIR(&va_rxDATA);
		//chk si están llegando datos con el protocolo bax
		rxVA_chk_baxFORMAT(rxBYTE);
		//Aqui se analizarian eventualmente otros protocolos
		//....
	}
}


void rxVA_chk_baxFORMAT(byte dato){

	//se guarda el dato en ASCII en dataSKYPATROL_baxFORMAT.RxBuffer
	BAX_Rx_data(dato, &rxVA_protBAX_STAGE, &rxVA_baxFORMAT);

	//RECEPCION EXITOSA
	if (rxVA_baxFORMAT.Rx_fin){
	  rxVA_baxFORMAT.Rx_fin = 0;      // Bajo Bandera
	  huart7.Rx_TO_cnt = 0;
	}
}



/*  RECEPCION POR SERIE DE SKYPATROL   *
*******************************************************************************************************************/


   void RxDA_BaxFORMAT_toAIR_RxBuffer (void){
     // Pre-proesamiento de los datos recibidos por la serie de VISOR ANDROID
     // Lo que hace es:
     //  - Copiar del buffer de VISOR ANDROID al buffer de Aire
     //  - Agregar fin de datos (0xDF0A)
     //  - Incrementar cantidad de recepciones

	 byte* GETptr;
     byte RxCMD;
     byte done;

     if (rxVA_baxFORMAT.Rx_cnt > 0){
       // Si voy a usar el puntero GET debo hacerlo antes de copiarlo al buffer de AIR Rx
       // ya que despues, el puntero apunta a otro lado
       huart7.Rx_TO_cnt = 0;											//timout disparado por el byete de stop
       GETptr = rxVA_baxFORMAT.RxGPA[rxVA_baxFORMAT.RxGPA_GETidx];   	// Extraigo puntero GET, para poder tomar su direccion
       inc_ptr(&GETptr, rxVA_baxFORMAT.RxBuffer, dim_BAX);   			// salto N
       //inc_ptr(&GETptr, rxVA_baxFORMAT.RxBuffer, dim_BAX);   			// salto MOV_H
       //inc_ptr(&GETptr, rxVA_baxFORMAT.RxBuffer, dim_BAX);   			// salto MOV_L
       RxCMD = get_byte (&GETptr, rxVA_baxFORMAT.RxBuffer, dim_BAX); 	// Extraigo CMD

       done = VA_copyRxDAChain();         // Skypatrol Buffer -> Air Buffer

       if (done){
         //Como ya tengo un dato completo, ahora si marco un nuevo puntero GET para extraer
         //los futuros datos desde esta dirección
         DA_addRxPUT_toRxGPA();                   // Nuevo puntero GET dentro del GPA
         DA_saveRxPUTptr_BKP();                   // Guardo puntero, xq acabo de recibir una tira entera
         DA_incRxcnt();                           // Incremento contador de Recepciones por Aire
       }
     }
   }

	/* COPIAR CADENA RECIBIDA DESDE skypatrol A BUFFER DE Rx AIRE */
	/***************************************************************/

	static byte VA_copyRxDAChain (void){
		// Ahora esta rutina nos devuelve una indicacion de exito o no
		byte* GETptr;
		byte N, dato;
		byte TO_F;
		uint16_t space;
		byte done;

		GETptr = rxVA_baxFORMAT.RxGPA[rxVA_baxFORMAT.RxGPA_GETidx];   // Extraigo puntero GET, para poder tomar su direccion

		// Extraigo N y lo agrego al Buffer de AirRx
		// Solo agrego los datos al buffer de AirRx, si hay lugar.
		// A la cantidad de datos a agregar, le debo sumar 3, xq incluye el N
		// y los 2 bytes de fin
		N = get_byte (&GETptr, rxVA_baxFORMAT.RxBuffer, dim_BAX);     // Extraigo N
		space = chkSpace_onDA_RxBuffer();            				  // Determino espacio libre en buffer de AirRx

		if (space >= N + 3){
			DA_putByte_RxBuffer(N);                      			  // Agrego N a Buffer de Rx de Aire
			N--;                                          			 // Decremento N

			// Agrego datos al Buffer de AirRx
			TO_F = 0;
			dispararTO_lazo();                            // Disparo TimeOut Lazo
			while ((N > 0) && !TO_F){
				TO_F = chkTO_lazo_F();                      // Chequeo bandera de time out de lazo
				dato = get_byte (&GETptr, rxVA_baxFORMAT.RxBuffer, dim_BAX);     // Extraigo N
				DA_putByte_RxBuffer(dato);                 // Agrego Dato a Buffer de Rx de Aire
				N--;                                        // Decremento N
			}
			detenerTO_lazo();                             // Detener TimeOut Lazo

			// Agrego fin de datos al Buffer de AirRx
			DA_putByte_RxBuffer(fin_datos_msb);          // Agrego fin de datos MSB
			DA_putByte_RxBuffer(fin_datos_lsb);          // Agrego fin de datos LSB

			// Incremento GET pointer de RxGPA ya que extraje los datos que acabo de recibir por la serie
			rxVA_baxFORMAT.RxGPA_GETidx++;                // Incremento indice GET del GPA
			if (rxVA_baxFORMAT.RxGPA_GETidx == dim_BAX_GPA){
				rxVA_baxFORMAT.RxGPA_GETidx = 0;            // Doy la vuelta al indice
			}

			// Si el comando fue copiado al buffer de AirRx => me lo saque del buffer de
			// dataSKYPATROL_baxFORMAT, por lo tanto debo decrementar el contador de Rx
			rxVA_baxFORMAT.Rx_cnt--;                  // Decremento contador de Recepciones

			done = 1;

		}else{
			done = 0;
		}

		return(done);
	}



	void rxVA_baxFORMAT_TimeOut (void){
	     // Ejecutada cada 1mseg
	     if (rxVA_baxFORMAT.Rx_TO_cnt != 0){
	    	 rxVA_baxFORMAT.Rx_TO_cnt--;
	       if (rxVA_baxFORMAT.Rx_TO_cnt == 0){
	    	   rxVA_baxFORMAT_ini();
	       }
	     }
	}


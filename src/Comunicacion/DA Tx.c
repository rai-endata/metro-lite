/* File: <Air Tx.c> */

/* INCLUDES */
/************/
#include "DA Tx.h"
#include "DA Communication.h"
#include "DA Define Commands.h"

#include "DTE - Tx.h"
#include "Definicion de tipos.h"
#include "DA Rx.h"
#include "stddef.h"
#include "Timer.h"
#include "Reloj.h"
#include "Inicio.h"
#include "DA Communication.h"
#include "Bluetooth.h"


/*********************************************************************************************/
/* ESTRUCTURAS */
/***************/
typedef enum{
         OK,                           // No hay Error
         ERR_TO,                       // Se cumplio el TimeOut del Lazo
         ERR_NyDF0A,					// No coincide el N con el DF0A
         ERR_Nmin,                     // El N es muy chico (menor al minimo)
         ERR_N_NULO,				// El N es NULO
         ERR_waitTxRta,                // Espero a finalizar la Transmision de la Rta anterior
         ERR_noRx                      // No recibi nada
    }typeERROR;


/*********************************************************************************************/
/* PROTOTIPOS */
/**************/
static void DA_saveTxPUTptr_BKP (void);
static typeERROR DA_TxBuffer_addBuffer (byte* buffer, byte N);
static void DA_restoreTxPUTptr (void);
static void recargar_Reintento_CMD (byte cmd);
static byte chk_TxPendiente (void);
static uint16_t chkSpace_onDA_TxBuffer (void);

/****************************
 *  MACROS Y CONSTANTES
 ***************************/


//#define dim_AirTx       15918
//#define dim_AirTx       15926
#define dim_AirTx       40*112	//(libre , ocupado cobrando usa 112 bytes)


#define DA_TxBuffer_addData(data)  put_byte(&DA_TxPUTptr, data, DA_TxBuffer, (uint16_t) dim_AirTx)


/****************************
 *   VARIABLES
 * **************************/
static byte* DA_TxPUTptr;				    	// Puntero PUT del Buffer de Tx de DATOS
static byte* DA_TxPUTptr;            			// Puntero GET del Buffer de Tx de DATOS
static byte* DA_TxPUTptr_bkp;	      			// [BACKUP] Puntero PUT del Buffer de Tx de DATOS
static byte* DA_TxGETptr;            			// Puntero GET del Buffer de Tx de DATOS
//static byte* DA_TxGETptrBackup;            	// Puntero GET del Buffer de Tx de DATOS
static byte  DA_TxBuffer[dim_AirTx];  			// Buffer de Transmisión de DATOS
static byte reTx_timer_cnt[0x80];     			// Contadores para los reintentos de transmisión de cada comando

typeTxCMD CMD_RTA ={0,OFFSET_RTA,0,0,0,NULL}; // Comando para GENERALIZAR las RESPUESTAS

byte CMD_a_Tx;                 // No de Comando a Transmitir
      #define HAY_COMANDO_PENDIENTE       (CMD_a_Tx != 0)
      #define TX_F_CMD_A_TX               (Tabla_ComandosTx[CMD_a_Tx]->Tx_F)
      #define REINT_CMD_A_TX              (Tabla_ComandosTx[CMD_a_Tx]->Reintentos)
      #define TIME_REINT_CMD_A_TX         (Tabla_ComandosTx[CMD_a_Tx]->timeReint)
      #define N_CMD_A_TX                  (Tabla_ComandosTx[CMD_a_Tx]->N)
      #define BUFFER_CMD_A_TX             (Tabla_ComandosTx[CMD_a_Tx]->buffer)

uint16_t pauseTx_to_cnt;



/* TRANSMITIR AL DATA TERMINAL */
/*******************************/

	void pasarCMDS_BUFFER_to_TxBUFFER (void){
	 uint16_t space;

	  CMD_a_Tx = chk_TxPendiente();       					// Me fijo si hay comandos pendiente

	  if (HAY_COMANDO_PENDIENTE ){

		  //transmitiendo_RTA = 0;            				// Asumo que no voy a transmitir una Rta

		  if(bluetoothCONECTADO){
			  space = chkSpace_onDA_TxBuffer();
			  if( space >3+Tabla_ComandosTx[CMD_a_Tx]->N){
			   Tabla_TxDA[CMD_a_Tx]();
			   TxBufferDA_cntCMD++;       						// Incremento cantidad de comandos copiados a buffer de Tx
			  }
		  }else{
			//SI NO ESTA CONECTADO EL BLUETOOTH, SOLO ACUMULO
			//COMANDOS DE RELOJ
            if(CMD_RELOJ ){
              space = chkSpace_onDA_TxBuffer();
  			  if(space > 3+Tabla_ComandosTx[CMD_a_Tx]->N){
  			   //pasa datos de buffer de comandos a buffer intermedio (DA_TxBuffer)
  			   Tabla_TxDA[CMD_a_Tx]();
  			   TxBufferDA_cntCMD++;       						// Incremento cantidad de comandos copiados a buffer de Tx
  			  }
            }else{
            	clear_TxPendiente();
            }
		  }
		}
	}


/* CHEQUEAR SI HAY ALGUN COMANDO POR TRANSMITIR */
/************************************************/
	static byte chk_TxPendiente (void){
	  // Devuelve el comando que este pendiente por Tx, en caso de haberlo.
	  // Si no hay pendiente, devuelve el comando inválido: CMD_NULL
	  byte exit;
	  uint16_t i;
	  byte cmd;
	  typeTxCMD* cmd_reloj;

	  cmd = CMD_NULL.cmd;                 // Comando no valido (ERROR)

	  exit = 0;                           // No salgo

	  //Priorizo la Respuesta, xq es la primera bandera que miro
	  //if (CMD_RTA.Tx_F){
	  if (TX_F_CMD_A_RESP){

		exit = 1;                         // Salgo xq la Rta esta pendiente
		cmd = CMD_RTA.cmd;                // Extraigo comando de Rta
	  }

	  // Después priorizo Encendido de Equipo
	  if (CMD_Encendido_EQUIPO.Tx_F && !exit){
		exit = 1;                            // Salgo xq el Encendido de Visor esta pendiente
		cmd = CMD_Encendido_EQUIPO.cmd;      // Extraigo comando de Encendido de Visor
	  }

	  // Después de la Respuesta y el Encendido de EQUIPO, priorizo los Comandos de Reloj
	  i = 0;                              // Reseteo valor de cuenta
	  while ((i<(sizeof(Tabla_CMD_Reloj)/4) - 1) && !exit){    // Divido el tamaño /2 xq es una tabla de WORDs
		  cmd_reloj = Tabla_CMD_Reloj[i]; // Cargo comando de RELOJ
		  if (cmd_reloj->Tx_F){
		  exit = 1;                       // Salgo xq hay comando de reloj pendiente
		  cmd = cmd_reloj->cmd;           // Extraigo comando de reloj
		}
		i++;                              // Incremento valor
	  }

	  i = 0;                              // Reseteo valor de cuenta
	  while ((i<(sizeof(Tabla_ComandosTx)/4) - 1) && !exit){    // Divido el tamaño /4 xq es una tabla de punteros (y la direccion es de 32 bits) y le resto 1 xq la Rta la mire antes
		if (Tabla_ComandosTx[i]->Tx_F){
		  exit = 1;                       // Salgo xq encontré comando con Tx Pendiente
		  cmd = Tabla_ComandosTx[i]->cmd; // Extraigo ese comando
		}
		i++;                              // Incremento valor
	  }
	  return(cmd);
	}



	void clear_TxPendiente (void){
	  // Devuelve el comando que este pendiente por Tx, en caso de haberlo.
	  // Si no hay pendiente, devuelve el comando inválido: CMD_NULL
	  uint16_t i;
	  byte cmd;
	  typeTxCMD* cmd_reloj;

	  i = 0;                              // Reseteo valor de cuenta
	  while ((i<(sizeof(Tabla_ComandosTx)/4))){    // Divido el tamaño /4 xq es una tabla de punteros (y la direccion es de 32 bits)

		if (Tabla_ComandosTx[i]->Tx_F){
			//los comandos de reloj no los borro
			if( (i<24) || (i > 30) ){
				Tabla_ComandosTx[i]->Tx_F = 0;                       // Salgo xq encontré comando con Tx Pendiente
			}
		}
		i++;
	  }
	}


/* DETERMINAR ESPACIO DISPONIBLE EN BUFFER DE Tx */
/*************************************************/
	static uint16_t chkSpace_onDA_TxBuffer (void){
			  uint16_t space;
			  byte* GET;
			  byte* PUT;

			  GET = DA_TxGETptr;
			  PUT = DA_TxPUTptr;
			  if ((PUT == GET) && (TxBufferDA_cntCMD > 0)){

				// Si los punteros GET y PUT son iguales y HAY paquetes encolados (N != 0)
				// => No hay mas lugar
				space = 0;

			  }else if (GET > PUT){
				/*
						 +---------+
						 |         |
						 |         |
				  PUT -> |         |
						 |*********|
						 |*********|
						 |*********|
						 |         | -> GET
						 |         |
						 +---------+


					=> SPACE = GET - PUT
				*/
				space = GET - PUT;

			  }else{
				/*
						 +---------+
						 |*********|
						 |*********|
						 |         | -> GET
						 |         |
						 |         |
						 |         |
				  PUT -> |         |
						 |*********|
						 +---------+


					=> SPACE = DIM - (PUT - GET) = DIM - PUT + GET
				*/
				space = dim_AirTx;
				space -= (PUT - GET);
			  }

			  return(space);
	}

/* TRANSMISION GENERICA DE RESPUESTA */
/*-----------------------------------*/
void Tx_RTA_toDA (void){
	// Rutina que envía una respuesta. La respuesta puede ser SIMPLE o con BUFFER
	// de acuerdo al valor de la propiedad ".buffer" del typeTxCMD
	typeERROR error;

	if (Tx_RTA_F){
		// Por seguridad, verifico que la bandera de Tx de la respuesta este en alto.
		// No bajo la bandera ahora, sino que lo hago luego de copiar los datos al buffer
		// de Tx, dado que si llegan 2 comandos (que requieren respuesta con datos) juntos
		// y todavía no termine de armar una de las respuestas y proceso el nuevo comando
		// que llego, cambio los valores de CMD_A_RESP y después voy a responder con
		// Cualquier buffer; xq en esta rutina dependiente del valor de CMD_A_RESP selecciono
		// el buffer que voy a enviar como respuesta.
		// Por ejemplo, llega consulta posición y consulta estado juntas. Si antes de copiar
		// el buffer de posición al buffer de Tx, hago tiempo a procesar el comando de consulta
		// de estado; cambio el valor de CMD_A_RESP y respondo la consulta de datos con el
		// buffer de estado. Si bien es poco probable, demasiado poco, de esta manera me
		// aseguro que eso no pase => lo que hago es lo siguiente:
		//  1- Bajo la bandera de Tx Rta luego de copiar los datos al buffer de Tx
		//
		//  2- en Rx Central, si esta levantada la bandera de Tx Rta, y recibo un comando que
		//     requiere Rta con datos, no lo proceso hasta que no haya bajado la bandera de
		//     Tx Rta
		error = OK;                                   // Asumo que no va a haber error

		DA_saveTxPUTptr_BKP();                       // Guardo Puntero PUT de Tx

		DA_TxBuffer_addData(getN_CMD_A_RESP);        // Si bien es FIJO, lo extraigo del COMANDO

		//DA_TxBuffer_addData(NRO_MOVIL_H);            // Nro Móvil HIGH
		//DA_TxBuffer_addData(NRO_MOVIL_L);            // Nro Móvil LOW

		DA_TxBuffer_addData(getCMD_A_RESP + 0x80);   // Comando-Respuesta

		if (getBUFFER_A_RESP != NULL){
		  // Respuesta con Datos -> El Fin de Datos debe estar incluido en los datos para
		  // poder hacer el correcto traslado de datos al buffer de Tx. De todos modos, en
		  // el traslado el fin de datos no se copia, sino que por eso se agrega al final
		  error = DA_TxBuffer_addBuffer(getBUFFER_A_RESP, getN_CMD_A_RESP);  // Buffer + Fin de Datos
		}

		// Fin de Datos
		DA_TxBuffer_addData(fin_datos_msb);      // 0xDF
		DA_TxBuffer_addData(fin_datos_lsb);      // 0x0A

		clrTx_RTA_F;	                            // Bajo Bandera de Tx, luego de copiar datos a buffer de Tx

		if (error == OK){
		  // Si no hay error comando la Tx. El error se puede dar por
		  // TimeOut del Lazo que carga el buffer o por error en el N
		  // al cargar datos desde el buffer: No coincide N con DF0A.
		  transmitiendo_RTA = 1;                  // Voy a Transmitir una Respuesta

		  DA_requestTx();            			 // Inicia Transmisión de datos

		}else{
		  DA_restoreTxPUTptr();                  // Reestablezco puntero PUT
		}
	}
}




  /* TRANSMISION GENERICA DE BUFFER, EXTRAYENDO EL "N" DEL  MISMO */
  /*---------------------------------------------------------------*/
/*
void Tx_BUFFER_getN (void){
        // Rutina que transmite un BUFFER de datos. El buffer debe finalizar en DF0A.
        // El N a transmitir es parte del los datos del buffer, es el 1º dato en el
        // mismo.
        typeERROR error;

        if (TX_F_CMD_A_TX){
          // Por seguridad, verifico que la bandera de Tx del comando que vaya a
          // transmitir este en alto.
          TX_F_CMD_A_TX = 0;	                        // Bajo Bandera de Tx

          DA_saveTxPUTptr_BKP();                     // Guardo Puntero PUT de Tx

          DA_TxBuffer_addData(BUFFER_CMD_A_TX[0]);   // Longitud del comando
          DA_TxBuffer_addData(NRO_MOVIL_H);          // Nro Móvil HIGH
          DA_TxBuffer_addData(NRO_MOVIL_L);          // Nro Móvil LOW
          DA_TxBuffer_addData(CMD_a_Tx);             // Comando
          error = DA_TxBuffer_addBuffer(&BUFFER_CMD_A_TX[1], BUFFER_CMD_A_TX[0]);  // Buffer + Fin de Datos

          if (error == OK){
            // Si no hay error, agrego fin de datos y comando la Tx.
            // El error se puede dar por TimeOut del Lazo que carga el
            // buffer o por error en el N al cargar datos desde el
            // buffer: No coincide N con DF0A.

            // Fin de Datos
            DA_TxBuffer_addData(fin_datos_msb);      	// 0xDF
            DA_TxBuffer_addData(fin_datos_lsb);      	// 0x0A

            recargar_Reintento_CMD(CMD_a_Tx);         	// Recargo reintentos

            DA_requestTx();             				// Inicia Transmisión de datos al Aire via SATELITECH

          }else{
            DA_restoreTxPUTptr();                    	// Reestablezco puntero PUT
          }
        }
      }
*/

      /* TRANSMISION GENERICA DE BUFFER */
       /*--------------------------------*/
         void Tx_BUFFER (void){
           // Rutina que transmite un BUFFER de datos. El buffer debe finalizar en DF0A.
           //
           // EDIT 25/04/2013
           // Se agrego compilación condicional para trasmisión de numero de reintentos
           typeERROR error;

           byte N,a1,a2;
           byte* b1,b2,b0;

           if (TX_F_CMD_A_TX){
             // Por seguridad, verifico que la bandera de Tx del comando que vaya a
             // transmitir este en alto.
             TX_F_CMD_A_TX = 0;	                        // Bajo Bandera de Tx

             DA_saveTxPUTptr_BKP();                     // Guardo Puntero PUT de Tx

             N = N_CMD_A_TX;

             DA_TxBuffer_addData(N);                    // Longitud del COMANDO


             //DA_TxBuffer_addData(NRO_MOVIL_H);          // Nro Móvil HIGH
             //DA_TxBuffer_addData(NRO_MOVIL_L);          // Nro Móvil LOW
             //N = N-2;									  //porque elimine nro de movil

             DA_TxBuffer_addData(CMD_a_Tx);               // Comando

             error = DA_TxBuffer_addBuffer(BUFFER_CMD_A_TX, N_CMD_A_TX);  // Buffer + Fin de Datos

             if (error == OK){
               // Si no hay error, agrego fin de datos y comando la Tx.
               // El error se puede dar por TimeOut del Lazo que carga el
               // buffer o por error en el N al cargar datos desde el
               // buffer: No coincide N con DF0A.

               // Fin de Datos
               DA_TxBuffer_addData(fin_datos_msb);      	// 0xDF
               DA_TxBuffer_addData(fin_datos_lsb);      	// 0x0A

               if (CMD_a_Tx != CMD_comandoMENSAJE){
            	   recargar_Reintento_CMD(CMD_a_Tx);         	// Recargo reintentos
               }


               DA_requestTx();             				 	// Inicia Transmisión de datos al Aire

             }else{
               DA_restoreTxPUTptr();                    	// Reestablezco puntero PUT
             }
           }
         }


/* GUARDAR BACKUP DEL PUNTERO PUT DE Tx DE DA */
/************************************************/
  static void DA_saveTxPUTptr_BKP (void){
	DA_TxPUTptr_bkp = DA_TxPUTptr;
  }

  /* AGREGAR BUFFER AL BUFFER DE Tx DE DA */
   /******************************************/
     static typeERROR DA_TxBuffer_addBuffer (byte* buffer, byte N){
       // Agregar datos de un buffer LINEAL al buffer de DA
       byte TO_F;
       typeERROR error;
       byte N_chk;

       N = N - N_CMD;                  // Resto MOV_H, MOV_L, CMD
       N_chk = 0;                      // Reseteo Control de N

       error = OK;                     // Asumo que no hay error

       dispararTO_lazo();              // Disparo TimeOut Lazo
       TO_F = 0;                       // Limpio Bandera
       while ((*buffer != fin_datos_msb) || ((*(buffer+1) != fin_datos_lsb) && !TO_F)){
         TO_F = chkTO_lazo_F();        // Chequeo bandera de time out de lazo
         DA_TxBuffer_addData(*buffer++);
         N_chk++;                      // Voy Incrementando Control de N
       }
       detenerTO_lazo();               // Detener TimeOut Lazo

       if (TO_F){
         // ERROR --> Time Out de Lazo
         error = ERR_TO;

       }else if (N_chk != N){
         // ERROR --> No coincide N con DF0A
         error = ERR_NyDF0A;
       }

       return (error);
     }

/* RESTAURAR PUNTERO PUT DE Tx DE DA */
/***************************************/
	static void DA_restoreTxPUTptr (void){
	  DA_TxPUTptr = DA_TxPUTptr_bkp;
	}

/* RECARGAR REINTENTOS DE COMANDO */
/**********************************/
	static void recargar_Reintento_CMD (byte cmd){
	  // Como el nro de comando coincide con el orden el la tabla,
	  // cargo el contador de reintentos asociado al comando y tb
	  // modifico los reintentos del coma
	  if (Tabla_ComandosTx[cmd]->Reintentos > 0){
		if (Tabla_ComandosTx[cmd]->Reintentos != reintINFINITOS){
		  Tabla_ComandosTx[cmd]->Reintentos--;                  // Decremento Reintentos
		}

		reTx_timer_cnt[cmd] = Tabla_ComandosTx[cmd]->timeReint; // Cargo tiempo entre Reintentos

	  }
	}



/* EXTRAER DATO DEL BUFFER DE Tx DE DA */
/*****************************************/
	byte DA_TxBuffer_get (byte* TxGETptr){
	  // Extrae dato del buffer de Tx e incrementa el puntero
		return(get_byte(&TxGETptr, DA_TxBuffer, (uint16_t) dim_AirTx));
	}


/* EXTRAER PUNTERO GET DEL BUFFER DE Tx DE DA */
/**********************************************/
	byte* DA_TxBuffer_getGETptr (void){

	  return(DA_TxGETptr);
	}

/* EXTRAER PUNTERO PUT DEL BUFFER DE Tx DE DA */
/**********************************************/
	byte* DA_TxBuffer_getPUTptr (void){

	  return(DA_TxPUTptr);
	}



/* SETEA PUNTERO GET DEL BUFFER DE Tx DE DA */
/**********************************************/
	void DA_TxBuffer_setGETptr (byte* ptrDA){

	  DA_TxGETptr = ptrDA;
	}


/* EXTRAER DATO DEL BUFFER DE Tx DE DA */
/*****************************************/
	byte DA_TxBuffer_getData (void){
	  // Extrae dato del buffer de Tx e incrementa el puntero
		return(get_byte(&DA_TxGETptr, DA_TxBuffer, (uint16_t) dim_AirTx));

	}

/* INICIALIZAR BUFFER DE Tx POR DA */
/************************************/
	void DA_iniTx (void){
		DA_TxGETptr = DA_TxBuffer;
		DA_TxPUTptr = DA_TxBuffer;
		TxBufferDA_cntCMD = 0;
		DA_Txing = 0;
	}

	/* ANULAR TRANSMISION DE COMANDO */
	/*********************************/
	void anularTx_cmd (byte cmd){
		// Rutina que anula la transmision del comando pasado como
		// argumento

		reTx_timer_cnt[cmd] = 0;                // Detengo Timer Reintentos
		Tabla_ComandosTx[cmd]->Tx_F = 0;        // Bajo bandera de Transmision
		Tabla_ComandosTx[cmd]->Reintentos = 0;  // Agoto Reintentos
	}

	void anularReTx_RELOJ (void){
			// Rutina que anula la transmision del comando de reloj

			byte i;

			i = 0;

			while(i < 7){
			    reTx_timer_cnt[CMD_RELOJ_Pase_a_LIBRE+i] = 0;                // Detengo Timer Reintentos
				Tabla_ComandosTx[CMD_RELOJ_Pase_a_LIBRE+i]->Tx_F = 0;        // Bajo bandera de Transmision
				Tabla_ComandosTx[CMD_RELOJ_Pase_a_LIBRE+i]->Reintentos = 0;  // Agoto Reintentos
				i++;
			}
			// Detengo Timer Reintentos
			reTx_timer_cnt[CMD_RELOJ_Pase_a_LIBRE_SC] = 0;
			reTx_timer_cnt[CMD_RELOJ_Pase_a_OCUPADO_APP_SC] = 0;
			reTx_timer_cnt[CMD_RELOJ_Pase_a_OCUPADO_SA_SC] = 0;
			reTx_timer_cnt[CMD_RELOJ_Pase_a_COBRANDO_SC] = 0;
			// Bajo bandera de Transmision
			Tabla_ComandosTx[CMD_RELOJ_Pase_a_LIBRE_SC]->Tx_F = 0;
			Tabla_ComandosTx[CMD_RELOJ_Pase_a_OCUPADO_APP_SC]->Tx_F = 0;
			Tabla_ComandosTx[CMD_RELOJ_Pase_a_OCUPADO_SA_SC]->Tx_F = 0;
			Tabla_ComandosTx[CMD_RELOJ_Pase_a_COBRANDO_SC]->Tx_F = 0;
			// Agoto Reintentos
			Tabla_ComandosTx[CMD_RELOJ_Pase_a_LIBRE_SC]->Reintentos = 0;
			Tabla_ComandosTx[CMD_RELOJ_Pase_a_OCUPADO_APP_SC]->Reintentos = 0;
			Tabla_ComandosTx[CMD_RELOJ_Pase_a_OCUPADO_SA_SC]->Reintentos = 0;
			Tabla_ComandosTx[CMD_RELOJ_Pase_a_COBRANDO_SC]->Reintentos = 0;
		}


    void pauseTx_TIMEOUT (void){
          // Ejecutada cada 10 mseg
          if (pauseTx_to_cnt != 0){
        	  pauseTx_to_cnt--;
            if (pauseTx_to_cnt == 0){
            	pauseTx=0;
            }
          }
     }

    /* TIMER REINTENTOS DE TRANSMISION */
     /***********************************/
       void reTx_timer (void){
         // Ejecutada cada 1seg de forma no precisa
         byte i;
         if(bluetoothCONECTADO){
             for (i=0; i<0x80; i++){
               if (reTx_timer_cnt[i] != 0){
                 reTx_timer_cnt[i]--;
                 if (reTx_timer_cnt[i] == 0){
                   Tabla_ComandosTx[i]->Tx_F = 1;  // Levanto bandera de Transmision
                 }
               }
             }
         }
       }


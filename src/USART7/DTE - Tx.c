#include "DTE - Tx.h"
#include "usart7.h"
#include "Protocolo BAX.h"
#include "Manejo de Buffers.h"
#include "Timer.h"
#include "stm32f0xx_hal_uart.h"
#include "Bluetooth.h"
#include "DA Tx.h"
#include "DA Tx.h"
#include "Lista Comandos.h"
#include "DA Define Commands.h"
#include "Air Update.h"
#include "Comandos sin conexion a central.h"
#include "Programacion Parametros.h"


#define dim_DATx       150


void DA_Tx (void);


static void copyDA_TxBuffer_to_LinealBuff (byte* dest_ptr);


byte TxBufferDA_cntCMD;        // Contador de cantidad de COMANDOS copiados al buffer de Tx

/***************************************************
 * 					FUNCIONES
 ***************************************************/

/* DA_Tx: Toma los datos del buffer DA_TxBuffer donde se acumulan los comandos a enviar  y los pone en el buffer de salida de la sci para enviarlos
 * al dispositivo android.
 *
 * DA_TxBuffer -> va_txDATA_buffer

		va_txDATA_buffer

		+-------+					-
		|       | <- va_txGETptr	|
		+-------+					|
		|       | <- va_txPUTptr  	|
		+-------+					| <- dim_va_txDATA
		|       |					|
		+-------+					|
		|       |					|
		+-------+					-

		va_tx_cntDATOS : Cantidadd de comandos en DA_TxBuffer
		va_tx_cntBYTE:   cantidad de bytes en el buffer

  * */

void DA_Tx (void){

	byte tempTxBuffer[dimMAX+10];
	byte linealAirTx[dimMAX+10];
	byte error;
	byte N,data,i;
	uint16_t availableSpace;
	byte* DA_TxGET;
	byte* DA_TxPUT;
	byte* DA_TxGETBackup;
	static byte CMD_a_Tx;


	DA_TxGET = DA_TxBuffer_getGETptr();
	DA_TxPUT = DA_TxBuffer_getPUTptr();

	// && (datosSC_cntBYTE == 0)

	//if (!pauseTx && bluetoothCONECTADO && DA_doTx && (!DA_Txing ) && (DA_TxGET != DA_TxPUT) && (TxBufferDA_cntCMD>0 ) ){
	if ((!pauseTx && bluetoothCONECTADO && DA_doTx && (!DA_Txing ) && (TxBufferDA_cntCMD>0 ))){
	//if ((bluetoothCONECTADO && DA_doTx && (!DA_Txing ) && (TxBufferDA_cntCMD>0 ))){
		 //para que no se demore en responder al programador
		 //|| ((prgRELOJ_COMUNES_OK_F && DA_doTx)	&& (TxBufferDA_cntCMD>0 ))){
	//if ((!pauseTx || (CMD_a_Tx == 128)) && bluetoothCONECTADO && DA_doTx && (!DA_Txing ) && (TxBufferDA_cntCMD>0 ) ){

		//copia datos de DA_TxBuffer a un buffer lineal
		DA_TxGETBackup = DA_TxBuffer_getGETptr();
		copyDA_TxBuffer_to_LinealBuff(linealAirTx);

		//verifico si en buffer de salida hay espacio disponible
		N = *linealAirTx;

		//availableSpace = chkSpace_onSCI_TxBuffer();
		availableSpace = chkSpace_BuffCIR(&va_txDATA);
		if (availableSpace >= (N+3)){
			//le agrego el protocolo bax y 0x0d 0x0a al final
			error = BAX_formatting(*linealAirTx, linealAirTx+1, tempTxBuffer);
			if (!error){
			// En tempTxBuffer, voy a tener el formato " START | N | CHK | DATA | STOP |0x0D|0x0A "
			//copio datos a transmitir en buffer de salida
				i=0;
				//while(i <= N+3){
				//en lugar de 03 como fin, uso 0d 0a por eso un byte mas
				N = N+1;
				while(i <= N+4){
				  //tomo data de buffer del DTE
				  data = tempTxBuffer[i++];
				  //Envió dato al buffer de salida
				  //put_SCI_TxBuffer(data);
				  putBUFCIR (data, &va_txDATA);

				}
				TxBufferDA_cntCMD--;
				DA_Txing = 1;
				CMD_a_Tx = tempTxBuffer[3];
				//pauseTx = 1;
				//if(CMD_RELOJ){
					//pauseTx_to_cnt = PAUSE_TX_LARGA;
				//}else{
					//pauseTx_to_cnt = PAUSE_TX_CORTA;
				//}
				pauseTx = 1;
				pauseTx_to_cnt = PAUSE_TX;

				HAL_UART_Transmit_IT(&huart7, va_txDATA.get, N+3);	// Habilito Transmisión
			}else{
				//restauro el puntero
				 DA_TxBuffer_setGETptr (DA_TxGETBackup);
			}
		}else{
			//restauro el puntero
			DA_TxBuffer_setGETptr (DA_TxGETBackup);
		}
	}
}



  /* INICIAR TRANSMISION DE DATOS POR SERIE DE DTE */
  /*******************************************************/



/* COPIAR CADENA DE DATOS A ENVIAR DESDE EL BUFFER DE Tx DE AIRE */
  /*****************************************************************/
    static void copyDA_TxBuffer_to_LinealBuff (byte* dest_ptr){
			// Como el buffer de AIR Tx no es LINEAL, acá lo linealizamos
      byte TO_F, exit;
      byte data_1, data_2;
      byte N;

      TO_F = 0;                                     // Limpio Bandera
      exit = 0;                                     // Reseteo variable
      data_1 = DA_TxBuffer_getData();              // Extraigo dato a enviar
      N = data_1 + 1;                               // Le sumo 1 xq tb agrego el N
      dispararTO_lazo();                            // Disparo TimeOut Lazo
      while (!exit && !TO_F){
        TO_F = chkTO_lazo_F();                      // Chequeo bandera de time out de lazo

        data_2 = DA_TxBuffer_getData();            // Extraigo dato a enviar

        if ((data_1 == finSCI_H) && (data_2 == finSCI_L)){
          // Encontré un DF0A => Si N=0 termine de extraer los datos. Sino
          // continuo justamente porque el DF0A son datos y no fin
         // if (N == 0){
            exit = 1;                               // Salgo por fin
         // }
        }

        if (!exit){
          *dest_ptr++ = data_1;
          N--;
          data_1 = data_2;
        }
      }
      detenerTO_lazo();                             // Detener TimeOut Lazo

      *dest_ptr++ = finSCI_H;                       // Agrego Fin
      *dest_ptr++ = finSCI_L;                       // Agrego Fin
    }

    void DA_requestTx (void){
          DA_doTx = 1;
    }

    void UART_Tx_VA(UART_HandleTypeDef *huart, buffcirTYPE* DATA){

    	byte dataTx;

    	if (DATA->get == DATA->put){
    		__HAL_UART_DISABLE_IT(huart, UART_IT_TXE);
    		__HAL_UART_DISABLE_IT(huart, UART_IT_TC);
    		huart->State = HAL_UART_STATE_BUSY_RX;
    		huart->Tx_TO_cnt = 0;

    		//printPRUEBA(1);

    		//analizo que dispositivo serie finalizo la transmisión
    		if(huart == &huart7){
    			//termino la transmison al visor android
    			DA_Txing = 0;      //Finalice la transmisión.
   			    transmitiendo_RTA = 0;            				// Asumo que no voy a transmitir una Rta
    			if(cmd_resp == 0xE0){
    				if(waitToTx_upDateSuccess){
    					waitToTx_upDateSuccess = 0;
    					updateFinRTA_LOCAL = 1;
    				}
    			}else if(cmd_resp == 0xc0){
    			 //se temino de enviar una respuesta de comando transparente
    				if(waitToTx_upDateSuccess){
    					waitToTx_upDateSuccess = 0;
    					updateFinRTA_TRANSPARENTE = 1;
    				}

    			}
    			cmd_resp = 0xff;
    		}

    	}else{
    		dataTx = getBUFCIR (DATA);
    		huart->Instance->TDR = (uint8_t)(dataTx & (uint8_t)0x00FFU);
    		huart->Tx_TO_cnt = UART_TX_TIME_OUT;
    	}
    }


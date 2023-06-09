/* File: <DA Tx.h> */

#ifndef _DA_TX_
  #define _DA_TX_
#include "DA Tx.h"
#include "stdint.h"
#include "DA Communication.h"
#include "Definicion de tipos.h"

/*********************************************************************************************/
/* ESTRUCTURAS */
/***************/

    typedef union{
    byte Byte;
    struct{
      byte  tx_rta				:1;       // Indica que lo que se transmitio fue RTA
      byte  pause_tx			:1;		  // pausa entre comando y comando
      byte  :1;
      byte  :1;
      byte  :1;
      byte  :1;
      byte  :1;
      byte  :1;
    }Bits;
  }CTRLTx_FLAGS;



#define NRO_MOVIL_H		0x08
#define NRO_MOVIL_L		0x09


/*********************************************************************************************/
/* VARIABLES */
/*************/

 // extern typeTxCMD CMD_RTA;

CTRLTx_FLAGS  _ctrlTx_Central_F1;     // Banderas de Control de Transmisión 1
	#define ctrlTx_Central_F1						_ctrlTx_Central_F1.Byte
	#define transmitiendo_RTA           _ctrlTx_Central_F1.Bits.tx_rta
	#define pauseTx                     _ctrlTx_Central_F1.Bits.pause_tx


 extern uint16_t pauseTx_to_cnt;
#define PAUSE_TX				15			//150 mseg de pausa entre comando y comando
#define PAUSE_TX_CORTA			100			//2 seg de pausa entre comando y comando para prueba
#define PAUSE_TX_LARGA			500			//2 seg de pausa entre comando y comando para prueba

/* RUTINAS */
/***********/

	extern void pasarCMDS_BUFFER_to_TxBUFFER (void);

	extern void Tx_RTA_toDA (void);

	extern void DA_iniTx (void);
	extern byte DA_TxBuffer_get (byte* TxGETptr);
	extern byte* DA_TxBuffer_getGETptr (void);
	extern byte DA_TxBuffer_getData (void);
	extern byte* DA_TxBuffer_getPUTptr (void);
	extern void DA_TxBuffer_setGETptr (byte* ptrDA);
	extern void anularTx_cmd (byte cmd);
	extern void clear_TxPendiente (void);
	extern void pauseTx_TIMEOUT (void);
	extern void reTx_timer (void);
	extern void anularReTx_RELOJ (void);

	extern byte Rx_DA_getNRta (void);
	 #define getN_CMD_A_RESP   (Rx_DA_getNRta())

	//extern void finTxRta_actions (void);
	extern byte cmd_resp;


#endif

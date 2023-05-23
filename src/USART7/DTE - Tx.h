//#include "TypeDef.h"


#ifndef	_DTE_TX_
#define _DTE_TX_

#include "stm32f0xx_hal_uart.h"
#include "Definicion de tipos.h"
#include "Manejo de Buffers.h"
#include "Protocolo BAX.h"

/*********************************************************************************************/
/* ESTRUCTURAS */
/***************/
  // Banderas de DTE
  typedef union{
    byte Byte;
    struct{
      byte doTx     :1;		  // Solicita inicio de transmision
      byte doRx     :1;       // Indica que se deben pre-procesar los datos recibidos
      byte chDatos  :1;       // Indica que esta en canal de DATOS
      byte noCx     :1;       // Indica que no tengo comunicacion GPRS
      byte Txing	:1;		  // Indica que esta en transmision
    }Bits;
  }tDTE_FLAGS;


	tDTE_FLAGS            	_DTE_F;

	#define DA_F              	_DTE_F.Byte
	#define DA_doTx           	_DTE_F.Bits.doTx
	#define DA_doRx           	_DTE_F.Bits.doRx
	#define DA_chDATOS        	_DTE_F.Bits.chDatos
	#define DA_noCx          	_DTE_F.Bits.noCx
	#define DA_Txing      		_DTE_F.Bits.Txing





	/**************
   * PROTOTIPOS *
   **************************************************************************/

  extern void DA_Tx (void);


  void DA_requestTx (void);

  /**************
   * VARIABLES  *
   **************************************************************************/

  extern byte TxBufferDA_cntCMD;        // Contador de cantidad de COMANDOS copiados al buffer de Tx
  extern void UART_Tx_VA(UART_HandleTypeDef *huart, buffcirTYPE* DATA);


#endif


#ifndef   _VA_UART__
  #define _VA_UART_

  #include "main.h"
  #include "stm32f0xx_hal_uart.h"
  #include "Definicion de tipos.h"
  #include "Manejo de Buffers.h"




 
/* *************************
 * DEFINICION DE VARIABLES *
 ***********************************************************************************************/

//#define USART7_dataRxSave(sci_data, huart);	 	putBUFCIR (sci_data, &va_rxDATA);
#define USART7_dataRxSave(huart, sci_data);	 	UART_Rx_ISR (&huart7,sci_data);
#define USART7_dataTxSave();	 				UART_Tx_ISR (&huart7);

  #define 	dim_va_rxDATA				600
  extern byte 		va_rxDATA_buffer[dim_va_rxDATA];			//buffer de recepción serie del visor android
  extern buffcirTYPE va_rxDATA;
	#define va_iniRxBuffer	 va_rxDATA.iniBUFFER
	#define va_rxGETptr      va_rxDATA.get
	#define va_rxPUTptr      va_rxDATA.put
	#define va_rx_cntBYTE    va_rxDATA.cntBYTE
	#define va_rx_cntDATOS   va_rxDATA.cntDATOS
	#define va_rxSizeBUFFER  va_rxDATA.sizeBUFFER

  #define 	dim_va_txDATA				600
  extern byte 		va_txDATA_buffer[dim_va_txDATA];
  extern buffcirTYPE va_txDATA;				//buffer de transmisión serie del visor android
	#define va_iniTxBuffer   va_txDATA.iniBUFFER
	#define va_txGETptr      va_txDATA.get
	#define va_txPUTptr      va_txDATA.put
	#define va_tx_cntBYTE    va_txDATA.cntBYTE
	#define va_tx_cntDATOS   va_txDATA.cntDATOS
	#define va_txSizeBUFFER  va_txDATA.sizeBUFFER

  extern void USART7_Ini (void);


    

 
#endif

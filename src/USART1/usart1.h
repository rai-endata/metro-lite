/*
 * uart1.h
 *
 *  Created on: 15/9/2017
 *      Author: Rai
 */

#ifndef _UART1_H_
#define _UART1_H_

#include "stdint.h"

//#define USART1_dataRxSave(sci_data, huart);	 	PRINT_SCI_RxISR(sci_data);
//#define USART1_dataTxSave();	 				PRINT_SCI_TxISR(&huart1);

//#define USART1_dataRxSave(sci_data, huart);		UART_Rx_PRG(sci_data);
#define PRINT_DEVICE   0
#define PROG_DEVICE    1

extern uint8_t choice_device_uart1;

#define USART1_dataRxSave(huart, sci_data);		UART_Rx_ISR (&huart1,sci_data);
#define USART1_dataTxSave();					UART_Tx_ISR (&huart1);

extern void USART1_Ini(void);

#endif /* USART3_UART3_H_ */

/* File <Print UART.h> */

#ifndef _Print_UART_
  #define _Print_UART_

  /* INCLUDE */
  /***********/
#include "Definicion de tipos.h"
#include "stm32f0xx_hal_uart.h"
#include "main.h"
#include "- PRINT Config -.h"


  //extern void USART1_Ini (void);
  extern void PRINT_SCI_TxISR (UART_HandleTypeDef *huart);
  extern void PRINT_SCI_RxISR(byte sci_data);
  extern void PRINT_ptrIni (void);

  extern byte PRINT_RxBuffer[dimPRINT_Rx];

  extern void clearPRINT_pendingCNT (void);

  //extern void MX_USART1_UART_Init(void);
  //extern void MX_GPIO_Init(void);

	/* VARIABLES */
  /*************/

  extern uint16_t PRINT_N_Tx;


  #define AT_END_H				0x0D
  #define AT_END_L				0x0A

#define NO_TXING_PRINTER  (PRINT_N_Tx == 0)
#define NO_RXING_PRINTER  (PRINT_pendingCNT == 0)
#define exit_PRINT_send      (timeout_PRINT_send > 3)


	


  /* RUTINAS */
  /***********/

		extern void PRINT_send (byte* CMD, uint16_t N);
		extern void PRINT_send_modeAT (byte* CMD, byte N);
		extern void PRINT_send_debug (byte* CMD);
		
		extern void PRINT_clrSendTO (void);
		extern byte PRINT_chkSendTOF (void);
		
		extern void PRINT_sendHEX (byte* CMD, uint16_t N);
		extern void PRINT_preProcess (void);
		extern byte PRINT_Rx_compare(byte* CMD);
		extern byte* PRINT_getRx_ptr (void);
		extern byte PRINT_chkNEW (void);
		extern void PRINT_doneNEW (void);
		extern void PRINT_reint (byte cnt);
		extern void timeout_PRINT_send_TIMER (void);

    extern void SCI1_Cx_TO (void);

  /* TIMER */
  /*********/
    extern void PRINT_newCMD_TO_timer (void);
    extern void PRINT_sendTO_timer (void);


#endif

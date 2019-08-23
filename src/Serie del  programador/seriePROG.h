/* File: <seriePROG.h> */

#ifndef __seriePROG__
  #define __seriePROG__
  
  /* INCLUDE */
  /***********/
    //#include "Air Tx.h"
    #include "Protocolo BAX.h"
	#include "stm32f0xx_hal_uart.h"
	#include "stm32f0xx_hal.h"
	#include "usart1.h"

  
  /* ESTRUCTURAS */
  /***************/
    // TIPO DE DATOS PARA SERIE DE RELOJ/PROGRAMADOR
#define SCI_tipoRLJ   0xAA        // Indica que uso SCI como RELOJ
#define SCI_tipoPRG   0xBB        // Indica que uso SCI como PROGRAMADOR
#define seriePROG_endTx      0xFF00

/*
    typedef struct{
      byte tipoRx;                      // Indica si lo que RECIBO es de RELOJ o PROGRAMADOR
      byte tipoTx;                      // Indica si lo que TRANSMITO es de RELOJ o PROGRAMADOR
      tBAX_SCI Prog;                    // Datos de SCI de PROGRAMADOR
    }tRLJ_PRG_BAX_SCI;
*/



  /* VARIABLES */
  /*************/
    #define PRG_BAXstart  0x04          // Start de PROGRAMADOR para protocolo BAX

    extern tBAX_SCI BAX_SCI_PROG;         			// Datos de SCI segun protocolo BAX para RELOJ y PROG

  /* RUTINAS */
  /***********/
    extern void PRG_ini (void);
    extern void PRG_startTx (byte N,byte* data);
    extern void PROGRAMADOR_chkTx (void);
    
    //UART

    extern word PRG_UART_TxNewData (void);
    
    extern void UART_Rx_PRG (byte dato);
    extern void UART_Tx_PRG(UART_HandleTypeDef *huart);
    extern void UART_PROG_IniTx(void);
    extern void UART_PROG_IniRx(void);


  
  /* TIMER */
  /*********/
    extern void PRG_TO_serieRx (void);
  
#endif


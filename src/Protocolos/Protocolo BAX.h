/* File: <Protocolo BAX.h> */


#ifndef _PROTOCOLO_BAX_
  #define _PROTOCOLO_BAX_
  
  /* INCLUDE */
  /***********/
    //#include "- Reloj-Blue Config -.h"
	#include "- metroBLUE Config -.h"
	#include "Definicion de tipos.h"
	#include "BAX Config.h"
    #include "stdint.h"
    
  /* ESTRUCTURAS */
  /***************/
    // TIPO DE DATOS DE SERIE PARA PROTOCOLO BAX
    typedef struct{
      byte  start;                      // Carácter de START
      byte  RxBuffer[dim_BAX];          // Buffer de Rx
      byte* RxGPA[dim_BAX_GPA];         // Arreglo de Punteros GET de Rx
      byte  RxGPA_GETidx;               // Indice GET para extraer del GPA
      byte  RxGPA_PUTidx;               // Indice PUT para ingresar al GPA
      byte* RxPUT;                      // Puntero PUT de Rx
      byte* RxPUT_bkp;                  // BackUp Puntero PUT de Rx
      byte  Rx_TO_cnt;                  // Contador del TimeOut de Rx
      byte  Rx_cnt;                     // Contador Recepción Exitosa
      byte  Rx_fin;                     // Bandera de Recepción Exitosa
      byte  RxFull;                     // Bandera que indica que no hay lugar en Buffer de Rx
      byte  TxBuffer[dim_BAX];			// Buffer de Tx
      byte* TxGET;                      // Puntero GET de Tx
      byte* TxPUT;						// Puntero PUT de Tx
      byte  Tx_cnt;                     // Contador de Transmisiones pendientes
      byte  Txing;                      // Bandera que indica que esta transmitiendo
      byte  TxFull;                     // Bandera que indica que no hay lugar en Buffer de Tx
    }tBAX_SCI;
    
    // ETAPAS DEL PROTOCOLO BAX
    typedef enum{
      BAX_START,
      BAX_N,
      BAX_CHKSUM,
      BAX_DATA,
      BAX_STOP
    }tBAX;
    
    typedef tBAX_SCI  tBAX_SPI;

    // Tipo Puntero de Etapas BAX
    typedef tBAX (*BAXptr_type)(byte stage, tBAX_SCI* bax_ptr);


    typedef union {
	  uint8_t Byte;
	  struct{
		  uint8_t dosByteN		    :1;     // Indica que que al transmitir con protocolo bax no se tenega en cuen el N
		  uint8_t aux1    			:1;     // Bandera auxiliar 1
		  uint8_t aux_2            	:1;		// Bandera auxiliar 2
		  uint8_t aux_3            	:1;		// Bandera auxiliar 3
		  uint8_t aux_4            	:1;		// Bandera auxiliar 4
		  uint8_t aux_5            	:1;		// Bandera auxiliar 5
		  uint8_t aux_6            	:1;		// Bandera auxiliar 6
		  uint8_t aux_7            	:1;     // Bandera auxiliar 7
	  }Bits;
    }tBAX_AUX;

  /* VARIABLES */
  /*************/
    #define BAX_start       0x01      // Start de cadena BAX NORMAL
    #define BAX_stop        0x03
    
    #define finSCI_H        0xDF
    #define finSCI_L        0x0A


    extern tBAX_AUX _baxAUX;

	#define   baxAUX	_baxAUX.Byte;
    #define   dosBYTE_N   _baxAUX.Bits.dosByteN;


  /* RUTINAS */
  /***********/
    extern void BAX_Rx_data (byte data, tBAX* stage_ptr, tBAX_SCI* BAX_SCI_ptr);
 
  #ifdef  VISOR_USES_SKYPATROL  
    extern byte BAX_Tx_data (uint16_t N, byte* DATA_buffer, tBAX_SCI* BAX_SCI_ptr);
    extern byte buff_ECHO_CONTROL[2];
    extern byte pick_BAX_N_Rx (void);
  #else
    extern byte BAX_Tx_data (byte N, byte* DATA_buffer, tBAX_SCI* BAX_SCI_ptr);
  #endif
    
    extern uint16_t chkSpace_onBAX_RxBuffer (tBAX_SCI* BAX_SCI_ptr);
    extern uint16_t chkSpace_onBAX_TxBuffer (tBAX_SCI* BAX_SCI_ptr);
    
    extern byte BAX_formatting (byte N, byte* orig, byte* dest);
#endif

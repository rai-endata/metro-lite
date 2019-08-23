/* File <Print UART.c> */

/* INCLUDES */
/************/
 	
	#include "main.h"
	#include "Definicion de tipos.h"
	#include "Definiciones Generales.h"
	#include "Print UART.h"
	#include "Manejo de Buffers.h"
    #include "timer.h"
	#include "usart7.h"
	#include "usart1.h"
	 
	#include "- PRINT Config -.h"
	#include "Ticket Common.h"

	#include "stm32f0xx_hal_uart.h"
	#include "stm32f0xx_hal.h"
	


/*********************************************************************************************/
/* ESTRUCTURAS */
/***************/



/*********************************************************************************************/
/* PROTOTIPOS */
/***************/


	void PRINT_ptrIni (void);

	static void PRINT_RxPut (byte dat);
	static byte PRINT_RxGet (void);
	static void PRINT_RxIncGET (void);
			
	

/*********************************************************************************************/
/* VARIABLES */
/*************/
	//UART_HandleTypeDef huart1;						//

	static byte timeout_PRINT_send;
	byte PRINT_RxBuffer[dimPRINT_Rx];			// Buffer de Rx de AT CMDs
	static byte* PRINT_RxGET;						// Puntero de extracción
	static byte* PRINT_RxPUT;						// Puntero de inserción
	static uint16_t PRINT_pendingCNT;					// Contador de bytes pendientes de procesamiento
	 #define  PRINT_pendingMax     dimPRINT_Rx-2     	// max cantidad de comandos pendientes(debe ser menor a processCNTmax-2 , de
	                                              	// lo contrario se puede dar el caso que no salga del lazo que hay
	                                              	// en PRINT_preProcess (while(!foundCRLF && (processCNT <= pendingCNT)))
	                                              
	byte PRINT_TxBuffer[dimPRINT_Tx];			// Buffer de Tx de AT CMDs
	static byte* PRINT_TxGET;						// Puntero de extracción
	uint16_t PRINT_N_Tx;						// N a transmitir
	
	static byte PRINT_buffer[dimPRINT_Rx];				// Buffer de 1 comando AT
	
	static byte PRINT_newCMD;						// Indica que tengo un nuevo comando para procesar
	static byte PRINT_newCMD_TO_cnt;               // Contador del TimeOut de nuevo comando AT
	  #define seg_newCMD_TO                 5       // 10seg para descartar dato si no lo proceso
	
	
	static byte PRINT_sendTO_cnt;                  // Contador de TimeOut de envío de un comando AT
	  #define seg_sendTO                    5
	
	static byte PRINT_TOF;                         // Bandera de TimeOut
                         
  
  
 static byte PRINT_Rx_TO_cnt;                         // Contador del Timer de Time Out de Comunicacion
    #define seg_PRINT_noRx_TO    2                    // base 100 ms. timout de sci
    #define PRINT_TO_noRx      (PRINT_Rx_TO_cnt = seg_PRINT_noRx_TO)


	// MACROS
	#define inc_PRINT_RxPTR(ptr)			inc_ptr(&ptr, PRINT_RxBuffer, (uint16_t) dimPRINT_Rx)



    /**************************************************************************************************/
    /* RUTINAS INTERNAS */
    /********************/

	/* USART1 init function */
	void MX_USART1_UART_Init(void)
	{

	  huart1.Instance = USART1;
	  huart1.Init.BaudRate = 9600;
	  huart1.Init.WordLength = UART_WORDLENGTH_8B;
	  huart1.Init.StopBits = UART_STOPBITS_1;
	  huart1.Init.Parity = UART_PARITY_NONE;
	  huart1.Init.Mode = UART_MODE_TX_RX;
	  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	  HAL_UART_Init(&huart1);

	}

	/** Configure pins as
			* Analog
			* Input
			* Output
			* EVENT_OUT
			* EXTI
	*/





  /* INICIALIZACION DE PUNTEROS  */
 /*******************************/

   void PRINT_ptrIni (void){
			   uint16_t i;

				for(i=0; i<dimPRINT_Rx; i++){
     				PRINT_RxBuffer[i] = 0;						// Limpio Buffer
					PRINT_TxBuffer[i] = 0;						// Limpio Buffer
				}
				PRINT_RxGET = PRINT_RxBuffer;
				PRINT_RxPUT = PRINT_RxBuffer;
				PRINT_newCMD = 0;
				PRINT_newCMD_TO_cnt = 0;
				PRINT_pendingCNT=0;
				PRINT_N_Tx=0;
				statusPRINT = NO_HAY_IMPRESION_EN_PROCESO;
	}




/* AGREGAR DATO A BUFFER DE Rx DE AT CMD */
/*****************************************/
	static void PRINT_RxPut (byte dat){
		// Ademas de agregar un dato, debo incrementar el contador de bytes pendientes
		if(PRINT_pendingCNT < PRINT_pendingMax){
		put_byte(&PRINT_RxPUT, dat, PRINT_RxBuffer, (uint16_t) dimPRINT_Rx);
		PRINT_pendingCNT++;
		}
	}


/* EXTRAER DATO DE BUFFER DE Rx DE AT CMD */
/******************************************/
	static byte PRINT_RxGet (void){
		// Ademas de extraer el dato, debo incrementar el puntero
		byte dato;

		dato = *PRINT_RxGET;
		PRINT_RxIncGET();

		return(dato);
	}


/* AVANZAR PUNTERO GET A BUFFER DE Rx DE AT CMD */
/************************************************/
	static void PRINT_RxIncGET (void){
		// Ademas de incrementar el puntero GET, debo decrementar el contador de bytes pendientes
		inc_PRINT_RxPTR(PRINT_RxGET);

		PRINT_pendingCNT--;
	}


/*********************************************************************************************/
/* RUTINAS */
/***********/




		
	/* ENVIAR COMANDO AT */
	/*********************/
		void PRINT_send (byte* CMD, uint16_t N){
			
			if((NO_TXING_PRINTER && NO_RXING_PRINTER )|| exit_PRINT_send){                           //hay transmision en proceso ? agregado rai

			choice_device_uart1 = PRINT_DEVICE;
			USART_Ini(&huart1, 9600);

  			timeout_PRINT_send=0;                       				// timout de inhabilitacion de esta rutina
  			
  	//stringCopyN (PRINT_TxBuffer, CMD, N);							// Copio comando a buffer

  			PRINT_N_Tx = N;					// Cantidad de datos a Tx

  	//PRINT_TxBuffer[PRINT_N_Tx] = 0x0D;							// Agrego <CR> para ejecutar comando

  	//* CMD = 0x0D;
	//PRINT_N_Tx++;

  	//PRINT_TxGET = PRINT_TxBuffer;								// Puntero de extraccion al inicio del buffer
  	  PRINT_TxGET = CMD;

  			huart1.Tx_TO_cnt = UART_TX_TIME_OUT;
  			HAL_UART_Transmit_IT(&huart1,PRINT_TxBuffer,PRINT_N_Tx);	// Habilito Transmisión

			}
		  PRINT_reint(seg_sendTO);                       				// reintento de comando
		}

 		void PRINT_send_modeAT (byte* CMD,byte N){
			
  			buffer_N_copy (PRINT_TxBuffer, CMD, N);					// Copio comando a buffer
  			PRINT_N_Tx = string_length(PRINT_TxBuffer);				// Cantidad de datos a Tx
  			PRINT_TxGET = PRINT_TxBuffer;								// Puntero de extraccion al inicio del buffer
  			HAL_UART_Transmit_IT(&huart1,PRINT_TxBuffer,PRINT_N_Tx);	// Habilito Transmisión
        }


  /* DETENER TIME OUT DE ENVIO DE COMANDO AT */
  /*******************************************/
    void PRINT_clrSendTO (void){
      PRINT_sendTO_cnt = 0;
      PRINT_TOF = 0;
    }


  /* REINTENTAR ENVIO DE COMANDO AT */
  /**********************************/
    void PRINT_reint (byte cnt){
      PRINT_sendTO_cnt = cnt;
      PRINT_TOF = 0;
    }



  /* CHEQUEAR BANDERA DE TIME OUT DE COMANDO AT ENVIADO */
  /******************************************************/
    byte PRINT_chkSendTOF (void){
      return(PRINT_TOF);
    }    

		
	/* ENVIAR COMANDO AT (HEX) */
	/***************************/
		void PRINT_sendHEX (byte* CMD, uint16_t N){
			byte* ptr;
			
		if(PRINT_N_Tx == 0){                         //hay transmision en proceso ? agregado rai
  			PRINT_N_Tx = N;														// Cantidad de datos a Tx
  			ptr = PRINT_TxBuffer;
  			while(N > 0){
  				N--;
  				*ptr++ = *CMD++;
  			}
  			
  			PRINT_TxBuffer[PRINT_N_Tx] = 0x0D;					// Agrego <CR> para ejecutar comando
  			PRINT_N_Tx++;																// Incremento N
  			PRINT_TxGET = PRINT_TxBuffer;								// Puntero de extraccion al inicio del buffer
  			HAL_UART_Transmit_IT(&huart1,PRINT_TxBuffer,PRINT_N_Tx);	// Habilito Transmisión
		}
		PRINT_clrSendTO();                           // preparo timout (agregado rai 9/8/2014
		PRINT_sendTO_cnt = seg_sendTO;               // TimeOut de envio de comando AT
		}
		

	/* PRE-PORCESAMIENTO DE COMANDOS AT */
	/************************************/
		void PRINT_preProcess (void){
			// 1. El Puntero GET nunca va a apuntar a un 0x00-0x0D-0x0A
			// 2. Luego busco un <CR><LF> para determinar el fin
			// 3. Guardo el comando completo en un buffer especial
			byte* searchPTR;
			byte end_1;
			byte end_2;
			uint16_t pendingCNT;
			uint16_t processCNT;
			byte foundCRLF;
			byte* atCMD_endPTR;
			byte* atPUT;
			
			// Si no tengo un comando pendiente de ser procesado....
			atCMD_endPTR = NULL;
			if (!PRINT_newCMD){
				// 1. El Puntero GET nunca va a apuntar a un 0x00-0x0D-0x0A-> Lo hago como menor a 0x20 para que
				// descarte basura y le agrego >0x7F (fin ASCII).
				// Al "< 0x20" le agrego que si es 0x01, lo tome como dato ya que es como comienzan los
				// datos de central.
			 dispararTO_lazo();                        // Disparo Time Out de Lazo
  			while ((!chkTO_lazo_F()) && PRINT_pendingCNT > 0 && ((*PRINT_RxGET < 0x20 && *PRINT_RxGET != 0x01) || *PRINT_RxGET > 0x7F)){
					PRINT_RxIncGET();						// Avanzo puntero GET
				}
			 detenerTO_lazo();                        // Disparo Time Out de Lazo
  			
				// 2. Luego busco un <CR><LF> para determinar el fin (por lo menos necesito 2 bytes)
				// El fin tambien lo puede marcar un '^', por ejemplo en el arranque marca '^SYSTART'
				// pero puede tener basura antes que eso y necesito decartarla
				pendingCNT = PRINT_pendingCNT;
				processCNT = 0;
				if (processCNT < pendingCNT){
					searchPTR = PRINT_RxGET;									// Puntero de busqueda
					
					end_1 = get_byte(&searchPTR, PRINT_RxBuffer, (uint16_t) dimPRINT_Rx);
					processCNT++;
	
					end_2 = get_byte(&searchPTR, PRINT_RxBuffer, (uint16_t) dimPRINT_Rx);
					processCNT++;
	
					foundCRLF = 0;
					dispararTO_lazo();
					while((!chkTO_lazo_F()) && !foundCRLF && (processCNT <= pendingCNT)){
						if (end_1 == AT_END_H && end_2 == AT_END_L){
							foundCRLF = 1;
						}else{
							end_1 = end_2;
							end_2 = get_byte(&searchPTR, PRINT_RxBuffer, (uint16_t) dimPRINT_Rx);
							processCNT++;
						}
					}
					detenerTO_lazo();
						
					// Si encontre un CR-LF, copio el comando al buffer
					if (foundCRLF){
						atCMD_endPTR = searchPTR;							// Puntero de fin del comando
					}
				}
				
				// 3. Guardo el comando completo en un buffer especial
				if (atCMD_endPTR != NULL){
					atPUT = PRINT_buffer;									// Puntero a inicio de buffer comando
 					dispararTO_lazo();
					while ((!chkTO_lazo_F()) && PRINT_RxGET != atCMD_endPTR){
						*atPUT++ = PRINT_RxGet();							// Extraigo dato, avanzo puntero, decremento pending y lo agrego a buffer de comando
					}
					detenerTO_lazo();
          					
					// Como termino con CR-LF, tengo 2 bytes de mas, que debo eliminar
					atPUT--;
					atPUT--;
					*atPUT = 0x00;											// Agrego fin de cadena
					PRINT_newCMD = 1;
					PRINT_newCMD_TO_cnt = seg_newCMD_TO;
				}
			}
		}
		
	/* COMPARAR COMANDO AT RECIBIDO */
	/********************************/
		byte PRINT_Rx_compare(byte* CMD){
			// Detarmina si el comando recibido es igual al pasado como argumento, hace un N compare,
			// con la longitud del argumento
			// 	1: Iguales
			//	0: Distintos
			uint16_t N;
			byte diff;
			
			N = string_length(CMD);
			
			diff = string_N_compare(PRINT_buffer, CMD, N);
			
			return(!diff);
		}

	/* EXTRAER PUNTERO A BUFFER DE COMANDO AT */
	/******************************************/
		byte* PRINT_getRx_ptr (void){
			return(PRINT_buffer);
		}
		

	/* DETERMINAR SI TENGO UN COMANDO AT NUEVO */
	/*******************************************/		
		byte PRINT_chkNEW (void){
			return(PRINT_newCMD);
		}
		

	/* COMANDO AT PROCESADO */
	/************************/
		void PRINT_doneNEW (void){
			PRINT_newCMD = 0;
			PRINT_buffer[0] = 0;
			
			PRINT_newCMD_TO_cnt = 0;
		}


		void clearPRINT_pendingCNT (void){
			PRINT_pendingCNT=0;

		}


/**************************************************************************************************/
/* TIMER */
/*********/
  
  /* TIME OUT DE COMANDO AT NO PROCESADO */
  /***************************************/
    void PRINT_newCMD_TO_timer (void){
      // Ejecutada cada 1 seg
      if (PRINT_newCMD_TO_cnt != 0){
        PRINT_newCMD_TO_cnt--;
        if (PRINT_newCMD_TO_cnt == 0){
          PRINT_doneNEW();
        }
      }
    }


  /* TIME OUT DE COMANDO ENVIO DE COMANDO AT */
  /*******************************************/
    void PRINT_sendTO_timer (void){
      // Ejecutada cada 1 seg
      if (PRINT_sendTO_cnt != 0){
        PRINT_sendTO_cnt--;
        if (PRINT_sendTO_cnt == 0){
          PRINT_TOF = 1;
        }
      }
    }    

                 
 /* TIME OUT COMUNICACION */
  /*************************/

    void PRINT_Rx_TO (void){
      if (PRINT_Rx_TO_cnt != 0){
    	  PRINT_Rx_TO_cnt--;
        if (PRINT_Rx_TO_cnt == 0){
         // PRINT_noRx = 1;                 // No tengo comunicación
   			 PRINT_RxPUT = PRINT_RxGET;
   			 PRINT_pendingCNT=0;
        }
      }
    }		
 
 
 /* TIME OUT DE INHABILITACION PRINT_send  */
  /******************************************/

    void timeout_PRINT_send_TIMER (void){
       if(NO_TXING_PRINTER){
        timeout_PRINT_send=0;
       }else{
        if (timeout_PRINT_send != 0xff){
          timeout_PRINT_send++;
        }
     }
    }		
    
    /*********************************************************************************************/
    /* RUTINAS DEBUG */
    /****************/

    	/* ENVIAR COMANDO AT */
    	/*********************/
    		byte stringLenth_debug (byte* orig_ptr);

    		void PRINT_send_debug (byte* CMD){
    			string_copy (PRINT_TxBuffer, CMD);						// Copio comando a buffer
    			PRINT_N_Tx = stringLenth_debug(PRINT_TxBuffer);	// Cantidad de datos a Tx

    			//PRINT_TxBuffer[PRINT_N_Tx] = 0x0D;					// Agrego <CR> para ejecutar comando
    			//PRINT_N_Tx++;																// Incremento N

    			PRINT_TxBuffer[PRINT_N_Tx] = 0x00;          //AGREGO FIN DE CADENA

    			PRINT_TxGET = PRINT_TxBuffer;								// Puntero de extraccion al inicio del buffer
    			HAL_UART_Transmit_IT(&huart1,PRINT_TxBuffer,PRINT_N_Tx);	// Habilito Transmisión


    			PRINT_sendTO_cnt = seg_sendTO;               // TimeOut de envio de comando AT
    		}

        //calcula longitud de una cadena que termana en 0x0D y 0x0A
        byte stringLenth_debug (byte* orig_ptr){
          byte len;
          byte exit;

          len = 0;
          exit= 0;
          while (!exit){
            //a=*orig_ptr;
            //b=*(orig_ptr+1);
            if(*orig_ptr==0x0D && *(orig_ptr+1)==0x0A ){
               exit=1;
            }
            orig_ptr++;
            len++;
          }

          return(len-1);
        }
    
    
/* PROCESO DE INTERRUPCIONES POR SCI */
 /***********************************************************************************************/


    	/* PROCESO DE Rx IRQ */
    	/*********************/


    	void PRINT_SCI_RxISR(uint8_t sci_data){
    		PRINT_RxPut(sci_data);             		// Agrego dato al buffer de AT CMDs
    		PRINT_TO_noRx;								//refresco timeout
    	}


    	/* PROCESO DE Tx IRQ */
    	/*********************/
    	void PRINT_SCI_TxISR (UART_HandleTypeDef *huart){

    		if (PRINT_N_Tx == 0){
				 __HAL_UART_DISABLE_IT(huart, UART_IT_TXE);
				 __HAL_UART_DISABLE_IT(huart, UART_IT_TC);
				huart->State = HAL_UART_STATE_READY;
				choice_device_uart1 = PROG_DEVICE;
				USART_Ini(&huart1, 4800);
				huart->Tx_TO_cnt = 0;
				statusPRINT = NO_HAY_IMPRESION_EN_PROCESO;
        	}else{
				huart->Instance->TDR = (uint8_t)(*PRINT_TxGET++ & (uint8_t)0x00FFU);
				PRINT_N_Tx--;
				huart->Tx_TO_cnt = UART_TX_TIME_OUT;
    		}
    	}



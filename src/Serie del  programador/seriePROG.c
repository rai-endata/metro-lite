/* File: <seriePROG.c> */

/* INCLUDES */
/************/
	#include "seriePROG.h"
	#include "Manejo de Buffers.h"
	#include "Protocolo BAX.h"

	#include "stm32f0xx_hal_uart.h"
	#include "stm32f0xx_hal.h"
	#include "main.h"
	#include "Programacion Parametros.h"
	#include "tipo de equipo.h"
	#include "Reloj.h"

	//#include "file aux1.h"

/*********************************************************************************************/
/* VARIABLES */
/*************/
  

  static tBAX PRG_protBAX_STAGE;    // Etapa del protocolo BAX
  
  tBAX_SCI BAX_SCI_PROG;            // Datos de SCI segun protocolo BAX  PROG
  
  #define SCI_PROG_getByte(GETptr_ptr)    get_byte(GETptr_ptr, BAX_SCI_PROG.TxBuffer, dim_BAX)
  
  static byte PRG_N_Tx;             // Cantidad total de bytes a transmitir por la serie
  
  static byte PRG_loopback;         // Bandera que indica que externamente tengo un LOOP (por ahora en modo test)
  // Lo que hace la bandera de loopback es NO resetear el tipo, una vez finalizada
  // la transmision, porque si lo hago, en la recepcion pierdo el tipo y no logro
  // recibir correctamente

  static byte ENDATA_errorTx;
  static byte ENDATA_errorTipo;
  

/**************************************************************************************************/
/* RUTINAS */
/***********/

  /* INICIALIZACION DE LA SERIE RELOJ/PROGRAMADOR */
  /************************************************/
    void PRG_ini (void){
      //seriePROG_ini(48);                 // Inicializao SCI para 48Mhz

    	UART_PROG_IniRx();
    	UART_PROG_IniTx();
    }

    void UART_PROG_IniRx(void){
        BAX_SCI_PROG.RxGPA_GETidx = 0;
        BAX_SCI_PROG.RxGPA_PUTidx = 1;
        BAX_SCI_PROG.RxGPA[0] = BAX_SCI_PROG.RxBuffer;
        BAX_SCI_PROG.RxPUT = BAX_SCI_PROG.RxBuffer;
        BAX_SCI_PROG.Rx_TO_cnt = 0;
        BAX_SCI_PROG.Rx_fin = 0;
    }


    void UART_PROG_IniTx(void){
        BAX_SCI_PROG.TxGET = BAX_SCI_PROG.TxBuffer;
        BAX_SCI_PROG.TxPUT = BAX_SCI_PROG.TxBuffer;
        BAX_SCI_PROG.start = PRG_BAXstart;
        BAX_SCI_PROG.Tx_cnt = 0;
        BAX_SCI_PROG.Txing = 0;
    }
    /* SOLCIITAR INICIO DE TRANSMISION DE DATOS POR SERIE PROGRAMADOR */
    /******************************************************************/
   void PRG_startTx (byte N,byte* data){

      byte error;
      
      //Agrego protocolo BAX a los N datos a transmitir
      //luego PROGRAMADOR_chkTx se ocupara de iniciar transmision si se puede
      error = BAX_Tx_data(N, data, &BAX_SCI_PROG);

      if (!error){
      }

    }
    

    /* INICIAR TRANSMITIR DATOS POR SERIE RELOJ/PROGRAMADOR */
    /********************************************************/
      void PROGRAMADOR_chkTx (void){
        byte N;
        uint8_t *verdura1; //no lo uso
        uint16_t verdura2; //no lo uso
        uint8_t* ptrAUX;
        
        if ((BAX_SCI_PROG.Tx_cnt > 0) && (!BAX_SCI_PROG.Txing)){
          
          BAX_SCI_PROG.Tx_cnt--;                    // Decremento contador de Tx
          BAX_SCI_PROG.Txing = 1;                   // A partir de ahora estoy transmitiendo
          //seriePROG_startTx(SCI_PROG_getByte(&BAX_SCI_PROG.TxGET));  // Extraigo dato y avanzo putnero

          ptrAUX = BAX_SCI_PROG.TxGET;
          SCI_PROG_getByte(&BAX_SCI_PROG.TxGET);	  //es para avanzar puntero para luego tomar N
          N = *(BAX_SCI_PROG.TxGET);                  // N(datos + chksum) + START + N + STOP
          BAX_SCI_PROG.TxGET = ptrAUX;				  //recupero puntero para poder transmitir desde el inicio

          PRG_N_Tx = N+3;                     		// - df y 0a
          choice_device_uart1 = PROG_DEVICE;
          USART_Ini(&huart1, 4800);

          //Habilito Transmisión
          huart1.Tx_TO_cnt = UART_TX_TIME_OUT;
          HAL_UART_Transmit_IT(&huart1,verdura1,verdura2);
        }
      }
    


	
/**************************************************************************************************/
/* Tx/Rx VIA UART */
/******************/
  
  /* Rx NUEVO DATO DE RELOJ/PROGRAMADOR (UART) */
  /*************************************************/
    void UART_Rx_PRG (byte dato){
      
	  if(prog_mode){
		//indica que esta programando con el led de salida de bandera
		HAL_GPIO_TogglePin(BANDERA_OUT_PORT, BANDERA_OUT_PIN);
	  }
      
      BAX_Rx_data (dato, &PRG_protBAX_STAGE, &BAX_SCI_PROG);
      
      if (BAX_SCI_PROG.Rx_fin){
        // Si finalizo alguna de las recepciones, tengo que resetear el tipo, xq
        // sino que ese mismo tipo seteado para la proxima recepcion, y no debe
        // ser asi
    	if((tipo_de_equipo == METRO_LITE)){
    	  prog_mode=1;
	    }

        BAX_SCI_PROG.Rx_fin = 0;				// Bajo Bandera
        
      }
    }


    void UART_Tx_PRG(UART_HandleTypeDef *huart){
    	word newData;

		if(prog_mode){
			//indica que esta programando con el led de salida de bandera
			HAL_GPIO_TogglePin(BANDERA_OUT_PORT, BANDERA_OUT_PIN);
		}


    	// Extraigo nuevo dato y determino si debo finalizar Tx
        newData = PRG_UART_TxNewData();
        if ((newData & seriePROG_endTx) == seriePROG_endTx){
            __HAL_UART_DISABLE_IT(huart, UART_IT_TXE);
         	 __HAL_UART_DISABLE_IT(huart, UART_IT_TC);
        	huart->State = HAL_UART_STATE_BUSY_RX;
        	BAX_SCI_PROG.Txing = 0;
        	huart->Tx_TO_cnt = 0;
        	if(resetAFTER_txRTA_F){
        		//NVIC_SystemReset();
        	}

        }else{
    		 huart->Instance->TDR = (uint8_t)(newData & (uint8_t)0x00FFU);
    		 huart->Tx_TO_cnt = UART_TX_TIME_OUT;
        }
    }

  /* Tx NUEVO DATO DE RELOJ/PROGRAMADOR (UART SCI) */
  /*************************************************/
    word PRG_UART_TxNewData (void){
      // Esta rutina esta pensada para ser usada en una transmision por UART . Por eso
      // el resultado son 2 bytes.
      // La parte ALTA es la que indica fin de Tx, y en la parte BAJA, tenemos el dato
      // extraido.
      // Normalmente, la parte alta seria siempre 0x00, y al recibir 0xFF quiere decir que
      // debo detener la Tx
      tBAX_SCI* BAX_SCI_ptr;
      byte doTx;
      word result;
      byte* GETptr;
      byte fin1, fin2;
      
      doTx = 1;                                     // Reseteo bandera
      result = 0;                                   // Reseteo variable

      // Datos de Programador
	  BAX_SCI_ptr = &BAX_SCI_PROG;
	  GETptr = BAX_SCI_PROG.TxGET;                // Duplico puntero GET
	  fin1 = SCI_PROG_getByte(&GETptr);           // Extraigo byte
	  fin2 = SCI_PROG_getByte(&GETptr);           // Extraigo byte
        
      
      if ((doTx) && (fin1 == finSCI_H) && (fin2 == finSCI_L)){
        // Si llegue al fin de datos, deshabilito Tx IRQ y Tx. No puedo deshabilitar la
        // tarnsmision en si, xq sino "tal vez" no sale este ultimo dato
        //
        // EDIT 30/11/11
        // No basta con comparar con el fin de datos, ademas se debe cumplir que el N a
        // transmitir sea nulo, ya que DF0A puede ser parte de los datos
        // Al finalizar la transmision, baja la bandera que indica que esta transmitiendo
        if (PRG_N_Tx == 0){
          result = seriePROG_endTx;                 							// Fin de Transmision
          doTx = 0;                                 							// No transmito
          BAX_SCI_ptr->Txing = 0;                   							// Ya no estoy transmitiendo
          (void)get_byte(&BAX_SCI_ptr->TxGET, BAX_SCI_ptr->TxBuffer, dim_BAX);	// Extraigo FIN_H
          (void)get_byte(&BAX_SCI_ptr->TxGET, BAX_SCI_ptr->TxBuffer, dim_BAX);	// Extraigo FIN_L
        }
      }else{
        if (PRG_N_Tx == 0){
         //no llego el fin de cadena -> hay error ya que se transmitio N bytes
         doTx = 0;
         result = seriePROG_endTx;                     // Fin de Transmision (ERROR)
         BAX_SCI_ptr->Txing = 0;                      // Ya no estoy transmitiendo
         ENDATA_errorTx++;
         PRG_ini();
        }
      }
      
      
      if (doTx){
        if (PRG_N_Tx > 0){
          //Decremento bytes a transmitir
       	  PRG_N_Tx--;
       	  //Extraigo dato y avanzo puntero
          result |= get_byte(&BAX_SCI_ptr->TxGET, BAX_SCI_ptr->TxBuffer, dim_BAX);
        }else if (BAX_SCI_ptr->Txing){
          // Si N == 0 y todavia estoy transmitiendo es porque hay un error
          // => Finalizo la transmision y reseteo los buffers
          ENDATA_errorTx++;
          PRG_ini();
        }
      }
      return(result);
    }



	/**************************************************************************************************/
	/* TIMER */
	/*********/


  /* TIME OUT Rx SERIE PROGRAMADOR */
  /*********************************/
    void PRG_TO_serieRx (void){
      // Ejecutada cada 1mseg
      // Los contadores de Time Out se levantan en las etapas del protocolo BAX
      // - Reseteo Tipo
      // - Reestablezco puntero PUT de Rx
      if (BAX_SCI_PROG.Rx_TO_cnt != 0){
        BAX_SCI_PROG.Rx_TO_cnt--;
        if (BAX_SCI_PROG.Rx_TO_cnt == 0){
          BAX_SCI_PROG.RxPUT = BAX_SCI_PROG.RxPUT_bkp;    	// Restablezco Puntero PUT de Rx
          PRG_protBAX_STAGE = BAX_START;              		// Reseteo ETAPA del Protocolo BAX
        }
      }
    }    



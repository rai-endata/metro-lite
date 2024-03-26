/* File: <Air Update.c> */

                    
/* INCLUDES */
/************/
    //#include "- Visor Config -.h"

	//#include "Despacho\Posicionamiento en Tiempo Real.h"
	
	//#include "Display\Pantallas\Actualizacion Pantalla.h"
	
	#include "Air Update.h"
	
	#include "eeprom.h"
	#include "Flash.h"
	//#include "GPS\Zonas y Bases.h"
	//#include "GPS\Entrada-Salida Base.h"
	//#include "GPS\Localidades\- Localidad -.h"
	#include "Programacion Parametros.h"
    #include "Reportes.h"
	#include "Timer.h"
	#include "Constantes.h"
	#include "Lista Comandos.h"
	#include "DA Communication.h"
	#include "Definicion de tipos.h"
	#include "DTE - Tx.h"
	#include "DA Define Commands.h"


/* FUNCIONAMIENTO */
/******************/
/*
  PROGRAMACION
  ------------
  Por ahora se trabaja de la siguiente manera:
  
    CMD | ID | ACCION | VALORES | CRC
    
  
  + El ID de actualizacion tiene una longitud de 2 bytes, y se usa para que a la hora de
  responder con ACTUALIZACION EXITOSA, se indique se que se trata esa actualizacion
  
  + La accion puede ser de 0x00-0xFF y puede venir, o no, acompañada de hasta "dim_airPROG_valores"
  bytes de valores
	
	+ Se creo una tabla en la que se ingresa con la ACCION y de acuerdo a ella se ejecuta una
	funcion
	
	+ En cuanto a los valores, no es necesario que la CENTRAL los envie, esos son parametros
  extra, que si no son necesarios, pueden ser obviados para ahorrar aire
  
  + El CRC es el byte menos significativo de la suma de TODOS los bytes del CMD en adelante
*/





/*********************************************************************************************/
/* PROTOTIPOS */
/**************/
  // RUTINAS INTERNAS
  
  static byte doTxBuffer_progParameter (byte* param_ptr);
  static byte doTxBuffer_ZonaBase (byte* ZB_ptr);
  static byte doTxBuffer_readPyV (byte*far PyV_param_inFlash);
  static byte doTxBuffer_readInOutBase (byte*far BASEinOut_param_inFlash);
  
  // ACCIONES
  static byte updateZona (byte* values);
  static byte updateBase (byte* values);
  static byte updateProgRadio (byte* values);
  static byte updateProgReloj_comun (byte* values);
  static byte updateProgReloj_tarifa (byte* values);
  static byte updateProgReloj_eqpesos (byte* values);
  static byte updateProgReloj_calendario (byte* values);
  static byte updateProgGeoFence (byte* values);
  static byte createZona (byte* values);
  static byte createBase (byte* values);
  static byte updatePyV (byte* values);
  static byte updateInOutBase (byte* values);
 
  // LECTURAS
  static byte readZona (byte value);
  static byte readBase (byte value);
  static byte readProgRadio (byte value);
  static byte readProgReloj_comun (byte value);
  static byte readProgReloj_tarifa (byte value);
  static byte readProgReloj_eqpesos (byte value);
  static byte readProgReloj_calendario (byte value);
  static byte readProgGeoFence (byte value);

/*********************************************************************************************/
/* CONSTANTES */
/**************/

  #define dim_airPROG_valores  70
  
  #define dim_airPROG_actions  12   // Cantidad de comandos soportados
 
  
 

  // FUNCIONES DE ACCION
  typedef byte (*actions_ptr)(byte* values);
  static const actions_ptr far TABLA_ACCIONES[] = {
    updateZona,
    updateBase,
    updateProgRadio,
    updateProgReloj_comun,
    updateProgReloj_tarifa,
    updateProgReloj_eqpesos,
    updateProgReloj_calendario
  };
  
  typedef byte (*read_ptr)(byte value);
  static const read_ptr far TABLA_LECTURAS[] = {
    readZona,
    readBase,
    readProgRadio,
    readProgReloj_comun,
    readProgReloj_tarifa,
    readProgReloj_eqpesos,
    readProgReloj_calendario
  };



/*********************************************************************************************/
/* VARIABLES */
/*************/
  // Comando de ACTUALIZACION EXITOSA
  #define N_DATOS_AirUpdateSuccess	2
  #define N_AirUpdateSuccess        (N_CMD + N_DATOS_AirUpdateSuccess)
  static byte AirUpdateSuccess_Buffer[N_DATOS_AirUpdateSuccess + 2];   // Sumo DF + 0A
  typeTxCMD  CMD_AIRUPDATE_SUCCESS={0,CMD_AIR_UPDATE_DONE,0,timeReint_normal,N_AirUpdateSuccess, AirUpdateSuccess_Buffer};
  
  // Comando de LECTURA POR AIRE
  #define dim_AirRead               100 // TAMAÑO AL AZAR... DEBEN ENTRAR LECTURAS DE PROG
  static byte AirRead_Buffer[dim_AirRead + 2];   // Sumo DF + 0A
  typeTxCMD  CMD_AirRead={0,CMD_AIR_READ,0,timeReint_normal,0, AirRead_Buffer};
  
  

  tACCIONES AIR_UPDATE_action;   // Accion de actualizacion a Realizar
  byte AIR_UPDATE_ID[2];         // ID de actualizacion
  static byte AIR_READ_ID[2];           // ID de lectura
  static byte AIR_UPDATE_values_N;      // Cantidad de valores
  
  tAirUpdate_FLAGS             _AIR_UPDATE_F;
  
/*********************************************************************************************/  
/* RUTINAS */
/***********/

  /* RECEPCION DE ACTUALIZACION POR AIRE */
  /***************************************/
  void AIR_UPDATE_Rx_LOCAL (byte* Rx_data_ptr){

  }
  void AIR_UPDATE_Rx_TRANSPARENTE (byte* Rx_data_ptr){

  }

    void AIR_UPDATE_Rx (byte* Rx_data_ptr){
      // Recibi la Programacion por Aire. Lo que debo hacer es extraer la ACCION y los VALORES
      // del comando, para luego desde la tabla saber que rutina invocar.
      //
      // En cuanto a los valores, no es necesario que la CENTRAL los envie, esos son parametros
      // extra, que si no son necesarios, pueden ser obviados para ahorrar aire
			byte N;
      byte valores[dim_airPROG_valores];
      byte* ptr;
      
      
      if(FLASH_chkCanUpdate()){
        N = (*Rx_data_ptr++) - 1;             // Extraigo N, le resto 1 x el CRC
        
        if (AIR_UPDATE_chkCRC(Rx_data_ptr, N)){
          // Si el checksum da bien, extraigo la ACCION junto con sus valores y la 
          // proceso
          Rx_data_ptr++;					   // Salteo Comando
          N--;                                // Le resto 1 xq el N incluye el CMD
          
          AIR_UPDATE_ID[0] = *Rx_data_ptr++;  // Extraigo ID msb
          N--;                                // Decremento N
          
          AIR_UPDATE_ID[1] = *Rx_data_ptr++;  // Extraigo ID lsb
          N--;                                // Decremento N
        
          AIR_UPDATE_action = (tACCIONES) *Rx_data_ptr++; // Extraigo ACCION
          N--;                                // Deceremento N
          
          if (N <= dim_airPROG_valores){
            
            AIR_UPDATE_values_N = N;          // Guardo cantidad de valores
            
            ptr = valores;                    // Puntero apuntando a VALORES
            while (N > 0){
              N--;                            // Deceremento cantidad de datos
              *ptr++ = *Rx_data_ptr++;        // Voy extrayendo valores
            }
            
            // EDIT 24/04/2013
            //  Si la accion recibida no esta dentro de la lista de acciones, no le da bola
            if (AIR_UPDATE_action < dim_airPROG_actions){
              TABLA_ACCIONES[AIR_UPDATE_action](valores);  // Proceso ACCION
            }
          }
        }
      }
    }



    void AIR_UPDATE_RxTransparente (byte* Rx_data_ptr){
       // Recibi la Programacion por Aire. Lo que debo hacer es extraer la ACCION y los VALORES
       // del comando, para luego desde la tabla saber que rutina invocar.
       //
       // En cuanto a los valores, no es necesario que la CENTRAL los envie, esos son parametros
       // extra, que si no son necesarios, pueden ser obviados para ahorrar aire
 			byte N;
       byte valores[dim_airPROG_valores];
       byte* ptr;


       if(FLASH_chkCanUpdate()){
         N = (*Rx_data_ptr++) - 1;             // Extraigo N, le resto 1 x el CRC

         if (AIR_UPDATE_chkCRC(Rx_data_ptr, N)){
           // Si el checksum da bien, extraigo la ACCION junto con sus valores y la
           // proceso
           Rx_data_ptr += 2;				   // Salteo Comando
           N -=2;                              // Le resto 2 xq el N incluye el CMD

           AIR_UPDATE_ID[0] = *Rx_data_ptr++;  // Extraigo ID msb
           N--;                                // Decremento N

           AIR_UPDATE_ID[1] = *Rx_data_ptr++;  // Extraigo ID lsb
           N--;                                // Decremento N

           AIR_UPDATE_action = (tACCIONES) *Rx_data_ptr++; // Extraigo ACCION
           N--;                                // Deceremento N

           if (N <= dim_airPROG_valores){

             AIR_UPDATE_values_N = N;          // Guardo cantidad de valores

             ptr = valores;                    // Puntero apuntando a VALORES
             while (N > 0){
               N--;                            // Deceremento cantidad de datos
               *ptr++ = *Rx_data_ptr++;        // Voy extrayendo valores
             }

             // EDIT 24/04/2013
             //  Si la accion recibida no esta dentro de la lista de acciones, no le da bola
             if (AIR_UPDATE_action < dim_airPROG_actions){
               TABLA_ACCIONES[AIR_UPDATE_action](valores);  // Proceso ACCION
             }
           }
         }
       }
     }

  /* RECEPCION DE LECTURA POR AIRE */
  /*********************************/    
    void AIR_READ_Rx (byte* Rx_data_ptr){
      // Recibi la Lectura por Aire. Lo que debo hacer es extraer el subCMD de LECTURA,
      // es decir, lo que hay que leer y luego transmitir el comando con los valores
      // solicitados.
			byte cmd;
      
      Rx_data_ptr++;								        // Salteo N
      Rx_data_ptr++;								        // Salteo Comando
      AIR_READ_ID[0] = *Rx_data_ptr++;      // Extraigo ID msb
      AIR_READ_ID[1] = *Rx_data_ptr++;      // Extraigo ID lsb
      cmd = *Rx_data_ptr++;								  // Extraigo subCMD LECTURA
      
      // EDIT 24/04/2013
      //  Si la accion recibida no esta dentro de la lista de acciones, no le da bola
      if (AIR_UPDATE_action < dim_airPROG_actions){
        TABLA_LECTURAS[cmd](*Rx_data_ptr);    // Proceso LECTURA
      }
    }    



  /* ACTUALIZACION POR AIRE */
  /**************************/
    void AIR_UPDATE_update (void){
      // Llamada desde el LOOP principal
      //
      // Dado que la actualizacion implica grabacion de FLASH/EEPROM, no se puede hacer
      // en cualquier momento, porque ambas implican la deshabilitacion de las IRQs, ya
      // que si las atiende a ellas, no puede atender otras cosas.
      // Por tanto, para poder efectuar la actualizacion, es preciso que:
      // - Se ya terminado de enviar la Rta al Comando de Update

      byte error;
      
      if (FLASH_chkCanUpdate()){
        // No bajo la bandera de updateRequest xq ella se baja en la actualizacion
         if(updateFinRTA_LOCAL){
        	 updateFinRTA_LOCAL = 0;                                 // Bajo Bandera
             AIR_UPDATE_success_Tx_LOCAL();                        // Transmito comando de Actualizacion por Aire Exitosa
         }
         if(updateFinRTA_TRANSPARENTE){
        	 updateFinRTA_TRANSPARENTE = 0;                                 // Bajo Bandera
             AIR_UPDATE_success_Tx_TRANSPARENTE();                        // Transmito comando de Actualizacion por Aire Exitosa
         }
      }
    }



  

/*********************************************************************************************/
/* RUTINAS INTERNAS */
/********************/

     
  /* VERIFICACION DE CRC DEL COMANDO RECIBIDO */
  /********************************************/
    byte AIR_UPDATE_chkCRC (byte* data, byte N){
      // El CRC es el byte menos significativo de la suma de TODOS los bytes del CMD en adelante

      word chksum;
      byte ok;
      byte TO_F;
      
      byte chk;
      word CHK;
      
      chksum = 0;                      // Reseteo variable
      
      TO_F = 0;                     // Reseteo Bandera de TimeOut
      dispararTO_lazo();            // Disparo Time Out de Lazo
      while ((N > 0) && (!TO_F)){
        TO_F = chkTO_lazo_F();      // Chequeo bandera de time out de lazo 
        N--;                        // Decremento N
        
        chksum += *data++;             // Voy sumando los N bytes de datos
      }
      detenerTO_lazo();             // Detengo Time Out de Lazo
      
      chk = (chksum & 0x00FF);
      
      // Ahora el puntero de datos apunta al chksum, si coinciden, esta todo bien
      if ((*data == chk) && !TO_F){
        ok = 1;
      }else{
        ok = 0;
      }
      
      return(ok);
    }


  /* TRANSMITIR ACTUALIZACION EXITOSA */
  /************************************/
    void AIR_UPDATE_success_Tx_LOCAL (void){
      // Formato de Actualizacion Exitosa
      //
      //  | x | x |
      //  +-------+
      //      +-----------------------> ID
      //
      // Longitud de datos = 2
      CMD_AIRUPDATE_SUCCESS.Tx_F = 1;                 // Levanto Bandera de Tx
      CMD_AIRUPDATE_SUCCESS.Reintentos = cantReint;
      
      AirUpdateSuccess_Buffer[0] = AIR_UPDATE_ID[0];  // ID msb
      AirUpdateSuccess_Buffer[1] = AIR_UPDATE_ID[1];  // ID lsb
      
      AirUpdateSuccess_Buffer[N_DATOS_AirUpdateSuccess] = fin_datos_msb;  // Fin Datos
      AirUpdateSuccess_Buffer[N_DATOS_AirUpdateSuccess+1] = fin_datos_lsb;// Fin Datos
    }


    void AIR_UPDATE_success_Tx_TRANSPARENTE (void){
          // Formato de Actualizacion Exitosa
          //
          //  | x | x |
          //  +-------+
          //      +-----------------------> ID
          //
    	byte buff_aux[20];
    	byte N, i;

    	i=0;
		buff_aux[i++] = CMD_AIRUPDATE_SUCCESS.cmd;     //subcomando
		buff_aux[i++] = 0;   						  	//fuente de hora
		getDate();
		buff_aux[i++] = RTC_Date.hora[0];   		  	//HORA
		buff_aux[i++] = RTC_Date.hora[1];   		  	//MINUTOS
		buff_aux[i++] = RTC_Date.hora[2];   		  	//SEGUNDOS
		buff_aux[i++] = RTC_Date.fecha[0];  		  	//DIA
		buff_aux[i++] = RTC_Date.fecha[1];  		  	//MES
		buff_aux[i++] = RTC_Date.fecha[2];  		  	//AÑO

		buff_aux[i++] = AIR_UPDATE_ID[0];  			    //ID msb
		buff_aux[i++] = AIR_UPDATE_ID[1];  			    //ID lsb

		N = i+1;
		Tx_cmdTRANSPARENTE(N, buff_aux, reint_3 );
    }

  /* TRANSMITIR LECTURA DE PARAMETRO PROGRAMABLE */
  /***********************************************/
    static byte doTxBuffer_progParameter (byte* param_ptr){
      byte TO_F;
      byte* ptr;
      byte N;
      
      ptr = AirRead_Buffer;             // Puntero apuntando a Buffer de Tx
      N = 0;                            // Resetep Longitud
      
      *ptr++ = AIR_READ_ID[0];          // Agrego parte alta del ID de lectura
      N++;                              // Incremento Longitud de Datos
      *ptr++ = AIR_READ_ID[1];          // Agrego parte baja del ID de lectura
      N++;                              // Incremento Longitud de Datos

      
      TO_F = 0;                         // Limpio Bandera
      dispararTO_lazo();                // Disparo TimeOut Lazo
      while ((*param_ptr != finEEPROM_H) || (*(param_ptr+1) != finEEPROM_L) && !TO_F && N<240){
        TO_F = chkTO_lazo_F();          // Chequeo bandera de time out de lazo 
        *ptr++ = *param_ptr++;          // Agrego datos
        N++;                            // Incremento Longitud de Datos
      }
      detenerTO_lazo();                 // Detener TimeOut Lazo
      
      if (!TO_F){
        CMD_AirRead.N = N;              // Agrego Longitud
        CMD_AirRead.N += N_CMD;         // Agrego cantidad de bytes necesarias para Tx CMD
        
        // Agrego fin de datos
        *ptr++ = fin_datos_msb;           
        *ptr++ = fin_datos_lsb;
        
        CMD_AirRead.Tx_F = 1;           // Levanto Bandera de Tx
        CMD_AirRead.Reintentos = cantReint;
      }
      
      return(TO_F);
    }    

  
  /* TRANSMITIR LECTURA DE ZONA/BASE */
  /***********************************/
    static byte doTxBuffer_ZonaBase (byte* ZB_ptr){
      byte TO_F;
      byte* ptr;
      byte* endPTR;
      byte N;
      
      ptr = AirRead_Buffer;                 // Puntero apuntando a Buffer de Tx
      N = 0;                                // Resetep Longitud
      
      *ptr++ = AIR_READ_ID[0];              // Agrego parte alta del ID de lectura
      N++;                                  // Incremento Longitud de Datos
      *ptr++ = AIR_READ_ID[1];              // Agrego parte baja del ID de lectura
      N++;                                  // Incremento Longitud de Datos
      
      TO_F = 0;                             // Limpio Bandera
      dispararTO_lazo();                    // Disparo TimeOut Lazo
      endPTR = ZB_ptr + sizeof(tZonaBase);  // Puntero de fin de datos
      while ((ZB_ptr != endPTR) && !TO_F && N<240){
        TO_F = chkTO_lazo_F();              // Chequeo bandera de time out de lazo 
        *ptr++ = *ZB_ptr++;                 // Agrego datos
        N++;                                // Incremento Longitud de Datos
      }
      detenerTO_lazo();                     // Detener TimeOut Lazo
      
      if (!TO_F){
        CMD_AirRead.N = N;                  // Agrego Longitud
        CMD_AirRead.N += N_CMD;             // Agrego cantidad de bytes necesarias para Tx CMD
        
        // Agrego fin de datos
        *ptr++ = fin_datos_msb;           
        *ptr++ = fin_datos_lsb;
        
        CMD_AirRead.Tx_F = 1;           // Levanto Bandera de Tx
        CMD_AirRead.Reintentos = cantReint;
      }
      
      return(TO_F);
    }    

  
  /* TRANSMITIR LECTURA DE POSICION y VELOCIDAD */
  /**********************************************/
    static byte doTxBuffer_readPyV (byte*far PyV_param_inFlash){
      byte* ptr;
      byte N;
      
      ptr = AirRead_Buffer;                 // Puntero apuntando a Buffer de Tx
      N = 0;                                // Resetep Longitud
      
      *ptr++ = AIR_READ_ID[0];              // Agrego parte alta del ID de lectura
      N++;                                  // Incremento Longitud de Datos
      *ptr++ = AIR_READ_ID[1];              // Agrego parte baja del ID de lectura
      N++;                                  // Incremento Longitud de Datos
      
      *ptr++ = *PyV_param_inFlash++;        // Agrego datos
      N++;                                  // Incremento Longitud de Datos
      *ptr++ = *PyV_param_inFlash++;        // Agrego datos
      N++;                                  // Incremento Longitud de Datos
      *ptr++ = *PyV_param_inFlash++;        // Agrego datos
      N++;                                  // Incremento Longitud de Datos
      *ptr++ = *PyV_param_inFlash++;        // Agrego datos
      N++;                                  // Incremento Longitud de Datos
      *ptr++ = *PyV_param_inFlash++;        // Agrego datos
      N++;                                  // Incremento Longitud de Datos
      *ptr++ = *PyV_param_inFlash++;        // Agrego datos
      N++;                                  // Incremento Longitud de Datos
      
      CMD_AirRead.N = N;                    // Agrego Longitud
      CMD_AirRead.N += N_CMD;               // Agrego cantidad de bytes necesarias para Tx CMD
      
      // Agrego fin de datos
      *ptr++ = fin_datos_msb;           
      *ptr++ = fin_datos_lsb;
      
      CMD_AirRead.Tx_F = 1;                 // Levanto Bandera de Tx
      CMD_AirRead.Reintentos = cantReint;
      
      return(0);
    }    


  /* TRANSMITIR LECTURA DE PARAMETROS ENTRADA/SALIDA BASE */
  /********************************************************/
    static byte doTxBuffer_readInOutBase (byte*far BASEinOut_param_inFlash){
      byte* ptr;
      byte N;
      
      ptr = AirRead_Buffer;                 // Puntero apuntando a Buffer de Tx
      N = 0;                                // Resetep Longitud
      
      *ptr++ = AIR_READ_ID[0];              // Agrego parte alta del ID de lectura
      N++;                                  // Incremento Longitud de Datos
      *ptr++ = AIR_READ_ID[1];              // Agrego parte baja del ID de lectura
      N++;                                  // Incremento Longitud de Datos
      
      *ptr++ = *BASEinOut_param_inFlash++;  // Agrego datos
      N++;                                  // Incremento Longitud de Datos
      *ptr++ = *BASEinOut_param_inFlash++;  // Agrego datos
      N++;                                  // Incremento Longitud de Datos
      *ptr++ = *BASEinOut_param_inFlash++;  // Agrego datos
      N++;                                  // Incremento Longitud de Datos
      *ptr++ = *BASEinOut_param_inFlash++;  // Agrego datos
      N++;                                  // Incremento Longitud de Datos
      *ptr++ = *BASEinOut_param_inFlash++;  // Agrego datos
      N++;                                  // Incremento Longitud de Datos
      *ptr++ = *BASEinOut_param_inFlash++;  // Agrego datos
      N++;                                  // Incremento Longitud de Datos
      
      CMD_AirRead.N = N;                    // Agrego Longitud
      CMD_AirRead.N += N_CMD;               // Agrego cantidad de bytes necesarias para Tx CMD
      
      // Agrego fin de datos
      *ptr++ = fin_datos_msb;           
      *ptr++ = fin_datos_lsb;
      
      CMD_AirRead.Tx_F = 1;                 // Levanto Bandera de Tx
      CMD_AirRead.Reintentos = cantReint;
      
      return(0);
    }    
    
        
/*********************************************************************************************/
/* ACCIONES y UPDATES */
/**********************/
/* 
 Estas rutinas son duales, ya que en ellas:
  - Se procesa la ACCION solicitada por el COMANDO
  - Se realizar la actualizacion de la Memoria No Volatil

 Esta dualidad depende de si ya esta el pedido de actualizacion o no

 El resultado de esta funcion (error) solo se usa para la actualizacion, es
 decir, la grabacion de la memoria no volatil (NVM).
 Y es para confirmar la actualizacion. En caso de que no haya error, se envia
 a la Central el comando de actualizacion satisfactoria
*/

  /* 00 - ACTUALIZAR ZONA */
  /************************/
    static byte updateZona (byte* values){
      
      return(1);
    }


  /* 01 - ACTUALIZAR BASE */
  /************************/
    static byte updateBase (byte* values){
      
      return(1);
    }    


  /* 02 - ACTUALIZAR PROGRAMACION DE RADIO */
  /*****************************************/
    static byte updateProgRadio (byte* values){
      return(1);
    }    


  /* 03 - ACTUALIZAR PROGRAMACION DE RELOJ (COMUN) */
  /*************************************************/
    static byte updateProgReloj_comun (byte* values){
      byte error;
      byte N;
      
       error = 1;                            // Asumo error
       // PROCESO DE ACCION
       PROG_saveRELOJ_COMUN_air(values, AIR_UPDATE_values_N+1); // Guardo datos de Actualizacion
      
		// GRABACION NVM
		error = PROG_writeRELOJ_COMUN_air(AIR_UPDATE_values_N); // Re-Programo la EEPROM
		#ifdef VISOR_PRINT_TICKET_FULL
		  if (REPORTES_HABILITADOS && !error){
			(void)REPORTE_queueVarios(getDate(), RELOJ_INTERNO_getChofer(), 0, progAire); // Encolo Programacion por Aire
		  }
		#endif

		return(error);
    }



  /* 04 - ACTUALIZAR TARIFA */
  /**************************/
    static byte updateProgReloj_tarifa (byte* values){
      // En los valores, el primer byte es el "indice" de Tarifa.
      // Y luego si estan los valores a grabar en EEPROM
      // Valores de 1-8
      byte error;
      
        error = 1;                            // Asumo error
        // PROCESO DE ACCION
        PROG_saveRELOJ_TARIFA_air(values, AIR_UPDATE_values_N); // Guardo datos de Actualizacion  
      
        // GRABACION NVM
        updateRequest = 0;                  // Bajo 
        error = PROG_writeRELOJ_TARIFA_air(AIR_UPDATE_values_N-1); // Re-Programo la EEPROM (N-1 xq indica tarifa)
        #ifdef VISOR_PRINT_TICKET_FULL
          if (REPORTES_HABILITADOS && !error){
            (void)REPORTE_queueVarios(getDate(), RELOJ_INTERNO_getChofer(), 0, progAire); // Encolo Programacion por Aire
          }
        #endif

        return(error);
    }
  
 
  /* 05 - ACTUALIZAR PROGRAMACION DE RELOJ (EqPESOS) */
  /***************************************************/
    static byte updateProgReloj_eqpesos (byte* values){
      byte error;
      
      error = 1;                            // Asumo error
      if (updateRequest == 0){
        // PROCESO DE ACCION
        updateRequest = 1;                  // Seteo pedido de actualizacion
        PROG_saveRELOJ_EqPESOS_air(values, AIR_UPDATE_values_N); // Guardo datos de Actualizacion  
      
      }else{
        // GRABACION NVM
        updateRequest = 0;                  // Bajo 
        error = PROG_writeRELOJ_EqPESOS_air(AIR_UPDATE_values_N); // Re-Programo la EEPROM
        #ifdef VISOR_PRINT_TICKET_FULL
          if (REPORTES_HABILITADOS && !error){
            (void)REPORTE_queueVarios(getDate(), RELOJ_INTERNO_getChofer(), 0, progAire); // Encolo Programacion por Aire
          }
        #endif
      }
      
      return(error);
    }    
  
  
  /* 06 - ACTUALIZAR PROGRAMACION DE RELOJ (CALENDARIO) */
  /******************************************************/
    static byte updateProgReloj_calendario (byte* values){
      byte error;
      
      error = 1;                            // Asumo error
      if (updateRequest == 0){
        // PROCESO DE ACCION
        updateRequest = 1;                  // Seteo pedido de actualizacion
        PROG_saveRELOJ_CALEND_air(values, AIR_UPDATE_values_N); // Guardo datos de Actualizacion  
      
      }else{
        // GRABACION NVM
        updateRequest = 0;                  // Bajo 
        error = PROG_writeRELOJ_CALEND_air(AIR_UPDATE_values_N); // Re-Programo la EEPROM
        #ifdef VISOR_PRINT_TICKET_FULL
          if (REPORTES_HABILITADOS && !error){
            (void)REPORTE_queueVarios(getDate(), RELOJ_INTERNO_getChofer(), 0, progAire); // Encolo Programacion por Aire
          }
        #endif
      }
      
      return(error);
    }    


  /* 07 - ACTUALIZAR PROGRAMACION DE GEO FENCE */
  /*********************************************/
    static byte updateProgGeoFence (byte* values){
      // En los valores, el primer byte es el "indice" de Geo Fence:
      //  0 - GF 1 y 2
      //  1 - GF 3 y 4
      //  2 - GF 5 y 6
      //  3 - GF 7 y 9
      //  4 - GF 9 y 10
      //  5 - GF 11 y 12
      //  6 - GF 13 y 14
      //  7 - GF 15 y 16
      //
      // Y luego si estan los valores a grabar en EEPROM
      byte error;
      
      error = 1;                            // Asumo error
      
            
      return(error);
    }



/*********************************************************************************************/
/* LECTURAS */
/************/
/* 
  Se genera el comando de LECTURA y luego se comanda su transmision
*/
    /* 00 - LEER ZONA */
      /******************/
        static byte readZona (byte value){
          //return(doTxBuffer_ZonaBase((byte*) &TABLA_ZONAS[value]));
        }


      /* 01 - LEER BASE */
      /******************/
        static byte readBase (byte value){
          //return(doTxBuffer_ZonaBase((byte*) &TABLA_BASES[value]));
        }

  /* 02 - LEER PROGRAMACION DE RADIO */
  /***********************************/
    static byte readProgRadio (byte value){
      return(doTxBuffer_progParameter((byte*) &EEPROM_PROG_RADIO));
    }    


  /* 03 - LEER PROGRAMACION DE RELOJ (COMUN) */
  /*******************************************/
    static byte readProgReloj_comun (byte value){
      return(doTxBuffer_progParameter((byte*) &EEPROM_PROG_relojCOMUN));
    }



  /* 04 - LEER TARIFA */
  /********************/
    static byte readProgReloj_tarifa (byte value){
      // TARIFA: de 1-8
      //return(doTxBuffer_progParameter((byte*) PROG_RELOJtarifa_getEEPROM_ptr(value)));
    	return(doTxBuffer_progParameter((byte*) getDir_tarifaX_BlockProg(value, ADDRESS_PROG1)));
    }
  
    /* 05 - LEER PROGRAMACION DE RELOJ (EqPESOS) */
  /*********************************************/
    static byte readProgReloj_eqpesos (byte value){
      return(doTxBuffer_progParameter((byte*) &EEPROM_PROG_relojEqPESOS));
    }    
  
  
  
  /* 06 - LEER PROGRAMACION DE RELOJ (CALENDARIO) */
  /************************************************/
    static byte readProgReloj_calendario (byte value){
      return(doTxBuffer_progParameter((byte*) &EEPROM_PROG_relojCALEND));
    } 


  /* 07 - LEER GEO FENCE */
  /***********************/
    static byte readProgGeoFence (byte value){
      // GF: de 0-7
        return(1); //ERROR
    }    

  


  

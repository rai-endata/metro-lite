/* File: <Programacion Parametros.c> */

/* INCLUDE */
/***********/
  #include "- metroBLUE Config -.h"
  #include "Main.h"
  #include "Manejo de Buffers.h"
  #include "Programacion Parametros.h"
  #include "Programacion Config.h"
  #include "Parametros Reloj.h"
  #include "Parametros Movil.h"
  #include "Reloj.h"
  #include "Parametros Ticket.h"
    //#include "- Serie RelojProg Selection -.h"
  #include "Timer.h"
  #include "seriePROG.h"
  #include "Buzzer.h"
  #include "display-7seg.h"
  #include "tipo de equipo.h"
  #include "eeprom.h"
  
  //#include "Air Update.h"
  

/*********************************************************************************************/
/* ESTRUCTURAS */
/***************/
  
  typedef void (*PROG_ptr)(byte N);   // Tipo Puntero de PROGRAMACION

/*********************************************************************************************/
/* VARIABLES */
/***************/
  

byte longTarifa;
tBAX_SCI RLJ_PRG_SCI;         // Datos de SCI segun protocolo BAX para RELOJ y PROG

byte no_grabo_nada;
uint8_t finPRG_TO_RESET_TO_cnt;


/*********************************************************************************************/
/* PROTOTIPOS */
/**************/
  // RUTINAS INTERNAS
  static void PROGRAMADOR_processRx (tBAX_SCI* BAX_SCI_ptr);
  static void PROG_none (byte N);
  static void PROG_finPROGRAMACION (byte N);
  static void PROGRAMADOR_incProgBuffer_ptr (byte** ptr_ptr);
    

/*********************************************************************************************/
/* CONSTANTES */
/**************/
  static const PROG_ptr  PROGRAMADOR_actions[40][2]={
    // Acciones de lectura o escritura
    // El offset es el (subCMD - 0x80)
    
    /*      LECTURA      *//*|*//*      ESCRITURA      */
    /*********************//*|*//***********************/
		PROG_readMOVIL      ,    PROG_writeMOVIL,           // 0x80
		PROG_readRADIO      ,    PROG_writeRADIO    ,      // 0x81
		PROG_none     		,    PROG_none      ,      		// 0x82
		PROG_none     		,    PROG_none      ,      		// 0x83

		PROG_none     		,    PROG_none      ,            // 0x84
		PROG_readRELOJcomun ,    PROG_writeRELOJcomun  ,     // 0x85
		PROG_readRELOJ_T1D  ,    PROG_writeRELOJ_T1D  ,      // 0x86
		PROG_readRELOJ_T2D  ,    PROG_writeRELOJ_T2D  ,      // 0x87
		PROG_readRELOJ_T3D  ,    PROG_writeRELOJ_T3D  ,      // 0x88
		PROG_readRELOJ_T4D  ,    PROG_writeRELOJ_T4D  ,      // 0x89
		PROG_none       	,    PROG_finPROGRAMACION  ,     // 0x8A
#ifdef VISOR_PROG_TICKET
        PROG_readTICKET     ,     PROG_writeTICKET  ,      	 // 0x8B
#else         
		PROG_none       	,     PROG_none      ,      	 // 0x8B
#endif
		PROG_readRELOJ_T1N  ,     PROG_writeRELOJ_T1N  ,      // 0x8C
		PROG_readRELOJ_T2N  ,     PROG_writeRELOJ_T2N  ,      // 0x8D
		PROG_readRELOJ_T3N  ,     PROG_writeRELOJ_T3N  ,      // 0x8E
		PROG_readRELOJ_T4N  ,     PROG_writeRELOJ_T4N  ,      // 0x8F
		PROG_readRELOJcalendar  , PROG_writeRELOJcalendar ,   // 0x90
		PROG_none       	,     PROG_none          ,  	// 0x91
		PROG_none       	,     PROG_none          ,  // 0x92
		PROG_none       	,     PROG_none          ,  // 0x93
		PROG_none       	,     PROG_none          ,  // 0x94
		PROG_none       	,     PROG_none          ,  // 0x95
		PROG_none       	,     PROG_none          ,  // 0x96
		PROG_none       	,     PROG_none          ,  // 0x97
		PROG_none       	,     PROG_none          ,  // 0x98
		PROG_readRELOJeqPesos   , PROG_writeRELOJeqPesos      ,  // 0x99
		PROG_none       	,     PROG_none      ,      // 0x9A
#ifdef VISOR_PROG_TICKET
		PROG_readTICKET_RECAUD  , PROG_writeTICKET_RECAUD   ,  // 0x9B
#else
		PROG_none       	,     PROG_none          ,  // 0x9B
#endif
		PROG_none       	,     PROG_none      ,      // 0x9C
		PROG_none       	,     PROG_none      ,      // 0x9D
		PROG_none       	,     PROG_none      ,      // 0x9E
		PROG_none       	,     PROG_none      ,      // 0x9F
		PROG_none       	,     PROG_none      ,      // 0xA0
		PROG_none       	,     PROG_none      ,      // 0xA1
		PROG_none       	,     PROG_none      ,      // 0xA2
		PROG_none       	,     PROG_none      ,      // 0xA3
		PROG_none       	,     PROG_none      ,      // 0xA4
		PROG_none       	,     PROG_none      ,      // 0xA5
		PROG_none       	,     PROG_none      ,      // 0xA6
		PROG_none       	,     PROG_none             // 0xA7
  };




/*********************************************************************************************/
/* VARIABLES */
/*************/
  PRG_FLAGS      _prog_F;               // Banderas de Programacion

  static byte    progBuffer[dim_progBuffer];
  static byte*   progPUTptr;            // Puntero de insercion de datos en buffer de programacion 
    #define incProgPUT                  PROGRAMADOR_incProgBuffer_ptr(&progPUTptr)


 // VARIABLES DE EEPROM
  PRG_FLAGS1     _prgEEPROM_F1;         // Banderas de Grabacion de EEPROM 1
  PRG_FLAGS2     _prgEEPROM_F2;         // Banderas de Grabacion de EEPROM 2
  PRG_FLAGS3     _prgEEPROM_F3;         // Banderas de Grabacion de EEPROM 3
  PRG_FLAGS4     _prgEEPROM_F4;         // Banderas de Grabacion de EEPROM 4
  
  PRG_FLAGS1     _prgOK_F1;             // Banderas de Parametro Correcto 1
  PRG_FLAGS2     _prgOK_F2;             // Banderas de Parametro Correcto 2
  PRG_FLAGS3     _prgOK_F3;             // Banderas de Parametro Correcto 3
  PRG_FLAGS4     _prgOK_F4;             // Banderas de Parametro Correcto 4

  // MACROS
  #define RECIBI_DATOS_PROG      (BAX_SCI_PROG.Rx_cnt > 0)
  #define PROCECE_DATOS_PROG     (BAX_SCI_PROG.Rx_cnt--)
  
  static byte ENDATA_progError;         // Contador de Errores de Programacion

/*********************************************************************************************/
/* RUTINAS */
/***********/

  /* INICILIZACION DE PROGRAMACION */
  /*********************************/
    void PROGRAMADOR_ini (void){
      
      progPUTptr = progBuffer;
      //inicializo punteros control serie
      PRG_ini();
      //levanta datos de programacion de RELOJ
      prgRELOJ_ini();

      #ifdef VISOR_PROG_TICKET
      //levanta datos de programacion de TICKET
       prgTICKET_ini();                  // Parametros por defecto si no hay nada programado
      #endif

       prgMOVIL_ini();

    }

  /* RECEPCION DE DATOS DE PROGRAMACION */
  /**************************************/
    uint16_t test_cnt;
    void PROGRAMADOR_chkRx (void){
      if (RECIBI_DATOS_PROG){
        PROCECE_DATOS_PROG;                 // Decremento contador de Rx
        test_cnt++;
        huart1.Rx_TO_cnt = 0;
        PROGRAMADOR_processRx(&BAX_SCI_PROG);
        
        BAX_SCI_PROG.RxGPA_GETidx++;       // Incremento indice GET del GPA
        if (BAX_SCI_PROG.RxGPA_GETidx == dim_BAX_GPA){
          BAX_SCI_PROG.RxGPA_GETidx = 0;   // Doy la vuelta al indice
        }


      }
    }


  /* TRANSMITIR COMANDO HACIA EL PROGRAMADOR */
  /*******************************************/
    void PROGRAMADOR_startTx(byte CMD, byte subCMD, byte* DATA_buffer){
      byte buffer[255];
      byte* ptr;
      byte TO_F, OVF_F;
      byte N,k;
      byte* bufferTarifa;

      
      ptr = buffer;

      
      *ptr++ = CMD;                     // Agrego Comando
      *ptr++ = subCMD;                  // Agrego SubComando
      
      TO_F = 0;                         // Limpio Bandera
      OVF_F = 0;                        // Limpio Bandera
      dispararTO_lazo();                // Disparo TimeOut Lazo
      N = 2;                            // Arranco en 2, x el CMD y sl subCMD
      while (((*DATA_buffer != finSCI_H) || (*(DATA_buffer+1) != finSCI_L)) && !TO_F && !OVF_F){
        TO_F = chkTO_lazo_F();          // Chequeo bandera de time out de lazo 
        
        *ptr++ = *DATA_buffer++;        // Agrego datos

        
        N++;
        if (N > 255 - 2){
          // El N supero el maximo permitido (dim - 2, para que que entren los 2
          // bytes de fin
          // => Debo salir y cancelar
          OVF_F = 1;                    // Ocurrio OVERFLOW
        }
      }
      

      detenerTO_lazo();                 // Detener TimeOut Lazo
      
      if (/*!OVF_F &&*/ !TO_F){


        *ptr++ = finSCI_H;              // Agrego finSCI_H
        *ptr++ = finSCI_L;              // Agrego finSCI_H

        
        PRG_startTx(N, buffer);         // Inicio transmision SCI
      
      }else{
        ENDATA_progError++;
      }
    }




  /* TRANSMITIR RESPUESTA HACIA EL PROGRAMADOR */
  /*******************************************/
    void PROGRAMADOR_startTxRTA(byte CMD){
      // La respuesta solo se transmite si no hay errores en el comando que envia el
      // PROGRAMADOR. Sino no responde, para que el programador acuse error de
      // programacion
      byte buffer[100];
      byte* ptr;
      byte N;
      
      ptr = buffer;

      *ptr++ = CMD - 0x80;              // Agrego Comando
      N=1;

      *ptr++ = finSCI_H;                // Agrego finSCI_H
      *ptr++ = finSCI_L;                // Agrego finSCI_H
      //Agrego protocolo BAX a los N datos a transmitir
      //e incrementa BAX_SCI_PROG.Tx_cnt
      //con lo cual luego PROGRAMADOR_chkTx se ocupara
      //de iniciar transmision si se puede
      PRG_startTx(N, buffer);           //
    }       


  /* VERIFICACION DE LONGITUD DE DATOS */
  /*************************************/
    tPRG_ERROR PROGRAMADOR_chkN (byte N_Rx, byte N_expect){
      // El N_expect es el N ESPERADO, y el N_Rx, el RECIBIDO. El N recibido incluye
      // al subCMD, el CMD y ChkSum; mientras que el N Esperado solo contempla la 
      // cantidad de datos. Por lo tanto para compararlos, tengo que retarle 3 al Rx
      tPRG_ERROR error;
      
      if ((N_Rx - 3) != N_expect){
        error = PRG_ERROR_N;
      }else{
        error = PRG_OK;
      }
      
      return(error);
    }

  /* AGREGAR DATOS RECIBIDOS A BUFFER DE PROGRAMACION */
  /****************************************************/
    byte* PROGRAMADOR_addRxData (tBAX_SCI* BAX_SCI_ptr){
      // Esta rutina agrega los datos recibidos por la serie al buffer de programacion;
      // para luego ser grabados. Devuelve un puntero al inicio de estos datos, dentro
      // del buffer de programacion => A la hora de grabar la EEPROM, extrae los datos
      // segun ese puntero.
      //
      // En caso de error, devuelve NULL
      byte* returnPTR;
      byte N, CMD, subCMD;
      byte* GETptr;
      byte TO_F;
      
      returnPTR = progPUTptr;                                       // Puntero de extraccion (el que voy a devovler)
      
      GETptr = BAX_SCI_ptr->RxGPA[BAX_SCI_ptr->RxGPA_GETidx];       // Extraigo puntero GET, para poder tomar su direccion
      N = get_byte (&GETptr, BAX_SCI_ptr->RxBuffer, dim_BAX);       // Extraigo N

      CMD = get_byte (&GETptr, BAX_SCI_ptr->RxBuffer, dim_BAX);     // Extraigo Comando
      subCMD = get_byte (&GETptr, BAX_SCI_ptr->RxBuffer, dim_BAX);  // Extraigo Sub-Comando
      
      TO_F = 0;                                                     // Limpio Bandera
      dispararTO_lazo();                                            // Disparo TimeOut Lazo
      while ((N > 0) && (!TO_F)){
        TO_F = chkTO_lazo_F();                                      // Chequeo bandera de time out de lazo 
        N--;                                                        // Decremento longitud de datos
        *progPUTptr = get_byte (&GETptr, BAX_SCI_ptr->RxBuffer, dim_BAX); // Voy extrayendo y guardando el dato de programacion 
        incProgPUT;                                                 // Avanzo puntero PUT
      }
      detenerTO_lazo();                                             // Detener TimeOut Lazo
      
      if (TO_F){
        // En caso de error, reestablezco el puntero PUT y devuelvo NULL
        progPUTptr = returnPTR;                                     // Reestablezco puntero PUT
        returnPTR = NULL;                                           // devuelvo NULL
      }
       
      return(returnPTR);
    }
    
    
    /* AGREGAR BYTE A BUFFER DE PROGRAMACION */
    /*****************************************/
    byte* PROGRAMADOR_addByte (byte dato){
      // Esta rutina agrega un solo byte al buffer de programacion; para luego ser grabados.
      // Devuelve un puntero al inicio de estos datos, dentro del buffer de programacion
      // => A la hora de grabar la EEPROM, extrae los datos segun ese puntero.
      byte* returnPTR;

      returnPTR = progPUTptr;         // Puntero de extraccion (el que voy a devovler)
      
      *progPUTptr = dato;             // Agrego dato
      incProgPUT;                     // Avanzo puntero PUT
       
      return(returnPTR);
    }


  /* EXTRAER DATOS DE BUFFER DE PROGRAMACION */
  /*******************************************/
    byte PROGRAMADOR_getProgData (byte** ptr_ptr){
      byte data;
      
      data = **ptr_ptr;
      PROGRAMADOR_incProgBuffer_ptr(ptr_ptr);
      return(data);
    }
    
    
  /* GUARDAR STRING DE PROGRAMACION */
  /**********************************/
    tPRG_ERROR PROGRAMADOR_saveSTRING (byte* RAMbuffer, tBAX_SCI* BAX_SCI_ptr){
      // Guarda temporalmente el string de programacion recibido por el programador
      // para hacer efectiva la programacion (entiendase, grabar la EEPROM) cuando
      // finaliza la misma
      volatile byte N, CMD, subCMD;
      byte* GETptr;
      byte TO_F;
      tPRG_ERROR error;
      
      error = PRG_OK;                                               // Asumo que no hay error
      
      GETptr = BAX_SCI_ptr->RxGPA[BAX_SCI_ptr->RxGPA_GETidx];       // Extraigo puntero GET, para poder tomar su direccion
      N = get_byte (&GETptr, BAX_SCI_ptr->RxBuffer, dim_BAX);       // Extraigo N

      CMD = get_byte (&GETptr, BAX_SCI_ptr->RxBuffer, dim_BAX);     // Extraigo Comando
      subCMD = get_byte (&GETptr, BAX_SCI_ptr->RxBuffer, dim_BAX);  // Extraigo Sub-Comando
      
      TO_F = 0;                                                     // Limpio Bandera
      dispararTO_lazo();                                            // Disparo TimeOut Lazo
      while ((N > 0) && (!TO_F)){
        TO_F = chkTO_lazo_F();                                      // Chequeo bandera de time out de lazo 
        N--;                                                        // Decremento longitud de datos
        *RAMbuffer++ = get_byte (&GETptr, BAX_SCI_ptr->RxBuffer, dim_BAX); // Voy extrayendo y guardando el string de programacion 
      }
      detenerTO_lazo();                                             // Detener TimeOut Lazo
      
      if (TO_F){
        error = PRG_ERROR_TO;                                       // Error de TimeOut
      }
      
      return (error);
    }


  /* PROGRAMACION DE PARAMETROS - aka GRABACIONES EN EEPROM */
  /**********************************************************/
    tEEPROM_ERROR PROGRAMADOR_grabarEEPROM (void){
      tEEPROM_ERROR error;
      
      // Programacion de Reloj
      error = PROG_RELOJcomunes_grabarEEPROM();     // Grabacion de Parametros de Reloj Comunes en EEPROM
      
      if (error == EEPROM_OK){
        error = PROG_RELOJT1D_grabarEEPROM;         // Grabacion de Tarifa 1 Diurna en EEPROM
      }
      if (error == EEPROM_OK){
        error = PROG_RELOJT2D_grabarEEPROM;         // Grabacion de Tarifa 2 Diurna en EEPROM
      }
      if (error == EEPROM_OK){
        error = PROG_RELOJT3D_grabarEEPROM;         // Grabacion de Tarifa 3 Diurna en EEPROM
      }
      if (error == EEPROM_OK){
        error = PROG_RELOJT4D_grabarEEPROM;         // Grabacion de Tarifa 4 Diurna en EEPROM
      }
      if (error == EEPROM_OK){
        error = PROG_RELOJT1N_grabarEEPROM;         // Grabacion de Tarifa 1 Nocturna en EEPROM
      }
      if (error == EEPROM_OK){
        error = PROG_RELOJT2N_grabarEEPROM;         // Grabacion de Tarifa 2 Nocturna en EEPROM
      }
      if (error == EEPROM_OK){
        error = PROG_RELOJT3N_grabarEEPROM;         // Grabacion de Tarifa 3 Nocturna en EEPROM
      }
      if (error == EEPROM_OK){
        error = PROG_RELOJT4N_grabarEEPROM;         // Grabacion de Tarifa 4 Nocturna en EEPROM
      }
      if (error == EEPROM_OK){
        error = PROG_RELOJeqPesos_grabarEEPROM();   // Grabacion de Equivalencia en Pesos en EEPROM
      }
      if (error == EEPROM_OK){
        error = PROG_RELOJcalend_grabarEEPROM();    // Grabacion de Calendario en EEPROM
        //levantar_progRELOJ();
        if (error == EEPROM_OK){
        	prgRELOJ_ini();
        }
      }

      // Programacion de Ticket
      #ifdef VISOR_PROG_TICKET
        if (error == EEPROM_OK){
          error = PROG_TICKET_grabarEEPROM();       // Grabacion de Parametros de Ticket en EEPROM
        }
        
        if (error == EEPROM_OK){
          error = PROG_TICKET_RECAUD_grabarEEPROM();// Grabacion de Parametros de Ticket Recaudacion en EEPROM
          if(error == EEPROM_OK){
        	  levantar_progTICKET();
        	  finPRG_TO_RESET_TO_cnt = FIN_PRG_TO;
          }
        }
      #endif
            
 	//Programacion de Movil + Radio
	if (error == EEPROM_OK){
	  error = PROG_MOVIL_grabarEEPROM();          // Grabacion de Parametros de Movil en EEPROM
	  EEPROM_ReadBuffer(&EEPROM_PROG_MOVIL,ADDRESS_PROG_MOVIL,sizeof(tPARAM_MOVIL));
      if (!TIPO_RELOJ_VALIDO){
    	  error = EEPROM_ACCESS_ERROR;
      }
	}

      // Fin de Grabaciones
      if ((error == EEPROM_OK) && prgREQUEST_F){
        // Si no hubo error al culminar las grabaciones de los parametros en EEPROM =>
        // ya puedo enviar la respuesta a la programación y disparo un timer para
        // resetear el equipo. [El timer es para no esperar que finalice el envio
        // del comando de RTA a Fin de Programacion]
        // El REQUEST es xq sino, cada vez que entra a grabar LO QUE SEA a la EEPROM
        // devuelve OK, xq no hay nada por grabar y enviaría por la serie el comando
        // de fin. Con esta bandera se obvia eso.
        // Lo mismo sucede cuando arranca por primera vez el equipo que carga los valores
        // por defecto y comanda su programacion. Enviaría fin de Programacion cuando en
        // realidad el programados nunca dio la orden de grabacion
        prgREQUEST_F = 0;
        
        ///comparar lo grabado con lo recibido

        prgFIN_F = 1;                             // Fin de Programación => Comando envio de Rta a FIN
 
        no_grabo_nada=0;
        //BUZZER_play(RING_ok);                  // Reproduzo sonido de grabacion correcta
       	 Buzzer_On(BEEP_PROGRAMCION_OK);
       	//lo suficiente como para que termine de sonar el buzzer
       	 finPRG_TO_RESET_TO_cnt = FIN_PRG_TO;

      }else if (error != EEPROM_OK){
    	  prgREQUEST_F = 0;
    	  // BUZZER_play(RING_error);                  // Reproduzo sonido de error
    	  Buzzer_On(BEEP_PROGRAMCION_ERROR);
    	  if((tipo_de_equipo == METRO_BLUE)){
    		  prog_mode=1;
    	  }
      }
      
      if(error == EEPROM_OK){
      //indica que hubo al menos una grabacion correcta
       no_grabo_nada=0;
      }
 
      
      return(error);
    }
    




  /* RESETEAR EQUIPO POR PROGRAMACION */
  /************************************/
    void PROGRAMADOR_reset (void){
      // Al finalizar el tono de programacion se debe resetear el equipo.
      if (prgRESET_F){
        prgRESET_F = 0;                 // Bajo Bandera
        
        MOTIVO_RESET = RST_PROG;        // Reset por Programacion
        resetVISOR;                     // Resetear Visor Compact
      }
    }    


  /* CHEQUEO DEL JUMPER DE PROGRAMACION */
  /**************************************/    
    byte PROGRAMADOR_chkJumper (void){
      byte jumper;

      #ifdef VISOR_CHK_JUMPER_PROG
        jumper = 0;
        if (jumper_pressed == jumperON){
          jumper = 1;
        }
      
      #else
        jumper = 1;
      #endif
      //el equipo metrolite no se fija jumper de programacion
	  if((tipo_de_equipo == METRO_LITE)){
		  jumper = 1;
	  }

      return(jumper);
    }

    void check_jumperPROG(void){
    	GPIO_PinState pinSTATE;

    	pinSTATE = HAL_GPIO_ReadPin(JUMPER_PROG_PORT, JUMPER_PROG_PIN);
    	if(!pinSTATE){
    	  jumper_pressed=1;
      	  if((tipo_de_equipo == METRO_BLUE)){
      		  prog_mode=1;
      	  }

    	}
    }

    void chkProgMode(void){
    	if(!prog_mode){
        	if(jumper_pressed==1){
			  if((tipo_de_equipo == METRO_BLUE)){
				  prog_mode=1;
			  }
        	}
    	}
    }

    void chkEepromDATA(void){
    	if(error_eepromDATA){
      	  if((tipo_de_equipo == METRO_BLUE)){
      		  prog_mode=1;
      	  }
      	  Buzzer_On(BEEP_PROGRAMCION_ERROR);
    	}
    }



  /* ENVIAR "NO SE ENCUENTRA EL JUMPER DE PROGRAMACION" */
  /******************************************************/    
    void PROGRAMADOR_TxNoJumper (void){
      byte data[2];
      
      data[0] = finSCI_H;
      data[1] = finSCI_L;
      
      PROGRAMADOR_startTx(CMD_INTERNO, 0x02, data);
    }


  /* DETERMINAR SI ESTOY POR RESETEAR EL EQUIPO POR PROGRAMACION */
  /***************************************************************/        
    byte PROGRAMADOR_chkReset (void){
      // Funcion que me informa si tras finalizada una programacion, estoy
      // por resetear el equipo. Por ahora solo la uso en onFirstDate
      return(prgRESET_F || (MOTIVO_RESET == RST_PROG));
    }
        

    /* FIN DE PROGRAMACION */
     /***********************/
       void PROGRAMADOR_fin (void){
         // Al finalizar la programacion, debe enviar la RTA A FIN para que el programador
         // displaye el OK en su pantalla.
         // Además, reproduce un sonido de programacion
         if (prgFIN_F){
           prgFIN_F = 0;                             // Bajo Bandera

           PROGRAMADOR_startTxRTA(subCMD_FIN_PROG);  // Envio la Rta hacia el programador

          // BUZZER_play(RING_programacion);           // Reproduzo sonido de programacion

           prgRESET_F = 1;                           // Resetear equipo (al finalizar tonos)
         }
       }
/*********************************************************************************************/
/* RUTINAS INTERNAS */
/********************/

  /* PROCESAR DATOS PROGRAMACION */
  /*******************************/
    static void PROGRAMADOR_processRx (tBAX_SCI* BAX_SCI_ptr){
      // Los datos enviados por el programador se guardan de la forma
      //
      //                    | N | ...DATOS... |
      //
      // El N incluye al N
      volatile uint16_t N, CMD, subCMD;
      byte* GETptr;

      GETptr = BAX_SCI_ptr->RxGPA[BAX_SCI_ptr->RxGPA_GETidx];       // Extraigo puntero GET, para poder tomar su direccion
      N = get_byte (&GETptr, BAX_SCI_ptr->RxBuffer, dim_BAX);       // Extraigo N
      
      
      CMD = get_byte (&GETptr, BAX_SCI_ptr->RxBuffer, dim_BAX);     // Extraigo Comando
      subCMD = get_byte (&GETptr, BAX_SCI_ptr->RxBuffer, dim_BAX);  // Extraigo Sub-Comando
  //prueba
      if(subCMD == 0x90){
    	  __asm volatile ("nop");
      }
  ////////

      if((subCMD - subCMD_MOVIL<30) && (CMD >= CMD_LECTURA)){
    	  PROGRAMADOR_actions[subCMD - subCMD_MOVIL][CMD - CMD_LECTURA](N);
      }

    }



  /* PROG NONE */
  /*************/
    static void PROG_none (byte N){
    }


  /* FIN DE PROGRAMACION */
  /***********************/
    static void PROG_finPROGRAMACION (byte N){
      // Cuando recibo el comando de fin de programación, el móvil corrobora que los 
      // parametros recibidos hayan sido correctamente procesados y de ser así, comanda
      // la grabación de los mismo en EEPROM. 
      // Al finalizar la grabación, recien ahi, responde al comando de fin de programación;
      // justamente para que en el programador se ve el OK.
      // Luego, el equipo se resete solo y levanta los nuevos parametros programados
      tPRG_ERROR error;
      
      error = PROGRAMADOR_chkN (N, N_lectura);    // Comparo que el N recibido se igual al esperado
      
      if (error == PRG_OK){
    	  // PROGRAMACION DE MOVIL y RADIO
    	if (prgMOVIL_OK_F){
			// El comando de progrmación de RADIO y programación de MOVIL han sido recibidos
			// y procesados satisfactoriamente => Ya puedo "programar" el equipo, es decir
			// grabarlos en la EEPROM
			prgMOVIL_OK_F = 0;            // Bajo Bandera
			prgREQUEST_F = 1;             // Peido de Grabacion de EEPROM
			PROG_MOVIL_to_EEPROM(0);      // Grabar parametros en EEPROM, sin iniciar EEPROM IRQ
			EEPROM_iniGrabacion();        // Inicio grabacion de EEPROM AL INSTANTE xq es por IRQ
    	}
        // PROGRAMACION DE RELOJ
        if (prgRELOJ_COMUNES_OK_F && prgRELOJ_T1D_OK_F && prgRELOJ_T2D_OK_F &&
            prgRELOJ_T3D_OK_F && prgRELOJ_T4D_OK_F && prgRELOJ_T1N_OK_F &&
            prgRELOJ_T2N_OK_F && prgRELOJ_T3N_OK_F && prgRELOJ_T4N_OK_F &&
            prgRELOJ_EQPESOS_OK_F && prgRELOJ_CALEND_OK_F){
          // Los comando de progrmación de RELOJ han sido recibidos y procesados 
          // satisfactoriamente => Ya puedo "programar" el equipo, es decir
          // grabarlos en la EEPROM
          prgRELOJ_COMUNES_OK_F = 0;              // Bajo Bandera
          prgRELOJ_T1D_OK_F = 0;                  // Bajo Bandera
          prgRELOJ_T2D_OK_F = 0;                  // Bajo Bandera
          prgRELOJ_T3D_OK_F = 0;                  // Bajo Bandera
          prgRELOJ_T4D_OK_F = 0;                  // Bajo Bandera
          prgRELOJ_T1N_OK_F = 0;                  // Bajo Bandera
          prgRELOJ_T2N_OK_F = 0;                  // Bajo Bandera
          prgRELOJ_T3N_OK_F = 0;                  // Bajo Bandera
          prgRELOJ_T4N_OK_F = 0;                  // Bajo Bandera
          prgRELOJ_EQPESOS_OK_F = 0;              // Bajo Bandera
          prgRELOJ_CALEND_OK_F = 0;               // Bajo Bandera
          
          prgREQUEST_F = 1;                       // Pedio de Grabacion de EEPROM
          
          PROG_RELOJcomun_to_EEPROM(0);           // Levanto bandera de grabacion de programacion de parametros de reloj comunes en eeprom
          PROG_RELOJtarifa_to_EEPROM(tarifa1D,0); // Levanto bandera de grabacion de programacion de tarifa 1 diurna en eeprom
          PROG_RELOJtarifa_to_EEPROM(tarifa2D,0); // Levanto bandera de grabacion de programacion de tarifa 2 diurna en eeprom
          PROG_RELOJtarifa_to_EEPROM(tarifa3D,0); // Levanto bandera de grabacion de programacion de tarifa 3 diurna en eeprom
          PROG_RELOJtarifa_to_EEPROM(tarifa4D,0); // Levanto bandera de grabacion de programacion de tarifa 4 diurna en eeprom
          PROG_RELOJtarifa_to_EEPROM(tarifa1N,0); // Levanto bandera de grabacion de programacion de tarifa 1 nocturna en eeprom
          PROG_RELOJtarifa_to_EEPROM(tarifa2N,0); // Levanto bandera de grabacion de programacion de tarifa 2 nocturna en eeprom
          PROG_RELOJtarifa_to_EEPROM(tarifa3N,0); // Levanto bandera de grabacion de programacion de tarifa 3 nocturna en eeprom
          PROG_RELOJtarifa_to_EEPROM(tarifa4N,0); // Levanto bandera de grabacion de programacion de tarifa 4 nocturna en eeprom
          PROG_RELOJeqPesos_to_EEPROM(0);         // Levanto bandera de grabacion de programacion de eq pesos en eeprom
          PROG_RELOJcalend_to_EEPROM(0);          // Levanto bandera de grabacion de programacion de calendario en eeprom
          
          EEPROM_iniGrabacion();                  // Inicio grabacion de EEPROM AL INSTANTE xq es por IRQ
        }
        
        
        // PROGRAMACION DE TICKET
        #ifdef VISOR_PROG_TICKET
          if (prgTICKET_OK_F && prgTICKET_RECAUD_OK_F){
            // El comando de progrmación de RADIO y programación de MOVIL han sido recibidos
            // y procesados satisfactoriamente => Ya puedo "programar" el equipo, es decir
            // grabarlos en la EEPROM
            prgTICKET_OK_F = 0;               // Bajo Bandera
            prgTICKET_RECAUD_OK_F = 0;        // Bajo Bandera
            
            prgREQUEST_F = 1;                 // Pedido de Grabacion de EEPROM
            
            PROG_TICKET_to_EEPROM(0);         // Grabar parametros en EEPROM, sin iniciar EEPROM IRQ
            PROG_TICKET_RECAUD_to_EEPROM(0);  // Grabar parametros en EEPROM, sin iniciar EEPROM IRQ
            
            EEPROM_iniGrabacion();            // Inicio grabacion de EEPROM AL INSTANTE xq es por IRQ
          }
        #endif
        
        
      }else{
        // Error en el N del comando de FIN PROGRAMACION
       // BUZZER_play(RING_error);                  // Reproduzo sonido de error
      }
    }


  /* AVANZAR PUNTERO EN BUFFER DE PROGRAMACION */
  /*********************************************/
    static void PROGRAMADOR_incProgBuffer_ptr (byte** ptr_ptr){
      // Rutina que avanza puntero asociado a buffer de programacion
      (*ptr_ptr)++;
      
      if ((*ptr_ptr) == progBuffer + dim_progBuffer){
        (*ptr_ptr) = progBuffer;
      }
    }


    /* TIME OUT TO RESET */
     /********************/

        void finPRG_TO_RESET (void){

        	if (finPRG_TO_RESET_TO_cnt != 0){
        	  finPRG_TO_RESET_TO_cnt--;
            if (finPRG_TO_RESET_TO_cnt == 0){
            	//NVIC_SystemReset();
            }
          }
        }

        void  JUMPER_PROG_Init(void){
        	GPIO_InitTypeDef gpioinitstruct;

        	JUMPER_PROG_GPIO_CLK_ENABLE();

        	gpioinitstruct.Pin = JUMPER_PROG_PIN;
        	gpioinitstruct.Pull = GPIO_NOPULL;
        	//gpioinitstruct.Pull = GPIO_PULLUP;
        	gpioinitstruct.Speed = GPIO_SPEED_FREQ_HIGH;

        	// Configura pin como entrada con interrupcion externa
        	gpioinitstruct.Mode = GPIO_MODE_IT_FALLING;

        	HAL_GPIO_Init(JUMPER_PROG_PORT, &gpioinitstruct);

        	//habilita y setea EXTI Interrupt a la mas baja prioridad
        	HAL_NVIC_SetPriority((IRQn_Type)(JUMPER_PROG_EXTI_IRQn), 0x02, 0x00);
        	HAL_NVIC_EnableIRQ((IRQn_Type)(JUMPER_PROG_EXTI_IRQn));
        }

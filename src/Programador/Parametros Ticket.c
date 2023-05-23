/* File: <Parametros Ticket.c> */

/* INCLUDES */
/************/
#include "- metroBLUE Config -.h"

#ifdef VISOR_PROG_TICKET
  
  
	#include "Manejo de Buffers.h"
	#include "Grabaciones en EEPROM.h"
	#include "Programacion Parametros.h"
	#include "Timer.h"
	#include "Textos.h"

  
  

/*********************************************************************************************/
/* PROTOTIPOS */
/**************/
  // INTERNAS
  static void prgTICKET_armarDEFAULT (void);
  static void prgTICKET_RECAUD_armarDEFAULT (void);
  
  // EEPROM
  static void armarBuffer_progTICKET_EEPROM (tPRG_TICKET* RAMbuffer);
  static void armarBuffer_progTICKET_RECAUD_EEPROM (byte* RAMbuffer);


/*********************************************************************************************/
/* VARIABLES */
/*************/
  #define TICKET_ID_Leyenda_1       0xC8
  #define TICKET_ID_Leyenda_2       0xC9
  #define TICKET_ID_Leyenda_3       0xCA
  #define TICKET_ID_Localidad       0xCB
  #define TICKET_ID_Servicio        0xCC
  #define TICKET_ID_Titular         0xCD
  #define TICKET_ID_CUIT            0xCE
  #define TICKET_ID_MarcaModelo     0xCF
  #define TICKET_ID_Patente         0xD0
  #define TICKET_ID_Licencia        0xD1
  #define TICKET_ID_IVA             0xD2
  
  static byte* progTICKET_GET;            // Puntero de extraccion de datos de parametros de TICKET TEXTOS
  static byte* progTICKET_RECAUD_GET;     // Puntero de extraccion de datos de parametros de TICKET RECAUD
  
  

/*********************************************************************************************/
/* RUTINAS */
/***********/

  /* LEVANTAR PROGRAMACION DE PARAMETROS DE TICKET */
  /*************************************************/
    void prgTICKET_ini (void){
        byte* eeprom_ptr;
      tEEPROM_ERROR error;
      byte dataEEPROM;

      byte EEPROM_AUX[sizeof(tPRG_TICKET)];


     // eeprom_ptr = &EEPROM_PROG_TICKET;   // Puntero a Inicio de Parametros TICKET
      EEPROM_ReadBuffer(&dataEEPROM, ADDRESS_PROG_TICKET_PAGE1, 1);
      if (dataEEPROM == 0xFF){
			// No hay programacion => programo por defecto
			prgTICKET_armarDEFAULT();
			PROG_TICKET_to_EEPROM(0);                   // Solciitar grabacion parametros en EEPROM, sin iniciar EEPROM IRQ
			error = PROG_TICKET_grabarEEPROM();         // Grabar parametro en EEPROM
			if (error != EEPROM_OK){
			  //BUZZER_play(RING_error);
			}

			prgTICKET_RECAUD_armarDEFAULT();
			PROG_TICKET_RECAUD_to_EEPROM(0);            // Solciitar grabacion parametros en EEPROM, sin iniciar EEPROM IRQ
			error = PROG_TICKET_RECAUD_grabarEEPROM();  // Grabar parametro en EEPROM
			if (error != EEPROM_OK){
			  //BUZZER_play(RING_error);
			}
      }
	 //levanta desde eprom datos de reloj comun
	 //EEPROM_ReadBuffer(&EEPROM_PROG_TICKET,ADDRESS_PROG_TICKET,sizeof(tPRG_TICKET));
	// EEPROM_ReadBuffer(&EEPROM_AUX,ADDRESS_PROG_TICKET,sizeof(tPRG_TICKET));
	 //EEPROM_ReadBuffer(&EEPROM_PROG_TICKET_RECAUD,ADDRESS_PROG_TICKET_RECAUD,sizeof(tPRG_TICKET_RECAUD));
     levantar_progTICKET();

 }


/*********************************************************************************************/
/* INTERACCION CON EL PROGRAMADOR */
/**********************************/
  
  /* LEER PARAMETROS DE TICKET */
  /*****************************/
    void PROG_readTICKET (byte N){
      // Se ejecuta cuando el PROGRAMADOR da la orden de LEER
      tPRG_ERROR error;
      uint8_t buffer_prog[EEPROMsize_PROG_TICKET];
      
      error = PROGRAMADOR_chkN (N, N_lectura);    // Comparo que el N recibido se igual al esperado
      
      if (error == PRG_OK){
    	  read_TICKET_eeprom((uint8_t*)&buffer_prog);
	  	  buffer_prog[EEPROMsize_PROG_TICKET]   = 0xdf;
	  	  buffer_prog[EEPROMsize_PROG_TICKET+1] = 0x0a;

          //PROGRAMADOR_startTx(CMD_LECTURA+0x80, subCMD_TICKETtext, (byte*) &EEPROM_PROG_TICKET);

    	  PROGRAMADOR_startTx(CMD_LECTURA+0x80, subCMD_TICKETtext, (uint8_t*)&buffer_prog);
      }
    }
  

    /* LEER PARAMETROS DE TICKET RECAUDACION */
      /*****************************************/
        void PROG_readTICKET_RECAUD (byte N){
          // Se ejecuta cuando el PROGRAMADOR da la orden de LEER
          tPRG_ERROR error;

          uint8_t buffer_prog[EEPROMsize_PROG_TICKET_RECAUD];

          error = PROGRAMADOR_chkN (N, N_lectura);    // Comparo que el N recibido se igual al esperado

          if (error == PRG_OK){
        	  read_TICKET_RECAUD_eeprom((uint8_t*)&buffer_prog);
        	  PROGRAMADOR_startTx(CMD_LECTURA+0x80, subCMD_TICKETracaud, (byte*) &EEPROM_PROG_TICKET_RECAUD);
          }
        }

  
  /* PROGRAMAR PARAMETROS DE TICKET */
  /**********************************/
 void PROG_writeTICKET (byte N){
      // Se ejecuta cuando el PROGRAMADOR da la orden de PROGRAMAR
      tPRG_ERROR error;

      if (PROGRAMADOR_chkJumper()){

    error = PRG_OK;                             // Asumo sin error
      
      progTICKET_GET = PROGRAMADOR_addRxData(&BAX_SCI_PROG);
      (void)PROGRAMADOR_addByte(finEEPROM_H);     // Para poder delimitar el buffer y saber cuando parar en armarBuffer_EEPROM
      if (progTICKET_GET == NULL){
        error = PRG_NULL_POINTER;
      }
      
      prgTICKET_OK_F = 0;                         // Asumo Parametro Erroneo
      
      if (error == PRG_OK){
        // Solo respondo si no hay errores, xq sino voy a grabar cualquiera en el EEPROM
        PROGRAMADOR_startTxRTA(subCMD_TICKETtext);// Envio la Rta hacia el programador
        prgTICKET_OK_F = 1;                       // Parametro Correcto y Listo para ser Programado al finalizar Proceso
      
      }else{
        // Error al intentar programar los parametros
       // BUZZER_play(RING_error);                  // Reproduzo sonido de error
      }
    }else{
          // No tiene el jumper de programacion
          error = PRG_NO_JUMPER;
          PROGRAMADOR_TxNoJumper();                   // Notifico al programador
    }
 }


  
  
 /* PROGRAMAR PARAMETROS DE TICKET RECAUDACION*/
  /*********************************************/
  void PROG_writeTICKET_RECAUD (byte N){
      // Se ejecuta cuando el PROGRAMADOR da la orden de PROGRAMAR
      tPRG_ERROR error;
      
    if (PROGRAMADOR_chkJumper()){

      error = PROGRAMADOR_chkN (N, N_TICKETrecaud);      // Comparo que el N recibido se igual al esperado
      
      if (error == PRG_OK){
        progTICKET_RECAUD_GET = PROGRAMADOR_addRxData(&BAX_SCI_PROG);
        if (progTICKET_RECAUD_GET == NULL){
          error = PRG_NULL_POINTER;
        }
      }
      
      prgTICKET_RECAUD_OK_F = 0;                    // Asumo Parametro Erroneo
      
      if (error == PRG_OK){
        // Solo respondo si no hay errores, xq sino voy a grabar cualquiera en el EEPROM
        PROGRAMADOR_startTxRTA(subCMD_TICKETracaud);// Envio la Rta hacia el programador
        prgTICKET_RECAUD_OK_F = 1;                  // Parametro Correcto y Listo para ser Programado al finalizar Proceso
      
      }else{
        // Error al intentar programar los parametros
       // BUZZER_play(RING_error);                    // Reproduzo sonido de error
      }
    }else{
                // No tiene el jumper de programacion
                error = PRG_NO_JUMPER;
                PROGRAMADOR_TxNoJumper();                   // Notifico al programador
    }

 }


/*********************************************************************************************/
/* RUTINAS INTERNAS */
/********************/

  /* ARMAR STRING DE PARAMETROS TICKET */
  /*************************************/
    static void prgTICKET_armarDEFAULT (void){
      // Los parametros de ticket se grabaran en EEPROM con el siguiente formato:
      // (recordemos que la EEPROM se divide en bloques de 1 longword, es decir
      //  4bytes)
      //
      //  +----|----|----|----++----|----|----|----++----|----|----|----++----|----|----|----+
      //  |LEY1|LEY2|LEY3|LOCA||SERV|TITU|CUIT|MODE||PATE|LICE| IVA| DF || 0A |              |
      //  +----|----|----|----++----|----|----|----++----|----|----|----++----|----|----|----+
      //
      // Solo tiene sentido guardar el puntero GET la primera vez que agrego un
      // dato, asi cuando extraigo los mismo para grabarlos en el EEPROM, los
      // grabo desde el primero
      byte len;
      byte i;
      byte txt[25];
      
      // LEYENDA 1
      progTICKET_GET = PROGRAMADOR_addByte(TICKET_ID_Leyenda_1);
      (void)PROGRAMADOR_addByte(TICKET_fontNormal);
      (void)PROGRAMADOR_addByte('P');
      (void)PROGRAMADOR_addByte('R');
      (void)PROGRAMADOR_addByte('O');
      (void)PROGRAMADOR_addByte('G');
      (void)PROGRAMADOR_addByte('R');
      (void)PROGRAMADOR_addByte('A');
      (void)PROGRAMADOR_addByte('M');
      (void)PROGRAMADOR_addByte('A');
      (void)PROGRAMADOR_addByte('C');
      (void)PROGRAMADOR_addByte('I');
      (void)PROGRAMADOR_addByte('O');
      (void)PROGRAMADOR_addByte('N');

      
      // LEYENDA 2
      (void)PROGRAMADOR_addByte(TICKET_ID_Leyenda_2);
      (void)PROGRAMADOR_addByte(TICKET_fontNormal);
      (void)PROGRAMADOR_addByte('P');
      (void)PROGRAMADOR_addByte('O');
      (void)PROGRAMADOR_addByte('R');
      (void)PROGRAMADOR_addByte(' ');
      (void)PROGRAMADOR_addByte('D');
      (void)PROGRAMADOR_addByte('E');
      (void)PROGRAMADOR_addByte('F');
      (void)PROGRAMADOR_addByte('E');
      (void)PROGRAMADOR_addByte('C');
      (void)PROGRAMADOR_addByte('T');
      (void)PROGRAMADOR_addByte('O');
      
      // LEYENDA 3
      (void)PROGRAMADOR_addByte(TICKET_ID_Leyenda_3);
      (void)PROGRAMADOR_addByte(TICKET_fontNormal);
      (void)PROGRAMADOR_addByte(0);
      
      // LOCALIDAD
      (void)PROGRAMADOR_addByte(TICKET_ID_Localidad);
      (void)PROGRAMADOR_addByte(TICKET_fontNormal);
      /*
      string_FAR_copy(txt, _Localidad_Desconocida);
      len = string_length(txt);
      i = 0;
      while (i < len){
        (void)PROGRAMADOR_addByte(txt[i]);
        i++;
      }
      */
      (void)PROGRAMADOR_addByte(0);
      
      // SERVICIO
      (void)PROGRAMADOR_addByte(TICKET_ID_Servicio);
      (void)PROGRAMADOR_addByte(TICKET_fontNormal);
      /*
      string_FAR_copy(txt, _Tipo_de_Servicio_Desconocido);
      len = string_length(txt);
      i = 0;
      while (i < len){
        (void)PROGRAMADOR_addByte(txt[i]);
        i++;
      }
      */
      (void)PROGRAMADOR_addByte(0);
      
      // TITULAR
      (void)PROGRAMADOR_addByte(TICKET_ID_Titular);
      (void)PROGRAMADOR_addByte(TICKET_fontNormal);
      /*
      string_FAR_copy(txt, _Titular_Desconocido);
      len = string_length(txt);
      i = 0;
      while (i < len){
        (void)PROGRAMADOR_addByte(txt[i]);
        i++;
      }
      */
      (void)PROGRAMADOR_addByte(0);
      
      // CUIT
      (void)PROGRAMADOR_addByte(TICKET_ID_CUIT);
      (void)PROGRAMADOR_addByte(TICKET_fontNormal);
      /*
      string_FAR_copy(txt, _CUIT_Desconocido);
      len = string_length(txt);
      i = 0;
      while (i < len){
        (void)PROGRAMADOR_addByte(txt[i]);
        i++;
      }
      */
      (void)PROGRAMADOR_addByte(0);
            
      // MARCA MODELO
      (void)PROGRAMADOR_addByte(TICKET_ID_MarcaModelo);
      (void)PROGRAMADOR_addByte(TICKET_fontNormal);
      /*
      string_FAR_copy(txt, _Coche_Desconocido);
      len = string_length(txt);
      i = 0;
      while (i < len){
        (void)PROGRAMADOR_addByte(txt[i]);
        i++;
      }
      */
      (void)PROGRAMADOR_addByte(0);
            
      // PATENTE
      (void)PROGRAMADOR_addByte(TICKET_ID_Patente);
      (void)PROGRAMADOR_addByte(TICKET_fontNormal);
      /*
      string_FAR_copy(txt, _Patente_Desconocida);
      len = string_length(txt);
      i = 0;
      while (i < len){
        (void)PROGRAMADOR_addByte(txt[i]);
        i++;
      }
      */
      (void)PROGRAMADOR_addByte(0);
            
      // LICENCIA
      (void)PROGRAMADOR_addByte(TICKET_ID_Licencia);
      (void)PROGRAMADOR_addByte(TICKET_fontNormal);
      /*
      string_FAR_copy(txt, _Licencia_Desconocida);
      len = string_length(txt);
      i = 0;
      while (i < len){
        (void)PROGRAMADOR_addByte(txt[i]);
        i++;
      }
      */
      (void)PROGRAMADOR_addByte(0);
            
      // IVA
      (void)PROGRAMADOR_addByte(TICKET_ID_IVA);
      (void)PROGRAMADOR_addByte(TICKET_fontNormal);
      (void)PROGRAMADOR_addByte(0);
      
      (void)PROGRAMADOR_addByte(finEEPROM_H);     // Fin de Datos
      (void)PROGRAMADOR_addByte(finEEPROM_L);
    }
    
    
  /* ARMAR STRING DE RECAUDACION TICKET */
  /**************************************/
    static void prgTICKET_RECAUD_armarDEFAULT (void){
      // La recaudacion de ticket se grabaran en EEPROM con el siguiente formato:
      // (recordemos que la EEPROM se divide en bloques de 1 longword, es decir
      //  4bytes)
      //
      //  %CHOFER
      //
      // Solo tiene sentido guardar el puntero GET la primera vez que agrego un
      // dato, asi cuando extraigo los mismo para grabarlos en el EEPROM, los
      // grabo desde el primero
      progTICKET_RECAUD_GET = PROGRAMADOR_addByte(0x00);
      
      
      (void)PROGRAMADOR_addByte(finEEPROM_H);     // Fin de Datos
      (void)PROGRAMADOR_addByte(finEEPROM_L);
    }


/*********************************************************************************************/
/* RUTINAS DE EEPROM - TEXTOS TICKET */
/*************************************/

  /* INICIAR GRABACION DE PARAMETROS TICKET EN EEPROM */
  /****************************************************/
    void PROG_TICKET_to_EEPROM (byte iniGrabacion){
      // Da la orden de grabacion en EEPROM de los parametros de TICKET
      prgTICKET_EEPROM_F = 1;           // Levanto bandera de grabacion de programacion de parametros de ticket en eeprom
    }


  /* GRABACION DE PARAMETROS TICKET EN EEPROM */
  /********************************************/
    tEEPROM_ERROR PROG_TICKET_grabarEEPROM (void){
      tEEPROM_ERROR error;



      dword* EEPROM_ptr;
      dword* EEPROM_ptr_AUX;
      byte* EEPROM_buffer_ptr_aux;

      //tPRG_TICKET EEPROM_buffer;
      byte EEPROM_buffer[EEPROMsize_PROG_TICKET];
      
      error = EEPROM_OK;                // Asumo que no hay error
      if (prgTICKET_EEPROM_F){
        prgTICKET_EEPROM_F = 0;          // Bajo Bandera
        
        //EEPROM_ptr = ADDRESS_PROG_TICKET;
        //EEPROM_ptr_AUX = ADDRESS_PROG_TICKET+88;
        //EEPROM_buffer_ptr_aux = &EEPROM_buffer;
        //EEPROM_buffer_ptr_aux += 88;

        armarBuffer_progTICKET_EEPROM(&EEPROM_buffer);  // Armo buffer de grabación según formato
        
        EEPROM_ptr = ADDRESS_PROG_TICKET_PAGE1;
        error = grabar_buffer_EEPROM_TICKET((uint16_t*) EEPROM_buffer, (uint16_t*) EEPROM_ptr, 128);

        EEPROM_ptr = ADDRESS_PROG_TICKET_PAGE2;
        EEPROM_buffer_ptr_aux = &EEPROM_buffer;
        EEPROM_buffer_ptr_aux += 128;
        error = grabar_buffer_EEPROM_TICKET((uint16_t*) EEPROM_buffer_ptr_aux, (uint16_t*) EEPROM_ptr, SIZE_PROG_TICKET-128);


		//PRUEBA
		//uint8_t buffer_prog[EEPROMsize_PROG_TICKET];
		//read_TICKET_eeprom((uint8_t*)&buffer_prog);


      }
      
      return(error);
    }    
 
 
 /* ARMAR BUFFER DE GRABACION DE PARAMETROS TICKET */
  /*************************************************/
    static void armarBuffer_progTICKET_EEPROM (tPRG_TICKET* RAMbuffer){
      // Los parametros demovil se grabaran en EEPROM con el siguiente formato:
      // (recordemos que la EEPROM se divide en bloques de 1 longword, es decir
      //  4bytes)
      byte* PUTptr;
      byte** GETptr_ptr;
      byte TO_F;
      byte dato;
      byte N;
      
      GETptr_ptr = &progTICKET_GET;                                 // Puntero a inicio de datos de TICKET
      
      PUTptr = (byte*)RAMbuffer;
      for (N=0; N<sizeof(tPRG_TICKET); N++){
        *PUTptr++= 0;
      }
   
      TO_F = 0;                                                     // Limpio Bandera
      PUTptr = NULL;                                                // Reseteo Puntero
      dato = 0;                                                     // Reseteo variable
      N = EEPROMsize_PRG_TICKET - 2;                                // Guardo 2 para DF - 0A
      dispararTO_lazo();                                            // Disparo TimeOut Lazo
      while ((dato != finEEPROM_H) && (N > 0) && (!TO_F)){
        TO_F = chkTO_lazo_F();                                      // Chequeo bandera de time out de lazo 
        N--;                                                        // Decremento longitud de datos
        dato = PROGRAMADOR_getProgData(GETptr_ptr);                 // Extraigo dato y avanzo puntero GET
        if ((dato >= TICKET_ID_Leyenda_1) && (dato <= TICKET_ID_IVA)){
          // Cambiar de "campo". Antes del cambio agrego fin de 
          // cadena en el anterior; siempre y cuando hay anterior
          if (PUTptr != NULL){
            *PUTptr++ = 0;                                          // Agrego fin de cadena
          }
          
          if(dato == TICKET_ID_Leyenda_1){
            PUTptr = RAMbuffer->LEYENDA_1;
          }else if(dato == TICKET_ID_Leyenda_2){
            PUTptr = RAMbuffer->LEYENDA_2;
          }else if(dato == TICKET_ID_Leyenda_3){
            PUTptr = RAMbuffer->LEYENDA_3;
          }else if(dato == TICKET_ID_Localidad){
            PUTptr = RAMbuffer->LOCALIDAD;
          }else if(dato == TICKET_ID_Servicio){
            PUTptr = RAMbuffer->SERVICIO;
          }else if(dato == TICKET_ID_Titular){
            PUTptr = RAMbuffer->TITULAR;
          }else if(dato == TICKET_ID_CUIT){
            PUTptr = RAMbuffer->CUIT;
          }else if(dato == TICKET_ID_MarcaModelo){
            PUTptr = RAMbuffer->MARCA_MODELO;
          }else if(dato == TICKET_ID_Patente){
            PUTptr = RAMbuffer->PATENTE;
          }else if(dato == TICKET_ID_Licencia){
            PUTptr = RAMbuffer->LICENCIA;
          }else if(dato == TICKET_ID_IVA){
            PUTptr = RAMbuffer->IVA;
          }
          
          *PUTptr++ = dato;                                         // Agrego ID de campo
        
        }else{
          *PUTptr++ = dato;                                         // Agrego dato
        }
      }
      detenerTO_lazo();                                             // Detener TimeOut Lazo
      
      if (TO_F){
       // BUZZER_play(RING_error);
      }else{
        // Si no ocurrio TimeOut, como hago el while hasta el finH N==, falta agregar el finL y quizas el finH

    	  if((dato != finEEPROM_H)){
    	   //correccion rai 30/08/2018
    	   *PUTptr++ = finEEPROM_H;                                 // Agrego fin de datos High
    	  }
    	 *PUTptr++ = finEEPROM_L;                                    // Agrego fin de datos LOW
      }
    }
    
/*********************************************************************************************/
/* RUTINAS DE EEPROM - RECAUDACION TICKET */
/******************************************/

  /* INICIAR GRABACION DE RECAUDACION TICKET EN EEPROM */
  /*****************************************************/
    void PROG_TICKET_RECAUD_to_EEPROM (byte iniGrabacion){
      // Da la orden de grabacion en EEPROM de los parametros de TICKET
      prgTICKET_RECAUD_EEPROM_F = 1;    // Levanto bandera de grabacion de programacion de parametros de ticket en eeprom
      
    }


  /* GRABACION DE RECAUDACION TICKET EN EEPROM */
  /*********************************************/
    tEEPROM_ERROR PROG_TICKET_RECAUD_grabarEEPROM (void){
      tEEPROM_ERROR error;
        dword* EEPROM_ptr;
      byte EEPROM_buffer[sizeof(tPRG_TICKET_RECAUD)];
      
      error = EEPROM_OK;                // Asumo que no hay error
      if (prgTICKET_RECAUD_EEPROM_F){
        prgTICKET_RECAUD_EEPROM_F = 0;  // Bajo Bandera
        
        //EEPROM_ptr = &EEPROM_PROG_TICKET_RECAUD;
        EEPROM_ptr = ADDRESS_PROG_TICKET_RECAUD;

        
        armarBuffer_progTICKET_RECAUD_EEPROM(&EEPROM_buffer);  // Armo buffer de grabación según formato
        
        error = grabar_buffer_EEPROM((uint16_t*) EEPROM_buffer, (uint16_t*) EEPROM_ptr, SIZE_PROG_TICKET_RECAUD);
      }
      
      return(error);
    }    
 
 
 /* ARMAR BUFFER DE GRABACION DE RECAUDACION TICKET */
  /**************************************************/
    static void armarBuffer_progTICKET_RECAUD_EEPROM (byte* RAMbuffer){
      // Los parametros demovil se grabaran en EEPROM con el siguiente formato:
      // (recordemos que la EEPROM se divide en bloques de 1 longword, es decir
      //  4bytes)
      byte* PUTptr;
      byte** GETptr_ptr;
      
      PUTptr = RAMbuffer;                               // Ingreso datos desde inicio de Buffer
      GETptr_ptr = &progTICKET_RECAUD_GET;              // Puntero a inicio de datos de RECAUDACION
      *PUTptr++ = PROGRAMADOR_getProgData(GETptr_ptr);
      
      *PUTptr++ = finEEPROM_H;          // Fin de Datos
      *PUTptr++ = finEEPROM_L;
    }


#endif

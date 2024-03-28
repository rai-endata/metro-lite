/* File: <Parametros Movil.c> */


/* INCLUDES */

/************/
	#include "Parametros Movil.h"
	#include "Programacion Parametros.h"
	#include "Parametros Reloj.h"
	#include "eeprom.h"
	#include "Reloj.h"
  


/*********************************************************************************************/
/* PROTOTIPOS */
/**************/

  // EEPROM
  static void armarBuffer_progMOVIL_EEPROM (byte* RAMbuffer);
  

/*********************************************************************************************/
/* VARIABLES */
/*************/
  tPARAM_MOVIL EEPROM_PROG_MOVIL;

  tPARAM_RADIO EEPROM_PROG_RADIO;

  byte LOGICA_BANDERITA;                // No es un parametro de programacion, pero se deduce de el
  
  static byte* progMOVIL_GET;           // Puntero de extraccion de datos de parametros de MOVIL
  
/*********************************************************************************************/
/* RUTINAS */
/***********/

  /* INICIALIZACION DE PARAMETROS DE MOVIL */
  /*****************************************/
    void prgMOVIL_ini (void){
        byte* eeprom_ptr;
      tEEPROM_ERROR error;
      
      //parche hasta resolver mejor problema de error crc en programacion de movil
      //no lo mira al crc hasta resolverlo mejor
      //error = chkCRC_EnEEPROM(ADDRESS_PROG_MOVIL, SIZE_PROG_MOVIL);
      error = EEPROM_OK;

    	if(error == EEPROM_OK){
      		EEPROM_ReadBuffer(&EEPROM_PROG_MOVIL,ADDRESS_PROG_MOVIL,sizeof(tPARAM_MOVIL));
            // Inicializo logica de banderita
            if (TIPO_RELOJ == BANDERITA_LIBRE_ALTO){
              LOGICA_BANDERITA = LIBRE_enALTO;
            }else if (TIPO_RELOJ == BANDERITA_OCUPADO_ALTO){
              LOGICA_BANDERITA = OCUPADO_enALTO;
            }

            if (!TIPO_RELOJ_VALIDO){
            	//error_eepromDATA=1;
            	//prog_mode=1;
            }

      	}else{
      		error_eepromDATA=1;
      		prog_mode=1;
      	}
    }


/*********************************************************************************************/
/* INTERACCION CON EL PROGRAMADOR */
/**********************************/
  
  /* LEER PARAMETROS DE MOVIL */
  /****************************/
    void PROG_readMOVIL (uint8_t N){
      // Se ejecuta cuando el PROGRAMADOR da la orden de LEER

      tPRG_ERROR error;
      uint8_t buffer_prog[SIZE_PROG_MOVIL+2];

      error = PROGRAMADOR_chkN (N, N_lectura);    // Comparo que el N recibido se igual al esperado

      if (error == PRG_OK){
	  	  read_MOVIL_eeprom((uint8_t*)&buffer_prog);
	  	  buffer_prog[SIZE_PROG_MOVIL]   = 0xdf;
	  	  buffer_prog[SIZE_PROG_MOVIL+1] = 0x0a;
          PROGRAMADOR_startTx(CMD_LECTURA+0x80, subCMD_MOVIL, (uint8_t*)&buffer_prog);
      }

    }
  

  /* PROGRAMAR PARAMETROS DE MOVIL */
  /*********************************/
    void PROG_writeMOVIL (uint8_t N){
      // Se ejecuta cuando el PROGRAMADOR da la orden de PROGRAMAR
      tPRG_ERROR error;
      
      
      error = PROGRAMADOR_chkN (N, N_MOVIL); // Comparo que el N recibido se igual al esperado
      if (PROGRAMADOR_chkJumper()){
        // Tiene el jumper o no es necesario chequearlo
        if (error == PRG_OK){
        	progMOVIL_GET = PROGRAMADOR_addRxData(&BAX_SCI_PROG);
          if (progMOVIL_GET == NULL){
            error = PRG_NULL_POINTER;
          }
        }
        prgMOVIL_OK_F = 0;                          // Asumo Parametro Erroneo
        if (error == PRG_OK){
          // Solo respondo si no hay errores, xq sino voy a grabar cualquiera en el EEPROM
          PROGRAMADOR_startTxRTA(subCMD_MOVIL);     // Envio la Rta hacia el programador
          prgMOVIL_OK_F = 1;                        // Parametro Correcto y Listo para ser Programado al finalizar Proceso
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

    /* LEER PARAMETROS DE RADIO */
    /****************************/

    void PROG_readRADIO (uint8_t N){
        // Se ejecuta cuando el PROGRAMADOR da la orden de LEER
        tPRG_ERROR error;

        error = PROGRAMADOR_chkN (N, N_lectura);    // Comparo que el N recibido se igual al esperado

        if (error == PRG_OK){
        	EEPROM_PROG_RADIO.CTCSS_TxLevel = 10;
        	EEPROM_PROG_RADIO.chDS 			= 10;
        	EEPROM_PROG_RADIO.chDatos		= 1;
        	EEPROM_PROG_RADIO.chVoz   		= 2;
        	EEPROM_PROG_RADIO.delayPTT      = 10;
        	EEPROM_PROG_RADIO.micLevel		= 7;
        	EEPROM_PROG_RADIO.subToneRx		= 1;
        	EEPROM_PROG_RADIO.subToneTx		= 2;
        	EEPROM_PROG_RADIO.tVoz			= 15;
        	EEPROM_PROG_RADIO.tipoRadio		= 2;
        	//EEPROM_PROG_RADIO.varios.Bits_		= 1;
        	EEPROM_PROG_RADIO.fin[0]		= 0xdf;
        	EEPROM_PROG_RADIO.fin[1]		= 0x0a;


            PROGRAMADOR_startTx(CMD_LECTURA+0x80, subCMD_RADIO, (uint8_t*)&EEPROM_PROG_RADIO);
        }
      }



    /* PROGRAMAR PARAMETROS DE RADIO */
    /*********************************/
      void PROG_writeRADIO (uint8_t N){
        // Se ejecuta cuando el PROGRAMADOR da la orden de PROGRAMAR
        tPRG_ERROR error;

          // Solo respondo si no hay errores, xq sino voy a grabar cualquiera en el EEPROM
          PROGRAMADOR_startTxRTA(subCMD_RADIO);     // Envio la Rta hacia el programador
          prgRADIO_OK_F = 1;                        // Parametro Correcto y Listo para ser Programado al finalizar Proceso

      }
/*********************************************************************************************/
/* RUTINAS INTERNAS */
/********************/

  /* ARMAR STRING DE PARAMETROS MOVIL */
  /************************************/
    void prgMOVIL_armarDEFAULT (void){
      // Los parametros demovil se grabaran en EEPROM con el siguiente formato:
      // (recordemos que la EEPROM se divide en bloques de 1 longword, es decir         
      //  4bytes)
      //
      //  +----|----|----|----++----|----|----|----+
      //  | Nro.Mov | Rx | Tx ||tRLJ|	DF | 0A | xx |
      //  +----|----|----|----++----|----|----|----+
      //
      // Solo tiene sentido guardar el puntero GET la primera vez que agrego un
      // dato, asi cuando extraigo los mismo para grabarlos en el EEPROM, los
      // grabo desde el primero
       progMOVIL_GET = PROGRAMADOR_addByte(0x00);  // Numero de movil por defecto (H)
       (void)PROGRAMADOR_addByte(0x01);            // Numero de movil por defecto (L)
       (void)PROGRAMADOR_addByte(0x78);            // Grupo de Rx por defecto
       (void)PROGRAMADOR_addByte(0x12);            // Grupo de Tx por defecto
       (void)PROGRAMADOR_addByte(INTERNO);         // Tipo Reloj por defecto
      
       (void)PROGRAMADOR_addByte(finEEPROM_H);     // Fin de Datos
       (void)PROGRAMADOR_addByte(finEEPROM_L);
    }


    void prgMOVIL_armar (byte* nroMOVIL){
      // Los parametros demovil se grabaran en EEPROM con el siguiente formato:
      // (recordemos que la EEPROM se divide en bloques de 1 longword, es decir
      //  4bytes)
      //
      //  +----|----|----|----++----|----|----|----+
      //  | Nro.Mov | Rx | Tx ||tRLJ|	DF | 0A | xx |
      //  +----|----|----|----++----|----|----|----+
      //
      // Solo tiene sentido guardar el puntero GET la primera vez que agrego un
      // dato, asi cuando extraigo los mismo para grabarlos en el EEPROM, los
      // grabo desde el primero
      progMOVIL_GET = PROGRAMADOR_addByte(*nroMOVIL++);  // Numero de movil por defecto (H)
      (void)PROGRAMADOR_addByte(*nroMOVIL);            // Numero de movil por defecto (L)
      (void)PROGRAMADOR_addByte(0x78);            // Grupo de Rx por defecto
      (void)PROGRAMADOR_addByte(0x12);            // Grupo de Tx por defecto
      (void)PROGRAMADOR_addByte(INTERNO);         // Tipo Reloj por defecto
      
      (void)PROGRAMADOR_addByte(finEEPROM_H);     // Fin de Datos
      (void)PROGRAMADOR_addByte(finEEPROM_L);
    }
	

/*********************************************************************************************/
/* RUTINAS DE EEPROM - PARAMETROS MOVIL */
/****************************************/

  /* INICIAR GRABACION DE PARAMETROS MOVIL EN EEPROM */
  /***************************************************/
    void PROG_MOVIL_to_EEPROM (byte iniGrabacion){
      // Da la orden de grabacion en EEPROM de los parametros de MOVIL
      prgMOVIL_EEPROM_F = 1;            // Levanto bandera de grabacion de programacion de parametros de movil en eeprom
      
      if (iniGrabacion){
        EEPROM_iniGrabacion();          // Inicio grabacion de EEPROM AL INSTANTE xq es por IRQ
      }
    }


  /* GRABACION DE PARAMETROS MOVIL EN EEPROM */
  /*******************************************/
    tEEPROM_ERROR PROG_MOVIL_grabarEEPROM (void){
      
      tEEPROM_ERROR error;
      dword* EEPROM_ptr;
      uint8_t* ptrAUX;
      uint16_t valorCRC;
      byte EEPROM_buffer[EEPROMsize_PRG_MOVIL];
      uint16_t long_MOVIL_DATA;

      //error = EEPROM_OK;                // Asumo que no hay error
      error = EEPROM_ERROR_MASK;                //si no entra en el if el nro de tarifa no esta programada o hay algun error
      if (prgMOVIL_EEPROM_F){
         prgMOVIL_EEPROM_F = 0;          // Bajo Bandera



        armarBuffer_progMOVIL_EEPROM(EEPROM_buffer);  // Armo buffer de grabación según formato

        //calculo de crc de datos recibidos
		long_MOVIL_DATA = EEPROMsize_PRG_MOVIL - (sizeof(((tPARAM_MOVIL *)0)->checksum) + sizeof(((tPARAM_MOVIL *)0)->finDATA));
		valorCRC = GetCrc16(&EEPROM_buffer, long_MOVIL_DATA );

		//pongo crc en buffer de grabacion
		ptrAUX = &valorCRC;
		EEPROM_buffer[EEPROMsize_PRG_MOVIL-4] = *(ptrAUX+1);
		EEPROM_buffer[EEPROMsize_PRG_MOVIL-3] = *(ptrAUX+0);
		EEPROM_buffer[EEPROMsize_PRG_MOVIL-2] = finEEPROM_H;              // Fin de Datos
		EEPROM_buffer[EEPROMsize_PRG_MOVIL-1] = finEEPROM_L;


        error = grabar_buffer_EEPROM((uint16_t*) EEPROM_buffer, ADDRESS_PROG_MOVIL, SIZE_PROG_MOVIL);
        if(error == EEPROM_OK){
        	error = chkCRC_EnEEPROM(ADDRESS_PROG_MOVIL, EEPROMsize_PRG_MOVIL);
        }

        if(error == EEPROM_OK){
				//este es un parche por un tema del programador
				//cuando el programador programa la eeprom solo guardo datos en el primer sector
				//porque si guardo en todos los sectores, en el programador salta un mensaje de error de grabacion por timeout
				//los demas sectores los guardo cuando se reinicia el metrolite
				//para asegurarme guarde los nuevos datos guardados guardo 0xff en la cabecera de cada sector para que de error
				// de checksum al del sector y copie los datos programados, y no se quede con la programacion anterior
				//en el chequeo de arranque
				EEPROM_buffer[0] = 0xff;
				EEPROM_buffer[1] = 0xff;
				EEPROM_buffer[2] = finEEPROM_H;              // Fin de Datos
				EEPROM_buffer[3] = finEEPROM_L;
				grabar_buffer_EEPROM((uint16_t*)EEPROM_buffer, ADDRESS_PROG_MOVIL_bck1, 4);
				grabar_buffer_EEPROM((uint16_t*)EEPROM_buffer, ADDRESS_PROG_MOVIL_bck2, 4);
				grabar_buffer_EEPROM((uint16_t*)EEPROM_buffer, ADDRESS_PROG_MOVIL_bck3, 4);
        }
/*
        		if(error == EEPROM_OK){
        			error = grabar_buffer_EEPROM((uint16_t*) EEPROM_buffer, ADDRESS_PROG_MOVIL_bck2, SIZE_PROG_MOVIL);
                    if(error == EEPROM_OK){
                     	error = chkCRC_EnEEPROM(ADDRESS_PROG_MOVIL_bck2, EEPROMsize_PRG_MOVIL);
                     }

        		}

        		if(error == EEPROM_OK){
        			 error = grabar_buffer_EEPROM((uint16_t*) EEPROM_buffer, ADDRESS_PROG_MOVIL_bck3, SIZE_PROG_MOVIL);
                     if(error == EEPROM_OK){
                      	error = chkCRC_EnEEPROM(ADDRESS_PROG_MOVIL_bck3, EEPROMsize_PRG_MOVIL);
                      }

        		}

        		if(error == EEPROM_OK){
        			EEPROM_ReadBuffer(&EEPROM_PROG_MOVIL, ADDRESS_PROG_MOVIL, sizeof(tPARAM_MOVIL));
        		}
*/
       }else{
          	  error = EEPROM_OK;
            }
      return(error);
    }    
    
    
  /* ARMAR BUFFER DE GRABACION DE PARAMETROS MOVIL */
  /*************************************************/
    static void armarBuffer_progMOVIL_EEPROM (byte* RAMbuffer){
      // Los parametros demovil se grabaran en EEPROM con el siguiente formato:
      // (recordemos que la EEPROM se divide en bloques de 1 longword, es decir
      //  4bytes)
      //
      //  +----|----|----|----++----|----|----|----+
      //  | Nro.Mov | Rx | Tx ||tRLJ|	DF | 0A | xx |
      //  +----|----|----|----++----|----|----|----+
      byte* PUTptr;
      byte** GETptr_ptr;
      
      PUTptr = RAMbuffer;                               // Ingreso datos desde inicio de Buffer
      GETptr_ptr = &progMOVIL_GET;                      // Puntero a inicio de datos de MOVIL
      *PUTptr++ = PROGRAMADOR_getProgData(GETptr_ptr);  // Extraigo MOVH y avanzo puntero GET
      *PUTptr++ = PROGRAMADOR_getProgData(GETptr_ptr);  // Extraigo MOVL y avanzo puntero GET
      *PUTptr++ = PROGRAMADOR_getProgData(GETptr_ptr);  // Extraigo Address Rx y avanzo puntero GET
      *PUTptr++ = PROGRAMADOR_getProgData(GETptr_ptr);  // Extraigo Address Tx y avanzo puntero GET
      *PUTptr++ = PROGRAMADOR_getProgData(GETptr_ptr);  // Extraigo Tipo Reloj y avanzo puntero GET

      *PUTptr++ = finEEPROM_H;          // Fin de Datos
      *PUTptr++ = finEEPROM_L;
    }


    void read_MOVIL_eeprom(uint8_t* buffer_backup){

    	uint8_t* ptrDIR;
    	uint8_t i=0;

    	uint32_t addr;
    	uint32_t address_eeprom;

    	uint32_t size;
    	size = 88;

    	//address_eeprom = ADDRESS_PROG_MOVIL;
    	getDirProgOk();
    	address_eeprom = addressMovil;

    		//me fijo si hubo corte de alimentacion
    	EEPROM_ReadBuffer(buffer_backup,address_eeprom,SIZE_PROG_MOVIL);

    	//size = SIZE_PROG_MOVIL-88;
    	//address_eeprom = ADDRESS_PROG_MOVIL+88;
    	//me fijo si hubo corte de alimentacion
       	//EEPROM_ReadBuffer(buffer_backup,address_eeprom,SIZE_PROG_MOVIL);


    }

/*********************************************************************************************/
/* PROGRAMACION POR AIRE */
/*************************/
  /* GUARDAR DATOS DE AIRE A PROGRAMAR */
  /*************************************/
    void PROG_saveMOVIL_air (byte* data, byte N){
      // Rutina que guarda los datos para luego grabar la EEPROM. Utilizado en
      // la actualizacion por AIRE
      
      progMOVIL_GET = PROGRAMADOR_addByte(*data++);
      N--;
      while (N > 0){
        N--;
        (void)PROGRAMADOR_addByte(*data++);
      }
    }
  

  /* PROGRAMAR PARAMAETROS DE MOVIL POR AIRE */
  /*******************************************/
    byte PROG_writeMOVIL_air (byte N){
      // Programar parametros de MOVIL desde AIRE, graba los paramteros que
      // fueron guardados y hace un chequeo del N.
      // Si los datos no son correctos, indica esta situacion
      byte error;
      
      if (N != N_MOVIL){
        error = 1;
      
      }else{
        error = 0;
        PROG_MOVIL_to_EEPROM(1);
      }
      
      return (error);
    }    

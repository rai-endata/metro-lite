/* File: <Parametros Reloj.c> */


/* INCLUDES */
/************/
  
  #include "Calculos.h"
  #include "Manejo de Buffers.h"
  #include "Programacion Parametros.h"
  #include "Parametros Reloj.h"
  #include "Parametros Movil.h"
  #include "Tarifacion Reloj.h"
  #include "RTC.h"
  #include "Timer.h"
  #include "eeprom_aux.h"
  #include "eeprom.h"
  #include "eeprom_e.h"
  //prueba rivera
  #include "Odometro.h"
  #include "Flash.h"
  #include "Reloj.h"
  #include "Crc.h"
  #include "buzzer.h"
  #include "Grabaciones en EEPROM.h"
  #include "inicio.h"
  #include "Air Update.h"
  #include "DTE - Tx.h"


  //#include "Air Update.h"

    // EDIT 28/03/2013
  // A partir de ahora, en este archivo van a estar TODAS las variables de EEPROM
  // ordenadas de la misma manera en la que estan hasta el dia de hoy en el .MAP
  // De este modo puedo asegurar que en el futuro cada variable va a permanecer
  // siempre en el mismo lugar (posicion de memoria) y asi no va a haber problemas
  // al ahcer un upgrade/downgrade con el BOOTLOADER.
  // Recordemos que el bootloader no modifica la EEPROM, por este motivo si una version
  // tiene los parametros de reloj en un lugar y otra los busca en otro lugar, se puede
  // armar un lio barbaro
   byte EEPROM_HORA_APAGADO[EEPROMsize_HORA_APAGADO];
  // tPARAM_prueba pepe;
   tPARAM_RELOJ_COMUNES EEPROM_PROG_relojCOMUN;
   tPARAM_RELOJ_TARIFA EEPROM_PROG_relojT1D;
   tPARAM_RELOJ_TARIFA EEPROM_PROG_relojT2D;
   tPARAM_RELOJ_TARIFA EEPROM_PROG_relojT3D;
   tPARAM_RELOJ_TARIFA EEPROM_PROG_relojT4D;
   tPARAM_RELOJ_TARIFA EEPROM_PROG_relojT1N;
   tPARAM_RELOJ_TARIFA EEPROM_PROG_relojT2N;
   tPARAM_RELOJ_TARIFA EEPROM_PROG_relojT3N;
   tPARAM_RELOJ_TARIFA EEPROM_PROG_relojT4N;
   tPARAM_RELOJ_EQPESOS EEPROM_PROG_relojEqPESOS;
   tPARAM_RELOJ_CALEND EEPROM_PROG_relojCALEND[dim_Feriados];

/*
#ifdef VISOR_PROG_TICKET
  const tPRG_TICKET EEPROM_PROG_TICKET;
  const tPRG_TICKET_RECAUD EEPROM_PROG_TICKET_RECAUD;
#endif
*/

/*********************************************************************************************/
/* PROTOTIPOS */
/**************/
  // PROGRAMACION
  static tPRG_ERROR PROG_writeRELOJtarifa (byte N, byte subCMD);
  
  // INTERNAS
  static byte* prgRELOJ_COMUN_extractDATE_addRxData (tBAX_SCI* BAX_SCI_ptr);
  static void prgRELOJcomun_armarDEFAULT (void);
  static void prgRELOJtarifa_armarDEFAULT (byte nro);
  static void prgRELOJeqPesos_armarDEFAULT (void);
  static void prgRELOJcalendar_armarDEFAULT (void);
  
  // EEPROM
  static void armarBuffer_progCOMUNES_EEPROM (byte* RAMbuffer);
  static void armarBuffer_progTARIFA_EEPROM (byte nro, byte* RAMbuffer);

  static void armarBuffer_progEqPESOS_EEPROM (byte* buffer);
  static void armarBuffer_progCALEND_EEPROM (byte* buffer);
  static void progProgramacionPorDefecto(void);


/*********************************************************************************************/
/* VARIABLES */
/*************/
   byte PROG_RELOJ_DATE_PC[6];              // Hora y Fecha recibida desde la PC (DIA-MES-AÑO-HORA-MIN-SEG)
  
  static byte* progRELOJ_COMUN_KATE;              // Puntero de extraccion de datos de parametros de RELOJ KATE
  static byte* progRELOJ_COMUN_GET;               // Puntero de extraccion de datos de parametros de RELOJ COMUNES
  static byte* progRELOJ_T1D_GET;                 // Puntero de extraccion de datos de parametros de RELOJ T1D
  static byte* progRELOJ_T2D_GET;                 // Puntero de extraccion de datos de parametros de RELOJ T2D
  static byte* progRELOJ_T3D_GET;                 // Puntero de extraccion de datos de parametros de RELOJ T3D
  static byte* progRELOJ_T4D_GET;                 // Puntero de extraccion de datos de parametros de RELOJ T4D
  static byte* progRELOJ_T1N_GET;                 // Puntero de extraccion de datos de parametros de RELOJ T1N
  static byte* progRELOJ_T2N_GET;                 // Puntero de extraccion de datos de parametros de RELOJ T2N
  static byte* progRELOJ_T3N_GET;                 // Puntero de extraccion de datos de parametros de RELOJ T3N
  static byte* progRELOJ_T4N_GET;                 // Puntero de extraccion de datos de parametros de RELOJ T4N
  static byte* progRELOJ_EQ_GET;                  // Puntero de extraccion de datos de parametros de RELOJ EQ PESOS
  static byte* progRELOJ_CALEND_GET;              // Puntero de extraccion de datos de parametros de RELOJ CALENDAR
  
  
  static  byte** TARIFAS_GET_ptr[8]={
    &progRELOJ_T1D_GET,
    &progRELOJ_T2D_GET,
    &progRELOJ_T3D_GET,
    &progRELOJ_T4D_GET,
    &progRELOJ_T1N_GET,
    &progRELOJ_T2N_GET,
    &progRELOJ_T3N_GET,
    &progRELOJ_T4N_GET
  };

  static byte ELIMINAR_REPORTES;                  // Eliminar Reportes de RELOJ

  static byte TARIFA_toUpdate;                    // Nro de TARIFA a Actualizar


/*********************************************************************************************/
/* RUTINAS */
/***********/

/* LEVANTAR PROGRAMACION DE PARAMETROS DE RELOJ */
/************************************************/

  void prgRELOJ_ini (void){
    
	byte* eeprom_ptr;
	uint32_t EEPROM_ptr;
	byte nro;
	byte EEPROM_AUX[SIZE_PROG_relojCALEND];
	byte* ptr1AUX;
	byte i;
	byte dataEEPROM;
	tEEPROM_ERROR error;
    byte tarifa_max;
	//eeprom_ptr = (byte*) &EEPROM_PROG_relojCOMUN; // Puntero a Inicio de Parametros Comunes (CONSTANTE)
	 //dataEEPROM = EEPROM_ReadByte(ADDRESS_PROG_relojCOMUN);
	//EEPROM_ReadBuffer(&dataEEPROM,ADDRESS_PROG_relojCOMUN,1);
/*
	error = chkCRC_EnEEPROM(ADDRESS_PROG_relojCOMUN, EEPROMsize_PRG_relojCOMUN);
	//if(error != EEPROM_OK){
	if(error != EEPROM_OK){
		progProgramacionPorDefecto();
	}
	error = chkCRC_EnEEPROM(ADDRESS_PROG_relojCOMUN, EEPROMsize_PRG_relojCOMUN);
	if(error == EEPROM_OK){
    	 //levanta desde eeprom datos de reloj comun para obtener el valor de seleccionManualTarifas
    	 EEPROM_ReadBuffer(&EEPROM_PROG_relojCOMUN, ADDRESS_PROG_relojCOMUN, sizeof(tPARAM_RELOJ_COMUNES));
		if(seleccionManualTarifas){
			tarifa_max = prgRELOJ_determineCantTarifasD_MANUAL() + prgRELOJ_determineCantTarifasN_MANUAL();
		}else{
			tarifa_max = prgRELOJ_determineCantTarifasD_AUTOMATICA() + prgRELOJ_determineCantTarifasN_AUTOMATICA();
		}
	}

	nro = 1;
	while((error == EEPROM_OK) && (nro < tarifa_max+1)){
		EEPROM_ptr = (dword*) (getDir_tarifaX_BlockProg(nro, ADDRESS_PROG1));
		error = chkCRC_EnEEPROM(EEPROM_ptr, EEPROMsize_PRG_relojTARIFA);
		nro++;
	}
*/

    /*
	error = chkCRC_EnEEPROM(ADDRESS_PROG_relojCOMUN, EEPROMsize_PRG_relojCOMUN);
	if(error == EEPROM_OK){
		//levanta desde eeprom datos de reloj comun para obtener tarifas habilitadas
		EEPROM_ReadBuffer(&EEPROM_PROG_relojCOMUN, ADDRESS_PROG_relojCOMUN, sizeof(tPARAM_RELOJ_COMUNES));
		error = checkProgTarifas(ADDRESS_PROG1);
	}

	if(error == EEPROM_OK){
		error = chkCRC_EnEEPROM(ADDRESS_PROG_relojEqPESOS, EEPROMsize_PRG_relojEqPESOS);
	}
	if(error == EEPROM_OK){
		error = chkCRC_EnEEPROM(ADDRESS_PROG_relojCALEND, EEPROMsize_PRG_relojCALEND);
	}
*/

    checkDataProg();
    error = EEPROM_CHECK_SUM;
    if(blckPROG1_OK){
    	error = EEPROM_OK;
    }

    if (error == EEPROM_OK){
    	//levantar_progRELOJ();
    	readProgRELOJ (ADDRESS_PROG1);
		//ini varios
		if(TARIFA_PESOS){
			//muestra importe
			fichaPESOS=1;
		}else{
			//muestra ficha
			fichaPESOS=2;
			//PUNTO_DECIMAL=3;
		}
		nroTARIFA_HAB_MANUAL = prgRELOJ_determineCantTarifasD_MANUAL();
		nroTARIFA_HAB_AUTOMATICA = prgRELOJ_determineCantTarifasD_AUTOMATICA();

    }else{
 	  EEPROM_PROG_MOVIL.tipoReloj = INTERNO; //para que cuando este borrado toda la eeprom SE PUEDA CONSULTAR LA VERSION DE FIRMWARE
	  HORA_source = 1;						 //para que cuando este borrado toda la eeprom pueda inicializar reportes
    }
}


  tEEPROM_ERROR checkProgTarifas(uint32_t progBlock){

   uint32_t 		EEPROM_ptr;
   byte 			nro;
   tEEPROM_ERROR 	error;

   	   nro = 0;
	   if (T1D_hab){
			nro = 1;
			EEPROM_ptr = (dword*) (getDir_tarifaX_BlockProg(nro, progBlock));
			error = chkCRC_EnEEPROM(EEPROM_ptr, EEPROMsize_PRG_relojTARIFA);
		}
	   //para prueba
	   readTarifa(1, progBlock);

	   if (T2D_hab && error == EEPROM_OK){
			nro = 2;
			EEPROM_ptr = (dword*) (getDir_tarifaX_BlockProg(nro, progBlock));
			error = chkCRC_EnEEPROM(EEPROM_ptr, EEPROMsize_PRG_relojTARIFA);
		}
	   //para prueba
	   readTarifa(2, progBlock);

	   if (T3D_hab && error == EEPROM_OK ){
			nro = 3;
			EEPROM_ptr = (dword*) (getDir_tarifaX_BlockProg(nro, progBlock));
			error = chkCRC_EnEEPROM(EEPROM_ptr, EEPROMsize_PRG_relojTARIFA);
		}

		if (T4D_hab && error == EEPROM_OK){
			nro = 4;
			EEPROM_ptr = (dword*) (getDir_tarifaX_BlockProg(nro, progBlock));
			error = chkCRC_EnEEPROM(EEPROM_ptr, EEPROMsize_PRG_relojTARIFA);
		}

		if (T1N_hab && error == EEPROM_OK){
			nro = 5;
			EEPROM_ptr = (dword*) (getDir_tarifaX_BlockProg(nro, progBlock));
			error = chkCRC_EnEEPROM(EEPROM_ptr, EEPROMsize_PRG_relojTARIFA);
		}
	   //para prueba
	   readTarifa(5, progBlock);

		if (T2N_hab && error == EEPROM_OK){
			nro = 6;
			EEPROM_ptr = (dword*) (getDir_tarifaX_BlockProg(nro, progBlock));
			error = chkCRC_EnEEPROM(EEPROM_ptr, EEPROMsize_PRG_relojTARIFA);
		}
		if (T3N_hab && error == EEPROM_OK){
			nro = 7;
			EEPROM_ptr = (dword*) (getDir_tarifaX_BlockProg(nro, progBlock));
			error = chkCRC_EnEEPROM(EEPROM_ptr, EEPROMsize_PRG_relojTARIFA);
		}
		if (T4N_hab && error == EEPROM_OK){
			nro = 8;
			EEPROM_ptr = (dword*) (getDir_tarifaX_BlockProg(nro, progBlock));
			error = chkCRC_EnEEPROM(EEPROM_ptr, EEPROMsize_PRG_relojTARIFA);
		}
		if(nro == 0){
			error = EEPROM_CHECK_SUM;
		}
		return (error);
  }

  tEEPROM_ERROR readTarifa(byte nro, uint32_t  addressTarifa){

      	byte EEPROM_buffer_test[EEPROMsize_PRG_relojTARIFA];
      	tEEPROM_ERROR error;
      	dword* EEPROM_ptr;
      	uint32_t dir;

      	dir = (uint32_t)addressTarifa;

      	EEPROM_ptr = (uint32_t) (getDir_tarifaX_BlockProg(nro, addressTarifa));

      	//para prueba
      	EEPROM_ReadBuffer(&EEPROM_buffer_test, EEPROM_ptr, sizeof(tPARAM_RELOJ_TARIFA));
  }


  static void progProgramacionPorDefecto(void){
       tEEPROM_ERROR error;
       //parametros de movil por defecto
        prgMOVIL_armarDEFAULT();            // Armo programacion por defecto
        PROG_MOVIL_to_EEPROM(0);            // Solicitar grabacion parametros en EEPROM, sin iniciar EEPROM IRQ
        error = PROG_MOVIL_grabarEEPROM();
		if (error != EEPROM_OK){
		 // BUZZER_play(RING_error);
		}

		// No hay programacion o hay error=> Levanto defecto
		prgRELOJcomun_armarDEFAULT();               // Armo String con parametros por defecto
		PROG_RELOJcomun_to_EEPROM(0);               // Solciitar grabacion parametros en EEPROM, sin iniciar EEPROM IRQ
		error = PROG_RELOJcomunes_grabarEEPROM();   // Grabar Parametros Comunes
		if (error != EEPROM_OK){
		 // BUZZER_play(RING_error);
		}

		for (byte i=tarifa1D; i<(tarifa4N+1); i++){
		  prgRELOJtarifa_armarDEFAULT(i);           // Armo String con parametros por defecto
		  PROG_RELOJtarifa_to_EEPROM(i,0);		      // Solciitar grabacion parametros en EEPROM, sin iniciar EEPROM IRQ
		  error = PROG_RELOJtarifa_grabarEEPROM(i);	// Grabar Tarifa en EEPROM
		  if (error != EEPROM_OK){
			//BUZZER_play(RING_error);
		  }
		}

		prgRELOJeqPesos_armarDEFAULT();             // Armo String con parametros por defecto
		PROG_RELOJeqPesos_to_EEPROM(0);				// Solciitar grabacion parametros en EEPROM, sin iniciar EEPROM IRQ
		error = PROG_RELOJeqPesos_grabarEEPROM();   // Grabar Eq Pesos en EEPROM
		if (error != EEPROM_OK){
		 // BUZZER_play(RING_error);
		}

		prgRELOJcalendar_armarDEFAULT();            // Armo String con parametros por defecto
		PROG_RELOJcalend_to_EEPROM(0);						  // Solciitar grabacion parametros en EEPROM, sin iniciar EEPROM IRQ
		error = PROG_RELOJcalend_grabarEEPROM();    // Grabar Calendario en EEPROM
		if (error != EEPROM_OK){
		 // BUZZER_play(RING_error);
		}
		//Buzzer_On(BEEP_PROGRAMCION_ERROR);

		//error_eepromDATA=1;
		//prog_mode=1;

		//EEPROM_PROG_MOVIL.tipoReloj = INTERNO; //para que cuando este borrado toda la eeprom SE PUEDA CONSULTAR LA VERSION DE FIRMWARE
		//HORA_source = 1;		//para que cuando este borrado toda la eeprom pueda inicializar reportes

     }

 
  /* RE-PROGRAMACION DE RELOJ, CON VALORES ACTUALES Y CONSTANTE NUEVA */
  /********************************************************************/
    tEEPROM_ERROR prgRELOJ_reprogCOMUNES (word newK){
      // Esta rutina se llama cuando se va a re-programar la constante. Lo que
      // hace es armar el buffer de programacion con los valores actuales, y 
      // reemplaza la constante (en ese buffer de programacion), por el valor nuevo
      byte* EEPROM_ptr;
      byte* newK_ptr;
      tEEPROM_ERROR error;
      uint32_t prim;
      
      EEPROM_ptr = (byte*) &EEPROM_PROG_relojCOMUN;
      newK_ptr = (byte*) &newK;
      
      progRELOJ_COMUN_GET = PROGRAMADOR_addByte(*newK_ptr++); // Pulsos x KM calculada (H)
      EEPROM_ptr++;                                           // Avanzo puntero de EEPROM como si hubiese extraido la constante
      (void)PROGRAMADOR_addByte(*newK_ptr);                   // Pulsos x KM calculada (L)
      EEPROM_ptr++;                                           // Avanzo puntero de EEPROM como si hubiese extraido la constante
      
      while ((*EEPROM_ptr != finEEPROM_H) && (*(EEPROM_ptr + 1) != finEEPROM_L)){
        (void)PROGRAMADOR_addByte(*EEPROM_ptr++);             // Voy copiando valores de EEPRO
      }
      
      (void)PROGRAMADOR_addByte(finEEPROM_H);                 // Fin de Datos
      (void)PROGRAMADOR_addByte(finEEPROM_L);
      
		prim = __get_PRIMASK();
		__disable_irq();

      PROG_RELOJcomun_to_EEPROM(0);                           // Solciitar grabacion parametros en EEPROM, sin iniciar EEPROM IRQ
      error = PROG_RELOJcomunes_grabarEEPROM();               // Grabar Parametros Comunes
      if (error != EEPROM_OK){
       // BUZZER_play(RING_error);
        
      }
  	if (!prim) {
  				__enable_irq();
  	}

      return(error);
    }
  

  /* DETERMINAR SI DEBO ELIMINAR LOS REPORTES */
  /********************************************/
    byte PROG_determineDeleteReportes (void){
      return(ELIMINAR_REPORTES);
    }

/*********************************************************************************************/
/* INTERACCION CON EL PROGRAMADOR */
/**********************************/


  /* PROGRAMAR PARAMETROS DE RELOJ COMUNES */
  /*****************************************/
    void PROG_writeRELOJcomun (byte N){
      // Se ejecuta cuando el PROGRAMADOR da la orden de PROGRAMAR
      tPRG_ERROR error;
      
      error = PROGRAMADOR_chkN (N, N_RELOJcomun + 6); // Comparo que el N recibido se igual al esperado
      if (PROGRAMADOR_chkJumper()){
        // Tiene el jumper o no es necesario chequearlo
        if (error == PRG_OK){
          // Extraigo DATE recibido desde la PC
          //progRELOJ_COMUN_GET guarda direccion de inicio de buffer de programacion
          progRELOJ_COMUN_GET = prgRELOJ_COMUN_extractDATE_addRxData(&BAX_SCI_PROG);
          if (progRELOJ_COMUN_GET == NULL){
            error = PRG_NULL_POINTER;
          }
        }
        prgRELOJ_COMUNES_OK_F = 0;                  // Asumo Parametro Erroneo
        if (error == PRG_OK){
          // Solo respondo si no hay errores, xq sino voy a grabar cualquiera en el EEPROM
        //  RTC_updateDate(PROG_RELOJ_DATE_PC);       // Actualizo Fecha y Hora del RTC
          
          PROGRAMADOR_startTxRTA(subCMD_RELOJcomun);// Envio la Rta hacia el programador
          prgRELOJ_COMUNES_OK_F = 1;                // Parametro Correcto y Listo para ser Programado al finalizar Proceso
        
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

  
  /* PROGRAMAR PARAMETROS DE RELOJ TARIFA */
  /****************************************/
    static tPRG_ERROR PROG_writeRELOJtarifa (byte N, byte subCMD){
      // Se ejecuta cuando el PROGRAMADOR da la orden de PROGRAMAR
      tPRG_ERROR error;
      byte numeroTarifa, diaNoche;


      longTarifa = N;
      error = PROGRAMADOR_chkN (N, N_RELOJtarifa);            // programador viejo 

      if (PROGRAMADOR_chkJumper()){
        // Tiene el jumper o no es necesario chequearlo
        if (error == PRG_OK){
          numeroTarifa = subCMD - subCMD_RELOJtarifa1D;   // Quito Offset de subCMD_D
          if (numeroTarifa > 4){
            diaNoche = NOCTURNA;                          // Tarifa Nocturna
            numeroTarifa = subCMD - subCMD_RELOJtarifa1N; // Quito Offset de subCMD_D
            numeroTarifa += 4;                            // Las nocturnas arrancan con offset
          }else{
            diaNoche = DIURNA;                            // Tarifa Diurna
          }
          
          *TARIFAS_GET_ptr[numeroTarifa] = PROGRAMADOR_addByte(numeroTarifa + 1);  // Le sumo 1, xq al quitarle el "offset" arranca en 0
          (void)PROGRAMADOR_addByte(diaNoche);            // Agrego indicacion Dia/Noche
          (void)PROGRAMADOR_addRxData(&BAX_SCI_PROG);
          if (*TARIFAS_GET_ptr[numeroTarifa] == NULL){
            error = PRG_NULL_POINTER;
          }
        }
        
        if (error == PRG_OK){
          // Solo respondo si no hay errores, xq sino voy a grabar cualquiera en el EEPROM
          PROGRAMADOR_startTxRTA(subCMD);           // Envio la Rta hacia el programador
        
        }else{
          // Error al intentar programar los parametros
         // BUZZER_play(RING_error);                  // Reproduzo sonido de error
        }
      
      }else{
        // No tiene el jumper de programacion
        error = PRG_NO_JUMPER;
        PROGRAMADOR_TxNoJumper();                   // Notifico al programador
      }
      
      return(error);
    }    




	// PROGRAMAR PARAMETROS DE RELOJ TARIFA 1D
	void PROG_writeRELOJ_T1D (byte N){
		tPRG_ERROR error;

		error = PROG_writeRELOJtarifa (N, subCMD_RELOJtarifa1D);

		if (error == PRG_OK){
			prgRELOJ_T1D_OK_F = 1;                // Parametro Correcto y Listo para ser Programado al finalizar Proceso
		}else{
			prgRELOJ_T1D_OK_F = 0;                // Parametro Erroneo => Error de Programacion
		}
	}

	// PROGRAMAR PARAMETROS DE RELOJ TARIFA 2D
	void PROG_writeRELOJ_T2D (byte N){
		tPRG_ERROR error;

		error = PROG_writeRELOJtarifa (N, subCMD_RELOJtarifa2D);

		if (error == PRG_OK){
			prgRELOJ_T2D_OK_F = 1;                // Parametro Correcto y Listo para ser Programado al finalizar Proceso
		}else{
			prgRELOJ_T2D_OK_F = 0;                // Parametro Erroneo => Error de Programacion
		}
	}

	// PROGRAMAR PARAMETROS DE RELOJ TARIFA 3D
	void PROG_writeRELOJ_T3D (byte N){
		tPRG_ERROR error;

		error = PROG_writeRELOJtarifa (N, subCMD_RELOJtarifa3D);

		if (error == PRG_OK){
			prgRELOJ_T3D_OK_F = 1;                // Parametro Correcto y Listo para ser Programado al finalizar Proceso
		}else{
			prgRELOJ_T3D_OK_F = 0;                // Parametro Erroneo => Error de Programacion
		}
	}

	// PROGRAMAR PARAMETROS DE RELOJ TARIFA 4D
	void PROG_writeRELOJ_T4D (byte N){
		tPRG_ERROR error;

		error = PROG_writeRELOJtarifa (N, subCMD_RELOJtarifa4D);

		if (error == PRG_OK){
			prgRELOJ_T4D_OK_F = 1;                // Parametro Correcto y Listo para ser Programado al finalizar Proceso
		}else{
			prgRELOJ_T4D_OK_F = 0;                // Parametro Erroneo => Error de Programacion
		}
	}

	// PROGRAMAR PARAMETROS DE RELOJ TARIFA 1N
	void PROG_writeRELOJ_T1N (byte N){
		tPRG_ERROR error;

		error = PROG_writeRELOJtarifa (N, subCMD_RELOJtarifa1N);

		if (error == PRG_OK){
			prgRELOJ_T1N_OK_F = 1;                // Parametro Correcto y Listo para ser Programado al finalizar Proceso
		}else{
			prgRELOJ_T1N_OK_F = 0;                // Parametro Erroneo => Error de Programacion
		}
	}

	// PROGRAMAR PARAMETROS DE RELOJ TARIFA 2N
	void PROG_writeRELOJ_T2N (byte N){
		tPRG_ERROR error;

		error = PROG_writeRELOJtarifa (N, subCMD_RELOJtarifa2N);

		if (error == PRG_OK){
			prgRELOJ_T2N_OK_F = 1;                // Parametro Correcto y Listo para ser Programado al finalizar Proceso
		}else{
			prgRELOJ_T2N_OK_F = 0;                // Parametro Erroneo => Error de Programacion
		}
	}


	// PROGRAMAR PARAMETROS DE RELOJ TARIFA 3N
	void PROG_writeRELOJ_T3N (byte N){
		tPRG_ERROR error;

		error = PROG_writeRELOJtarifa (N, subCMD_RELOJtarifa3N);

		if (error == PRG_OK){
			prgRELOJ_T3N_OK_F = 1;                // Parametro Correcto y Listo para ser Programado al finalizar Proceso
		}else{
			prgRELOJ_T3N_OK_F = 0;                // Parametro Erroneo => Error de Programacion
		}
	}

	// PROGRAMAR PARAMETROS DE RELOJ TARIFA 4N
	void PROG_writeRELOJ_T4N (byte N){
		tPRG_ERROR error;

		error = PROG_writeRELOJtarifa (N, subCMD_RELOJtarifa4N);

		if (error == PRG_OK){
			prgRELOJ_T4N_OK_F = 1;                // Parametro Correcto y Listo para ser Programado al finalizar Proceso
		}else{
			prgRELOJ_T4N_OK_F = 0;                // Parametro Erroneo => Error de Programacion
		}
	}


	/* PROGRAMAR PARAMETROS DE RELOJ EQ. PESOS */
	/*******************************************/
	void PROG_writeRELOJeqPesos (byte N){
		// Se ejecuta cuando el PROGRAMADOR da la orden de PROGRAMAR
		tPRG_ERROR error;

		error = PROGRAMADOR_chkN (N, N_RELOJeqPesos);     // Comparo que el N recibido se igual al esperado

		if (PROGRAMADOR_chkJumper()){
		// Tiene el jumper o no es necesario chequearlo
			if (error == PRG_OK){
				progRELOJ_EQ_GET = PROGRAMADOR_addRxData(&BAX_SCI_PROG);
				if (progRELOJ_EQ_GET == NULL){
				  error = PRG_NULL_POINTER;
				}
			}

			prgRELOJ_EQPESOS_OK_F = 0;                        // Asumo Parametro Erroneo

			if (error == PRG_OK){
			// Solo respondo si no hay errores, xq sino voy a grabar cualquiera en el EEPROM
				PROGRAMADOR_startTxRTA(subCMD_RELOJeqPesos);    // Envio la Rta hacia el programador
				prgRELOJ_EQPESOS_OK_F = 1;                      // Parametro Correcto y Listo para ser Programado al finalizar Proceso

			}else{
			// Error al intentar programar los parametros
			// BUZZER_play(RING_error);                        // Reproduzo sonido de error
			}

			}else{
				// No tiene el jumper de programacion
				error = PRG_NO_JUMPER;
				PROGRAMADOR_TxNoJumper();                   // Notifico al programador
			}
	}

	/* PROGRAMAR PARAMETROS DE RELOJ CALENDARIO */
	/********************************************/
	void PROG_writeRELOJcalendar (byte N){
		// Se ejecuta cuando el PROGRAMADOR da la orden de PROGRAMAR
		tPRG_ERROR error;

		error = PROGRAMADOR_chkN (N, N_RELOJcalendar);    // Comparo que el N recibido se igual al esperado

		if (PROGRAMADOR_chkJumper()){
			// Tiene el jumper o no es necesario chequearlo
			if (error == PRG_OK){
				progRELOJ_CALEND_GET = PROGRAMADOR_addRxData(&BAX_SCI_PROG);
				if (progRELOJ_CALEND_GET == NULL){
				  error = PRG_NULL_POINTER;
				}
			}
			prgRELOJ_CALEND_OK_F = 0;                         // Asumo Parametro Erroneo
			if (error == PRG_OK){
				// Solo respondo si no hay errores, xq sino voy a grabar cualquiera en el EEPROM
				PROGRAMADOR_startTxRTA(subCMD_RELOJcalendar);   // Envio la Rta hacia el programador
				prgRELOJ_CALEND_OK_F = 1;                       // Parametro Correcto y Listo para ser Programado al finalizar Proceso
			}else{
				// Error al intentar programar los parametros
				//  BUZZER_play(RING_error);                        // Reproduzo sonido de error
			}

		}else{
			// No tiene el jumper de programacion
			error = PRG_NO_JUMPER;
			PROGRAMADOR_TxNoJumper();                   // Notifico al programador
		}
	}



	/* LEER PARAMETROS DE RELOJ COMUNES */
	  /************************************/
	    void PROG_readRELOJcomun (byte N){
	      // Se ejecuta cuando el PROGRAMADOR da la orden de LEER
	      tPRG_ERROR error;
	      uint8_t buffer_prog[SIZE_PROG_relojCOMUN+2];

	      error = PROGRAMADOR_chkN (N, N_lectura);    // Comparo que el N recibido se igual al esperado

	      if (error == PRG_OK){
		  	  read_relojCOMUN_eeprom((uint8_t*)&buffer_prog);
		  	  buffer_prog[SIZE_PROG_relojCOMUN]   = 0xdf;
		  	  buffer_prog[SIZE_PROG_relojCOMUN+1] = 0x0a;
	          PROGRAMADOR_startTx(CMD_LECTURA+0x80, subCMD_RELOJcomun, (uint8_t*)&buffer_prog);
	      }
	    }


	// LEER PARAMETROS DE RELOJ TARIFA 1D
	void PROG_readRELOJ_T1D (byte N){

		//uint8_t buffer_prog[128];
		uint8_t buffer_prog[SIZE_PROG_relojT1D+2];

		read_relojT1D_eeprom((uint8_t*)&buffer_prog);	  //26
		buffer_prog[SIZE_PROG_relojT1D] = 0xdf;
		buffer_prog[SIZE_PROG_relojT1D+1] = 0x0a;
		PROGRAMADOR_startTx(CMD_LECTURA+0x80, subCMD_RELOJtarifa1D, (uint8_t*)&buffer_prog[2]);
	}

	// LEER PARAMETROS DE RELOJ TARIFA 2D
	void PROG_readRELOJ_T2D (byte N){
		//uint8_t buffer_prog[128];
		uint8_t buffer_prog[SIZE_PROG_relojT2D+2];

		read_relojT2D_eeprom((uint8_t*)&buffer_prog);

		buffer_prog[SIZE_PROG_relojT2D]	  = 0xdf;
		buffer_prog[SIZE_PROG_relojT2D+1] = 0x0a;

		PROGRAMADOR_startTx(CMD_LECTURA+0x80, subCMD_RELOJtarifa2D, (uint8_t*)&buffer_prog[2]);
	}

	// LEER PARAMETROS DE RELOJ TARIFA 3D
	void PROG_readRELOJ_T3D (byte N){
		//uint8_t buffer_prog[128];
		uint8_t buffer_prog[SIZE_PROG_relojT3D+2];

		read_relojT3D_eeprom((uint8_t*)&buffer_prog);
		buffer_prog[SIZE_PROG_relojT3D] = 0xdf;
		buffer_prog[SIZE_PROG_relojT3D+1] = 0x0a;

		PROGRAMADOR_startTx(CMD_LECTURA+0x80, subCMD_RELOJtarifa3D, (uint8_t*)&buffer_prog[2]);
	}

	// LEER PARAMETROS DE RELOJ TARIFA 4D
	void PROG_readRELOJ_T4D (byte N){
		//uint8_t buffer_prog[128];
		uint8_t buffer_prog[SIZE_PROG_relojT4D+2];

		read_relojT4D_eeprom((uint8_t*)&buffer_prog);

		buffer_prog[SIZE_PROG_relojT4D]   = 0xdf;
		buffer_prog[SIZE_PROG_relojT4D+1] = 0x0a;

		PROGRAMADOR_startTx(CMD_LECTURA+0x80, subCMD_RELOJtarifa4D, (uint8_t*)&buffer_prog[2]);
	}

	// LEER PARAMETROS DE RELOJ TARIFA 1N
	void PROG_readRELOJ_T1N (byte N){
		//uint8_t buffer_prog[128];
		uint8_t buffer_prog[SIZE_PROG_relojT1N+2];

		read_relojT1N_eeprom((uint8_t*)&buffer_prog);

		buffer_prog[SIZE_PROG_relojT1N]   = 0xdf;
		buffer_prog[SIZE_PROG_relojT1N+1] = 0x0a;

		PROGRAMADOR_startTx(CMD_LECTURA+0x80, subCMD_RELOJtarifa1N, (uint8_t*)&buffer_prog[2]);
	}

	// LEER PARAMETROS DE RELOJ TARIFA 2N
	void PROG_readRELOJ_T2N (byte N){
		//uint8_t buffer_prog[128];
		uint8_t buffer_prog[SIZE_PROG_relojT2N+2];

		read_relojT2N_eeprom((uint8_t*)&buffer_prog);

		buffer_prog[SIZE_PROG_relojT2N]   = 0xdf;
		buffer_prog[SIZE_PROG_relojT2N+1] = 0x0a;

		PROGRAMADOR_startTx(CMD_LECTURA+0x80, subCMD_RELOJtarifa2N, (uint8_t*)&buffer_prog[2]);
	}

	// LEER PARAMETROS DE RELOJ TARIFA 3N
	void PROG_readRELOJ_T3N (byte N){
		//uint8_t buffer_prog[128];
		uint8_t buffer_prog[SIZE_PROG_relojT3N+2];

		read_relojT3N_eeprom((uint8_t*)&buffer_prog);
		buffer_prog[SIZE_PROG_relojT3N]   = 0xdf;
		buffer_prog[SIZE_PROG_relojT3N+1] = 0x0a;

		PROGRAMADOR_startTx(CMD_LECTURA+0x80, subCMD_RELOJtarifa3N, (uint8_t*)&buffer_prog[2]);
	}

	// LEER PARAMETROS DE RELOJ TARIFA 4N
	//uint8_t buffer_prog[128];
	void PROG_readRELOJ_T4N (byte N){

        //uint8_t i;
		uint8_t buffer_prog[SIZE_PROG_relojT4N+2];

		read_relojT4N_eeprom((uint8_t*)&buffer_prog);

		buffer_prog[SIZE_PROG_relojT4N]   = 0xdf;
		buffer_prog[SIZE_PROG_relojT4N+1] = 0x0a;

		PROGRAMADOR_startTx(CMD_LECTURA+0x80, subCMD_RELOJtarifa4N, (uint8_t*)&buffer_prog[2]);
		//PROGRAMADOR_startTx(CMD_LECTURA+0x80, subCMD_RELOJeqPesos, (uint8_t*)&buffer_prog);
	}

	/* LEER PARAMETROS DE RELOJ EQ. PESOS */
	/**************************************/
	void PROG_readRELOJeqPesos (byte N){
		//uint8_t buffer_prog[128];
		uint8_t buffer_prog[SIZE_PROG_relojEqPESOS+2];

		read_relojEqPESOS_eeprom((uint8_t*)&buffer_prog);	//long 22

		buffer_prog[SIZE_PROG_relojEqPESOS]   = 0xdf;
		buffer_prog[SIZE_PROG_relojEqPESOS+1] = 0x0a;

		PROGRAMADOR_startTx(CMD_LECTURA+0x80, subCMD_RELOJeqPesos, (uint8_t*)&buffer_prog);
	}


	/* LEER PARAMETROS DE RELOJ CALENDARIO */
	/***************************************/
	void PROG_readRELOJcalendar (byte N){
		//uint8_t buffer_prog[128];
		uint8_t buffer_prog[SIZE_PROG_relojCALEND+2];


		read_relojCALEND_eeprom((uint8_t*)&buffer_prog);  //long 44

		buffer_prog[SIZE_PROG_relojCALEND]   = 0xdf;
		buffer_prog[SIZE_PROG_relojCALEND+1] = 0x0a;

		PROGRAMADOR_startTx(CMD_LECTURA+0x80, subCMD_RELOJcalendar, (uint8_t*)&buffer_prog);
	}
  
  
  

/*********************************************************************************************/
/* RUTINAS INTERNAS */
/********************/

    
  
  /* EXTRAER DATE y AGREGAR PARAMETROS COMUNES A BUFFER PROGRAMADOR */
  /*****************************************************************/
    static byte* prgRELOJ_COMUN_extractDATE_addRxData (tBAX_SCI* BAX_SCI_ptr){
      // El parametro COMUN viene con FECHA y HORA por eso lo saco a parte
      byte N, CMD, subCMD, i;
      byte* GETptr;
      byte TO_F;
      byte dato;
      byte* DATEbuffer;
      byte* returnPTR;
      
      GETptr = BAX_SCI_ptr->RxGPA[BAX_SCI_ptr->RxGPA_GETidx];       // Extraigo puntero GET, para poder tomar su direccion
      N = get_byte (&GETptr, BAX_SCI_ptr->RxBuffer, dim_BAX);       // Extraigo N

      CMD = get_byte (&GETptr, BAX_SCI_ptr->RxBuffer, dim_BAX);     // Extraigo Comando
      subCMD = get_byte (&GETptr, BAX_SCI_ptr->RxBuffer, dim_BAX);  // Extraigo Sub-Comando

      // Extraigo DATE      
      i = 6;                                                        // Son 6 bytes de DATE
      DATEbuffer = PROG_RELOJ_DATE_PC;                              // Puntero a Date PC
      TO_F = 0;                                                     // Limpio Bandera
      dispararTO_lazo();                                            // Disparo TimeOut Lazo
      while ((i > 0) && (!TO_F)){
        TO_F = chkTO_lazo_F();                                      // Chequeo bandera de time out de lazo 
        N--;                                                        // Decremento longitud de datos
        i--;
        *DATEbuffer++ = get_byte (&GETptr, BAX_SCI_ptr->RxBuffer, dim_BAX); // Voy extrayendo y guardando el string de programacion 
      }
      detenerTO_lazo();                                             // Detener TimeOut Lazo
      
      // Extraigo PARAMETROS
      dato = get_byte (&GETptr, BAX_SCI_ptr->RxBuffer, dim_BAX);    // Voy extrayendo y guardando el string de programacion 
      //Agrego dato a buffer de PROGRAMACION y guardo direcion de inicio de
      //de buffer de programacion de reloj (returnPTR)
      returnPTR = PROGRAMADOR_addByte(dato);
      N--;                                                          // Decremento longitud de datos
      if (!TO_F){
        TO_F = 0;                                                   // Limpio Bandera
        dispararTO_lazo();                                          // Disparo TimeOut Lazo
        while ((N > 0) && (!TO_F)){
          TO_F = chkTO_lazo_F();                                    // Chequeo bandera de time out de lazo 
          N--;                                                      // Decremento longitud de datos
          dato = get_byte (&GETptr, BAX_SCI_ptr->RxBuffer, dim_BAX);// Voy extrayendo y guardando el string de programacion 
          (void)PROGRAMADOR_addByte(dato);                          // Agrego dato a buffer de PROGRAMACION
        }
        detenerTO_lazo();                                           // Detener TimeOut Lazo
      }
      
      if (TO_F){
        returnPTR = NULL;                                           // Error de TimeOut
      }
      
      return (returnPTR);
    } 
    
    
  /* ARMAR STRING DE PARAMETROS COMUNES */
  /**************************************/
    static void prgRELOJcomun_armarDEFAULT (void){
      // Los parametros comunes se grabaran en EEPROM con el siguiente formato:
      // (recordemos que la EEPROM se divide en bloques de 1 longword, es decir
      //  4bytes)
      //
      //  +----|----|----|----++----|----|----|----++----|----|----|----+
      //  | constan |carr.band|| v1	| v2 | v3 | v4 ||	DF | 0A	| xx	 xx	|
      //  +----|----|----|----++----|----|----|----++----|----|----|----+
      //
      // Solo tiene sentido guardar el puntero GET la primera vez que agrego un
      // dato, asi cuando extraigo los mismo para grabarlos en el EEPROM, los
      // grabo desde el primero

      progRELOJ_COMUN_GET = PROGRAMADOR_addByte(0x03);// Pulsos x KM por defecto (H)
      (void)PROGRAMADOR_addByte(0xE8);                // Pulsos x KM por defecto (L)
      (void)PROGRAMADOR_addByte(0x00);                // Carrera Bandera por defecto (H)
      (void)PROGRAMADOR_addByte(0x0A);                // Carrera Bandera por defecto (L)
      
      // Solo Tarifa 1 Diurnas Habilitada por defecto
      // Sin Sensor de Asiento por defecto
      // Servicio de Taxi por defecto
      (void)PROGRAMADOR_addByte(0x91);                // Varios 1
      
      // Correccion Horaria por defecto = 3
      // Signo de la Correccion Horaria negativo por defecto
      // Punto Decimal por Defecto = 2
      // Hora de GPS por defecto
      (void)PROGRAMADOR_addByte(0x88);                // Varios 2
      
      // Tarifas Nocturnas Deshabilitadas por defecto
      // Tecla Espera Deshabilitada por defecto
      // Espera habilitada por defecto -> Cuando esta por debajo de VelTrans cuenta Tiempo
      // Espera Acumulativa por defecto -> Convertir Tiempo A Pulsos
      // Tarifacion en Pesos por Defecto
      (void)PROGRAMADOR_addByte(0x00);                // Varios 3

      // Por defecto un UNICO chofer, si quiere mas, que los programe.
      // Seleccion manual de tarifa Habilitada
      (void)PROGRAMADOR_addByte(0x52);                // Varios 4

      (void)PROGRAMADOR_addByte(finEEPROM_H);         // Fin de Datos
      (void)PROGRAMADOR_addByte(finEEPROM_L);
    }
    
  
  /* ARMAR STRING DE PARAMETROS DE TARIFA */
  /****************************************/
    static void prgRELOJtarifa_armarDEFAULT (byte nro){
      // Las tarifas se grabaran en EEPROM con el siguiente formato:
      // (recordemos que la EEPROM se divide en bloques de 1 longword, es decir
      //  4bytes)
      //
      //  +----|----|----|----++----|----|----|----++----|----|----|----++----|----|----|----++----|----|----|----++----|----|----|----+
      //  | Nº | DN | tGRACIA || bajBand | distIni || disFich | v.Ficha || t.Ficha | vtFicha || horaIni | vg | DF	|| 0A | xx | xx |	xx |
      //  +----|----|----|----++----|----|----|----++----|----|----|----++----|----|----|----++----|----|----|----++----|----|----|----+
      //
      // Solo tiene sentido guardar el puntero GET la primera vez que agrego un
      // dato, asi cuando extraigo los mismo para grabarlos en el EEPROM, los
      // grabo desde el primero
     *TARIFAS_GET_ptr[nro-1] = PROGRAMADOR_addByte(nro);    // Numero de Tarifa
      
     if (nro > 4){
        (void)PROGRAMADOR_addByte(NOCTURNA);              // Nocturna
     }else{  
        (void)PROGRAMADOR_addByte(DIURNA);                // Diurna
     }
   //TARIFACION NORMAL
       (void)PROGRAMADOR_addByte(0x00);                    // Tiempo Gracia (H)
      (void)PROGRAMADOR_addByte(0x00);                    // Tiempo Gracia (L)
      
      (void)PROGRAMADOR_addByte(0x00);                    // Bajada Bandera (H)
      (void)PROGRAMADOR_addByte(0x00);                    // Bajada Bandera (L)
      
      (void)PROGRAMADOR_addByte(0x00);                    // Distancia Inicial (H)
      (void)PROGRAMADOR_addByte(0x01);                     // Distancia Inicial (L)
      
      (void)PROGRAMADOR_addByte(0x00);                    // Distancia Ficha (H)
      (void)PROGRAMADOR_addByte(0x01);                     // Distancia Ficha (L)
      
      (void)PROGRAMADOR_addByte(0x00);                    // Valor Ficha (H)
      (void)PROGRAMADOR_addByte(0x01);                      // Valor Ficha (L)
      
      (void)PROGRAMADOR_addByte(0x27);                     // Tiempo Ficha (H)
      (void)PROGRAMADOR_addByte(0x0F);                      // Tiempo Ficha (L)
      
      (void)PROGRAMADOR_addByte(0x00);                    // Valor Ficha Tiempo (H)
      (void)PROGRAMADOR_addByte(0X01);                      // Valor Ficha Tiempo (L)

      (void)PROGRAMADOR_addByte(0x00);                    // Hora Inicio (HH)
      (void)PROGRAMADOR_addByte(0x00);                    // Hora Inicio (MM)
      
      (void)PROGRAMADOR_addByte(0xFE);                    // Vigencia (L->D)

      (void)PROGRAMADOR_addByte(finEEPROM_H);             // Fin de Datos
      (void)PROGRAMADOR_addByte(finEEPROM_L);

    }

  /* ARMAR STRING DE PARAMETROS DE EQUIVALENCIA EN PESOS */
  /*******************************************************/
    static void prgRELOJeqPesos_armarDEFAULT (void){
      // La equivalencia en pesos se grabara en EEPROM con el siguiente formato:
      // (recordemos que la EEPROM se divide en bloques de 1 longword, es decir
      //  4bytes)
      //
      //  +----|----|----|----++----|----|----|----++----|----|----|----++----|----|----|----+
      //  | hb | bajBand | v.Ficha  | t.Ficha | bajBand  | v.Ficha | t.Ficha  | DF   0A		xx |
      //  +----|----|----|----++----|----|----|----++----|----|----|----++----|----|----|----+
      //
      // Solo tiene sentido guardar el puntero GET la primera vez que agrego un
      // dato, asi cuando extraigo los mismo para grabarlos en el EEPROM, los
      // grabo desde el primero
      progRELOJ_EQ_GET = PROGRAMADOR_addByte(0x00);   // Habilitacion
      (void)PROGRAMADOR_addByte(0x00);                // [DIURNA] Bajada Bandera (H)
      (void)PROGRAMADOR_addByte(0x00);                // [DIURNA] Bajada Bandera (L)
      (void)PROGRAMADOR_addByte(0x00);                // [DIURNA] Valor Ficha (H)
      (void)PROGRAMADOR_addByte(0x00);                // [DIURNA] Valor Ficha (L)
      (void)PROGRAMADOR_addByte(0x00);                // [DIURNA] Valor Ficha Tiempo (H)
      (void)PROGRAMADOR_addByte(0x00);                // [DIURNA] Valor Ficha Tiempo (L)
      (void)PROGRAMADOR_addByte(0x00);                // [NOCTURNA] Bajada Bandera (H)
      (void)PROGRAMADOR_addByte(0x00);                // [NOCTURNA] Bajada Bandera (L)
      (void)PROGRAMADOR_addByte(0x00);                // [NOCTURNA] Valor Ficha (H)
      (void)PROGRAMADOR_addByte(0x00);                // [NOCTURNA] Valor Ficha (L)
      (void)PROGRAMADOR_addByte(0x00);                // [NOCTURNA] Valor Ficha Tiempo (H)
      (void)PROGRAMADOR_addByte(0x00);                // [NOCTURNA] Valor Ficha Tiempo (L)

      (void)PROGRAMADOR_addByte(finEEPROM_H);         // Fin de Datos
      (void)PROGRAMADOR_addByte(finEEPROM_L);
    }    

  
  /* ARMAR STRING DE PARAMETROS DE CALENDARIO */
  /********************************************/
    static void prgRELOJcalendar_armarDEFAULT (void){
      // El calendario se graba en EEPROM con el siguiente formato:
      // (recordemos que la EEPROM se divide en bloques de 1 longword, es decir
      //  4bytes)
      //
      //  +----|----|----|----++
      //  | DD   MM | DD   MM || son <cant_Feriados> feriados, o sea <cant_Feriados> DD-MM
      //  +----|----|----|----++
      //
      // Solo tiene sentido guardar el puntero GET la primera vez que agrego un
      // dato, asi cuando extraigo los mismo para grabarlos en el EEPROM, los
      // grabo desde el primero
      byte i;
      
      progRELOJ_CALEND_GET = PROGRAMADOR_addByte(0x00);   // DIA 0
      (void)PROGRAMADOR_addByte(0x00);                    // MES 0
      
      
      for (i=1; i<cant_Feriados; i++){
        (void)PROGRAMADOR_addByte(0x00);          // DIA 1 - 19
        (void)PROGRAMADOR_addByte(0x00);          // MES 1 - 19
      }
      
      (void)PROGRAMADOR_addByte(finEEPROM_H);     // Fin de Datos
      (void)PROGRAMADOR_addByte(finEEPROM_L);
    }
  

/*********************************************************************************************/
/* RUTINAS DE EEPROM - PARAMETROS COMUNES */
/******************************************/

  /* INICIAR GRABACION DE PARAMETROS COMUNES EN EEPROM */
  /*****************************************************/
    void PROG_RELOJcomun_to_EEPROM (byte iniGrabacion){
      // Da la orden de grabacion en EEPROM de los parametros de RELOJ COMUNES
      prgRELOJ_COMUNES_EEPROM_F = 1;    // Levanto bandera de grabacion de programacion de parametros de reloj comunes en eeprom
      
    }


  /* GRABACION DE PARAMETROS COMUNES EN EEPROM */
  /*********************************************/
    tEEPROM_ERROR PROG_RELOJcomunes_grabarEEPROM (void){
      tEEPROM_ERROR error;
      dword* EEPROM_ptr;
      uint8_t* ptrAUX;
      uint16_t valorCRC;
      byte EEPROM_buffer[EEPROMsize_PRG_relojCOMUN];
      uint16_t long_relojCOMUN_DATA;
      
      error = EEPROM_ERROR_MASK;
      if (prgRELOJ_COMUNES_EEPROM_F){
        prgRELOJ_COMUNES_EEPROM_F = 0;  // Bajo Bandera
        
        armarBuffer_progCOMUNES_EEPROM(EEPROM_buffer);  // Armo buffer de grabación según formato

        if (ELIMINAR_REPORTES){
          	    EEPROM_write(EEPROM_PRIMER_ENCENDIDO, 0xffff); // Grabar Word en EEPROM
               // EEPROM_ReadBuffer(&PRUEBA,EEPROM_PRIMER_ENCENDIDO,SIZE_EEPROM_PRIMER_ENCENDIDO);
        }

        //calculo de crc de datos recibidos
		long_relojCOMUN_DATA = EEPROMsize_PRG_relojCOMUN - (sizeof(((tPARAM_RELOJ_COMUNES *)0)->checksum) + sizeof(((tPARAM_RELOJ_COMUNES *)0)->finDATA));
		valorCRC = GetCrc16(&EEPROM_buffer, long_relojCOMUN_DATA );

		//pongo crc en buffer de grabacion
		ptrAUX = &valorCRC;
		EEPROM_buffer[EEPROMsize_PRG_relojCOMUN-4] = *(ptrAUX+1);
		EEPROM_buffer[EEPROMsize_PRG_relojCOMUN-3] = *(ptrAUX+0);
		EEPROM_buffer[EEPROMsize_PRG_relojCOMUN-2] = finEEPROM_H;              // Fin de Datos
		EEPROM_buffer[EEPROMsize_PRG_relojCOMUN-1] = finEEPROM_L;

        grabar_buffer_EEPROM((uint16_t*)EEPROM_buffer, ADDRESS_PROG_relojCOMUN, SIZE_PROG_relojCOMUN);
        error = chkCRC_EnEEPROM(ADDRESS_PROG_relojCOMUN, EEPROMsize_PRG_relojCOMUN);

        if(error == EEPROM_OK){
            grabar_buffer_EEPROM((uint16_t*)EEPROM_buffer, ADDRESS_PROG_relojCOMUN_bck1, SIZE_PROG_relojCOMUN);
            error = chkCRC_EnEEPROM(ADDRESS_PROG_relojCOMUN_bck1, EEPROMsize_PRG_relojCOMUN);
        }
        if(error == EEPROM_OK){
            grabar_buffer_EEPROM((uint16_t*)EEPROM_buffer, ADDRESS_PROG_relojCOMUN_bck2, SIZE_PROG_relojCOMUN);
            error = chkCRC_EnEEPROM(ADDRESS_PROG_relojCOMUN_bck2, EEPROMsize_PRG_relojCOMUN);
        }
        if(error == EEPROM_OK){
            grabar_buffer_EEPROM((uint16_t*)EEPROM_buffer, ADDRESS_PROG_relojCOMUN_bck3, SIZE_PROG_relojCOMUN);
            error = chkCRC_EnEEPROM(ADDRESS_PROG_relojCOMUN_bck3, EEPROMsize_PRG_relojCOMUN);
        }
        waitToTx_upDateSuccess = 0;
        //grabacion correcta ?
    	if(error == EEPROM_OK){
    		waitToTx_upDateSuccess = 1;
    	}else{
    		//ring buzzer error
    	}

      }else{
    	  error = EEPROM_OK;
      }
      
      return(error);
    }    
    
    
  /* ARMAR BUFFER DE GRABACION DE PARAMETROS COMUNES */
  /***************************************************/
    static void armarBuffer_progCOMUNES_EEPROM (byte* RAMbuffer){
      // Los parametros comunes se grabaran en EEPROM con el siguiente formato:
      // (recordemos que la EEPROM se divide en bloques de 1 longword, es decir
      //  4bytes)
      //
      //  +----|----|----|----++----|----|----|----++----|----|----|----+
      //  | constan |carr.band|| v1	| v2 | v3 | v4 ||	DF | 0A	| xx	 xx	|
      //  +----|----|----|----++----|----|----|----++----|----|----|----+
      byte* PUTptr;
      byte** GETptr_ptr;
      tRLJ_V4 varios4;
      uint8_t* ptrAUX;
      
      PUTptr = RAMbuffer;                               // Ingreso datos desde inicio de Buffer
      GETptr_ptr = &progRELOJ_COMUN_GET;                // Puntero a inicio de datos de RELOJ COMUN



      *PUTptr++ = PROGRAMADOR_getProgData(GETptr_ptr);  // Extraigo PULSOSxKM (H) y avanzo puntero GET
      *PUTptr++ = PROGRAMADOR_getProgData(GETptr_ptr);  // Extraigo PULSOSxKM (L) y avanzo puntero GET

      //guardo constante en flash para prueba en rivera
	  //ptrAUX = &PULSOS_x_KM;
      //*(ptrAUX+1) = RAMbuffer[0];
      //*(ptrAUX+0) = RAMbuffer[1];
      //FLASH_updateSector((word*) &FLASH_PULSOS_x_KM, (byte*) &PULSOS_x_KM, (word) 2);

      *PUTptr++ = PROGRAMADOR_getProgData(GETptr_ptr);  // Extraigo CarreraBandera (H) y avanzo puntero GET
      *PUTptr++ = PROGRAMADOR_getProgData(GETptr_ptr);  // Extraigo CarreraBandera (L) y avanzo puntero GET
      *PUTptr++ = PROGRAMADOR_getProgData(GETptr_ptr);  // Extraigo Varios 1 y avanzo puntero GET
      *PUTptr++ = PROGRAMADOR_getProgData(GETptr_ptr);  // Extraigo Varios 2 y avanzo puntero GET
      *PUTptr++ = PROGRAMADOR_getProgData(GETptr_ptr);  // Extraigo Varios 3 y avanzo puntero GET
      
      // En VARIOS 4 tengo la indicacion de ELIMINAR REPORTES, esto no debe ser grabado en EEPROM
      varios4.Byte = PROGRAMADOR_getProgData(GETptr_ptr);    // Extraigo Varios 4 y avanzo puntero GET
      if (varios4.Bits.deleteReportes){
        varios4.Bits.deleteReportes = 0;    // Bajo Bandera
        ELIMINAR_REPORTES = 1;
  	   }else{
        ELIMINAR_REPORTES = 0;
      }
      
      *PUTptr++ = varios4.Byte;             // Agrego Varios 4

    //  *PUTptr++ = finEEPROM_H;              // Fin de Datos
    //  *PUTptr++ = finEEPROM_L;
    }


/*********************************************************************************************/
/* RUTINAS DE EEPROM - TARIFAS */
/*******************************/

  /* OBTENER PUNTERO A TARIFA EN EEPROM */
  /**************************************/
/*
    byte* PROG_RELOJtarifa_getEEPROM_ptr(byte nro){
      // Nro de tarifa de 1-8

    	if(nro<5){
    		return((byte*) (ADDRESS_PROG_relojT1D) + ((SIZE_PROG_relojT1D) * (nro - 1)));
    	}else{
    		return((byte*) (ADDRESS_PROG_relojT1N) + ((SIZE_PROG_relojT1N) * (nro - 4 - 1)));
    	}
    }


    byte* pruebaPROG_RELOJtarifa_getEEPROM_ptr(byte nro){
      // Nro de tarifa de 1-8

    	if(nro<5){
    		return((byte*) (ADDRESS_PROG1 + SIZE_PROG_relojCOMUN) + ((SIZE_PROG_relojT1D) * (nro - 1)));
    	}else{
    		return((byte*) (ADDRESS_PROG1 + 128) + ((SIZE_PROG_relojT1N) * (nro - 4 - 1)));
    	}
    }

*/
    byte* getDir_tarifaX_BlockProg(byte nro, byte* dirBlockProg){
         // Nro de tarifa de 1-8

    	byte* addr;

    	addr = dirBlockProg;

       	if(nro<5){
       		return((byte*) ( addr + SIZE_PROG_relojCOMUN) + ((SIZE_PROG_relojT1D) * (nro - 1)));
       	}else{
       		return((byte*) (addr + 128) + ((SIZE_PROG_relojT1N) * (nro - 4 - 1)));
       	}
       }



  /* INICIAR GRABACION DE TARIFA EN EEPROM */
  /*****************************************/
    void PROG_RELOJtarifa_to_EEPROM (byte nro, byte iniGrabacion){
      // Da la orden de grabacion en EEPROM de la Tarifa correspondiente
      byte mask;
      
      mask = (byte) potencia(2,nro-1);  // Calculo mascara de grabcion
      prgEEPROM_F2 |= mask;             // Levanto bandera de grabacion de programacion de tarifa 1 diurna en eeprom  
      
    }

  
  /* GRABACION DE TARIFA EN EEPROM */
  /*********************************/

   /*
    tEEPROM_ERROR PROG_RELOJtarifa_grabarEEPROM (byte nro, byte* addressTarifa){
      tEEPROM_ERROR error;
      byte mask;
      dword* EEPROM_ptr;
      
      uint8_t* ptrAUX;
      uint16_t valorCRC;
      byte EEPROM_buffer[EEPROMsize_PRG_relojTARIFA];

      byte EEPROM_buffer_test[EEPROMsize_PRG_relojTARIFA];

      uint16_t long_relojTARIFA_DATA;
      
     // error = EEPROM_OK;                // Asumo que no hay error
      error = EEPROM_ERROR_MASK;                //si no entra en el if el nro de tarifa no esta programada o hay algun error
      mask = (byte) potencia(2,nro - 1);
      if ((prgEEPROM_F2 & mask) == mask){
        prgEEPROM_F2 &= ~mask;          // Bajo Bandera
        
        EEPROM_ptr = (dword*) (getDir_tarifaX_BlockProg(nro, addressTarifa));
        //EEPROM_ptr = (dword*) (prueba1PROG_RELOJtarifa_getEEPROM_ptr(nro, (byte*)ADDRESS_PROG1));
        //EEPROM_ptr = (dword*) (pruebaPROG_RELOJtarifa_getEEPROM_ptr(nro));
        //EEPROM_ptr = (dword*) (PROG_RELOJtarifa_getEEPROM_ptr(nro));

        armarBuffer_progTARIFA_EEPROM(nro, EEPROM_buffer);                    // Armo buffer de grabación según formato

        //calculo de crc de datos recibidos
		long_relojTARIFA_DATA = EEPROMsize_PRG_relojTARIFA - (sizeof(((tPARAM_RELOJ_TARIFA *)0)->checksum) + sizeof(((tPARAM_RELOJ_TARIFA *)0)->finDATA));
		valorCRC = GetCrc16(&EEPROM_buffer, long_relojTARIFA_DATA );

		//pongo crc en buffer de grabacion
		ptrAUX = &valorCRC;
		EEPROM_buffer[EEPROMsize_PRG_relojTARIFA-4] = *(ptrAUX+1);
		EEPROM_buffer[EEPROMsize_PRG_relojTARIFA-3] = *(ptrAUX+0);
		EEPROM_buffer[EEPROMsize_PRG_relojTARIFA-2] = finEEPROM_H;              // Fin de Datos
		EEPROM_buffer[EEPROMsize_PRG_relojTARIFA-1] = finEEPROM_L;
        //grabar
        grabar_buffer_EEPROM((uint16_t*) EEPROM_buffer, (uint16_t*) EEPROM_ptr, EEPROMsize_PRG_relojTARIFA);
		error = chkCRC_EnEEPROM(EEPROM_ptr, EEPROMsize_PRG_relojTARIFA);
		//para prueba
	    EEPROM_ReadBuffer(&EEPROM_buffer_test, EEPROM_ptr, sizeof(tPARAM_RELOJ_TARIFA));
	    readTarifa(nro, addressTarifa);

        //grabacion correcta ?
		if(error == EEPROM_OK){
			waitToTx_upDateSuccess = 1;
		}else{
			waitToTx_upDateSuccess = 0;
		}
        
      }
      
      return(error);
    }    

*/

    tEEPROM_ERROR PROG_RELOJtarifa_grabarEEPROM (byte nro){
       tEEPROM_ERROR error;
       byte mask;
       dword* EEPROM_ptr;

       uint8_t* ptrAUX;
       uint16_t valorCRC;
       byte EEPROM_buffer[EEPROMsize_PRG_relojTARIFA];

       byte EEPROM_buffer_test[EEPROMsize_PRG_relojTARIFA];

       uint16_t long_relojTARIFA_DATA;

      // error = EEPROM_OK;                // Asumo que no hay error
       error = EEPROM_ERROR_MASK;                //si no entra en el if el nro de tarifa no esta programada o hay algun error
       mask = (byte) potencia(2,nro - 1);
       if ((prgEEPROM_F2 & mask) == mask){

    	   // Bajo Bandera de oden de grabacion
    	  prgEEPROM_F2 &= ~mask;

         // Armo buffer de grabación según formato
         armarBuffer_progTARIFA_EEPROM(nro, EEPROM_buffer);
         //calculo de crc de datos recibidos
 		long_relojTARIFA_DATA = EEPROMsize_PRG_relojTARIFA - (sizeof(((tPARAM_RELOJ_TARIFA *)0)->checksum) + sizeof(((tPARAM_RELOJ_TARIFA *)0)->finDATA));
 		valorCRC = GetCrc16(&EEPROM_buffer, long_relojTARIFA_DATA );
 		//pongo crc en buffer de grabacion
 		ptrAUX = &valorCRC;
 		EEPROM_buffer[EEPROMsize_PRG_relojTARIFA-4] = *(ptrAUX+1);
 		EEPROM_buffer[EEPROMsize_PRG_relojTARIFA-3] = *(ptrAUX+0);
 		EEPROM_buffer[EEPROMsize_PRG_relojTARIFA-2] = finEEPROM_H;              // Fin de Datos
 		EEPROM_buffer[EEPROMsize_PRG_relojTARIFA-1] = finEEPROM_L;

 		//grabar tarifa n en distintos sectores de eeprom
        EEPROM_ptr = (uint16_t*)(getDir_tarifaX_BlockProg(nro, ADDRESS_PROG1));
 		grabar_buffer_EEPROM((uint16_t*) EEPROM_buffer, EEPROM_ptr, EEPROMsize_PRG_relojTARIFA);
 		error = chkCRC_EnEEPROM(EEPROM_ptr, EEPROMsize_PRG_relojTARIFA);

 		//para prueba
		//readTarifa(nro, EEPROM_ptr);
 		//grabacion correcta en sector 1?
  		if(error == EEPROM_OK){
  			EEPROM_ptr = (uint16_t*)(getDir_tarifaX_BlockProg(nro, ADDRESS_PROG2));
  			grabar_buffer_EEPROM((uint16_t*) EEPROM_buffer, EEPROM_ptr, EEPROMsize_PRG_relojTARIFA);
  	 		error = chkCRC_EnEEPROM(EEPROM_ptr, EEPROMsize_PRG_relojTARIFA);
  			//para prueba
  		    //readTarifa(nro, EEPROM_ptr);
  		}
        //grabacion correcta en sector 2?
  		if(error == EEPROM_OK){
  			EEPROM_ptr = (uint16_t*)(getDir_tarifaX_BlockProg(nro, ADDRESS_PROG3));
  	 		grabar_buffer_EEPROM((uint16_t*) EEPROM_buffer, EEPROM_ptr, EEPROMsize_PRG_relojTARIFA);
  	 		error = chkCRC_EnEEPROM(EEPROM_ptr, EEPROMsize_PRG_relojTARIFA);
  			//para prueba
  		    //readTarifa(nro, EEPROM_ptr);
  		}

  		//grabacion correcta en sector 3?
  		if(error == EEPROM_OK){
  			EEPROM_ptr = (uint16_t*)(getDir_tarifaX_BlockProg(nro, ADDRESS_PROG4));
  	 		grabar_buffer_EEPROM((uint16_t*) EEPROM_buffer, EEPROM_ptr, EEPROMsize_PRG_relojTARIFA);
  	 		error = chkCRC_EnEEPROM(EEPROM_ptr, EEPROMsize_PRG_relojTARIFA);
  			//para prueba
  		    //readTarifa(nro, EEPROM_ptr);
  		}

         //grabacion correcta ?
 		if(error == EEPROM_OK){
 			waitToTx_upDateSuccess = 1;
 		}else{
 			waitToTx_upDateSuccess = 0;
 		}

       }else{
    	   error = EEPROM_OK;
       }

       return(error);
     }

		//para prueba
//	    EEPROM_ReadBuffer(&EEPROM_buffer_test, EEPROM_ptr, sizeof(tPARAM_RELOJ_TARIFA));
//	    readTarifa(nro, addressTarifa);


  /* ARMAR BUFFER DE GRABACION DE TARIFA */
  /***************************************/
     //PRUEBA
     byte** GET_PTR;
    static void armarBuffer_progTARIFA_EEPROM (byte nro, byte* RAMbuffer){
      // Las tarifas se grabaran en EEPROM con el siguiente formato:
      // (recordemos que la EEPROM se divide en bloques de 1 longword, es decir
      //  4bytes)
      //
      //  +----|----|----|----++----|----|----|----++----|----|----|----++----|----|----|----++----|----|----|----++----|----|----|----+
      //  | Nº | DN | tGRACIA || bajBand | distIni || disFich | v.Ficha || t.Ficha | vtFicha || horaIni | vg | DF	|| 0A | xx | xx |	xx |
      //  +----|----|----|----++----|----|----|----++----|----|----|----++----|----|----|----++----|----|----|----++----|----|----|----+
      byte* PUTptr;
      byte** GETptr_ptr;
      
      PUTptr = RAMbuffer;                               // Ingreso datos desde inicio de Buffer
      //PRUEBA
      GET_PTR    = TARIFAS_GET_ptr[nro-1];
      //*******************************
      GETptr_ptr = TARIFAS_GET_ptr[nro-1];              // Puntero a inicio de datos de RELOJ TARIFA <nro>
      
      *PUTptr++ = PROGRAMADOR_getProgData(GETptr_ptr);  // Extraigo Numero y avanzo puntero GET
      *PUTptr++ = PROGRAMADOR_getProgData(GETptr_ptr);  // Extraigo DIA / NOCHE y avanzo puntero GET
      *PUTptr++ = PROGRAMADOR_getProgData(GETptr_ptr);  // Extraigo Tiempo Gracia (H) y avanzo puntero GET
      *PUTptr++ = PROGRAMADOR_getProgData(GETptr_ptr);  // Extraigo Tiempo Gracia (L) y avanzo puntero GET
      *PUTptr++ = PROGRAMADOR_getProgData(GETptr_ptr);  // Extraigo Bajada Bandera (H) y avanzo puntero GET
      *PUTptr++ = PROGRAMADOR_getProgData(GETptr_ptr);  // Extraigo Bajada Bandera (L) y avanzo puntero GET
      *PUTptr++ = PROGRAMADOR_getProgData(GETptr_ptr);  // Extraigo Distancia Inicial (H) y avanzo puntero GET
      *PUTptr++ = PROGRAMADOR_getProgData(GETptr_ptr);  // Extraigo Distancia Inicial (L) y avanzo puntero GET
      *PUTptr++ = PROGRAMADOR_getProgData(GETptr_ptr);  // Extraigo Distancia Ficha (H) y avanzo puntero GET
      *PUTptr++ = PROGRAMADOR_getProgData(GETptr_ptr);  // Extraigo Distancia Ficha (L) y avanzo puntero GET
      *PUTptr++ = PROGRAMADOR_getProgData(GETptr_ptr);  // Extraigo Valor Ficha (H) y avanzo puntero GET
      *PUTptr++ = PROGRAMADOR_getProgData(GETptr_ptr);  // Extraigo Valor Ficha (L) y avanzo puntero GET
      *PUTptr++ = PROGRAMADOR_getProgData(GETptr_ptr);  // Extraigo Tiempo Ficha (H) y avanzo puntero GET
      *PUTptr++ = PROGRAMADOR_getProgData(GETptr_ptr);  // Extraigo Tiempo Ficha (L) y avanzo puntero GET
      *PUTptr++ = PROGRAMADOR_getProgData(GETptr_ptr);  // Extraigo Valor Ficha Tiempo (H) y avanzo puntero GET
      *PUTptr++ = PROGRAMADOR_getProgData(GETptr_ptr);  // Extraigo Valor Ficha Tiempo (L) y avanzo puntero GET
      *PUTptr++ = PROGRAMADOR_getProgData(GETptr_ptr);  // Extraigo Hora Inicio (HH) y avanzo puntero GET
      *PUTptr++ = PROGRAMADOR_getProgData(GETptr_ptr);  // Extraigo Hora Inicio (MM) y avanzo puntero GET
      *PUTptr++ = PROGRAMADOR_getProgData(GETptr_ptr);  // Extraigo Vigencia y avanzo puntero GET

    
      //*PUTptr++ = finEEPROM_H;              // Fin de Datos
      //*PUTptr++ = finEEPROM_L;
      
    }



  /* EXTRAER TARIFA SEGUN NUMERO */
  /*******************************/
    tTARIFA prgRELOJ_getTarifa (byte nro){
        dword* EEPROM_ptr;
      
        //EEPROM_ptr = (dword*) (&EEPROM_PROG_relojT1D) + ((EEPROMsize_PRG_relojTARIFA/4) * (nro - 1));

        switch(nro){
        				case  1:
        					EEPROM_ptr = (dword*) (&EEPROM_PROG_relojT1D);
        					break;
        				case  2 :
        					EEPROM_ptr = (dword*) (&EEPROM_PROG_relojT2D);
        					break;
        				case 3 :
        					EEPROM_ptr = (dword*) (&EEPROM_PROG_relojT3D);
        					break;
        				case 4 :
        					EEPROM_ptr = (dword*) (&EEPROM_PROG_relojT4D);
        					break;
        				case  5:
        					EEPROM_ptr = (dword*) (&EEPROM_PROG_relojT1N);
        					break;
        				case  6 :
        					EEPROM_ptr = (dword*) (&EEPROM_PROG_relojT2N);
        					break;
        				case 7 :
        					EEPROM_ptr = (dword*) (&EEPROM_PROG_relojT3N);
        					break;
        				case 8 :
        					EEPROM_ptr = (dword*) (&EEPROM_PROG_relojT4N);
        					break;

        				default:
        					EEPROM_ptr = (dword*) (&EEPROM_PROG_relojT1D);
        					break;
      }


      return(*((tTARIFA*)EEPROM_ptr));
    }

 
  
  /* EXTRAER PUNTERO A TARIFA SEGUN NUMERO */
  /*****************************************/
    tTARIFA* prgRELOJ_getTarifa_pointer (byte nro){
        dword* EEPROM_ptr;
      
       // EEPROM_ptr = (dword*) (&EEPROM_PROG_relojT1D) + ((EEPROMsize_PRG_relojTARIFA/4) * (nro - 1));

        switch(nro){
			case  1:
				EEPROM_ptr = (dword*) (&EEPROM_PROG_relojT1D);
				break;
			case  2 :
				EEPROM_ptr = (dword*) (&EEPROM_PROG_relojT2D);
				break;
			case 3 :
				EEPROM_ptr = (dword*) (&EEPROM_PROG_relojT3D);
				break;
			case 4 :
				EEPROM_ptr = (dword*) (&EEPROM_PROG_relojT4D);
				break;
			case  5:
				EEPROM_ptr = (dword*) (&EEPROM_PROG_relojT1N);
				break;
			case  6 :
				EEPROM_ptr = (dword*) (&EEPROM_PROG_relojT2N);
				break;
			case 7 :
				EEPROM_ptr = (dword*) (&EEPROM_PROG_relojT3N);
				break;
			case 8 :
				EEPROM_ptr = (dword*) (&EEPROM_PROG_relojT4N);
				break;

			default:
				break;
        }

      return((tTARIFA*)EEPROM_ptr);
    } 


  /* DETERMINAR CANTIDAD DE TARIFAS DIURNAS HABILITADAS */
  /******************************************************/
    byte prgRELOJ_determineCantTarifasD_AUTOMATICA (void){
      byte cantTarifas;
      byte exit;
      cantTarifas = 0;
      exit=0;
      if (T4D_hab){
        if(EEPROM_PROG_relojT4D.tarifa.vigencia.Byte!=0){
        	cantTarifas = 4;
        	exit=1;
        }
      }
      if (T3D_hab && !exit){
    	  if(EEPROM_PROG_relojT3D.tarifa.vigencia.Byte!=0){
        	  cantTarifas = 3;
        	  exit=1;
          }
      }
      if (T2D_hab && !exit){
    	  if(EEPROM_PROG_relojT2D.tarifa.vigencia.Byte!=0){
    	        cantTarifas = 2;
    	        exit=1;
          }
      }
      if(cantTarifas == 0){
   	      cantTarifas = 1;
      }

      return(cantTarifas);
    }


    byte prgRELOJ_determineCantTarifasD_MANUAL (void){

    	byte cantTarifas;

		  if (T4D_hab){
			cantTarifas = 4;
		  }else if (T3D_hab){
			cantTarifas = 3;
		  }else if (T2D_hab){
			cantTarifas = 2;
		  }else{
			cantTarifas = 1;
		  }

		  return(cantTarifas);
}



  /* DETERMINAR CANTIDAD DE TARIFAS NOCTURNAS HABILITADAS */
  /********************************************************/
    byte prgRELOJ_determineCantTarifasN_AUTOMATICA (void){
      byte cantTarifas;
      
      //Edit: Corregida por rai el21/08/2018
      //Por ej si se habilita tarifa 3 Diurna y Nocturna con vigencias todos los dias
      //y no se habilitaba la tarifa nocturna 1 y 2. No pasaba a la tarifa nocturna 3
      //(siempre pasaba a ala tarifa durna 3)

      cantTarifas = 0;
      if (T1N_hab){
        if(EEPROM_PROG_relojT1N.tarifa.vigencia.Byte!=0){
           	cantTarifas = 1;
         }
      }

      if (T2N_hab){
          if(EEPROM_PROG_relojT2N.tarifa.vigencia.Byte!=0){
             	cantTarifas = 2;
           }
      }

      if (T3N_hab){
          if(EEPROM_PROG_relojT3N.tarifa.vigencia.Byte!=0){
             	cantTarifas = 3;
           }
      }

      if (T4N_hab){
          if(EEPROM_PROG_relojT4N.tarifa.vigencia.Byte!=0){
             	cantTarifas = 4;
           }
      }
      
      return(cantTarifas);
    }

    /* DETERMINAR CANTIDAD DE TARIFAS NOCTURNAS HABILITADAS */
      /********************************************************/
        byte prgRELOJ_determineCantTarifasN_MANUAL (void){
          byte cantTarifas;

          cantTarifas = 0;
          if (T4N_hab){
            cantTarifas++;
          }
          if (T3N_hab){
            cantTarifas++;
          }
          if (T2N_hab){
            cantTarifas++;
          }
          if (T1N_hab){
            cantTarifas++;
          }

          return(cantTarifas);
        }


  /* DETERMINAR SI LA TARIFA X ESTA HABILITADA */
  /*********************************************/
    byte prgRELOJ_determineTarifaHab (byte nroTarifa){
      // Tarifas de 1 a 8
      byte hab;
      
      hab = 0;
      if (nroTarifa < tarifa1N){
        // DIURNA
        if (nroTarifa <= prgRELOJ_determineCantTarifasD_MANUAL()){
          hab = 1;
        }
      }else{
        if ((nroTarifa == tarifa1N) && T1N_hab){
          hab = 1;
        }
        
        if ((nroTarifa == tarifa2N) && T2N_hab){
          hab = 1;
        }
        
        if ((nroTarifa == tarifa3N) && T3N_hab){
          hab = 1;
        }
        
        if ((nroTarifa == tarifa4N) && T4N_hab){
          hab = 1;
        }
      }
      
      return(hab);
    }

/*********************************************************************************************/
/* RUTINAS DE EEPROM - EQUIVALENCIA EN PESOS */
/*********************************************/

  /* INICIAR GRABACION DE EQUIVALENCIA EN PESOS EN EEPROM */
  /********************************************************/
    void PROG_RELOJeqPesos_to_EEPROM (byte iniGrabacion){
      // Da la orden de grabacion en EEPROM de la Tarifa correspondiente
      prgRELOJ_EQPESOS_EEPROM_F = 1;    // Levanto bandera de grabacion de programacion de eq pesos en eeprom  
      
    }

  /* GRABACION DE EQUIVALENCIA EN PESOS EN EEPROM */
  /************************************************/
    tEEPROM_ERROR PROG_RELOJeqPesos_grabarEEPROM (void){
      tEEPROM_ERROR error;
      dword* EEPROM_ptr;
      byte EEPROM_buffer[EEPROMsize_PRG_relojEqPESOS];
      uint8_t* ptrAUX;
	  uint16_t valorCRC;
	  uint16_t long_relojEqPESOS_DATA;

	  //test
	 // byte EEPROM_buffer_test[EEPROMsize_PRG_relojEqPESOS];

      //error = EEPROM_OK;                // Asumo que no hay error
      error = EEPROM_ERROR_MASK;                //si no entra en el if el nro de tarifa no esta programada o hay algun error
      if (prgRELOJ_EQPESOS_EEPROM_F){
    	  // Bajo Bandera que da orden de grababacion
    	prgRELOJ_EQPESOS_EEPROM_F = 0;
        
        // Armo buffer de grabación según formato
        armarBuffer_progEqPESOS_EEPROM(EEPROM_buffer);

        //calculo de crc de datos recibidos
		long_relojEqPESOS_DATA = EEPROMsize_PRG_relojEqPESOS - (sizeof(((tPARAM_RELOJ_EQPESOS *)0)->checksum) + sizeof(((tPARAM_RELOJ_EQPESOS *)0)->finDATA));
		valorCRC = GetCrc16(&EEPROM_buffer, long_relojEqPESOS_DATA);
		//pongo crc en buffer de grabacion
		ptrAUX = &valorCRC;
		EEPROM_buffer[EEPROMsize_PRG_relojEqPESOS-4] = *(ptrAUX+1);
		EEPROM_buffer[EEPROMsize_PRG_relojEqPESOS-3] = *(ptrAUX+0);
		EEPROM_buffer[EEPROMsize_PRG_relojEqPESOS-2] = finEEPROM_H;              // Fin de Datos
		EEPROM_buffer[EEPROMsize_PRG_relojEqPESOS-1] = finEEPROM_L;

		//grabar eqPesos en los distintos sectores de grabacion
		error = grabar_buffer_EEPROM((uint16_t*)(&EEPROM_buffer), ADDRESS_PROG_relojEqPESOS, EEPROMsize_PRG_relojEqPESOS);
		//para prueba
		byte EEPROM_buffer_test[EEPROMsize_PRG_relojEqPESOS];
		EEPROM_ReadBuffer(&EEPROM_buffer_test, ADDRESS_PROG_relojEqPESOS, EEPROMsize_PRG_relojEqPESOS);

        //grabacion correcta ?
		if(error == EEPROM_OK){
			error = chkCRC_EnEEPROM(ADDRESS_PROG_relojEqPESOS, EEPROMsize_PRG_relojEqPESOS);
		}

		if(error == EEPROM_OK){
			error = grabar_buffer_EEPROM((uint16_t*)(&EEPROM_buffer), ADDRESS_PROG_relojEqPESOS_bck1, EEPROMsize_PRG_relojEqPESOS);
			//grabacion correcta ?
			if(error == EEPROM_OK){
				error = chkCRC_EnEEPROM(ADDRESS_PROG_relojEqPESOS_bck1, EEPROMsize_PRG_relojEqPESOS);
			}
		}

		if(error == EEPROM_OK){
			error = grabar_buffer_EEPROM((uint16_t*)(&EEPROM_buffer), ADDRESS_PROG_relojEqPESOS_bck2, EEPROMsize_PRG_relojEqPESOS);
			//grabacion correcta ?
			if(error == EEPROM_OK){
				error = chkCRC_EnEEPROM(ADDRESS_PROG_relojEqPESOS_bck2, EEPROMsize_PRG_relojEqPESOS);
			}
		}

		if(error == EEPROM_OK){
			error = grabar_buffer_EEPROM((uint16_t*)(&EEPROM_buffer), ADDRESS_PROG_relojEqPESOS_bck3, EEPROMsize_PRG_relojEqPESOS);
			//grabacion correcta ?
			if(error == EEPROM_OK){
				error = chkCRC_EnEEPROM(ADDRESS_PROG_relojEqPESOS_bck3, EEPROMsize_PRG_relojEqPESOS);
			}
		}

      }else{
    	  error = EEPROM_OK;
      }
      return(error);
    }    




  /* ARMAR BUFFER DE GRABACION DE EQUIVALENCIA EN PESOS */
  /******************************************************/
    static void armarBuffer_progEqPESOS_EEPROM (byte* RAMbuffer){
      // La equivalencia en pesos se grabara en EEPROM con el siguiente formato:
      // (recordemos que la EEPROM se divide en bloques de 1 longword, es decir
      //  4bytes)
      //
      //  +----|----|----|----++----|----|----|----++----|----|----|----++----|----|----|----+
      //  | hb | bajBand | v.Ficha  | t.Ficha | bajBand  | v.Ficha | t.Ficha  | DF   0A		xx |
      //  +----|----|----|----++----|----|----|----++----|----|----|----++----|----|----|----+
      byte* PUTptr;
      byte** GETptr_ptr;
      
      PUTptr = RAMbuffer;                               // Ingreso datos desde inicio de Buffer
      GETptr_ptr = &progRELOJ_EQ_GET;                   // Puntero a inicio de datos de RELOJ COMUN
      *PUTptr++ = PROGRAMADOR_getProgData(GETptr_ptr);  // Extraigo Habilitacion y avanzo puntero GET
      *PUTptr++ = PROGRAMADOR_getProgData(GETptr_ptr);  // Guardo BAJADA DE BANDERA HIGH
      *PUTptr++ = PROGRAMADOR_getProgData(GETptr_ptr);  // Guardo BAJADA DE BANDERA LOW
      *PUTptr++ = PROGRAMADOR_getProgData(GETptr_ptr);  // Guardo VALOR FICHA HIGH
      *PUTptr++ = PROGRAMADOR_getProgData(GETptr_ptr);  // Guardo VALOR FICHA LOW
      *PUTptr++ = PROGRAMADOR_getProgData(GETptr_ptr);  // Guardo VALOR FICHA TIEMPO HIGH
      *PUTptr++ = PROGRAMADOR_getProgData(GETptr_ptr);  // Guardo VALOR FICHA TIEMPO LOW
      *PUTptr++ = PROGRAMADOR_getProgData(GETptr_ptr);  // Guardo BAJADA DE BANDERA HIGH
      *PUTptr++ = PROGRAMADOR_getProgData(GETptr_ptr);  // Guardo BAJADA DE BANDERA LOW
      *PUTptr++ = PROGRAMADOR_getProgData(GETptr_ptr);  // Guardo VALOR FICHA HIGH
      *PUTptr++ = PROGRAMADOR_getProgData(GETptr_ptr);  // Guardo VALOR FICHA LOW
      *PUTptr++ = PROGRAMADOR_getProgData(GETptr_ptr);  // Guardo VALOR FICHA TIEMPO HIGH
      *PUTptr++ = PROGRAMADOR_getProgData(GETptr_ptr);  // Guardo VALOR FICHA TIEMPO LOW
      
     // *PUTptr++ = finEEPROM_H;                          // Fin de Datos
     // *PUTptr++ = finEEPROM_L;
    }


/*********************************************************************************************/
/* RUTINAS DE EEPROM - CALENDARIO */
/**********************************/

  /* INICIAR GRABACION DE CALENDARIO EN EEPROM */
  /*********************************************/
    void PROG_RELOJcalend_to_EEPROM (byte iniGrabacion){
      // Da la orden de grabacion en EEPROM de la Tarifa correspondiente
      prgRELOJ_CALEND_EEPROM_F = 1;     // Levanto bandera de grabacion de programacion de eq pesos en eeprom  
      
      if (iniGrabacion){
        EEPROM_iniGrabacion();          // Inicio grabacion de EEPROM AL INSTANTE xq es por IRQ
      }
    }

  
  /* GRABACION DE CALENDARIO EN EEPROM */
  /*************************************/
    tEEPROM_ERROR PROG_RELOJcalend_grabarEEPROM (void){
      tEEPROM_ERROR error;
      dword* EEPROM_ptr;
      uint8_t* ptrAUX;
      uint16_t valorCRC;
      byte EEPROM_buffer[EEPROMsize_PRG_relojCALEND];
     // uint16_t long_relojCALEND_DATA;
      
      //error = EEPROM_OK;                // Asumo que no hay error
      error = EEPROM_ERROR_MASK;                //si no entra en el if el nro de tarifa no esta programada o hay algun error
      if (prgRELOJ_CALEND_EEPROM_F){
        prgRELOJ_CALEND_EEPROM_F = 0;   // Bajo Bandera
        
        armarBuffer_progCALEND_EEPROM(EEPROM_buffer);  // Armo buffer de grabación según formato

        //calculo de crc de datos recibidos
		//long_relojCALEND_DATA = EEPROMsize_PRG_relojCALEND - (sizeof(((tPARAM_RELOJ_CALEND *)0)->checksum) + sizeof(((tPARAM_RELOJ_CALEND *)0)->finDATA));
		valorCRC = GetCrc16(&EEPROM_buffer, EEPROMsize_PRG_relojCALEND - 4); //resto cuatro por el crc y el df0a

		//pongo crc en buffer de grabacion
		ptrAUX = &valorCRC;
		EEPROM_buffer[EEPROMsize_PRG_relojCALEND-4] = *(ptrAUX+1);
		EEPROM_buffer[EEPROMsize_PRG_relojCALEND-3] = *(ptrAUX+0);
		EEPROM_buffer[EEPROMsize_PRG_relojCALEND-2] = finEEPROM_H;              // Fin de Datos
		EEPROM_buffer[EEPROMsize_PRG_relojCALEND-1] = finEEPROM_L;

		//grabar
		error = grabar_buffer_EEPROM((uint16_t*)(&EEPROM_buffer), ADDRESS_PROG_relojCALEND, EEPROMsize_PRG_relojCALEND);
        //grabacion correcta ?
		if(error == EEPROM_OK){
			error = chkCRC_EnEEPROM(ADDRESS_PROG_relojCALEND, EEPROMsize_PRG_relojCALEND);
		}

		if(error == EEPROM_OK){
			error = grabar_buffer_EEPROM((uint16_t*)(&EEPROM_buffer), ADDRESS_PROG_relojCALEND_bck1, EEPROMsize_PRG_relojCALEND);
			//grabacion correcta ?
			if(error == EEPROM_OK){
				error = chkCRC_EnEEPROM(ADDRESS_PROG_relojCALEND_bck1, EEPROMsize_PRG_relojCALEND);
			}
		}
		if(error == EEPROM_OK){
			error = grabar_buffer_EEPROM((uint16_t*)(&EEPROM_buffer), ADDRESS_PROG_relojCALEND_bck2, EEPROMsize_PRG_relojCALEND);
			//grabacion correcta ?
			if(error == EEPROM_OK){
				error = chkCRC_EnEEPROM(ADDRESS_PROG_relojCALEND_bck2, EEPROMsize_PRG_relojCALEND);
			}
		}
		if(error == EEPROM_OK){
			error = grabar_buffer_EEPROM((uint16_t*)(&EEPROM_buffer), ADDRESS_PROG_relojCALEND_bck3, EEPROMsize_PRG_relojCALEND);
			//grabacion correcta ?
			if(error == EEPROM_OK){
				error = chkCRC_EnEEPROM(ADDRESS_PROG_relojCALEND_bck3, EEPROMsize_PRG_relojCALEND);
			}
		}

     }else{
    	 error = EEPROM_OK;
     }
      return(error);
    }    

  /* ARMAR BUFFER DE GRABACION DE CALENDARIO */
  /*******************************************/
    static void armarBuffer_progCALEND_EEPROM (byte* RAMbuffer){
      // El calendario se graba en EEPROM con el siguiente formato:
      // (recordemos que la EEPROM se divide en bloques de 1 longword, es decir
      //  4bytes)
      //
      //  +----|----|----|----++
      //  | DD   MM | DD   MM || son <cant_Feriados> feriados, o sea <cant_Feriados> DD-MM
      //  +----|----|----|----++
      //
      byte* PUTptr;
      byte** GETptr_ptr;
      byte i;
      
      PUTptr = RAMbuffer;                                 // Ingreso datos desde inicio de Buffer
      GETptr_ptr = &progRELOJ_CALEND_GET;                 // Puntero a inicio de datos de RELOJ COMUN
      *PUTptr++ = PROGRAMADOR_getProgData(GETptr_ptr);    // Extraigo DIA 0 y avanzo puntero GET
      *PUTptr++ = PROGRAMADOR_getProgData(GETptr_ptr);    // Extraigo MES 0 y avanzo puntero GET

      for (i=1; i<cant_Feriados; i++){
        *PUTptr++ = PROGRAMADOR_getProgData(GETptr_ptr);  // Extraigo DIA 1->19 y avanzo puntero GET
        *PUTptr++ = PROGRAMADOR_getProgData(GETptr_ptr);  // Extraigo MES 1->19 y avanzo puntero GET
      }
      
     // *PUTptr++ = finEEPROM_H;                            // Fin de Datos
     // *PUTptr++ = finEEPROM_L;
    }

void read_progRELOJ_eeprom(void){
	uint8_t buffer_backup[128];
	uint8_t* buffer;

	uint32_t size;

	buffer = &buffer_backup;
	read_relojCOMUN_eeprom(buffer); //long 12
	read_relojT1D_eeprom(buffer);	  //26
	read_relojT2D_eeprom(buffer);
	read_relojT3D_eeprom(buffer);
	read_relojT4D_eeprom(buffer);
	read_relojT1N_eeprom(buffer);
	read_relojT2N_eeprom(buffer);
	read_relojT3N_eeprom(buffer);
	read_relojT4N_eeprom(buffer);
	read_relojEqPESOS_eeprom(buffer);	//long 22
	read_relojCALEND_eeprom(buffer);  //long 44

}

void read_relojCOMUN_eeprom(uint8_t* buffer_backup){

	uint8_t* ptrDIR;
	uint8_t i=0;

	uint32_t addr;
	uint32_t address_eeprom;

	uint32_t size;
	size = SIZE_PROG_relojCOMUN;

	//address_eeprom = ADDRESS_PROG_relojCOMUN;
	getDirProgOk();
	address_eeprom = addressReloj;

		//me fijo si hubo corte de alimentacion
	EEPROM_ReadBuffer(buffer_backup,address_eeprom,SIZE_PROG_relojCOMUN);
}



 void read_relojT1D_eeprom(uint8_t* buffer_backup){

	uint8_t* ptrDIR;
	uint8_t i=0;

	uint32_t addr;
	uint32_t address_eeprom;

	uint32_t size;
	size = SIZE_PROG_relojT1D;

	//address_eeprom = ADDRESS_PROG_relojT1D;
	address_eeprom = getDirProgOk();								//traigo sector de programacion
	address_eeprom = getDir_tarifaX_BlockProg(1, address_eeprom); 	//traigo direccion de tarifa del sector

	//me fijo si hubo corte de alimentacion
	EEPROM_ReadBuffer(buffer_backup,address_eeprom,SIZE_PROG_relojT1D);
}



void read_relojT2D_eeprom(uint8_t* buffer_backup){

	uint8_t* ptrDIR;
	uint8_t i=0;

	uint32_t addr;
	uint32_t address_eeprom;

	//address_eeprom = ADDRESS_PROG_relojT2D;
	address_eeprom = getDirProgOk();								//traigo sector de programacion
	address_eeprom = getDir_tarifaX_BlockProg(2, address_eeprom); 	//traigo direccion de tarifa del sector

	//me fijo si hubo corte de alimentacion
	EEPROM_ReadBuffer(buffer_backup,address_eeprom,SIZE_PROG_relojT2D);
}



void read_relojT3D_eeprom(uint8_t* buffer_backup){

	uint8_t* ptrDIR;
	uint8_t i=0;

	uint32_t addr;
	uint32_t address_eeprom;

	//address_eeprom = ADDRESS_PROG_relojT3D;
	address_eeprom = getDirProgOk();								//traigo sector de programacion
	address_eeprom = getDir_tarifaX_BlockProg(3, address_eeprom); 	//traigo direccion de tarifa del sector

	//me fijo si hubo corte de alimentacion
	EEPROM_ReadBuffer(buffer_backup,address_eeprom,SIZE_PROG_relojT3D);
}


void read_relojT4D_eeprom(uint8_t* buffer_backup){

	uint8_t* ptrDIR;
	uint8_t i=0;

	uint32_t addr;
	uint32_t address_eeprom;

	uint32_t size;
	size = SIZE_PROG_relojT4D;

	//address_eeprom = ADDRESS_PROG_relojT4D;
	address_eeprom = getDirProgOk();								//traigo sector de programacion
	address_eeprom = getDir_tarifaX_BlockProg(4, address_eeprom); 	//traigo direccion de tarifa del sector

	//me fijo si hubo corte de alimentacion
	EEPROM_ReadBuffer(buffer_backup,address_eeprom,SIZE_PROG_relojT4D);
}


void read_relojT1N_eeprom(uint8_t* buffer_backup){

	uint8_t* ptrDIR;
	uint8_t i=0;

	uint32_t addr;
	uint32_t address_eeprom;

	//address_eeprom = ADDRESS_PROG_relojT1N;
	address_eeprom = getDirProgOk();								//traigo sector de programacion
	address_eeprom = getDir_tarifaX_BlockProg(5, address_eeprom); 	//traigo direccion de tarifa del sector

	//me fijo si hubo corte de alimentacion
	EEPROM_ReadBuffer(buffer_backup,address_eeprom,SIZE_PROG_relojT1N);
}


void read_relojT2N_eeprom(uint8_t* buffer_backup){

	uint8_t* ptrDIR;
	uint8_t i=0;

	uint32_t addr;
	uint32_t address_eeprom;

	//address_eeprom = ADDRESS_PROG_relojT2N;
	address_eeprom = getDirProgOk();								//traigo sector de programacion
	address_eeprom = getDir_tarifaX_BlockProg(6, address_eeprom); 	//traigo direccion de tarifa del sector

	//me fijo si hubo corte de alimentacion
	EEPROM_ReadBuffer(buffer_backup,address_eeprom,SIZE_PROG_relojT2N);
}


void read_relojT3N_eeprom(uint8_t* buffer_backup){

	uint8_t* ptrDIR;
	uint8_t i=0;

	uint32_t addr;
	uint32_t address_eeprom;

	//address_eeprom = ADDRESS_PROG_relojT3N;
	address_eeprom = getDirProgOk();								//traigo sector de programacion
	address_eeprom = getDir_tarifaX_BlockProg(7, address_eeprom); 	//traigo direccion de tarifa del sector
	//me fijo si hubo corte de alimentacion
	EEPROM_ReadBuffer(buffer_backup,address_eeprom,SIZE_PROG_relojT3N);
}


void read_relojT4N_eeprom(uint8_t* buffer_backup){

	uint8_t* ptrDIR;
	uint8_t i=0;

	uint32_t addr;
	uint32_t address_eeprom;

	//address_eeprom = ADDRESS_PROG_relojT4N;
	address_eeprom = getDirProgOk();								//traigo sector de programacion
	address_eeprom = getDir_tarifaX_BlockProg(8, address_eeprom); 	//traigo direccion de tarifa del sector

	//me fijo si hubo corte de alimentacion
	EEPROM_ReadBuffer(buffer_backup,address_eeprom,SIZE_PROG_relojT4N);
}

void read_relojEqPESOS_eeprom(uint8_t* buffer_backup){

	uint8_t* ptrDIR;
	uint8_t i=0;

	uint32_t addr;
	uint32_t address_eeprom;

	uint32_t size;
	size = SIZE_PROG_relojEqPESOS;

	//address_eeprom = ADDRESS_PROG_relojEqPESOS;
	getDirProgOk();
	address_eeprom = addressEqPesos;

	//me fijo si hubo corte de alimentacion
	EEPROM_ReadBuffer(buffer_backup,address_eeprom,SIZE_PROG_relojEqPESOS);
}


void read_relojCALEND_eeprom(uint8_t* buffer_backup){

	uint8_t* ptrDIR;
	uint8_t i=0;

	uint32_t size;
	size = SIZE_PROG_relojCALEND;

	uint32_t addr;
	uint32_t address_eeprom;

	//address_eeprom = ADDRESS_PROG_relojCALEND;
	getDirProgOk();
	address_eeprom = addressCalend;

	//me fijo si hubo corte de alimentacion
	EEPROM_ReadBuffer(buffer_backup,address_eeprom,SIZE_PROG_relojCALEND);
}

void read_TICKET_eeprom(uint8_t* buffer_backup){

	uint32_t address_eeprom;
	uint32_t size_eeprom;

	size_eeprom = SIZE_PROG_TICKET;
	address_eeprom = ADDRESS_PROG_TICKET_PAGE1;
	EEPROM_ReadBuffer(buffer_backup,address_eeprom,128);

	address_eeprom = ADDRESS_PROG_TICKET_PAGE2;
	EEPROM_ReadBuffer(buffer_backup+128,address_eeprom,size_eeprom-128);

}


void read_TICKET_RECAUD_eeprom(uint8_t* buffer_backup){

	uint32_t address_eeprom;
	uint32_t size_eeprom;

	size_eeprom = SIZE_PROG_TICKET_RECAUD;
	address_eeprom = ADDRESS_PROG_TICKET_RECAUD;
	EEPROM_ReadBuffer(buffer_backup,address_eeprom,size_eeprom);
}

tEEPROM_ERROR chkCRC_EnEEPROM(uint32_t addrEEPROM, uint16_t longTOread){

	uint8_t* ptrAUX;
    uint16_t valorCRC;
    uint16_t CRC_EEPROM;
    tEEPROM_ERROR status;
    //byte RAM_buffer[longTOread];

    byte RAM_buffer[EEPROMsize_PRG_MAX];

	//levanta desde eeprom datos en buffer
	 EEPROM_ReadBuffer(RAM_buffer,addrEEPROM ,longTOread);
	 //calculo de crc de datos levantados desde eeprom
	  valorCRC = GetCrc16(RAM_buffer, longTOread-4);
	 //levanto crc guardado en eeprom
	 ptrAUX = &CRC_EEPROM;
	 *(ptrAUX+1) = RAM_buffer[longTOread-4];
	 *(ptrAUX+0) = RAM_buffer[longTOread-3];
	 if(valorCRC == CRC_EEPROM){
		 status = EEPROM_OK;
	 }else{
		 status = EEPROM_CHECK_SUM;
	 }
	 return(status);
}


/*********************************************************************************************/
/* PROGRAMACION POR AIRE - PARAMETROS COMUNES */
/**********************************************/

  /* GUARDAR DATOS DE AIRE A PROGRAMAR */
  /*************************************/
    void PROG_saveRELOJ_COMUN_air (byte* data, byte N){
      // Rutina que guarda los datos para luego grabar la EEPROM. Utilizado en
      // la actualizacion por AIRE
      
      progRELOJ_COMUN_GET = PROGRAMADOR_addByte(*data++);
      N--;
      while (N > 0){
        N--;
        (void)PROGRAMADOR_addByte(*data++);
      }
    }
  

  /* PROGRAMAR PARAMAETROS DE RELOJ COMUNES POR AIRE */
  /***************************************************/
    byte PROG_writeRELOJ_COMUN_air (byte N){
      // Programar parametros de RELOJ (comunes) desde AIRE, graba los paramteros que
      // fueron guardados y hace un chequeo del N.
      // Si los datos no son correctos, indica esta situacion
      byte error;
      

      if (N != N_RELOJcomun){
        error = 1;
      
      }else{


        error = 0;
        PROG_RELOJcomun_to_EEPROM(1);
        PROG_RELOJcomunes_grabarEEPROM();
      }
      
      return (error);
    }    


/*********************************************************************************************/
/* PROGRAMACION POR AIRE - TARIFAS */
/***********************************/

  //PRUEBA
  word *PTR;
  
  /* GUARDAR DATOS DE AIRE A PROGRAMAR */
  /*************************************/
    void PROG_saveRELOJ_TARIFA_air (byte* data, byte N){
      // Rutina que guarda los datos para luego grabar la EEPROM. Utilizado en
      // la actualizacion por AIRE
      TARIFA_toUpdate = *data;                  // Extraifo indice de tarifa
     // N--;
      
     //PRUEBA
      PTR = PROGRAMADOR_addByte(*data++);
      
      *TARIFAS_GET_ptr[TARIFA_toUpdate-1] = PTR; 
     // *TARIFAS_GET_ptr[TARIFA_toUpdate] = PROGRAMADOR_addByte(*data++);
      
      N--;
      while (N > 0){
        N--;
        (void)PROGRAMADOR_addByte(*data++);
      }
    }
  

  /* PROGRAMAR PARAMAETROS DE RELOJ TARIFA POR AIRE */
  /**************************************************/
    byte PROG_writeRELOJ_TARIFA_air (byte N){
      // Programar parametros de RELOJ (tarifa) desde AIRE, graba los paramteros que
      // fueron guardados y hace un chequeo del N.
      // Si los datos no son correctos, indica esta situacion
      byte error;
      
      //prueba
      N -=  1;

      if (N != N_RELOJtarifa){
        error = 1;
      
      }else{

	    //prueba
	    N +=  1;

    	error = 0;
    	//envio la ADDRESS_PROG_relojCOMUN porque las direcciones de tarifa estan referenciadas a esta
        PROG_RELOJtarifa_to_EEPROM(TARIFA_toUpdate, 1);
        PROG_RELOJtarifa_grabarEEPROM(TARIFA_toUpdate);

      }
      
      return (error);
    }    

/*********************************************************************************************/
/* PROGRAMACION POR AIRE - EQUIVALENCIA EN PESOS */
/*************************************************/

  /* GUARDAR DATOS DE AIRE A PROGRAMAR */
  /*************************************/
    void PROG_saveRELOJ_EqPESOS_air (byte* data, byte N){
      // Rutina que guarda los datos para luego grabar la EEPROM. Utilizado en
      // la actualizacion por AIRE
      
      progRELOJ_EQ_GET = PROGRAMADOR_addByte(*data++);
      N--;
      while (N > 0){
        N--;
        (void)PROGRAMADOR_addByte(*data++);
      }
    }
  

  /* PROGRAMAR PARAMAETROS DE RELOJ EQUIVALENCIA EN PESOS POR AIRE */
  /*****************************************************************/
    byte PROG_writeRELOJ_EqPESOS_air (byte N){
      // Programar parametros de RELOJ (eqPesos) desde AIRE, graba los paramteros que
      // fueron guardados y hace un chequeo del N.
      // Si los datos no son correctos, indica esta situacion
      byte error;
      
      if (N != N_RELOJeqPesos){
        error = 1;
      
      }else{
        error = 0;
        PROG_RELOJeqPesos_to_EEPROM(1);
      }
      
      return (error);
    }    


/*********************************************************************************************/
/* PROGRAMACION POR AIRE - CALENDARIO */
/**************************************/

  /* GUARDAR DATOS DE AIRE A PROGRAMAR */
  /*************************************/
    void PROG_saveRELOJ_CALEND_air (byte* data, byte N){
      // Rutina que guarda los datos para luego grabar la EEPROM. Utilizado en
      // la actualizacion por AIRE
      
      progRELOJ_CALEND_GET = PROGRAMADOR_addByte(*data++);
      N--;
      while (N > 0){
        N--;
        (void)PROGRAMADOR_addByte(*data++);
      }
    }
  

  /* PROGRAMAR PARAMAETROS DE RELOJ CALENDARIO POR AIRE */
  /******************************************************/
    byte PROG_writeRELOJ_CALEND_air (byte N){
      // Programar parametros de RELOJ (calendario) desde AIRE, graba los paramteros que
      // fueron guardados y hace un chequeo del N.
      // Si los datos no son correctos, indica esta situacion
      byte error;
      
      if (N != N_RELOJcalendar){
        error = 1;
      
      }else{
        error = 0;
        PROG_RELOJcalend_to_EEPROM(1);
      }
      
      return (error);
    }    



    void levantar_progRELOJ (void){


    	//levanta desde eeprom datos de reloj comun
    	 EEPROM_ReadBuffer(&EEPROM_PROG_relojCOMUN,ADDRESS_PROG_relojCOMUN,sizeof(tPARAM_RELOJ_COMUNES));
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojCOMUN.pulsosKm, 2);
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojCOMUN.carreraBandera, 2);

    	 //levanta desde eeprom datos de tarifa 1 DIURNA
    	 EEPROM_ReadBuffer(&EEPROM_PROG_relojT1D,ADDRESS_PROG_relojT1D,sizeof(tPARAM_RELOJ_TARIFA));
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT1D.tarifa.bajadaBandera, 2);
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT1D.tarifa.distFicha, 2);
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT1D.tarifa.distInicial, 2);
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT1D.tarifa.horaInicio, 2);
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT1D.tarifa.tiempoFicha, 2);
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT1D.tarifa.tiempoGracia, 2);
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT1D.tarifa.valorFicha, 2);
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT1D.tarifa.valorFichaTiempo, 2);
    	 //levanta desde eeorm datos de tarifa 2 DIURNA
    	 EEPROM_ReadBuffer(&EEPROM_PROG_relojT2D,ADDRESS_PROG_relojT2D,sizeof(tPARAM_RELOJ_TARIFA));
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT2D.tarifa.bajadaBandera, 2);
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT2D.tarifa.distFicha, 2);
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT2D.tarifa.distInicial, 2);
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT2D.tarifa.horaInicio, 2);
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT2D.tarifa.tiempoFicha, 2);
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT2D.tarifa.tiempoGracia, 2);
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT2D.tarifa.valorFicha, 2);
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT2D.tarifa.valorFichaTiempo, 2);
    	 //levanta desde eeorm datos de tarifa 3 DIURNA
    	 EEPROM_ReadBuffer(&EEPROM_PROG_relojT3D,ADDRESS_PROG_relojT3D,sizeof(tPARAM_RELOJ_TARIFA));
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT3D.tarifa.bajadaBandera, 2);
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT3D.tarifa.distFicha, 2);
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT3D.tarifa.distInicial, 2);
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT3D.tarifa.horaInicio, 2);
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT3D.tarifa.tiempoFicha, 2);
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT3D.tarifa.tiempoGracia, 2);
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT3D.tarifa.valorFicha, 2);
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT3D.tarifa.valorFichaTiempo, 2);
    	 //levanta desde eeorm datos de tarifa 4 DIURNA
    	 EEPROM_ReadBuffer(&EEPROM_PROG_relojT4D,ADDRESS_PROG_relojT4D,sizeof(tPARAM_RELOJ_TARIFA));
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT4D.tarifa.bajadaBandera, 2);
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT4D.tarifa.distFicha, 2);
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT4D.tarifa.distInicial, 2);
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT4D.tarifa.horaInicio, 2);
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT4D.tarifa.tiempoFicha, 2);
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT4D.tarifa.tiempoGracia, 2);
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT4D.tarifa.valorFicha, 2);
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT4D.tarifa.valorFichaTiempo, 2);

    	 //levanta desde eeorm datos de tarifa 1 NOCTURNA
    	 EEPROM_ReadBuffer(&EEPROM_PROG_relojT1N,ADDRESS_PROG_relojT1N,sizeof(tPARAM_RELOJ_TARIFA));
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT1N.tarifa.bajadaBandera, 2);
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT1N.tarifa.distFicha, 2);
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT1N.tarifa.distInicial, 2);
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT1N.tarifa.horaInicio, 2);
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT1N.tarifa.tiempoFicha, 2);
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT1N.tarifa.tiempoGracia, 2);
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT1N.tarifa.valorFicha, 2);
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT1N.tarifa.valorFichaTiempo, 2);
    	 //levanta desde eeorm datos de tarifa 2 NOCTURNA
    	 EEPROM_ReadBuffer(&EEPROM_PROG_relojT2N,ADDRESS_PROG_relojT2N,sizeof(tPARAM_RELOJ_TARIFA));
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT2N.tarifa.bajadaBandera, 2);
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT2N.tarifa.distFicha, 2);
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT2N.tarifa.distInicial, 2);
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT2N.tarifa.horaInicio, 2);
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT2N.tarifa.tiempoFicha, 2);
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT2N.tarifa.tiempoGracia, 2);
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT2N.tarifa.valorFicha, 2);
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT2N.tarifa.valorFichaTiempo, 2);
    	 //levanta desde eeorm datos de tarifa 3 DIURNA
    	 EEPROM_ReadBuffer(&EEPROM_PROG_relojT3N,ADDRESS_PROG_relojT3N,sizeof(tPARAM_RELOJ_TARIFA));
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT3N.tarifa.bajadaBandera, 2);
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT3N.tarifa.distFicha, 2);
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT3N.tarifa.distInicial, 2);
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT3N.tarifa.horaInicio, 2);
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT3N.tarifa.tiempoFicha, 2);
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT3N.tarifa.tiempoGracia, 2);
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT3N.tarifa.valorFicha, 2);
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT3N.tarifa.valorFichaTiempo, 2);
    	 //levanta desde eeorm datos de tarifa 4 NOCTURNA
    	 EEPROM_ReadBuffer(&EEPROM_PROG_relojT4N,ADDRESS_PROG_relojT4N,sizeof(tPARAM_RELOJ_TARIFA));
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT4N.tarifa.bajadaBandera, 2);
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT4N.tarifa.distFicha, 2);
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT4N.tarifa.distInicial, 2);
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT4N.tarifa.horaInicio, 2);
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT4N.tarifa.tiempoFicha, 2);
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT4N.tarifa.tiempoGracia, 2);
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT4N.tarifa.valorFicha, 2);
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT4N.tarifa.valorFichaTiempo, 2);

    	 //levanta desde eeprom datos de equivalencia en pesos
    	 //EEPROM_ReadBuffer(&EEPROM_AUX,ADDRESS_PROG_relojEqPESOS,(sizeof(tPARAM_RELOJ_EQPESOS)));
    	 //EEPROM_ReadBuffer(&EEPROM_PROG_relojEqPESOS,ADDRESS_PROG_relojEqPESOS,(sizeof(tPARAM_RELOJ_EQPESOS)));
    	 //levanto de esta forma porque hacendolo como esta comentad arriba se corre un byte y guarda los datos en los elmentos de la estructura
    	 //en forma errornea.
    	 EEPROM_ReadBuffer(&EEPROM_PROG_relojEqPESOS.hab,ADDRESS_PROG_relojEqPESOS,1);
    	 EEPROM_ReadBuffer(&EEPROM_PROG_relojEqPESOS.diurna.bajadaBandera,ADDRESS_PROG_relojEqPESOS+1,2);
    	 EEPROM_ReadBuffer(&EEPROM_PROG_relojEqPESOS.diurna.valorFicha,ADDRESS_PROG_relojEqPESOS+3,2);
    	 EEPROM_ReadBuffer(&EEPROM_PROG_relojEqPESOS.diurna.valorFichaTiempo,ADDRESS_PROG_relojEqPESOS+5,2);
    	 EEPROM_ReadBuffer(&EEPROM_PROG_relojEqPESOS.nocturna.bajadaBandera,ADDRESS_PROG_relojEqPESOS+7,2);
    	 EEPROM_ReadBuffer(&EEPROM_PROG_relojEqPESOS.nocturna.valorFicha,ADDRESS_PROG_relojEqPESOS+9,2);
    	 EEPROM_ReadBuffer(&EEPROM_PROG_relojEqPESOS.nocturna.valorFichaTiempo,ADDRESS_PROG_relojEqPESOS+11,2);
    	 //doy vuelta los bytes
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojEqPESOS.diurna.bajadaBandera, 2);
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojEqPESOS.diurna.valorFicha, 2);
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojEqPESOS.diurna.valorFichaTiempo, 2);
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojEqPESOS.nocturna.bajadaBandera, 2);
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojEqPESOS.nocturna.valorFicha, 2);
    	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojEqPESOS.nocturna.valorFichaTiempo, 2);

    	 //levanta desde eeprom datos de CALENDARIO
     	EEPROM_ReadBuffer(&EEPROM_PROG_relojCALEND[0],  ADDRESS_PROG_relojCALEND0 , longitudFECHA_CALENDARIO);
     	EEPROM_ReadBuffer(&EEPROM_PROG_relojCALEND[1],  ADDRESS_PROG_relojCALEND1 , longitudFECHA_CALENDARIO);
     	EEPROM_ReadBuffer(&EEPROM_PROG_relojCALEND[2],  ADDRESS_PROG_relojCALEND2 , longitudFECHA_CALENDARIO);
     	EEPROM_ReadBuffer(&EEPROM_PROG_relojCALEND[3],  ADDRESS_PROG_relojCALEND3 , longitudFECHA_CALENDARIO);
     	EEPROM_ReadBuffer(&EEPROM_PROG_relojCALEND[4],  ADDRESS_PROG_relojCALEND4 , longitudFECHA_CALENDARIO);
     	EEPROM_ReadBuffer(&EEPROM_PROG_relojCALEND[5],  ADDRESS_PROG_relojCALEND5 , longitudFECHA_CALENDARIO);
     	EEPROM_ReadBuffer(&EEPROM_PROG_relojCALEND[6],  ADDRESS_PROG_relojCALEND6 , longitudFECHA_CALENDARIO);
     	EEPROM_ReadBuffer(&EEPROM_PROG_relojCALEND[7],  ADDRESS_PROG_relojCALEND7 , longitudFECHA_CALENDARIO);
     	EEPROM_ReadBuffer(&EEPROM_PROG_relojCALEND[8],  ADDRESS_PROG_relojCALEND8 , longitudFECHA_CALENDARIO);
     	EEPROM_ReadBuffer(&EEPROM_PROG_relojCALEND[9],  ADDRESS_PROG_relojCALEND9 , longitudFECHA_CALENDARIO);
     	EEPROM_ReadBuffer(&EEPROM_PROG_relojCALEND[10], ADDRESS_PROG_relojCALEND10, longitudFECHA_CALENDARIO);
     	EEPROM_ReadBuffer(&EEPROM_PROG_relojCALEND[11], ADDRESS_PROG_relojCALEND11, longitudFECHA_CALENDARIO);
     	EEPROM_ReadBuffer(&EEPROM_PROG_relojCALEND[12], ADDRESS_PROG_relojCALEND12, longitudFECHA_CALENDARIO);
     	EEPROM_ReadBuffer(&EEPROM_PROG_relojCALEND[13], ADDRESS_PROG_relojCALEND13, longitudFECHA_CALENDARIO);
     	EEPROM_ReadBuffer(&EEPROM_PROG_relojCALEND[14], ADDRESS_PROG_relojCALEND14, longitudFECHA_CALENDARIO);
     	EEPROM_ReadBuffer(&EEPROM_PROG_relojCALEND[15], ADDRESS_PROG_relojCALEND15, longitudFECHA_CALENDARIO);
     	EEPROM_ReadBuffer(&EEPROM_PROG_relojCALEND[16], ADDRESS_PROG_relojCALEND16, longitudFECHA_CALENDARIO);
     	EEPROM_ReadBuffer(&EEPROM_PROG_relojCALEND[17], ADDRESS_PROG_relojCALEND17, longitudFECHA_CALENDARIO);
     	EEPROM_ReadBuffer(&EEPROM_PROG_relojCALEND[18], ADDRESS_PROG_relojCALEND18, longitudFECHA_CALENDARIO);
     	EEPROM_ReadBuffer(&EEPROM_PROG_relojCALEND[19], ADDRESS_PROG_relojCALEND19, longitudFECHA_CALENDARIO);
    }


    void readProgRELOJ (uint32_t blockProg){

    		 uint32_t blgPrg;

        	//levanta desde eeprom datos de reloj comun
        	 EEPROM_ReadBuffer(&EEPROM_PROG_relojCOMUN,blockProg,sizeof(tPARAM_RELOJ_COMUNES));
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojCOMUN.pulsosKm, 2);
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojCOMUN.carreraBandera, 2);

        	 //levanta desde eeprom datos de tarifa 1 DIURNA
        	 blgPrg = (uint32_t)getDir_tarifaX_BlockProg(1, blockProg);
        	 EEPROM_ReadBuffer(&EEPROM_PROG_relojT1D,blgPrg,sizeof(tPARAM_RELOJ_TARIFA));
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT1D.tarifa.bajadaBandera, 2);
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT1D.tarifa.distFicha, 2);
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT1D.tarifa.distInicial, 2);
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT1D.tarifa.horaInicio, 2);
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT1D.tarifa.tiempoFicha, 2);
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT1D.tarifa.tiempoGracia, 2);
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT1D.tarifa.valorFicha, 2);
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT1D.tarifa.valorFichaTiempo, 2);
        	 //levanta desde eeorm datos de tarifa 2 DIURNA
        	 blgPrg = (uint32_t)getDir_tarifaX_BlockProg(2, blockProg);
        	 EEPROM_ReadBuffer(&EEPROM_PROG_relojT2D,blgPrg,sizeof(tPARAM_RELOJ_TARIFA));
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT2D.tarifa.bajadaBandera, 2);
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT2D.tarifa.distFicha, 2);
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT2D.tarifa.distInicial, 2);
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT2D.tarifa.horaInicio, 2);
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT2D.tarifa.tiempoFicha, 2);
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT2D.tarifa.tiempoGracia, 2);
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT2D.tarifa.valorFicha, 2);
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT2D.tarifa.valorFichaTiempo, 2);
        	 //levanta desde eeprom datos de tarifa 3 DIURNA
        	 blgPrg = (uint32_t)getDir_tarifaX_BlockProg(3, blockProg);
        	 EEPROM_ReadBuffer(&EEPROM_PROG_relojT3D,blgPrg,sizeof(tPARAM_RELOJ_TARIFA));
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT3D.tarifa.bajadaBandera, 2);
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT3D.tarifa.distFicha, 2);
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT3D.tarifa.distInicial, 2);
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT3D.tarifa.horaInicio, 2);
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT3D.tarifa.tiempoFicha, 2);
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT3D.tarifa.tiempoGracia, 2);
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT3D.tarifa.valorFicha, 2);
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT3D.tarifa.valorFichaTiempo, 2);
        	 //levanta desde eeprom datos de tarifa 4 DIURNA
        	 blgPrg = (uint32_t)getDir_tarifaX_BlockProg(4, blockProg);
        	 EEPROM_ReadBuffer(&EEPROM_PROG_relojT4D,blgPrg,sizeof(tPARAM_RELOJ_TARIFA));
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT4D.tarifa.bajadaBandera, 2);
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT4D.tarifa.distFicha, 2);
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT4D.tarifa.distInicial, 2);
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT4D.tarifa.horaInicio, 2);
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT4D.tarifa.tiempoFicha, 2);
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT4D.tarifa.tiempoGracia, 2);
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT4D.tarifa.valorFicha, 2);
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT4D.tarifa.valorFichaTiempo, 2);

        	 //levanta desde eeprom datos de tarifa 1 NOCTURNA
        	 blgPrg = (uint32_t)getDir_tarifaX_BlockProg(5, blockProg);
        	 EEPROM_ReadBuffer(&EEPROM_PROG_relojT1N,blgPrg,sizeof(tPARAM_RELOJ_TARIFA));
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT1N.tarifa.bajadaBandera, 2);
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT1N.tarifa.distFicha, 2);
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT1N.tarifa.distInicial, 2);
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT1N.tarifa.horaInicio, 2);
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT1N.tarifa.tiempoFicha, 2);
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT1N.tarifa.tiempoGracia, 2);
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT1N.tarifa.valorFicha, 2);
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT1N.tarifa.valorFichaTiempo, 2);
        	 //levanta desde eeprom datos de tarifa 2 NOCTURNA
        	 blgPrg = (uint32_t)getDir_tarifaX_BlockProg(6, blockProg);
        	 EEPROM_ReadBuffer(&EEPROM_PROG_relojT2N, blgPrg,sizeof(tPARAM_RELOJ_TARIFA));
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT2N.tarifa.bajadaBandera, 2);
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT2N.tarifa.distFicha, 2);
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT2N.tarifa.distInicial, 2);
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT2N.tarifa.horaInicio, 2);
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT2N.tarifa.tiempoFicha, 2);
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT2N.tarifa.tiempoGracia, 2);
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT2N.tarifa.valorFicha, 2);
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT2N.tarifa.valorFichaTiempo, 2);
        	 //levanta desde eeprom datos de tarifa 3 DIURNA
        	 blgPrg = (uint32_t)getDir_tarifaX_BlockProg(7, blockProg);
        	 EEPROM_ReadBuffer(&EEPROM_PROG_relojT3N,blgPrg,sizeof(tPARAM_RELOJ_TARIFA));
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT3N.tarifa.bajadaBandera, 2);
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT3N.tarifa.distFicha, 2);
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT3N.tarifa.distInicial, 2);
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT3N.tarifa.horaInicio, 2);
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT3N.tarifa.tiempoFicha, 2);
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT3N.tarifa.tiempoGracia, 2);
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT3N.tarifa.valorFicha, 2);
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT3N.tarifa.valorFichaTiempo, 2);
        	 //levanta desde eeprom datos de tarifa 4 NOCTURNA
        	 blgPrg = (uint32_t)getDir_tarifaX_BlockProg(8, blockProg);
        	 EEPROM_ReadBuffer(&EEPROM_PROG_relojT4N,blgPrg,sizeof(tPARAM_RELOJ_TARIFA));
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT4N.tarifa.bajadaBandera, 2);
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT4N.tarifa.distFicha, 2);
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT4N.tarifa.distInicial, 2);
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT4N.tarifa.horaInicio, 2);
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT4N.tarifa.tiempoFicha, 2);
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT4N.tarifa.tiempoGracia, 2);
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT4N.tarifa.valorFicha, 2);
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojT4N.tarifa.valorFichaTiempo, 2);

        	 //levanta desde eeprom datos de equivalencia en pesos
        	 //EEPROM_ReadBuffer(&EEPROM_AUX,ADDRESS_PROG_relojEqPESOS,(sizeof(tPARAM_RELOJ_EQPESOS)));
        	 //EEPROM_ReadBuffer(&EEPROM_PROG_relojEqPESOS,ADDRESS_PROG_relojEqPESOS,(sizeof(tPARAM_RELOJ_EQPESOS)));
        	 //levanto de esta forma porque hacendolo como esta comentad arriba se corre un byte y guarda los datos en los elmentos de la estructura
        	 //en forma errornea.
        	 blgPrg = blockProg + 128 + SIZE_PROG_relojT1N + SIZE_PROG_relojT2N + SIZE_PROG_relojT3N  + SIZE_PROG_relojT4N; //ADDRESS_PROG_relojEqPESOS
        	 EEPROM_ReadBuffer(&EEPROM_PROG_relojEqPESOS.hab, blgPrg, 1);
        	 EEPROM_ReadBuffer(&EEPROM_PROG_relojEqPESOS.diurna.bajadaBandera, blgPrg+1, 2);
        	 EEPROM_ReadBuffer(&EEPROM_PROG_relojEqPESOS.diurna.valorFicha, blgPrg+3, 2);
        	 EEPROM_ReadBuffer(&EEPROM_PROG_relojEqPESOS.diurna.valorFichaTiempo, blgPrg+5, 2);
        	 EEPROM_ReadBuffer(&EEPROM_PROG_relojEqPESOS.nocturna.bajadaBandera, blgPrg+7, 2);
        	 EEPROM_ReadBuffer(&EEPROM_PROG_relojEqPESOS.nocturna.valorFicha, blgPrg+9, 2);
        	 EEPROM_ReadBuffer(&EEPROM_PROG_relojEqPESOS.nocturna.valorFichaTiempo, blgPrg+11, 2);
/*
        	 blgPrg = blockProg + 128 + SIZE_PROG_relojT1N + SIZE_PROG_relojT2N + SIZE_PROG_relojT3N  + SIZE_PROG_relojT4N; //ADDRESS_PROG_relojEqPESOS
        	 EEPROM_ReadBuffer(&EEPROM_PROG_relojEqPESOS, blgPrg, SIZE_PROG_relojEqPESOS);
*/
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojEqPESOS.diurna.bajadaBandera, 2);
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojEqPESOS.diurna.valorFicha, 2);
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojEqPESOS.diurna.valorFichaTiempo, 2);
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojEqPESOS.nocturna.bajadaBandera, 2);
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojEqPESOS.nocturna.valorFicha, 2);
        	 convert_bigINDIAN_to_litleINDIAN (&EEPROM_PROG_relojEqPESOS.nocturna.valorFichaTiempo, 2);


        	 //levanta desde eeprom datos de CALENDARIO
        	blgPrg = blockProg + 2*128;
        	EEPROM_ReadBuffer(&EEPROM_PROG_relojCALEND[0],  blgPrg + 0*longitudFECHA_CALENDARIO, longitudFECHA_CALENDARIO);
         	EEPROM_ReadBuffer(&EEPROM_PROG_relojCALEND[1],  blgPrg + 1*longitudFECHA_CALENDARIO, longitudFECHA_CALENDARIO);
         	EEPROM_ReadBuffer(&EEPROM_PROG_relojCALEND[2],  blgPrg + 2*longitudFECHA_CALENDARIO, longitudFECHA_CALENDARIO);
         	EEPROM_ReadBuffer(&EEPROM_PROG_relojCALEND[3],  blgPrg + 3*longitudFECHA_CALENDARIO, longitudFECHA_CALENDARIO);
         	EEPROM_ReadBuffer(&EEPROM_PROG_relojCALEND[4],  blgPrg + 4*longitudFECHA_CALENDARIO, longitudFECHA_CALENDARIO);
         	EEPROM_ReadBuffer(&EEPROM_PROG_relojCALEND[5],  blgPrg + 5*longitudFECHA_CALENDARIO, longitudFECHA_CALENDARIO);
         	EEPROM_ReadBuffer(&EEPROM_PROG_relojCALEND[6],  blgPrg + 6*longitudFECHA_CALENDARIO, longitudFECHA_CALENDARIO);
         	EEPROM_ReadBuffer(&EEPROM_PROG_relojCALEND[7],  blgPrg + 7*longitudFECHA_CALENDARIO, longitudFECHA_CALENDARIO);
         	EEPROM_ReadBuffer(&EEPROM_PROG_relojCALEND[8],  blgPrg + 8*longitudFECHA_CALENDARIO, longitudFECHA_CALENDARIO);
         	EEPROM_ReadBuffer(&EEPROM_PROG_relojCALEND[9],  blgPrg + 9*longitudFECHA_CALENDARIO, longitudFECHA_CALENDARIO);
         	EEPROM_ReadBuffer(&EEPROM_PROG_relojCALEND[10], blgPrg + 10*longitudFECHA_CALENDARIO, longitudFECHA_CALENDARIO);
         	EEPROM_ReadBuffer(&EEPROM_PROG_relojCALEND[11], blgPrg + 11*longitudFECHA_CALENDARIO, longitudFECHA_CALENDARIO);
         	EEPROM_ReadBuffer(&EEPROM_PROG_relojCALEND[12], blgPrg + 12*longitudFECHA_CALENDARIO, longitudFECHA_CALENDARIO);
         	EEPROM_ReadBuffer(&EEPROM_PROG_relojCALEND[13], blgPrg + 13*longitudFECHA_CALENDARIO, longitudFECHA_CALENDARIO);
         	EEPROM_ReadBuffer(&EEPROM_PROG_relojCALEND[14], blgPrg + 14*longitudFECHA_CALENDARIO, longitudFECHA_CALENDARIO);
         	EEPROM_ReadBuffer(&EEPROM_PROG_relojCALEND[15], blgPrg + 15*longitudFECHA_CALENDARIO, longitudFECHA_CALENDARIO);
         	EEPROM_ReadBuffer(&EEPROM_PROG_relojCALEND[16], blgPrg + 16*longitudFECHA_CALENDARIO, longitudFECHA_CALENDARIO);
         	EEPROM_ReadBuffer(&EEPROM_PROG_relojCALEND[17], blgPrg + 17*longitudFECHA_CALENDARIO, longitudFECHA_CALENDARIO);
         	EEPROM_ReadBuffer(&EEPROM_PROG_relojCALEND[18], blgPrg + 18*longitudFECHA_CALENDARIO, longitudFECHA_CALENDARIO);
         	EEPROM_ReadBuffer(&EEPROM_PROG_relojCALEND[19], blgPrg + 19*longitudFECHA_CALENDARIO, longitudFECHA_CALENDARIO);
        }
/*
    void levantar_progTICKET (void){

    uint8_t* EEPROM_PROG_TICKET_ptr;

     //EEPROM_ReadBuffer(&EEPROM_PROG_TICKET, ADDRESS_PROG_TICKET,sizeof(tPRG_TICKET));
 	 EEPROM_ReadBuffer(&EEPROM_PROG_TICKET, ADDRESS_PROG_TICKET_PAGE1,128);

 	 EEPROM_PROG_TICKET_ptr = &EEPROM_PROG_TICKET;
 	 EEPROM_PROG_TICKET_ptr += 128;
 	 EEPROM_ReadBuffer(EEPROM_PROG_TICKET_ptr,ADDRESS_PROG_TICKET_PAGE2,sizeof(tPRG_TICKET)-128);

     EEPROM_ReadBuffer(&EEPROM_PROG_TICKET_RECAUD,ADDRESS_PROG_TICKET_RECAUD,sizeof(tPRG_TICKET_RECAUD));
    // testEEPROM();
    }

*/


    void readProgTICKET (uint32_t blockProg1, uint32_t blockProg2, uint32_t blockProg3){

	    uint8_t* EEPROM_PROG_TICKET_ptr;

	     //EEPROM_ReadBuffer(&EEPROM_PROG_TICKET, ADDRESS_PROG_TICKET,sizeof(tPRG_TICKET));
	 	 EEPROM_ReadBuffer(&EEPROM_PROG_TICKET, blockProg1, 128);

	 	 EEPROM_PROG_TICKET_ptr = &EEPROM_PROG_TICKET;
	 	 EEPROM_PROG_TICKET_ptr += 128;
	 	 EEPROM_ReadBuffer(EEPROM_PROG_TICKET_ptr, blockProg2, sizeof(tPRG_TICKET)-128);

	     EEPROM_ReadBuffer(&EEPROM_PROG_TICKET_RECAUD, blockProg3, sizeof(tPRG_TICKET_RECAUD));
	    // testEEPROM();
	}

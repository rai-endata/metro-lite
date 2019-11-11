/* File: <Reporte 30 Dias.c> */

/* INCLUDES */
/************/
#include "Reporte 30 Dias.h"
#include "- metroBLUE Config -.h"
#ifdef VISOR_REPORTE_30DIAS

  #include "Flash.h"
  #include "Calculos.h"
  #include "Inicio.h"
  #include "Manejo de Buffers.h"
  #include "main.h"

  #include "Reloj.h"
  #include "Tarifacion Reloj.h"
  

  #include "Reportes.h"
  
  #include "Common Ticket.h"


/*********************************************************************************************/
/* ESTRUCTURAS */
/***************/
  typedef void (*addData30dias_ptr)(tREG_GENERIC* regPTR, tREG_CHOFER_30DIAS* newReg_PTR);


/*********************************************************************************************/
/* PROTOTIPOS */
/**************/
  // RUTINAS INTERNAS
  static void updateINDEX_inFlash (void);
  static void updatePUTptr_inFlash (void);
  static byte REPORTE_30DIAS_finBackwardSearch (tREG_30DIAS*far* ptr_ptr);

  static void REPORTE_30DIAS_addData_LIBRE (tREG_GENERIC* regPTR, tREG_CHOFER_30DIAS* newReg_PTR);
  static void REPORTE_30DIAS_addData_OCUPADO (tREG_GENERIC* regPTR, tREG_CHOFER_30DIAS* newReg_PTR);
  static void REPORTE_30DIAS_addData_A_PAGAR (tREG_GENERIC* regPTR, tREG_CHOFER_30DIAS* newReg_PTR);
  static void REPORTE_30DIAS_addData_FSERV (tREG_GENERIC* regPTR, tREG_CHOFER_30DIAS* newReg_PTR);
  static void REPORTE_30DIAS_addData_DESCONEXION_ALIM (tREG_GENERIC* regPTR, tREG_CHOFER_30DIAS* newReg_PTR);
  static void REPORTE_30DIAS_addData_SESION (tREG_GENERIC* regPTR, tREG_CHOFER_30DIAS* newReg_PTR);
  static void REPORTE_30DIAS_addData_DESCONEXION_ACC (tREG_GENERIC* regPTR, tREG_CHOFER_30DIAS* newReg_PTR);
  static void REPORTE_30DIAS_addData_VARIOS (tREG_GENERIC* regPTR, tREG_CHOFER_30DIAS* newReg_PTR);

/*********************************************************************************************/
/* CONSTANTES */
/**************/
  
  static const addData30dias_ptr TABLA_addData_30DIAS[] = {
    REPORTE_30DIAS_addData_LIBRE,
    REPORTE_30DIAS_addData_OCUPADO,
    REPORTE_30DIAS_addData_A_PAGAR,
    REPORTE_30DIAS_addData_FSERV,
    REPORTE_30DIAS_addData_SESION,
    REPORTE_30DIAS_addData_DESCONEXION_ALIM,
    REPORTE_30DIAS_addData_DESCONEXION_ACC,
    REPORTE_30DIAS_addData_VARIOS
  };
  

/*********************************************************************************************/
/* VARIABLES */
/*************/
  static tREG_30DIAS*far REPORTE_30DIAS_PUTptr;   // Puntero de Insersion de Registros
  
  static tFLASH_ERROR REPORTE_30DIAS_flashError;

// VARIABLES DE FLASH
  uint32_t REPORTE_30DIAS[dim_REPORTE_30DIAS];
  uint32_t REPORTE_30DIAS_PUT;
  uint16_t REPORTE_30DIAS_INDEX;



/*********************************************************************************************/
/* RUTINAS */
/***********/
  
  /* CHEQUEAR INTEGRIDAD DEL PUNTERO DE LOS REPORTES */
  /***************************************************/
    byte REPORTE_30DIAS_chkIntigrity (void){
      // Devuelve:
      //  0 -> Puntero fuera de Rango
      //  1 -> Punero OK
      return(!((REPORTE_30DIAS_PUTptr < EEPROM_REPORTE_30DIAS) || (REPORTE_30DIAS_PUTptr > EEPROM_REPORTE_30DIAS + dim_REPORTE_30DIAS)));
    }

  /* INICIALIZACION DE REPORTE DE 30 DIAS */
  /****************************************/
    void REPORTE_30DIAS_ini (byte reset){
      // La inicializacion de REPORTE 30 DIAS, depende de si es la primera vez que arranca
      // el equipo o no
      //
      // Solo tiene sentido para RELOJ INTERNO
      byte data[4] = {0x00,0x00,0x00,0x00};
      byte* data_ptr;
      uint16_t* aux_ptr;
      dword putBKP;
      
      #ifdef VISOR_REPORTES
        if (chkVISOR_firstBoot || reset){
          // PRIMER ENCENDIDO

          //inicializa puntero de tabla 30 dias en ram y la pasa a flash
          REPORTE_30DIAS_PUTptr = (tREG_GENERIC*)EEPROM_REPORTE_30DIAS;      // Inicializo puntero de insercion
          data_ptr = (byte*) &REPORTE_30DIAS_PUTptr;           			// Guardo puntero PUT en EEPROM
          REPORTE_flashError = FLASH_updateSector((uint16_t*) EEPROM_REPORTE_30DIAS_PUT, data_ptr, (uint16_t) 4); // Grabar PUT ptr
          
          //inicializa indice de tabla 30dias en eeprom
          aux_ptr = (uint16_t*)&data;
          data_ptr = (byte*) &aux_ptr;
          REPORTE_flashError = FLASH_updateSector((uint16_t*) EEPROM_REPORTE_30DIAS_INDEX, data_ptr, (uint16_t) 4); // Grabar PUT ptr

          //inicializa tabla de 30 dias
          aux_ptr = (uint16_t*)&data;
          data_ptr = (byte*) &aux_ptr;
          REPORTE_flashError = FLASH_updateSector((uint16_t*) REPORTE_30DIAS_PUTptr, data_ptr, (uint16_t) 4); // Grabar PUT ptr
        }

		//tomo datos de eeprom
		EEPROM_ReadBuffer(&REPORTE_30DIAS_PUTptr,EEPROM_REPORTE_30DIAS_PUT,SIZE_REPORTE_30DIAS_PUT);
		EEPROM_ReadBuffer(&REPORTE_30DIAS_INDEX,EEPROM_REPORTE_30DIAS_INDEX,SIZE_REPORTE_30DIAS_INDEX);

      #endif
    }


  /* AGREGAR DATOS A REPORTE 30 DIAS */
  /***********************************/
    void REPORTE_30DIAS_addData (tREG_GENERIC* regPTR){
      // Llamada al grabar un registro (de reporte de reloj) en FLASH
      // Rutina que agrega datos al reporte de 30 dias. Mientras se trata del mismo dia, lo
      // guarda en un mismo registro, al cambiar de dia, avanza de registro
      //
      // De acuerdo al tipo de registro, es la informacion que extraigo para agregar al reporte
      // de 30dias
      //
      // LIBRE:
      //  - Km -> Km Libre
      //  - Sensor Asiento
      //
      // OCUPADO:
      //  - Km -> Km Ocupado
      //
      // A PAGAR:
      //  - Cantidad Viajes++
      //  - Fichas T |
      //  - Fichas D +--->  Recaudado  (en tarifa correspondiente)
      //  - Viajes   |
      //
      // FUERA SERVICIO:
      //  - Km -> Km FServ
      //
      // DESCONEXION ALIMENTACION:
      //  - Cantidad Desconexiones++
      //  - Tiempo Desconectado
      //  - Fichas T |
      //  - Fichas D +--->  Importe Perdido (en tarifa correspondiente)
      //  - Viajes   |
      tREG_30DIAS newReg;
      word i;
      byte* ptr;
      
      // Borro datos
      ptr = (byte*) (&newReg);
      for (i=0; i<sizeof(tREG_30DIAS); i++){
        *ptr++ = 0;
      }
      
      if (REPORTE_30DIAS_PUTptr->fecha[0] == 0){
        // REGISTRO VACIO =>
        //  - Agrego fecha
        //  - Agrego indice y lo incremento
        //  - Agrego nueva fecha
        newReg.idx = REPORTE_30DIAS_INDEX;  // Agrego indice
        updateINDEX_inFlash();                    // Incremento indice
        
        newReg.fecha[0] = getDate().fecha[0];     // Agrego nueva fecha
        newReg.fecha[1] = getDate().fecha[1];
        newReg.fecha[2] = getDate().fecha[2];
        
        
      
      }else if (REPORTE_30DIAS_PUTptr->fecha[0] != getDate().fecha[0]){
        // CAMBIO DE  DIA => 
        //  - Agrego indice y lo incremento
        //  - Agrego nueva fecha
        //  - Avanzo puntero y actualizo FLASH
        newReg.idx = REPORTE_30DIAS_INDEX;  // Agrego nuevo indice
        updateINDEX_inFlash();                    // Incremento indice
        
        newReg.fecha[0] = getDate().fecha[0];     // Agrego nueva fecha
        newReg.fecha[1] = getDate().fecha[1];
        newReg.fecha[2] = getDate().fecha[2];
        
        updatePUTptr_inFlash();                   // Avanzo puntero y actualizo FLASH
        
      }else{
        // MISMO DIA
        newReg = *REPORTE_30DIAS_PUTptr;          // Extraigo registro actual a ser actualizado
      }
       
      TABLA_addData_30DIAS[(regPTR->tipo) - 1](regPTR, &(newReg.chofer[regPTR->chofer - 1]));
      
      REPORTE_30DIAS_flashError = FLASH_updateSector((word*__far) REPORTE_30DIAS_PUTptr, (byte*) &newReg, (word) sizeof(tREG_30DIAS));
    }
           

  /* EXTRAER PUNTERO PUT */
  /***********************/
    tREG_30DIAS*far REPORTE_30DIAS_getPUT_ptr (void){
      return(REPORTE_30DIAS_PUTptr);
    }
    
  /* DETERMINAR FIN REPORTE */
  /**************************/
    byte REPORTE_30DIAS_determineFin (tREG_30DIAS*far* ptr_ptr){
      // Esta rutina se llama desde la impresion de los, para saber
      // si debo seguir retrocediendo el puntero o si ya termino el reporte
      // Tambien se llama en la pantalla menu reporte dia
      return(REPORTE_30DIAS_finBackwardSearch(ptr_ptr));
    }    


  /* AVANZAR PUNTERO DE REPORTE 30 DIAS EN FLASH */
  /***********************************************/    
    void incFlashRep30_ptr (tREG_30DIAS*far* ptr_ptr){
      tREG_30DIAS*far ptr;
      
      ptr = *ptr_ptr;                 // Puntero
      ptr++;                          // Avanzo puntero
      
      if(ptr >= (EEPROM_REPORTE_30DIAS + dim_REPORTE_30DIAS)){
        ptr = EEPROM_REPORTE_30DIAS;   // Doy la vuelta al reporte
      }
      
      *ptr_ptr = ptr;                 // Actualizo puntero REAL
    }

  
  /* RETROCEDER PUNTERO DE REPORTE 30 DIAS EN FLASH */
  /**************************************************/    
    void decFlashRep30_ptr (tREG_30DIAS*far* ptr_ptr){
      tREG_30DIAS*far ptr;
      
      ptr = *ptr_ptr;                 // Puntero
      
      if(ptr <= EEPROM_REPORTE_30DIAS){
        ptr = (EEPROM_REPORTE_30DIAS + dim_REPORTE_30DIAS);  // Doy la vuelta al reporte
      }

      ptr--;                          // Retrocedo puntero
      
      *ptr_ptr = ptr;                 // Actualizo puntero REAL
    }    


/*********************************************************************************************/
/* RUTINAS INTERNAS */
/********************/

  /* ACTUALIZAR INDICE EN FLASH */
  /******************************/
    static void updateINDEX_inFlash (void){
      // Rutina que incrementa y actualiza el INDEX de los reportes en FLASH
      // Como aqui se llama al OVERFLOW, esta rutina se debe llamar luego
      // de:
      //  1- Haber grabado un nuevo registro
      //  2- Haber incrementado el puntero PUT
      
      EEPROM_ReadBuffer(&REPORTE_30DIAS_INDEX,EEPROM_REPORTE_30DIAS_INDEX,SIZE_REPORTE_30DIAS_INDEX);
      REPORTE_30DIAS_INDEX++;                             // Incremento indice en FLASH

      if (REPORTE_30DIAS_INDEX == 0){
    	  REPORTE_30DIAS_INDEX = dim_REPORTE_30DIAS + 1;    // Para que quede registrado que dio una vuelta
      }

      EEPROM_write(EEPROM_REPORTE_30DIAS_INDEX, REPORTE_30DIAS_INDEX);
    }  



  /* ACTUALIZAR PUNTERO DE INGRESO DE REGISTROS EN FLASH */
  /*******************************************************/
    static void updatePUTptr_inFlash (void){
      // Rutina que incrementa y actualiza el PUNTERO de los reportes en FLASH
      byte* data_ptr;
      
      incFlashRep30_ptr(&REPORTE_30DIAS_PUTptr);   // Avanzo puntero de grabacion en FLASH
      data_ptr = (byte*) &REPORTE_30DIAS_PUTptr;
      REPORTE_30DIAS_flashError = FLASH_updateSector((uint16_t*) EEPROM_REPORTE_30DIAS_PUT, data_ptr, (uint16_t) 4);

    }  
  


  /* DETERMINAR FIN DE BUSQUEDA HACIA ATRAS EN REPORTE DE 30DIAS */
  /***************************************************************/
    static byte REPORTE_30DIAS_finBackwardSearch (tREG_30DIAS*far* ptr_ptr){
      // El fin de la busqueda HACIA ATRAS, puede terminar:
      //  - Cuando se alcanza el inicio del buffer (si no se dio la vuelta al mismo)
      //  - Cuando se alcanza el puntero PUT (si ya se dio al menos una vuelta)
      //
      // Para saber si di una vuelta o no, me valgo del INDEX. Si el mismo es mayor o
      // igual al tamaño del buffer => ya dio al menos una vuelta
      byte fin;
      tREG_GENERIC*far ptr;
      
      EEPROM_ReadBuffer(&REPORTE_30DIAS_INDEX,EEPROM_REPORTE_30DIAS_INDEX,SIZE_REPORTE_30DIAS_INDEX);

      ptr = (tREG_GENERIC*far)*ptr_ptr;                 // Puntero
      if (REPORTE_30DIAS_INDEX >= dim_REPORTE_30DIAS){
        // SE DIO AL MENOS UNA VUELTA => finaliza con PUNTERO = PUT
        if (ptr == REPORTE_30DIAS_PUTptr){
          fin = 1;
        }else{
          fin = 0;
        }
      
      }else{
        // TODAVIA NO SE DIO LA VUELTA => finaliza con PUNTERO = INICIO
        if (ptr == EEPROM_REPORTE_30DIAS){
          fin = 1;
        }else{
          fin = 0;
        }
      }
      
      return(fin);
    }        


  /* AGREGAR DATOS A REPORTE 30 DIAS - LIBRE*/
  /******************************************/
    static void REPORTE_30DIAS_addData_LIBRE (tREG_GENERIC* regPTR, tREG_CHOFER_30DIAS* newReg_PTR){    
      // Agrega datos de un registro LIBRE al reporte de 30DIAS
      //
      // LIBRE:
      //  - Km -> Km Libre
      //  - [EDIT 02/11/12] Sensor Asiento
      newReg_PTR->KM_libre += ((tREG_LIBRE*)regPTR)->km;
      
      if (((tREG_LIBRE*)regPTR)->sensor == sensorAsiento_pressed){
        newReg_PTR->viajesAsiento++;
      }else if (((tREG_LIBRE*)regPTR)->sensor == sensorAsiento_error){
        newReg_PTR->viajesAsiento++;
        newReg_PTR->erroresAsiento++;
      }
    }


  /* AGREGAR DATOS A REPORTE 30 DIAS - OCUPADO*/
  /********************************************/
    static void REPORTE_30DIAS_addData_OCUPADO (tREG_GENERIC* regPTR, tREG_CHOFER_30DIAS* newReg_PTR){    
      // Agrega datos de un registro OCUPADO al reporte de 30DIAS
      //
      // OCUPADO:
      //  - Km -> Km Ocupado
      newReg_PTR->KM_ocupado += ((tREG_OCUPADO*)regPTR)->km;
    }
    
  
  /* AGREGAR DATOS A REPORTE 30 DIAS - A PAGAR */
  /*********************************************/
    static void REPORTE_30DIAS_addData_A_PAGAR (tREG_GENERIC* regPTR, tREG_CHOFER_30DIAS* newReg_PTR){    
      // Agrega datos de un registro A PAGAR al reporte de 30DIAS
      //
      // A PAGAR(separados por tarifa):
      //  - Cantidad Viajes++
      //  - Fichas T 
      //  - Fichas D 
      //
      // EDIT 21/01/13
      //  Agregado del importe
      byte T;
      dword valor;
      dword nuevoValor;
      dword fichasT, fichasD;
      byte* ptr;
      
      #ifdef VISOR_TARIFACION_ESPECIAL
        if ((((tREG_A_PAGAR*)regPTR)->tarifa) == nroTARIFA_ESPECIAL){
          T = 8;                                      // Tarifa especial en array de 30dias
        }else{
          T  = (((tREG_A_PAGAR*)regPTR)->tarifa) - 1; // Le resto 1 xq va de 0-7
        }
      #endif
      
      #ifdef VISOR_TARIFACION_NORMAL
      T  = (((tREG_A_PAGAR*)regPTR)->tarifa) - 1;     // Le resto 1 xq va de 0-7
      #endif
            
      newReg_PTR->tarifa[T].viajes++;
      
      // Fichas Tiempo
      valor = newReg_PTR->tarifa[T].fichasTime[0];
      valor <<= 8;
      valor |= newReg_PTR->tarifa[T].fichasTime[1];
      valor <<= 8;
      valor |= newReg_PTR->tarifa[T].fichasTime[2];
      
      nuevoValor = ((tREG_A_PAGAR*)regPTR)->fichasTime[0];
      nuevoValor <<= 8;
      nuevoValor |= ((tREG_A_PAGAR*)regPTR)->fichasTime[1];
      nuevoValor <<= 8;
      nuevoValor |= ((tREG_A_PAGAR*)regPTR)->fichasTime[2];
      
      fichasT = nuevoValor; 
      
      valor += nuevoValor;
      
      ptr = &valor;
      ptr++;
      newReg_PTR->tarifa[T].fichasTime[0] = *ptr++;
      newReg_PTR->tarifa[T].fichasTime[1] = *ptr++;
      newReg_PTR->tarifa[T].fichasTime[2] = *ptr++;
      
      // Fichas Distancia
      valor = newReg_PTR->tarifa[T].fichasDist[0];
      valor <<= 8;
      valor |= newReg_PTR->tarifa[T].fichasDist[1];
      valor <<= 8;
      valor |= newReg_PTR->tarifa[T].fichasDist[2];
      
      nuevoValor = ((tREG_A_PAGAR*)regPTR)->fichasDist[0];
      nuevoValor <<= 8;
      nuevoValor |= ((tREG_A_PAGAR*)regPTR)->fichasDist[1];
      nuevoValor <<= 8;
      nuevoValor |= ((tREG_A_PAGAR*)regPTR)->fichasDist[2];
      
      fichasD = nuevoValor;
      
      valor += nuevoValor;
      
      ptr = &valor;
      ptr++;
      newReg_PTR->tarifa[T].fichasDist[0] = *ptr++;
      newReg_PTR->tarifa[T].fichasDist[1] = *ptr++;
      newReg_PTR->tarifa[T].fichasDist[2] = *ptr++;
      
      // Importe
      valor = newReg_PTR->tarifa[T].importe;
      nuevoValor = ((tREG_A_PAGAR*)regPTR)->importe;
      //nuevoValor = RELOJ_calcularImporte(fichasT, fichasD, 1, ((tREG_A_PAGAR*)regPTR)->tarifa, NULL);
      
      valor += nuevoValor;
      newReg_PTR->tarifa[T].importe = valor;
    }
    
  
  /* AGREGAR DATOS A REPORTE 30 DIAS - FUERA SERVICIO */
  /****************************************************/
    static void REPORTE_30DIAS_addData_FSERV (tREG_GENERIC* regPTR, tREG_CHOFER_30DIAS* newReg_PTR){    
      // Agrega datos de un registro FUERA SERVICIO al reporte de 30DIAS
      //
      // FUERA SERVICIO:
      //  - Km -> Km FServ
      newReg_PTR->KM_fserv += ((tREG_FSERV*)regPTR)->km;
    }    
  
  
  /* AGREGAR DATOS A REPORTE 30 DIAS - DESCONEXION ALIMENTACION */
  /**************************************************************/
    static void REPORTE_30DIAS_addData_DESCONEXION_ALIM (tREG_GENERIC* regPTR, tREG_CHOFER_30DIAS* newReg_PTR){    
      // Agrega datos de un registro DESCONEXION ALIMENTACION al reporte de 30DIAS
      //
      // DESCONEXION ALIMENTACION:
      //  - Cantidad Desconexiones++
      //  - Segundos Desconectado
      //  - Fichas T |
      //  - Fichas D +--->  Importe Perdido
      //  - Tarifa   |
      dword fichasT;
      dword fichasD;
      dword valor;
      dword nuevoValor;
      byte* ptr;
      
      fichasT = ((tREG_DESCONEXION_ALIM*)regPTR)->fichasTime[0];
      fichasT <<= 8;
      fichasT |= ((tREG_DESCONEXION_ALIM*)regPTR)->fichasTime[1];
      fichasT <<= 8;
      fichasT |= ((tREG_DESCONEXION_ALIM*)regPTR)->fichasTime[2];

      fichasD = ((tREG_DESCONEXION_ALIM*)regPTR)->fichasDist[0];
      fichasD <<= 8;
      fichasD |= ((tREG_DESCONEXION_ALIM*)regPTR)->fichasDist[1];
      fichasD <<= 8;
      fichasD |= ((tREG_DESCONEXION_ALIM*)regPTR)->fichasDist[2];
      
      newReg_PTR->cantDesconAlim++;
      
      newReg_PTR->importePerdido += ((tREG_DESCONEXION_ALIM*)regPTR)->importe;
      
      // Tiempo Desconectado
      valor = newReg_PTR->segDesconAlim[0];
      valor <<= 8;
      valor |= newReg_PTR->segDesconAlim[1];
      valor <<= 8;
      valor |= newReg_PTR->segDesconAlim[2];
      
      nuevoValor = ((tREG_DESCONEXION_ALIM*)regPTR)->seconds[0];
      nuevoValor <<= 8;
      nuevoValor |= ((tREG_DESCONEXION_ALIM*)regPTR)->seconds[1];
      nuevoValor <<= 8;
      nuevoValor |= ((tREG_DESCONEXION_ALIM*)regPTR)->seconds[2];
      
      valor += nuevoValor;
      
      ptr = &valor;
      ptr++;
      newReg_PTR->segDesconAlim[0] = *ptr++;
      newReg_PTR->segDesconAlim[1] = *ptr++;
      newReg_PTR->segDesconAlim[2] = *ptr++;
    }    
  
  
  /* AGREGAR DATOS A REPORTE 30 DIAS - SESION */
  /********************************************/
    static void REPORTE_30DIAS_addData_SESION (tREG_GENERIC* regPTR, tREG_CHOFER_30DIAS* newReg_PTR){    
      // Agrega datos de un registro SESION al reporte de 30DIAS
      // SESION
      // NO HAGO NADA!!!
    }


  /* AGREGAR DATOS A REPORTE 30 DIAS - DESCONEXION ACCESORIO */
  /***********************************************************/
    static void REPORTE_30DIAS_addData_DESCONEXION_ACC (tREG_GENERIC* regPTR, tREG_CHOFER_30DIAS* newReg_PTR){    
      // Agrega datos de un registro DESCONEXION ACCESORIO al reporte de 30DIAS
      //
      // DESCONEXION ACCESORIO:
      //  - Cantidad Desconexiones++
      //  - Segundos Desconectado
      //  - Fichas T |
      //  - Fichas D +--->  Importe Perdido
      //  - Tarifa   |
      dword fichasT;
      dword fichasD;
      dword valor;
      dword nuevoValor;
      byte* ptr;
      
      fichasT = ((tREG_DESCONEXION_ACC*)regPTR)->fichasTime[0];
      fichasT <<= 8;
      fichasT |= ((tREG_DESCONEXION_ACC*)regPTR)->fichasTime[1];
      fichasT <<= 8;
      fichasT |= ((tREG_DESCONEXION_ACC*)regPTR)->fichasTime[2];

      fichasD = ((tREG_DESCONEXION_ACC*)regPTR)->fichasDist[0];
      fichasD <<= 8;
      fichasD |= ((tREG_DESCONEXION_ACC*)regPTR)->fichasDist[1];
      fichasD <<= 8;
      fichasD |= ((tREG_DESCONEXION_ACC*)regPTR)->fichasDist[2];
      
      newReg_PTR->cantDesconAcc++;
      
      newReg_PTR->importePerdidoAcc += ((tREG_DESCONEXION_ACC*)regPTR)->importe;
      
      // Tiempo Desconectado
      valor = newReg_PTR->segDesconAcc[0];
      valor <<= 8;
      valor |= newReg_PTR->segDesconAcc[1];
      valor <<= 8;
      valor |= newReg_PTR->segDesconAcc[2];
      
      nuevoValor = ((tREG_DESCONEXION_ACC*)regPTR)->seconds[0];
      nuevoValor <<= 8;
      nuevoValor |= ((tREG_DESCONEXION_ACC*)regPTR)->seconds[1];
      nuevoValor <<= 8;
      nuevoValor |= ((tREG_DESCONEXION_ACC*)regPTR)->seconds[2];
      
      valor += nuevoValor;
      
      ptr = &valor;
      ptr++;
      newReg_PTR->segDesconAcc[0] = *ptr++;
      newReg_PTR->segDesconAcc[1] = *ptr++;
      newReg_PTR->segDesconAcc[2] = *ptr++;
    }


  /* AGREGAR DATOS A REPORTE 30 DIAS - VARIOS */
  /********************************************/
    static void REPORTE_30DIAS_addData_VARIOS (tREG_GENERIC* regPTR, tREG_CHOFER_30DIAS* newReg_PTR){    
      // Agrega datos de un registro SVARIOS al reporte de 30DIAS
      // VARIOS
      // NO HAGO NADA!!!
    }

#endif

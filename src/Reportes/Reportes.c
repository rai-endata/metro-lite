/* File: <Reportes.c> */
//#include "- Reloj-Blue Config -.h"
#include "Reportes.h"
#include "- metroBLUE Config -.h"
#include "tipo de equipo.h"

#ifdef RELOJ_DEBUG
 #include "printDEBUG.h"
#endif

/*
  Los reportes se trabajan de la siguiente manera:
  
  Hay una única tabla, que cumple la funcion de historial o bitácora. En ella, se van registrando
  los eventos a medida que suceden y en el orden correspondiente.
  De este modo, cada evento genera un registro en la tabla:
  
    +--------------+
    |  REGISTRO 1  |
    +--------------+
    |  REGISTRO 2  |
    +--------------+
    |  REGISTRO 3  |
    +--------------+
    |  REGISTRO 4  |
    +--------------+
  
  Estos registros tienen una longitud fija! Es decir que no importa si se trata de un LIBRE, un
  OCUPADO o una SESION, siempre se graba la misma cantidad de bytes.
  Con la salvedad de que para ciertos tipos de eventos, van a haber un par de bytes que no lleven
  informacion.
  
  Luego, existe un tipo de datos para cada tipo de registro, y a la hora de levantar los datos, 
  basta con hacer un cast al tipo de datos que se requiera
*/

/* INCLUDES */
/************/
  #ifdef VISOR_REPORTES

	#include "Reportes.h"
	#include "Flash.h"
	#include "Calculos.h"
	#include "Inicio.h"
	#include "Manejo de Buffers.h"
	#include "Reloj.h"
	#include "main.h"
	#include "Definicion de tipos.h"
	//#include "Param Reloj.h"
	#include "Grabaciones en EEPROM.h"
	#include "Parametros Reloj.h"
	#include "rtc.h"
	#include "inicio.h"
	#include "Ticket Turno.h"

  
  #ifdef VISOR_REPORTE_30DIAS
    #include "Reporte 30 Dias.h"
  #endif
  
  


/*********************************************************************************************/
/* PROTOTIPOS */
/**************/
  // RUTINAS INTERNAS
  static void REPORTES_ini (byte reset);
  static byte REPORTE_chkIntigrity (void);
  static byte queueNewReg (tREG_GENERIC* newReg);
  static byte chkQueueAvailable (void);
  static void incRegQueue_ptr (tREG_GENERIC** ptr_ptr);
  
  static byte REPORTES_finBackwardSearch (tREG_GENERIC** ptr_ptr);
  
  static void updateINDEX_inFlash (void);
  static void updateNroVIAJE_inFlash (byte type);
  static void updateNroTURNO_inFlash (byte type);
  static void updateIndexLastSESION_inFlash (byte type);
  static void updatePUTptr_inFlash (void);
  static void chkPerdidaDatosTurno (byte type);
  


/*********************************************************************************************/
/* VARIABLES */
/*************/
  tREG_GENERIC* REPORTE_PUTptr;
  static tREG_GENERIC newRegistro[dim_RegQueue];  // Registros encolados para ser grabados en EEPROM
  static tREG_GENERIC* newReg_PUTptr;             // Puntero de insercion en cola
  static tREG_GENERIC* newReg_GETptr;             // Puntero de extraccion de cola
  static byte newReg_qeueu_cnt;                   // Contador de registros encolados
  
  static tFLASH_ERROR REPORTE_flashError;
  
  static byte REPORTES_forcedReset;               // Reset Forzado de Reportes
  
  static uint8_t REPORTES_delay_cnt;                 // Delay de grabacion de EEPROM
    #define seg_delayReportes                   2
  
  uint8_t REPORTES_HABILITADOS;
  

// VARIABLES DE FLASH
/*
//#pragma CONST_SEG FLASH_DATA_PAGE
  const tREG_GENERIC FLASH_REPORTE[dim_REPORTE]={NULL};
  const uint32_t FLASH_REPORTE_PUT={NULL};
  const uint16_t FLASH_REPORTE_INDEX={NULL};
  const uint16_t FLASH_REPORTE_NRO_VIAJE={NULL};
  const uint16_t FLASH_REPORTE_NRO_TURNO={NULL};
  const uint32_t FLASH_INDEX_LAST_SESION={NULL};
*/

  //tREG_GENERIC EEPROM_REPORTE[dim_REPORTE]={NULL};
  uint32_t REPORTE_PUT;
  uint16_t REPORTE_INDEX;
  uint16_t REPORTE_NRO_VIAJE;
  uint16_t REPORTE_NRO_TURNO;
  uint16_t INDEX_LAST_SESION;


  tREG_LIBRE regLibre;

/*********************************************************************************************/
/* RUTINAS */
/***********/
  
  /* INICIALIZACION GLOBAL DE  REPORTES */
  /**************************************/
    void iniREPORTES (void){
      // Primero inicializo los reportes de la manera tradicional. Si luego de hcaerlo hay un error en los
      // punteros => Los REINICIALIZO correctamente
      
      #ifdef VISOR_REPORTES
       if (RELOJ_INTERNO && HORA_RTC){
          REPORTES_HABILITADOS = 1;
        }else{
          REPORTES_HABILITADOS = 0;
        }
      #else
        REPORTES_HABILITADOS = 0;
      #endif
      
      if (REPORTES_HABILITADOS){
        REPORTES_ini(0);                    // Incializacion de los Reportes [NORMAL]
        
        #ifdef VISOR_REPORTE_30DIAS
          REPORTE_30DIAS_ini(0);            // Incializacion del Reporte 30Dias [NORMAL]
        #endif
        
        //if (!chkVISOR_firstBoot){
        if(!VISOR_firstBoot){

          // Si no se trata del PRIMER ENCENDIDO => Chequeo la integridad de los punteros de los
          // reportes
          
          #ifdef VISOR_REPORTES
          if (!REPORTE_chkIntigrity()){
            // El puntero PUT esta en un rango invalido => Lo mas probable es que no se haya
            // tildado ELIMINAR REPORTES en el PROGRAMADOR, luego de cargar el firmware y programar
            // por primera vez
            // => Genero un FIRST BOOT para evitar problemas y muestro el aviso
            REPORTES_ini(1);                // Re-Incializacion de los Reportes
            
            REPORTES_forcedReset = 1;       // Para saber que hubo un reset forzado
           // mostrar_IAR();                  // Mostrar Aviso: Inicializacion Automatica de Reportes
            //_Error_Handler(__FILE__, __LINE__);
          }
          #endif
          
          #ifdef VISOR_REPORTE_30DIAS
          if (!REPORTE_30DIAS_chkIntigrity()){
            // El puntero PUT esta en un rango invalido => Lo mas probable es que no se haya
            // tildado ELIMINAR REPORTES en el PROGRAMADOR, luego de cargar el firmware y programar
            // por primera vez
            // => Genero un FIRST BOOT para evitar problemas y muestro el aviso
            REPORTE_30DIAS_ini(1);          // Re-Incializacion del Reporte 30Dias
            
            REPORTES_forcedReset = 1;       // Para saber que hubo un reset forzado
            mostrar_IAR();                  // Mostrar Aviso: Inicializacion Automatica de Reportes
          }
          #endif
        }
      }
    }

  /* INICIALIZACION DE REPORTES */
  /******************************/
    static void REPORTES_ini (byte reset){
      // La inicializacion de REPORTES, depende de si es la primera vez que arranca
      // el equipo o no
      //
      // Solo tiene sentido para RELOJ INTERNO
      byte data[2];
      byte* data_ptr;
      uint32_t putBKP;
      uint16_t valor;
      
       uint32_t* addressEEPROM_REPORTE_PUT;

      newReg_PUTptr = newRegistro;      // Inicializo puntero PUT de cola
      newReg_GETptr = newRegistro;      // Inicializo puntero GET de cola
      newReg_qeueu_cnt = 0;             // Reseteo contador de Registros encolados
      valor = 0;
      #ifdef VISOR_REPORTES
      if (REPORTES_HABILITADOS){
        //if (chkVISOR_firstBoot || reset){
        if (VISOR_firstBoot || reset){
          // PRIMER ENCENDIDO o RESET REPORTES
          REPORTE_PUTptr = (tREG_GENERIC*)ADDR_EEPROM_REPORTE;      // Inicializo puntero de insercion
          data_ptr = (byte*) &REPORTE_PUTptr;           			// Guardo puntero PUT en EEPROM
          //FLASH_setDontTouchIRQ();                        		// No modificar IRQ al grabar FLASH
          REPORTE_flashError = FLASH_updateSector((uint16_t*) ADDR_EEPROM_REPORTE_PUT, data_ptr, (uint16_t) 4); // Grabar PUT ptr
          //FLASH_setDontTouchIRQ();                      			// No modificar IRQ al grabar FLASH
          EEPROM_write(ADDR_EEPROM_REPORTE_INDEX, valor);
          //FLASH_setDontTouchIRQ();                      			// No modificar IRQ al grabar FLASH
          EEPROM_write(ADDR_EEPROM_REPORTE_NRO_VIAJE, valor);
          //FLASH_setDontTouchIRQ();                      			// No modificar IRQ al grabar FLASH
          EEPROM_write(ADDR_EEPROM_REPORTE_NRO_TURNO, valor);
        }
		//tomo datos de eeprom
		EEPROM_ReadBuffer(&REPORTE_PUTptr,ADDR_EEPROM_REPORTE_PUT,SIZE_EEPROM_REPORTE_PUT);
		EEPROM_ReadBuffer(&REPORTE_INDEX,ADDR_EEPROM_REPORTE_INDEX,SIZE_EEPROM_REPORTE_INDEX);
		EEPROM_ReadBuffer(&REPORTE_NRO_VIAJE,ADDR_EEPROM_REPORTE_NRO_VIAJE,SIZE_EEPROM_REPORTE_NRO_VIAJE);
		EEPROM_ReadBuffer(&REPORTE_NRO_TURNO, ADDR_EEPROM_REPORTE_NRO_TURNO,SIZE_EEPROM_REPORTE_NRO_TURNO);
		nroChofer = RELOJ_INTERNO_getChofer();
      }
      #endif
    }


  /* ENCOLAR NUEVO REGISTRO LIBRE */
  /********************************/
    byte REPORTE_queueLibre (tDATE date, byte chofer, uint16_t km, byte vel, uint16_t segP, uint16_t segM, byte asiento){
      // Siempre y cuando haya lugar en la cola, encola un nuevo registro LIBRE para ser
      // grabado en FLASH.
      // Devuelve "1" en caso de haber encolado, devuelve "0" si no pudo encolar
      //
      // Solo tiene sentido encolar un registro, si trabajo con RELOJ INTERNO

      byte queueDone;
      
      queueDone = 0;
      #ifdef VISOR_REPORTES
      if (REPORTES_HABILITADOS && dateValid(date)){
			/* regLibre
			+---+---+---+---+---+---+---+---+---+---+---+---+---+----+
			|   2   | 1 | 7 | 1 |   2   | 1 |   2   |   2   | 1 | 10 |
			+---+---+---+---+---+---+---+---+---+---+---+---+---+----+

			xx xx xx xx   xx xx xx xx    xx xx xx xx  xx xx xx xx   xx xx xx xx   xx xx xx xx
			_____ __ _________________________ __     _____ __      _____ _____   ___
			  |    |            |               |       |    |        |     |      |
			indice |           date           chofer   km    vel     segP  segM   asiento
				  tipo

			uint16_t   idx;            // Indice
			uint8_t    tipo;           // Tipo de registro
			tDATE      date;           // Date [fecha y hora]
			uint8_t    chofer;         // Chofer

			uint16_t   km;             // Km recorridos con 1 decimal
			uint8_t    velMax;         // Velocidad Maxima
			uint16_t   segParado;      // Segundos que permanecio Detenido
			uint16_t   segMarcha;      // Segundos que estuvo en Marcha
			uint8_t    sensor;         // Se ocupo por Sensor de Asiento
			uint8_t    empty[10];      // Relleno para longitud PAR */

    	regLibre.tipo = REG_libre;        // Tipo de registro
        regLibre.date = date;             // Fecha y Hora del Pase a Libre
        regLibre.chofer = chofer;         // Chofer
        regLibre.km = km;                 // KM Recorridos en Libre, con 1 decimal
        regLibre.velMax = vel;            // Velocidad Maxima en Libre
        regLibre.segParado = segP;        // Segundos que permanecio Detenido en Libre
        regLibre.segMarcha = segM;        // Segundos que estuvo en Movimiento en Libre
        regLibre.sensor = asiento;        // Indica si se ocupo por sensor de asiento
        
        queueDone = queueNewReg((tREG_GENERIC*) &regLibre);  // Intento encolar nuevo registro
      }else{
        queueDone = 0;
      }
      #endif
      
      return(queueDone);
    }

    tREG_OCUPADO regOcupa;

  /* ENCOLAR NUEVO REGISTRO OCUPADO */
  /********************************/
    byte REPORTE_queueOcupado (tDATE date, byte chofer, uint16_t km, byte vel, uint16_t segP, uint16_t segM){
      // Siempre y cuando haya lugar en la cola, encola un nuevo registro OCUPADO para ser
      // grabado en FLASH.
      // Devuelve "1" en caso de haber encolado, devuelve "0" si no pudo encolar
      //
      // Solo tiene sentido encolar un registro, si trabajo con RELOJ INTERNO
      byte queueDone;
      
      queueDone = 0;
      #ifdef VISOR_REPORTES
      if (REPORTES_HABILITADOS && dateValid(date)){

    	  /*
    	  		uint16_t   idx;            // Indice
    	  		uint8_t    tipo;           // Tipo de registro
    	  		tDATE      date;           // Date [fecha y hora]
    	  		uint8_t    chofer;         // Chofer
    	  		uint16_t   km;             // Km recorridos con 1 decimal
    	  		uint8_t    velMax;         // Velocidad Maxima
    	  		uint16_t   segParado;      // Segundos que permanecio Detenido
    	  		uint16_t   segMarcha;      // Segundos que estuvo en Marcha
    	  		uint8_t    empty[11];       // Relleno para longitud PAR

			+---+---+---+---+---+---+---+---+---+---+---+---+---+----+
			|   2   | 1 | 7 | 1 |   2   | 1 |   2   |   2   | 1 | 10 |
			+---+---+---+---+---+---+---+---+---+---+---+---+---+----+

			xx xx xx xx   xx xx xx xx    xx xx xx xx  xx xx xx xx   xx xx xx xx   xx xx xx xx
			_____ __ _________________________ __     _____ __      _____ _____
			  |    |            |               |       |    |        |     |
			indice |           date           chofer   km    vel     segP  segM
				  tipo
 */
    	regOcupa.tipo = REG_ocupado;      // Tipo de registro
        regOcupa.date = date;             // Fecha y Hora del Pase a Ocupado
        regOcupa.chofer = chofer;         // Chofer
        regOcupa.km = km;                 // KM Recorridos en Ocupado, con 1 decimal
        regOcupa.velMax = vel;            // Velocidad Maxima en Ocupado
        regOcupa.segParado = segP;        // Segundos que permanecio Detenido en Ocupado
        regOcupa.segMarcha = segM;        // Segundos que estuvo en Movimiento en Ocupado
        
        queueDone = queueNewReg((tREG_GENERIC*) &regOcupa);  // Intento encolar nuevo registro
      }else{
        queueDone = 0;
      }
      #endif
      
      return(queueDone);
    }    

    tREG_A_PAGAR regAPagar;

  /* ENCOLAR NUEVO REGISTRO A PAGAR */
  /**********************************/
    byte REPORTE_queueAPagar (tDATE date, byte chofer, byte nroViaje, byte tarifa, uint32_t fichasD, uint32_t fichasT, uint32_t valorViaje, uint32_t espera){
      // Siempre y cuando haya lugar en la cola, encola un nuevo registro A PAGAR para ser
      // grabado en FLASH.
      // Devuelve "1" en caso de haber encolado, devuelve "0" si no pudo encolar
      //
      // Solo tiene sentido encolar un registro, si trabajo con RELOJ INTERNO
      //
      // EDIT 21/01/13
      //  Agregado de IMPORTE aka Valor Viaje
      byte queueDone;
      byte* ptr;
      
      queueDone = 0;
      #ifdef VISOR_REPORTES
      if (REPORTES_HABILITADOS && dateValid(date)){
/*
 Apagar
+---+---+---+---+---+---+---+---+---+---+---+---+---+----+---+----+
|   2   | 1 | 7 | 1 |   2   | 1 |   3   |   3   |   2    |  2     |
+---+---+---+---+---+---+---+---+---+---+---+---+---+----+---+----+

xx xx xx xx   xx xx xx xx    xx xx xx xx  xx xx xx xx   xx xx xx xx   xx xx xx xx   xx xx xx xx
_____ __ _________________________ __     _____ __ __________ __________    _____   _____
  |    |            |               |       |    |    |          |            |       |
indice |           date           chofer  nroVje |  fichasD    fichasT      importe   |
      tipo										tarifa                              espera

 * */

    	regAPagar.tipo = REG_apagar;      // Tipo de registro
        regAPagar.date = date;            // Fecha y Hora del Pase a Ocupado
        regAPagar.chofer = chofer;        // Chofer
        regAPagar.nroViaje = nroViaje;    // Nº Correlativo de Viaje
        //cambiar cuando se agrega programador
        /*
        if(tarifa == 1){
        	regAPagar.tarifa = tarifa1D;
        }else if (tarifa==2){
        	regAPagar.tarifa =	tarifa1N;
        }else if (tarifa==3){
          	regAPagar.tarifa =	tarifa1D;
        }else if (tarifa==4){
          	regAPagar.tarifa =	tarifa1N;
        }*/

        regAPagar.tarifa = tarifa;
        
        //guarda en formato little indian
        ptr = (byte*) &fichasD;           // Puntero a fichas por Distancia
        regAPagar.fichasDist[2] = *ptr++; // Agrego fichasD
        regAPagar.fichasDist[1] = *ptr++; // Agrego fichasD
        regAPagar.fichasDist[0] = *ptr++; // Agrego fichasD
        
        //guarda en formato little indian
        ptr = (byte*) &fichasT;           // Puntero a fichas por Tiempo (unt32_t -> cuatro bytes)
        regAPagar.fichasTime[2] = *ptr++; // Agrego fichasT
        regAPagar.fichasTime[1] = *ptr++; // Agrego fichasT
        regAPagar.fichasTime[0] = *ptr++; // Agrego fichasT
        
        regAPagar.importe = valorViaje;   // Agrego Importe (Valor del Viaje)
        regAPagar.espera = espera;        // Agrego Espera (Segundos de Espera del viaje)
        
        queueDone = queueNewReg((tREG_GENERIC*) &regAPagar);  // Intento encolar nuevo registro
      }else{
        queueDone = 0;
      }
      #endif
      
      return(queueDone);
    }
  
  
  /* ENCOLAR NUEVO REGISTRO FUERA DE SERVICIO */
  /********************************************/
    byte REPORTE_queueFueraServ (tDATE date, byte chofer, uint16_t km, byte vel, uint16_t segP, uint16_t segM){
      // Siempre y cuando haya lugar en la cola, encola un nuevo registro FUERA DE SERVICIO para ser
      // grabado en FLASH.
      // Devuelve "1" en caso de haber encolado, devuelve "0" si no pudo encolar
      //
      // Solo tiene sentido encolar un registro, si trabajo con RELOJ INTERNO
      tREG_FSERV regFServ;
      byte queueDone;
      
      queueDone = 0;
      #ifdef VISOR_REPORTES
      if (REPORTES_HABILITADOS && dateValid(date)){
        regFServ.tipo = REG_fserv;        // Tipo de registro
        regFServ.date = date;             // Fecha y Hora del Pase a Fuera de Servicio
        regFServ.chofer = chofer;         // Chofer
        regFServ.km = km;                 // KM Recorridos en Fuera de Servicio, con 1 decimal
        regFServ.velMax = vel;            // Velocidad Maxima en Fuera de Servicio
        regFServ.segParado = segP;        // Segundos que permanecio Detenido en Fuera de Servicio
        regFServ.segMarcha = segM;        // Segundos que estuvo en Movimiento en Fuera de Servicio
        
        queueDone = queueNewReg((tREG_GENERIC*) &regFServ);  // Intento encolar nuevo registro
      }else{
        queueDone = 0;
      }
      #endif
      
      return(queueDone);
    }


  /* ENCOLAR NUEVO REGISTRO SESION */
  /*********************************/
    byte REPORTE_queueSesion (byte chofer){
      // Siempre y cuando haya lugar en la cola, encola un nuevo registro SESION para ser
      // grabado en FLASH.
      // Devuelve "1" en caso de haber encolado, devuelve "0" si no pudo encolar
      //
      // El NroTURNO se lo agrego justo antes de grabar la flash, por si hay varios encolados
      //
      // Solo tiene sentido encolar un registro, si trabajo con RELOJ INTERNO
      tREG_SESION regSesion;
      byte queueDone;
      
      queueDone = 0;
      #ifdef VISOR_REPORTES
      if (REPORTES_HABILITADOS && dateValid(getDate())){
        regSesion.tipo = REG_sesion;       // Tipo de registro
        regSesion.date = *DATE_ptr;        // Fecha y Hora actual
        regSesion.chofer = chofer;         // Chofer
        //string_FAR_copy(regSesion.choferSys, LOGUEO_getChoferLogueado()); 	// Chofer logueado al Sistema en ASCII
        queueDone = queueNewReg((tREG_GENERIC*) &regSesion);  				// Intento encolar nuevo registro
      }else{
        queueDone = 0;
      }
      #endif
      
      return(queueDone);
    }


  /* ENCOLAR NUEVO REGISTRO DESCONEXION DE ALIMENTACION */
  /******************************************************/
    byte REPORTE_queueDesconexionAlim (tDATE dateApagado, byte chofer, byte tarifa, uint32_t fichasD, uint32_t fichasT, uint32_t importe){
      // Siempre y cuando haya lugar en la cola, encola un nuevo registro DESCONEXION ALIMENTACION
      // para ser grabado en FLASH.
      // Devuelve "1" en caso de haber encolado, devuelve "0" si no pudo encolar
      //
      // Solo tiene sentido encolar un registro, si trabajo con RELOJ INTERNO
      //
      // EDIT 21/01/13
      //  Agregado de IMPORTE
      tREG_DESCONEXION_ALIM regDesconexionAlim;
      byte queueDone;
      byte* ptr;
      uint32_t segundosDescon;
      uint32_t difDias;

      queueDone = 0;
      #ifdef VISOR_REPORTES
      if (REPORTES_HABILITADOS && dateValid(dateApagado)){
        regDesconexionAlim.tipo = REG_desconexAlim; // Tipo de registro
        regDesconexionAlim.date = dateApagado;      // Fecha y Hora de desconexion
        regDesconexionAlim.chofer = chofer;         // Chofer
        regDesconexionAlim.tarifa = tarifa;         // Tarifa
        
        ptr = (byte*) &fichasD;                     // Puntero a fichas por Distancia
        //ptr++;                                      // Me interesan los 3 ultimos byte => avanzo puntero
        regDesconexionAlim.fichasDist[2] = *ptr++;  // Agrego fichasD
        regDesconexionAlim.fichasDist[1] = *ptr++;  // Agrego fichasD
        regDesconexionAlim.fichasDist[0] = *ptr++;  // Agrego fichasD
        
        ptr = (byte*) &fichasT;                     // Puntero a fichas por Tiempo
       // ptr++;                                      // Me interesan los 3 ultimos byte => avanzo puntero
        regDesconexionAlim.fichasTime[2] = *ptr++;  // Agrego fichasT
        regDesconexionAlim.fichasTime[1] = *ptr++;  // Agrego fichasT
        regDesconexionAlim.fichasTime[0] = *ptr++;  // Agrego fichasT
        
        difDias = restaDate(getDate(), dateApagado);      // Diferencia de Dias
        segundosDescon = diferenicaDias_toSeconds(difDias, getDate().hora, dateApagado.hora);
        ptr = (byte*) &segundosDescon;              // Puntero a segundos desconectado
        //ptr++;                                      // Me interesan los 3 ultimos byte => avanzo puntero
        regDesconexionAlim.seconds[2] =  *ptr++;    // Segundos deconectado
        regDesconexionAlim.seconds[1] =  *ptr++;    // Segundos deconectado
        regDesconexionAlim.seconds[0] =  *ptr++;    // Segundos deconectado
        
        regDesconexionAlim.importe = importe;       // Agrego importe perdido
              
        queueDone = queueNewReg((tREG_GENERIC*) &regDesconexionAlim);  // Intento encolar nuevo registro
      }else{
        queueDone = 0;
      }
      #endif
      
      return(queueDone);
    }


  /* ENCOLAR NUEVO REGISTRO DESCONEXION DE ACCESORIO */
  /***************************************************/
    byte REPORTE_queueDesconexionAcc (tDATE dateDescon, byte chofer, byte tarifa, uint32_t fichasD, uint32_t fichasT, uint32_t importe){
      // Siempre y cuando haya lugar en la cola, encola un nuevo registro DESCONEXION ACCESORIO
      // para ser grabado en FLASH.
      // Devuelve "1" en caso de haber encolado, devuelve "0" si no pudo encolar
      //
      // Solo tiene sentido encolar un registro, si trabajo con RELOJ INTERNO
      tREG_DESCONEXION_ACC regDesconexionAcc;
      byte queueDone;
      byte* ptr;
      uint32_t segundosDescon;
      uint32_t difDias;

      queueDone = 0;
      #ifdef VISOR_REPORTES
      if (REPORTES_HABILITADOS && dateValid(dateDescon)){
        regDesconexionAcc.tipo = REG_desconexAcc;   // Tipo de registro
        regDesconexionAcc.date = dateDescon;        // Fecha y Hora de desconexion de accesorio
        regDesconexionAcc.chofer = chofer;          // Chofer
        regDesconexionAcc.tarifa = tarifa;          // Tarifa
        
        ptr = (byte*) &fichasD;                     // Puntero a fichas por Distancia
        ptr++;                                      // Me interesan los 3 ultimos byte => avanzo puntero
        regDesconexionAcc.fichasDist[0] = *ptr++;   // Agrego fichasD
        regDesconexionAcc.fichasDist[1] = *ptr++;   // Agrego fichasD
        regDesconexionAcc.fichasDist[2] = *ptr++;   // Agrego fichasD
        
        ptr = (byte*) &fichasT;                     // Puntero a fichas por Tiempo
        ptr++;                                      // Me interesan los 3 ultimos byte => avanzo puntero
        regDesconexionAcc.fichasTime[0] = *ptr++;   // Agrego fichasT
        regDesconexionAcc.fichasTime[1] = *ptr++;   // Agrego fichasT
        regDesconexionAcc.fichasTime[2] = *ptr++;   // Agrego fichasT
        
        difDias = restaDate(getDate(), dateDescon);      // Diferencia de Dias
        segundosDescon = diferenicaDias_toSeconds(difDias, getDate().hora, dateDescon.hora);
        ptr = (byte*) &segundosDescon;              // Puntero a segundos desconectado
        ptr++;                                      // Me interesan los 3 ultimos byte => avanzo puntero
        regDesconexionAcc.seconds[0] =  *ptr++;     // Segundos deconectado
        regDesconexionAcc.seconds[1] =  *ptr++;     // Segundos deconectado
        regDesconexionAcc.seconds[2] =  *ptr++;     // Segundos deconectado
        
        regDesconexionAcc.importe = importe;        // Agrego importe perdido
              
        queueDone = queueNewReg((tREG_GENERIC*) &regDesconexionAcc);  // Intento encolar nuevo registro
      }else{
        queueDone = 0;
      }
      #endif
      
      return(queueDone);
    }


  /* ENCOLAR NUEVO REGISTRO VARIOS */
  /*********************************/
    byte REPORTE_queueVarios (tDATE date, byte chofer, byte sinPulsos, byte prog){
      // Siempre y cuando haya lugar en la cola, encola un nuevo registro VARIOS
      // para ser grabado en FLASH.
      // Devuelve "1" en caso de haber encolado, devuelve "0" si no pudo encolar
      //
      // Solo tiene sentido encolar un registro, si trabajo con RELOJ INTERNO
      tREG_VARIOS regVarios;
      byte queueDone;
      
      queueDone = 0;
      #ifdef VISOR_REPORTES
      if (REPORTES_HABILITADOS && dateValid(date)){
      //if (REPORTES_HABILITADOS){
        regVarios.tipo = REG_varios;        // Tipo de registro
        regVarios.date = date;              // Fecha y Hora
        regVarios.chofer = chofer;          // Chofer
        
        regVarios.movSinPulsos = sinPulsos; // Movimiento sin pulsos
        regVarios.programacion = prog;      // Programacion
              
        queueDone = queueNewReg((tREG_GENERIC*) &regVarios);  // Intento encolar nuevo registro
      }else{
        queueDone = 0;
      }
      #endif
      
      return(queueDone);
    }        
        

  /* GRABAR REGISTRO DE REPORTES EN FLASH */
  /****************************************/
    void REPORTES_grabarFlash (void){
      // Llamada desde el LOOP principal.
      // Por mas que haya mas de un registro encolado, graba de a uno (sino requiere que use mas RAM)
      // En caso de que se den las condiciones para grabar la FLASH, y haya registros encolados, 
      // inicio la grabacion.
      //
      // Solo tiene sentido grabar la FLASH, si trabajo con RELOJ INTERNO
      //
      // EDIT 04/04/2013
      //  Nueva bandera/contador de bloqueo de grabacion, para demorar la misma
      uint16_t indice;
      uint16_t NroViaje;
      uint16_t NroTurno;
      byte tipo;
      



      uint32_t valor;

   	  uint8_t bufferPRINT[50];
      uint16_t ASCII;
      byte high;
      byte low;
      tREG_GENERIC Registro_En_EEPROM[1];
      tREG_GENERIC* newReg_GETptrEEPROM;

      #ifdef VISOR_REPORTES

      //addressEEPROM_REPORTE_NRO_VIAJE = (uint16_t*)ADDR_EEPROM_REPORTE_NRO_VIAJE;
      //addressEEPROM_REPORTE_NRO_TURNO = (uint16_t*)ADDR_EEPROM_REPORTE_NRO_TURNO;
      //addressEEPROM_REPORTE_INDEX = (uint16_t*)ADDR_EEPROM_REPORTE_INDEX;


      if (REPORTES_HABILITADOS && (newReg_qeueu_cnt > 0) /*&& (REPORTES_delay_cnt == 0)*/ && FLASH_chkCanUpdate()){
        newReg_qeueu_cnt--;               // Decremento contador de registros encolados
        EEPROM_ReadBuffer(&REPORTE_NRO_VIAJE,ADDR_EEPROM_REPORTE_NRO_VIAJE,SIZE_EEPROM_REPORTE_NRO_VIAJE);
        EEPROM_ReadBuffer(&REPORTE_NRO_TURNO,ADDR_EEPROM_REPORTE_NRO_TURNO,SIZE_EEPROM_REPORTE_NRO_TURNO);
        EEPROM_ReadBuffer(&REPORTE_INDEX,ADDR_EEPROM_REPORTE_INDEX,SIZE_EEPROM_REPORTE_INDEX);


        if (newReg_GETptr->tipo == REG_apagar){
          // TIPO A PAGAR => Agrego Nro de Viaje
          NroViaje = REPORTE_NRO_VIAJE;
          ((tREG_A_PAGAR*far)newReg_GETptr)->nroViaje = NroViaje;
        
        }else if (newReg_GETptr->tipo == REG_sesion){
          // TIPO SESION => Agrego Nro de Turno
        updateNroTURNO_inFlash(newReg_GETptr->tipo);         // Actualizacion de NRO TURNO
        //EEPROM_ReadBuffer(&REPORTE_NRO_TURNO,ADDR_EEPROM_REPORTE_NRO_TURNO,SIZE_EEPROM_REPORTE_NRO_TURNO);
        NroTurno = REPORTE_NRO_TURNO;
          ((tREG_SESION*far)newReg_GETptr)->nroTurno = NroTurno;
          //debug
		#ifdef RELOJ_DEBUG
          if((REPORTE_PUTptr < ADDR_EEPROM_REPORTE) || (REPORTE_PUTptr > (ADDR_EEPROM_REPORTE+ dim_REPORTE))){
        	  NroTurno = 12345;
          }
        #endif
        }
        
        indice = REPORTE_INDEX;       									// Extraigo indice de FLASH
        newReg_GETptr->idx = indice;        							// Agrego INDICE a registro a grabar
    	(void)EEPROM_WriteBuffer( newReg_GETptr, REPORTE_PUTptr, sizeof(tREG_GENERIC));

#ifdef RELOJ_DEBUG
        //imprime direccion de tabla
		newReg_GETptrEEPROM = &Registro_En_EEPROM;
		EEPROM_ReadBuffer(newReg_GETptrEEPROM,REPORTE_PUTptr,sizeof(tREG_GENERIC));
		printDIR_REPORTES(&bufferPRINT);
#endif

        #ifdef VISOR_REPORTE_30DIAS
          REPORTE_30DIAS_addData(newReg_GETptr);  // Agrego datos a reporte de 30dias
        #endif
        
        // Una vez finalizada la grabacion de FLASH:
        //  - Avanzo puntero de grabacion en FLASH
        //  - Guardo indice de ultima sesion en FLASH [si se trata de un SESION]
        //  - Incremento indice en FLASH
        //  - Incremento Nro de Turno en FLASH [si se trata de un SESION]
        //  - Incremento Nro de Viaje en FLASH [si se trata de un A PAGAR]
        //  - Avanzo puntero de extraccion de cola (lo tengo q incrementar al final xq sino pierdo el tipo)
        tipo = newReg_GETptr->tipo;           // Extraigo tipo para no perderlo
        updatePUTptr_inFlash();               // Actualizacion de Puntero de Grabacion
        updateIndexLastSESION_inFlash(tipo);  // Actualizacion de INDICE DE ULTIMA SESION
        updateINDEX_inFlash();                // Actualizacion de INDICE
        updateNroVIAJE_inFlash(tipo);         // Actualizacion de NRO VIAJE


#ifdef RELOJ_DEBUG

        //REGISTRO GUARDADO
        //printCabecera_REGISTER_TAB_REPORTES(newReg_GETptrEEPROM);
        if((newReg_GETptrEEPROM->tipo == REG_libre) || (newReg_GETptrEEPROM->tipo == REG_ocupado) || (newReg_GETptrEEPROM->tipo == REG_fserv)){
        	printLiOcFs_REGISTER_TAB_REPORTES(newReg_GETptrEEPROM);
        }else if((newReg_GETptrEEPROM->tipo == REG_apagar)){
        	//printA_PAGAR_REGISTER_TAB_REPORTES(newReg_GETptrEEPROM);
        	printA_PAGAR_REGISTER_TAB_REPORTES((tREG_A_PAGAR*)newReg_GETptrEEPROM,(newReg_GETptrEEPROM->empty+14));

        }else if((newReg_GETptrEEPROM->tipo == REG_sesion)){
            printSESIONS_REGISTER_TAB_REPORTES(newReg_GETptrEEPROM);
        }else{
        	printLiOcFs_REGISTER_TAB_REPORTES(newReg_GETptrEEPROM);
        }

#endif
        
        incRegQueue_ptr(&newReg_GETptr);      // Avanzo puntero de extraccion de cola
        
        // Luego, debo verificar que si el tipo esta trabajando con un UNICO chofer y no cierra
        // nunca el turno, no pierda los datos.
        chkPerdidaDatosTurno(tipo);           // Chequear si estoy por perder datos de turno


      }
      #endif
    }


  /* AVANZAR PUNTERO DE REPORTE EN FLASH */
  /***************************************/    
    void incFlashRep_ptr (tREG_GENERIC*far* ptr_ptr){
      tREG_GENERIC*far ptr;
      
      ptr = *ptr_ptr;                 // Puntero
      ptr++;                          // Avanzo puntero
      
      if(ptr >= (ADDR_EEPROM_REPORTE + dim_REPORTE)){
        ptr = ADDR_EEPROM_REPORTE;          // Doy la vuelta al reporte
      }
      
      *ptr_ptr = ptr;                 // Actualizo puntero REAL
    }

  
  /* RETROCEDER PUNTERO DE REPORTE EN FLASH */
  /******************************************/    
    void decFlashRep_ptr (tREG_GENERIC*far* ptr_ptr){
      tREG_GENERIC*far ptr;
      
      ptr = *ptr_ptr;                 // Puntero
      
      if(ptr <= ADDR_EEPROM_REPORTE){
        ptr = (ADDR_EEPROM_REPORTE + dim_REPORTE);  // Doy la vuelta al reporte
      }

      ptr--;                          // Retrocedo puntero
      
      *ptr_ptr = ptr;                 // Actualizo puntero REAL
    }


  /* DETERMINAR FIN REPORTE */
  /**************************/
    byte REPORTES_determineFin (tREG_GENERIC*far* ptr_ptr){
      // Esta rutina se llama desde la impresion de los, para saber
      // si debo seguir retrocediendo el puntero o si ya termino el reporte
      return(REPORTES_finBackwardSearch(ptr_ptr));
    }    


  /* DETERMINAR SI OCURRIO UN RESET FORZADO DE REPORTES */
  /******************************************************/
    byte REPORTES_chkForcedReset (void){
      return(REPORTES_forcedReset);
    }
    

/*********************************************************************************************/
/* EXTRACCION DE DATOS */
/***********************/

  /* EXTRAER PUNTERO PUT */
  /***********************/
    tREG_GENERIC*far REPORTES_getPUT_ptr (void){
      return(REPORTE_PUTptr);
    }
  
  
  /* OBTENER PUNTERO A REGISTRO MAS CERCANO AL ACTUAL SEGUN TIPO */
  /***************************************************************/
    tREG_GENERIC*far REPORTES_getPrevRegister_byType (byte tipo){
      // Esta funcion nos devuelve un puntero al registro anterior-más-proximo del tipo
      // especificado.
      // Como se trata del ANTERIOR-MAS-PROXIMO, la búsqueda es SIEMPRE hacia atras, por
      // en ese sentido obtengo el historial más reciente.
      // La busqueda no es eterna, si no que se hace hasta el fin de busqueda
      byte exit;
      tREG_GENERIC* GET_ptr;
      tREG_GENERIC aux_GET;
      
      GET_ptr = REPORTE_PUTptr;               // Comienzo desde el puntero actual
      decFlashRep_ptr(&GET_ptr);              // Retrocedo puntero
      
      EEPROM_ReadBuffer(&aux_GET,GET_ptr,sizeof(tREG_GENERIC));
      exit = 0;
      dispararTO_lazo();
      while ((aux_GET.tipo != tipo) && !exit){
        // Continuo retrocediendo el puntero hasta encontrar (por primera vez) el
        // tipo especificado => Ahi ya habria encontrada el registro anterior mas
        // proximo.
        // O hasta que se cumpla la condicion de fin de busqueda
        exit = REPORTES_finBackwardSearch(&GET_ptr);  	// Fin de Busqueda Hacia Atras
        
        decFlashRep_ptr(&GET_ptr);            			// Retrocedo puntero
        
        if (chkTO_lazo_F() == 1){
          exit = 1;
        }else{
        	EEPROM_ReadBuffer(&aux_GET,GET_ptr,sizeof(tREG_GENERIC));
        }

      }
      detenerTO_lazo();
      
      if (exit){
        GET_ptr = NULL;                       // ERROR
      }

      return(GET_ptr);
    }
  
  
  /* EXTRAER FICHAS DE TIEMPO DE REGISTRO */
  /****************************************/
    uint32_t REPORTE_getFichasT_fromRegistry (tREG_GENERIC*far REG_ptr, byte type){
      // Esta funcion devuelve FICHAS DE TIEMPO del registro <tipo> pasado como argumento.
      // Previamente verifica que el puntero sea valido
      uint32_t fichas;

      tREG_GENERIC aux_REG;
      tREG_GENERIC* aux_REG_ptr;

      EEPROM_ReadBuffer(&aux_REG,REG_ptr,sizeof(tREG_GENERIC));
      aux_REG_ptr = &aux_REG;
      
      if(REPORTE_chkPointer(REG_ptr)){

    	if (type == REG_apagar){
          fichas = ((tREG_A_PAGAR*)aux_REG_ptr)->fichasTime[0];
          fichas <<= 8;
          fichas |= ((tREG_A_PAGAR*)aux_REG_ptr)->fichasTime[1];
          fichas <<= 8;
          fichas |= ((tREG_A_PAGAR*)aux_REG_ptr)->fichasTime[2];
        
        }else if (type == REG_desconexAlim){
          fichas = ((tREG_DESCONEXION_ALIM*)aux_REG_ptr)->fichasTime[0];
          fichas <<= 8;
          fichas |= ((tREG_DESCONEXION_ALIM*)aux_REG_ptr)->fichasTime[1];
          fichas <<= 8;
          fichas |= ((tREG_DESCONEXION_ALIM*)aux_REG_ptr)->fichasTime[2];
        }
      }else{
        fichas = 0;
      }
      
      return(fichas);
    }


  /* EXTRAER FICHAS DE DISTANCIA DE REGISTRO */
  /*******************************************/
    uint32_t REPORTE_getFichasD_fromRegistry (tREG_GENERIC*far REG_ptr, byte type){
      // Esta funcion devuelve FICHAS DE DISTANCIA del registro <tipo> pasado como argumento.
      // Previamente verifica que el puntero sea valido
      // Solo soporta tipo:
      //  - A PAGAR
      //  - DESCONEXION ALIMENTACION
      uint32_t fichas;

      tREG_GENERIC aux_REG;
      tREG_GENERIC* aux_REG_ptr;
      
      EEPROM_ReadBuffer(&aux_REG,REG_ptr,sizeof(tREG_GENERIC));
      aux_REG_ptr = &aux_REG;

      if(REPORTE_chkPointer(REG_ptr)){
        if (type == REG_apagar){
          fichas = ((tREG_A_PAGAR*)aux_REG_ptr)->fichasDist[0];
          fichas <<= 8;
          fichas |= ((tREG_A_PAGAR*)aux_REG_ptr)->fichasDist[1];
          fichas <<= 8;
          fichas |= ((tREG_A_PAGAR*)aux_REG_ptr)->fichasDist[2];
        
        }else if (type == REG_desconexAlim){
          fichas = ((tREG_DESCONEXION_ALIM*)aux_REG_ptr)->fichasDist[0];
          fichas <<= 8;
          fichas |= ((tREG_DESCONEXION_ALIM*)aux_REG_ptr)->fichasDist[1];
          fichas <<= 8;
          fichas |= ((tREG_DESCONEXION_ALIM*)aux_REG_ptr)->fichasDist[2];
        }
      }else{
        fichas = 0;
      }
      
      return(fichas);
    }    


  /* EXTRAER Nº TARIFA DE REGISTRO */
  /*********************************/
    byte REPORTE_getNroTarifa_fromRegistry (tREG_GENERIC*far REG_ptr, byte type){
      // Esta funcion devuelve la TARIFA del registro <tipo> pasado como argumento.
      // Previamente verifica que el puntero sea valido
      // Solo soporta tipo:
      //  - A PAGAR
      //  - DESCONEXION ALIMENTACION
      byte tarifa;

      tREG_GENERIC aux_REG;
      tREG_GENERIC* aux_REG_ptr;
      
      EEPROM_ReadBuffer(&aux_REG,REG_ptr,sizeof(tREG_GENERIC));
      aux_REG_ptr = &aux_REG;

      	tarifa = 0;
		if (type == REG_apagar){
		  tarifa = ((tREG_A_PAGAR*)aux_REG_ptr)->tarifa;
		}else if (type == REG_desconexAlim){
		  tarifa = ((tREG_DESCONEXION_ALIM*)aux_REG_ptr)->tarifa;
		}else if (type == REG_desconexAcc){
		  tarifa = ((tREG_DESCONEXION_ACC*)aux_REG_ptr)->tarifa;
		}
      return(tarifa);
    } 


  /* EXTRAER IMPORTE DE REGISTRO */
  /*******************************/
    uint32_t REPORTE_getImporte_fromRegistry (tREG_GENERIC*far REG_ptr, byte type){
      // Esta funcion devuelve IMPORTE del registro <tipo> pasado como argumento.
      // Previamente verifica que el puntero sea valido
      // Solo soporta tipo:
      //  - A PAGAR
      //  - DESCONEXION ALIMENTACION
      uint32_t importe;
      uint32_t importe_aux;
      tREG_GENERIC aux_REG;
      tREG_GENERIC* aux_REG_ptr;
      
      EEPROM_ReadBuffer(&aux_REG,REG_ptr,sizeof(tREG_GENERIC));
      aux_REG_ptr = &aux_REG;

      if(REPORTE_chkPointer(REG_ptr)){
        if (type == REG_apagar){
          importe = ((tREG_A_PAGAR*)aux_REG_ptr)->importe;

        }else if (type == REG_desconexAlim){
          importe = ((tREG_DESCONEXION_ALIM*)aux_REG_ptr)->importe;
        
        }else if (type == REG_desconexAcc){
          importe = ((tREG_DESCONEXION_ACC*)aux_REG_ptr)->importe;
        }
      }else{
        importe = 0;
      }
     // convert_bigINDIAN_to_litleINDIAN (&importe, 4);
      return(importe);
    }


  /* EXTRAER ESPERA DE A PAGAR */
  /*****************************/
    uint32_t REPORTE_getEspera_fromAPagar (tREG_A_PAGAR*far REG_ptr){
      // Esta funcion devuelve ESPERA del registro pasado como argumento.
      // Previamente verifica que el puntero sea valido
      // Solo soporta tipo:
      //  - A PAGAR
      uint32_t espera;

      tREG_GENERIC aux_REG;
      tREG_GENERIC* aux_REG_ptr;

      EEPROM_ReadBuffer(&aux_REG,REG_ptr,sizeof(tREG_GENERIC));
      aux_REG_ptr = &aux_REG;
      
      if(REPORTE_chkPointer(REG_ptr)){
        espera = ((tREG_A_PAGAR*)aux_REG_ptr)->espera;
      }else{
        espera = 0;
      }
      
      return(espera);
    }        


  /* OBTENER CANTIDAD DE VIAJES EN TARIFA SEGUN RANGO */
  /****************************************************/
    byte REPORTES_getCantidadViajes_byRango (tREG_GENERIC*far INI_ptr, tREG_GENERIC*far FIN_ptr, byte type, byte tarifa){
      // Esta funcion devuelve la cantidad de viajes en tarifa, desde el registro apuntando por INI_ptr
      // hasta el apuntado por FIN_ptr.
      // Por cada registro <tipo> que se encuentre con esa tarifa, es un viaje
      //
      // Como a esta rutina se ingresa con punteros, debo verificar que los mismos apunten
      // dentro de la tabla de reportes
      // Solo soporta tipo:
      //  - A PAGAR
      //  - DESCONEXION ALIMENTACION
      byte viajes;
      byte pointer_ok;
      byte TO_F;
      tREG_GENERIC aux_INI;
      tREG_GENERIC* aux_INI_ptr;
      byte tarif1, tarif2;


      pointer_ok = REPORTE_chkPointer(INI_ptr);   // Verifico que el puntero de inicio sea correcto

      if(pointer_ok){
        pointer_ok = REPORTE_chkPointer(FIN_ptr); // Verifico que el puntero de fin sea correcto
      }
      viajes = 0;                                 // Reseteo variable
      if(pointer_ok){
        // Solo proceso si los punteros son correctos
        TO_F = 0;                                 // Reseteo Bandera de TimeOut
        dispararTO_lazo();                        // Disparo Time Out de Lazo
        while ((INI_ptr != FIN_ptr) ){

          TO_F = chkTO_lazo_F();                  // Chequeo bandera de time out de lazo
          if(checkRANGE(INI_ptr, FIN_TABLA_REPORTE, TO_F )){break;}

          EEPROM_ReadBuffer(&aux_INI,INI_ptr,sizeof(tREG_GENERIC));
          if (aux_INI.tipo == type){
        	aux_INI_ptr = &aux_INI;
           	tarif1 = ((tREG_A_PAGAR*)aux_INI_ptr)->tarifa;
			if (type == REG_apagar){
				tarif2 = ((tREG_A_PAGAR*)aux_INI_ptr)->tarifa;
			}else if (type == REG_desconexAlim){
				tarif2 = ((tREG_DESCONEXION_ALIM*)aux_INI_ptr)->tarifa;
			}else if (type == REG_desconexAcc){
				tarif2 = ((tREG_DESCONEXION_ACC*)aux_INI_ptr)->tarifa;
			}
           	//tarif2 = REPORTE_getNroTarifa_fromRegistry(INI_ptr,type);

        	if (tarif2 == tarifa){
        	//if (REPORTE_getNroTarifa_fromRegistry(INI_ptr,type) == tarifa){
              // Como se trata de un registro <tipo> en esa tarifa => cuento un viaje
              viajes++;
            }
          }
          incFlashRep_ptr(&INI_ptr);              // Avanzo puntero
        }
        detenerTO_lazo();                         // Detengo Time Out de Lazo
      }
      
      return(viajes);
    }    


  /* OBTENER CANTIDAD DE VIAJES POR ASIENTO SEGUN RANGO Y VALOR */
  /**************************************************************/
    byte REPORTES_getCantidadViajesAsiento_byRango_Value (tREG_GENERIC*far INI_ptr, tREG_GENERIC*far FIN_ptr, byte value){
      // Esta funcion devuelve la cantidad de viajes por asiento, desde el registro apuntando por INI_ptr
      // hasta el apuntado por FIN_ptr.
      // Como a esta rutina se ingresa con punteros, debo verificar que los mismos apunten
      // dentro de la tabla de reportes
      //
      // EDIT 02/11/12
      //  Como ahora tambien se indica si fue un pase por sensor presionado (funcionamiento normal) o
      // por sensor desconectado (error), solo cuento las veces que fue igual al valor pasado como argumento
      byte viajes;
      byte pointer_ok;
      byte TO_F;
      tREG_GENERIC aux_INI;
      tREG_GENERIC* aux_INI_ptr;
      
      pointer_ok = REPORTE_chkPointer(INI_ptr);   // Verifico que el puntero de inicio sea correcto
      

      if(pointer_ok){
        pointer_ok = REPORTE_chkPointer(FIN_ptr); // Verifico que el puntero de fin sea correcto
      }
      viajes = 0;                                 // Reseteo variable
      if(pointer_ok){
        // Solo proceso si los punteros son correctos
        TO_F = 0;                                 // Reseteo Bandera de TimeOut
        dispararTO_lazo();                        // Disparo Time Out de Lazo
        while ((INI_ptr != FIN_ptr)){

          TO_F = chkTO_lazo_F();                  // Chequeo bandera de time out de lazo
          if(checkRANGE(INI_ptr, FIN_TABLA_REPORTE, TO_F )){break;}

          EEPROM_ReadBuffer(&aux_INI,INI_ptr,sizeof(tREG_GENERIC));
          if (aux_INI.tipo == REG_libre){
        	  aux_INI_ptr = &aux_INI;
            if (((tREG_LIBRE*)aux_INI_ptr)->sensor == value){
              // Incremento cantidad de viajes, por el motivo especificado
              viajes++;
            }
          }
          incFlashRep_ptr(&INI_ptr);              // Avanzo puntero
        }
        detenerTO_lazo();                         // Detengo Time Out de Lazo
      }
      
      return(viajes);
    }    
    
    
  /* OBTENER CANTIDAD DE FICHAS DE TIEMPO EN TARIFA SEGUN RANGO y TIPO */
  /*********************************************************************/
    uint32_t REPORTES_getFichasTime_byRango (tREG_GENERIC*far INI_ptr, tREG_GENERIC*far FIN_ptr, byte type, byte tarifa){
      // Esta funcion devuelve las fichas de tiempo, desde el registro apuntando por INI_ptr
      // hasta el apuntado por FIN_ptr.
      // Por cada registro <tipo> que se encuentre, se van acumulando las fichas por tiempo
      //
      // Como a esta rutina se ingresa con punteros, debo verificar que los mismos apunten
      // dentro de la tabla de reportes
      // Solo soporta tipo:
      //  - A PAGAR
      //  - DESCONEXION ALIMENTACION
      uint32_t fichas;
      uint32_t fichas_aux;
      byte pointer_ok;
      byte TO_F;
      tREG_GENERIC aux_INI;
      tREG_GENERIC* aux_INI_ptr;
      byte tarif1, tarif2;
      
      pointer_ok = REPORTE_chkPointer(INI_ptr);   // Verifico que el puntero de inicio sea correcto
      

      if(pointer_ok){
        pointer_ok = REPORTE_chkPointer(FIN_ptr); // Verifico que el puntero de fin sea correcto
      }
      fichas = 0;                                 // Reseteo variable
      if(pointer_ok){
        // Solo proceso si los punteros son correctos
        TO_F = 0;                                 // Reseteo Bandera de TimeOut
        dispararTO_lazo();                        // Disparo Time Out de Lazo
        while ((INI_ptr != FIN_ptr) ){

          TO_F = chkTO_lazo_F();                  // Chequeo bandera de time out de lazo 
          if(checkRANGE(INI_ptr, FIN_TABLA_REPORTE, TO_F )){break;}
          EEPROM_ReadBuffer(&aux_INI,INI_ptr,sizeof(tREG_GENERIC));
          if (aux_INI.tipo == type){
         	  aux_INI_ptr = &aux_INI;
			  tarif1 = ((tREG_A_PAGAR*)aux_INI_ptr)->tarifa;
			  tarif2 = REPORTE_getNroTarifa_fromRegistry(INI_ptr,type);
			  if (tarif2 == tarifa){
        	  //if (REPORTE_getNroTarifa_fromRegistry(INI_ptr,type) == tarifa){
              // Como se trata de un registro <tipo> en esa tarifa => Extraigo y acumulo fichas
				fichas_aux = ((tREG_A_PAGAR*)aux_INI_ptr)->fichasTime[0];
				fichas_aux <<= 8;
				fichas_aux |= ((tREG_A_PAGAR*)aux_INI_ptr)->fichasTime[1];
				fichas_aux <<= 8;
				fichas_aux |= ((tREG_A_PAGAR*)aux_INI_ptr)->fichasTime[2];
				fichas += fichas_aux;
				//fichas += REPORTE_getFichasT_fromRegistry(INI_ptr, type);

            }
          }
          incFlashRep_ptr(&INI_ptr);              // Avanzo puntero
        }
        detenerTO_lazo();                         // Detengo Time Out de Lazo
      }
      
      return(fichas);
    }    


  /* OBTENER CANTIDAD DE FICHAS DE DISTANCIA EN TARIFA SEGUN RANGO y TIPO */
  /************************************************************************/
    uint32_t REPORTES_getFichasDist_byRango (tREG_GENERIC*far INI_ptr, tREG_GENERIC*far FIN_ptr, byte type, byte tarifa){
      // Esta funcion devuelve las fichas de tiempo, desde el registro apuntando por INI_ptr
      // hasta el apuntado por FIN_ptr.
      // Por cada registro <tipo> que se encuentre, se van acumulando las fichas por tiempo
      //
      // Como a esta rutina se ingresa con punteros, debo verificar que los mismos apunten
      // dentro de la tabla de reportes
      // Solo soporta tipo:
      //  - A PAGAR
      //  - DESCONEXION ALIMENTACION
      uint32_t fichas;
      uint32_t fichas_aux;
      byte pointer_ok;
      byte TO_F;
      tREG_GENERIC aux_INI;
      tREG_GENERIC* aux_INI_ptr;

      pointer_ok = REPORTE_chkPointer(INI_ptr);   // Verifico que el puntero de inicio sea correcto
      
      if(pointer_ok){
        pointer_ok = REPORTE_chkPointer(FIN_ptr); // Verifico que el puntero de fin sea correcto
      }
      fichas = 0;                                 // Reseteo variable
      if(pointer_ok){
        // Solo proceso si los punteros son correctos
        TO_F = 0;                                 // Reseteo Bandera de TimeOut
        dispararTO_lazo();                        // Disparo Time Out de Lazo
        while ((INI_ptr != FIN_ptr)){

          TO_F = chkTO_lazo_F();                  // Chequeo bandera de time out de lazo 
          if(checkRANGE(INI_ptr, FIN_TABLA_REPORTE, TO_F )){break;}
          EEPROM_ReadBuffer(&aux_INI,INI_ptr,sizeof(tREG_GENERIC));
          if (aux_INI.tipo == type){
            if (REPORTE_getNroTarifa_fromRegistry(INI_ptr,type) == tarifa){
              // Como se trata de un registro <tipo> en esa tarifa => Extraigo y acumulo fichas
            	aux_INI_ptr = &aux_INI;
            	fichas_aux = ((tREG_A_PAGAR*)aux_INI_ptr)->fichasDist[0];
                fichas_aux <<= 8;
                fichas_aux |= ((tREG_A_PAGAR*)aux_INI_ptr)->fichasDist[1];
                fichas_aux <<= 8;
                fichas_aux |= ((tREG_A_PAGAR*)aux_INI_ptr)->fichasDist[2];
                fichas += fichas_aux;
            	//fichas += REPORTE_getFichasD_fromRegistry(INI_ptr, type);
            }
          }
          incFlashRep_ptr(&INI_ptr);              // Avanzo puntero
        }
        detenerTO_lazo();                         // Detengo Time Out de Lazo
      }
      
      return(fichas);
    }


  /* OBTENER IMPORTE EN TARIFA SEGUN RANGO y TIPO */
  /************************************************/
    uint32_t REPORTES_getImporte_byRango (tREG_GENERIC*far INI_ptr, tREG_GENERIC*far FIN_ptr, byte type, byte tarifa){
      // Esta funcion devuelve el importe, desde el registro apuntando por INI_ptr
      // hasta el apuntado por FIN_ptr.
      // Por cada registro <tipo> que se encuentre, se van acumulando las fichas por tiempo
      //
      // Como a esta rutina se ingresa con punteros, debo verificar que los mismos apunten
      // dentro de la tabla de reportes
      // Solo soporta tipo:
      //  - A PAGAR
      //  - DESCONEXION ALIMENTACION
      uint32_t importe;
      byte pointer_ok;
      byte TO_F;
      tREG_GENERIC aux_INI;
      tREG_GENERIC* aux_INI_ptr;
      
      
      pointer_ok = REPORTE_chkPointer(INI_ptr);   // Verifico que el puntero de inicio sea correcto

      if(pointer_ok){
        pointer_ok = REPORTE_chkPointer(FIN_ptr); // Verifico que el puntero de fin sea correcto
      }
      importe = 0;                                // Reseteo variable
      if(pointer_ok){
        // Solo proceso si los punteros son correctos
        TO_F = 0;                                 // Reseteo Bandera de TimeOut
        dispararTO_lazo();                        // Disparo Time Out de Lazo
        while ((INI_ptr != FIN_ptr)){

          TO_F = chkTO_lazo_F();                  // Chequeo bandera de time out de lazo 
          if(checkRANGE(INI_ptr, FIN_TABLA_REPORTE, TO_F )){break;}

          EEPROM_ReadBuffer(&aux_INI,INI_ptr,sizeof(tREG_GENERIC));
          if (aux_INI.tipo == type){
            if (REPORTE_getNroTarifa_fromRegistry(INI_ptr,type) == tarifa){
              // Como se trata de un registro <tipo> en esa tarifa => Extraigo y acumulo importe
            	aux_INI_ptr = &aux_INI;
                importe += ((tREG_A_PAGAR*)aux_INI_ptr)->importe;
            	//importe += REPORTE_getImporte_fromRegistry(INI_ptr, type);
         }
          }
          incFlashRep_ptr(&INI_ptr);              // Avanzo puntero
        }
        detenerTO_lazo();                         // Detengo Time Out de Lazo
      }
      
      return(importe);
    }    


  /* OBTENER TOTAL DE KM RECORRIDOS EN ESTADO DE RELOJ SEGUN RANGO */
  /*****************************************************************/
    uint32_t REPORTES_getKmRecorridos_byRango (tREG_GENERIC*far INI_ptr, tREG_GENERIC*far FIN_ptr, byte type){
      // Esta funcion devuelve el total de km recorridos en un estado de reloj (tipo de registro), desde el
      // registro apuntando por INI_ptr hasta el apuntado por FIN_ptr.
      //
      // Como a esta rutina se ingresa con punteros, debo verificar que los mismos apunten
      // dentro de la tabla de reportes
      uint32_t km;
      byte pointer_ok;
      byte TO_F;
      tREG_GENERIC aux_INI;
      tREG_GENERIC* aux_INI_ptr;
      
      pointer_ok = REPORTE_chkPointer(INI_ptr);   // Verifico que el puntero de inicio sea correcto
      
      if(pointer_ok){
        pointer_ok = REPORTE_chkPointer(FIN_ptr); // Verifico que el puntero de fin sea correcto
      }
      km = 0;                                     // Reseteo variable
      if(pointer_ok){
        // Solo proceso si los punteros son correctos
        TO_F = 0;                                 // Reseteo Bandera de TimeOut
        dispararTO_lazo();                        // Disparo Time Out de Lazo
        while ((INI_ptr != FIN_ptr)){

          TO_F = chkTO_lazo_F();                  // Chequeo bandera de time out de lazo 
          if(checkRANGE(INI_ptr, FIN_TABLA_REPORTE, TO_F )){break;}

          EEPROM_ReadBuffer(&aux_INI,INI_ptr,sizeof(tREG_GENERIC));
          if (aux_INI.tipo == type){
            // Como se trata de un registro <tipo> => sumo KM
        	  aux_INI_ptr = &aux_INI;
        	km += ((tREG_LIBRE*) aux_INI_ptr)->km;
          }
          incFlashRep_ptr(&INI_ptr);              // Avanzo puntero
        }
        detenerTO_lazo();                         // Detengo Time Out de Lazo
      }
      return(km);
    }
   
   
  /* OBTENER VELOCIDAD MAXIMA EN ESTADO DE RELOJ SEGUN RANGO */
  /***********************************************************/
    byte REPORTES_getVelMax_byRango (tREG_GENERIC*far INI_ptr, tREG_GENERIC*far FIN_ptr, byte type){
      // Esta funcion devuelve la velocidad maxima en un estado de reloj (tipo de registro), desde el
      // registro apuntando por INI_ptr hasta el apuntado por FIN_ptr.
      //
      // Como a esta rutina se ingresa con punteros, debo verificar que los mismos apunten
      // dentro de la tabla de reportes
      byte velMax;
      byte pointer_ok;
      byte TO_F;
      tREG_GENERIC aux_INI;
      tREG_GENERIC* aux_INI_ptr;
      
      pointer_ok = REPORTE_chkPointer(INI_ptr);   // Verifico que el puntero de inicio sea correcto
      
      if(pointer_ok){
        pointer_ok = REPORTE_chkPointer(FIN_ptr); // Verifico que el puntero de fin sea correcto
      }
      pointer_ok = 1;
      velMax = 0;                                 // Reseteo variable
      if(pointer_ok){
        // Solo proceso si los punteros son correctos
        TO_F = 0;                                 // Reseteo Bandera de TimeOut
        dispararTO_lazo();                        // Disparo Time Out de Lazo
        while ((INI_ptr != FIN_ptr) && !TO_F){

          TO_F = chkTO_lazo_F();                  // Chequeo bandera de time out de lazo 
          if(checkRANGE(INI_ptr, FIN_TABLA_REPORTE, TO_F )){break;}

          EEPROM_ReadBuffer(&aux_INI,INI_ptr,sizeof(tREG_GENERIC));
          if (aux_INI.tipo == type){
            // Como se trata de un registro <tipo> => sumo KM
        	  aux_INI_ptr = &aux_INI;
        	if (((tREG_LIBRE*) aux_INI_ptr)->velMax > velMax){
              velMax = ((tREG_LIBRE*) aux_INI_ptr)->velMax;  // Actualizo valor de velocidad maxima
            }
          }
          incFlashRep_ptr(&INI_ptr);              // Avanzo puntero
        }
        detenerTO_lazo();                         // Detengo Time Out de Lazo
      }
      return(velMax);
    }


  /* OBTENER TIEMPO DE MARCHA EN ESTADO DE RELOJ SEGUN RANGO */
  /***********************************************************/
    uint32_t REPORTES_getTiempoMarcha_byRango (tREG_GENERIC*far INI_ptr, tREG_GENERIC*far FIN_ptr, byte type){
      // Esta funcion devuelve el total de tiempo de marcha [s] en un estado de reloj (tipo de registro), desde el
      // registro apuntando por INI_ptr hasta el apuntado por FIN_ptr.
      //
      // Como a esta rutina se ingresa con punteros, debo verificar que los mismos apunten
      // dentro de la tabla de reportes
      uint32_t tiempo;
      byte pointer_ok;
      byte TO_F;
      tREG_GENERIC aux_INI;
      tREG_GENERIC* aux_INI_ptr;
      
      pointer_ok = REPORTE_chkPointer(INI_ptr);   // Verifico que el puntero de inicio sea correcto

      
      if(pointer_ok){
        pointer_ok = REPORTE_chkPointer(FIN_ptr); // Verifico que el puntero de fin sea correcto
      }

      tiempo = 0;                                 // Reseteo variable
      if(pointer_ok){
        // Solo proceso si los punteros son correctos
        TO_F = 0;                                 // Reseteo Bandera de TimeOut
        dispararTO_lazo();                        // Disparo Time Out de Lazo
        while ((INI_ptr != FIN_ptr)){

          TO_F = chkTO_lazo_F();                  // Chequeo bandera de time out de lazo 
          if(checkRANGE(INI_ptr, FIN_TABLA_REPORTE, TO_F )){break;}

          EEPROM_ReadBuffer(&aux_INI,INI_ptr,sizeof(tREG_GENERIC));
          if (INI_ptr->tipo == type){
            // Como se trata de un registro <tipo> => sumo tiempo
        	aux_INI_ptr = &aux_INI;
            tiempo += ((tREG_LIBRE*) aux_INI_ptr)->segMarcha;
          }
          incFlashRep_ptr(&INI_ptr);              // Avanzo puntero
        }
        detenerTO_lazo();                         // Detengo Time Out de Lazo
      }
      
      return(tiempo);
    }


  /* OBTENER TIEMPO PARADO EN ESTADO DE RELOJ SEGUN RANGO */
  /********************************************************/
    uint32_t REPORTES_getTiempoParado_byRango (tREG_GENERIC*far INI_ptr, tREG_GENERIC*far FIN_ptr, byte type){
      // Esta funcion devuelve el total de tiempo parado [s] en un estado de reloj (tipo de registro), desde el
      // registro apuntando por INI_ptr hasta el apuntado por FIN_ptr.
      //
      // Como a esta rutina se ingresa con punteros, debo verificar que los mismos apunten
      // dentro de la tabla de reportes
      uint32_t tiempo;
      byte pointer_ok;
      byte TO_F;
      tREG_GENERIC aux_INI;
      tREG_GENERIC* aux_INI_ptr;
      
      pointer_ok = REPORTE_chkPointer(INI_ptr);   // Verifico que el puntero de inicio sea correcto
      
      if(pointer_ok){
        pointer_ok = REPORTE_chkPointer(FIN_ptr); // Verifico que el puntero de fin sea correcto
      }
      tiempo = 0;                                 // Reseteo variable
      if(pointer_ok){
        // Solo proceso si los punteros son correctos
        TO_F = 0;                                 // Reseteo Bandera de TimeOut
        dispararTO_lazo();                        // Disparo Time Out de Lazo
        while ((INI_ptr != FIN_ptr) && !TO_F){

          TO_F = chkTO_lazo_F();                  // Chequeo bandera de time out de lazo 
          if(checkRANGE(INI_ptr, FIN_TABLA_REPORTE, TO_F )){break;}

          EEPROM_ReadBuffer(&aux_INI,INI_ptr,sizeof(tREG_GENERIC));
          if (aux_INI.tipo == type){
            //Como se trata de un registro <tipo> => sumo tiempo
        	aux_INI_ptr = &aux_INI;
        	tiempo += ((tREG_LIBRE*) aux_INI_ptr)->segParado;
          }
          incFlashRep_ptr(&INI_ptr);              // Avanzo puntero
        }
        detenerTO_lazo();                         // Detengo Time Out de Lazo
      }
      
      return(tiempo);
    }    
    

  /* OBTENER PUNTEROS A TODAS LAS SESIONES */
  /*****************************************/
    byte REPORTES_getSesions (tREG_SESION*far* buffer_ptr, byte maxTurnos){
      // Rutina que devuelve en el buffer pasado como argumento, los punteros a todos los turnos
      // que se encuentren, siempre y cuando sean menores la cantidad maxima establecida
      //
      // Lo que hago es recorrer los registros de reporte, hacia atras, desde donde estoy, hasta
      // estar de nuevo aca o hasta obtener la cantidad maxima
      //
      // Devuelve la cantidad de punteros de sesion extraidas
      tREG_GENERIC* search_ptr;
      byte exit;
      byte sesiones;
      tREG_GENERIC aux_search;
      tREG_GENERIC* aux_search_ptr;
      byte tamanio,tamanio1,tamanio2,tamanio3;
      tREG_GENERIC* tamanio_ptr;

      search_ptr = REPORTE_PUTptr;               // Extraigo valor actual
      decFlashRep_ptr(&search_ptr);              // Retrocedo puntero, porque el actual apunta donde va a agregar
      
      exit = 0;
      sesiones = 0;
      dispararTO_lazo();
      while(!exit){

    	if ((REPORTES_finBackwardSearch(&search_ptr)) || (sesiones >= maxTurnos)){
          // Si:
          //  - Finalizo la busqueda hacia atras o
          //  - Extraje la cantidad maxima de turnos
          exit = 1;
          if(sesiones == 0){
        	 //no hay viajes
        	  sesiones = 0xff;
          }
        }

        EEPROM_ReadBuffer(&aux_search,search_ptr,sizeof(tREG_GENERIC));

        if (aux_search.tipo == REG_sesion){
          *buffer_ptr++ = (tREG_SESION*)search_ptr;  // Guardo puntero de SESION
          sesiones++;                               // Incremento contador de sesiones
        }
        if (search_ptr <= ADDR_EEPROM_REPORTE){
        	exit = 1;
            if(sesiones == 0){
          	 //no hay viajes
          	  sesiones = 0xff;
            }
        }
        decFlashRep_ptr(&search_ptr);
        
        if (chkTO_lazo_F() == 1){
          exit = 1;
       	 //error de timeout
       	  sesiones = 0xfe;
        }
      }
      detenerTO_lazo();
      
      return(sesiones);
    }

    //(void)REPORTES_getSesions(sesion_ptrs, max_turnosReporte);        				// Obtengo punteros a todos las sesiones


    /*
      //MARCHA
  	  seconds_to_HMS(buffer,tiempoMarchaLIBRE);          			// Convierto a H-M-S
  	  seconds_to_HMS(buffer,tiempoMarchaOCUPADO);          			// Convierto a H-M-S
  	  seconds_to_HMS(buffer,tiempoMarchaFSERV);          			// Convierto a H-M-S
  	  //PARADO
  	  seconds_to_HMS(buffer,tiempoParadoLIBRE);          			// Convierto a H-M-S
  	  seconds_to_HMS(buffer,tiempoParadoOCUPADO);          			// Convierto a H-M-S
  	  seconds_to_HMS(buffer,tiempoParadoFSERV);          			// Convierto a H-M-S
  	  //buffer[5] = 0;										// Como solo me interesa imprimir HH:MM, pongo un fin de cadena

     * */

  /* OBTENER DISTANCIA RECORRIDA DESDE EL ULTIMO VIAJE */
  /*****************************************************/
    uint16_t REPORTES_getDistanciaSinceLastViaje (void){
      // Se llama al pasar el RELOJ INTERNO a OCUPADO => En la distancia recorrida
      // en LIBRE informa la distancia recorrida en TODOS los estados, desde que
      // finalizo el ultimo viaje o desde el inicio de la vida del equipo si no
      // hay un viaje anterior
      tREG_GENERIC* search_ptr;
      tREG_GENERIC aux_search;
      tREG_GENERIC* aux_search_ptr;

      byte exit;
      uint16_t km;
      
      km = 0;
      #ifdef VISOR_REPORTES
      if (REPORTES_HABILITADOS){
        search_ptr = REPORTE_PUTptr;            // Extraigo valor actual
        decFlashRep_ptr(&search_ptr);           // Retrocedo puntero, porque el actual apunta donde va a agregar
        

        exit = 0;
        km = 0;
        dispararTO_lazo();
        while(!exit){
          //levanto registro desde eeprom en aux_search
          EEPROM_ReadBuffer(&aux_search,search_ptr,sizeof(tREG_GENERIC));
          if ((REPORTES_finBackwardSearch(&search_ptr)) || (aux_search.tipo == REG_apagar)){
            // Si:
            //  - Finalizo la busqueda hacia atras o
            //  - Llegue al viaje anterior
            exit = 1;
          }
          
          if ((aux_search.tipo == REG_libre) ||
              (aux_search.tipo == REG_fserv)){
        	   aux_search_ptr = &aux_search;
        	  km += ((tREG_LIBRE*) aux_search_ptr)->km;  	// Incremento KM
          }
          decFlashRep_ptr(&search_ptr);
          
          if (chkTO_lazo_F() == 1){
            exit = 1;
            km = 0;                                   // Si salgo por TO, reseteo KM
          }
        }
        detenerTO_lazo();
      
      }else{
        km = 0;
      }
      #endif
      
      return(km);
    }


  /* OBTENER CANTIDAD DE DESCONEXIONES DE ACCESORIO SEGUN RANGO */
  /**************************************************************/
    byte REPORTES_getCantidadDesconexionesAcc_byRango (tREG_GENERIC*far INI_ptr, tREG_GENERIC*far FIN_ptr){
      // Esta funcion devuelve la cantidad de desconexiones de accesorio, desde el registro
      // apuntando por INI_ptr hasta el apuntado por FIN_ptr.
      //
      // Como a esta rutina se ingresa con punteros, debo verificar que los mismos apunten
      // dentro de la tabla de reportes
      byte descon;
      byte pointer_ok;
      byte TO_F;
      tREG_GENERIC aux_INI;
      tREG_GENERIC* aux_INI_ptr;
      
      pointer_ok = REPORTE_chkPointer(INI_ptr);   // Verifico que el puntero de inicio sea correcto
      
      if(pointer_ok){
        pointer_ok = REPORTE_chkPointer(FIN_ptr); // Verifico que el puntero de fin sea correcto
      }
      descon = 0;                                 // Reseteo variable
      if(pointer_ok){
        // Solo proceso si los punteros son correctos
        TO_F = 0;                                 // Reseteo Bandera de TimeOut
        dispararTO_lazo();                        // Disparo Time Out de Lazo
        while ((INI_ptr != FIN_ptr) ){

          TO_F = chkTO_lazo_F();                  // Chequeo bandera de time out de lazo 
          if(checkRANGE(INI_ptr, FIN_TABLA_REPORTE, TO_F )){break;}

          EEPROM_ReadBuffer(&aux_INI,INI_ptr,sizeof(tREG_GENERIC));
          if (aux_INI.tipo == REG_desconexAcc){
            // Como se trata de un registro de desconexion de acc => cuento una desconexion
            descon++;
          }
          incFlashRep_ptr(&INI_ptr);              // Avanzo puntero
        }
        detenerTO_lazo();                         // Detengo Time Out de Lazo
      }
      
      return(descon);
    } 

#ifdef VISOR_PRINT_TICKET_FULL
  /* OBTENER PUNTEROS A TODOS LOS DIAS EN LOS REPORTES */
  /*****************************************************/
    byte REPORTES_getDias (tREG_GENERIC*far* buffer_ptr, byte maxDias){
      // Rutina que devuelve en el buffer pasado como argumento, punteros a los registros de
      // reportes con diferentes dias que se encuentren, siempre y cuando sean menores a la
      // cantidad maxima establecida
      //
      // Lo que hago es recorrer los registros de reporte, hacia atras, desde donde estoy, hasta
      // estar de nuevo aca o hasta obtener la cantidad maxima
      //
      // Devuelve la cantidad de punteros de registros con distinto dia extraidas
      tREG_GENERIC*far search_ptr;
      byte exit;
      byte dias;
      byte lastDIA, lastMES, lastANIO;
      tREG_GENERIC aux_search;
      tREG_GENERIC* aux_search_ptr;
      
      search_ptr = REPORTE_PUTptr;            // Extraigo valor actual
      decFlashRep_ptr(&search_ptr);           // Retrocedo puntero, porque el actual apunta donde va a agregar
      
      exit = 0;
      dias = 0;
      lastDIA = 0;
      lastMES = 0;
      lastANIO = 0;
      dispararTO_lazo();
      while(!exit){
        if ((REPORTES_finBackwardSearch(&search_ptr)) || (dias >= maxDias)){
          // Si:
          //  - Finalizo la busqueda hacia atras o
          //  - Extraje la cantidad maxima de turnos
          exit = 1;
        }
        EEPROM_ReadBuffer(&aux_search, search_ptr,sizeof(tREG_GENERIC));
        aux_search_ptr = &aux_search;
        if ((aux_search_ptr->date.fecha[0] != lastDIA) || (aux_search_ptr->date.fecha[1] != lastMES) || (aux_search_ptr->date.fecha[2] != lastANIO)){
          lastDIA = aux_search_ptr->date.fecha[0];
          lastMES = aux_search_ptr->date.fecha[1];
          lastANIO = aux_search_ptr->date.fecha[2];
          *buffer_ptr++ = search_ptr;                   // Guardo puntero GENERICO
          dias++;                                       // Incremento contador de dias
        }
        decFlashRep_ptr(&search_ptr);
        
        if (chkTO_lazo_F() == 1){
          exit = 1;
          dias = 0;
        }
      }
      detenerTO_lazo();
      
      return(dias);
    }
#endif


#ifdef VISOR_PRINT_TICKET_FULL
  /* OBTENER PUNTERO A PRIMER REGISTRO SEGUN FECHA y HORA */
  /********************************************************/
    tREG_GENERIC*far REPORTES_getRegister_byDate (tDATE date){
      // Esta funcion nos devuelve un puntero al primer registro que satisfaga
      // la condición de FECHA y HORA. Con satisfacer, nos referimos a:
      //  - DIA IGUAL
      //  - HORA IGUAL o ANTERIOR
      //
      // La búsqueda es SIEMPRE hacia atras.
      // La busqueda no es eterna, si no que se hace hasta el fin de busqueda
      tREG_GENERIC*far search_ptr;
      tREG_GENERIC*far result_ptr;
      tREG_GENERIC*far lastSameDate_ptr;
      tCOMPARE_FECHA compare;
      byte exit;
      byte diaOK;
      byte horaOK;
      tREG_GENERIC aux_search;
      tREG_GENERIC* aux_search_ptr;
      
      search_ptr = REPORTE_PUTptr;            // Comienzo desde el puntero actual
      decFlashRep_ptr(&search_ptr);           // Retrocedo puntero, porque el actual apunta donde va a agregar
      
      exit = 0;
      diaOK = 0;
      result_ptr = NULL;
      lastSameDate_ptr = NULL;
      dispararTO_lazo();
      while(!exit){
    	EEPROM_ReadBuffer(&aux_search, search_ptr,sizeof(tREG_GENERIC));

        compare = compareFECHA(date, aux_search.date);
        
        if (compare == FECHA_IGUAL){
          // Coincidio el DIA => Busco hora IGUAL o MAYOR
          diaOK = 1;
          
          lastSameDate_ptr = search_ptr;                // Guardo puntero de la ultima vez que coincidio el DIA
          
          if ((aux_search.date.hora[0] == date.hora[0]) && (aux_search.date.hora[1] == date.hora[1]) && (aux_search.date.hora[2] == date.hora[2])){
            horaOK = 1;                                 // Ya coincidio la HORA
            result_ptr = search_ptr;                    // Posible resultado
          
          }else if (aux_search.date.hora[0] < date.hora[0]){
            // Estoy bien con el DIA, pero la hora es anterior =>
            //  - Si no tenia un posible resultado, este es el resultado
            //  - Si ya tenia un posible resultado, ese es el resutlado
            if (result_ptr == NULL){
              // No tenia resultado => ahora lo tengo
              result_ptr = search_ptr;                  // Ahora tengo un posible resultado              
            }else{
              // Ya tenia un posible resultado y encontre una hora menor => Salgo
            }
            exit = 1;
          }
        
        }else if (compare == FECHA_MENOR){
          // Ya me pase del dia =>
          //  - Si ya tenia un posible resultado, es ese.
          //  - Si no lo tenia, el resultado es la ultima vez que coincidio el DIA
          if (result_ptr == NULL){
            if (lastSameDate_ptr == NULL){
              // De entrada, la fecha es menor y nunca antes coincidio el dia => el
              // puntero actual es el puntero de resultado
              // Esto es para cuando pido para FIN DEL DIA, y no cambio de dia todavia
              // => El dia siguiente nunca se va a encontrar porque no existe y de entrada
              // va a dar que la fecha es menor
              result_ptr = search_ptr;
            }else{
              // No tenia resultado => ahora lo tengo y es la ultima vez que coindicio el dia
              result_ptr = lastSameDate_ptr;
            }
            
          }else{
            // Ya tenia un posible resultado y encontre una hora menor => Salgo
          }
          exit = 1;
        }

        if (chkTO_lazo_F() == 1){
          result_ptr = NULL;
          exit = 1;
        }
        
        if (REPORTES_finBackwardSearch(&search_ptr)){
          // Si ya tenia un resultado, es ese. Sino, es la ultima vez que coincidio el dia
          if (result_ptr == NULL){
              // No tenia resultado => ahora lo tengo
            result_ptr = lastSameDate_ptr;
          }else{
            // Ya tenia un posible resultado y finalizo la busqueda => Salgo
          }
          exit = 1;
        
        }else{
          // Si no termino la busqueda, decremento el puntero para seguir buscando
          decFlashRep_ptr(&search_ptr);
        }
      }
      detenerTO_lazo();
      

      return(result_ptr);
    }
#endif


  /* OBTENER CANTIDAD DE MOVIMIENTO SIN PULSOS SEGUN RANGO */
  /*********************************************************/
    uint16_t REPORTES_getCantidadMovSinPulsos_byRango (tREG_GENERIC*far INI_ptr, tREG_GENERIC*far FIN_ptr){
      // Esta funcion devuelve la cantidad de mov. sin pulsoso, desde el registro
      // apuntando por INI_ptr hasta el apuntado por FIN_ptr.
      //
      // Como a esta rutina se ingresa con punteros, debo verificar que los mismos apunten
      // dentro de la tabla de reportes
      uint16_t movSinPulsos;
      byte pointer_ok;
      byte TO_F;
      tREG_GENERIC aux_INI;
      tREG_GENERIC* aux_INI_ptr;
      
      pointer_ok = REPORTE_chkPointer(INI_ptr);   // Verifico que el puntero de inicio sea correcto
      
      if(pointer_ok){
        pointer_ok = REPORTE_chkPointer(FIN_ptr); // Verifico que el puntero de fin sea correcto
      }
      movSinPulsos = 0;                           // Reseteo variable
      if(pointer_ok){
        // Solo proceso si los punteros son correctos
        TO_F = 0;                                 // Reseteo Bandera de TimeOut
        dispararTO_lazo();                        // Disparo Time Out de Lazo
        while((INI_ptr != FIN_ptr) ){

          TO_F = chkTO_lazo_F();                  // Chequeo bandera de time out de lazo 
          if(checkRANGE(INI_ptr, FIN_TABLA_REPORTE, TO_F )){break;}

          EEPROM_ReadBuffer(&aux_INI, INI_ptr,sizeof(tREG_GENERIC));
          aux_INI_ptr = &aux_INI;
          if (aux_INI.tipo == REG_varios && (((tREG_VARIOS*)aux_INI_ptr)->movSinPulsos == 1)){
            // Como se trata de un registro varios con mov sin pulsos => incremento cantidad
            movSinPulsos++;
          }
          incFlashRep_ptr(&INI_ptr);              // Avanzo puntero
        }
        detenerTO_lazo();                         // Detengo Time Out de Lazo
      }
      
      return(movSinPulsos);
    }    


  /* OBTENER TIEMPO DE ALIMENTACION/ACCESORIO DESCONECTADO SEGUN RANGO */
  /*********************************************************************/
    uint32_t REPORTES_getTiempoDesconexion_byRango (tREG_GENERIC*far INI_ptr, tREG_GENERIC*far FIN_ptr, byte type){
      // Esta funcion devuelve el total de tiempo que estuve desconectada la alimentacion o el accesorio [s],
      // desde el registro apuntando por INI_ptr hasta el apuntado por FIN_ptr.
      //
      // Como a esta rutina se ingresa con punteros, debo verificar que los mismos apunten
      // dentro de la tabla de reportes
      uint32_t tiempo;
      byte* tiempo_ptr;
      uint32_t tiempoTotal;
      byte pointer_ok;
      byte TO_F;
      tREG_GENERIC aux_INI;
      tREG_GENERIC* aux_INI_ptr;

      
      pointer_ok = REPORTE_chkPointer(INI_ptr);   // Verifico que el puntero de inicio sea correcto
      
      if(pointer_ok){
        pointer_ok = REPORTE_chkPointer(FIN_ptr); // Verifico que el puntero de fin sea correcto
      }
      tiempoTotal = 0;                            // Reseteo variable
      if(pointer_ok){
        // Solo proceso si los punteros son correctos
        TO_F = 0;                                 // Reseteo Bandera de TimeOut
        dispararTO_lazo();                        // Disparo Time Out de Lazo
        while((INI_ptr != FIN_ptr)){

          TO_F = chkTO_lazo_F();                  // Chequeo bandera de time out de lazo 
          if(checkRANGE(INI_ptr, FIN_TABLA_REPORTE, TO_F )){break;}
          
          EEPROM_ReadBuffer(&aux_INI, INI_ptr,sizeof(tREG_GENERIC));

          if (aux_INI.tipo == type){
            // Como se trata de un registro <tipo> => sumo tiempo
        	aux_INI_ptr = &aux_INI;
        	tiempo = 0;
            tiempo_ptr = &tiempo;
            //tiempo_ptr++;                         // Son 3 bytes, no 4
            if (type == REG_desconexAlim){
              *tiempo_ptr++ = ((tREG_DESCONEXION_ALIM*) aux_INI_ptr)->seconds[2];
              *tiempo_ptr++ = ((tREG_DESCONEXION_ALIM*) aux_INI_ptr)->seconds[1];
              *tiempo_ptr++ = ((tREG_DESCONEXION_ALIM*) aux_INI_ptr)->seconds[0];
            }else if (type == REG_desconexAcc){
              *tiempo_ptr++ = ((tREG_DESCONEXION_ACC*) aux_INI_ptr)->seconds[2];
              *tiempo_ptr++ = ((tREG_DESCONEXION_ACC*) aux_INI_ptr)->seconds[1];
              *tiempo_ptr++ = ((tREG_DESCONEXION_ACC*) aux_INI_ptr)->seconds[0];
            }
            tiempoTotal += tiempo;                // Voy acumulando tiempo
          }
          incFlashRep_ptr(&INI_ptr);              // Avanzo puntero
        }
        detenerTO_lazo();                         // Detengo Time Out de Lazo
      }
      
      return(tiempoTotal);
    }


  /* DEMORAR GRABACION DE REPORTES */
  /*********************************/
    void REPORTES_delayEEPROM_update (void){
      // 04/04/2013
      //  Se pudo notar que la grabacion en FLASH "jodia" la comunicacion, tanto
      // es así que por ejemplo, al pasar a OCUPADO, no llega la Rta sino que entra
      // el 2do reintento (esto pasa la 2da vez q se pasa a OCUPADO tanto SIMPLEX
      // como DUPLEX).
      // Por eso, lo que se hace aca es disparar un timer que bloquea la grabacion
      // de flash, hasta que el mismo llega a cero.
      // => Siempre que se encola un nuevo comando, se resetea este valor, y cuando
      // se quiere demorarlo, se llama a esta funcion que lo demora
      REPORTES_delay_cnt = seg_delayReportes;
    }
    

/*********************************************************************************************/
/* RUTINAS INTERNAS */
/********************/
  
  /* CHEQUEAR INTEGRIDAD DEL PUNTERO DE LOS REPORTES */
  /***************************************************/
    static byte REPORTE_chkIntigrity (void){
      // Devuelve:
      //  0 -> Puntero fuera de Rango
      //  1 -> Punero OK
      return(!((REPORTE_PUTptr < ADDR_EEPROM_REPORTE) || (REPORTE_PUTptr > ADDR_EEPROM_REPORTE + dim_REPORTE)));
    }
    

  /* ENCOLAR NUEVO REGISTRO A SER GRABADO EN FLASH */
  /*************************************************/
    static byte queueNewReg (tREG_GENERIC* newReg_ptr){
      byte done;
      
      if (chkQueueAvailable()){
        newReg_qeueu_cnt++;               // Incremento cantidad de registros encolados
        *newReg_PUTptr = *newReg_ptr;     // Agrego nuevo registro en la cola
        incRegQueue_ptr(&newReg_PUTptr);  // Avanzo puntero de insercion en cola
        done = 1;                         // Pude encolar
        
        // EDIT 04/04/2013
        //  Siempre que encole un nuevo registro, reseteo este valor para que la 
        // grabación sea instantanea. En cambio si quisiera demorarla, debo
        // llamar a la rutina de delay luego de agregar el registro, con el fin
        // de cambiar este valor
        REPORTES_delay_cnt = 0;
        
      
      }else{
        done = 0;                         // No Pude encolar xq no hay lugar
      }
      
      return(done);
    }


  /* DETERMINAR SI HAY LUGAR EN LA COLA DE REGISTROS DE REPORTE */
  /**************************************************************/
    static byte chkQueueAvailable (void){
      // Determina si hay lugar disponible en la cola de reporte
      //  - 1: hay lugar
      //  - 0: no hay lugar
      byte available;
      
      if (newReg_qeueu_cnt < dim_RegQueue){
        available = 1;
      }else{
        available = 0;
      }
      
      return(available);
    }


  /* AVANZAR PUNTERO DE COLA DE REGISTROS DE REPORTES */
  /****************************************************/    
    static void incRegQueue_ptr (tREG_GENERIC** ptr_ptr){
      tREG_GENERIC* ptr;
      
      ptr = *ptr_ptr;                 // Puntero
      ptr++;                          // Avanzo puntero
      
      if(ptr >= newRegistro + dim_RegQueue){
        ptr = newRegistro;            // Doy la vuelta al buffer de cola
      }
      
      *ptr_ptr = ptr;                 // Actualizo puntero REAL
    }

 
  /* DETERMINAR FIN DE BUSQUEDA HACIA ATRAS */
  /******************************************/
    static byte REPORTES_finBackwardSearch (tREG_GENERIC*far* ptr_ptr){
      // El fin de la busqueda HACIA ATRAS, puede terminar:
      //  - Cuando se alcanza el inicio del buffer (si no se dio la vuelta al mismo)
      //  - Cuando se alcanza el puntero PUT (si ya se dio al menos una vuelta)
      //
      // Para saber si di una vuelta o no, me valgo del INDEX. Si el mismo es mayor o
      // igual al tamaño del buffer => ya dio al menos una vuelta
      byte fin;
      tREG_GENERIC* ptr;
      


      EEPROM_ReadBuffer(&REPORTE_INDEX,ADDR_EEPROM_REPORTE_INDEX,SIZE_EEPROM_REPORTE_INDEX);

      ptr = *ptr_ptr;                 // Puntero
      if (REPORTE_INDEX >= dim_REPORTE){
        // SE DIO AL MENOS UNA VUELTA => finaliza con PUNTERO = PUT
        if (ptr == REPORTE_PUTptr){
          fin = 1;
        }else{
          fin = 0;
        }
      
      }else{
        // TODAVIA NO SE DIO LA VUELTA => finaliza con PUNTERO = INICIO
        if (ptr == ADDR_EEPROM_REPORTE){
          fin = 1;
        }else{
          fin = 0;
        }
      }
      
      return(fin);
    }    


  /* VERIFICAR QUE EL PUNTERO SE ENCUENTRE DENTRO DE LA TABLA DE REPORTES */
  /************************************************************************/
   byte REPORTE_chkPointer (tREG_GENERIC*far pointer){
      byte result;
      
      if ((pointer >= ADDR_EEPROM_REPORTE) && (pointer <= ADDR_EEPROM_REPORTE + dim_REPORTE)){
        result = 1;
      }else{
        result = 0;
      }
      
      return(result);
    }


  /* ACTUALIZAR INDICE EN FLASH */
  /******************************/
    static void updateINDEX_inFlash (void){
      // Rutina que incrementa y actualiza el INDEX de los reportes en FLASH
      // Como aqui se llama al OVERFLOW, esta rutina se debe llamar luego
      // de:
      //  1- Haber grabado un nuevo registro
      //  2- Haber incrementado el puntero PUT
      byte* data_ptr;
      uint16_t indice;
      //uint16_t* addressEEPROM_REPORTE_INDEX;
      
      EEPROM_ReadBuffer(&REPORTE_INDEX,ADDR_EEPROM_REPORTE_INDEX,SIZE_EEPROM_REPORTE_INDEX);

      indice = REPORTE_INDEX;     // Extraigo valor de FLASH
      indice++;                   // Incremento indice en FLASH
      if (indice == 0){
        indice = dim_REPORTE + 1; // Para que quede registrado que dio una vuelta
      }
      data_ptr = (byte*) &indice;
      //REPORTE_flashError = FLASH_updateSector((uint16_t*) ADDR_EEPROM_REPORTE_INDEX, data_ptr, SIZE_EEPROM_REPORTE_INDEX);
      //REPORTE_flashError = FLASH_updateSector((uint16_t*) ADDR_EEPROM_REPORTE_INDEX, &indice, SIZE_EEPROM_REPORTE_INDEX);
      EEPROM_write(ADDR_EEPROM_REPORTE_INDEX, indice);

    }


  /* ACTUALIZAR NRO VIAJE EN FLASH */
  /*********************************/
    static void updateNroVIAJE_inFlash (byte type){
      // Rutina que incrementa y actualiza el NroVIAJE de los reportes en FLASH
		byte* data_ptr;
		uint16_t NroViaje;
		uint16_t* addressEEPROM_REPORTE_NRO_VIAJE;

		//addressEEPROM_REPORTE_NRO_VIAJE = (uint16_t*)ADDR_EEPROM_REPORTE_NRO_VIAJE;

		EEPROM_ReadBuffer(&REPORTE_NRO_VIAJE,ADDR_EEPROM_REPORTE_NRO_VIAJE,SIZE_EEPROM_REPORTE_NRO_VIAJE);

      if (type == REG_apagar){
        //NroViaje = *addressEEPROM_REPORTE_NRO_VIAJE; // Extraigo valor de EEPROM
        NroViaje = REPORTE_NRO_VIAJE;
    	NroViaje++;                         // Incremento NroViaje en FLASH
        if (NroViaje == 0xFFFF){
          NroViaje = 0;
        }
        data_ptr = (byte*) &NroViaje;
        //REPORTE_flashError = FLASH_updateSector((uint16_t*) ADDR_EEPROM_REPORTE_NRO_VIAJE, data_ptr, (uint16_t) 2);
        //REPORTE_flashError = FLASH_updateSector((uint16_t*) ADDR_EEPROM_REPORTE_NRO_VIAJE, &NroViaje, (uint16_t) 2);
        EEPROM_write(ADDR_EEPROM_REPORTE_NRO_VIAJE, NroViaje);
      }
    }    


  /* ACTUALIZAR NRO TURNO EN FLASH */
  /*********************************/
    static void updateNroTURNO_inFlash (byte type){
      // Rutina que incrementa y actualiza el NroTURNO de los reportes en FLASH
      byte* data_ptr;
      uint16_t NroTurno;
      uint16_t* addressEEPROM_REPORTE_NRO_TURNO;

      //addressEEPROM_REPORTE_NRO_TURNO = (uint16_t)ADDR_EEPROM_REPORTE_NRO_TURNO;
      EEPROM_ReadBuffer(&REPORTE_NRO_TURNO,ADDR_EEPROM_REPORTE_NRO_TURNO,SIZE_EEPROM_REPORTE_NRO_TURNO);

      if (type == REG_sesion){
        NroTurno = *addressEEPROM_REPORTE_NRO_TURNO; // Extraigo valor de FLASH
        NroTurno = REPORTE_NRO_TURNO;
        NroTurno++;                         // Incremento NroViaje en FLASH
        if (NroTurno == 0xFFFF){
          NroTurno = 0;
        }
        data_ptr = (byte*) &NroTurno;
        //REPORTE_flashError = FLASH_updateSector((uint16_t*) ADDR_EEPROM_REPORTE_NRO_TURNO, data_ptr, (uint16_t) 2);
        //REPORTE_flashError = FLASH_updateSector((uint16_t*) ADDR_EEPROM_REPORTE_NRO_TURNO, &NroTurno, (uint16_t) 2);
        EEPROM_write(ADDR_EEPROM_REPORTE_NRO_TURNO, NroTurno);
      }
    }


  /* ACTUALIZAR INDICE DE ULTIMA SESION EN FLASH */
  /***********************************************/
    static void updateIndexLastSESION_inFlash (byte type){
      // Rutina que incrementa y actualiza el Indice de la Ultima Sesion en FLASH
      // Como esta rutina guarda el indice en el caso de que lo que se haya grabado
      // sea una sesion, es preciso llamar ANTES de actualizar el INDEX
      byte* data_ptr;
      uint16_t indice;



      EEPROM_ReadBuffer(&REPORTE_INDEX,ADDR_EEPROM_REPORTE_INDEX,SIZE_EEPROM_REPORTE_INDEX);
      if (type == REG_sesion){
        indice = REPORTE_INDEX;     // Extraigo valor de EEPROM
        data_ptr = (byte*) &indice;
        //REPORTE_flashError = FLASH_updateSector((uint16_t*) ADDR_INDEX_LAST_SESION, data_ptr, SIZE_EEPROM_REPORTE_INDEX);
        //REPORTE_flashError = FLASH_updateSector((uint16_t*) ADDR_INDEX_LAST_SESION, &indice, SIZE_EEPROM_REPORTE_INDEX);
        EEPROM_write(ADDR_INDEX_LAST_SESION, indice);
      }
    }    


  /* ACTUALIZAR PUNTERO DE INGRESO DE REGISTROS EN EEPROM */
  /*******************************************************/
    static void updatePUTptr_inFlash (void){
      // Rutina que incrementa y actualiza el PUNTERO de los reportes en FLASH
      byte* data_ptr;
      
      incFlashRep_ptr(&REPORTE_PUTptr);   // Avanzo puntero de grabacion en FLASH
      data_ptr = (byte*) &REPORTE_PUTptr;
      REPORTE_flashError = FLASH_updateSector((uint16_t*) ADDR_EEPROM_REPORTE_PUT, data_ptr, (uint16_t) 4);
    }       


  /* VERIFICAR PERDIDA DE DATOS DE TURNO */
  /***************************************/
    static void chkPerdidaDatosTurno (byte type){
      // Esta rutina se llama cada vez que se termina de grabar un registro en la tabla
      // de reportes.
      // La idea de esta rutina es evitar que se pierdan datos en caso de que el tipo
      // trabaje con un UNICO chofer y no cierre nunca el turno.
      // Para ello, lo que hacemos es guardar el indice de la sesion, cada vez que
      // se crea un registro tipo SESION.
      // Luego, la diferencia entre este indice y el indice actual no debe llegar a un
      // valor igual al tamaño de los reportes, xq sino se pisaria el registro de sesion
      // y no podria imprimir un reporte de turno.
      // Por eso, es que antes de que esto suceda, se le avisa al chofer de esta condicion,
      // invitandolo a cerrar el turno y comenzar uno nuevo.
      // De todos modos, y en caso de que el chofer nunca lo hago, cuando se alcance una
      // situacion límite, se va a cerrar el turno automáticamente y tambien se va a imprimir
      // automaticamente el ticket de reporte de turno
      //
      // Como cuando inicia sesion, previamente graba un LIBRE, en caso de que haya grabado
      // un registro de este tipo NO HAGO ESTA COMPROBACION
      uint16_t dif;
      //uint16_t* addressEEPROM_REPORTE_INDEX;
      //uint16_t* addressEEPROM_INDEX_LAST_SESION;
      uint16_t flash_reporte_index;
      uint16_t flash_index_last_sesion;
      
      //addressEEPROM_REPORTE_INDEX = (uint16_t*)ADDR_EEPROM_REPORTE_INDEX;
      EEPROM_ReadBuffer(&REPORTE_INDEX,ADDR_EEPROM_REPORTE_INDEX,SIZE_EEPROM_REPORTE_INDEX);

      //addressEEPROM_INDEX_LAST_SESION = (uint16_t*)ADDR_INDEX_LAST_SESION;
      EEPROM_ReadBuffer(&INDEX_LAST_SESION,ADDR_INDEX_LAST_SESION,SIZE_INDEX_LAST_SESION);

      flash_reporte_index = REPORTE_INDEX;
      flash_index_last_sesion = INDEX_LAST_SESION;

      if (type != REG_libre){
        if(flash_reporte_index >= flash_index_last_sesion){
          dif = flash_reporte_index - flash_index_last_sesion;  // Diferencia de indices
        }else{
          dif = flash_index_last_sesion - flash_reporte_index;  // Diferencia de indices
        }
        
        if ((dif >= (dim_REPORTE - 10)) && (dif < (dim_REPORTE - 3))){
          // Quedan menos de 10 y mas de 3 indices para alcanzar a la ultima sesion => Muestro aviso
          //mostrar_MRL();
          
        }else if (dif >= (dim_REPORTE - 3)){
          // CONDICION LIMITE => Fuerzo nueva sesion e imprimo reporte de turno
          RELOJ_INTERNO_reOpenTurno();
        }
      }
    }

 tREG_GENERIC*far get_iniFlashTable_ptr(void){
       return(ADDR_EEPROM_REPORTE);
 }




/*********************************************************************************************/
/* TIMER */
/*********/
  
  /* TIMER DE DELAY DE GRABACION DE REPORTES */
  /*******************************************/
    void REPORTES_delayEEPROM_update_timer (void){
      // Ejecutada cada 1 seg no preciso.
      // Simplemente decrementa un contador
      if (REPORTES_delay_cnt != 0){
        REPORTES_delay_cnt--;
      }
    }

#endif

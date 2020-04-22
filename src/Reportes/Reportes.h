/* File: <Reportes.h> */


#ifndef   _REPORTES_
  #define _REPORTES_
  
  /* INCLUDES */
  /************/
#include "Definicion de tipos.h"
#include "stdint.h"
#include "- metroBLUE Config -.h"
#include "Flash.h"

//#include "eeprom.h"


#define __far
#define far

#ifdef VISOR_REPORTES


  /* ESTRUCTURAS */
  /***************/

  // TIPO DE REPORTE GENERICO
  // todos los registros, de todos lo estados deben tener el mismo tamaño
    typedef struct {
      uint16_t    	idx;            // Indice
      uint8_t    	tipo;           // Tipo de registro
      tDATE   		date;           // Date [fecha y hora]
      uint8_t    	chofer;         // Chofer
      uint8_t    	empty[29];      // Relleno para igualar tamaños y longitud PAR
    }tREG_GENERIC;
    
  
  // TIPO DE REPORTE LIRBE
  // todos los registros, de todos lo estados deben tener el mismo tamaño
    typedef struct {
    	uint16_t   idx;            // Indice
    	uint8_t    tipo;           // Tipo de registro
    	tDATE      date;           // Date [fecha y hora]
    	uint8_t    chofer;         // Chofer
    	uint8_t    estadoConexion; // comando transmitido
    	uint8_t    nroViaje;        	// Numero Correlativo de Viaje
    	uint8_t    velMax;         // Velocidad Maxima
    	uint8_t    sensor;         // Se ocupo por Sensor de Asiento
    	uint16_t   km;             // Km recorridos con 1 decimal
    	uint16_t   segParado;      // Segundos que permanecio Detenido
    	uint16_t   segMarcha;      // Segundos que estuvo en Marcha
    	uint8_t    sgnLatLon;	  //signo de latitud y longitud de gps
    	uint8_t    latitud[3];  		// latitud  de GPS DEL CELULAR
		uint8_t    longitud[3];  		// longitud de GPS DEL CELULAR
		uint8_t	   velgps;			// velocidad de GPS DEL CELULAR
    	uint8_t    empty[10];       // Relleno para longitud PAR
    }tREG_LIBRE;


  // TIPO DE REPORTE OCUPADO
  // todos los registros, de todos lo estados deben tener el mismo tamaño
    typedef struct {
    	uint16_t    idx;            // Indice
    	uint8_t     tipo;           // Tipo de registro
    	tDATE       date;           // Date [fecha y hora]
    	uint8_t     chofer;         // Chofer
    	uint8_t     estadoConexion;	// comando transmitido
    	uint8_t     nroViaje;      	// Numero Correlativo de Viaje
    	uint8_t     velMax;         // Velocidad Maxima
    	uint8_t     minutosEspera;  // minutos espera
    	uint8_t     fichaPesos;     // la tarifa esta en: Pesos: 1 , fichas: 2
    	uint8_t     punto_decimal;  //
    	uint16_t    km;             // Km recorridos con 1 decimal
    	uint16_t    bajadaBandera;  // valor bajada de bandera
      	uint16_t    segParado;      // Segundos que permanecio Detenido
      	uint16_t    segMarcha;      // Segundos que estuvo en Marcha
      	uint8_t     sgnLatLon;	    //signo de latitud y longitud de gps
      	uint8_t     latitud[3];  	// latitud  de GPS DEL CELULAR
		uint8_t     longitud[3];  	// longitud de GPS DEL CELULAR
		uint8_t	    velgps;		// velocidad de GPS DEL CELULAR
      	uint8_t     empty[6];       // Relleno para longitud PAR
    }tREG_OCUPADO;    


  // TIPO DE REPORTE A PAGAR
	typedef struct {
		uint16_t   idx;            		// Indice
		uint8_t    tipo;           		// Tipo de registro
		tDATE      date;           		// Date [fecha y hora]
		uint8_t    chofer;         		// Chofer
		uint8_t    estadoConexion;		// comando transmitido
		uint8_t    nroViaje;        	// Numero Correlativo de Viaje
		uint8_t    tarifa;         		// Tarifa
		uint32_t   importe;        		// Importe (PESOS)
		uint8_t    fichasDist[3];  		// Cantidad de Fichas de Distancia
		uint8_t    fichasTime[3];  		// Cantidad de Fichas de Tiempo
		uint16_t   segundosEspera; 		// Espera (SEGUNDOS)
		uint16_t   segundosTarifacion; 	// Espera (SEGUNDOS)
		uint8_t    sgnLatLon;	  		//signo de latitud y longitud de gps
		uint8_t    latitud[3];  		// latitud cobrando de GPS DEL CELULAR
		uint8_t    longitud[3];  		// longitud cobrando de GPS DEL CELULAR
		uint8_t	   velgps;			// velocidad de GPS DEL CELULAR
		uint8_t    empty[2];            // Relleno para longitud PAR
	}tREG_A_PAGAR;


  // TIPO DE REPORTE FUERA DE SERVICIO
  // todos los registros, de todos lo estados deben tener el mismo tamaño
    typedef struct {
      uint16_t  idx;            // Indice
      uint8_t   tipo;           // Tipo de registro
      tDATE   	date;           // Date [fecha y hora]
      uint8_t   chofer;         // Chofer
      uint8_t   estadoConexion;	// comando transmitido
      uint16_t  km;             // Km recorridos con 1 decimal
      uint8_t   velMax;         // Velocidad Maxima
      uint16_t  segParado;      // Segundos que permanecio Detenido
      uint16_t  segMarcha;      // Segundos que estuvo en Marcha
      uint8_t   sgnLatLon;	   //signo de latitud y longitud de gps
      uint8_t   latitud[3];		// latitud  de GPS DEL CELULAR
	  uint8_t   longitud[3];	// longitud de GPS DEL CELULAR
	  uint8_t   velgps;		// velocidad de GPS DEL CELULAR
      uint8_t   empty[12];      // Relleno para longitud PAR
    }tREG_FSERV;


  // TIPO DE REPORTE DESCONEXION ALIMENTACION
  // todos los registros, de todos lo estados deben tener el mismo tamaño
    typedef struct {
      uint16_t   idx;            // Indice
      uint8_t    tipo;           // Tipo de registro
      tDATE      date;           // Date [fecha y hora]
      uint8_t    chofer;         // Chofer
      uint32_t   importe;        // Importe (PESOS)
      uint8_t    seconds[3];     // Segundos desconectado
      uint8_t    tarifa;         // Tarifa
      uint8_t    fichasDist[3];  // Cantidad de Fichas de Distancia
      uint8_t    fichasTime[3];  // Cantidad de Fichas de Tiempo
      uint8_t    empty[13];       // Relleno para longitud PAR
    }tREG_DESCONEXION_ALIM;    


  // TIPO DE REPORTE DESCONEXION ACCESORIO
  // todos los registros, de todos lo estados deben tener el mismo tamaño
    typedef struct {
      uint16_t    idx;            // Indice
      uint8_t    tipo;           // Tipo de registro
      tDATE      date;           // Date [fecha y hora]
      uint8_t    chofer;         // Chofer
      uint8_t    seconds[3];     // Segundos desconectado
      uint8_t    tarifa;         // Tarifa
      uint8_t    fichasDist[3];  // Cantidad de Fichas de Distancia
      uint8_t    fichasTime[3];  // Cantidad de Fichas de Tiempo
      uint32_t   importe;        // Importe (PESOS)
      uint8_t    empty[12];       // Relleno para longitud PAR
    }tREG_DESCONEXION_ACC;        

    // TIPO DE REPORTE SESION
      // todos los registros, de todos lo estados deben tener el mismo tamaño
        typedef struct {
          uint16_t      idx;            // Indice
          uint8_t       tipo;           // Tipo de registro
          tDATE   		date;           // Date [fecha y hora]
          uint8_t       chofer;         // Chofer
          uint16_t      nroTurno;       // Numero de Turno
          uint8_t    	punto_decimal;  // Relleno para longitud PAR
       	  uint8_t     	fichaPesos;     // la tarifa esta en: Pesos: 1 , fichas: 2
          uint16_t    	bajadaBandera;   // valor bajada de bandera
          uint8_t       empty[22];       // Relleno para igualar tamaños y longitud PAR
        }tREG_SESION;


  // TIPO DE REPORTE VARIOS
  // todos los registros, de todos lo estados deben tener el mismo tamaño
    typedef struct {
      uint16_t   idx;            // Indice
      uint8_t    tipo;           // Tipo de registro
      tDATE      date;           // Date [fecha y hora]
      uint8_t    chofer;         // Chofer
      uint8_t    movSinPulsos;   // Movimiento sin pulsos
      uint8_t    programacion;   // Programacion del equipo
      uint8_t    empty[26];      // Relleno para igualar tamaños y longitud PAR
    }tREG_VARIOS;


	typedef struct {
		uint8_t    nroViaje;			// Numero Correlativo de Viaje
		uint8_t    chofer;				// Chofer

	    tDATE      dateLIBRE;           // Date [fecha y hora]
    	uint16_t   kmLIBRE;             // Km recorridos con 1 decimal
    	uint8_t    velMaxLIBRE;         // Velocidad Maxima
    	uint16_t   segParadoLIBRE;      // Segundos que permanecio Detenido
    	uint16_t   segMarchaLIBRE;      // Segundos que estuvo en Marcha
    	uint8_t    sgnLatLonLibre;	  	//signo de latitud y longitud de gps
	    uint8_t    latitudLIBRE[3];		// latitud  de GPS DEL CELULAR
		uint8_t    longitudLIBRE[3];	// longitud de GPS DEL CELULAR
		uint8_t    velgpsLIBRE;             // Velocidad
		uint8_t    estadoConexion_LIBRE;       //

    	tDATE       dateOCUPADO;           // Date [fecha y hora]
    	uint16_t    kmOCUPADO;             // Km recorridos con 1 decimal
    	uint8_t     velMaxOCUPADO;         // Velocidad Maxima
      	uint16_t    segParadoOCUPADO;       // Segundos que permanecio Detenido
      	uint16_t    segMarchaOCUPADO;       // Segundos que estuvo en Marcha
      	uint8_t     sgnLatLonOcupado;	  //signo de latitud y longitud de gps
      	uint8_t     latitudOCUPADO[3];		// latitud  de GPS DEL CELULAR
		uint8_t     longitudOCUPADO[3];	    // longitud de GPS DEL CELULAR
		uint8_t     velgpsOCUPADO;           // Velocidad
		uint8_t     estadoConexion_OCUPADO;       //

		tDATE      dateA_PAGAR;         	// Date [fecha y hora]
    	uint8_t    sensor;         			// Se ocupo por Sensor de Asiento
		uint8_t    tarifa;         			// Tarifa
		uint8_t    fichaPesos;     			// el importe esta en fichas o pesos
		uint8_t    puntoDecimal;   			// punto decimal
		uint8_t    fichasDist[3];  			// Cantidad de Fichas de Distancia
		uint8_t    fichasTime[3];  			// Cantidad de Fichas de Tiempo
		uint32_t   importe;        			// Importe (PESOS)
	    uint16_t   segundosEspera; 			// Espera (SEGUNDOS)
	    uint16_t   segundosTarifacion; 		// Espera (SEGUNDOS)
	    uint8_t    minutosEspera;       	// minutos espera
	    uint8_t    estadoConexion_COBRANDO; //
	    uint8_t    sgnLatLonCobrando;	  //signo de latitud y longitud de gps
	    uint8_t    latitudCOBRANDO[3];		// latitud  de GPS DEL CELULAR
   		uint8_t    longitudCOBRANDO[3];		// longitud de GPS DEL CELULAR
   		uint8_t    velgpsCOBRANDO;          // Velocidad
    }tVIAJE;

  
  /* TIPO DE DATOS */
  /*****************/
    #define SESION_typeINI    0
    #define SESION_typeFIN    1
    
    
    #define CHOFER_1          1
    #define CHOFER_2          2
    #define CHOFER_3          3
    #define CHOFER_4          4
    
    
    // TIPOS DE REGISTRO
    #define REG_generico      0           // Registro Generico
    #define REG_libre         1           // Registro Libre
    #define REG_ocupado       2           // Registro Ocupado
    #define REG_apagar        3           // Registro A Pagar
    #define REG_fserv         4           // Registro Fuera de Servicio
    #define REG_sesion        5           // Registro Sesion
    #define REG_desconexAlim  6           // Registro Desconexion Alimentacion
    #define REG_desconexAcc   7           // Registro Desconexion Accesorio
    #define REG_varios        8           // Registro Varios

      // Valores de PROGRAMACION para REG VARIOS
      #define progTaller      1
      #define progAire        2

#define	NUMERO_DE_REGISTROS_DE_TABLA	1000
#define NUMERO_DE_CAMPOS_DE_REGISTRO	sizeof(tREG_GENERIC)
#define DIM_REPORTE       				(NUMERO_DE_REGISTROS_DE_TABLA*NUMERO_DE_CAMPOS_DE_REGISTRO)   //cantidad de bytes de tabla de reportes
#define FIN_TABLA_REPORTE    			(ADDR_EEPROM_REPORTE + SIZE_EEPROM_REPORTE - 1)

#define dim_RegQueue          5

#define	ADDR_EEPROM_REPORTE 				ADDR_BASE_TABLA_REPORTE
#define	SIZE_EEPROM_REPORTE    	    		DIM_REPORTE

#define	EEPROM_REPORTE_30DIAS			    ADDR_BASE_TABLA_REPORTE_30DIAS
#define	SIZE_EEPROM_REPORTE_30DIAS    		dim_REPORTE_30DIAS

#define	ADDR_EEPROM_REPORTE_PUT 			ADDR_BASE_VAR_REPORTES
#define	SIZE_EEPROM_REPORTE_PUT				sizeof(uint32_t)

#define	ADDR_EEPROM_REPORTE_INDEX 			ADDR_BASE_VAR_REPORTES + sizeof(uint32_t)
#define	SIZE_EEPROM_REPORTE_INDEX			sizeof(uint16_t)

#define	ADDR_EEPROM_REPORTE_NRO_VIAJE		ADDR_BASE_VAR_REPORTES + sizeof(uint32_t) + sizeof(uint16_t)
#define	SIZE_EEPROM_REPORTE_NRO_VIAJE		sizeof(uint16_t)

#define	ADDR_EEPROM_REPORTE_NRO_TURNO		ADDR_BASE_VAR_REPORTES + sizeof(uint32_t)+ 2*sizeof(uint16_t)
#define	SIZE_EEPROM_REPORTE_NRO_TURNO		sizeof(uint16_t)

#define	ADDR_INDEX_LAST_SESION 				ADDR_BASE_VAR_REPORTES + sizeof(uint32_t) + 3*sizeof(uint16_t)
#define	SIZE_INDEX_LAST_SESION				sizeof(uint16_t)

#define	EEPROM_REPORTE_30DIAS_PUT			ADDR_BASE_VAR_REPORTES + sizeof(uint32_t) + 4*sizeof(uint16_t)
#define	SIZE_REPORTE_30DIAS_PUT	    		sizeof(uint32_t)

#define	EEPROM_REPORTE_30DIAS_INDEX			ADDR_BASE_VAR_REPORTES + 2*sizeof(uint32_t) + 4*sizeof(uint16_t)
#define	SIZE_REPORTE_30DIAS_INDEX  	    	sizeof(uint16_t)

    extern tREG_GENERIC* REPORTE_PUTptr;         // Puntero de Insersion de Registros (tipo GENERICO)
    extern tREG_LIBRE regLibre;
    extern tVIAJE regVIAJE;
    extern tFLASH_ERROR REPORTE_flashError;
    extern uint32_t REPORTE_PUT;
    extern uint16_t REPORTE_INDEX;
    extern uint16_t REPORTE_NRO_VIAJE;
    extern uint16_t REPORTE_NRO_TURNO;
    extern uint16_t INDEX_LAST_SESION;
    extern uint8_t REPORTES_HABILITADOS;
  
    extern void iniREPORTES (void);
    extern byte REPORTE_queueLibre (tDATE date, byte chofer, uint16_t km, byte vel, uint16_t segP, uint16_t segM, byte asiento, uint8_t estado_de_conexion, uint8_t nroviaje, byte* ptrLATITUD, byte* ptrLONGITUD,  byte sgnLatLon, byte velocidad);
    extern byte REPORTE_queueOcupado (tDATE date, byte chofer, uint16_t km, byte vel, uint16_t segP, uint16_t segM, uint8_t minutosEspera, uint8_t estado_de_conexion, uint8_t nroCorrelativo_INTERNO, byte* ptrLATITUD, byte* ptrLONGITUD,  byte sgnLatLon, byte velocidad);
    extern byte REPORTE_queueAPagar (tDATE date, byte chofer, byte nroViaje, byte tarifa, uint32_t fichasD, uint32_t fichasT, uint32_t valorViaje, uint16_t esperaEspera, uint16_t esperaTarifacion, uint8_t estado_de_conexion, byte* ptrLATITUD, byte* ptrLONGITUD,  byte sgnLatLon, byte velocidad);
    extern byte REPORTE_queueFueraServ (tDATE date, byte chofer, uint16_t km, byte vel, uint16_t segP, uint16_t segM, uint8_t estado_de_conexion);
    extern byte REPORTE_queueSesion (byte chofer);
    extern byte REPORTE_queueDesconexionAlim (tDATE date, byte chofer, byte tarifa, uint32_t fichasD, uint32_t fichasT, uint32_t importe);
    extern byte REPORTE_queueDesconexionAcc (tDATE dateDescon, byte chofer, byte tarifa, uint32_t fichasD, uint32_t fichasT, uint32_t importe);
    extern byte REPORTE_queueVarios (tDATE date, byte chofer, byte sinPulsos, byte prog);
    
    extern void REPORTES_grabarFlash (void);
    
    extern void incFlashRep_ptr (tREG_GENERIC** ptrptr);
    extern void decFlashRep_ptr (tREG_GENERIC** ptrptr);
    
    extern void incFlashReportes_ptr (tREG_GENERIC*** ptr_ptr_ptr);

    extern byte REPORTES_determineFin (tREG_GENERIC*far* ptr_ptr);
    
    extern byte REPORTES_chkForcedReset (void);
    
    // EXTRACCION DE DATOS
    extern tREG_GENERIC*far REPORTES_getPUT_ptr (void);
    
    extern tREG_GENERIC*far REPORTES_getPrevRegister_byType (byte tipo);
    
    extern uint32_t REPORTE_getFichasT_fromRegistry (tREG_GENERIC*far REG_ptr, byte type);
      #define REPORTE_getFichasT_fromAPagar(ptr)          REPORTE_getFichasT_fromRegistry ((tREG_GENERIC*far)ptr, REG_apagar)
      #define REPORTE_getFichasT_fromDesconexion(ptr)     REPORTE_getFichasT_fromRegistry ((tREG_GENERIC*far)ptr, REG_desconexAlim)
    
    extern uint32_t REPORTE_getFichasD_fromRegistry (tREG_GENERIC*far REG_ptr, byte type);
      #define REPORTE_getFichasD_fromAPagar(ptr)          REPORTE_getFichasD_fromRegistry ((tREG_GENERIC*far)ptr, REG_apagar)
      #define REPORTE_getFichasD_fromDesconexion(ptr)     REPORTE_getFichasD_fromRegistry ((tREG_GENERIC*far)ptr, REG_desconexAlim)

    extern byte REPORTE_getNroTarifa_fromRegistry (tREG_GENERIC*far REG_ptr, byte type);
      #define REPORTE_getNroTarifa_fromAPagar(ptr)        REPORTE_getNroTarifa_fromRegistry ((tREG_GENERIC*far)ptr, REG_apagar)
      #define REPORTE_getNroTarifa_fromDesconexion(ptr)   REPORTE_getNroTarifa_fromRegistry ((tREG_GENERIC*far)ptr, REG_desconexAlim)

    extern uint32_t REPORTE_getImporte_fromRegistry (tREG_GENERIC*far REG_ptr, byte type);
      #define REPORTE_getImporte_fromAPagar(ptr)          REPORTE_getImporte_fromRegistry ((tREG_GENERIC*far)ptr, REG_apagar)
      #define REPORTE_getImporte_fromDesconexion(ptr)     REPORTE_getImporte_fromRegistry ((tREG_GENERIC*far)ptr, REG_desconexAlim)
    
    //extern uint32_t REPORTE_getEspera_fromAPagar (tREG_A_PAGAR*far REG_ptr);
    
    extern byte REPORTES_getCantidadViajes_byRango (tREG_GENERIC*far INI_ptr, tREG_GENERIC*far FIN_ptr, byte type, byte tarifa);
    extern byte REPORTES_getCantidadViajesAsiento_byRango_Value (tREG_GENERIC*far INI_ptr, tREG_GENERIC*far FIN_ptr, byte value);
    extern uint32_t REPORTES_getFichasTime_byRango (tREG_GENERIC*far INI_ptr, tREG_GENERIC*far FIN_ptr, byte type, byte tarifa);
    extern uint32_t REPORTES_getFichasDist_byRango (tREG_GENERIC*far INI_ptr, tREG_GENERIC*far FIN_ptr, byte type, byte tarifa);
    extern uint32_t REPORTES_getImporte_byRango (tREG_GENERIC*far INI_ptr, tREG_GENERIC*far FIN_ptr, byte type, byte tarifa);
    
    extern uint32_t REPORTES_getKmRecorridos_byRango (tREG_GENERIC*far INI_ptr, tREG_GENERIC*far FIN_ptr, byte type);
    extern byte REPORTES_getVelMax_byRango (tREG_GENERIC*far INI_ptr, tREG_GENERIC*far FIN_ptr, byte type);
    extern uint32_t REPORTES_getTiempoMarcha_byRango (tREG_GENERIC*far INI_ptr, tREG_GENERIC*far FIN_ptr, byte type);
    extern uint32_t REPORTES_getTiempoParado_byRango (tREG_GENERIC*far INI_ptr, tREG_GENERIC*far FIN_ptr, byte type);
    
    extern byte REPORTES_getSesions (tREG_SESION*far* buffer_ptr, byte cantTurnos);
    extern byte REPORTES_getTurno (tREG_SESION*far* buffer_ptr, uint16_t nroTurno, byte maxTurnos);
    
    extern uint16_t REPORTES_getDistanciaSinceLastViaje (void);
    
    extern byte REPORTES_getCantidadDesconexionesAcc_byRango (tREG_GENERIC*far INI_ptr, tREG_GENERIC*far FIN_ptr);
  
  #ifdef VISOR_PRINT_TICKET_FULL 
    extern byte REPORTES_getDias (tREG_GENERIC*far* buffer_ptr, byte maxDias);
  #endif
    
  #ifdef VISOR_PRINT_TICKET_FULL
    extern tREG_GENERIC*far REPORTES_getRegister_byDate (tDATE date);
  #endif
    
    extern uint16_t REPORTES_getCantidadMovSinPulsos_byRango (tREG_GENERIC*far INI_ptr, tREG_GENERIC*far FIN_ptr);
    
    extern uint32_t REPORTES_getTiempoDesconexion_byRango (tREG_GENERIC*far INI_ptr, tREG_GENERIC*far FIN_ptr, byte type);
      #define REPORTES_getTiempoDesconexionAlim_byRango(INI_ptr, FIN_ptr) REPORTES_getTiempoDesconexion_byRango((tREG_GENERIC*far) INI_ptr, (tREG_GENERIC*far) FIN_ptr, REG_desconexAlim)
      #define REPORTES_getTiempoDesconexionAcc_byRango(INI_ptr, FIN_ptr)  REPORTES_getTiempoDesconexion_byRango((tREG_GENERIC*far) INI_ptr, (tREG_GENERIC*far) FIN_ptr, REG_desconexAcc)
      
    extern void REPORTES_delayEEPROM_update (void);
    extern tREG_GENERIC*far get_iniFlashTable_ptr(void);
    extern byte* LOGUEO_getChoferLogueado (void);
    byte REPORTE_chkPointer (tREG_GENERIC*far pointer);

    extern tREG_GENERIC* get_regLIBRE_by_ptrREG_APAGAR ( tREG_GENERIC* INI_ptr, byte nro_viaje);
    extern tREG_GENERIC* get_regOCUPADO_by_ptrREG_APAGAR ( tREG_GENERIC* INI_ptr, byte nro_viaje);
    extern tREG_GENERIC* get_regAPAGAR_byNUMERO_VIAJE ( byte nro_viaje);
    extern void chkPerdidaDatosTurno (void);

  /* TIMER */
  /*********/
    extern void REPORTES_delayEEPROM_update_timer (void);

  #endif
#endif

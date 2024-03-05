#ifndef _RELOJ_
  #define _RELOJ_

#include "Definicion de tipos.h"
#include "DA Communication.h"
#include "stdint.h"
#include "Flash.h"
#include "driver_eeprom.h"_
/*******************************************************************
*	 	ESTRUCTURAS
********************************************************************/


#define SIN_CONEXION_CENTRAL	0
#define CON_CONEXION_CENTRAL	1
#define CI_ENCENDIDO_EQUIPO		3 //conexion indeterminada encendido de equipo



#define LIB			0
#define OCUP		1
#define COBR		2


	// ESTADOS DE RELOJ
	typedef enum{
		LIBRE=0x18,
		OCUPADO=0x19,
		COBRANDO=0x1A,
		FUERA_SERVICIO=0x1B,
		DESCONECTADO=0x27
	}tRELOJ;

	// ESTADOS DE ASIGNACION
	 typedef enum{
	    NO_ASIGNADO = 0,
	    ASIGNADO_LIBRE = 1,
		ASIGNADO_OCUPADO=2,
	  }tASIGNACION;


	// ERRORES DE RELOJ
	typedef enum{
		RLJ_OK,                           // No hay error de Reloj
		RLJ_reachPUT,                     // El puntero GET alcanzo al puntero PUT
		RLJ_TO,                           // TimeOut en lazo de Reloj
		RLJ_noFIN                         // No se encontro el fin de datos en 25bytes
	}tRELOJ_ERROR;

	typedef union{
	byte Byte;

		struct{
		  byte chofer_0   :1;
		  byte chofer_1   :1;
		  byte chofer_2   :1;
		  byte chofer_3   :1;
		  byte :1;
		  byte :1;
		  byte :1;
		  byte update     :1;     // Pedido de actualizacion
		}Bits;

		struct{
		  byte grpChofer  :4;     // Nuevo valor de chofer
		}MergedBits;
	}tUPDATE_CHOFER;

	extern byte chofer;

		typedef union {
	      byte Byte;
	      struct{
			  byte por_boton				:1;
			  byte por_sensor_asientoOCUP  	:1;
			  byte por_sensor_asientoLIB  	:1;
			  byte por_banderita			:1;
			  byte por_app					:1;
			  byte :1;
			  byte :1;
			  byte :1;
	      }Bits;
	 }tPASE_OCUPADO;


	extern tPASE_OCUPADO _status_paseOCUPADO;

	 	#define status_paseOCUPADO	_status_paseOCUPADO.byte
	 	#define paseOCUPADO_BOTON			_status_paseOCUPADO.Bits.por_boton
		#define paseOCUPADO_SENSOR_ASIENTO	_status_paseOCUPADO.Bits.por_sensor_asientoOCUP
		#define paseLIBRE_SENSOR_ASIENTO	_status_paseOCUPADO.Bits.por_sensor_asientoLIB
		#define paseOCUPADO_BANDERITA		_status_paseOCUPADO.Bits.por_banderita
		#define paseOCUPADO_APP				_status_paseOCUPADO.Bits.por_app

	//#define BANDERA_OUT_PIN                GPIO_PIN_5
	#define BANDERA_OUT_PIN                GPIO_PIN_5
	#define BANDERA_OUT_PORT               GPIOB
	#define BANDERA_OUT_CLK_ENABLE()       __GPIOB_CLK_ENABLE()
	#define BANDERA_OUT_CLK_DISABLE()      __GPIOB_CLK_DISABLE()


  // Estado Asignacion
  extern tASIGNACION       	  ESTADO_ASIGNACION;
	#define asignar_libre     (ESTADO_ASIGNACION = ASIGNADO_LIBRE)
	#define asignar_ocupado   (ESTADO_ASIGNACION = ASIGNADO_OCUPADO)
	#define quitar_asignado   (ESTADO_ASIGNACION = NO_ASIGNADO)

/*******************************************************************
*	 	MACROS
********************************************************************/


	#define dim_Tabla_CMD_Reloj			11

    #define HAY_CMDS_RELOJ_ENCOLADOS    (RELOJ_CMDencolados_cnt > 0)
    #define CMD_RELOJ_TRANSMITIDO       (RELOJ_CMDencolados_cnt--)
    #define RELOJ_LIBRE                     (ESTADO_RELOJ == LIBRE)
    #define RELOJ_OCUPADO                   (ESTADO_RELOJ == OCUPADO)
	#define RELOJ_COBRANDO                  (ESTADO_RELOJ == COBRANDO)
	#define RELOJ_FUERA_SERVICIO            (ESTADO_RELOJ == FUERA_SERVICIO)
	#define RELOJ_DESCONECTADO              (ESTADO_RELOJ == DESCONECTADO)
	#define	RELOJ_NO_ASIGNADO				(ESTADO_ASIGNACION == NO_ASIGNADO)
	#define	RELOJ_ASIGNADO_LIBRE			(ESTADO_ASIGNACION == ASIGNADO_LIBRE)
	#define	RELOJ_ASIGNADO_OCUPADO			(ESTADO_ASIGNACION == ASIGNADO_OCUPADO)

    extern void RELOJ_INTERNO_setAsiento(byte causa);
      #define sensorAsiento_pressed     1 // Ocupado por sensor de asiento presionado (funcionamiento normal)
      #define sensorAsiento_error       2 // Ocupado por error en lectura de sensor de asiento inteligente

#define TARIFA_MANUAL       0
#define TARIFA_AUTO         1

    // MACROS
        #define paseLIBRE_interno_conKM         paseLIBRE(&kmRecorridos_INTERNO)
        #define paseLIBRE_interno_sinKM         paseLIBRE(NULL)
        #define paseOCUPADO_interno             paseOCUPADO(&kmRecorridos_INTERNO, velMax_INTERNO, RELOJ_INTERNO_getChofer(), nroCorrelativo_INTERNO)
        //#define paseAPAGAR_interno              paseAPAGAR(&kmRecorridos_INTERNO, velMax_INTERNO, minutosEspera_INTERNO, TARIFA.numero,((byte*)(&importe_INTERNO))+1, RELOJ_INTERNO_getChofer(), nroCorrelativo_INTERNO)
		#define paseAPAGAR_interno              paseAPAGAR(&kmRecorridos_INTERNO, velMax_INTERNO, minutosEspera, TARIFA.numero,((byte*)(&importe_INTERNO))+1, RELOJ_INTERNO_getChofer(), nroCorrelativo_INTERNO)

    #define paseFUERA_SERVICIO_interno       paseFUERA_SERVICIO(&kmRecorridos_INTERNO)

/*******************************************************************
*	 	CONSTANTES
********************************************************************/

#define distanciaA_PAGAR_to_LIBRE     30 //60  // 60mts a recorrer en A PAGAR para pasar a LIBRE

#define CAMBIO_RELOJ_PERMITIDO 				0x00	//permite cambio reloj
#define CAMBIO_RELOJ_NO_PERMITIDO_NC	 	0x01	//no hay cambio porque el reloj esta en el estado pedido
#define CAMBIO_RELOJ_NO_PERMITIDO_ESPERA 	0x02	//Esta en espera necesario para el cambio
#define CAMBIO_RELOJ_NO_PERMITIDO_MOV	 	0x03	//Esta en MOVIMIENTO
#define CAMBIO_RELOJ_NO_PERMITIDO_OTROS	 	0x04	//Esta en MOVIMIENTO

#define VEHICULO_EN_MOVIMIENTO			MENSAJE1
#define ESPERA_DE_PASE_A_COBRANDO		MENSAJE2
#define YA_PUEDE_PASAR_A_LIBRE			MENSAJE3
#define TARIFA_INVALIDA					MENSAJE4
#define TARIFA_NO_PROGRAMADA			MENSAJE5
#define COMANDO_TRANSPARENTE			MENSAJE6
#define RECAUDACION_PARCIAL				MENSAJE7
#define DEBE_REALIZAR_UN_VIAJE			MENSAJE8
#define SESION_CERRADA_EXITOSAMENTE		MENSAJE9
#define PASE_A_COBRANDO_NO_PERMITIDO	MENSAJE10
#define TARIFA_AUTOMATICA				MENSAJE11
#define CIERRE_TURNO					MENSAJE12
#define TURNO_CERRADO					MENSAJE13
#define SINCRONIZANDO_CON_CENTRAL		MENSAJE14
#define EQUIPO_O_RELOJ_DESCONOCIDO		MENSAJE15


#define CUENTA_MAX_TARIFA	4
/*******************************************************************
*	 	VARIABLES
********************************************************************/

  // COMANDOS DE TRANSMISION
    extern typeTxCMD   CMD_Pase_a_LIBRE;
    extern typeTxCMD   CMD_Pase_a_LIBRE_SC;
    extern typeTxCMD   CMD_Pase_a_OCUPADO;
    extern typeTxCMD   CMD_Pase_a_OCUPADO_SA;
    extern typeTxCMD   CMD_Pase_a_OCUPADO_SA_SC;
    extern typeTxCMD   CMD_Pase_a_OCUPADO_BANDERA;
    extern typeTxCMD   CMD_Pase_a_OCUPADO_APP;
    extern typeTxCMD   CMD_Pase_a_OCUPADO_APP_SC;
    extern typeTxCMD   CMD_Pase_a_COBRANDO;
    extern typeTxCMD   CMD_Pase_a_COBRANDO_SC;
    extern typeTxCMD   CMD_Pase_a_FUERA_SERVICIO;

    extern uint8_t nroTARIFA_HAB_MANUAL;		//CANTIDAD DE TARIFAS HABILITADAS
    extern uint8_t nroTARIFA_HAB_AUTOMATICA;		//CANTIDAD DE TARIFAS HABILITADAS

    extern typeTxCMD* const Tabla_CMD_Reloj[dim_Tabla_CMD_Reloj];

    extern tRELOJ     ESTADO_RELOJ;
    extern byte     ESTADO_RELOJ_CONEXION;

    extern byte ESTADO_RELOJ_X_PULSADOR;
    extern void Reloj_iniTarifa(byte tarifa);
    extern void Reloj_iniTarifa1(void);
    extern void Reloj_iniTarifa2(void);
    extern void Reloj_iniTarifa3(void);
    extern void Reloj_iniTarifa4(void);

    extern void dispararIndicacionTarifaTiempo (void);
    extern uint16_t kmRecorridos, kmRecorridos_LIBRE, kmRecorridos_OCUPADO;
    extern byte velMax, velMax_LIBRE, velMax_OCUPADO;
    extern uint16_t timerMarcha_cnt, timerMarcha_cnt_LIBRE, timerMarcha_cnt_OCUPADO;	//Tiempo de marcha
    extern uint16_t timerParado_cnt, timerParado_cnt_LIBRE, timerParado_cnt_OCUPADO;	//Tiempo de PARDO
    extern byte nroCorrelativo_INTERNO;          											//Nro Correlativo de Viaje
    extern byte minutosEspera;           											//Minutos de Espera
    extern uint8_t minutosEspera_anterior;
    extern byte nroChofer;

    extern byte turnoChofer;
		#define turno_Noiniciado		0
	    #define turno_iniciado			1

    extern uint32_t importe_INTERNO;
    extern uint16_t kmRecorridos;
    extern uint16_t kmRecorridos_LIBRE;
    extern uint16_t kmRecorridos_OCUPADO;			//KM Recorridos
    extern byte velMax;
    extern byte velMax_LIBRE;
    extern byte velMax_OCUPADO;								//Velocidad Máxima

    extern byte puntoDECIMAL;
    extern byte fichaPESOS;				//la tarifa esta en: Pesos: 1 , fichas: 2
    extern tDATE libreDATE;
    extern tDATE ocupadoDATE;
    extern tDATE cobrandoDATE;
    extern tDATE fueraServicioDATE;

    extern  tDATE HoraApagado;
    extern uint8_t tarifa_aux;
    extern uint8_t tarifa_to_cnt;
		#define TARIFA_TO		3;



    extern byte sgnLatLon_LIBRE_CEL;						//signo de latitud y longitud
    extern byte latitudGPS_LIBRE_CEL[3];
    extern byte longitudGPS_LIBRE_CEL[3];
    extern byte velocidadGPS_LIBRE_CEL;

    extern byte sgnLatLon_OCUPADO_CEL;						//signo de latitud y longitud
    extern byte latitudGPS_OCUPADO_CEL[3];
    extern byte longitudGPS_OCUPADO_CEL[3];
    extern byte velocidadGPS_OCUPADO_CEL;

    extern byte sgnLatLon_COBRANDO;						//signo de latitud y longitud
    extern byte latitudGPS_COBRANDO_CEL[3];
    extern byte longitudGPS_COBRANDO_CEL[3];
    extern byte velocidadGPS_COBRANDO_CEL;


/*******************************************************************
*	 	PROTOTIPOS
********************************************************************/
    void  ini_pulsador_cambio_reloj(void);
    byte get_sensorAsiento(void);
    extern void RELOJ_reset (void);
    extern void RELOJ_Tx (void);
    extern void Pase_a_LIBRE (byte estado);
    extern void paseLIBRE (byte* KM_ptr);
    extern void PaseLibreFromFServ_INTERNO (void);
    extern void Pase_a_OCUPADO (byte estado);
    extern void Pase_a_COBRANDO (byte estado);
    //extern void Pase_a_FUERA_SERVICIO (void);
    extern void Pase_a_FUERA_SERVICIO (void);
    extern void paseFUERA_SERVICIO (byte* KM_ptr);
    extern void Send_Tx_Valor_VIAJE (void);
    extern void Toggle_BANDERA (void);

    extern void RLJ_INI (void);
    extern void Libre_RxRTA (byte* Rx_data_ptr);
    extern void Ocupado_RxRTA (byte* Rx_data_ptr);
    extern void Cobrando_RxRTA (byte* Rx_data_ptr);

    extern void BanderaOut_Ini(void);
    extern void BanderaOut_On(void);
    extern void BanderaOut_Off(void);
    extern void BanderaOut_Toggle(void);
    extern void preparar_print (uint32_t importe, byte decimales, byte* buffer_reloj, byte printPesos);
    extern void preparar_print_poneCOMA (uint32_t importe, byte decimales, byte* buffer_reloj, byte printPesos);
    extern void preparar_print_new (uint32_t importe, byte decimales, byte* buffer_reloj, byte printPesos, byte** buffer_print);
    extern void RELOJ_tomarDateCambioReloj_Arranque (void);
    extern tDATE RELOJ_getDateCambio(void);
    extern byte RELOJ_INTERNO_getChofer(void);
    extern void inc_nroCorrelativo (void);
    void preparar_print_nroTICKET (uint32_t nroTICKET,  byte* bufferTICKET);
    extern byte Read_nroCorrelativo_from_EEPROM (void);
    extern void RELOJ_INTERNO_reOpenTurno (void);
    extern void RELOJ_INTERNO_newSesion (byte nroChofer);
    extern void relojINTERNO_updateCHOFER (void);
    extern void CHOFER_setNewChofer (byte newChofer);
    extern void CHOFER_firstBoot (void);
    extern void RELOJ_INTERNO_resetMarchaParado (void);
    extern void RELOJ_INTERNO_timerMarchaParado (void);
    extern void cambio_de_reloj_x_pulsador(void);
    extern void cambio_de_reloj_x_sensor_asiento(void);
    extern void RELOJ_a_pagar_to_libre (void);
    extern void RELOJ_a_pagar_to_libre_timer (void);
    extern void cambioRELOJ_timer (void);
    extern void RELOJ_clearMOTIVO_paseOCUPADO(void);

    extern void cobrandoDATE_enFLASH (byte forced);
    extern void ocupadoDATE_enFLASH (byte forced);
    extern void  SaveDatGps(byte* data_ptr, byte estado);
    extern void  ResetDatGps(byte estado);

    extern uint8_t tarifa_1_4;		//valor de 1 a 4 a mostrar en display
    extern uint8_t  tarifa_1_8;	//valor de 1 a 8 que sirve de indice para tomar uno de los 8 valores de tarifa programados en eeprom

    const tDATE_flash FLASH_ocupadoDATE;
    const tDATE_flash FLASH_cobrandoDATE;
    const uint16_t FLASH_PULSOS_x_KM;
    extern uint8_t cntSendValorViaje;

    extern void Tx_Pase_a_LIBRE  (byte tipo);
    extern void Tx_Pase_a_OCUPADO (uint8_t tipo);
    extern void Tx_Pase_a_COBRANDO (uint8_t tipo);
    extern void Tx_Pase_a_FUERA_SERVICIO (void);

    extern void rearmar_y_TX_cmdOcupado(byte nroViaje, byte status);
    extern void rearmar_y_TX_cmdLibre(byte nroViaje);
    extern byte chkCmd_Reloj(byte Rx_cmd);
    extern void guardar_dateLIBRE(void);
    extern void guardar_dateOCUPADO(void);

#endif

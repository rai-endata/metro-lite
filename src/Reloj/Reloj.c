
	#include "Reloj.h"
	#include "stdint.h"
	#include "Manejo de buffers.h"
	#include "Timer.h"
	#include "DTE - Tx.h"
	#include "usart7.h"
	#include "Odometro.h"
	#include "Tarifacion Reloj.h"
	#include "DA Define Commands.h"
	#include "DA Communication.h"
	#include "DA Tx.h"
	#include "Constantes.h"
	#include "Lista Comandos.h"
	#include "driver_eeprom.h"
	#include "eeprom.h"
	#include "teclas.h"
    #include "buzzer.h"
    #include "display-7seg.h"
	#include "DA Rx.h"
	#include "Ticket Viaje.h"
	#include "Parametros Reloj.h"
	#include "tipo de equipo.h"
	#include "Comandos sin conexion a central.h"
	#include "Tarifa Automatica.h"
	#include "inicio.h"
	//#include "Param Reloj.h"

	#include "DA Rx.h"
	#include "main.h"
	#include "rtc.h"
	#include "Variables en flash.h"
	#include "flash_test.h"
	#include "calculos.h"
	#include "Flash.h"
	#include "Reportes.h"
	#include "Print UART.h"
	#include "Ticket Viaje.h"
	#include "Odometro.h"
	#include "Variables en flash.h"
	#include "eeprom.h"


/* PROTOTIPOS */
/*************/
void RELOJ_INTERNO_addAPagarReportes (uint8_t estado_de_conexion);
void setTARIFA_MANUAL(void);
static byte RELOJ_chkTransmitiendo (void);
static byte RELOJ_chkTransmitiendo (void);
static typeTxCMD* RELOJ_verificarCMDencolado (byte cmd);
static tRELOJ_ERROR RELOJ_TxBuffer_to_CMD (typeTxCMD* CMD_ptr, byte** GET_ptr_ptr);
static void RELOJ_resetBuffer (void);
static void RELOJ_resetReintentos (void);
static void RELOJ_requestBKP_enFLASH (void);
static tRELOJ_ERROR RELOJ_incGETptr_endDATA (void);
static tTARIFA* updateTarifa (tDATE date, byte numTarifaActual);

static byte* getLATITUD(byte estado);
static byte* getLONGITUD(byte estado);
static byte  getSgnLatLon(byte estado);
static byte  getVELOCIDAD(byte estado);

static tUPDATE_CHOFER       _CHOFER_F;
  #define CHOFER_F          _CHOFER_F.Byte
  #define CHOFER_newValue   _CHOFER_F.MergedBits.grpChofer
  #define CHOFER_update     _CHOFER_F.Bits.update

static uint16_t timerA_PAGAR_to_LIBRE_cnt;	// Contador del timer para pasar de A PAGAR -> LIBRE
    #define seg_A_PAGAR_to_LIBRE    11  	// 11seg

static uint16_t cambioRELOJ_TO_cnt;	// Contador del timer para pasar de A PAGAR -> LIBRE
    #define seg_cambioRELOJ_TO_cnt    2  	// 11seg



// BANDERAS DE RELOJ
	    typedef union {
	        byte Byte;
	        struct {
	          byte from_APAGAR      :1;     // Indica que el reloj viene de A PAGAR
	          byte 					:1;
	          byte conectado        :1;     // Indica RELOJ CONECTADO
	          byte consultar        :1;     // Indica que se debe enviar la consulta al reloj con PROTOCOLO
	          byte bkp_Flash        :1;     // Solicita grabacion de BKP de Buffer de Tx en FLASH
	          byte bufferFull       :1;     // Indica que se lleno el buffer de RELOJ
	        } Bits;
	    }RELOJ_FLAGS;

// BANDERAS DE RELOJ
    extern RELOJ_FLAGS                  _RELOJ_F;
      #define RELOJ_F                   _RELOJ_F.Byte
      #define RELOJ_fromAPAGAR          _RELOJ_F.Bits.from_APAGAR
      #define RELOJ_conectado           _RELOJ_F.Bits.conectado
      #define RELOJ_doBkpFLASH          _RELOJ_F.Bits.bkp_Flash
      #define RELOJ_bufferFull          _RELOJ_F.Bits.bufferFull


/* VARIABLES */
/*************/
// Variables de Informe de Comandos
	uint8_t cntSendValorViaje = 3;

    uint16_t kmRecorridos_INTERNO;            // KM Recorridos segun el reloj interno
	uint8_t velMax_INTERNO;                  // Velocidad Maxima segun el reloj interno
	uint8_t nroCorrelativo_INTERNO;          // Nro Correlativo de Viaje segun el reloj interno
	//uint8_t minutosEspera_INTERNO;           // Minutos de Espera del Reloj Interno segun el reloj interno
	uint32_t importe_INTERNO;                // Importe del Viaje segun el reloj interno
	tPASE_OCUPADO _status_paseOCUPADO;

	byte ESTADO_RELOJ_X_PULSADOR;
	byte ESTADO_RELOJ_CONEXION;

	byte sgnLatLon_LIBRE_CEL;						//signo de latitud y longitud
	byte latitudGPS_LIBRE_CEL[3];
	byte longitudGPS_LIBRE_CEL[3];
	byte velocidadGPS_LIBRE_CEL;

	byte sgnLatLon_OCUPADO_CEL;						//signo de latitud y longitud
	byte latitudGPS_OCUPADO_CEL[3];
	byte longitudGPS_OCUPADO_CEL[3];
	byte velocidadGPS_OCUPADO_CEL;

	byte sgnLatLon_COBRANDO_CEL;						//signo de latitud y longitud
	byte latitudGPS_COBRANDO_CEL[3];
	byte longitudGPS_COBRANDO_CEL[3];
	byte velocidadGPS_COBRANDO_CEL;

	tASIGNACION  	ESTADO_ASIGNACION = NO_ASIGNADO;

static byte 	sensorAsiento;            // Indica si se ocupo por sensor de asiento y el motivo
static tDATE  	RELOJ_dateCambio;
const uint16_t  FLASH_CHOFER={NULL};

uint8_t tarifa_1_4;		//valor de 1 a 4 a mostrar en display
uint8_t  tarifa_1_8;	//valor de 1 a 8 que sirve de indice para tomar uno de los 8 valores de tarifa programados en eeprom

uint8_t tarifa_to_cnt;		//timeout de botn de pase a ocupado con el cual se va cambiando de tarifa.

uint8_t nroTARIFA_HAB_MANUAL;		//CANTIDAD DE TARIFAS HABILITADAS
uint8_t nroTARIFA_HAB_AUTOMATICA;		//CANTIDAD DE TARIFAS HABILITADAS

#define dim_RELOJ         200

  tRELOJ	ESTADO_RELOJ;                    				 // Estado del Reloj
  byte		turnoChofer;
  byte 		puntoDECIMAL;
  byte 		fichaPESOS;

  tDATE libreDATE;
  tDATE ocupadoDATE;
  tDATE cobrandoDATE;
  tDATE fueraServicioDATE;



  static byte RELOJ_TxBuffer[dim_RELOJ];        // Buffer de Tx de Comandos de Reloj
  static byte* RELOJ_TxPUT;                     // Puntero PUT del Buffer de Tx del Reloj
  static byte* RELOJ_TxGET;						// Puntero GET del Buffer de Tx del Reloj

   #define RELOJ_TxBuffer_put(data)      (put_byte(&RELOJ_TxPUT, data, RELOJ_TxBuffer, (uint16_t) dim_RELOJ))

    #define RELOJ_TxBuffer_get(ptr_ptr)   (get_byte(ptr_ptr, RELOJ_TxBuffer, (uint16_t) dim_RELOJ))
    #define RELOJ_incGET                  (inc_ptr(&RELOJ_TxGET, RELOJ_TxBuffer, (uint16_t) dim_RELOJ))

  byte RELOJ_CMDencolados_cnt;                  // Contador de comandos encolados, para saber cuando dejar de Tx
  byte RELOJ_lastCMDencolado;                   // Ultimo comando encolados

//  static byte RELOJ_reint_cnt;                      // Contador de reintentos de reloj


	// Comando LIBRE
	#define N_DATOS_Pase_a_LIBRE           41             //7 con posicion
	#define N_Pase_a_LIBRE									(N_CMD + N_DATOS_Pase_a_LIBRE)
	static byte Pase_a_LIBRE_Buffer[N_DATOS_Pase_a_LIBRE + 2];       // Sumo DF + 0A
	typeTxCMD   CMD_Pase_a_LIBRE={0,CMD_RELOJ_Pase_a_LIBRE,0,timeReint_rapido,N_Pase_a_LIBRE,Pase_a_LIBRE_Buffer};
	typeTxCMD   CMD_Pase_a_LIBRE_SC={0,CMD_RELOJ_Pase_a_LIBRE_SC,0,timeReint_rapido,N_Pase_a_LIBRE,Pase_a_LIBRE_Buffer};

	// Comando OCUPADO
    #define N_DATOS_Pase_a_OCUPADO         44				//12
    #define N_Pase_a_OCUPADO               (N_CMD + N_DATOS_Pase_a_OCUPADO)
    static byte Pase_a_OCUPADO_Buffer[N_DATOS_Pase_a_OCUPADO + 2];   // Sumo DF + 0A
    typeTxCMD   CMD_Pase_a_OCUPADO={0,CMD_RELOJ_Pase_a_OCUPADO,0,timeReint_rapido,N_Pase_a_OCUPADO,Pase_a_OCUPADO_Buffer};
    typeTxCMD   CMD_Pase_a_OCUPADO_APP_SC={0,CMD_RELOJ_Pase_a_OCUPADO_APP_SC,0,timeReint_rapido,N_Pase_a_OCUPADO,Pase_a_OCUPADO_Buffer};


    // Comando OCUPADO
        #define N_DATOS_Pase_a_OCUPADO_SA         44				//12
        #define N_Pase_a_OCUPADO_SA               (N_CMD + N_DATOS_Pase_a_OCUPADO_SA)
       // static byte Pase_a_OCUPADO_SA_Buffer[N_DATOS_Pase_a_OCUPADO_SA + 2];   // Sumo DF + 0A
        typeTxCMD   CMD_Pase_a_OCUPADO_SA={0,CMD_RELOJ_Pase_a_OCUPADO_SA,0,timeReint_rapido,N_Pase_a_OCUPADO,Pase_a_OCUPADO_Buffer};
        typeTxCMD   CMD_Pase_a_OCUPADO_SA_SC={0,CMD_RELOJ_Pase_a_OCUPADO_SA_SC,0,timeReint_rapido,N_Pase_a_OCUPADO,Pase_a_OCUPADO_Buffer};

    	// Comando OCUPADO
        #define N_DATOS_Pase_a_OCUPADO_APP         44				//12
        #define N_Pase_a_OCUPADO_APP               (N_CMD + N_DATOS_Pase_a_OCUPADO_APP)
        //static byte Pase_a_OCUPADO_APP_Buffer[N_DATOS_Pase_a_OCUPADO_APP + 2];   // Sumo DF + 0A
        typeTxCMD   CMD_Pase_a_OCUPADO_APP={0,CMD_RELOJ_Pase_a_OCUPADO_APP,0,timeReint_rapido,N_Pase_a_OCUPADO_APP,Pase_a_OCUPADO_Buffer};

    	// Comando OCUPADOf
        #define N_DATOS_Pase_a_OCUPADO_BANDERA         44				//12
        #define N_Pase_a_OCUPADO_BANDERA               (N_CMD + N_DATOS_Pase_a_OCUPADO)
        //static byte Pase_a_OCUPADO_BANDERA_Buffer[N_DATOS_Pase_a_OCUPADO_BANDERA + 2];   // Sumo DF + 0A
        typeTxCMD   CMD_Pase_a_OCUPADO_BANDERA = {0,CMD_RELOJ_Pase_a_OCUPADO_BANDERA,0,timeReint_rapido,N_Pase_a_OCUPADO_BANDERA,Pase_a_OCUPADO_Buffer};

	// Comando COBRANDO
    #define N_DATOS_Pase_a_COBRANDO         44				//12
    #define N_Pase_a_COBRANDO               (N_CMD + N_DATOS_Pase_a_COBRANDO)
    static byte Pase_a_COBRANDO_Buffer[N_DATOS_Pase_a_COBRANDO + 2];   // Sumo DF + 0A
    typeTxCMD   CMD_Pase_a_COBRANDO={0,CMD_RELOJ_Pase_a_COBRANDO,0,timeReint_rapido,N_Pase_a_COBRANDO,Pase_a_COBRANDO_Buffer};
    typeTxCMD   CMD_Pase_a_COBRANDO_SC={0,CMD_RELOJ_Pase_a_COBRANDO_SC,0,timeReint_rapido,N_Pase_a_COBRANDO,Pase_a_COBRANDO_Buffer};


	// Comando COBRANDO
    #define N_DATOS_Pase_a_FUERA_SERVICIO         8				//12
    #define N_Pase_a_FUERA_SERVICIO               (N_CMD + N_DATOS_Pase_a_FUERA_SERVICIO)
    static byte Pase_a_FUERA_SERVICIO_Buffer[N_DATOS_Pase_a_FUERA_SERVICIO + 2];   // Sumo DF + 0A
    typeTxCMD   CMD_Pase_a_FUERA_SERVICIO={0,CMD_RELOJ_Pase_a_FUERA_SERVICIO,0,timeReint_rapido,N_Pase_a_FUERA_SERVICIO,Pase_a_FUERA_SERVICIO_Buffer};

// Tabla de Punteros a Comandos de Reloj
	typeTxCMD* const Tabla_CMD_Reloj[dim_Tabla_CMD_Reloj]={
		&CMD_Pase_a_LIBRE,
		&CMD_Pase_a_OCUPADO,
		&CMD_Pase_a_OCUPADO_SA,
		&CMD_Pase_a_OCUPADO_BANDERA,
		&CMD_Pase_a_OCUPADO_APP,
		&CMD_Pase_a_COBRANDO,
		&CMD_Pase_a_FUERA_SERVICIO,
		&CMD_Pase_a_LIBRE_SC,
		&CMD_Pase_a_OCUPADO_SA_SC,
		&CMD_Pase_a_OCUPADO_APP_SC,
		&CMD_Pase_a_COBRANDO_SC
	};

	static byte timer_tarifaTiempo_cnt;            	//Timer para le parpadeo de la tarifacion por tiempo
	    #define x100mseg_timer_tarifaTiempo     5   //x100mseg (500mseg) para el parpadeo de la indicación tarifando por tiempo

	 // COMANDOS DE TRANSMISION
	 //   typeTxCMD   CMD_LIBRE;
	 //   typeTxCMD   CMD_OCUPADO;


	    uint16_t kmRecorridos; uint16_t kmRecorridos_LIBRE; uint16_t kmRecorridos_OCUPADO;			//KM Recorridos
	    uint8_t velMax; uint8_t velMax_LIBRE; uint8_t velMax_OCUPADO;								//Velocidad Máxima
	    uint16_t timerMarcha_cnt; uint16_t timerMarcha_cnt_LIBRE; uint16_t timerMarcha_cnt_OCUPADO;	//Tiempo de marcha
	    uint16_t timerParado_cnt; uint16_t timerParado_cnt_LIBRE; uint16_t timerParado_cnt_OCUPADO;	//Tiempo de PARDO
	    uint8_t nroCorrelativo_INTERNO;          											//Nro Correlativo de Viaje
	    uint8_t minutosEspera;           											//Minutos de Espera
	    uint8_t minutosEspera_anterior;
	    uint8_t nroChofer;															//nro de CHOFER

	    tDATE HoraApagado;


	    const tDATE_flash FLASH_ocupadoDATE={NULL};
	    const tDATE_flash FLASH_cobrandoDATE={NULL};
	    const uint16_t FLASH_PULSOS_x_KM={NULL};

/*******************************
 *  PRUEBAS
 * ****************************/




void Pase_a_LIBRE (byte estado){
	tFLASH_ERROR error;
    byte nro_correlativo;
	uint32_t Address;
	sc_data dataSC;

	ESTADO_RELOJ_CONEXION = estado;
	DISTANCIA_iniCalculo_PULSE_ACCUM();

	#ifdef RELOJ_DEBUG
	timerA_PAGAR_to_LIBRE_cnt = 0;
	#endif

	if(tipo_de_equipo == METRO_LITE || tipo_de_equipo == MINI_BLUE){
		timerA_PAGAR_to_LIBRE_cnt = 0;
	}
	if(timerA_PAGAR_to_LIBRE_cnt == 0){
		anularReTx_RELOJ();
		if(estado == CON_CONEXION_CENTRAL || estado == CI_ENCENDIDO_EQUIPO){
			CMD_a_RESP = 0x03;
		}else{
			CMD_a_RESP = 0x23;
		}
		TxRta_conDATOS(CAMBIO_RELOJ_PERMITIDO);

		//if(RELOJ_INTERNO){

		//	if((ESTADO_RELOJ==FUERA_SERVICIO) || (ESTADO_RELOJ==COBRANDO)&& NO_PRINTING && NO_TXING_PRINTER){

			if(ESTADO_RELOJ==FUERA_SERVICIO){
				PaseLibreFromFServ_INTERNO();
			}else{
		          (void)calcularDISTANCIA_entreEstados;   // Resetear contador de Distancia
		          paseLIBRE_interno_sinKM;          	  // Paso a LIBRE desde A PAGAR => no cuento KM
			}



			if(estado == CON_CONEXION_CENTRAL || estado == CI_ENCENDIDO_EQUIPO){
				//envio pase a libre al celular
				Tx_Pase_a_LIBRE(estado);
			}else if(estado == SIN_CONEXION_CENTRAL){
	  			dataSC.nro_viaje = nroCorrelativo_INTERNO;
	  			dataSC.estado_reloj = ESTADO_RELOJ;
	  			push_dataSC(dataSC);
	  			push_dataSC_inEeprom(dataSC);
			}

			RELOJ_INTERNO_resetMarchaParado();  // Reset de tiempo de PARADO y MARCHA
			VELOCIDAD_MAX = 0;
			libreDATE = getDate();
			RELOJ_dateCambio = libreDATE;
			writeEepromDATE(libreDATE);
			//PRUEBA
			tDATE testDATE;
			readEepromDATE(&testDATE);

			ESTADO_RELOJ=LIBRE;
			ESTADO_RELOJ_X_PULSADOR = LIBRE;
			off_display();
			on_display_tarifa();
			//mostrar_ini_dsplyIMPORTE=0;

			//enciendo bandera ? (en asignado doble queda apagada)
			if(RELOJ_NO_ASIGNADO || RELOJ_ASIGNADO_LIBRE){
				BanderaOut_On();
				quitar_asignado;
			}else{
				BanderaOut_Off();
			}

			cambioRELOJ_TO_cnt = seg_cambioRELOJ_TO_cnt;
		//}
	}else{
		TxRta_conDATOS(CAMBIO_RELOJ_NO_PERMITIDO_ESPERA);
		Tx_Comando_MENSAJE(ESPERA_DE_PASE_A_COBRANDO);
	}
	chkPerdidaDatosTurno();           // Chequear si estoy por perder datos de turno

}

void Pase_a_OCUPADO (byte estado){

    sc_data dataSC;

	//if(ESTADO_RELOJ==LIBRE ){

		//Reloj_iniTarifa(tarifa);

	    //RELOJ_setTarifa(TARIFA_AUTO_getNroTarifa());        // Seteo automatico de tarifa
	    //tarifa = TARIFA.numero;
		anularReTx_RELOJ();

		t_pulsos_anterior = 0;
		t_pulsos = 0;
		cntIC_anterior   = 0;
		__HAL_TIM_SET_COUNTER(&pulsoACCUM,0);
		//DISTANCIAm = 0;
		//DISTANCIA_100 = 0;

		RELOJ_setTarifa(tarifa_1_8);
		RELOJ_iniTarifacion();
	    RLJ_INI();
		guardar_tarifa_backup();

		BanderaOut_Off(); 					//apago bandera
		quitar_asignado;

		ocupadoDATE  = getDate();
		RELOJ_dateCambio = ocupadoDATE;
		//inicializo cobrandoDATE por si hay un corte largo en ocupado
		cobrandoDATE.hora[0] = 0xff;   // HORA
		cobrandoDATE.hora[1] = 0xff;   // MINUTOS
		cobrandoDATE.hora[2] = 0xff;   // SEGUNDOS
		cobrandoDATE.fecha[0] = 0xff;  // DIA
		cobrandoDATE.fecha[1] = 0xff;  // MES
		cobrandoDATE.fecha[2] = 0xff;  // AÑO
		viajeInconcluso = 0;
		DISTANCIAm_OCUPADO = 0;
		VELOCIDAD_MAX_VIAJE = 0;
		//EEPROM_WriteBuffer((uint8_t*) &VELOCIDAD_MAX_VIAJE, ADDR_EEPROM_VELOCIDAD_MAX_VIAJE, SIZE_VELOCIDAD_MAX_VIAJE);

		ocupadoDATE_writeFLASH = 1;

		ESTADO_RELOJ = OCUPADO;
		ESTADO_RELOJ_X_PULSADOR = OCUPADO;
		update_valor_tarifa(tarifa_1_4);

		on_display_all();
		velMax_INTERNO = (uint8_t)VELOCIDAD_MAX;
		velMax_LIBRE = (uint8_t)velMax_INTERNO;                         // Velocidad Maxima en LIBRE
		DISTANCIAm_LIBRE = DISTANCIAm;
		kmRecorridos_INTERNO = calcularDISTANCIA_entreEstados;			// Distancia Recorrida en LIBRE (KM xxx.x)
		kmRecorridos_LIBRE = kmRecorridos_INTERNO;
		DISTANCIA_iniCalculo_PULSE_ACCUM();
		DISTANCIAm = 0;
	  	DISTANCIA_100 = 0;

		//nroCorrelativo_INTERNO = Read_nroCorrelativo_from_EEPROM();        // Nro Correlativo de Viaje
#ifdef VISOR_REPORTES
		if (REPORTES_HABILITADOS){
			//uint8_t nroviaje = nroCorrelativo_INTERNO;
			//if(nroviaje==0xFF){nroviaje=1;}else{nroviaje++;}
			//(void)REPORTE_queueLibre (libreDATE, RELOJ_INTERNO_getChofer(),kmRecorridos_LIBRE, velMax_LIBRE, timerParado_cnt, timerMarcha_cnt, sensorAsiento, ESTADO_RELOJ_CONEXION, nroviaje, getLATITUD(LIB), getLONGITUD(LIB), getSgnLatLon(LIB),getVELOCIDAD(LIB));
			(void)REPORTE_queueLibre (libreDATE, RELOJ_INTERNO_getChofer(),kmRecorridos_LIBRE, velMax_LIBRE, timerParado_cnt, timerMarcha_cnt, sensorAsiento, ESTADO_RELOJ_CONEXION, nroCorrelativo_INTERNO, getLATITUD(LIB), getLONGITUD(LIB), getSgnLatLon(LIB),getVELOCIDAD(LIB));

		}
#endif


	#ifdef VISOR_REPORTE_ACUMULATIVO
		//REPORTES_delayFLASH_update();                       // Demoro grabacion de REPORTES en FLASH
		km = REPORTES_getDistanciaSinceLastViaje();         	//Calculo distancia desde el ultimo viaje
		kmRecorridos_INTERNO += km;                         	//Agrego KM anteriores
		if (kmRecorridos_INTERNO != NULL){
			// Al pasar a OCUPADO, solo cuento los km de cuando paso LIBRE -> OCUPADO
			// Si esta con reloj interno => solo puede pasar a OCUPADO desde LIBRE. En cambio
			// si esta con reloj con banderita, si esta LOGUEADO pasa desde LIBRE y si esta
			// deslogueado, pasa desde FUERA DE SERVICIO
			if (RELOJ_INTERNO){
			  //RELOJ_REPACUM_addKmLibre(kmRecorridos_INTERNO);             // Agrego km Libre a Reporte Acumulativo
			}else if (RELOJ_BANDERITA){
			  if (chkVISOR_logueado){
				RELOJ_REPACUM_addKmLibre(kmRecorridos_INTERNO);           // Agrego km Libre a Reporte Acumulativo
			  }else{
				RELOJ_REPACUM_addKmFueraServ(kmRecorridos_INTERNO);       // Agrego km Fuera Servicio a Reporte Acumulativo
			  }
		}
	}
	#endif

		//comienza un viaje incremento nuemro correlativo
		//lo incremento aca para que los datos del libre se guarde
		//con el numero de viaje enviado en el pase a libre
		inc_nroCorrelativo();
		inc_nroTICKET();


	if(estado == CON_CONEXION_CENTRAL ){
		//envio pase a ocupado al celular
		Tx_Pase_a_OCUPADO(CON_CONEXION_CENTRAL);
		ESTADO_RELOJ_CONEXION = CON_CONEXION_CENTRAL;
	}else if (estado == SIN_CONEXION_CENTRAL ){

		ESTADO_RELOJ_CONEXION = SIN_CONEXION_CENTRAL;
		dataSC.nro_viaje = nroCorrelativo_INTERNO;
		dataSC.estado_reloj = ESTADO_RELOJ;
		push_dataSC(dataSC);	//guardo estado nro de viaje, para reconstruir el cmd cuando vuelva la conexion
		push_dataSC_inEeprom(dataSC);
	}

	cambioRELOJ_TO_cnt = seg_cambioRELOJ_TO_cnt;
	resetVELOCIDAD_MAX;
	RELOJ_INTERNO_resetMarchaParado();  //Reset de tiempo de PARADO y MARCHA

}

void Pase_a_COBRANDO (byte estado){

	//if(ESTADO_RELOJ==OCUPADO){

		anularReTx_RELOJ();
		cobrandoDATE = getDate();
		RELOJ_dateCambio = cobrandoDATE;
		cobrandoDATE_writeFLASH = 1;
		ESTADO_RELOJ=COBRANDO;
		ESTADO_RELOJ_X_PULSADOR = COBRANDO;
		//display_set_toggle();
		off_display();
		toggle_display();
	 #ifdef RELOJ_HOMOLOGADO
		if(!RELOJ_BANDERITA){
			timerA_PAGAR_to_LIBRE_cnt = seg_A_PAGAR_to_LIBRE;
		}

        #else
        timerA_PAGAR_to_LIBRE_cnt = 0;  // Al no estar Homologado, no espero nada
      #endif

        //Esta misma rutina esta REPLICADA para cuando el importe se ingresa de manera MANUAL
		velMax_INTERNO = (uint8_t)VELOCIDAD_MAX;
		//VELOCIDAD_MAX_VIAJE = VELOCIDAD_MAX;
		//EEPROM_WriteBuffer((uint8_t*) &VELOCIDAD_MAX_VIAJE, ADDR_EEPROM_VELOCIDAD_MAX_VIAJE, SIZE_VELOCIDAD_MAX_VIAJE);

		velMax_OCUPADO = (uint8_t)velMax_INTERNO;                         // Velocidad Maxima en LIBRE
		kmRecorridos_INTERNO = calcularDISTANCIA_entreEstados;		// Distancia Recorrida en OCUPADO (KM xxx.xx)
		kmRecorridos_OCUPADO = kmRecorridos_INTERNO;
		DISTANCIAm_OCUPADO = DISTANCIAm;

		//lo saco creo que se no usa, y causaba problemas en la actualizacion de hora
		//write_backup_rtcCONTADOR_PULSOS();  //guardo los pulsos porque cuando calcule la distancia entre estados

		DISTANCIA_iniCalculo_PULSE_ACCUM();
		//minutosEspera_INTERNO = (byte) (segundosTarifacion/60);   // Calculo Minutos de Espera
		importe_INTERNO = VALOR_VIAJE;                          	// Valor del Viaje
		//nroCorrelativo_INTERNO = Read_nroCorrelativo_from_EEPROM;        		// Nro Correlativo de Viaje

		// Incremento y Grabo Nro Correlativo en EEPROM

		// lo paso a ocupado porque el viaje comienza en ocupado
		//inc_nroCorrelativo();
		//inc_nroTICKET();

		// Guardo reporte -> datos de OCUPADO
		#ifdef VISOR_REPORTES
		if (REPORTES_HABILITADOS){

		(void)REPORTE_queueOcupado (ocupadoDATE, RELOJ_INTERNO_getChofer(),kmRecorridos_OCUPADO, velMax_OCUPADO, timerParado_cnt, timerMarcha_cnt, minutosEspera, ESTADO_RELOJ_CONEXION, nroCorrelativo_INTERNO, getLATITUD(OCUP), getLONGITUD(OCUP), getSgnLatLon(OCUP), getVELOCIDAD(OCUP));

		 ESTADO_RELOJ_CONEXION = estado;

		 #ifdef _ABONAR_SEGUN_TARIFA_HORA_A_PAGAR_
		   // En MONTEVIDEO, esta reglamentado que la tarifa que debe abonarse no corresponde a la hora
		   // de abordaje, sino la hora de fin del viaje. Por lo tanto, debo verificar si es necesario
		   // actualizar la tarifa.

		   //nro = updateTarifa(getDate(), TARIFA.numero)->numero;
		   //RELOJ_setTarifa(nro);            // Seteo automatico de tarifa

		 //RELOJ_setTarifa(TARIFA.numero);            // Seteo automatico de tarifa
		 #endif

		 // Guardo reporte -> datos de A PAGAR
		 RELOJ_INTERNO_addAPagarReportes(estado);

		 // EDIT 04/04/2013
		 //  Para permitir la correcta recepcion de la respuesta, demoro la grabacion de FLASH, para
		 // no inhibir las IRQs
		 //REPORTES_delayFLASH_update();                       // Demoro grabacion de REPORTES en FLASH
		}
		#endif

	#ifdef VISOR_REPORTE_ACUMULATIVO
         RELOJ_REPACUM_addViaje();                     // Agrego VIAJE a reporte Acumulativo
         if (KM_ptr != NULL){
           // Al pasar a A PAGAR, solo cuento los km de cuando paso OCUPADO -> A PAGAR
           km = *KM_ptr++;                             // KM en OCUPADO (MSB)
           km <<= 8;                                   // KM en OCUPADO (LSB)
           km |= *KM_ptr++;
           RELOJ_REPACUM_addKmOcupado(km);             // Agrego km Ocupado a Reporte Acumulativo
         }

         if (importe_ptr != NULL){
           // Son 3bytes de importe
           importe = *importe_ptr++;
           importe <<= 8;
           importe |= *importe_ptr++;
           importe <<= 8;
           importe |= *importe_ptr++;
           RELOJ_REPACUM_addImporte(importe);          // Agrego Importe a Reporte Acumulativo
         }
     #endif


         if(estado==CON_CONEXION_CENTRAL){
     		//envio pase a cobrando al celular
     		Tx_Pase_a_COBRANDO(CON_CONEXION_CENTRAL);
         }

         cambioRELOJ_TO_cnt = seg_cambioRELOJ_TO_cnt;


		//resetVELOCIDAD_MAX;                             // Reseteo Velocidad Maxima
		RELOJ_INTERNO_resetMarchaParado();  // Reset de tiempo de PARADO y MARCHA
		RELOJ_clearMOTIVO_paseOCUPADO();
		 sensorAsiento = 0;                  // Borro Indicacion
	//}

}



static void fuera_de_servicio(void);

void Pase_a_FUERA_SERVICIO (void){
	if(EQUIPO_METRO_BLUE){
		if(ESTADO_RELOJ==LIBRE){
			fuera_de_servicio();
		}
	}else{
		if(ESTADO_RELOJ!=FUERA_SERVICIO){
			fuera_de_servicio();
		}
	}
	chkPerdidaDatosTurno();           // Chequear si estoy por perder datos de turno
}

static void fuera_de_servicio(void){
	fueraServicioDATE = getDate();
	RELOJ_dateCambio = fueraServicioDATE;
	velMax_INTERNO = (uint8_t)VELOCIDAD_MAX;
	 kmRecorridos_INTERNO = calcularDISTANCIA_entreEstados;	// Distancia Recorrida en LIBRE (KM xxx.x)
//	 nroCorrelativo_INTERNO = EEPROM_NRO_CORRELATIVO;        // Nro Correlativo de Viaje
	 DISTANCIA_iniCalculo_PULSE_ACCUM();
	 // Guardo reporte -> datos de LIBRE
	 #ifdef VISOR_REPORTES
	   if (REPORTES_HABILITADOS){
		//uint8_t nroviaje = nroCorrelativo_INTERNO;
		//if(nroviaje==0xFF){nroviaje=1;}else{nroviaje++;}

		(void)REPORTE_queueLibre (RELOJ_getDateCambio(), RELOJ_INTERNO_getChofer(),kmRecorridos_INTERNO, velMax_INTERNO, timerParado_cnt, timerMarcha_cnt, 0, ESTADO_RELOJ_CONEXION, nroCorrelativo_INTERNO, getLATITUD(LIB), getLONGITUD(LIB), getSgnLatLon(LIB), getVELOCIDAD(LIB));
		 // EDIT 04/04/2013
		 //  Para permitir la correcta recepcion de la respuesta, demoro la grabacion de FLASH, para
		 // no inhibir las IRQs
		 //REPORTES_delayEEPROM_update_timer;                       // Demoro grabacion de REPORTES en FLASH
	   }
	 #endif
	 ESTADO_RELOJ_CONEXION = CON_CONEXION_CENTRAL;
	 paseFUERA_SERVICIO_interno;        // Paso a FUERA DE SERVICIO
	 RELOJ_INTERNO_resetMarchaParado();  // Reset de tiempo de PARADO y MARCHA
}



/* PASE A FUERA DE SERVICIO */
/*--------------------------*/
  void paseFUERA_SERVICIO (byte* KM_ptr){


	anularReTx_RELOJ();
	BanderaOut_Off();
	off_display();
    ESTADO_RELOJ = FUERA_SERVICIO;                  // Actualizo Estado de Reloj
    resetVELOCIDAD_MAX;                             // Reseteo Velocidad Maxima

    if(EQUIPO_METRO_BLUE){
    	Tx_Pase_a_FUERA_SERVICIO();
    }

    cambioRELOJ_TO_cnt = seg_cambioRELOJ_TO_cnt;

    #ifdef VISOR_REPORTE_ACUMULATIVO
      if (KM_ptr != NULL){
        // Al pasar a FUERA SERVICIO, solo cuento los km de cuando paso LIBRE -> FUERA SERVICIO
        km = *KM_ptr++;                             // KM en LIBRE (MSB)
        km <<= 8;                                   // KM en LIBRE (LSB)
        km |= *KM_ptr++;
        RELOJ_REPACUM_addKmLibre(km);               // Agrego km Libre a Reporte Acumulativo
      }
    #endif
  }


	void Send_Tx_Valor_VIAJE (void){
		byte diffEspera;
		byte diffDistancia;


		if(ESTADO_RELOJ==OCUPADO  ){
			diffEspera = minutosEspera- minutosEspera_anterior;
			if(diffEspera > 0){
				minutosEspera_anterior = minutosEspera;
				Tx_Valor_VIAJE ();
			}
            diffDistancia = DISTANCIA_100 - DISTANCIA_100_anterior;
			if(diffDistancia > 99){
				DISTANCIA_100_anterior = DISTANCIA_100;
				Tx_Valor_VIAJE ();
			}
		}
	}

	void Toggle_BANDERA (void){
		if(ESTADO_RELOJ==COBRANDO){
			BanderaOut_Toggle();
		}
	}

void RLJ_INI (void){

   //ESTADO_RELOJ=OCUPADO;
   //ESTADO_RELOJ=LIBRE;
   //VELOCIDAD = 0x0102;

	//prueba
	//set_tarifPulsos();          // Comienzo a tarifar por Pulsos
	//RELOJ_iniPulsos();            // Inicializacion de tarifacion por pulsos

   RELOJ_reset();
   RELOJ_resetReintentos();
}


/* TRANSMITIR BUFFER DE RELOJ */
/******************************/
void RELOJ_Tx (void){
	// Rutina que dispara la transmisión de un comando en el Buffer de Reloj. Para ello, se fija
	// que haya comandos encolados en el buffer. Cada stream comienza con el COMANDO y finaliza
	// con los 2 bytes de fin de datos.
	// Al iniciar una transmisión, extraigo el comando; en primer lugar lo valido, es decir que
	// sea alguno de los comandos de reloj validos. Luego, si pasa la validación, armo el buffer
	// correspondiente al comando y levanto su respectiva bandera de transmisión, así como también
	// cargo la cantidad de reintentos.
	// No avanzo el puntero que extrae los datos del buffer (puntero GET) ya que el mismo se avanza
	// al recibir la respuesta. Del mismo modo NO decremento el contador de comandos encolados hasta
	// no recibir la respuesta al mismo
	// Falto aclarar que esta rutina se llama desde los distintos lugares en los que se necesita
	// transmitir comandos de reloj:
	// - Cambio de Reloj
	typeTxCMD* CMD_RELOJ_ptr;
	byte* GET_ptr;
	byte cmd_encolado;
	tRELOJ_ERROR error;

	if (HAY_CMDS_RELOJ_ENCOLADOS && (!RELOJ_chkTransmitiendo())){

		GET_ptr = RELOJ_TxGET;                                    // Puntero GET para extraer datos sin avanzar el puntero "real"

		cmd_encolado = RELOJ_TxBuffer_get(&GET_ptr);              // Extraigo comando encolados
		CMD_RELOJ_ptr = RELOJ_verificarCMDencolado(cmd_encolado); // Verifico que el comando encolados sea un CMD de RELOJ

		if (CMD_RELOJ_ptr != &CMD_NULL){
			// Si se trata de un comando valido, es xq el puntero antes devuelto NO es el puntero al
			// comando NULO
			// Entonces, lo que tengo que hacer es copiar los datos del buffer de RELOJ al buffer del
			// comando en cuestión, cargar su contador de reintentos y levantar su bandera de transmisión
			error = RELOJ_TxBuffer_to_CMD(CMD_RELOJ_ptr, &GET_ptr); // Cargo el CMD desde el Buffer de Tx de Reloj

			if (error == RLJ_OK){
				// No hay error => Transmito el comando
				CMD_RELOJ_ptr->Tx_F = 1;                    // Levanto Bandera de Tx
				CMD_RELOJ_ptr->Reintentos = reintINFINITOS; // Los comandos de reloj tienen reintentos infinitos
			}else{
				// En cualquier caso de error, por las dudas, Re Inicializo el buffer de reloj
				RELOJ_reset();                      // Reset de Reloj
			}
		}
	}
}


/* CHEQUEAR SI SE ESTA TRANSMITIENDO ALGUN COMANDO DE RELOJ */
/************************************************************/
	static byte RELOJ_chkTransmitiendo (void){
		// Rutina que se fija si se esta transmitiendo algún comando de reloj. Para ello se
		// fija si TODOS los contadores de reintentos son nulos. En caso de ser los, no se
		// esta transmitiendo.
		// Se devuelve un '1' si esta transmitiendo y '0' si no lo esta
		byte i, transmitiendo;

		transmitiendo = 0;                // Asumo que no estoy transmitiendo
		for (i=0; i<(sizeof(Tabla_CMD_Reloj)/4); i++){    // Divido el tamaño /2 xq la tabla contiene direcciones  con uint32_t
			if (Tabla_CMD_Reloj[i]->Reintentos > 0){
			  transmitiendo = 1;
			  break;
			}
		}
		if(!transmitiendo){
			if (CMD_Pase_a_LIBRE_SC.Reintentos > 0){
				transmitiendo = 1;
			}else if(CMD_Pase_a_OCUPADO_APP_SC.Reintentos > 0){
				transmitiendo = 1;
			}else if(CMD_Pase_a_OCUPADO_SA_SC.Reintentos > 0){
				transmitiendo = 1;
			}else if(CMD_Pase_a_COBRANDO_SC.Reintentos > 0){
				transmitiendo = 1;
			}
		}

		return(transmitiendo);
	}

/* VERIFICACION DE COMANDO ENCOLADO */
/************************************/
	static typeTxCMD* RELOJ_verificarCMDencolado (byte cmd){
		// Esta rutina verifica que el comando pasado como argumento sea un comando de
		// reloj VALIDO. En caso de ser valido, devuelve un puntero al comando de reloj
		// en cuestión; indicando de esta manera, de que comando se trata. En caso
		// contrario devuelve un puntero a CMD_NONE indicando ERROR.
		byte i;
		typeTxCMD* cmd_ptr;
		byte TO_F;

		i = 0;                            // Reseteo variable
		cmd_ptr = &CMD_NULL;              // Puntero a comando inválido (ERROR)
		TO_F = 0;                         // Reseteo Bandera de TimeOut
		dispararTO_lazo();                // Disparo Time Out de Lazo
		while ((i<(sizeof(Tabla_CMD_Reloj)/4)) && (cmd_ptr == &CMD_NULL) && !TO_F){    // Divido el tamaño /2 xq es una tabla de WORDs
			TO_F = chkTO_lazo_F();          // Chequeo bandera de time out de lazo

			if (cmd == Tabla_CMD_Reloj[i]->cmd){
				cmd_ptr = Tabla_CMD_Reloj[i]; // Guardo el puntero al tipoCMD y salgo del while
			}

			i++;                            // Incremento contador
		}
		detenerTO_lazo();                 // Detengo Time Out de Lazo

		return(cmd_ptr);
	}


/* PASAR DATOS DEL BUFFER DE TX DEL RELOJ AL BUFFER DE CMD */
/***********************************************************/
	static tRELOJ_ERROR RELOJ_TxBuffer_to_CMD (typeTxCMD* CMD_ptr, byte** GET_ptr_ptr){
		byte dato1, dato2, n;
		byte* cmd_buffer_ptr;
		tRELOJ_ERROR error;

		error = RLJ_OK;                             // No hay error

		cmd_buffer_ptr = CMD_ptr->buffer;           // Puntero del Comando donde se guardan datos

		n = 0;                                      // Reseteo contador de bytes copiados
		dato1 = RELOJ_TxBuffer_get(GET_ptr_ptr);		// Extraigo 1er dato y avanzo puntero
		//dato1 = CMD_ptr->cmd;
		dato2 = **GET_ptr_ptr;                      // Extraigo 2do dato
		while (((dato1 != fin_datos_msb) || (dato2 != fin_datos_lsb)) && !error){
			*(cmd_buffer_ptr)++ = dato1;

			dato1 = RELOJ_TxBuffer_get(GET_ptr_ptr);	// Extraigo 1er dato y avanzo puntero
			dato2 = **GET_ptr_ptr;                    // Extraigo 2do dato

			if (*GET_ptr_ptr == RELOJ_TxPUT){
				// Mi puntero GET temporal, alcanzo al puntero PUT. Lo cual no debería suceder
				// xq al finalizar el puntero GET temporal debería estar apuntando al fin_datos_lsb
				error = RLJ_reachPUT;                   // Alcance al puntero PUT => salgo
			}

			n++;                                      // Incremento contador de bytes copiados
			if (n >= 25){
				// Si tras haber copiado 25bytes, no encuentro el fin de datos, es xq algo no anda
				// bien. Por eso salgo del lazo e indico que no encontré el FIN
				error = RLJ_noFIN;                      // No encontré el FIN => salgo
			}
		}

		if (!error){
			// Si al salir del lazo no hubo error => Falta agregar el fin de datos
			*(cmd_buffer_ptr)++ = fin_datos_msb;      // Agrego parte baja de fin de datos
			*(cmd_buffer_ptr)++ = fin_datos_lsb;      // Agrego parte baja de fin de datos
		}

		return(error);
	}

/* RESET DE RELOJ */
/******************/
void RELOJ_reset (void){
		RELOJ_TxPUT = RELOJ_TxBuffer;     // PUT a inicio de Buffer
		RELOJ_TxGET = RELOJ_TxBuffer;     // GET a inicio de Buffer

		RELOJ_resetBuffer();              // Reseteo buffer de reloj, para cuando re-inicializo

		RELOJ_CMDencolados_cnt = 0;       // Reseteo contador de comandos encolados
	}


/* RESET BUFFER RELOJ */
/**********************/
	static void RELOJ_resetBuffer (void){
		uint16_t i;

			for(i=0; i<dim_RELOJ; i++){
			RELOJ_TxBuffer[i] = 0;                        // Limpio byte del buffer
			}
	}


	/* RESET BUFFER RELOJ */
	/**********************/

static void RELOJ_resetReintentos (void){
	uint16_t i;

	for (i=0; i<(sizeof(Tabla_CMD_Reloj)/4); i++){
		Tabla_CMD_Reloj[i]->Reintentos = 0;
	}
}




      /* AVANZAR PUNTERO GET AL FIN DE DATOS */
       /***************************************/
         static tRELOJ_ERROR RELOJ_incGETptr_endDATA (void){
           // Rutina que avanzar el puntero GET hasta el fin de datos
           // Eso si, siempre y cuando no rebase al puntero PUT.
           byte dato1, dato2, n;
           tRELOJ_ERROR error;

           error = RLJ_OK;                             // Asumo que no va a haber error

     			n = 0;                                      // Reseteo contador de bytes
     			dato1 = RELOJ_TxBuffer_get(&RELOJ_TxGET);   // Extraigo 1er dato y avanzo puntero
     			dato2 = *RELOJ_TxGET;                       // Extraigo 2do dato

           dispararTO_lazo();                          // Disparo Time Out de Lazo
           while (((dato1 != fin_datos_msb) || (dato2 != fin_datos_lsb)) && !error){
             dato1 = RELOJ_TxBuffer_get(&RELOJ_TxGET);	// Extraigo 1er dato y avanzo puntero
             dato2 = *RELOJ_TxGET;                     // Extraigo 2do dato

             if (RELOJ_TxGET == RELOJ_TxPUT){
               // Alcance al puntero PUT. Lo cual no debería suceder xq al finalizar el putnero GET
               // debería estar apuntando al fin_datos_lsb
               error = RLJ_reachPUT;                   // Alcance al puntero PUT => salgo
             }

             n++;                                      // Incremento contador de bytes
             if (n >= 25){
               // Si tras haber avanzado 25bytes, no encuentro el fin de datos, es xq algo no anda
               // bien. Por eso salgo del lazo e indico que no encontre el FIN
               error = RLJ_noFIN;                      // No encontre el FIN => salgo
             }

             if (chkTO_lazo_F()){
               error = RLJ_TO;                         // Time Out de Reloj
             }
           }
           detenerTO_lazo();                           // Detengo Time Out de Lazo

           if (error == RLJ_OK){
             // Si luego del lazo no hay error => es xq encontro el fin, pero el puntero
             // quedo apuntando al LSB del fin. Por eso lo incremento una vez mas
             RELOJ_incGET;                             // Avanzo puntero GET
           }

           return(error);
         }

         /* SOLICITAR GRABACION DE BKP DE BUFFER DE Tx DE RELOJ EN FLASH */
         /****************************************************************/
           static void RELOJ_requestBKP_enFLASH (void){
             //RELOJ_doBkpFLASH = 1;
            // _RELOJ_F.Bits.bkp_Flash = 1;
           }

/* DISPARAR INDICACION DE TARIFANDO POR TIEMPO */
/***********************************************/
	void dispararIndicacionTarifaTiempo (void){
		timer_tarifaTiempo_cnt = x100mseg_timer_tarifaTiempo;
	}

/* DETENER INDICACION DE TARIFANDO POR TIEMPO */
/**********************************************/
	void detenerIndicacionTarifaTiempo (void){
		timer_tarifaTiempo_cnt = 0;
	}

/* Inicialización de Puerto de Salida de Bandera */
/*************************************************/

	void BanderaOut_Ini(void)
	{
		GPIO_InitTypeDef  GPIO_InitStruct;


		if(!EQUIPO_MINI_BLUE){
			/* Enable the BANDERA OUT Clock */
			BANDERA_OUT_CLK_ENABLE();

			/* Configure the GPIO_LED pin */
			GPIO_InitStruct.Pin = BANDERA_OUT_PIN;
			GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
			GPIO_InitStruct.Pull = GPIO_PULLUP;
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

			HAL_GPIO_Init(BANDERA_OUT_PORT, &GPIO_InitStruct);
		}
	}

	/* SET Puerto de Salida de Bandera */
	/***********************************/

	void BanderaOut_On(void)
	{
		if(!RELOJ_BANDERITA){
			HAL_GPIO_WritePin(BANDERA_OUT_PORT, BANDERA_OUT_PIN, GPIO_PIN_SET);
		}
	}

	/* RESET Puerto de Salida de Bandera */
	/*************************************/

	void BanderaOut_Off(void)
	{
		//porque la salida de bandera va a la entrada de banderita
		if(!RELOJ_BANDERITA){
			HAL_GPIO_WritePin(BANDERA_OUT_PORT, BANDERA_OUT_PIN, GPIO_PIN_RESET);
		}
	}


	/* Conmuto Puerto de Salida de Bandera */
	/*************************************/

	void BanderaOut_Toggle(void)
	{
		if(!RELOJ_BANDERITA){
			HAL_GPIO_TogglePin(BANDERA_OUT_PORT, BANDERA_OUT_PIN);
		}
	}


/* MOSTRAR IMPORTE */
/*******************/

	void preparar_print (uint32_t importe, byte decimales, byte* buffer_reloj, byte printPesos){
	// Muestra en pantalla un importe.
		byte espacios;
		byte* buffer;
		byte importe_ascii[20];
		byte digitos;

		buffer = importe_ascii;

		convert_to_string_with_decimals (buffer, importe, 0xFF, decimales, base_DECIMAL);
		digitos = string_length(importe_ascii);


		if(TARIFA_PESOS){

			if(importe < 99999){
				espacios = 5 - digitos;  // Cantidad de espacios en blanco para completar 5 digitos
			}else{
				espacios =0;
			}

		}else{
			if(importe < 999999){
				espacios = 6 - digitos;  // Cantidad de espacios en blanco para completar 5 digitos
			}else{

				espacios = 0;
			}
		}

		if (decimales != 0){
			espacios += 1;                              // El length incluye el punto decimal entonces me restó un espacio
		}

		buffer = buffer_reloj;


		while (espacios > 0){
			espacios--;
			if(espacios > 5){
				break;
			}
			*buffer++ = ' ';                          // Agrego espacio
		}

		if (printPesos){
			*buffer++ = '$';                          // Agrego signo PESOS
			*buffer++ = ' ';                          // Agrego espacio en blanco
		}else{
			//*buffer++ = ' ';                        // Agrego espacio en blanco
			*buffer++ = ' ';                          // Agrego espacio en blanco
		}

		*buffer++ = 0;                                // Agrego fin de cadena para concatenar

		string_concat(buffer_reloj, importe_ascii);   // Concateno importe
	}


	void preparar_print_poneCOMA (uint32_t importe, byte decimales, byte* buffer_reloj, byte printPesos){
		// Muestra en pantalla un importe.
			byte espacios;
			byte* buffer;
			byte importe_ascii[20];
			byte digitos;

			buffer = importe_ascii;

			convert_to_string_with_decimals_poneCOMA (buffer, importe, 0xFF, decimales, base_DECIMAL);
			digitos = string_length(importe_ascii);


			if(TARIFA_PESOS){

				if(importe < 99999){
					espacios = 5 - digitos;  // Cantidad de espacios en blanco para completar 5 digitos
				}else{
					espacios =0;
				}

			}else{
				if(importe < 999999){
					espacios = 6 - digitos;  // Cantidad de espacios en blanco para completar 5 digitos
				}else{

					espacios = 0;
				}
			}

			if (decimales != 0){
				espacios += 1;                              // El length incluye el punto decimal entonces me restó un espacio
			}

			buffer = buffer_reloj;


			while (espacios > 0){
				espacios--;
				if(espacios > 5){
					break;
				}
				*buffer++ = ' ';                          // Agrego espacio
			}

			if (printPesos){
				*buffer++ = '$';                          // Agrego signo PESOS
				*buffer++ = ' ';                          // Agrego espacio en blanco
			}else{
				//*buffer++ = ' ';                        // Agrego espacio en blanco
				*buffer++ = ' ';                          // Agrego espacio en blanco
			}

			*buffer++ = 0;                                // Agrego fin de cadena para concatenar

			string_concat(buffer_reloj, importe_ascii);   // Concateno importe
		}


	void preparar_print_new (uint32_t importe, byte decimales, byte* buffer_reloj, byte printPesos, byte** buffer_print){
	// Muestra en pantalla un importe.
		byte espacios;
		byte* buffer;
		byte importe_ascii[20];
		byte digitos;

		buffer = importe_ascii;

		convert_to_string_with_decimals (buffer, importe, 0xFF, decimales, base_DECIMAL);
		digitos = string_length(importe_ascii);


		if(TARIFA_PESOS){

			if(importe < 99999){
				espacios = 5 - digitos;  // Cantidad de espacios en blanco para completar 5 digitos
			}else{
				espacios =0;
			}

		}else{
			if(importe < 999999){
				espacios = 6 - digitos;  // Cantidad de espacios en blanco para completar 5 digitos
			}else{

				espacios = 0;
			}
		}

		if (decimales != 0){
			espacios += 1;                              // El length incluye el punto decimal entonces me restó un espacio
		}

		buffer = buffer_reloj;


		while (espacios > 0){
			espacios--;
			if(espacios > 5){
				break;
			}
			*buffer++ = ' ';                          // Agrego espacio
		}

		if (printPesos){
			*buffer++ = '$';                          // Agrego signo PESOS
			*buffer++ = ' ';                          // Agrego espacio en blanco
		}else{
			//*buffer++ = ' ';                        // Agrego espacio en blanco
			*buffer++ = ' ';                          // Agrego espacio en blanco
		}

		*buffer++ = 0;                                // Agrego fin de cadena para concatenar

		if(digitos > 6){
			byte i = digitos - 6;
			while(i>0){
				(*buffer_print)--;
				i--;
			}

		}
		string_concat(buffer_reloj, importe_ascii);   // Concateno importe
	}


void preparar_print_nroTICKET (uint32_t nroTICKET,  byte* bufferTICKET){
	// Muestra en pantalla un importe.
		byte espacios;
		byte* buffer;
		byte importe_ascii[20];

		buffer = importe_ascii;

		convert_to_string_with_decimals (buffer, nroTICKET, 0xFF, 0, base_DECIMAL);
		espacios = 10 - string_length(importe_ascii);  // Cantidad de espacios en blanco para completar 10 digitos
		buffer = bufferTICKET;

		while (espacios > 0){
			espacios--;
			if(espacios > 10){
				break;
			}
			*buffer++ = ' ';                            // Agrego espacio
		}
		*buffer++ = 0;                                // Agrego fin de cadena para concatenar
		string_concat(bufferTICKET, importe_ascii);   // Concateno importe
	}

	  /* TOMAR DATE DE CAMBIO DE RELOJ EN EL ARRANQUE */
	  /************************************************/
	    void RELOJ_tomarDateCambioReloj_Arranque (void){
	      // Se llama cuando se tiene el primer DATE valido, en onFirstDate
	      RELOJ_dateCambio = getDate();       // Guardo DATE del cambio de reloj (arranque)
	    }

	  /* EXTRAER DATE DE CAMBIO DE RELOJ */
	  /***********************************/
	    tDATE RELOJ_getDateCambio(void){
	      return(RELOJ_dateCambio);
	    }


/* GRABACION DE NUMERO CORRELATIVO DE VIAJE EN EEPROM */
 /******************************************************/

   void inc_nroCorrelativo (void){
		// Incremento valor del Nro Correlativo y lo grabo en EEPROM

	   error_t error;

		if (nroCorrelativo_INTERNO == 0xFF){
			nroCorrelativo_INTERNO = 1;
		}else{
			nroCorrelativo_INTERNO++;
		}
		error = EEPROM_WriteByte(EEPROM_NRO_CORRELATIVO, nroCorrelativo_INTERNO);

		if (error != errNone){
			_Error_Handler(__FILE__, __LINE__);
		}
	}

   byte Read_nroCorrelativo_from_EEPROM (void){

	   byte data;
	   data = EEPROM_ReadByte(EEPROM_NRO_CORRELATIVO);
	   return(data);
   }


#ifdef _ABONAR_SEGUN_TARIFA_HORA_A_PAGAR_
  /* ACTUALIZAR TARIFA (MONTEVIDEO) */
  /**********************************/
  static tTARIFA* updateTarifa (tDATE date, byte numTarifaActual){
      // Actualiza la tarifa, por si se ocupo en DIURNA/NOCTURNA y se desocupó en la
      // opuesta
      tTARIFA* tarifaDiurnaPTR;
      tTARIFA* tarifaNocturnaPTR;
      tTARIFA* resultTarifaPTR;
      byte numD;

      tarifaDiurnaPTR = NULL;
      tarifaNocturnaPTR = NULL;

      if (numTarifaActual >= tarifa1N){
        // Quiere decir que la tarifa actual es NOCTURNA
        numD = numTarifaActual-tarifa1N+1;
      }else{
        numD = numTarifaActual;
      }

      // DIURNA
      tarifaDiurnaPTR = prgRELOJ_getTarifa_pointer(numD);



      // NOCTURNA
      if (prgRELOJ_determineCantTarifasN() >= numD){
        // Si existe la nocturna; es decir si hay al menos tantas nocturnas como la diurna actual
        tarifaNocturnaPTR = prgRELOJ_getTarifa_pointer(numD + (tarifa1N - 1) );
      }

      // TENGO que determinar cual de las 2 elijo.
      // La tarifa NOCTURNA puede estar programada o no. En cuyo caso la DIURNA esta
      // siempre vigente.
      if (tarifaDiurnaPTR != NULL){
        if (tarifaNocturnaPTR == NULL){
          // No existe la nocturna => Queda vigente la DIURNA. (puede ser que sea NULL
          // lo cual indicaria un error, pero eso lo veo mas adelante; es decir que lo
          // propago)
          resultTarifaPTR = tarifaDiurnaPTR;

        }else{
          // Existen tanto la DIURNA como la NOCTURA => Depende de la hora
          resultTarifaPTR = determineTarifaDiurnaNocturna(date, tarifaDiurnaPTR, tarifaNocturnaPTR);
        }

      }else{
        resultTarifaPTR = NULL;           // Hay un error xq no encontre tarifa Diurna
      }

      return(resultTarifaPTR);
    }
#endif

  /* AGREGAR A PAGAR A REPORTES */
   /******************************/
     void RELOJ_INTERNO_addAPagarReportes (uint8_t estado_de_conexion){
       // Esta misma rutina esta REPLICADA para cuando el importe se ingresa de manera MANUAL
       uint32_t importe;

       importe_INTERNO = VALOR_VIAJE;                          // Valor del Viaje
       //nroCorrelativo_INTERNO = Read_nroCorrelativo_from_EEPROM();        // Nro Correlativo de Viaje
       // Incremento y Grabo Nro Correlativo en EEPROM
      // if (RELOJ_nroCorrelativo_to_EEPROM() != errNone){
       //		_Error_Handler(__FILE__, __LINE__);
       //	}
       // Guardo reporte -> datos de OCUPADO
       #ifdef VISOR_REPORTES
         if (REPORTES_HABILITADOS){
           // Guardo reporte -> datos de A PAGAR
           //
           // EDIT 27/03/2013
           //  Para el caso en que trabaje con FICHAS con EQ.PESOS, como importe NO DEBE pasar el valor del viaje,
           // sino la equivalencia en PESOS
           // En TARIFACION NORMAL, el importe lo calculo, para que en caso de que sea FICHAS, sea la equivalencia


			if(paseOCUPADO_PACTADO){
				importe = VALOR_VIAJE;
			}else{
				importe = RELOJ_calcularImporte(TARIFACION_getFichasT(), TARIFACION_getFichasD(), 1, TARIFA.numero, NULL);
			}

           (void)REPORTE_queueAPagar (cobrandoDATE, RELOJ_INTERNO_getChofer(), nroCorrelativo_INTERNO, TARIFA.numero,
        		   TARIFACION_getFichasD(), TARIFACION_getFichasT(), importe, segundosEspera, segundosTarifacion, estado_de_conexion,
				   getLATITUD(COBR), getLONGITUD(COBR), getSgnLatLon(COBR), getVELOCIDAD(COBR));

           // EDIT 04/04/2013
           //  Para permitir la correcta recepcion de la respuesta, demoro la grabacion de FLASH, para
           // no inhibir las IRQs
           //REPORTES_delayFLASH_update();                       // Demoro grabacion de REPORTES en FLASH
         }
       #endif
     }

     /* INICIO DE SESION EN EL PRIMER ARRANQUE */
      /******************************************/
        void CHOFER_firstBoot (void){
          // En el primer arranque, se debe iniciar turno con el chofer 1
          CHOFER_setNewChofer(1);         // Chofer 1
          CHOFER_update = 1;              // Grabar CHOFER en Flash
          nroChofer = 1;
          #ifdef VISOR_REPORTES
            if (REPORTES_HABILITADOS){
              (void)REPORTE_queueSesion(1);   // Inicio de Sesion con chofer 1
              //parche
              (void)REPORTE_queueFueraServ (RELOJ_getDateCambio(), nroChofer, 0, 0, 0, 0, SIN_CONEXION_CENTRAL);
            }
          #endif
        }


      /* SETEAR CHOFER CAMBIAR */
      /*************************/
        void CHOFER_setNewChofer (byte newChofer){
          // Rutina que se llama para guardar el chofer al cual se quiere pasar
          CHOFER_newValue = newChofer;
        }




      /* ACTUALIZAR CHOFER EN FLASH */
      /******************************/
        void relojINTERNO_updateCHOFER (void){
          // Rutina que se llama desde el LOOP principal y graba la flash cuando se dan las condiciones
          byte flashData[2];
          tFLASH_ERROR error;
          byte* data_ptr;
          byte* flash_ptr;
          uint16_t chofer;

          if (RELOJ_INTERNO && CHOFER_update /*&& FLASH_chkCanUpdate()*/){
            CHOFER_update = 0;

            //flashData[0] = 5;
            //flashData[0] = CHOFER_newValue;
            //flashData[1] = 0;
            //flash_ptr =	(byte*) &flashData;
            //data_ptr = (byte*) flash_ptr;
            chofer = (uint16_t)CHOFER_newValue;
            data_ptr = (byte*) &chofer;
            EEPROM_write(ADDR_EEPROM_CHOFER, chofer);

            //error = FLASH_updateSector((uint16_t*) ADDR_EEPROM_CHOFER, data_ptr , (uint16_t) 2);
            //error = FLASH_updateSector((uint16_t*) ADDR_EEPROM_REPORTE_INDEX, data_ptr , (uint16_t) 2);

            //EEPROM_write(ADDR_EEPROM_CHOFER, (uint16_t)flashData);

            EEPROM_ReadBuffer(data_ptr,ADDR_EEPROM_CHOFER,SIZE_EEPROM_CHOFER);
            //EEPROM_ReadBuffer(data_ptr,ADDR_EEPROM_REPORTE_INDEX,SIZE_EEPROM_CHOFER);

            RELOJ_INTERNO_getChofer();
          }
        }


      /* EXTRAER CHOFER DE FLASH */
      /***************************/
        byte RELOJ_INTERNO_getChofer(void){
        byte chofer;

        uint16_t CHOFER;

			//addressFLASH_CHOFER = (byte*)&FLASH_CHOFER;
			//chofer = *(addressFLASH_CHOFER+0);
            EEPROM_ReadBuffer(&CHOFER,ADDR_EEPROM_CHOFER,SIZE_EEPROM_CHOFER);
			chofer = (byte)CHOFER;
			return(chofer);
        }


      /* NUEVA SESION DE CHOFER */
      /**************************/
        void RELOJ_INTERNO_newSesion (byte nroChofer){
          // Como el inicio de sesion sólo puede hacerse estando en LIBRE, antes de efectuar el cambio de chofer,
          // debo guardar el reporte del chofer actual, en el estado actual (LIBRE).
          // Luego de guardar los datos del chofer anterior, debe cerrar la sesion del chofer anterior e iniciar
          // la del nuevo.
          // Si se trata de un INICIO de sesion => El chofer anterior es SIEMPRE el chofer DEFAULT
          kmRecorridos_INTERNO = calcularDISTANCIA_entreEstados;				// Distancia Recorrida en LIBRE (KM xxx.x)
          velMax_INTERNO = VELOCIDAD_MAX;                         			    // Velocidad Maxima en LIBRE
          //nroCorrelativo_INTERNO = Read_nroCorrelativo_from_EEPROM();         // Nro Correlativo de Viaje


          #ifdef VISOR_REPORTES
            if (REPORTES_HABILITADOS){
              //(void)REPORTE_queueLibre (RELOJ_getDateCambio(), RELOJ_INTERNO_getChofer(),kmRecorridos_INTERNO, velMax_INTERNO, timerParado_cnt, timerMarcha_cnt, 0);
            	// Guardo reporte -> datos de f.servicio (porque la nueva sesion siempre es en fuera de servicio)

            	(void)REPORTE_queueFueraServ (RELOJ_getDateCambio(), nroChofer,kmRecorridos_INTERNO, velMax_INTERNO, timerParado_cnt, timerMarcha_cnt, CON_CONEXION_CENTRAL);

              // Guardo reporte -> nueva SESION
              (void)REPORTE_queueSesion (nroChofer);
            }
          #endif

          VELOCIDAD_MAX = 0;
          RELOJ_INTERNO_resetMarchaParado();

          CHOFER_update = 1;
          CHOFER_newValue = nroChofer;
        }


      /* RE-APERTURA DE TURNO POR MEMORIA LLENA */
      /******************************************/
        void RELOJ_INTERNO_reOpenTurno (void){
          // Cuando la memoria de los reportes esta por llenarse se lo invita al chofer a
          // cerrar el turno.
          // Y en caso de que se llegue a una situacion límite, se auto-reabre el turno
          // sin la intervencion del chofer.
          //
          // Cuando esto sucede, sólo tengo que guardar un registro del tipo de SESION,
          // sin el LIBRE como cuando hago un cambio de chofer.
          // Guardo reporte -> nueva SESION

        byte nroChofer;

          #ifdef VISOR_REPORTES
			kmRecorridos_INTERNO = calcularDISTANCIA_entreEstados;				// Distancia Recorrida en LIBRE (KM xxx.x)
			velMax_INTERNO = VELOCIDAD_MAX;                         			    // Velocidad Maxima en LIBRE

			 if (REPORTES_HABILITADOS){
			   // Guardo reporte -> nueva SESION
				nroChofer = RELOJ_INTERNO_getChofer();
			   (void)REPORTE_queueSesion (nroChofer);
			 }
			VELOCIDAD_MAX = 0;
			RELOJ_INTERNO_resetMarchaParado();
			if(!(EQUIPO_METRO_LITE_RELOJ_BANDERITA)){
				Tx_Comando_MENSAJE(TURNO_CERRADO);
			}
          #endif
        }



           /* SETEAR OCUPADO POR SENSOR DE ASIENTO */
            /****************************************/
              void RELOJ_INTERNO_setAsiento(byte causa){
                sensorAsiento = causa;
              }

              byte get_sensorAsiento(void){
            	  return(sensorAsiento);
              }
/*********************************************************************************************/
/* TIEMPO MARCHA y TIEMPO PARADO */
/*********************************/

/* RESETEO DE TIEMPO MARCHA/PARADO */
/***********************************/
void RELOJ_INTERNO_resetMarchaParado (void){
   // Se llama en cada cambio de reloj (mas alla de que se interno o no). Y tambien se
   // llama al iniciar sesion
   timerMarcha_cnt = 0;
   timerParado_cnt = 0;
 }


/* TIMER DE TIEMPO MARCHA/PARADO */
/*********************************/
void RELOJ_INTERNO_timerMarchaParado (void){
	// Ejecutada cada 1seg
	// En cada cambio de RELOJ se resetea.
	if (VELOCIDAD > 0){
	  timerMarcha_cnt++;
	}else{
	  timerParado_cnt++;
	}
}


/* PASAR A LIBRE DESDE A PAGAR AUTOMATICAMENTE */
 /***********************************************/
   void RELOJ_a_pagar_to_libre (void){
     // Cuando se trabaja con reloj interno, y se esta en A PAGAR, luego de
     // recorrer una cierta distancia, se pasa automaticamente a LIBRE
     // Tener en cuenta que el timer de A PAGAR -> LBRE debe ser nulo, por
     // si el reloj es homologado
     //
     // EDIT 02/10/13 MiK
     //  Si se esta en TARIFACION ESPECIAL, no se hace uso de esta funcionalidad, ya que
     // la distancia acumulada es la del VIAJE (xq no se borra, para mantenerla en pantalla)
     // Y esto hasce que SIEMPRE se pase a LIBRE cuando pase a A PAGAR

	   uint32_t dist_recorrida;

	   if (RELOJ_INTERNO && RELOJ_COBRANDO){
         //(void)calcularDISTANCIA_acumulada;
    	   dist_recorrida = cal_dist();
    	   if ((dist_recorrida >= distanciaA_PAGAR_to_LIBRE) && (timerA_PAGAR_to_LIBRE_cnt == 0)){
    		 if(appConectada_ACentral){
    			 Pase_a_LIBRE(CON_CONEXION_CENTRAL);
    		 }else{
    			 Pase_a_LIBRE(SIN_CONEXION_CENTRAL);
    		 }

         }
       }
   }

void cambio_de_reloj_x_pulsador(void){

	#ifdef RELOJ_DEBUG
	 cambioRELOJ_TO_cnt = 0;
	#endif

    if(cambioRELOJ_TO_cnt == 0){
    	if(ESTADO_RELOJ_X_PULSADOR == LIBRE){
    			if(VELCOCIDAD_PERMITE_CAMBIO_RELOJ){

    				//if(tarifa_to_cnt==0){
    					if (seleccionManualTarifas){
    						//set tarifa a mostrar en display
    						if(tarifa_1_4 == nroTARIFA_HAB_MANUAL){
    							tarifa_1_4 = 1;
    						}else{
    							tarifa_1_4++;
    						};
    						setTARIFA_MANUAL();
    					}else{
    						tarifa_1_8 = TARIFA_AUTO_getNroTarifa();
    						//set tarifa a mostrar en display
    						if(tarifa_1_8 < 5){
								tarifa_1_4 = tarifa_1_8;
							}else{
								tarifa_1_4 = tarifa_1_8 - 4;
							}
    					}
    				//}//else{
    					//si se pulsa mas de una vez el boton de pase a ocupado
    					//pasa a tarifa manual.
    				 //	setTARIFA_MAX();
    				//}
    				tarifa_to_cnt = TARIFA_TO;
					update_valor_tarifa(tarifa_1_4);
					on_display_tarifa();
					Buzzer_On(BEEP_TECLA_CORTA);
    			}else{
    				//TxRta_conDATOS(CAMBIO_RELOJ_NO_PERMITIDO_MOV);
    				//Tx_Comando_MENSAJE(VEHICULO_EN_MOVIMIENTO);
    				Buzzer_On(VEHICULO_EN_MOVIMIENTO);
    			}
    	}else if(ESTADO_RELOJ_X_PULSADOR == OCUPADO){
    			if(VELCOCIDAD_PERMITE_CAMBIO_RELOJ){
    		    	Pase_a_COBRANDO(CON_CONEXION_CENTRAL);
    		    	//TxRta_conDATOS(CAMBIO_RELOJ_PERMITIDO);
    		    	Buzzer_On(BEEP_TECLA_CORTA);
    			}else{
    				//TxRta_conDATOS(CAMBIO_RELOJ_NO_PERMITIDO_MOV);
    				//Tx_Comando_MENSAJE(VEHICULO_EN_MOVIMIENTO);
    				Buzzer_On(VEHICULO_EN_MOVIMIENTO);
    			}
    	}else if(ESTADO_RELOJ_X_PULSADOR == COBRANDO){
    	    	Pase_a_LIBRE(CON_CONEXION_CENTRAL);
    	    	Buzzer_On(BEEP_TECLA_CORTA);
    	    	tarifa_1_4=0;
    	}
    }else{
    	Buzzer_On(BEEP_TECLA_CORTA);
    }

}


//setea la tarifa_1_8, que es el numero de tarifa que usara en el
//pase a ocupado para levantar la tarifa que coresponde
void setTARIFA_MANUAL(void){
	//tarifa_1_4 tarifa a mostrar en display
	//tarifa_1_8 indice que determina cual de las 8 tarifas levanto de tabla

	tTARIFA TARIFA_D;
	tTARIFA TARIFA_N;
	tTARIFA* TARIFA_D_ptr;
	tTARIFA* TARIFA_N_ptr;
	tTARIFA* resultTarifaPTR;
	byte hab;

	TARIFA_D = prgRELOJ_getTarifa(tarifa_1_4);
	TARIFA_N = prgRELOJ_getTarifa(tarifa_1_4+4);
	hab = 0;
	//me fijo si tarifa nocturna esta habilitada
	if((TARIFA_N.numero == 5)  &&  T1N_hab){
		hab = 1;
	}else if((TARIFA_N.numero == 6) &&  T2N_hab){
		hab = 1;
	}else if((TARIFA_N.numero == 7) &&  T3N_hab){
		hab = 1;
	}else if((TARIFA_N.numero == 8) && T4N_hab){
		hab = 1;
	}

	if(hab){
		//tarifa nocturna habilitada
		//la tarifa depende del horario
		TARIFA_D_ptr = &TARIFA_D;
		TARIFA_N_ptr = &TARIFA_N;
        getDate();
		resultTarifaPTR = determineTarifaDiurnaNocturna(RTC_Date,TARIFA_D_ptr,TARIFA_N_ptr);
		tarifa_1_8 = resultTarifaPTR->numero;
	}else{
		//tarifa nocturna no habilitada
		//por lo tanto siempre es diurna
		tarifa_1_8 = tarifa_1_4;
	}
}


void cambio_de_reloj_x_sensor_asiento(void){

	if(ESTADO_RELOJ == LIBRE){
		if(appConectada_ACentral){
			Pase_a_OCUPADO(CON_CONEXION_CENTRAL);
		}else{
			Pase_a_OCUPADO(SIN_CONEXION_CENTRAL);
    		//parche para la app para que no se trabe cuando esta sin conexion
        	CMD_a_RESP = 0x24;
    		TxRta_conDATOS(CAMBIO_RELOJ_PERMITIDO);
    	}

	}else if(ESTADO_RELOJ == FUERA_SERVICIO){
    	Pase_a_LIBRE(CON_CONEXION_CENTRAL);

    }
}


/* TIMER A PAGAR -> LIBRE */
/**************************/
  void RELOJ_a_pagar_to_libre_timer (void){
    // Ejecutada cada 1seg no preciso.
    // Este timer se dispara cuando se pasa a A PAGAR y
    // estando el RELOJ HOMOLOGADO, se deben esperar 15seg
    // para pasar de A PAGAR -> LIBRE
    if (timerA_PAGAR_to_LIBRE_cnt != 0){
      timerA_PAGAR_to_LIBRE_cnt--;
      if (timerA_PAGAR_to_LIBRE_cnt == 0){
    	  Buzzer_On(BEEP_HABILITA_PASE_LIBRE);
          if(tipo_de_equipo == METRO_BLUE){
        	  Tx_Comando_MENSAJE(YA_PUEDE_PASAR_A_LIBRE);
          }
      }
    }
  }

  void cambioRELOJ_timer (void){
     // Ejecutada cada 1seg no preciso.
     // Este timer se dispara cuando se pasa a A PAGAR y
     // estando el RELOJ HOMOLOGADO, se deben esperar 15seg
     // para pasar de A PAGAR -> LIBRE
     if (cambioRELOJ_TO_cnt != 0){
    	 cambioRELOJ_TO_cnt--;
       if (cambioRELOJ_TO_cnt == 0){
       }
     }
   }



  /* CAMBIO DE RELOJ INTERNO */
    /***************************/
  /*
  void CambioReloj_INTERNO (void){
        // EDIT 31/07/2012
        //  Se debe esperar a que el contador del timer para pasar a de A PAGAR -> LIBRE
        // este detenido, para poder realizar un cambio de reloj interno
        // Para el caso en que el reloj NO este HOMOLOGADO, no espero nada, porque siempre
        // es nulo.
        // En cambio, para cuando SI lo esta, debo esperar 15seg


         if (RELOJ_INTERNO && (timerA_PAGAR_to_LIBRE_cnt == 0)){


          if (RELOJ_LIBRE){
            // PASE A OCUPADO
            PaseOcupado_INTERNO(TARIFA_AUTO);

          }else if (RELOJ_OCUPADO){
            // PASE A A PAGAR
            // En caso de que el reloj este homologado, debo esperar 15seg, antes de
            // permitirle volver a LIBRE
            #ifdef VISOR_RELOJ_HOMOLOGADO
              timerA_PAGAR_to_LIBRE_cnt = seg_A_PAGAR_to_LIBRE;
            #else
              timerA_PAGAR_to_LIBRE_cnt = 0;  // Al no estar Homologado, no espero nada
            #endif

            #ifdef VISOR_TARIFACION_ESPECIAL
              if (TARIFA.numero == nroTARIFA_ESPECIAL){
                IMPORTE_MANUAL_mostrar();
                ESTADO_RELOJ = A_PAGAR;                         // Actualizo Estado de Reloj
                PaseAPagar_INTERNO_especial();
              }else{
                PaseAPagar_INTERNO();
              }
            #else
              PaseAPagar_INTERNO();
            #endif

          }else if (RELOJ_COBRANDO){
            // PASE A LIBRE DESDE A PAGAR
            (void)calcularDISTANCIA_entreEstados;   // Resetear contador de Distancia

            paseLIBRE_interno_sinKM;          // Paso a LIBRE desde A PAGAR => no cuento KM

          #ifdef   _RELOJ_AVL_
            sendLIBRE;              //reloj compact envia datos por la serie
          #endif

          }else if (RELOJ_FUERA_SERVICIO){
            // PASE A LIBRE DESDE FUERA SERVICIO

        	  PaseLibreFromFServ_INTERNO();     // Paso a LIBRE desde FUERA SERVICIO

          #ifdef   _RELOJ_AVL_
            sendLIBRE;              //reloj compact envia datos por la serie
          #endif

          }else if (RELOJ_DESCONECTADO && RELOJ_conectado){
            // Si estaba en RELOJ DESCONECTADO y lo re-conecta (RELOJ_conectado = 1)
            // => lo paso a LIBRE
            (void)calcularDISTANCIA_entreEstados;   // Resetear contador de Distancia
            paseLIBRE_interno_sinKM;          // Paso a LIBRE desde DESCONECTADO => no cuento KM
          #ifdef   _RELOJ_AVL_
            sendLIBRE;              //reloj compact envia datos por la serie
          #endif

          }


          sensorAsiento = 0;                  // Borro Indicacion

          CambioReloj();                      // Acciones Comunes a TODOS los cambios de Reloj

          RELOJ_INTERNO_resetMarchaParado();  // Reset de tiempo de PARADO y MARCHA
        }
      }
*/


  //	void ocupadoDATE_enFLASH (byte forced){

  //		tFLASH_ERROR error;

  //	error = FLASH_ERR_NONE;
  //		if (ocupadoDATE_writeFLASH && (/*FLASH_chkCanUpdate() ||*/ forced)){
  //			ocupadoDATE_writeFLASH = 0;
  //			error = FLASH_updateSector((word*) &FLASH_ocupadoDATE, (byte*) &ocupadoDATE, (word) 8);
  //			if(error != FLASH_ERR_NONE){
  //				//si hubo error vuelvo a intentar
  //				cobrandoDATE_writeFLASH = 1;
  //			}

  //		}
  //	}


	//	void cobrandoDATE_enFLASH (byte forced){

	//		tFLASH_ERROR error;

	//error = FLASH_ERR_NONE;
	//if (cobrandoDATE_writeFLASH && (/*FLASH_chkCanUpdate() ||*/ forced)){
	//	cobrandoDATE_writeFLASH = 0;
	//	error = FLASH_updateSector((word*) &FLASH_cobrandoDATE, (byte*) &cobrandoDATE, (word) 8);
	//	if(error != FLASH_ERR_NONE){
	//		//si hubo error vuelvo a intentar
	//		cobrandoDATE_writeFLASH = 1;
	//	}
	//}
	//}


	void  ini_pulsador_cambio_reloj(void){
		ini_tecla1();
	}

 void Tx_Pase_a_LIBRE  (byte tipo){
	    /*
	    LIBRE
	    ******
	    KM_ptr  	 		(2 BYTE )km recorrido en libre
	    velMAX  			(1 BYTE)velocidad máxima en libre
	    timerMarcha_cnt		(2 BYTE)tiempo en marcha en libre
	    timerParado_cnt  	(2 BYTE)tiempo parado en libre

	    OCUPADO
	    *******
	    KM_ptr  	 		(2 BYTE)km recorrido en ocupado
	    velMAX  	 		(1 BYTE)velocidad máxima en ocupado
	    timerMarcha_cnt		(2 BYTE)tiempo en marcha en ocupado
	    timerParado_cnt  	(2 BYTE)tiempo parado en ocupado
	    MIN	    	 		(1 BYTE)minutos de espera
	    tarifa  	 		(1 BYTE)tarifa
	    importe_ptr  		(4 BYTE)importe de viaje
	    chofer  	 		(1 BYTE)nro chofer con que inicio turno (1,2,3,4)
	    NC      	 		(1 BYTE)nro correlativo
	    */

	   //corregir bien el cambio de km a metros
		byte* ptr_mRecorridos_LIBRE;
		byte* ptr_mRecorridos_OCUPADO;


		byte* ptr_timerMarcha_cnt_LIBRE;
		byte* ptr_timerMarcha_cnt_OCUPADO;
		byte* ptr_timerParado_cnt_LIBRE;
		byte* ptr_timerParado_cnt_OCUPADO;
 		byte* ptrVALOR_VIAJE;
 		byte* ptrVALOR_VIAJE_mostrar;


		byte* ptr_importe;

		ptr_mRecorridos_LIBRE 		= (byte*)&DISTANCIAm_LIBRE;
		ptr_mRecorridos_OCUPADO 	= (byte*)&DISTANCIAm_OCUPADO;
		ptr_timerMarcha_cnt_LIBRE   = (byte*)&timerMarcha_cnt_LIBRE;
		ptr_timerMarcha_cnt_OCUPADO = (byte*)&timerMarcha_cnt_OCUPADO;
		ptr_timerParado_cnt_LIBRE 	= (byte*)&timerParado_cnt_LIBRE;
		ptr_timerParado_cnt_OCUPADO = (byte*)&timerParado_cnt_OCUPADO;
		ptr_importe = (byte*)&importe_INTERNO;

 		ptrVALOR_VIAJE = &VALOR_VIAJE;
 		ptrVALOR_VIAJE_mostrar = &VALOR_VIAJE_mostrar;

		if(tipo == CON_CONEXION_CENTRAL || tipo == CI_ENCENDIDO_EQUIPO){
			//BANDERA DE TRANSMISION
			CMD_Pase_a_LIBRE.Tx_F = 1;                      // Levanto Bandera de Tx
			CMD_Pase_a_LIBRE.Reintentos = reint_3;   // Cargo Cantidad de Reintentos (3)
		}else{
			//BANDERA DE TRANSMISION
			CMD_Pase_a_LIBRE_SC.Tx_F = 1;                      // Levanto Bandera de Tx
			CMD_Pase_a_LIBRE_SC.Reintentos = reint_3;   // Cargo Cantidad de Reintentos (3)
		}
        //
		getDate();

		//fuente de hora
		Pase_a_LIBRE_Buffer[0] = HORA_source;                           // Hora GPS/RTC (0:GPS - 1:RTC)

 		//DATE
 		Pase_a_LIBRE_Buffer[1] = RTC_Date.hora[0];   					// HORA
 		Pase_a_LIBRE_Buffer[2] = RTC_Date.hora[1];   					// MINUTOS
 		Pase_a_LIBRE_Buffer[3] = RTC_Date.hora[2];   					// SEGUNDOS
 		Pase_a_LIBRE_Buffer[4] = RTC_Date.fecha[0];  					// DIA
 		Pase_a_LIBRE_Buffer[5] = RTC_Date.fecha[1];  					// MES
 		Pase_a_LIBRE_Buffer[6] = RTC_Date.fecha[2];  					// AÑO

 		//LIBRE
 		Pase_a_LIBRE_Buffer[7]  = *(ptr_mRecorridos_LIBRE+2);  		//m recorrido en  LIBRE parte baja
 		Pase_a_LIBRE_Buffer[8]  = *(ptr_mRecorridos_LIBRE+1);  		//m recorrido en LIBRE parte alta
 		Pase_a_LIBRE_Buffer[9]  = *(ptr_mRecorridos_LIBRE+0);  		//m recorrido en LIBRE parte alta

 		Pase_a_LIBRE_Buffer[10]  = velMax_LIBRE;  					//velocidad máxima en LIBRE
 		Pase_a_LIBRE_Buffer[11]  = *(ptr_timerMarcha_cnt_LIBRE+1);  //tiempo de marcha parte alta
 		Pase_a_LIBRE_Buffer[12] = *(ptr_timerMarcha_cnt_LIBRE+0);  	//tiempo de marcha parte baja en LIBRE
 		Pase_a_LIBRE_Buffer[13] = *(ptr_timerParado_cnt_LIBRE+1);  	//tiempo parado parte alta en LIBRE
 		Pase_a_LIBRE_Buffer[14] = *(ptr_timerParado_cnt_LIBRE+0);  	//tiempo parado parte baja en LIBRE
 		//OCUPADO
 		Pase_a_LIBRE_Buffer[15]  = *(ptr_mRecorridos_OCUPADO+2);  	//m recorrido en OCUPADO parte baja
 		Pase_a_LIBRE_Buffer[16]  = *(ptr_mRecorridos_OCUPADO+1);  	//m recorrido en OCUPADO parte alta
 		Pase_a_LIBRE_Buffer[17]  = *(ptr_mRecorridos_OCUPADO+0);  	//m recorrido en OCUPADO parte alta

 		Pase_a_LIBRE_Buffer[18]  = velMax_OCUPADO;  					//velocidad máxima en OCUPADO parte alta
 		Pase_a_LIBRE_Buffer[19]  = *(ptr_timerMarcha_cnt_OCUPADO+1);  	//tiempo marcha en OCUPADO parte alta
 		Pase_a_LIBRE_Buffer[20]  = *(ptr_timerMarcha_cnt_OCUPADO+0); 	//tiempo marcha en OCUPADO parte baja

 		Pase_a_LIBRE_Buffer[21]  = *(ptr_timerParado_cnt_OCUPADO+1);  	//tiempo parado en OCUPADO parte alta
 		Pase_a_LIBRE_Buffer[22]  = *(ptr_timerParado_cnt_OCUPADO+0);  	//tiempo parado en OCUPADO parte baja

 		Pase_a_LIBRE_Buffer[23]  = minutosEspera;  						//minutos de espera
 		Pase_a_LIBRE_Buffer[24]  = TARIFA.numero;  						//numero de tarifa

 		//Pase_a_LIBRE_Buffer[25]  = *(ptr_importe+3);  					//importe parte baja
 		//Pase_a_LIBRE_Buffer[26]  = *(ptr_importe+2);  					//importe parte media baja
 		//Pase_a_LIBRE_Buffer[27]  = *(ptr_importe+1);  					//importe parte media alta
 		//Pase_a_LIBRE_Buffer[28]  = *(ptr_importe+0);  					//importe parte alta

		if(TARIFA_PESOS){
		//valor en pesos
			Pase_a_LIBRE_Buffer[25] = *(ptrVALOR_VIAJE+3);             //
			Pase_a_LIBRE_Buffer[26] = *(ptrVALOR_VIAJE+2);             //
			Pase_a_LIBRE_Buffer[27] = *(ptrVALOR_VIAJE+1);             //
			Pase_a_LIBRE_Buffer[28] = *(ptrVALOR_VIAJE+0);             //
		}else{
			//valor en fichas
			if(EqPESOS_hab){
				RedondeoValorViaje_3();
				VALOR_VIAJE_mostrar = VALOR_VIAJE/10;
				Pase_a_LIBRE_Buffer[25] = *(ptrVALOR_VIAJE_mostrar+3);             //
				Pase_a_LIBRE_Buffer[26] = *(ptrVALOR_VIAJE_mostrar+2);             //
				Pase_a_LIBRE_Buffer[27] = *(ptrVALOR_VIAJE_mostrar+1);             //
				Pase_a_LIBRE_Buffer[28] = *(ptrVALOR_VIAJE_mostrar+0);             //

			}else{
				Pase_a_LIBRE_Buffer[25] = *(ptrVALOR_VIAJE+3);             //
				Pase_a_LIBRE_Buffer[26] = *(ptrVALOR_VIAJE+2);             //
				Pase_a_LIBRE_Buffer[27] = *(ptrVALOR_VIAJE+1);             //
				Pase_a_LIBRE_Buffer[28] = *(ptrVALOR_VIAJE+0);             //
			}
		}

 		Pase_a_LIBRE_Buffer[29]  = PUNTO_DECIMAL;  						//opcional1
 		Pase_a_LIBRE_Buffer[30]  = nroChofer;  						//numero de chofer
 		Pase_a_LIBRE_Buffer[31]  = nroCorrelativo_INTERNO;  					//numero correlativo

		//Posicion libre
 		Pase_a_LIBRE_Buffer[32] = sgnLatLon_LIBRE_CEL;
		bufferNcopy(&Pase_a_LIBRE_Buffer[33] ,(byte*)&latitudGPS_LIBRE_CEL,3);
		bufferNcopy(&Pase_a_LIBRE_Buffer[36] ,(byte*)&longitudGPS_LIBRE_CEL, 3);
		Pase_a_LIBRE_Buffer[39] = velocidadGPS_LIBRE_CEL;
		Pase_a_LIBRE_Buffer[40] = datosSC_cntWORD;

 		//FIN
 		Pase_a_LIBRE_Buffer[N_DATOS_Pase_a_LIBRE] = fin_datos_msb;  	// Fin Datos
 		Pase_a_LIBRE_Buffer[N_DATOS_Pase_a_LIBRE+1] = fin_datos_lsb;	// Fin Datos
}


 void Tx_Pase_a_OCUPADO (uint8_t tipo){

 		byte* ptrVALOR_VIAJE;
 		byte* ptrVALOR_VIAJE_mostrar;
 		byte* ptrDISTAMNCIAm;
 		byte* ptrVELOCIDAD;

 		byte* ptrFichasDistancia;
 		byte* ptrFichasTiempo;
 		byte* ptrBajadaBandera;

 		uint32_t FichasDistancia;
 		uint32_t FichasTiempo;
 		uint16_t BajadaBandera;
		uint8_t tarifa_mostrar;

		//tipo = SIN_CONEXION_CENTRAL;
		if(paseOCUPADO_APP){
			if(tipo ==CON_CONEXION_CENTRAL){
		 	    CMD_Pase_a_OCUPADO_APP.Tx_F = 1;                     // Levanto Bandera de Tx
		 		CMD_Pase_a_OCUPADO_APP.Reintentos = reint_3;   		// Cargo Cantidad de Reintentos (INFINITOS)
			}else{
		 	    CMD_Pase_a_OCUPADO_APP_SC.Tx_F = 1;                 // Levanto Bandera de Tx
		 		CMD_Pase_a_OCUPADO_APP_SC.Reintentos = reint_3;   	// Cargo Cantidad de Reintentos (INFINITOS)
			}
		}else if(paseOCUPADO_SENSOR_ASIENTO){
			if(tipo ==CON_CONEXION_CENTRAL){
		 	    CMD_Pase_a_OCUPADO_SA.Tx_F = 1;                      // Levanto Bandera de Tx
		 		CMD_Pase_a_OCUPADO_SA.Reintentos = reint_3;   		// Cargo Cantidad de Reintentos (INFINITOS)
			}else{
		 	    CMD_Pase_a_OCUPADO_SA_SC.Tx_F = 1;                  // Levanto Bandera de Tx
		 		CMD_Pase_a_OCUPADO_SA_SC.Reintentos = reint_3;   	// Cargo Cantidad de Reintentos (INFINITOS)
			}

		}else if(paseOCUPADO_BANDERITA){
	 	    CMD_Pase_a_OCUPADO_BANDERA.Tx_F = 1;                      // Levanto Bandera de Tx
	 		CMD_Pase_a_OCUPADO_BANDERA.Reintentos = reint_3;   // Cargo Cantidad de Reintentos (INFINITOS)
		}else{
			//paseOCUPADO_BOTON
			CMD_Pase_a_OCUPADO.Tx_F = 1;                      // Levanto Bandera de Tx
	 		CMD_Pase_a_OCUPADO.Reintentos = reint_3;   // Cargo Cantidad de Reintentos (INFINITOS)
		}

 		ptrVALOR_VIAJE = (byte*)&VALOR_VIAJE;
 		ptrVALOR_VIAJE_mostrar = (byte*)&VALOR_VIAJE_mostrar;
 		ptrDISTAMNCIAm = (byte*)&DISTANCIAm;
 		ptrVELOCIDAD   = (byte*)&VELOCIDAD;

		ptrBajadaBandera = (byte*)&BajadaBandera;
		ptrFichasDistancia = (byte*)&FichasDistancia;
		ptrFichasTiempo = (byte*)&FichasTiempo;
		FichasDistancia = TARIFACION_getFichasD();
 		FichasTiempo =	TARIFACION_getFichasT();
 		BajadaBandera = TARIFA.bajadaBandera;                             // Bajada Bandera

		if(TARIFA.diaNoche == 0){
					//DIURNA
			if(paseOCUPADO_APP){
				tarifa_mostrar = tarifa;
			}else{
				tarifa_mostrar = tarifa_1_4;
			}
		}else{
			//NOCTURNA
			if(paseOCUPADO_APP){
				tarifa_mostrar = tarifa+0x80;;
			}else{
				tarifa_mostrar = tarifa_1_4+0x80;
			}
		}

		getDate();

		//fuente de hora
		Pase_a_OCUPADO_Buffer[0] = HORA_source;                           // Hora GPS/RTC (0:GPS - 1:RTC)

 		Pase_a_OCUPADO_Buffer[1] = RTC_Date.hora[0];   // HORA
 		Pase_a_OCUPADO_Buffer[2] = RTC_Date.hora[1];   // MINUTOS
 		Pase_a_OCUPADO_Buffer[3] = RTC_Date.hora[2];   // SEGUNDOS
 		Pase_a_OCUPADO_Buffer[4] = RTC_Date.fecha[0];  // DIA
 		Pase_a_OCUPADO_Buffer[5] = RTC_Date.fecha[1];  // MES
 		Pase_a_OCUPADO_Buffer[6] = RTC_Date.fecha[2];  // AÑO

 		Pase_a_OCUPADO_Buffer[7]  = tarifa_mostrar;  			  //numero de tarifa
 		Pase_a_OCUPADO_Buffer[8]  = fichaPESOS;  				  //ficha o pesos

		if(TARIFA_PESOS){
		//valor en pesos
			Pase_a_OCUPADO_Buffer[9] = PUNTO_DECIMAL;  				  //ficha o pesos
			Pase_a_OCUPADO_Buffer[10] = *(ptrVALOR_VIAJE+3);             //
			Pase_a_OCUPADO_Buffer[11] = *(ptrVALOR_VIAJE+2);             //
			Pase_a_OCUPADO_Buffer[12] = *(ptrVALOR_VIAJE+1);             //
			Pase_a_OCUPADO_Buffer[13] = *(ptrVALOR_VIAJE+0);             //
		}else{
			//valor en fichas
			if(EqPESOS_hab){
				RedondeoValorViaje_3();
				VALOR_VIAJE_mostrar = VALOR_VIAJE/10;
				Pase_a_OCUPADO_Buffer[9] = 2;
				Pase_a_OCUPADO_Buffer[10] = *(ptrVALOR_VIAJE_mostrar+3);             //
				Pase_a_OCUPADO_Buffer[11] = *(ptrVALOR_VIAJE_mostrar+2);             //
				Pase_a_OCUPADO_Buffer[12] = *(ptrVALOR_VIAJE_mostrar+1);             //
				Pase_a_OCUPADO_Buffer[13] = *(ptrVALOR_VIAJE_mostrar+0);             //

			}else{
				Pase_a_OCUPADO_Buffer[9] = PUNTO_DECIMAL;  				  //ficha o pesos
				Pase_a_OCUPADO_Buffer[10] = *(ptrVALOR_VIAJE+3);             //
				Pase_a_OCUPADO_Buffer[11] = *(ptrVALOR_VIAJE+2);             //
				Pase_a_OCUPADO_Buffer[12] = *(ptrVALOR_VIAJE+1);             //
				Pase_a_OCUPADO_Buffer[13] = *(ptrVALOR_VIAJE+0);             //
			}
		}

		Pase_a_OCUPADO_Buffer[14] = *(ptrDISTAMNCIAm+3);             //
 		Pase_a_OCUPADO_Buffer[15] = *(ptrDISTAMNCIAm+2);             //
 		Pase_a_OCUPADO_Buffer[16] = *(ptrDISTAMNCIAm+1);             //
 		Pase_a_OCUPADO_Buffer[17] = *(ptrDISTAMNCIAm+0);             //
 		Pase_a_OCUPADO_Buffer[18] = minutosEspera;  				  //minutos de espera

 		Pase_a_OCUPADO_Buffer[19] = *(ptrVELOCIDAD+1) ;            	//
 		Pase_a_OCUPADO_Buffer[20] = *(ptrVELOCIDAD+0);            		//

 		Pase_a_OCUPADO_Buffer[21] = (byte)ESTADO_RELOJ; 				// estado de reloj
 		Pase_a_OCUPADO_Buffer[22] = turnoChofer;						// estado d e turno
 		Pase_a_OCUPADO_Buffer[23] = nroChofer;  						// chofer quien inicio turno (si no inicio turno es cero)

 		Pase_a_OCUPADO_Buffer[24] = *(ptrBajadaBandera+1);
 		Pase_a_OCUPADO_Buffer[25] = *(ptrBajadaBandera+0);

 		Pase_a_OCUPADO_Buffer[26] = *(ptrFichasDistancia+3);             //
 		Pase_a_OCUPADO_Buffer[27] = *(ptrFichasDistancia+2);             //
 		Pase_a_OCUPADO_Buffer[28] = *(ptrFichasDistancia+1);             //
 		Pase_a_OCUPADO_Buffer[29] = *(ptrFichasDistancia+0);             //

 		Pase_a_OCUPADO_Buffer[30] = *(ptrFichasTiempo+3);             //
 		Pase_a_OCUPADO_Buffer[31] = *(ptrFichasTiempo+2);             //
 		Pase_a_OCUPADO_Buffer[32] = *(ptrFichasTiempo+1);             //
 		Pase_a_OCUPADO_Buffer[33] = *(ptrFichasTiempo+0);             //

		//Posicion ocupado
 		Pase_a_OCUPADO_Buffer[34] = sgnLatLon_OCUPADO_CEL;
		bufferNcopy(&Pase_a_OCUPADO_Buffer[35] ,(byte*)&latitudGPS_OCUPADO_CEL   ,3);
		bufferNcopy(&Pase_a_OCUPADO_Buffer[38] ,(byte*)&longitudGPS_OCUPADO_CEL   ,3);
		Pase_a_OCUPADO_Buffer[41] = velocidadGPS_OCUPADO_CEL;
		Pase_a_OCUPADO_Buffer[42] = nroCorrelativo_INTERNO;  					//numero correlativo
		Pase_a_OCUPADO_Buffer[43] = datosSC_cntWORD;


 		Pase_a_OCUPADO_Buffer[N_DATOS_Pase_a_OCUPADO]  = fin_datos_msb;  // Fin Datos
 		Pase_a_OCUPADO_Buffer[N_DATOS_Pase_a_OCUPADO+1] = fin_datos_lsb;// Fin Datos
    }


 void Tx_Pase_a_COBRANDO (uint8_t tipo){

  		byte* ptrVALOR_VIAJE;
  		byte* ptrVALOR_VIAJE_mostrar;
  		byte* ptrDISTAMNCIAm;
  		byte* ptrVELOCIDAD;

  		byte* ptrFichasDistancia;
  		byte* ptrFichasTiempo;
  		byte* ptrBajadaBandera;

  		uint32_t FichasDistancia;
  		uint32_t FichasTiempo;
  		uint16_t BajadaBandera;
  		uint8_t tarifa_mostrar;

		if(CON_CONEXION_CENTRAL){
	  	    CMD_Pase_a_COBRANDO.Tx_F = 1;                      // Levanto Bandera de Tx
	  		CMD_Pase_a_COBRANDO.Reintentos = reint_3;   // Cargo Cantidad de Reintentos (INFINITOS)
		}else{
	  	    CMD_Pase_a_COBRANDO_SC.Tx_F = 1;                      // Levanto Bandera de Tx
	  		CMD_Pase_a_COBRANDO_SC.Reintentos = reint_3;   // Cargo Cantidad de Reintentos (INFINITOS)
		}

  		ptrVALOR_VIAJE = &VALOR_VIAJE;
  		ptrVALOR_VIAJE_mostrar = &VALOR_VIAJE_mostrar;
  		ptrDISTAMNCIAm = &DISTANCIAm;
  		ptrVELOCIDAD   = &VELOCIDAD;

 		ptrBajadaBandera = &BajadaBandera;
 		ptrFichasDistancia = &FichasDistancia;
 		ptrFichasTiempo = &FichasTiempo;
 		//BajadaBandera = TARIFA.bajadaBandera;
 		FichasDistancia = TARIFACION_getFichasD();
  		FichasTiempo =	TARIFACION_getFichasT();

  		BajadaBandera = TARIFA.bajadaBandera;                             // Bajada Bandera

  		if(TARIFA.diaNoche == 0){
  					//DIURNA
  					if(paseOCUPADO_APP){
  						tarifa_mostrar = tarifa;
  					}else{
  						tarifa_mostrar = tarifa_1_4;
  					}
  				}else{
  					//NOCTURNA
  					if(paseOCUPADO_APP){
  						tarifa_mostrar = tarifa+0x80;;
  					}else{
  						tarifa_mostrar = tarifa_1_4+0x80;
  					}
  		}

  		getDate();

  		//fuente de hora
  		Pase_a_COBRANDO_Buffer[0] = HORA_source;                           // Hora GPS/RTC (0:GPS - 1:RTC)

  		Pase_a_COBRANDO_Buffer[1] = RTC_Date.hora[0];   // HORA
  		Pase_a_COBRANDO_Buffer[2] = RTC_Date.hora[1];   // MINUTOS
  		Pase_a_COBRANDO_Buffer[3] = RTC_Date.hora[2];   // SEGUNDOS
  		Pase_a_COBRANDO_Buffer[4] = RTC_Date.fecha[0];  // DIA
  		Pase_a_COBRANDO_Buffer[5] = RTC_Date.fecha[1];  // MES
  		Pase_a_COBRANDO_Buffer[6] = RTC_Date.fecha[2];  // AÑO

  		Pase_a_COBRANDO_Buffer[7]  = tarifa_mostrar;  			  //numero de tarifa
  		Pase_a_COBRANDO_Buffer[8]  = fichaPESOS;  				  //ficha o pesos

		if(TARIFA_PESOS){
		//valor en pesos
			Pase_a_COBRANDO_Buffer[9] = PUNTO_DECIMAL;  				  //ficha o pesos
			Pase_a_COBRANDO_Buffer[10] = *(ptrVALOR_VIAJE+3);             //
			Pase_a_COBRANDO_Buffer[11] = *(ptrVALOR_VIAJE+2);             //
			Pase_a_COBRANDO_Buffer[12] = *(ptrVALOR_VIAJE+1);             //
			Pase_a_COBRANDO_Buffer[13] = *(ptrVALOR_VIAJE+0);             //

		}else{
			//valor en fichas
			if(EqPESOS_hab){
				RedondeoValorViaje_3();
				VALOR_VIAJE_mostrar = VALOR_VIAJE/10;
				Pase_a_COBRANDO_Buffer[9] = 2;
				Pase_a_COBRANDO_Buffer[10] = *(ptrVALOR_VIAJE_mostrar+3);             //
				Pase_a_COBRANDO_Buffer[11] = *(ptrVALOR_VIAJE_mostrar+2);             //
				Pase_a_COBRANDO_Buffer[12] = *(ptrVALOR_VIAJE_mostrar+1);             //
				Pase_a_COBRANDO_Buffer[13] = *(ptrVALOR_VIAJE_mostrar+0);             //

			}else{
				Pase_a_COBRANDO_Buffer[9] = PUNTO_DECIMAL;  				  //ficha o pesos
				Pase_a_COBRANDO_Buffer[10] = *(ptrVALOR_VIAJE+3);             //
				Pase_a_COBRANDO_Buffer[11] = *(ptrVALOR_VIAJE+2);             //
				Pase_a_COBRANDO_Buffer[12] = *(ptrVALOR_VIAJE+1);             //
				Pase_a_COBRANDO_Buffer[13] = *(ptrVALOR_VIAJE+0);             //
			}
		}


  		Pase_a_COBRANDO_Buffer[14] = *(ptrDISTAMNCIAm+3);             //
  		Pase_a_COBRANDO_Buffer[15] = *(ptrDISTAMNCIAm+2);             //
  		Pase_a_COBRANDO_Buffer[16] = *(ptrDISTAMNCIAm+1);             //
  		Pase_a_COBRANDO_Buffer[17] = *(ptrDISTAMNCIAm+0);             //
  		Pase_a_COBRANDO_Buffer[18] = minutosEspera;  				  //minutos de espera

  		Pase_a_COBRANDO_Buffer[19] = *(ptrVELOCIDAD+1) ;            	//
  		Pase_a_COBRANDO_Buffer[20] = *(ptrVELOCIDAD+0);            		//

  		Pase_a_COBRANDO_Buffer[21] = (byte)ESTADO_RELOJ; 				// estado de reloj
  		Pase_a_COBRANDO_Buffer[22] = turnoChofer;						// estado d e turno
  		Pase_a_COBRANDO_Buffer[23] = nroChofer;  						// chofer quien inicio turno (si no inicio turno es cero)

  		Pase_a_COBRANDO_Buffer[24] = *(ptrBajadaBandera+1);
  		Pase_a_COBRANDO_Buffer[25] = *(ptrBajadaBandera+0);

  		Pase_a_COBRANDO_Buffer[26] = *(ptrFichasDistancia+3);             //
  		Pase_a_COBRANDO_Buffer[27] = *(ptrFichasDistancia+2);             //
  		Pase_a_COBRANDO_Buffer[28] = *(ptrFichasDistancia+1);            	 //
  		Pase_a_COBRANDO_Buffer[29] = *(ptrFichasDistancia+0);             	//

  		Pase_a_COBRANDO_Buffer[30] = *(ptrFichasTiempo+3);             		//
  		Pase_a_COBRANDO_Buffer[31] = *(ptrFichasTiempo+2);             		//
  		Pase_a_COBRANDO_Buffer[32] = *(ptrFichasTiempo+1);             		//
  		Pase_a_COBRANDO_Buffer[33] = *(ptrFichasTiempo+0);             		//

		//Posicion cobrando
  		Pase_a_COBRANDO_Buffer[34] = sgnLatLon_COBRANDO_CEL;
		bufferNcopy(&Pase_a_COBRANDO_Buffer[35] ,(byte*)&latitudGPS_COBRANDO_CEL  ,3);
		bufferNcopy(&Pase_a_COBRANDO_Buffer[38] ,(byte*)&longitudGPS_COBRANDO_CEL  ,3);
		Pase_a_COBRANDO_Buffer[41] = velocidadGPS_COBRANDO_CEL;
		Pase_a_COBRANDO_Buffer[42] = nroCorrelativo_INTERNO;  					//numero correlativo
		Pase_a_COBRANDO_Buffer[43] = datosSC_cntWORD;

  		Pase_a_COBRANDO_Buffer[N_DATOS_Pase_a_COBRANDO]  = fin_datos_msb;  // Fin Datos
  		Pase_a_COBRANDO_Buffer[N_DATOS_Pase_a_COBRANDO+1] = fin_datos_lsb;// Fin Datos
     }

//manda dato de ficha o pesos con el valor 0x55
//para que la central sepa que es un comando trucho
 void Tx_Pase_a_COBRANDO_trucho (uint8_t tipo){

   		byte* ptrVALOR_VIAJE;
   		byte* ptrVALOR_VIAJE_mostrar;
   		byte* ptrDISTAMNCIAm;
   		byte* ptrVELOCIDAD;

   		byte* ptrFichasDistancia;
   		byte* ptrFichasTiempo;
   		byte* ptrBajadaBandera;

   		uint32_t FichasDistancia;
   		uint32_t FichasTiempo;
   		uint16_t BajadaBandera;
   		uint8_t tarifa_mostrar;

		CMD_Pase_a_COBRANDO.Tx_F = 1;                      // Levanto Bandera de Tx
		CMD_Pase_a_COBRANDO.Reintentos = reint_3;   // Cargo Cantidad de Reintentos (INFINITOS)

   		ptrVALOR_VIAJE = &VALOR_VIAJE;
   		ptrVALOR_VIAJE_mostrar = &VALOR_VIAJE_mostrar;
   		ptrDISTAMNCIAm = &DISTANCIAm;
   		ptrVELOCIDAD   = &VELOCIDAD;

  		ptrBajadaBandera = &BajadaBandera;
  		ptrFichasDistancia = &FichasDistancia;
  		ptrFichasTiempo = &FichasTiempo;
  		//BajadaBandera = TARIFA.bajadaBandera;
  		FichasDistancia = TARIFACION_getFichasD();
   		FichasTiempo =	TARIFACION_getFichasT();

   		BajadaBandera = TARIFA.bajadaBandera;                             // Bajada Bandera

   		if(TARIFA.diaNoche == 0){
   					//DIURNA
   					if(paseOCUPADO_APP){
   						tarifa_mostrar = tarifa;
   					}else{
   						tarifa_mostrar = tarifa_1_4;
   					}
   				}else{
   					//NOCTURNA
   					if(paseOCUPADO_APP){
   						tarifa_mostrar = tarifa+0x80;;
   					}else{
   						tarifa_mostrar = tarifa_1_4+0x80;
   					}
   		}

   		getDate();

   		//fuente de hora
   		Pase_a_COBRANDO_Buffer[0] = HORA_source;                           // Hora GPS/RTC (0:GPS - 1:RTC)

   		Pase_a_COBRANDO_Buffer[1] = RTC_Date.hora[0];   // HORA
   		Pase_a_COBRANDO_Buffer[2] = RTC_Date.hora[1];   // MINUTOS
   		Pase_a_COBRANDO_Buffer[3] = RTC_Date.hora[2];   // SEGUNDOS
   		Pase_a_COBRANDO_Buffer[4] = RTC_Date.fecha[0];  // DIA
   		Pase_a_COBRANDO_Buffer[5] = RTC_Date.fecha[1];  // MES
   		Pase_a_COBRANDO_Buffer[6] = RTC_Date.fecha[2];  // AÑO

   		Pase_a_COBRANDO_Buffer[7]  = tarifa_mostrar;  			  //numero de tarifa
   		Pase_a_COBRANDO_Buffer[8]  = 0x55;  				  //ficha o pesos

 		if(TARIFA_PESOS){
 		//valor en pesos
 			Pase_a_COBRANDO_Buffer[9] = PUNTO_DECIMAL;  				  //ficha o pesos
 			Pase_a_COBRANDO_Buffer[10] = *(ptrVALOR_VIAJE+3);             //
 			Pase_a_COBRANDO_Buffer[11] = *(ptrVALOR_VIAJE+2);             //
 			Pase_a_COBRANDO_Buffer[12] = *(ptrVALOR_VIAJE+1);             //
 			Pase_a_COBRANDO_Buffer[13] = *(ptrVALOR_VIAJE+0);             //

 		}else{
 			//valor en fichas
 			if(EqPESOS_hab){
 				RedondeoValorViaje_3();
 				VALOR_VIAJE_mostrar = VALOR_VIAJE/10;
 				Pase_a_COBRANDO_Buffer[9] = 2;
 				Pase_a_COBRANDO_Buffer[10] = *(ptrVALOR_VIAJE_mostrar+3);             //
 				Pase_a_COBRANDO_Buffer[11] = *(ptrVALOR_VIAJE_mostrar+2);             //
 				Pase_a_COBRANDO_Buffer[12] = *(ptrVALOR_VIAJE_mostrar+1);             //
 				Pase_a_COBRANDO_Buffer[13] = *(ptrVALOR_VIAJE_mostrar+0);             //

 			}else{
 				Pase_a_COBRANDO_Buffer[9] = PUNTO_DECIMAL;  				  //ficha o pesos
 				Pase_a_COBRANDO_Buffer[10] = *(ptrVALOR_VIAJE+3);             //
 				Pase_a_COBRANDO_Buffer[11] = *(ptrVALOR_VIAJE+2);             //
 				Pase_a_COBRANDO_Buffer[12] = *(ptrVALOR_VIAJE+1);             //
 				Pase_a_COBRANDO_Buffer[13] = *(ptrVALOR_VIAJE+0);             //
 			}
 		}


   		Pase_a_COBRANDO_Buffer[14] = *(ptrDISTAMNCIAm+3);             //
   		Pase_a_COBRANDO_Buffer[15] = *(ptrDISTAMNCIAm+2);             //
   		Pase_a_COBRANDO_Buffer[16] = *(ptrDISTAMNCIAm+1);             //
   		Pase_a_COBRANDO_Buffer[17] = *(ptrDISTAMNCIAm+0);             //
   		Pase_a_COBRANDO_Buffer[18] = minutosEspera;  				  //minutos de espera

   		Pase_a_COBRANDO_Buffer[19] = *(ptrVELOCIDAD+1) ;            	//
   		Pase_a_COBRANDO_Buffer[20] = *(ptrVELOCIDAD+0);            		//

   		Pase_a_COBRANDO_Buffer[21] = (byte)ESTADO_RELOJ; 				// estado de reloj
   		Pase_a_COBRANDO_Buffer[22] = turnoChofer;						// estado d e turno
   		Pase_a_COBRANDO_Buffer[23] = nroChofer;  						// chofer quien inicio turno (si no inicio turno es cero)

   		Pase_a_COBRANDO_Buffer[24] = *(ptrBajadaBandera+1);
   		Pase_a_COBRANDO_Buffer[25] = *(ptrBajadaBandera+0);

   		Pase_a_COBRANDO_Buffer[26] = *(ptrFichasDistancia+3);             //
   		Pase_a_COBRANDO_Buffer[27] = *(ptrFichasDistancia+2);             //
   		Pase_a_COBRANDO_Buffer[28] = *(ptrFichasDistancia+1);            	 //
   		Pase_a_COBRANDO_Buffer[29] = *(ptrFichasDistancia+0);             	//

   		Pase_a_COBRANDO_Buffer[30] = *(ptrFichasTiempo+3);             		//
   		Pase_a_COBRANDO_Buffer[31] = *(ptrFichasTiempo+2);             		//
   		Pase_a_COBRANDO_Buffer[32] = *(ptrFichasTiempo+1);             		//
   		Pase_a_COBRANDO_Buffer[33] = *(ptrFichasTiempo+0);             		//

 		//Posicion cobrando
   		Pase_a_COBRANDO_Buffer[34] = sgnLatLon_COBRANDO_CEL;
 		bufferNcopy(&Pase_a_COBRANDO_Buffer[35] ,(byte*)&latitudGPS_COBRANDO_CEL  ,3);
 		bufferNcopy(&Pase_a_COBRANDO_Buffer[38] ,(byte*)&longitudGPS_COBRANDO_CEL  ,3);
 		Pase_a_COBRANDO_Buffer[41] = velocidadGPS_COBRANDO_CEL;
 		Pase_a_COBRANDO_Buffer[42] = nroCorrelativo_INTERNO;  					//numero correlativo
 		Pase_a_COBRANDO_Buffer[43] = datosSC_cntWORD;

   		Pase_a_COBRANDO_Buffer[N_DATOS_Pase_a_COBRANDO]  = fin_datos_msb;  // Fin Datos
   		Pase_a_COBRANDO_Buffer[N_DATOS_Pase_a_COBRANDO+1] = fin_datos_lsb;// Fin Datos
      }

 void Tx_Pase_a_FUERA_SERVICIO (void){

   	    CMD_Pase_a_FUERA_SERVICIO.Tx_F = 1;                      // Levanto Bandera de Tx
   		CMD_Pase_a_FUERA_SERVICIO.Reintentos = reint_3;   // Cargo Cantidad de Reintentos (INFINITOS)

   		getDate();

  		//fuente de hora
   		Pase_a_FUERA_SERVICIO_Buffer[0] = HORA_source;                           // Hora GPS/RTC (0:GPS - 1:RTC)

   		Pase_a_FUERA_SERVICIO_Buffer[1] = RTC_Date.hora[0];   // HORA
   		Pase_a_FUERA_SERVICIO_Buffer[2] = RTC_Date.hora[1];   // MINUTOS
   		Pase_a_FUERA_SERVICIO_Buffer[3] = RTC_Date.hora[2];   // SEGUNDOS
   		Pase_a_FUERA_SERVICIO_Buffer[4] = RTC_Date.fecha[0];  // DIA
   		Pase_a_FUERA_SERVICIO_Buffer[5] = RTC_Date.fecha[1];  // MES
   		Pase_a_FUERA_SERVICIO_Buffer[6] = RTC_Date.fecha[2];  // AÑO
   		Pase_a_FUERA_SERVICIO_Buffer[7] = datosSC_cntWORD;


   		Pase_a_FUERA_SERVICIO_Buffer[N_DATOS_Pase_a_FUERA_SERVICIO]  = fin_datos_msb;  // Fin Datos
   		Pase_a_FUERA_SERVICIO_Buffer[N_DATOS_Pase_a_FUERA_SERVICIO+1] = fin_datos_lsb;// Fin Datos
      }



 void TARIFA_TimeOut (void){
      // Ejecutada cada 1mseg
      if (tarifa_to_cnt != 0){
    	  tarifa_to_cnt--;
        if (tarifa_to_cnt == 0){
        	tarifa = tarifa_1_4;
        	paseOCUPADO_BOTON = 1;
			Pase_a_OCUPADO(CON_CONEXION_CENTRAL);
			Buzzer_On(BEEP_TECLA_CORTA);
        }
      }
 }

 void RELOJ_clearMOTIVO_paseOCUPADO(void){
	 paseOCUPADO_BOTON=0;
	 paseOCUPADO_SENSOR_ASIENTO=0;
	 paseOCUPADO_BANDERITA=0;
	 paseOCUPADO_APP=0;
	 paseOCUPADO_PACTADO=0;
 }

 /* PASE A LIBRE INTERNO - desde FUERA DE SERVICIO */
  /**************************************************/
   void PaseLibreFromFServ_INTERNO (void){

	    ESTADO_RELOJ=LIBRE;
		if(VELOCIDAD_MAX < 255){
			velMax_INTERNO = (uint8_t)VELOCIDAD_MAX;
		}else{
			velMax_INTERNO = (uint8_t)255;
		}
		if(paseLIBRE_SENSOR_ASIENTO = 1){
			//llamo a esta para que no recargue pulsos_cnt_old
			//y calcule mal los km acumulados en libre cuando pasa de
			//fserv a libre y luego a ocupado cuando el cambio es por sensor de asiento
			paseLIBRE_SENSOR_ASIENTO = 0;
			calcularDISTANCIA_acumulada;
		}else{
			kmRecorridos_INTERNO = calcularDISTANCIA_entreEstados;	// Distancia Recorrida en LIBRE (KM xxx.x)
		}


	   //RELOJ_setTarifa(TARIFA_AUTO_getNroTarifa());            // Seteo automatico de tarifa

	#ifdef VISOR_REPORTES
	   if (REPORTES_HABILITADOS){

      // Guardo reporte -> datos de FUERA DE SERVICIO
          (void)REPORTE_queueFueraServ (RELOJ_getDateCambio(), RELOJ_INTERNO_getChofer(),kmRecorridos_INTERNO, velMax_INTERNO, timerParado_cnt, timerMarcha_cnt, CON_CONEXION_CENTRAL);
          // EDIT 04/04/2013
          //  Para permitir la correcta recepcion de la respuesta, demoro la grabacion de FLASH, para
          // no inhibir las IRQs
        //  REPORTES_delayFLASH_update();                       // Demoro grabacion de REPORTES en FLASH
        }
    #endif

      paseLIBRE_interno_conKM;                                // Paso a LIBRE desde FSERV => cuento KM
    }

   /* PASE A LIBRE */
   /*--------------*/
       void paseLIBRE (byte* KM_ptr){

         ESTADO_RELOJ = LIBRE;                           // Actualizo Estado de Reloj
         if (!RELOJ_fromAPAGAR){
           // Si no venia de A PAGAR => Venia de Fuera de Servicio, por tanto encolo
           // el pase a LIBRE
          // Libre_encolarTx();                            // Encolo Comando de Reloj
         }

        //   BANDERA_encender;                             // Encender Bandera

       //  RELOJ_fromAPAGAR = 0;                           // Bajo Bandera
         resetVELOCIDAD_MAX;                             // Reseteo Velocidad Maxima

         #ifdef VISOR_REPORTE_ACUMULATIVO
           if (KM_ptr != NULL){
             // Al pasar a LIBRE, solo cuento los km de cuando paso FSERV -> LIBRE
             km = *KM_ptr++;                               // KM en FUERA SERV (MSB)
             km <<= 8;                                     // KM en FUERA SERV (LSB)
             km |= *KM_ptr++;
             RELOJ_REPACUM_addKmFueraServ(km);             // Agrego km Fuera Servicio a Reporte Acumulativo
           }
         #endif
       }



       static   byte* getLATITUD(byte estado){
    	   if(estado == LIB){
    		   return((byte*)&latitudGPS_LIBRE_CEL);
    	   }else if(estado == OCUP){
    		   return((byte*)&latitudGPS_OCUPADO_CEL);
    	   }else if(estado == COBR){
    		   return((byte*)&latitudGPS_COBRANDO_CEL);
    	   }
       }

       static  byte* getLONGITUD(byte estado){

    	   if(estado == LIB){
    		   return((byte*)&longitudGPS_LIBRE_CEL);
    	   }else if(estado == OCUP){
    		   return((byte*)&longitudGPS_OCUPADO_CEL);
    	   }else if(estado == COBR){
    		   return((byte*)&longitudGPS_COBRANDO_CEL);
    	   }
       }


       static byte getSgnLatLon(byte estado){
    	   if(estado == LIB){
    		   return(sgnLatLon_LIBRE_CEL);
    	   }else if(estado == OCUP){
    		   return(sgnLatLon_OCUPADO_CEL);
    	   }else if(estado == COBR){
    		   return(sgnLatLon_COBRANDO_CEL);
    	   }


       }


       static byte  getVELOCIDAD(byte estado){

    	   if(estado == LIB){
    		   return(velocidadGPS_LIBRE_CEL);
    	   }else if(estado == OCUP){
    		   return(velocidadGPS_OCUPADO_CEL);
    	   }else if(estado == COBR){
    		   return(velocidadGPS_COBRANDO_CEL);
    	   }
       }




     void  SaveDatGps(byte* data_ptr, byte estado){
    	word vel;

    	byte i=0;

		vel = BCDtoHEX_2BYTES(&data_ptr[9]);

		if(estado == LIB){
			 sgnLatLon_LIBRE_CEL = *data_ptr++;
			 while(i<3){
				 latitudGPS_LIBRE_CEL[i]  = data_ptr[i];
				 longitudGPS_LIBRE_CEL[i] = data_ptr[i+4];
				 i++;
			 }
			 if(vel < 255){
				 velocidadGPS_LIBRE_CEL = vel;
			 }else{
				 velocidadGPS_LIBRE_CEL = 255;
			 }
		}else if(estado == OCUP){
			 sgnLatLon_OCUPADO_CEL = *data_ptr++;
			 while(i<3){
				 latitudGPS_OCUPADO_CEL[i]  = data_ptr[i];
				 longitudGPS_OCUPADO_CEL[i] = data_ptr[i+4];
				 i++;
			 }
			 if(vel < 255){
				 velocidadGPS_OCUPADO_CEL = vel;
			 }else{
				 velocidadGPS_OCUPADO_CEL = 255;
			 }
		}else if(estado == COBR){
			 sgnLatLon_COBRANDO_CEL = *data_ptr++;
			 while(i<3){
				 latitudGPS_COBRANDO_CEL[i]  = data_ptr[i];
				 longitudGPS_COBRANDO_CEL[i] = data_ptr[i+4];
				 i++;
			 }
			 if(vel < 255){
				 velocidadGPS_COBRANDO_CEL = vel;
			 }else{
				 velocidadGPS_COBRANDO_CEL = 255;
			 }
		}
     }



     void  ResetDatGps(byte estado){
       	word vel;

       	byte i=0;

   		vel = 0;
   		if(estado == LIB){
   			 sgnLatLon_LIBRE_CEL = 0;
   			 while(i<3){
   				 latitudGPS_LIBRE_CEL[i]  = 0;
   				 longitudGPS_LIBRE_CEL[i] = 0;
   				 i++;
   			 }
   		}else if(estado == OCUP){
   			 sgnLatLon_OCUPADO_CEL = 0;
   			 while(i<3){
   				 latitudGPS_OCUPADO_CEL[i]  = 0;
   				 longitudGPS_OCUPADO_CEL[i] = 0;
   				 i++;
   			 }
   		}else if(estado == COBR){
   			 sgnLatLon_COBRANDO_CEL = 0;
   			 while(i<3){
   				 latitudGPS_COBRANDO_CEL[i]  = 0;
   				 longitudGPS_COBRANDO_CEL[i] = 0;
   				 i++;
   			 }
   		}
 }


     void  getDatGps_COBRANDO(byte* data_ptr){
    	 word vel;
    	 sgnLatLon_COBRANDO = *data_ptr++;
    	 byte i=0;
    	 while(i<3){
    		 latitudGPS_COBRANDO_CEL[i]  = data_ptr[i];
    		 longitudGPS_COBRANDO_CEL[i] = data_ptr[i+4];
    		 i++;
    	 }
    	 vel = BCDtoHEX_2BYTES(&data_ptr[i+5]);
    	 if(vel < 255){
    		 velocidadGPS_COBRANDO_CEL = vel;
    	 }else{
    		 velocidadGPS_COBRANDO_CEL = 255;
    	 }
     }

/************************************************
 *  	Rearmar comandos sin conexion  		*****
 ************************************************/


void rearmar_y_TX_cmdLibre(byte nroViaje){

     	tREG_LIBRE*		ptrREG_LIBRE;
     	tREG_OCUPADO*	ptrREG_OCUPADO;
     	tREG_A_PAGAR*	ptrREG_APAGAR;
     	uint32_t		DISTANCIAm_LIB;
     	uint32_t		DISTANCIAm_OCUP;
    	byte* 			ptr_mRecorridos_LIBRE;
   		byte*			ptr_mRecorridos_OCUPADO;

		byte* ptr_timerMarcha_cnt_LIBRE;
		byte* ptr_timerMarcha_cnt_OCUPADO;
		byte* ptr_timerParado_cnt_LIBRE;
		byte* ptr_timerParado_cnt_OCUPADO;
 		byte* ptrVALOR_VIAJE;
 		byte* ptrVALOR_VIAJE_mostrar;

     	//convierto km con un decimal a metros
     	DISTANCIAm_LIB = (uint32_t)regVIAJE.kmLIBRE*100;
     	DISTANCIAm_OCUP = (uint32_t)regVIAJE.kmOCUPADO*100;
   		ptr_mRecorridos_LIBRE  = (byte*)&DISTANCIAm_LIB;
   		ptr_mRecorridos_OCUPADO = (byte*)&DISTANCIAm_OCUP;


		ptr_timerMarcha_cnt_LIBRE   = (byte*)&regVIAJE.segMarchaLIBRE;
		ptr_timerMarcha_cnt_OCUPADO = (byte*)&regVIAJE.segMarchaOCUPADO;
		ptr_timerParado_cnt_LIBRE 	= (byte*)&regVIAJE.segParadoLIBRE;
		ptr_timerParado_cnt_OCUPADO = (byte*)&regVIAJE.segParadoOCUPADO;

 		ptrVALOR_VIAJE = (byte*)&VALOR_VIAJE;
 		ptrVALOR_VIAJE_mostrar = (byte*)&VALOR_VIAJE_mostrar;

		//BANDERA DE TRANSMISION
		CMD_Pase_a_LIBRE.Tx_F = 1;                      				// Levanto Bandera de Tx
		CMD_Pase_a_LIBRE.Reintentos = reint_3;   				    // Cargo Cantidad de Reintentos (INFINITOS)

		//fuente de hora
		Pase_a_LIBRE_Buffer[0] = HORA_source;                           // Hora GPS/RTC (0:GPS - 1:RTC)

		//DATE
		Pase_a_LIBRE_Buffer[1] = regVIAJE.dateLIBRE.hora[0];   					// HORA
		Pase_a_LIBRE_Buffer[2] = regVIAJE.dateLIBRE.hora[1];   					// MINUTOS
		Pase_a_LIBRE_Buffer[3] = regVIAJE.dateLIBRE.hora[2];   					// SEGUNDOS
		Pase_a_LIBRE_Buffer[4] = regVIAJE.dateLIBRE.fecha[0];  					// DIA
		Pase_a_LIBRE_Buffer[5] = regVIAJE.dateLIBRE.fecha[1];  					// MES
		Pase_a_LIBRE_Buffer[6] = regVIAJE.dateLIBRE.fecha[2];  					// AÑO

		//LIBRE
		Pase_a_LIBRE_Buffer[7]  = *(ptr_mRecorridos_LIBRE+2);  		//m recorrido en  LIBRE parte baja
		Pase_a_LIBRE_Buffer[8]  = *(ptr_mRecorridos_LIBRE+1);  		//m recorrido en LIBRE parte alta
		Pase_a_LIBRE_Buffer[9]  = *(ptr_mRecorridos_LIBRE+0);  		//m recorrido en LIBRE parte alta

		Pase_a_LIBRE_Buffer[10] = regVIAJE.velMaxLIBRE;  					//velocidad máxima en LIBRE
		Pase_a_LIBRE_Buffer[11] = *(ptr_timerMarcha_cnt_LIBRE+1);  //tiempo de marcha parte alta
		Pase_a_LIBRE_Buffer[12] = *(ptr_timerMarcha_cnt_LIBRE+0);  	//tiempo de marcha parte baja en LIBRE
		Pase_a_LIBRE_Buffer[13] = *(ptr_timerParado_cnt_LIBRE+1);  	//tiempo parado parte alta en LIBRE
		Pase_a_LIBRE_Buffer[14] = *(ptr_timerParado_cnt_LIBRE+0);  	//tiempo parado parte baja en LIBRE
		//OCUPADO
		Pase_a_LIBRE_Buffer[15]  = *(ptr_mRecorridos_OCUPADO+2);  	//m recorrido en OCUPADO parte baja
		Pase_a_LIBRE_Buffer[16]  = *(ptr_mRecorridos_OCUPADO+1);  	//m recorrido en OCUPADO parte alta
		Pase_a_LIBRE_Buffer[17]  = *(ptr_mRecorridos_OCUPADO+0);  	//m recorrido en OCUPADO parte alta

		Pase_a_LIBRE_Buffer[18]  = regVIAJE.velMaxOCUPADO;;  				//velocidad máxima en OCUPADO parte alta
		Pase_a_LIBRE_Buffer[19]  = *(ptr_timerMarcha_cnt_OCUPADO+1);  		//tiempo marcha en OCUPADO parte alta
		Pase_a_LIBRE_Buffer[20]  = *(ptr_timerMarcha_cnt_OCUPADO+0); //tiempo marcha en OCUPADO parte baja
		Pase_a_LIBRE_Buffer[21]  = *(ptr_timerParado_cnt_OCUPADO+1);  		//tiempo parado en OCUPADO parte alta
		Pase_a_LIBRE_Buffer[22]  = *(ptr_timerParado_cnt_OCUPADO+0);  	//tiempo parado en OCUPADO parte baja


		Pase_a_LIBRE_Buffer[23]  = regVIAJE.minutosEspera;  			//minutos de espera
		Pase_a_LIBRE_Buffer[24]  = regVIAJE.tarifa;  					//numero de tarifa


		VALOR_VIAJE = regVIAJE.importe;
		if(TARIFA_PESOS){
			//valor en pesos
			Pase_a_LIBRE_Buffer[25] = *(ptrVALOR_VIAJE+3);             //
			Pase_a_LIBRE_Buffer[26] = *(ptrVALOR_VIAJE+2);             //
			Pase_a_LIBRE_Buffer[27] = *(ptrVALOR_VIAJE+1);             //
			Pase_a_LIBRE_Buffer[28] = *(ptrVALOR_VIAJE+0);             //
		}else{
			//valor en fichas
			if(EqPESOS_hab){
				RedondeoValorViaje_3();
				VALOR_VIAJE_mostrar = VALOR_VIAJE/10;
				Pase_a_LIBRE_Buffer[25] = *(ptrVALOR_VIAJE_mostrar+3);             //
				Pase_a_LIBRE_Buffer[26] = *(ptrVALOR_VIAJE_mostrar+2);             //
				Pase_a_LIBRE_Buffer[27] = *(ptrVALOR_VIAJE_mostrar+1);             //
				Pase_a_LIBRE_Buffer[28] = *(ptrVALOR_VIAJE_mostrar+0);             //

			}else{
				Pase_a_LIBRE_Buffer[25] = *(ptrVALOR_VIAJE+3);             //
				Pase_a_LIBRE_Buffer[26] = *(ptrVALOR_VIAJE+2);             //
				Pase_a_LIBRE_Buffer[27] = *(ptrVALOR_VIAJE+1);             //
				Pase_a_LIBRE_Buffer[28] = *(ptrVALOR_VIAJE+0);             //
			}
		}

		Pase_a_LIBRE_Buffer[29]  = regVIAJE.puntoDecimal;  						//opcional1
		Pase_a_LIBRE_Buffer[30]  = regVIAJE.chofer;  			//numero de chofer
		Pase_a_LIBRE_Buffer[31]  = nroViaje;  					//numero correlativo

		//Posicion libre
		Pase_a_LIBRE_Buffer[32] = regVIAJE.sgnLatLonLibre;
		Pase_a_LIBRE_Buffer[33] = regVIAJE.latitudLIBRE[0];
		Pase_a_LIBRE_Buffer[34] = regVIAJE.latitudLIBRE[1];
		Pase_a_LIBRE_Buffer[35] = regVIAJE.latitudLIBRE[2];
		Pase_a_LIBRE_Buffer[36] = regVIAJE.longitudLIBRE[0];
		Pase_a_LIBRE_Buffer[37] = regVIAJE.longitudLIBRE[1];
		Pase_a_LIBRE_Buffer[38] = regVIAJE.longitudLIBRE[2];
		Pase_a_LIBRE_Buffer[39] = regVIAJE.velgpsLIBRE;
		Pase_a_LIBRE_Buffer[40] = datosSC_cntWORD;

		//FIN
		Pase_a_LIBRE_Buffer[N_DATOS_Pase_a_LIBRE] = fin_datos_msb;  	// Fin Datos
		Pase_a_LIBRE_Buffer[N_DATOS_Pase_a_LIBRE+1] = fin_datos_lsb;	// Fin Datos
     }

void rearmar_y_TX_cmdOcupado(byte nroViaje, byte status){

		byte* ptrVALOR_VIAJE;
		byte* ptrVALOR_VIAJE_mostrar;

		//uint16_t BajadaBandera;
		uint8_t tarifa_mostrar;
		tTARIFA tarif;

 		byte* ptrFichasDistancia;
 		byte* ptrFichasTiempo;
 		byte* ptrBajadaBandera;

 		uint32_t FichasDistancia;
 		uint32_t FichasTiempo;
 		uint16_t BajadaBandera;
	    byte puntoDECIMAL, numeroCHOFER, fPESOS;

		//tomo la tarifa para tener la bajada de bandera y poder reconstruir el valor de viaje inicial
		ptrBajadaBandera = (byte*)&BajadaBandera;
		tarif = prgRELOJ_getTarifa(regVIAJE.tarifa);
		BajadaBandera = tarif.bajadaBandera;                             // Bajada Bandera
		ptrFichasDistancia = (byte*)&regVIAJE.fichasDist;
		ptrFichasTiempo = (byte*)&regVIAJE.fichasTime;
		puntoDECIMAL = regVIAJE.puntoDecimal;;
		numeroCHOFER = regVIAJE.chofer;  	//numero de chofer
		fPESOS  = regVIAJE.fichaPesos;		//ficha o pesos

		CMD_Pase_a_OCUPADO_APP.Tx_F = 1;                     // Levanto Bandera de Tx
		CMD_Pase_a_OCUPADO_APP.Reintentos = reint_3;   		// Cargo Cantidad de Reintentos (INFINITOS)

		ptrVALOR_VIAJE = (byte*)&VALOR_VIAJE;
		ptrVALOR_VIAJE_mostrar = (byte*)&VALOR_VIAJE_mostrar;

		if(tarif.diaNoche == 0){
			//DIURNA
			tarifa_mostrar = tarif.numero;
		}else{
			//NOCTURNA
			tarifa_mostrar = tarif.numero+0x80;;
		}
		//fuente de hora
		Pase_a_OCUPADO_Buffer[0] = HORA_source;                           // Hora GPS/RTC (0:GPS - 1:RTC)

		//DATE
		Pase_a_OCUPADO_Buffer[1] = regVIAJE.dateOCUPADO.hora[0];   					// HORA
		Pase_a_OCUPADO_Buffer[2] = regVIAJE.dateOCUPADO.hora[1];   					// MINUTOS
		Pase_a_OCUPADO_Buffer[3] = regVIAJE.dateOCUPADO.hora[2];   					// SEGUNDOS
		Pase_a_OCUPADO_Buffer[4] = regVIAJE.dateOCUPADO.fecha[0];  					// DIA
		Pase_a_OCUPADO_Buffer[5] = regVIAJE.dateOCUPADO.fecha[1];  					// MES
		Pase_a_OCUPADO_Buffer[6] = regVIAJE.dateOCUPADO.fecha[2];  					// AÑO


		Pase_a_OCUPADO_Buffer[7]  = tarifa_mostrar;  	   		  //numero de tarifa
		Pase_a_OCUPADO_Buffer[8]  = fPESOS;		  //ficha o pesos

		if(TARIFA_PESOS){
			VALOR_VIAJE  = tarif.bajadaBandera;
			//valor en pesos
			Pase_a_OCUPADO_Buffer[9] = puntoDECIMAL;

			Pase_a_OCUPADO_Buffer[10] = *(ptrVALOR_VIAJE+3);             //
			Pase_a_OCUPADO_Buffer[11] = *(ptrVALOR_VIAJE+2);             //
			Pase_a_OCUPADO_Buffer[12] = *(ptrVALOR_VIAJE+1);             //
			Pase_a_OCUPADO_Buffer[13] = *(ptrVALOR_VIAJE+0);             //
		}else{
			//valor en fichas
			if(EqPESOS_hab){
				//valor en fichas
				if(tarif.diaNoche==0){
					//tarifa diurna
					VALOR_VIAJE  = EqPESOS_DIURNA.bajadaBandera;                             // Bajada Bandera
				}else{
					//tarifa Nocturna
					VALOR_VIAJE  = EqPESOS_NOCTURNA.bajadaBandera;                             // Bajada Bandera
				}

				RedondeoValorViaje_3();
				VALOR_VIAJE_mostrar = VALOR_VIAJE/10;
				Pase_a_OCUPADO_Buffer[9] = 2;
				Pase_a_OCUPADO_Buffer[10] = *(ptrVALOR_VIAJE_mostrar+3);             //
				Pase_a_OCUPADO_Buffer[11] = *(ptrVALOR_VIAJE_mostrar+2);             //
				Pase_a_OCUPADO_Buffer[12] = *(ptrVALOR_VIAJE_mostrar+1);             //
				Pase_a_OCUPADO_Buffer[13] = *(ptrVALOR_VIAJE_mostrar+0);             //

			}else{

				VALOR_VIAJE  = tarif.bajadaBandera * TARIFA.valorFicha;

				Pase_a_OCUPADO_Buffer[9] = puntoDECIMAL;  				  //ficha o pesos
				Pase_a_OCUPADO_Buffer[10] = *(ptrVALOR_VIAJE+3);             //
				Pase_a_OCUPADO_Buffer[11] = *(ptrVALOR_VIAJE+2);             //
				Pase_a_OCUPADO_Buffer[12] = *(ptrVALOR_VIAJE+1);             //
				Pase_a_OCUPADO_Buffer[13] = *(ptrVALOR_VIAJE+0);             //
			}
		}

		Pase_a_OCUPADO_Buffer[14] = 0;             					//distancia en metros
		Pase_a_OCUPADO_Buffer[15] = 0;             					//distancia en metros
		Pase_a_OCUPADO_Buffer[16] = 0;             					//distancia en metros
		Pase_a_OCUPADO_Buffer[17] = 0;             					//distancia en metros
		Pase_a_OCUPADO_Buffer[18] = 0;			  				  	//minutos de espera

		Pase_a_OCUPADO_Buffer[19] = 0;				            	//velocidad
		Pase_a_OCUPADO_Buffer[20] = 0;            					//velocidad

		Pase_a_OCUPADO_Buffer[21] = (byte)ESTADO_RELOJ; 			// estado de reloj
		Pase_a_OCUPADO_Buffer[22] = turnoChofer;					// estado d e turno
		Pase_a_OCUPADO_Buffer[23] = regVIAJE.chofer;  				//numero de chofer

		Pase_a_OCUPADO_Buffer[24] = *(ptrBajadaBandera+1);
		Pase_a_OCUPADO_Buffer[25] = *(ptrBajadaBandera+0);

		if(status == 0){
	 		Pase_a_OCUPADO_Buffer[26] = 0;             					//MSB
	 		Pase_a_OCUPADO_Buffer[27] = *(ptrFichasDistancia+0);        //
	 		Pase_a_OCUPADO_Buffer[28] = *(ptrFichasDistancia+1);        //
	 		Pase_a_OCUPADO_Buffer[29] = *(ptrFichasDistancia+2);        //LSB

	 		Pase_a_OCUPADO_Buffer[30] = 0;           //MSB
	 		Pase_a_OCUPADO_Buffer[31] = *(ptrFichasTiempo+0);           //
	 		Pase_a_OCUPADO_Buffer[32] = *(ptrFichasTiempo+1);           //
	 		Pase_a_OCUPADO_Buffer[33] = *(ptrFichasTiempo+2);           //LSB

		}else{
	 		Pase_a_OCUPADO_Buffer[26] = *(ptrFichasDistancia+3);         //MSB
	 		Pase_a_OCUPADO_Buffer[27] = *(ptrFichasDistancia+2);         //
	 		Pase_a_OCUPADO_Buffer[28] = *(ptrFichasDistancia+1);         //
	 		Pase_a_OCUPADO_Buffer[29] = *(ptrFichasDistancia+0);         //LSB

	 		Pase_a_OCUPADO_Buffer[30] = *(ptrFichasTiempo+3);             //MSB
	 		Pase_a_OCUPADO_Buffer[31] = *(ptrFichasTiempo+2);             //
	 		Pase_a_OCUPADO_Buffer[32] = *(ptrFichasTiempo+1);             //
	 		Pase_a_OCUPADO_Buffer[33] = *(ptrFichasTiempo+0);             //LSB
		}

		//Posicion ocupado
		Pase_a_OCUPADO_Buffer[34] = regVIAJE.sgnLatLonOcupado;
		Pase_a_OCUPADO_Buffer[35] = regVIAJE.latitudOCUPADO[0];
		Pase_a_OCUPADO_Buffer[36] = regVIAJE.latitudOCUPADO[1];
		Pase_a_OCUPADO_Buffer[37] = regVIAJE.latitudOCUPADO[2];
		Pase_a_OCUPADO_Buffer[38] = regVIAJE.longitudOCUPADO[0];
		Pase_a_OCUPADO_Buffer[39] = regVIAJE.longitudOCUPADO[1];
		Pase_a_OCUPADO_Buffer[40] = regVIAJE.longitudOCUPADO[2];
		Pase_a_OCUPADO_Buffer[41] = regVIAJE.velgpsOCUPADO;
		Pase_a_OCUPADO_Buffer[42] = nroViaje;
		Pase_a_OCUPADO_Buffer[43] = datosSC_cntWORD;

		Pase_a_OCUPADO_Buffer[N_DATOS_Pase_a_OCUPADO]  = fin_datos_msb;  // Fin Datos
		Pase_a_OCUPADO_Buffer[N_DATOS_Pase_a_OCUPADO+1] = fin_datos_lsb;// Fin Datos
}

byte chkCmd_Reloj(byte Rx_cmd){
	byte status;

	status = 0;
	if(Rx_cmd == CMD_RELOJ_Pase_a_LIBRE ||
	   Rx_cmd == CMD_RELOJ_Pase_a_OCUPADO ||
	   Rx_cmd == CMD_RELOJ_Pase_a_COBRANDO ||
	   Rx_cmd == CMD_RELOJ_Pase_a_FUERA_SERVICIO ||
	   Rx_cmd == CMD_RELOJ_Pase_a_OCUPADO_SA ||
	   Rx_cmd == CMD_RELOJ_Pase_a_OCUPADO_BANDERA ||
	   Rx_cmd == CMD_RELOJ_Pase_a_OCUPADO_APP)
	{
		status=1;
	}
	return(status);
}

void corregir_dateLIBRE(void){
	//tomo hora y fecha
	getDate();
	//fuente de hora
	Pase_a_LIBRE_Buffer[0] = HORA_source;                           // Hora GPS/RTC (0:GPS - 1:RTC)

	//DATE
	Pase_a_LIBRE_Buffer[1] = RTC_Date.hora[0];   					// HORA
	Pase_a_LIBRE_Buffer[2] = RTC_Date.hora[1];   					// MINUTOS
	Pase_a_LIBRE_Buffer[3] = RTC_Date.hora[2];   					// SEGUNDOS
	Pase_a_LIBRE_Buffer[4] = RTC_Date.fecha[0];  					// DIA
	Pase_a_LIBRE_Buffer[5] = RTC_Date.fecha[1];  					// MES
	Pase_a_LIBRE_Buffer[6] = RTC_Date.fecha[2];  					// AÑO
}

void corregir_dateOCUPADO(void){
	//tomo hora y fecha
	getDate();

	//fuente de hora
	Pase_a_OCUPADO_Buffer[0] = HORA_source;                           // Hora GPS/RTC (0:GPS - 1:RTC)

	//DATE
	Pase_a_OCUPADO_Buffer[1] = regVIAJE.dateOCUPADO.hora[0];   					// HORA
	Pase_a_OCUPADO_Buffer[2] = regVIAJE.dateOCUPADO.hora[1];   					// MINUTOS
	Pase_a_OCUPADO_Buffer[3] = regVIAJE.dateOCUPADO.hora[2];   					// SEGUNDOS
	Pase_a_OCUPADO_Buffer[4] = regVIAJE.dateOCUPADO.fecha[0];  					// DIA
	Pase_a_OCUPADO_Buffer[5] = regVIAJE.dateOCUPADO.fecha[1];  					// MES
	Pase_a_OCUPADO_Buffer[6] = regVIAJE.dateOCUPADO.fecha[2];  					// AÑO}
}

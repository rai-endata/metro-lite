#include  "- metroBLUE Config -.h"
#include "DA Define Commands.h"
#include "DA Define Null Commands.h"
#include "DA Tx.h"
#include "DA Communication.h"
#include "Reloj.h"
#include "Inicio.h"
#include "Constantes.h"
#include "- Reloj-Blue - Config -.h"
#include "rtc.h"
#include "Tarifacion Reloj.h"
#include "Odometro.h"
#include "gps.h"
#include "reloj.h"
#include "Parametros Reloj.h"
#include "main.h"
#include "Ticket Turno.h"
#include "Reportes.h"
#include "Ticket Parcial.h"
#include "Air Update.h"


typeTxCMD CMD_NULL={0,0,0,0,0,0x0000};


	// Comando Encendido de Visor
	#define N_DATOS_EncendidoEquipo		17
	#define N_EncendidoEquipo          (N_CMD + N_DATOS_EncendidoEquipo)
	static byte EncendidoEquipo_Buffer[N_DATOS_EncendidoEquipo + 2];   // Sumo DF + 0A
	typeTxCMD   CMD_Encendido_EQUIPO={0,cmdEncendido_EQUIPO,0,timeReint_rapido,N_EncendidoEquipo,EncendidoEquipo_Buffer};


	//Comando Botón EMERGENCIA
	#define N_DATOS_Boton_EMERGENCIA		24
	#define N_Boton_EMERGENCIA          (N_CMD + N_DATOS_Boton_EMERGENCIA)
	static byte Boton_EMERGENCIA_Buffer[N_DATOS_Boton_EMERGENCIA + 2];   // Sumo DF + 0A
	typeTxCMD   CMD_Boton_EMERGENCIA={0,cmdBoton_EMERGENCIA,0,timeReint_rapido,N_Boton_EMERGENCIA,Boton_EMERGENCIA_Buffer};


	//Comando Balor de Viaje
	#define N_DATOS_Valor_VIAJE		34
	#define N_Valor_VIAJE          (N_CMD + N_DATOS_Valor_VIAJE)
	static byte Valor_VIAJE_Buffer[N_DATOS_Valor_VIAJE + 2];   // Sumo DF + 0A
	typeTxCMD   CMD_Valor_VIAJE={0,cmdValor_VIAJE,0,timeReint_rapido,N_Valor_VIAJE,Valor_VIAJE_Buffer};

	//Comando Bajada de bandera
	#define N_DATOS_Valor_BAJADA_BANDERA		34

	#define N_Valor_BAJADA_BANDERA          (N_CMD + N_DATOS_Valor_BAJADA_BANDERA)
	static byte Valor_BAJADA_BANDERA_Buffer[N_DATOS_Valor_BAJADA_BANDERA + 2];   // Sumo DF + 0A
	typeTxCMD   CMD_Valor_BAJADA_BANDERA={0,cmdValor_BAJADA_BANDERA,0,timeReint_rapido,N_Valor_BAJADA_BANDERA,Valor_BAJADA_BANDERA_Buffer};


	//Comando Cuenta FICHAS
	//#define N_DATOS_Distancia_Velocidad		14
	//#define N_Distancia_Velocidad          (N_CMD + N_DATOS_Distancia_Velocidad)
	//static byte Distancia_Velocidad_Buffer[N_DATOS_Distancia_Velocidad + 2];   // Sumo DF + 0A
	//typeTxCMD   CMD_Distancia_Velocidad={0,cmdDistancia_Velocidad,0,timeReint_rapido,N_Distancia_Velocidad,Distancia_Velocidad_Buffer};

	//#define N_DATOS_PedidoPase_OCUPADO		8
	//#define N_PedidoPase_OCUPADO          (N_CMD + N_DATOS_PedidoPase_OCUPADO)
	//static byte PedidoPase_OCUPADO_Buffer[N_DATOS_PedidoPase_OCUPADO + 2];   // Sumo DF + 0A
	//typeTxCMD   CMD_PedidoPase_OCUPADO={0,cmdPedidoPase_OCUPADO,0,timeReint_rapido,N_PedidoPase_OCUPADO,PedidoPase_OCUPADO_Buffer};

	//Comando Resumen de VIAJE
	//#define N_DATOS_Resumen_VIAJE		32
	//#define N_Resumen_VIAJE          (N_CMD + N_DATOS_Resumen_VIAJE)
	//static byte Resumen_VIAJE_Buffer[N_DATOS_Resumen_VIAJE + 2];   // Sumo DF + 0A
	//typeTxCMD   CMD_Resumen_VIAJE={0,cmdResumen_VIAJE,0,timeReint_rapido,N_Resumen_VIAJE,Resumen_VIAJE_Buffer};

	//Comando Velocidad y posición
	//#define N_DATOS_Posicion		24
	//#define N_Posicion          (N_CMD + N_DATOS_Posicion)
	//static byte Posicion_Buffer[N_DATOS_Posicion + 2];   // Sumo DF + 0A
	//typeTxCMD   CMD_Posicion={0,cmdPosicion,0,timeReint_rapido,N_Posicion,Posicion_Buffer};


	//Comando Consulta ESTADO
	#define N_DATOS_Status_RELOJ		12
	#define N_Status_RELOJ          (N_CMD + N_DATOS_Status_RELOJ)
	static byte Status_RELOJ_Buffer[N_DATOS_Status_RELOJ + 2];   // Sumo DF + 0A
	typeTxCMD   CMD_Status_RELOJ={0,cmdStatus_RELOJ,0,timeReint_rapido,N_Status_RELOJ,Status_RELOJ_Buffer};

	//Comando minutos de espera
	#define N_DATOS_TARIFAS		   102
	#define N_TARIFAS          (N_CMD + N_DATOS_TARIFAS)
	static byte TARIFAS_Buffer[N_DATOS_TARIFAS + 2];   // Sumo DF + 0A
	typeTxCMD   CMD_TARIFAS={0,cmdTARIFAS,0,timeReint_rapido,N_TARIFAS,TARIFAS_Buffer};

	//Comando inicio de turno
	#define N_DATOS_Conf_inicioTURNO		7
	#define N_Conf_inicioTURNO          	(N_CMD + N_DATOS_Conf_inicioTURNO)
	static byte Conf_inicioTURNO_Buffer[N_DATOS_Conf_inicioTURNO + 2];   // Sumo DF + 0A
	typeTxCMD   CMD_Conf_inicioTURNO ={0,cmdConf_inicioTURNO,0,timeReint_rapido,N_Conf_inicioTURNO,Conf_inicioTURNO_Buffer};

	//Comando fin de turno
	//#define N_DATOS_Conf_finTURNO		7
	//#define N_Conf_finTURNO          (N_CMD + N_DATOS_Conf_finTURNO)
	//static byte Conf_finTURNO_Buffer[N_DATOS_Conf_finTURNO + 2];   // Sumo DF + 0A
	//typeTxCMD   CMD_Conf_finTURNO={0,cmdConf_finTURNO,0,timeReint_rapido,N_Conf_finTURNO,Conf_finTURNO_Buffer};


	// Comando comando transparente
	#define N_DATOS_comandoTRANSPARENTE		6
	#define N_comandoTRANSPARENTE          (N_CMD + N_DATOS_comandoTRANSPARENTE)
	//static byte comandoTRANSPARENTE_Buffer[N_DATOS_comandoTRANSPARENTE + 2];   // Sumo DF + 0A

	static byte comandoTRANSPARENTE_Buffer[dim_comandoTRANSPARENTE_Buffer];   // Sumo DF + 0A
	typeTxCMD   CMD_TRANSPARENTE={0,CMD_comandoTRANSPARENTE,0,timeReint_cmdCentral,N_comandoTRANSPARENTE,comandoTRANSPARENTE_Buffer};


	// Comando comando MENSAJE
	#define N_DATOS_comandoMENSAJE		6
	#define N_comandoMENSAJE          (N_CMD + N_DATOS_comandoMENSAJE)
	//static byte comandoTRANSPARENTE_Buffer[N_DATOS_comandoTRANSPARENTE + 2];   // Sumo DF + 0A
	static byte comandoMENSAJE_Buffer[255];   // Sumo DF + 0A
	typeTxCMD   CMD_MENSAJE={0,CMD_comandoMENSAJE,0,timeReint_rapido,N_comandoMENSAJE,comandoMENSAJE_Buffer};


	static void getVersion(byte* buf);
	byte RESP_DATOS_buffer[4];

/*********************************************************************************************/
/* CONSTANTES */
/**************/
  // Tabla con punteros a los distintos comandos de Tx.
  typeTxCMD* const Tabla_ComandosTx[129]={
    //esta tabla contiene la dirección de las variables tipo typeTxCMD
	// Cada variable del tipo typeTxCMD contiene la siguiente información:
    //  - Bandera de Tx del Comando
    //  - Comando
    //  - Contador de Reintentos
    //  - Tiempo entre reintentos
    //  - Puntero a Buffer (en caso de ser necesario)
    &Comando_00,
	&CMD_Encendido_EQUIPO,
	&CMD_Boton_EMERGENCIA,
	&CMD_Valor_BAJADA_BANDERA,
	&CMD_Valor_VIAJE,
	&Comando_05,
	&Comando_06,
	&Comando_07,
	//&CMD_PedidoPase_OCUPADO,
	//&CMD_Resumen_VIAJE,
	//&CMD_Posicion,
	&CMD_Status_RELOJ,
	&CMD_TARIFAS,
	&CMD_Conf_inicioTURNO,
	//&CMD_Conf_finTURNO,
	&CMD_AIRUPDATE_SUCCESS,             // 0x0B - Actualizacion por Aire Exitosa,
	&CMD_AirRead,
    //&CMD_Distancia_Velocidad,
    &Comando_0D,
    &Comando_0E,
    &Comando_0F,
    &Comando_10,
    &Comando_11,
    &Comando_12,
    &Comando_13,
    &Comando_14,
    &Comando_15,
    &Comando_16,
    &Comando_17,
    &CMD_Pase_a_LIBRE,                         // 0x18 - Libre
    &CMD_Pase_a_OCUPADO,                       // 0x19 - Ocupado
    &CMD_Pase_a_COBRANDO,
    &CMD_Pase_a_FUERA_SERVICIO,
    &CMD_Pase_a_OCUPADO_SA,
    &CMD_Pase_a_OCUPADO_BANDERA,
    &CMD_Pase_a_OCUPADO_APP,
    &Comando_1F,
    &Comando_20,
    &Comando_21,
    &Comando_22,
    &Comando_23,
    &Comando_24,
    &Comando_25,
    &Comando_26,
    &Comando_27,
    &CMD_Pase_a_LIBRE_SC,                         // 0x28 - Libre sin conexion
    &Comando_29,
	&CMD_Pase_a_COBRANDO_SC,
    &Comando_2B,
    &CMD_Pase_a_OCUPADO_SA_SC,
    &Comando_2D,
    &CMD_Pase_a_OCUPADO_APP_SC,
    &Comando_2F,
    &Comando_30,
    &Comando_31,
    &Comando_32,
    &Comando_33,
    &Comando_34,
    &Comando_35,
    &Comando_36,
    &Comando_37,
    &Comando_38,
    &Comando_39,
    &Comando_3A,
    &Comando_3B,
    &Comando_3C,
    &Comando_3D,
    &Comando_3E,
    &Comando_3F,
    &CMD_TRANSPARENTE,
    &Comando_41,
    &Comando_42,
    &Comando_43,
    &Comando_44,
    &Comando_45,
    &Comando_46,
    &Comando_47,
    &Comando_48,
    &Comando_49,
    &Comando_4A,
    &Comando_4B,
    &Comando_4C,
    &Comando_4D,
    &Comando_4E,
    &Comando_4F,
    &CMD_MENSAJE,
    &Comando_51,
    &Comando_52,
    &Comando_53,
    &Comando_54,
	&Comando_55,
    &Comando_56,
    &Comando_57,
    &Comando_58,
    &Comando_59,
	&Comando_5A,
    &Comando_5B,
    &Comando_5C,
    &Comando_5D,
    &Comando_5E,
    &Comando_5F,
    &Comando_60,
    &Comando_61,
    &Comando_62,
    &Comando_63,
    &Comando_64,
    &Comando_65,
    &Comando_66,
    &Comando_67,
    &Comando_68,
    &Comando_69,
    &Comando_6A,
    &Comando_6B,
    &Comando_6C,
    &Comando_6D,
    &Comando_6E,
    &Comando_6F,
    &Comando_70,
    &Comando_71,
    &Comando_72,
    &Comando_73,
    &Comando_74,
    &Comando_75,
    &Comando_76,
    &Comando_77,
    &Comando_78,
    &Comando_79,
    &Comando_7A,
    &Comando_7B,
    &Comando_7C,
    &Comando_7D,
    &Comando_7E,
    &Comando_7F,
    &CMD_RTA
  };


  // Tabla de Funciones de Transmission de Comandos
  const typeTABLA_Tx Tabla_TxDA[]={
    // Se ingresa con el No de Comando, y se ejecuta la rutina de Tx
    // de ese comando
    Tx_comando_00,
	Tx_BUFFER,
	Tx_BUFFER,
	Tx_BUFFER,
	Tx_BUFFER,
	Tx_BUFFER,
	Tx_BUFFER,
	Tx_BUFFER,
	Tx_BUFFER,
	Tx_BUFFER,
	Tx_BUFFER,
	Tx_BUFFER,
	Tx_BUFFER,
    Tx_comando_0D,
    Tx_comando_0E,
    Tx_comando_0F,
    Tx_comando_10,
    Tx_comando_11,
    Tx_comando_12,
    Tx_comando_13,
    Tx_comando_14,
    Tx_comando_15,
    Tx_comando_16,
    Tx_comando_17,
    Tx_BUFFER,                          // 0x18 - Libre
    Tx_BUFFER,                          // 0x19 - Ocupado
	Tx_BUFFER,							// 0x1A - Cobrando
	Tx_BUFFER,							// 0x1B - Fuera de Servicio
	Tx_BUFFER,                          // 0x1C - Ocupado SENSOR DE ASIENTO
	Tx_BUFFER,                          // 0x1D - Ocupado BANDERA
	Tx_BUFFER,                          // 0x1E - Ocupado APP
    Tx_comando_1F,
    Tx_comando_20,
    Tx_comando_21,
	Tx_comando_22,
	Tx_comando_23,
	Tx_comando_24,
	Tx_comando_25,
	Tx_comando_26,
    Tx_comando_27,
	Tx_BUFFER,								//0x28 - Libre SIN CONEXION
	Tx_comando_29,
	Tx_BUFFER,								//0x2A - cobrando SIN CONEXION
    Tx_comando_2B,
	Tx_BUFFER,								//0x2C - ocupado SA SIN CONEXION
	Tx_comando_2D,
	Tx_BUFFER,								//0x2E - ocupado APP SIN CONEXION
	Tx_comando_2F,
	Tx_comando_30,
	Tx_comando_31,
	Tx_comando_32,
	Tx_comando_33,
	Tx_comando_34,
	Tx_comando_35,
    Tx_comando_36,
	Tx_comando_37,
	Tx_comando_38,
	Tx_comando_39,
	Tx_comando_3A,
	Tx_comando_3B,
	Tx_comando_3C,
	Tx_comando_3D,
	Tx_comando_3E,
	Tx_comando_3F,
	Tx_BUFFER,
    Tx_comando_41,
    Tx_comando_42,
    Tx_comando_43,
    Tx_comando_44,
    Tx_comando_45,
    Tx_comando_46,
    Tx_comando_47,
    Tx_comando_48,
    Tx_comando_49,
    Tx_comando_4A,
	Tx_comando_4B,
    Tx_comando_4C,
    Tx_comando_4D,
    Tx_comando_4E,
    Tx_comando_4F,
	Tx_BUFFER,
    Tx_comando_51,
	Tx_comando_52,
	Tx_comando_53,
	Tx_comando_54,
	Tx_comando_55,
	Tx_comando_56,
	Tx_comando_57,
	Tx_comando_58,
    Tx_comando_59,
	Tx_comando_5A,
    Tx_comando_5B,
    Tx_comando_5C,
	Tx_comando_5D,
	Tx_comando_5E,
	Tx_comando_5F,
    Tx_comando_60,
    Tx_comando_61,
    Tx_comando_62,
    Tx_comando_63,
    Tx_comando_64,
    Tx_comando_65,
    Tx_comando_66,
    Tx_comando_67,
	Tx_comando_68,
	Tx_comando_69,
    Tx_comando_6A,
    Tx_comando_6B,
    Tx_comando_6C,
    Tx_comando_6D,
    Tx_comando_6E,
    Tx_comando_6F,
    Tx_comando_70,
    Tx_comando_71,
    Tx_comando_72,
    Tx_comando_73,
    Tx_comando_74,
    Tx_comando_75,
    Tx_comando_76,
    Tx_comando_77,
    Tx_comando_78,
    Tx_comando_79,
    Tx_comando_7A,
    Tx_comando_7B,
    Tx_comando_7C,
    Tx_comando_7D,
    Tx_comando_7E,
    Tx_comando_7F,
    Tx_RTA_toDA,                             // TODAS LAS RESPUESTAS
  };




  /* DUMMY - PARA RELLENAR LA TABLA DE TxCENTRAL */
  /*----------------------------------------------*/
   void Tx_NONE (void){
    }


   void Tx_Encendido_EQUIPO  (void){

	   tDATE datePrueba;

	   CMD_Encendido_EQUIPO.Tx_F = 1;                      // Levanto Bandera de Tx
	   CMD_Encendido_EQUIPO.Reintentos = reint_3;   // Cargo Cantidad de Reintentos (INFINITOS)

		//prueba
		//motivo_reset = 2;
		//HoraApagado.hora[0]  = 23;  // Hora
		//HoraApagado.hora[1]  = 30;  // Minutos
		//HoraApagado.hora[2]  = 59;  // Segundos
		//HoraApagado.fecha[0] = 31;  // Día
		//HoraApagado.fecha[1] = 12;  // Mes
		//HoraApagado.fecha[2] = 17;  // Año
	    getDate();
		EncendidoEquipo_Buffer[0] = HORA_source;        // Hora GPS/RTC (0:GPS - 1:RTC)
		EncendidoEquipo_Buffer[1] = RTC_Date.hora[0];   // HORA
		EncendidoEquipo_Buffer[2] = RTC_Date.hora[1];   // MINUTOS
		EncendidoEquipo_Buffer[3] = RTC_Date.hora[2];   // SEGUNDOS
		EncendidoEquipo_Buffer[4] = RTC_Date.fecha[0];  // DIA
		EncendidoEquipo_Buffer[5] = RTC_Date.fecha[1];  // MES
		EncendidoEquipo_Buffer[6] = RTC_Date.fecha[2];  // AÑO

		getVersion(&EncendidoEquipo_Buffer[7]);

		EncendidoEquipo_Buffer[10] = HoraApagado.hora[0];   // Hora
		EncendidoEquipo_Buffer[11]= HoraApagado.hora[1];   // Minutos
		EncendidoEquipo_Buffer[12]= HoraApagado.hora[2];   // Segundos
		EncendidoEquipo_Buffer[13]= HoraApagado.fecha[0];  // Día
		EncendidoEquipo_Buffer[14]= HoraApagado.fecha[1];  // Mes
		EncendidoEquipo_Buffer[15]= HoraApagado.fecha[2];  // Año
		EncendidoEquipo_Buffer[16]= TIPO_DE_EQUIPO;        // tipode equipo


		EncendidoEquipo_Buffer[N_DATOS_EncendidoEquipo] = fin_datos_msb;  // Fin Datos
		EncendidoEquipo_Buffer[N_DATOS_EncendidoEquipo+1] = fin_datos_lsb;// Fin Datos

   }

   static void getVersion(byte* buf){
	   byte*  ver_firm;
       byte aux;

	   ver_firm = &versionFIRMWARE;
	   aux = '.';

	   for(byte i=0; i < 3; i++){

		   if(i == 2){
    		   aux = 0;  //fin de cadena
    	   }

		   if(*(ver_firm+2) == aux){
    		   ver_firm++;
    		   *buf++ = (*ver_firm & (0x0F));
    		   ver_firm++;
    	   }else{
    		    ver_firm++;
    		   *buf++ = (*ver_firm & 0x0F)<<4 | (*(ver_firm+1) & 0x0F);
    		   ver_firm++;
    		   ver_firm++;
    	   }
       }


   }

   void Tx_Boton_EMERGENCIA (void){

   			CMD_Boton_EMERGENCIA.Tx_F = 1;                      // Levanto Bandera de Tx
   			CMD_Boton_EMERGENCIA.Reintentos = reint_3;   // Cargo Cantidad de Reintentos (INFINITOS)
   			getDate();
   			Boton_EMERGENCIA_Buffer[0] = HORA_source;        // Hora GPS/RTC (0:GPS - 1:RTC)

   			Boton_EMERGENCIA_Buffer[1] = RTC_Date.hora[0];   // HORA
   			Boton_EMERGENCIA_Buffer[2] = RTC_Date.hora[1];   // MINUTOS
   			Boton_EMERGENCIA_Buffer[3] = RTC_Date.hora[2];   // SEGUNDOS

   			Boton_EMERGENCIA_Buffer[4] = RTC_Date.fecha[0];  // DIA
   			Boton_EMERGENCIA_Buffer[5] = RTC_Date.fecha[1];  // MES
   			Boton_EMERGENCIA_Buffer[6] = RTC_Date.fecha[2];  // AÑO

   			Boton_EMERGENCIA_Buffer[7]  = GPS.Pos.lat[0];
   			Boton_EMERGENCIA_Buffer[8]  = GPS.Pos.lat[1];
   			Boton_EMERGENCIA_Buffer[9]  = GPS.Pos.lat[2];
   			Boton_EMERGENCIA_Buffer[10] = GPS.Pos.lat[3];

   			Boton_EMERGENCIA_Buffer[11] = GPS.Pos.lon[0];
   			Boton_EMERGENCIA_Buffer[12] = GPS.Pos.lon[1];
   			Boton_EMERGENCIA_Buffer[13] = GPS.Pos.lon[2];
   			Boton_EMERGENCIA_Buffer[14] = GPS.Pos.lon[3];

   			Boton_EMERGENCIA_Buffer[15] = GPS.Date.hora[0];
   			Boton_EMERGENCIA_Buffer[16] = GPS.Date.hora[1];
   			Boton_EMERGENCIA_Buffer[17] = GPS.Date.hora[2];

   			Boton_EMERGENCIA_Buffer[18] = GPS.Date.fecha[0];
   			Boton_EMERGENCIA_Buffer[19] = GPS.Date.fecha[1];
   			Boton_EMERGENCIA_Buffer[20] = GPS.Date.fecha[2];

   			Boton_EMERGENCIA_Buffer[21] = GPS.course;
   			Boton_EMERGENCIA_Buffer[22] = GPS.vel;

   			if(GPS.valid){
   				Boton_EMERGENCIA_Buffer[23] = 'A';
   			}else{
   				Boton_EMERGENCIA_Buffer[23] = 'V';
   			}

   			Boton_EMERGENCIA_Buffer[N_DATOS_Boton_EMERGENCIA] = fin_datos_msb;  // Fin Datos
   			Boton_EMERGENCIA_Buffer[N_DATOS_Boton_EMERGENCIA+1] = fin_datos_lsb;// Fin Datos

      }


   void Tx_Valor_VIAJE (void){

		byte* ptrVALOR_VIAJE;
		byte* ptrVALOR_VIAJE_mostrar;
		byte* ptrfichas_Total;
		byte* ptrDISTAMNCIA_100;
		byte* ptrVELOCIDAD;

		byte* ptrFichasDistancia;
		byte* ptrFichasTiempo;
		byte* ptrBajadaBandera;

		uint32_t FichasDistancia;
		uint32_t FichasTiempo;
		uint16_t BajadaBandera;
		uint8_t tarifa_mostrar;



	    CMD_Valor_VIAJE.Tx_F = 1;                      // Levanto Bandera de Tx
		CMD_Valor_VIAJE.Reintentos = reint_0;   // Cargo Cantidad de Reintentos (INFINITOS)

		ptrVALOR_VIAJE = &VALOR_VIAJE;
		ptrVALOR_VIAJE_mostrar = &VALOR_VIAJE_mostrar;
		ptrDISTAMNCIA_100 = &DISTANCIA_100;
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
		Valor_VIAJE_Buffer[0] = HORA_source;   				//fuente de hora

		Valor_VIAJE_Buffer[1] = RTC_Date.hora[0];   // HORA
		Valor_VIAJE_Buffer[2] = RTC_Date.hora[1];   // MINUTOS
		Valor_VIAJE_Buffer[3] = RTC_Date.hora[2];   // SEGUNDOS

		Valor_VIAJE_Buffer[4] = RTC_Date.fecha[0];  // DIA
		Valor_VIAJE_Buffer[5] = RTC_Date.fecha[1];  // MES
		Valor_VIAJE_Buffer[6] = RTC_Date.fecha[2];  // AÑO

		Valor_VIAJE_Buffer[7]  = tarifa_mostrar;  				  //numero de tarifa
		Valor_VIAJE_Buffer[8]  = fichaPESOS;  				  	//ficha o pesos

		if(TARIFA_PESOS){
		//valor en pesos
			Valor_VIAJE_Buffer[9] = PUNTO_DECIMAL;  				  //ficha o pesos
			Valor_VIAJE_Buffer[10] = *(ptrVALOR_VIAJE+3);             //
			Valor_VIAJE_Buffer[11] = *(ptrVALOR_VIAJE+2);             //
			Valor_VIAJE_Buffer[12] = *(ptrVALOR_VIAJE+1);             //
			Valor_VIAJE_Buffer[13] = *(ptrVALOR_VIAJE+0);             //

		}else{
			//valor en fichas
			if(EqPESOS_hab){
				RedondeoValorViaje_3();
				VALOR_VIAJE_mostrar = VALOR_VIAJE/10;
				Valor_VIAJE_Buffer[9] = 2;
				Valor_VIAJE_Buffer[10] = *(ptrVALOR_VIAJE_mostrar+3);             //
				Valor_VIAJE_Buffer[11] = *(ptrVALOR_VIAJE_mostrar+2);             //
				Valor_VIAJE_Buffer[12] = *(ptrVALOR_VIAJE_mostrar+1);             //
				Valor_VIAJE_Buffer[13] = *(ptrVALOR_VIAJE_mostrar+0);             //

			}else{
				Valor_VIAJE_Buffer[9] = PUNTO_DECIMAL;  				  //ficha o pesos
				Valor_VIAJE_Buffer[10] = *(ptrVALOR_VIAJE+3);             //
				Valor_VIAJE_Buffer[11] = *(ptrVALOR_VIAJE+2);             //
				Valor_VIAJE_Buffer[12] = *(ptrVALOR_VIAJE+1);             //
				Valor_VIAJE_Buffer[13] = *(ptrVALOR_VIAJE+0);             //
			}
		}


		Valor_VIAJE_Buffer[14] = *(ptrDISTAMNCIA_100+3);             //
		Valor_VIAJE_Buffer[15] = *(ptrDISTAMNCIA_100+2);             //
		Valor_VIAJE_Buffer[16] = *(ptrDISTAMNCIA_100+1);             //
		Valor_VIAJE_Buffer[17] = *(ptrDISTAMNCIA_100+0);             //
		Valor_VIAJE_Buffer[18] = minutosEspera;  				  //minutos de espera

		Valor_VIAJE_Buffer[19] = *(ptrVELOCIDAD+1) ;            	//
		Valor_VIAJE_Buffer[20] = *(ptrVELOCIDAD+0);            		//

		Valor_VIAJE_Buffer[21] = (byte)ESTADO_RELOJ; 				// estado de reloj
		Valor_VIAJE_Buffer[22] = turnoChofer;						// estado d e turno
		Valor_VIAJE_Buffer[23] = nroChofer;  						// chofer quien inicio turno (si no inicio turno es cero)

		Valor_VIAJE_Buffer[24] = *(ptrBajadaBandera+1);
		Valor_VIAJE_Buffer[25] = *(ptrBajadaBandera+0);

		Valor_VIAJE_Buffer[26] = *(ptrFichasDistancia+3);             //Cantidad de FICHAS POR DISTANCIA
		Valor_VIAJE_Buffer[27] = *(ptrFichasDistancia+2);             //Cantidad de FICHAS POR DISTANCIA
		Valor_VIAJE_Buffer[28] = *(ptrFichasDistancia+1);             //Cantidad de FICHAS POR DISTANCIA
		Valor_VIAJE_Buffer[29] = *(ptrFichasDistancia+0);             //Cantidad de FICHAS POR DISTANCIA

		Valor_VIAJE_Buffer[30] = *(ptrFichasTiempo+3);             //Cantidad de fichas por Tiempo
		Valor_VIAJE_Buffer[31] = *(ptrFichasTiempo+2);             //Cantidad de fichas por Tiempo
		Valor_VIAJE_Buffer[32] = *(ptrFichasTiempo+1);             //Cantidad de fichas por Tiempo
		Valor_VIAJE_Buffer[33] = *(ptrFichasTiempo+0);             //Cantidad de fichas por Tiempo

		Valor_VIAJE_Buffer[N_DATOS_Valor_VIAJE]  = fin_datos_msb;  // Fin Datos
		Valor_VIAJE_Buffer[N_DATOS_Valor_VIAJE+1] = fin_datos_lsb;// Fin Datos
   }

/*
   void Tx_Distancia_Velocidad  (void){

	    byte* ptrDISTAMNCIAm;
	    byte* ptrVELOCIDAD;
	    byte opcional1,opcional2;

	    CMD_Distancia_Velocidad.Tx_F = 1;                      // Levanto Bandera de Tx
		CMD_Distancia_Velocidad.Reintentos = reint_3;   // Cargo Cantidad de Reintentos (INFINITOS)

		ptrDISTAMNCIAm = &DISTANCIAm;
		ptrVELOCIDAD   = &VELOCIDAD;
		getDate();
		Distancia_Velocidad_Buffer[0] = RTC_Date.hora[0];   // HORA
		Distancia_Velocidad_Buffer[1] = RTC_Date.hora[1];   // MINUTOS
		Distancia_Velocidad_Buffer[2] = RTC_Date.hora[2];   // SEGUNDOS
		Distancia_Velocidad_Buffer[3] = RTC_Date.fecha[0];  // DIA
		Distancia_Velocidad_Buffer[4] = RTC_Date.fecha[1];  // MES
		Distancia_Velocidad_Buffer[5] = RTC_Date.fecha[2];  // AÑO

		Distancia_Velocidad_Buffer[6] = *(ptrDISTAMNCIAm+3);              //
		Distancia_Velocidad_Buffer[7] = *(ptrDISTAMNCIAm+2);             //
		Distancia_Velocidad_Buffer[8] = *(ptrDISTAMNCIAm+1);             //
		Distancia_Velocidad_Buffer[9] = *(ptrDISTAMNCIAm+0);             //
		Distancia_Velocidad_Buffer[10] = *(ptrVELOCIDAD+1);            	    //
		Distancia_Velocidad_Buffer[11] = *(ptrVELOCIDAD+0);            		//
		Distancia_Velocidad_Buffer[12] = opcional1;            				//
		Distancia_Velocidad_Buffer[13] = opcional2;            				//

		Distancia_Velocidad_Buffer[N_DATOS_Distancia_Velocidad] = fin_datos_msb;  // Fin Datos
		Distancia_Velocidad_Buffer[N_DATOS_Distancia_Velocidad+1] = fin_datos_lsb;// Fin Datos
   }

*/

/*
   void Tx_PedidoPase_OCUPADO (void){

	   byte opcional1,opcional2;

		CMD_PedidoPase_OCUPADO.Tx_F = 1;                      // Levanto Bandera de Tx
		CMD_PedidoPase_OCUPADO.Reintentos = reint_3;   // Cargo Cantidad de Reintentos (INFINITOS)

		// Armo el buffer. Solo los datos de ESTE comando
		getDate();
		PedidoPase_OCUPADO_Buffer[0] = RTC_Date.hora[0];   // HORA
		PedidoPase_OCUPADO_Buffer[1] = RTC_Date.hora[1];   // MINUTOS
		PedidoPase_OCUPADO_Buffer[2] = RTC_Date.hora[2];   // SEGUNDOS
		PedidoPase_OCUPADO_Buffer[3] = RTC_Date.fecha[0];  // DIA
		PedidoPase_OCUPADO_Buffer[4] = RTC_Date.fecha[1];  // MES
		PedidoPase_OCUPADO_Buffer[5] = RTC_Date.fecha[2];  // AÑO

		PedidoPase_OCUPADO_Buffer[6] = opcional1 ;            //
		PedidoPase_OCUPADO_Buffer[7] = opcional2;            //

		PedidoPase_OCUPADO_Buffer[N_DATOS_PedidoPase_OCUPADO] = fin_datos_msb;  // Fin Datos
		PedidoPase_OCUPADO_Buffer[N_DATOS_PedidoPase_OCUPADO+1] = fin_datos_lsb;// Fin Datos
   }*/


    byte* ptr_timerMarcha_cnt_LIBRE;
    byte* ptr_timerMarcha_cnt_OCUPADO;

/*
   void Tx_Resumen_VIAJE  (void){
 //
 //LIBRE
 //******
 //KM_ptr  	 		(2 BYTE )km recorrido en libre
 //velMAX  			(1 BYTE)velocidad máxima en libre
 //timerMarcha_cnt		(2 BYTE)tiempo en marcha en libre
 //timerParado_cnt  	(2 BYTE)tiempo parado en libre

 //OCUPADO
 //*******
 //KM_ptr  	 		(2 BYTE)km recorrido en ocupado
 //velMAX  	 		(1 BYTE)velocidad máxima en ocupado
 //timerMarcha_cnt		(2 BYTE)tiempo en marcha en ocupado
 //timerParado_cnt  	(2 BYTE)tiempo parado en ocupado
 //MIN	    	 		(1 BYTE)minutos de espera
 //tarifa  	 		(1 BYTE)tarifa
 //importe_ptr  		(4 BYTE)importe de viaje
 //chofer  	 		(1 BYTE)nro chofer con que inicio turno (1,2,3,4)
 //NC      	 		(1 BYTE)nro correlativo


 	   //corregir bien el cambio de km a metros
	   byte* ptr_mRecorridos_LIBRE;
 	    byte* ptr_mRecorridos_OCUPADO;


 	    byte* ptr_timerParado_cnt_LIBRE;
 	    byte* ptr_timerParado_cnt_OCUPADO;


 	    byte* ptr_importe;
 	    byte opcional1, opcional2, opcional3, opcional4;

 	    ptr_mRecorridos_LIBRE 		= &kmRecorridos_LIBRE;
 	    ptr_mRecorridos_OCUPADO 	= &kmRecorridos_OCUPADO;
 	    ptr_timerMarcha_cnt_OCUPADO = &timerMarcha_cnt_LIBRE;
 	    ptr_timerMarcha_cnt_OCUPADO = &timerMarcha_cnt_OCUPADO;
 	    ptr_timerParado_cnt_LIBRE 	= &timerParado_cnt_LIBRE;
 	    ptr_timerParado_cnt_OCUPADO = &timerParado_cnt_OCUPADO;
 	    ptr_importe = &VALOR_VIAJE;

 	    //BANDERA DE TRANSMISION
 		CMD_Resumen_VIAJE.Tx_F = 1;                      // Levanto Bandera de Tx
 		CMD_Resumen_VIAJE.Reintentos = reint_3;   // Cargo Cantidad de Reintentos (INFINITOS)
 		getDate();
 		Resumen_VIAJE_Buffer[0] = HORA_source;        // Hora GPS/RTC (0:GPS - 1:RTC)
 		//DATE
 		getDate();
 		Resumen_VIAJE_Buffer[1] = RTC_Date.hora[0];   					// HORA
 		Resumen_VIAJE_Buffer[2] = RTC_Date.hora[1];   					// MINUTOS
 		Resumen_VIAJE_Buffer[3] = RTC_Date.hora[2];   					// SEGUNDOS
 		Resumen_VIAJE_Buffer[4] = RTC_Date.fecha[0];  					// DIA
 		Resumen_VIAJE_Buffer[5] = RTC_Date.fecha[1];  					// MES
 		Resumen_VIAJE_Buffer[6] = RTC_Date.fecha[2];  					// AÑO

 		//LIBRE
 		Resumen_VIAJE_Buffer[7]  = *(ptr_mRecorridos_LIBRE+2);  			//km recorrido en  LIBRE parte baja
 		Resumen_VIAJE_Buffer[8]  = *(ptr_mRecorridos_LIBRE+1);  		//km recorrido en LIBRE parte alta
 		Resumen_VIAJE_Buffer[9]  = *(ptr_mRecorridos_LIBRE+0);  		//km recorrido en LIBRE parte alta

 		Resumen_VIAJE_Buffer[10]  = velMax_LIBRE;  						//velocidad máxima en LIBRE
 		Resumen_VIAJE_Buffer[11]  = *(ptr_timerMarcha_cnt_LIBRE+1);  		//tiempo de marcha parte alta
 		Resumen_VIAJE_Buffer[12] = *(ptr_timerMarcha_cnt_LIBRE+0);  	//tiempo de marcha parte baja en LIBRE
 		Resumen_VIAJE_Buffer[13] = *(ptr_timerParado_cnt_LIBRE+1);  		//tiempo parado parte alta en LIBRE
 		Resumen_VIAJE_Buffer[14] = *(ptr_timerParado_cnt_LIBRE+0);  	//tiempo parado parte baja en LIBRE
 		//OCUPADO
 		Resumen_VIAJE_Buffer[15]  = *(ptr_mRecorridos_OCUPADO+2);  		//km recorrido en OCUPADO parte baja
 		Resumen_VIAJE_Buffer[16]  = *(ptr_mRecorridos_OCUPADO+1);  	//km recorrido en OCUPADO parte alta
 		Resumen_VIAJE_Buffer[17]  = *(ptr_mRecorridos_OCUPADO+0);  	//km recorrido en OCUPADO parte alta

 		Resumen_VIAJE_Buffer[18]  = velMax_OCUPADO;  					//velocidad máxima en OCUPADO parte alta
 		Resumen_VIAJE_Buffer[19]  = *(ptr_timerMarcha_cnt_OCUPADO+1);  		//tiempo marcha en OCUPADO parte alta
 		Resumen_VIAJE_Buffer[20]  = *(ptr_timerMarcha_cnt_OCUPADO+0);  	//tiempo marcha en OCUPADO parte baja


 		Resumen_VIAJE_Buffer[21]  = *(ptr_timerParado_cnt_OCUPADO+1);  		//tiempo parado en OCUPADO parte alta
 		Resumen_VIAJE_Buffer[22]  = *(ptr_timerParado_cnt_OCUPADO+0);  	//tiempo parado en OCUPADO parte baja


 		Resumen_VIAJE_Buffer[23]  = minutosEspera;  					//minutos de espera
 		Resumen_VIAJE_Buffer[24]  = TARIFA.numero;  					//numero de tarifa

 		Resumen_VIAJE_Buffer[25]  = *(ptr_importe+3);  					//importe parte baja
 		Resumen_VIAJE_Buffer[26]  = *(ptr_importe+2);  					//importe parte media baja
 		Resumen_VIAJE_Buffer[27]  = *(ptr_importe+1);  					//importe parte media alta
 		Resumen_VIAJE_Buffer[28]  = *(ptr_importe+0);  					//importe parte alta

 		Resumen_VIAJE_Buffer[29]  = PUNTO_DECIMAL;  						//opcional1
 		Resumen_VIAJE_Buffer[30]  = nroChofer;  						//numero de chofer
 		Resumen_VIAJE_Buffer[31]  = nroCorrelativo_INTERNO;  					//numero correlativo

 		//FIN
 		Resumen_VIAJE_Buffer[N_DATOS_Resumen_VIAJE] = fin_datos_msb;  	// Fin Datos
 		Resumen_VIAJE_Buffer[N_DATOS_Resumen_VIAJE+1] = fin_datos_lsb;	// Fin Datos
   }*/

/*
   void Tx_Posicion (void){


	    CMD_Posicion.Tx_F = 1;                      // Levanto Bandera de Tx
		CMD_Posicion.Reintentos = reint_3;   // Cargo Cantidad de Reintentos (INFINITOS)
		getDate();
		Posicion_Buffer[0]  = HORA_source;        // Hora GPS/RTC (0:GPS - 1:RTC)
		Posicion_Buffer[1]  = RTC_Date.hora[0];   // HORA
		Posicion_Buffer[2]  = RTC_Date.hora[1];   // MINUTOS
		Posicion_Buffer[3]  = RTC_Date.hora[2];   // SEGUNDOS
		Posicion_Buffer[4]  = RTC_Date.fecha[0];  // DIA
		Posicion_Buffer[5]  = RTC_Date.fecha[1];  // MES
		Posicion_Buffer[6]  = RTC_Date.fecha[2];  // AÑO

		Posicion_Buffer[7]  = GPS.Pos.lat[0];
		Posicion_Buffer[8]  = GPS.Pos.lat[1];
		Posicion_Buffer[9]  = GPS.Pos.lat[2];
		Posicion_Buffer[10]  = GPS.Pos.lat[3];
		Posicion_Buffer[11] = GPS.Pos.lon[0];
		Posicion_Buffer[12] = GPS.Pos.lon[1];
		Posicion_Buffer[13] = GPS.Pos.lon[2];
		Posicion_Buffer[14] = GPS.Pos.lon[3];

		Posicion_Buffer[15] = GPS.Date.hora[0];
		Posicion_Buffer[16] = GPS.Date.hora[1];
		Posicion_Buffer[17] = GPS.Date.hora[2];
		Posicion_Buffer[18] = GPS.Date.fecha[0];
		Posicion_Buffer[19] = GPS.Date.fecha[1];
		Posicion_Buffer[20] = GPS.Date.fecha[2];

		Posicion_Buffer[21] = GPS.course;
		Posicion_Buffer[22] = GPS.vel;

		if(GPS.valid){
		  Posicion_Buffer[23] = 'A';
		}else{
		  Posicion_Buffer[23] = 'V';
		}

		Posicion_Buffer[N_DATOS_Posicion] = fin_datos_msb;  // Fin Datos
		Posicion_Buffer[N_DATOS_Posicion+1] = fin_datos_lsb;// Fin Datos
   }
*/

   void Tx_Status_RELOJ  (void){

	   byte opcional1, opcional2, opcional3,opcional4;

		CMD_Status_RELOJ.Tx_F = 1;                      // Levanto Bandera de Tx
		CMD_Status_RELOJ.Reintentos = reint_0;   // Cargo Cantidad de Reintentos (INFINITOS)
		//CMD_Status_RELOJ.Reintentos = reint_3;   // Cargo Cantidad de Reintentos 3
		getDate();
		Status_RELOJ_Buffer[0] = 0;   				//fuente de hora
		Status_RELOJ_Buffer[1] = RTC_Date.hora[0];   // HORA
		Status_RELOJ_Buffer[2] = RTC_Date.hora[1];   // MINUTOS
		Status_RELOJ_Buffer[3] = RTC_Date.hora[2];   // SEGUNDOS
		Status_RELOJ_Buffer[4] = RTC_Date.fecha[0];  // DIA
		Status_RELOJ_Buffer[5] = RTC_Date.fecha[1];  // MES
		Status_RELOJ_Buffer[6] = RTC_Date.fecha[2];  // AÑO

		Status_RELOJ_Buffer[7] = (byte)ESTADO_RELOJ; // estado de reloj
		Status_RELOJ_Buffer[8] = turnoChofer;		   // estado de turno
		Status_RELOJ_Buffer[9] = nroChofer;  		   // chofer quien inicio turno (si no inicio turno es cero)

		Status_RELOJ_Buffer[10] = TIPO_DE_EQUIPO;  		   // chofer quien inicio turno (si no inicio turno es cero)
		Status_RELOJ_Buffer[11] = VELOCIDAD;  		   // chofer quien inicio turno (si no inicio turno es cero)

		Status_RELOJ_Buffer[N_DATOS_Status_RELOJ] = fin_datos_msb;  // Fin Datos
		Status_RELOJ_Buffer[N_DATOS_Status_RELOJ+1] = fin_datos_lsb;// Fin Datos
   }


   void Tx_TARIFAS  (void){

	    byte j;
 		byte* ptrTARIFA1; byte* ptrTARIFA2; byte* ptrTARIFA3; byte* ptrTARIFA4;
 		byte* ptrEqPesosDiurno; byte* ptrEqPesosNocturno;
		byte* ptr_pulsosXKm; byte* ptr_carreraBandera;
		byte* ptrRTC_Date;

		ptr_pulsosXKm        		  = &PULSOS_x_KM;
		ptr_carreraBandera   		  = &CARRERA_BANDERA;
		getDate();
		//inicio punteo a fecha y hora
		ptrRTC_Date = &RTC_Date;
		//inicio puntero a tarifas
  		ptrTARIFA1 = &TARIFA1; ptrTARIFA2 = &TARIFA2; ptrTARIFA3 = &TARIFA3; ptrTARIFA4 = &TARIFA4;
        //inicio puntero a equivalente en peso
  		//ptrEqPesosDiurno = &EqPesosDiurno;
  		ptrEqPesosDiurno = &EqPESOS_DIURNA;

  		//ptrEqPesosNocturno = &EqPesosNocturno;
  		ptrEqPesosNocturno = &EqPESOS_NOCTURNA;

  		CMD_TARIFAS.Tx_F = 1;                      // Levanto Bandera de Tx
  		CMD_TARIFAS.Reintentos = reint_3;   // Cargo Cantidad de Reintentos (INFINITOS)

  		j=0;
  		while(j <19){
  			//guardo fecha y hora
			if(j<6){
				//RTC_Date = {diaMes,Mes,Año,DiaSemana,Hora,Min,Seg}
				if(j<3){
					TARIFAS_Buffer[j]  = *ptrRTC_Date++;  	//copio fecha
				}
				if(j>2){
					TARIFAS_Buffer[j]  = *(ptrRTC_Date+1);	//salto DiaSemana y copio hora
					ptrRTC_Date++;
				}
			}
  			//guardo tarifas
  			TARIFAS_Buffer[6+19*0+j] = *ptrTARIFA1++;
			TARIFAS_Buffer[6+19*1+j] = *ptrTARIFA2++;
			TARIFAS_Buffer[6+19*2+j] = *ptrTARIFA3++;
			TARIFAS_Buffer[6+19*3+j] = *ptrTARIFA4++;
  		    //guardo equivalente en pesos
			if(j<7){
				TARIFAS_Buffer[6+19*4+j]  = *ptrEqPesosDiurno++;
				TARIFAS_Buffer[6+19*4+7+j] = *ptrEqPesosNocturno++;
			}
			j++;
  		}
        //guardo constantes comunes a distintas tarifas
  		TARIFAS_Buffer[6+19*4+7+7+0]  = *(ptr_pulsosXKm+1);				//uint16_t
  		TARIFAS_Buffer[6+19*4+7+7+1]  = *(ptr_pulsosXKm+0);				//uint16_t
  		TARIFAS_Buffer[6+19*4+7+7+2]  = *(ptr_carreraBandera+1);			//uint16_t
  		TARIFAS_Buffer[6+19*4+7+7+3]  = *(ptr_carreraBandera+0);			//uint16_t
  		TARIFAS_Buffer[6+19*4+7+7+4]	= fichaPESOS;						//byte
  		TARIFAS_Buffer[6+19*4+7+7+5]	= PUNTO_DECIMAL;						//byte
  		//TARIFAS_Buffer[6+19*4+7+7+5]	= 1;						//byte

		TARIFAS_Buffer[N_DATOS_TARIFAS] = fin_datos_msb;  // Fin Datos
		TARIFAS_Buffer[N_DATOS_TARIFAS+1] = fin_datos_lsb;// Fin Datos
     }


   void Tx_Conf_inicioTURNO  (byte status){

     		CMD_Conf_inicioTURNO.Tx_F = 1;                   // Levanto Bandera de Tx
     		CMD_Conf_inicioTURNO.Reintentos = reint_3;   	 // Cargo Cantidad de Reintentos (INFINITOS)
     		getDate();
     		Conf_inicioTURNO_Buffer[0] = RTC_Date.hora[0];   // HORA
     		Conf_inicioTURNO_Buffer[1] = RTC_Date.hora[1];   // MINUTOS
     		Conf_inicioTURNO_Buffer[2] = RTC_Date.hora[2];   // SEGUNDOS
     		Conf_inicioTURNO_Buffer[3] = RTC_Date.fecha[0];  // DIA
     		Conf_inicioTURNO_Buffer[4] = RTC_Date.fecha[1];  // MES
     		Conf_inicioTURNO_Buffer[5] = RTC_Date.fecha[2];  // AÑO
     		Conf_inicioTURNO_Buffer[6] = status; 			 // inicio de turno o motivo de no inicio

     		Conf_inicioTURNO_Buffer[N_DATOS_Conf_inicioTURNO] = fin_datos_msb;  // Fin Datos
     		Conf_inicioTURNO_Buffer[N_DATOS_Conf_inicioTURNO+1] = fin_datos_lsb;// Fin Datos
        }

/*
   void Tx_Conf_finTURNO  (byte status){

			CMD_Conf_finTURNO.Tx_F = 1;                      // Levanto Bandera de Tx
			CMD_Conf_finTURNO.Reintentos = reint_3;   // Cargo Cantidad de Reintentos (INFINITOS)
			getDate();
			Conf_finTURNO_Buffer[0] = RTC_Date.hora[0];   // HORA
			Conf_finTURNO_Buffer[1] = RTC_Date.hora[1];   // MINUTOS
			Conf_finTURNO_Buffer[2] = RTC_Date.hora[2];   // SEGUNDOS
			Conf_finTURNO_Buffer[3] = RTC_Date.fecha[0];  // DIA
			Conf_finTURNO_Buffer[4] = RTC_Date.fecha[1];  // MES
			Conf_finTURNO_Buffer[5] = RTC_Date.fecha[2];  // AÑO
			Conf_finTURNO_Buffer[6] = status; 			  // fin de turno o motivo de no finalizacion

			Conf_finTURNO_Buffer[N_DATOS_Conf_finTURNO] = fin_datos_msb;  // Fin Datos
			Conf_finTURNO_Buffer[N_DATOS_Conf_finTURNO+1] = fin_datos_lsb;// Fin Datos
		}
*/

   void Tx_cmdTRANSPARENTE(byte N, byte* buffer){

   	  // tDATE datePrueba;
   	   byte i;
   	   byte chk;
   	   byte n;

   	   CMD_TRANSPARENTE.Tx_F = 1;               // Levanto Bandera de Tx
   	   CMD_TRANSPARENTE.Reintentos = reint_3;   // no tiene reintentos

       i=0;
       n = N-1;
       chk = 0;
   	   while(i<n){
   		comandoTRANSPARENTE_Buffer[i] = buffer[i];
   		i++;
   	   }

   	   CMD_TRANSPARENTE.N = N_CMD +i;

   	   comandoTRANSPARENTE_Buffer[i] = fin_datos_msb;  // Fin Datos
  	   comandoTRANSPARENTE_Buffer[i+1] = fin_datos_lsb;// Fin Datos


      }



   void Tx_Comando_TRANSPARENTE_prueba(void){

      	   tDATE datePrueba;
      	   byte i;
      	   byte chk;

      	   CMD_TRANSPARENTE.Tx_F = 1;                      // Levanto Bandera de Tx
      	   CMD_TRANSPARENTE.Reintentos = reint_3;   // Cargo Cantidad de Reintentos (INFINITOS)
            i=0;
      		comandoTRANSPARENTE_Buffer[i++] = 'P';
      		comandoTRANSPARENTE_Buffer[i++] = 'r';
      		comandoTRANSPARENTE_Buffer[i++] = 'u';
      		comandoTRANSPARENTE_Buffer[i++] = 'e';
      		comandoTRANSPARENTE_Buffer[i++] = 'b';
      		comandoTRANSPARENTE_Buffer[i++] = 'a';

      		CMD_TRANSPARENTE.N = N_CMD +i;

    		comandoTRANSPARENTE_Buffer[i++] = fin_datos_msb;  // Fin Datos
     		comandoTRANSPARENTE_Buffer[i] = fin_datos_lsb;// Fin Datos

      		//comandoTRANSPARENTE_Buffer[N_DATOS_comandoTRANSPARENTE] = fin_datos_msb;  // Fin Datos
      		//comandoTRANSPARENTE_Buffer[N_DATOS_comandoTRANSPARENTE+1] = fin_datos_lsb;// Fin Datos

         }


   void Tx_Comando_MENSAJE_fromBUFFER(byte N, byte* buffer, byte subCMD){

	   byte i;
   	   byte chk;
   	   byte n,k;

   	    CMD_MENSAJE.Tx_F = 1;                      // Levanto Bandera de Tx
   		CMD_MENSAJE.Reintentos = reint_3;   // Cargo Cantidad de Reintentos (INFINITOS)
   		getDate();
   		i=0;
   		comandoMENSAJE_Buffer[i++] = 0;   				//fuente de hora
   		comandoMENSAJE_Buffer[i++] = RTC_Date.hora[0];   // HORA
   		comandoMENSAJE_Buffer[i++] = RTC_Date.hora[1];   // MINUTOS
   		comandoMENSAJE_Buffer[i++] = RTC_Date.hora[2];   // SEGUNDOS
   		comandoMENSAJE_Buffer[i++] = RTC_Date.fecha[0];  // DIA
   		comandoMENSAJE_Buffer[i++] = RTC_Date.fecha[1];  // MES
   		comandoMENSAJE_Buffer[i++] = RTC_Date.fecha[2];  // AÑO
   		comandoMENSAJE_Buffer[i++] = subCMD;   			 //tipo de mensaje

   	   k=i;
   	   n = N-1;
       while(i<n+k){
   		comandoMENSAJE_Buffer[i] = RxDA_buffer[i-k+2];
   		i++;
   	   }
   	   CMD_MENSAJE.N = N_CMD +i;

   	   comandoMENSAJE_Buffer[i] = fin_datos_msb;  // Fin Datos
   	   comandoMENSAJE_Buffer[i+1] = fin_datos_lsb;// Fin Datos
	}


   void Tx_Comando_MENSAJE(byte subCMD){

		tDATE datePrueba;
		uint8_t i,n,chk,exit;
		uint16_t k;

		CMD_MENSAJE.Tx_F = 1;                      // Levanto Bandera de Tx
		CMD_MENSAJE.Reintentos = reint_3;   // Cargo Cantidad de Reintentos (INFINITOS)

		i=0;
		comandoMENSAJE_Buffer[i++] = 0;   				//fuente de hora
		comandoMENSAJE_Buffer[i++] = RTC_Date.hora[0];   // HORA
		comandoMENSAJE_Buffer[i++] = RTC_Date.hora[1];   // MINUTOS
		comandoMENSAJE_Buffer[i++] = RTC_Date.hora[2];   // SEGUNDOS
		comandoMENSAJE_Buffer[i++] = RTC_Date.fecha[0];  // DIA
		comandoMENSAJE_Buffer[i++] = RTC_Date.fecha[1];  // MES
		comandoMENSAJE_Buffer[i++] = RTC_Date.fecha[2];  // AÑO
		comandoMENSAJE_Buffer[i++] = subCMD;   			 //tipo de mensaje

		exit = 0;
		if(subCMD == MENSAJE1){
			k = string_copy_returnN(&comandoMENSAJE_Buffer[i],"Vehiculo en movimiento");
			i=i+k;
			exit = 1;
		}

		if(subCMD == MENSAJE2 && !exit){
			k = string_copy_returnN(&comandoMENSAJE_Buffer[i],"Espere unos segundos para pasar a LIBRE");
			i=i+k;
			exit = 1;
		}

		if(subCMD == MENSAJE3 && !exit){
			k = string_copy_returnN(&comandoMENSAJE_Buffer[i],"Ya puede pasar a LIBRE");
			i=i+k;
			exit = 1;
		}

		if(subCMD == MENSAJE4 && !exit){
					k = string_copy_returnN(&comandoMENSAJE_Buffer[i],"Tarifa INVALIDA");
					i=i+k;
					exit = 1;
		}

		if(subCMD == MENSAJE5 && !exit){
					k = string_copy_returnN(&comandoMENSAJE_Buffer[i],"Tarifa NO PROGRAMADA");
					i=i+k;
					exit = 1;
		}

		if(subCMD == MENSAJE6 && !exit){
			exit = 1;
		}

		if(subCMD == MENSAJE7 && !exit){

			uint16_t aux16; uint32_t aux32; uint8_t buffer_aux[20];
            uint8_t existe_sesion_iniciada;
            uint32_t kmLIBRE, kmOCUPADO;
			uint32_t datosMOV[1][2];


				iniTURNO_ptr = &iniTURNO;	finTURNO_ptr = &finTURNO;

				existe_sesion_iniciada = REPORTES_getSesions(sesion_ptrs, max_turnosReporte);        				// Obtengo punteros a todos las sesiones
				if(existe_sesion_iniciada == 0xff){
						k = string_copy_returnN(&comandoMENSAJE_Buffer[i],"Aun no tiene viajes en este turno");
						i=i+k;
						exit = 1;
				}else{
					TICKET_PARCIAL_setFin(sesion_ptrs[MENU_REPORTE_TURNO_index], finTURNO_ptr);   // Puntero a inicio de turno seleccionado


					//date inicio turno
					k = string_copy_returnN(&comandoMENSAJE_Buffer[i],"\Inicio:    "); //7 espacios
					i=i+k;
					comandoMENSAJE_Buffer[i++] = 0x0D;
					comandoMENSAJE_Buffer[i++] = 0x0A;
					date_to_string(&buffer_aux, finTURNO_ptr->date);
					//preparar_print (finTURNO_ptr->date, 0, &buffer_aux, 0);
					k = string_copy_returnN(&comandoMENSAJE_Buffer[i],&buffer_aux);
					i=i+k;

					//Nro Turno
					k = string_copy_returnN(&comandoMENSAJE_Buffer[i],"\nTurno:    "); //7 espacios
					i=i+k;
					preparar_print (finTURNO_ptr->nroTurno, 0, &buffer_aux, 0);
					k = string_copy_returnN(&comandoMENSAJE_Buffer[i],&buffer_aux);
					i=i+k;

					//calc kmLIB, KmOCUP
					(void)REPORTES_cal_kml_kmo(datosMOV);
					kmLIBRE = datosMOV[0][0];
					kmOCUPADO = datosMOV[0][1];

					//kmLIB
					k = string_copy_returnN(&comandoMENSAJE_Buffer[i],"\nKmLib:    "); //15 espacios
					i=i+k;
					preparar_print (kmLIBRE, 1, &buffer_aux, 0);
					k = string_copy_returnN(&comandoMENSAJE_Buffer[i],&buffer_aux);
					i=i+k;

					//kmOCUP
					k = string_copy_returnN(&comandoMENSAJE_Buffer[i],"\nKmOcup:"); //15 espacios
					i=i+k;
					preparar_print (kmOCUPADO, 1, &buffer_aux, 0);
					k = string_copy_returnN(&comandoMENSAJE_Buffer[i],&buffer_aux);
					i=i+k;

					//viajes
					k = string_copy_returnN(&comandoMENSAJE_Buffer[i],"\nViajes:   "); //15 espacios
					i=i+k;
					aux16 = getViajes_Parcial();
					//aux16 = aux16 + 1;
					preparar_print (aux16, 0, &buffer_aux, 0);
					k = string_copy_returnN(&comandoMENSAJE_Buffer[i],&buffer_aux);
					i=i+k;

					//REC PARCIAL
					k = string_copy_returnN(&comandoMENSAJE_Buffer[i],"\nRecaud:    ");
					i=i+k;
					aux32 = getRecaudacion_Parcial();
					if(ESTADO_RELOJ == COBRANDO){
						//solo si se llama en cobrando se le suma valor de viaje(porq. todavia no se guardo en la tabla)
						//aux32 = aux32 + VALOR_VIAJE;
					}
					uint8_t puntoDECIMAL;
					if(TARIFA_PESOS){
						//muestra importe
						puntoDECIMAL = PUNTO_DECIMAL;
					}else{
						//muestra ficha
						puntoDECIMAL = 3;
					}
					//preparar_print_poneCOMA (aux32, puntoDECIMAL, &buffer_aux, 0 );
					preparar_print (aux32, puntoDECIMAL, &buffer_aux, 0);
					k = string_copy_returnN(&comandoMENSAJE_Buffer[i],&buffer_aux);
					i=i+k;
					exit = 1;
				}
			}

		if(subCMD == MENSAJE8 && !exit){
			k = string_copy_returnN(&comandoMENSAJE_Buffer[i],"Debe realizar al menos un viaje en este turno, para poder finalizarlo");
			i=i+k;
			exit = 1;
		}

		if(subCMD == MENSAJE9 && !exit){
				k = string_copy_returnN(&comandoMENSAJE_Buffer[i],"Ha finalizado el turno con exito, y ha iniciado un nuevo turno");
				i=i+k;
				exit = 1;
		}

		if(subCMD == MENSAJE10 && !exit){
				k = string_copy_returnN(&comandoMENSAJE_Buffer[i],"No puede pasar a Cobrando en este estado del reloj");
				i=i+k;
				exit = 1;
		}

		if(subCMD == MENSAJE11 && !exit){
				k = string_copy_returnN(&comandoMENSAJE_Buffer[i],"Pase a Ocupado con tarifa automatica");
				i=i+k;
				exit = 1;
		}

		if(subCMD == MENSAJE12 && !exit){
				k = string_copy_returnN(&comandoMENSAJE_Buffer[i],"CIERRE EL TURNO, se esta quedando sin espacio para guardar reportes.");
				i=i+k;
				exit = 1;
		}

		if(subCMD == MENSAJE13 && !exit){
				k = string_copy_returnN(&comandoMENSAJE_Buffer[i],"TURNO CERRADO, por falta de espacio para reportes");
				i=i+k;
				exit = 1;
		}


		if(subCMD == MENSAJE14 && !exit){
				k = string_copy_returnN(&comandoMENSAJE_Buffer[i],"Sincronizando el Reloj");
				i=i+k;
				exit = 1;
		}

		if(subCMD == MENSAJE15 && !exit){
					k = string_copy_returnN(&comandoMENSAJE_Buffer[i],"Tipo de equipo o reloj desconocido, verifique programacion de reloj");
					i=i+k;
					exit = 1;
		}


		CMD_MENSAJE.N = N_CMD + i;

		comandoMENSAJE_Buffer[i] = fin_datos_msb;  // Fin Datos
		comandoMENSAJE_Buffer[i+1] = fin_datos_lsb;// Fin Datos

   }


   /* TRANSMITIR RESPUESTA ESPONTANEAMENTE */
     /****************************************/

   void TxRta_conDATOS (byte dato){
         // A veces quiere transmitir respuesta CON DATOS, para
         // ello, uso esta rutina.
         RESP_DATOS_buffer[0]= dato;
         RESP_DATOS_buffer[1]= fin_datos_msb;
         RESP_DATOS_buffer[2]= fin_datos_lsb;

         Tabla_ComandosTx[OFFSET_RTA]->N = 2;       // Longitud de Respuesta Simple
         Tabla_ComandosTx[OFFSET_RTA]->buffer = &RESP_DATOS_buffer;   // No lleva Buffer (Rta Simple)

         Tabla_ComandosTx[OFFSET_RTA]->Tx_F = 1;        // Levanto Bandera de Tx Respuesta
       }


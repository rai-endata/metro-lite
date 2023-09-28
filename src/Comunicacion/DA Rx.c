
#include "DA Rx.h"
#include "main.h"
#include "print UART.h"
#include "DA Communication.h"
#include "DA Define Commands.h"
#include "Manejo de Buffers.h"
#include "stddef.h"
#include "Timer.h"
#include "Reloj.h"
//#include "Param Reloj.h"
#include "Parametros Reloj.h"

#include "Ticket Viaje.h"
#include "Ticket Turno.h"
#include "odometro.h"
#include "buzzer.h"
#include "Reportes.h"
#include "tipo de equipo.h"
#include "printDEBUG.h"
#include "Bluetooth.h"
#include "Lista Comandos.h"
#include "ConsultaTurno.h"
#include "Comandos sin conexion a central.h"
#include "DA Define Commands.h"
#include "Display-7seg.h"
#include "Reportes.h"
#include ".\Cx - Visor Android\Rx-VA.h"
#include "Air Update.h"
#include "Constantes.h"


//#include "usart1.h"

/*********************************************************************************************/
/* ESTRUCTURAS */
/***************/
  typedef void (*typeTABLA_RxCMD)(byte* rx_ptr);
  typedef void (*typeTABLA_RxRTA)(byte* rx_ptr);

  typedef enum{
           OKEY,                           // No hay Error
           ERROR_TO,                       // Se cumplio el TimeOut del Lazo
           ERROR_NyDF0A,				   // No coincide el N con el DF0A
           ERROR_Nmin,                     // El N es muy chico (menor al minimo)
           ERROR_N_NULO,				   // El N es NULO
           ERROR_waitTxRta,                // Espero a finalizar la Transmision de la Rta anterior
           ERROR_noRx                      // No recibi nada
  }errorTYPE;

/*********************************************************************************************/
/* PROTOTIPOS */
/**************/
  	static void Rx_NONE (byte* Rx_data_ptr);
  	static	void procesar_CMDS_TRANSPARENTES(byte* Rx_data_ptr);
  	static void procesar_respCMDS_TRANSPARENTES(byte* Rx_data_ptr);
	static errorTYPE DA_chkRx (void);
	static errorTYPE DA_chkRxLength (void);
	static errorTYPE DA_extractRxData (byte* buffer);
	static	void DA_RxData_procesado (void);
	static void DA_newGET_inGPA (byte* GETptr);
	static void DA_incGPAidx (byte* idxPTR);

	static void borrarTABLA_REPORTES_BUFFER_Rx (byte* Rx_data_ptr);
	static void setREGISTRO_TABLA_Rx (byte* Rx_data_ptr);
	static void print_and_wait(byte* bufferTx, uint16_t i);
	static void writeREG_TABLA(byte* ptrTABLA, byte dato);
	static void Leer_reporteTURNO(byte* Rx_data_ptr);
	static void Leer_reporteTURNO_PARCIAL(byte* Rx_data_ptr);
	static void READandPRINT(byte** ptrptrTABLA, byte tipo);
	static void Print_turno (byte* Rx_data_ptr);
	static void Rx_Borrar_toda_la_eeprom (byte* Rx_data_ptr);
	static void UpdateSuccess_Rx (byte* Rx_data_ptr);

	byte RxDA_buffer[255];                    // Buffer de Recepcion de datos desde la Central

	#define dim_AirRx               250
	#define dim_AirRxGPA            5
	//#define DA_Rx_Nmin             4           // Menor longitud a recibir por DA (N-MOVH-MOVL-CMD)
	#define DA_Rx_Nmin             2           // Menor longitud a recibir por DA (N-CMD)

	static byte  DA_RxBuffer[dim_AirRx];       // Buffer de Recepciones de Aire
	static byte* DA_RxPUTptr;				          // Puntero PUT del Buffer de Rx de Aire
	static byte* DA_RxPUTptr_bkp;              // BackUp del Puntero que pone datos en el buffer de Rx, para reestablecerlo en caso de error de CRC o TO
		#define DA_INCptr_to_RxBuffer(ptr)       inc_ptr(ptr,DA_RxBuffer, (uint16_t) dim_AirRx)

	static byte* DA_RxGPA[dim_AirRxGPA];       // GPA (Get Pointer Array) de Rx de Aire
	//static byte  DA_RxGPAidxGET;               // Indice GET del GPA de Rx de Aire
	static byte  DA_RxGPAidxPUT;               // Indice PUT del GPA de Rx de Aire

	static byte  DA_Rx_cnt;                    // Contador de Recepciones por DA
	    #define DA_decRx_cnt                     (DA_Rx_cnt--)

	static byte* DA_RxGPA[dim_AirRxGPA];       // GPA (Get Pointer Array) de Rx de Aire
	static byte  DA_RxGPAidxGET;               // Indice GET del GPA de Rx de Aire
	static byte  DA_RxGPAidxPUT;               // Indice PUT del GPA de Rx de Aire

	byte CMD_a_RESP;                            // Comando a Responder
		#define TX_F_CMD_A_RESP                 (Tabla_ComandosTx[OFFSET_RTA]->Tx_F)
		#define N_CMD_A_RESP                    (Tabla_ComandosTx[OFFSET_RTA]->N)
		#define BUFFER_CMD_A_RESP               (Tabla_ComandosTx[OFFSET_RTA]->buffer)


	byte longitud_RX;

	 /* COMANDOS DE RECEPCION VACIOS */
	  /********************************/
	  #define Rx_comando_00     Rx_NONE
	  #define Rx_comando_01     Rx_NONE
	  #define Rx_comando_02     Rx_NONE
	  #define Rx_comando_03     Rx_NONE
	  #define Rx_comando_04     Rx_NONE
	  #define Rx_comando_05     Rx_NONE
	  #define Rx_comando_06     Rx_NONE
	  #define Rx_comando_07     Rx_NONE
	  #define Rx_comando_08     Rx_NONE
	  #define Rx_comando_09     Rx_NONE
	  #define Rx_comando_0A     Rx_NONE
	  #define Rx_comando_0B     Rx_NONE
	  #define Rx_comando_0C     Rx_NONE
	  #define Rx_comando_0D     Rx_NONE
	  #define Rx_comando_0E     Rx_NONE
	  #define Rx_comando_0F     Rx_NONE

	  #define Rx_comando_10     Rx_NONE
	  #define Rx_comando_11     Rx_NONE
	  #define Rx_comando_12     Rx_NONE
	  #define Rx_comando_13     Rx_NONE
	  #define Rx_comando_14     Rx_NONE
	  #define Rx_comando_15     Rx_NONE
	  #define Rx_comando_16     Rx_NONE
	  #define Rx_comando_17     Rx_NONE
	  #define Rx_comando_18     Rx_NONE
	  #define Rx_comando_19     Rx_NONE
	  #define Rx_comando_1A     Rx_NONE
	  #define Rx_comando_1B     Rx_NONE
	  #define Rx_comando_1C     Rx_NONE
	  #define Rx_comando_1D     Rx_NONE
	  #define Rx_comando_1E     Rx_NONE
	  #define Rx_comando_1F     Rx_NONE

	  #define Rx_comando_20     Rx_NONE
	  #define Rx_comando_21     Rx_NONE
	  #define Rx_comando_22     Rx_NONE
	  #define Rx_comando_23     Rx_NONE
	  #define Rx_comando_24     Rx_NONE
	  #define Rx_comando_25     Rx_NONE
	  #define Rx_comando_26     Rx_NONE
	  #define Rx_comando_27     Rx_NONE
	  #define Rx_comando_28     Rx_NONE
	  #define Rx_comando_29     Rx_NONE
	  #define Rx_comando_2A     Rx_NONE
	  #define Rx_comando_2B     Rx_NONE
	  #define Rx_comando_2C     Rx_NONE
	  #define Rx_comando_2D     Rx_NONE
	  #define Rx_comando_2E     Rx_NONE
	  #define Rx_comando_2F     Rx_NONE

	  #define Rx_comando_30     Rx_NONE
	  #define Rx_comando_31     Rx_NONE
	  #define Rx_comando_32     Rx_NONE
	  #define Rx_comando_33     Rx_NONE
	  #define Rx_comando_34     Rx_NONE
	  #define Rx_comando_35     Rx_NONE
	  #define Rx_comando_36     Rx_NONE
	  #define Rx_comando_37     Rx_NONE
	  #define Rx_comando_38     Rx_NONE
	  #define Rx_comando_39     Rx_NONE
	  #define Rx_comando_3A     Rx_NONE
	  #define Rx_comando_3B     Rx_NONE
	  #define Rx_comando_3C     Rx_NONE
	  #define Rx_comando_3D     Rx_NONE
	  #define Rx_comando_3E     Rx_NONE
	  #define Rx_comando_3F     Rx_NONE

	  #define Rx_comando_40     Rx_NONE
	  #define Rx_comando_41     Rx_NONE
	  #define Rx_comando_42     Rx_NONE
	  #define Rx_comando_43     Rx_NONE
	  #define Rx_comando_44     Rx_NONE
	  #define Rx_comando_45     Rx_NONE
	  #define Rx_comando_46     Rx_NONE
	  #define Rx_comando_47     Rx_NONE
	  #define Rx_comando_48     Rx_NONE
	  #define Rx_comando_49     Rx_NONE
	  #define Rx_comando_4A     Rx_NONE
	  #define Rx_comando_4B     Rx_NONE
	  #define Rx_comando_4C     Rx_NONE
	  #define Rx_comando_4D     Rx_NONE
	  #define Rx_comando_4E     Rx_NONE
	  #define Rx_comando_4F     Rx_NONE

	  #define Rx_comando_50     Rx_NONE
	  #define Rx_comando_51     Rx_NONE
	  #define Rx_comando_52     Rx_NONE
	  #define Rx_comando_53     Rx_NONE
	  #define Rx_comando_54     Rx_NONE
	  #define Rx_comando_55     Rx_NONE
	  #define Rx_comando_56     Rx_NONE
	  #define Rx_comando_57     Rx_NONE
	  #define Rx_comando_58     Rx_NONE
	  #define Rx_comando_59     Rx_NONE
	  #define Rx_comando_5A     Rx_NONE
	  #define Rx_comando_5B     Rx_NONE
	  #define Rx_comando_5C     Rx_NONE
	  #define Rx_comando_5D     Rx_NONE
	  #define Rx_comando_5E     Rx_NONE
	  #define Rx_comando_5F     Rx_NONE

	  #define Rx_comando_60     Rx_NONE
	  #define Rx_comando_61     Rx_NONE
	  #define Rx_comando_62     Rx_NONE
	  #define Rx_comando_63     Rx_NONE
	  #define Rx_comando_64     Rx_NONE
	  #define Rx_comando_65     Rx_NONE
	  #define Rx_comando_66     Rx_NONE
	  #define Rx_comando_67     Rx_NONE
	  #define Rx_comando_68     Rx_NONE
	  #define Rx_comando_69     Rx_NONE
	  #define Rx_comando_6A     Rx_NONE
	  #define Rx_comando_6B     Rx_NONE
	  #define Rx_comando_6C     Rx_NONE
	  #define Rx_comando_6D     Rx_NONE
	  #define Rx_comando_6E     Rx_NONE
	  #define Rx_comando_6F     Rx_NONE

	  #define Rx_comando_70     Rx_NONE
	  #define Rx_comando_71     Rx_NONE
	  #define Rx_comando_72     Rx_NONE
	  #define Rx_comando_73     Rx_NONE
	  #define Rx_comando_74     Rx_NONE
	  #define Rx_comando_75     Rx_NONE
	  #define Rx_comando_76     Rx_NONE
	  #define Rx_comando_77     Rx_NONE
	  #define Rx_comando_78     Rx_NONE
	  #define Rx_comando_79     Rx_NONE
	  #define Rx_comando_7A     Rx_NONE
	  #define Rx_comando_7B     Rx_NONE
	  #define Rx_comando_7C     Rx_NONE
	  #define Rx_comando_7D     Rx_NONE
	  #define Rx_comando_7E     Rx_NONE
	  #define Rx_comando_7F     Rx_NONE


	/*********************************************************************************************/
	/* CONSTANTES */
	/**************/

	  // Tabla de Rx de COMANDOS
	  const typeTABLA_RxCMD Tabla_RxDA[]={
		Rx_comando_00,                    // 0x00 - Acepta Viaje
		Inicio_TURNO_Rx,
		Rx_comando_02,
		Pedido_Pase_LIBRE_Rx,
		Pedido_Pase_OCUPADO_Rx,
		Pedido_Pase_COBRANDO_Rx,
		Inicio_EPERA_Rx,
		Fin_EPERA_Rx,
		Impresion_Rx,
		ConsultaTarifas,
		ConsultaEstado,
		Rx_comando_0B,
		Entra_DESCANSO_Rx,
		Sale_DESCANSO_Rx,
		Pedido_reportePARCIAL_Rx,
	    Rx_comando_0F,

		Rx_appConectada_aCentral,
		Rx_appDesconectada_deCentral,
	    Rx_comando_12,
		Rx_comando_13,
		Rx_comando_14,
	    Rx_comando_15,
	    Rx_comando_16,
		Rx_comando_17,
		Rx_comando_18,
		Rx_comando_19,
		Rx_comando_1A,
		Rx_comando_1B,
		Rx_comando_1C,
	    Rx_comando_1D,
		Rx_comando_1E,
		Rx_comando_1F,

		Rx_comando_20,
	    Rx_comando_21,
	    Rx_comando_22,
		Pedido_Pase_LIBRE_SC_Rx,
		Pedido_Pase_OCUPADO_SC_Rx,
		Pedido_Pase_COBRANDO_SC_Rx,
	    Rx_comando_26,
	    Rx_comando_27,
	    Rx_comando_28,
	    Rx_comando_29,
		Rx_comando_2A,
		Rx_comando_2B,
		Rx_comando_2C,
		Rx_comando_2D,
		Rx_comando_2E,
		Rx_comando_2F,

		ASIGNAR_Rx,
		quitarASIGNADO_Rx,
	    Rx_comando_32,
		Rx_comando_33,
		Rx_comando_34,
	    Rx_comando_35,
	    Rx_comando_36,
	    Rx_comando_37,
	    Rx_comando_38,
	    Rx_comando_39,
	    Rx_comando_3A,
	    Rx_comando_3B,
	    Rx_comando_3C,
	    Rx_comando_3D,
	    Rx_comando_3E,
		Rx_comando_3F,

		Comando_TRANSPARENTE_Rx,
		Rx_comando_41,
	    Rx_comando_42,
	    Rx_comando_43,
	    Rx_comando_44,
	    Rx_comando_45,
	    Rx_comando_46,
	    Rx_comando_47,
	    Rx_comando_48,
	    Rx_comando_49,
	    Rx_comando_4A,
	    Rx_comando_4B,
	    Rx_comando_4C,
	    Rx_comando_4D,
	    Rx_comando_4E,
	    Rx_comando_4F,

		Rx_comando_50,
	    Rx_comando_51,
	    Rx_comando_52,
	    Rx_comando_53,
	    Rx_comando_54,
	    Rx_comando_55,
	    Rx_comando_56,
	    Rx_comando_57,
	    Rx_comando_58,
	    Rx_comando_59,
	    Rx_comando_5A,
	    Rx_comando_5B,
	    Rx_comando_5C,
	    Rx_comando_5D,
	    Rx_comando_5E,
	    Rx_comando_5F,

		AIR_UPDATE_Rx,
		AIR_READ_Rx,
	    Rx_comando_62,
	    Rx_comando_63,
	    Rx_comando_64,
	    Rx_comando_65,
	    Rx_comando_66,
	    Rx_comando_67,
	    Rx_comando_68,
	    Rx_comando_69,
	    Rx_comando_6A,
	    Rx_comando_6B,
	    Rx_comando_6C,
	    Rx_comando_6D,
	    Rx_comando_6E,
	    Rx_comando_6F,

	    Rx_comando_70,
	    Rx_comando_71,
	    Rx_comando_72,
	    Rx_comando_73,
	    Rx_comando_74,
	    Rx_Borrar_toda_la_eeprom,
		Leer_reporteTURNO_PARCIAL,
	    Print_turno,
		Leer_reporteTURNO,
		setREGISTRO_TABLA_Rx,
		borrarTABLA_REPORTES_BUFFER_Rx,
		Escribir_BUFFER_Rx,
		Escribir_BYTE_Rx,
	    Leer_REGISTRO_Rx,
		Leer_EEPROM_Rx,
	    Rx_comando_7F
	  };


	  // Tabla de Rx de RESPUESTAS
	  const typeTABLA_RxRTA Tabla_RxRTA_DA[]={
	    Rx_comando_00,
	    Rx_comando_01,
	    Rx_comando_02,
	    //Libre_RxRTA,                        // 0x83 (Rta 0x03) Libre
	    //Ocupado_RxRTA,                      // 0x84 (Rta 0x04) Ocupado
		//Cobrando_RxRTA,
		Rx_comando_03,
		Rx_comando_04,
		Rx_comando_05,
		Rx_comando_06,
	    Rx_comando_07,
	    Rx_comando_08,
		Rx_comando_09,
		Rx_comando_0A,
		UpdateSuccess_Rx,
	    Rx_comando_0C,
	    Rx_comando_0D,
	    Rx_comando_0E,
	    Rx_comando_0F,

	    Rx_comando_10,
	    Rx_comando_11,
	    Rx_comando_12,
	    Rx_comando_13,
	    Rx_comando_14,
	    Rx_comando_15,
	    Rx_comando_16,
	    Rx_comando_17,
		Rx_comando_18,
		Rx_comando_19,
	    Rx_comando_1A,
	    Rx_comando_1B,
	    Rx_comando_1C,
	    Rx_comando_1D,
	    Rx_comando_1E,
	    Rx_comando_1F,

	    Rx_comando_20,
	    Rx_comando_21,
	    Rx_comando_22,
	    Rx_comando_23,
	    Rx_comando_24,
	    Rx_comando_25,
	    Rx_comando_26,
	    Rx_comando_27,
	    Rx_comando_28,
	    Rx_comando_29,
	    Rx_comando_2A,
	    Rx_comando_2B,
	    Rx_comando_2C,
	    Rx_comando_2D,
	    Rx_comando_2E,
	    Rx_comando_2F,

		Rx_comando_30,
	    Rx_comando_31,
	    Rx_comando_32,
	    Rx_comando_33,
	    Rx_comando_34,
	    Rx_comando_35,
	    Rx_comando_36,
	    Rx_comando_37,
	    Rx_comando_38,
		Rx_comando_39,
	    Rx_comando_3A,
	    Rx_comando_3B,
	    Rx_comando_3C,
	    Rx_comando_3D,
		Rx_comando_3E,
		Rx_comando_3F,

	    Rx_comando_40,
	    Rx_comando_41,
	    Rx_comando_42,
	    Rx_comando_43,
	    Rx_comando_44,
	    Rx_comando_45,
	    Rx_comando_46,
	    Rx_comando_47,
	    Rx_comando_48,
	    Rx_comando_49,
	    Rx_comando_4A,
	    Rx_comando_4B,
	    Rx_comando_4C,
	    Rx_comando_4D,
	    Rx_comando_4E,
	    Rx_comando_4F,

		Rx_comando_50,
		Rx_comando_51,
	    Rx_comando_52,
	    Rx_comando_53,
	    Rx_comando_54,
	    Rx_comando_55,
	    Rx_comando_56,
	    Rx_comando_57,
        Rx_comando_58,
	    Rx_comando_59,
	    Rx_comando_5A,
	    Rx_comando_5B,
	    Rx_comando_5C,
	    Rx_comando_5D,
	    Rx_comando_5E,
	    Rx_comando_5F,

	    Rx_comando_60,
	    Rx_comando_61,
	    Rx_comando_62,
	    Rx_comando_63,
	    Rx_comando_64,
	    Rx_comando_65,
	    Rx_comando_66,
	    Rx_comando_67,
	    Rx_comando_68,
	    Rx_comando_69,
	    Rx_comando_6A,
	    Rx_comando_6B,
	    Rx_comando_6C,
	    Rx_comando_6D,
	    Rx_comando_6E,
	    Rx_comando_6F,

	    Rx_comando_70,
	    Rx_comando_71,
	    Rx_comando_72,
	    Rx_comando_73,
	    Rx_comando_74,
	    Rx_comando_75,
	    Rx_comando_76,
	    Rx_comando_77,
	    Rx_comando_78,
	    Rx_comando_79,
	    Rx_comando_7A,
	    Rx_comando_7B,
	    Rx_comando_7C,
	    Rx_comando_7D,
	    Rx_comando_7E,
	    Rx_comando_7F
	  };

	/*********************************************************************************************/
	/* RUTINAS */
	/***********/

	/* DUMMY - PARA RELLENAR LA TABLA DE RxCENTRAL */
	/***********************************************/
	static void Rx_NONE (byte* Rx_data_ptr){
	}

	  /* Proceso de DATOS  recibidos del VISOR ANDROID */
	  /*************************************************/

	void RxDA_process (void){
	      // Esta rutina procesa los datos recibidos desde el visor android
	      // - Si lo datos tienen algun error, se descartan
	      // Si recibí algo, mas alla de que condición haya sido, avanzo el indice
	      // GET de datos, para indicar que los datos recibidos fueron procesados
		errorTYPE error;
		volatile byte Rx_cmd;       // Necesito que no lo optmice, xq sino se confunde el tontito

		//DEPEDIENDO DEL TIPO DE EQUIPO MIRA O NO COMANDOS RECIBIDOS DEL DISPOSITIVO ANDROID
			error = DA_chkRx();             // Me fijo si recibi algo
			if ((error == OKEY) ){
				error = DA_extractRxData(RxDA_buffer);     // Extraigo datos en buffer
				if (error == OKEY){
					// Luego extraigo comando recibido, me fijo si es comando
					// o respuesta, y lo proceso.
					// Como en el buffer de recepción tiene el siguiente formato:
					//
					//      | N | CMD | DATOS |
					//
					// El comando se encuentra en la 2da posición del buffer
					//setTO_Bluetooth(TO_BLUETOOTH_3);
					Rx_cmd = RxDA_buffer[1];        								//Extraigo comando
					if(EQUIPO_METRO_LITE_RELOJ_INTERNO ||
					  (((EQUIPO_METRO_BLUE) || (EQUIPO_METRO_LITE_RELOJ_BANDERITA)) && !CMD_ACTIONS)){
						//prueba (envia comandos recibidos por puerto serie)
						//Enviar_BUFFER_Rx_porPuertoSerie ((byte*) RxDA_buffer, N_Rx_App);
						  if (Rx_cmd < 0x80){
							  // Recibí un comando de la Central -> Debo Transmitir Respuesta
							  // Asumo Rta Simple (sin datos) => Seteo N y Buffer.
							  // Si no fuera Rta simple, la rutina de Rx del comando en cuestión debe
							  // setear la longitud y el buffer de Rta

							  N_CMD_A_RESP = N_CMD;         						// Longitud de Respuesta Simple
							  BUFFER_CMD_A_RESP = NULL;     						// No lleva Buffer (Rta Simple)
							  Tabla_RxDA[Rx_cmd](RxDA_buffer);      			// Proceso Comando
							  CMD_a_RESP = Rx_cmd;        							// Qué voy a Responder
							  if(Rx_cmd != cmdConsultaTarifas ){
								 //if(Rx_cmd != 0x0B){
								  TX_F_CMD_A_RESP = 1;        							// Levanto Bandera de Tx Respuesta
								 //}
							  }
						  }else{
							// Recibí la respuesta a un comando
							pauseTx = 0;
							Rx_cmd -= 0x80;
							//me fijo si la respuesta fue de un comando de reloj
							if(chkCmd_Reloj(Rx_cmd)){
								//cuando se transmite comando sin conexion
								//espero la respuesta para transmitir el sgte.
								esperarRespuesta_cmdReloj = 0;
							}

							anularTx_cmd(Rx_cmd);           				// Anulo la transmisión del comando
							Tabla_RxRTA_DA[Rx_cmd](RxDA_buffer);  			// Proceso Respuesta
						  }
					}else{
						Tx_Comando_MENSAJE(EQUIPO_O_RELOJ_DESCONOCIDO);
					}

				}
				// Si recibí algo, mas alla de que sea  coherente o no, avanzo
				// el GET dentro del GPA, xq ya procese lo que acabo de recibir
				DA_RxData_procesado();             // Dato procesado
			  }

	}



	    /* CHEQUEAR SI SE RECIBIERON DATOS DE DA */
	    /*******************************************/

		static errorTYPE DA_chkRx (void){
			// Rutina que me dice recibí un comando o no. Para ello se fija que el contador
			// de recepciones sea mayor a cero.
			// Pero no basta con esto, además:
			//  - Verifica que el N de los datos recibidos se condiga con el fin de datos,
			//    y así saber que tenemos una cadena coherente; y que además, ese N sea
			//    mayor o igual a un mínimo
			//
			errorTYPE error;

			error = ERROR_noRx;                           // Asumo que no recibí nada

			if (DA_Rx_cnt > 0){
				error = DA_chkRxLength();                // Verificar que se trate de una cadena coherente
			}
			return(error);

		}


		/* CHEQUEO DE <N> CON <DF0A> DE DATOS RECIBIDOS aka CHEQUEO DE LONGITUD DE DATOS */
		/*********************************************************************************/

		static errorTYPE DA_chkRxLength (void){               // Rutina que verifica que en el buffer de Rx del Modem, se condigan el N con el DF0A.
		  // es N-MOVH-MOVL-CMD
			byte* GETptr;
			byte  nextGPAidxGET;
			byte* nextGETptr;
			byte N;
			uint16_t cmd_length;
			errorTYPE error;

			error = OKEY;

			GETptr = DA_RxGPA[DA_RxGPAidxGET];      // Extraigo GET de GPA
			N = * GETptr;                             // Extraigo N

			if (N < DA_Rx_Nmin){
			  error = ERROR_Nmin;                       // ERROR: El N es menor al minimo

			}else{
			  // El N es mayor o igual al minimo => voy a verificar el N con el DF0A. Para
			  // eso me valgo del proximo GET en el GPA. Calculo la diferencia entre ambos
			  // punteros y le resto 2 por el DF0A y lo comparo con el N, si son iguales,
			  // es porque el N es coherente con el DF0A
			  nextGPAidxGET = DA_RxGPAidxGET;        // Duplico idxGET actual
			  DA_incGPAidx(&nextGPAidxGET);          // Lo incremento para pasar a proximo dato
			  nextGETptr = DA_RxGPA[nextGPAidxGET];  // Extraigo GET de GPA

			  // Calculo la diferencia entre el GET actual y el proximo
			  cmd_length = diferencia_punteros(GETptr,nextGETptr, dim_AirRx);
			  cmd_length -= 2;                        // Resto 2 x el DF0A

			  if (cmd_length != N){
				error = ERROR_NyDF0A;                   // La longitud no coincide con el fin de datos
			  }
			}
			return(error);
		}

		/* EXTRAER COMANDO RECIBIDO DEL DA */
		/*************************************/
		static errorTYPE DA_extractRxData (byte* buffer){
			// Esta rutina me devuelve en un buffer, el comando recibido por aire, y que se encuentra
			// en la direccion apuntada por el GET pointer del buffer de Rx.
			// Como el primer byte de cada stream recibido es la longitud del mismo, voy a extraer tantos
			// byte como me indique esa longitud.
			// El stream es devuelto en el buffer que se le pasa como argumento. En este buffer, no se va
			// a incluir el numero de movil; sino que los datos se presentaran como | N | CMD | DATA |
			byte* GETptr;
			errorTYPE error;
			byte N, TO_F;

			error = OKEY;                               // Asumo que no va a haber error

			GETptr = DA_RxGPA[DA_RxGPAidxGET];      // Extraigo GET de GPA

			N = *GETptr;                              // Extraigo longitud de datos recibida
			longitud_RX = N;
			DA_INCptr_to_RxBuffer(&GETptr);          // Avanzo puntero GET de Recepción de datos de Aire
			// La longitud de datos recibida incluye al N y al Numero de Móvil, por esta razón
			// es que a la longitud le resto 3.
			//N -= 3;                                   // N sin el N, MOV_H y MOV_L
			N -= 1;                                   // N sin el N,
			*buffer++ = N;										        // Guardo nuevo N en el Buffer

			// Ahora, para no extraer el numero de móvil, avanzo 2 bytes el puntero GET
			//DA_INCptr_to_RxBuffer(&GETptr);          	// Avanzo puntero GET de Recepción de datos de Aire
			//DA_INCptr_to_RxBuffer(&GETptr);          	// Avanzo puntero GET de Recepción de datos de Aire

			TO_F = 0;                                 	// Reseteo Bandera de TimeOut
			dispararTO_lazo();                        	// Disparo Time Out de Lazo
			while ((N > 0) && (!TO_F)){
				TO_F = chkTO_lazo_F();                  // Chequeo bandera de time out de lazo
				N--;

				*buffer++ = *GETptr;                    // Extraigo datos según el puntero GET y los guardo en el buffer
				DA_INCptr_to_RxBuffer(&GETptr);        // Avanzo puntero GET de Recepción de datos de Aire
			}
			detenerTO_lazo();                         	// Detengo Time Out de Lazo

			if (TO_F){
				// ERROR --> Time Out Lazo
				error = ERROR_TO;
			}
			return(error);
		}

	/* INICIALIZAR BUFFER DE Rx POR DA */
	/*************************************/
	void DA_iniRx (void){
		DA_RxGPAidxGET = 0;
		DA_RxGPAidxPUT = 0;
		DA_newGET_inGPA(DA_RxBuffer);

		DA_RxPUTptr = DA_RxBuffer;
		DA_RxPUTptr_bkp = DA_RxPUTptr;
	}

	/* CHEQUEAR SI ESTA EN ALTO LA BANDERA DE TX DE RESPUESTA */
	/**********************************************************/
	byte Rx_DA_chkTxRta_F (void){
	  return(TX_F_CMD_A_RESP);
	}



	/* EXTRAER N DE LA RESPUESTA */
	/*****************************/
	byte Rx_DA_getNRta (void){
	  //return(N_CMD_A_RESP);
	  return(N_CMD_A_RESP);          //-2 porque elimine numero de movil del protocolo
	}


	/* EXTRAER CMD DE LA RESPUESTA */
	/*******************************/
	byte Rx_DA_getCMDRta (void){
	  return(CMD_a_RESP);
	}

	/* EXTRAER BUFFER DE LA RESPUESTA */
	/**********************************/
	byte* Rx_DA_getBufferRta (void){
	  return(BUFFER_CMD_A_RESP);
	}

	/* BAJAR BANDERA DE TX DE RESPUESTA */
	/************************************/
	void Rx_DA_clrTxRta_F (void){
	  TX_F_CMD_A_RESP = 0;
	}

	/* AGREGAR DATO A BUFFER DE Rx DE DA */
	/***************************************/
	void DA_putByte_RxBuffer (byte dato){
	  put_byte(&DA_RxPUTptr, dato, DA_RxBuffer, (uint16_t) dim_AirRx);
	}


	/* AGREGAR PUNTERO PUT A GPA DE Rx DA */
	/****************************************/
	void DA_addRxPUT_toRxGPA (void){
	  DA_RxGPA[DA_RxGPAidxPUT] = DA_RxPUTptr;
	  DA_incGPAidx(&DA_RxGPAidxPUT);      // Incremento PUT
	}

	/* NUEVO PUNTERO GET EN GPA */
	/****************************/
	static void DA_newGET_inGPA (byte* GETptr){
		DA_RxGPA[DA_RxGPAidxPUT] = GETptr;
		DA_incGPAidx(&DA_RxGPAidxPUT);      // Incremento PUT
	}


	/* INCREMENTAR GPA Rx DA */
	/***************************/
	static void DA_incGPAidx (byte* idxPTR){
	// Incrementa PUTNERO (sea GET o PUT) y da la vuelta al buffer de GPA de Rx de Aire
		(*idxPTR)++;
		if ((*idxPTR) == dim_AirRxGPA){
			(*idxPTR) = 0;
		}
	}


	/* GUARDAR BACKUP DEL PUNTERO PUT DE Rx DE DA */
	/************************************************/
	void DA_saveRxPUTptr_BKP (void){
		DA_RxPUTptr_bkp = DA_RxPUTptr;
	}

	/* INCREMENTAR CONTADOR DE RECEPCIONES DE DA */
	/***********************************************/
	void DA_incRxcnt (void){
		DA_Rx_cnt++;
	}

	/* DATO RECIBIDO POR DA, PROCESADO */
	/***********************************/

	static	void DA_RxData_procesado (void){
		DA_incGPAidx(&DA_RxGPAidxGET);    			// Avanzo GET del GPA de Rx
		DA_decRx_cnt;                      			// Decremento Contador de Datos Recibidos, xq fueron procesados
	}

	/* CALCULAR ESPACIO DISPONIBLE EN BUFFER Rx DE DA */
	/****************************************************/

	uint16_t chkSpace_onDA_RxBuffer (void){
	  uint16_t space;
	  uint16_t queue_cnt;
	  byte* GET;
	  byte* PUT;

	  GET = DA_RxGPA[DA_RxGPAidxGET];
	  PUT = DA_RxPUTptr;
	  queue_cnt = DA_Rx_cnt;     // Extriago cantidad de encolados

	  if ((PUT == GET) && (queue_cnt != 0)){
		// Si los punteros GET y PUT son iguales y HAY paquetes encolados
		// => No hay mas lugar
		space = 0;

	  }else if (GET > PUT){
		/*
				 +---------+
				 |         |
				 |         |
		  PUT -> |         |
				 |*********|
				 |*********|
				 |*********|
				 |         | -> GET
				 |         |
				 +---------+


			=> SPACE = GET - PUT
		*/
		space = GET - PUT;

	  }else{
		/*
				 +---------+
				 |*********|
				 |*********|
				 |         | -> GET
				 |         |
				 |         |
				 |         |
		  PUT -> |         |
				 |*********|
				 +---------+


			=> SPACE = DIM - (PUT - GET) = DIM - PUT + GET
		*/
		space = dim_AirRx;
		space -= (PUT - GET);
	  }

	  return(space);
	}



	void Leer_EEPROM_Rx (byte* Rx_data_ptr){

		tREG_GENERIC 	bufferDATOS_LEIDOS;
		uint8_t 		bufferTx[2*sizeof(tREG_GENERIC)];
		tREG_GENERIC* 	ptrTABLA;
        uint 			status, i;
        byte* 			INI_TABLA_ptr; byte* auxRX_DATA_ptr;
        uint32_t 		INI_TABLA, auxRX_DATA;
        byte TO_F;
        tREG_GENERIC* fin;

		// El formato de datos de recepcion es

        //    | N | CMD | DATA_1 | DATA_2 | . | . |


        Rx_data_ptr++;               // salto N
		Rx_data_ptr++;               // salto CMD

		fin = (tREG_GENERIC*)FIN_TABLA_REPORTE - 1;
		//apunto a inicio de tabla en eeprom
		ptrTABLA = ADDR_EEPROM_REPORTE;
		dispararTO_lazo();
		while(ptrTABLA <= fin){
			if((NO_TXING_PRINTER && NO_RXING_PRINTER  && NO_ESTA_IMPRIMIENDO)){
				if(checkRANGE(ptrTABLA, FIN_TABLA_REPORTE, chkTO_lazo_F())){break;}		//cuando INI_ptr > FIN_TABLA_REPORTE sale del while
				READandPRINT(&ptrTABLA, REG_generico);
			}
		}
		READandPRINT(&ptrTABLA, REG_generico);
		detenerTO_lazo();
	}


	void Leer_REGISTRO_Rx (byte* Rx_data_ptr){
			//
			// El formato de datos de recepcion es
			//
			//    | N | CMD | DATA_1 | DATA_2 | . | . |
			//
			//
			// Si el bit mas significativo esta seteado (0x80) indica que va a indicar
			// cantidad maxima de FICHAS/PESOS.

			tREG_GENERIC* 	ptrTABLA;

			uint32_t 	aux_TABLA;
			uint8_t* 	aux_ptrTABLA;
			uint8_t*  	aux_Rx_data_ptr;

	        Rx_data_ptr++;               // Extraigo N
			Rx_data_ptr++;               // Extraigo CMD

			aux_Rx_data_ptr = &Rx_data_ptr;
			aux_ptrTABLA = &ptrTABLA;

			//apunto al registro de tabla que quiero leer (con ptrTABLA)
			aux_ptrTABLA[3] = 0;
			aux_ptrTABLA[2] = 0;
			aux_ptrTABLA[1] = *Rx_data_ptr;
			aux_ptrTABLA[0] = *(Rx_data_ptr+1);
			READandPRINT_regTABLA_REPORTES (ptrTABLA, REG_generico);

		}


	static void Print_turno (byte* Rx_data_ptr){
				//
				// El formato de datos de recepcion es
				//
				//    | N | CMD | DATA_1 | DATA_2 | . | . |
				//
				//
				// Si el bit mas significativo esta seteado (0x80) indica que va a indicar
				// cantidad maxima de FICHAS/PESOS.

				tREG_GENERIC* 	ptrTABLA;

				uint32_t 	aux_TABLA;
				uint8_t* 	aux_ptrTABLA;
				uint8_t*  	aux_Rx_data_ptr;
                uint8_t 	nroTURNO, i;
        		uint8_t 		bufferTx[120*sizeof(tREG_GENERIC)];

		        Rx_data_ptr++;               // Extraigo N
				Rx_data_ptr++;               // Extraigo CMD
				nroTURNO = *Rx_data_ptr;

				print_ticket_turno(nroTURNO);
			}



	static void Rx_Borrar_toda_la_eeprom (byte* Rx_data_ptr){
					//
					// El formato de datos de recepcion es
					//
					//    | N | CMD | DATA_1 | DATA_2 | . | . |
					//
					//
					// Si el bit mas significativo esta seteado (0x80) indica que va a indicar
					// cantidad maxima de FICHAS/PESOS.

			        Rx_data_ptr++;               // Extraigo N
					Rx_data_ptr++;               // Extraigo CMD

					borrar_EEPROM();
				}

	void READandPRINT_regTABLA_REPORTES (byte* ptrTABLA, byte tipo){

		uint8_t 		i;
		tREG_GENERIC 	bufferDATOS_LEIDOS;
		uint8_t 		bufferTx[2*sizeof(tREG_GENERIC)];
		uint8_t* 	aux_ptrTABLA;

		aux_ptrTABLA = &ptrTABLA;

		EEPROM_ReadBuffer(&bufferDATOS_LEIDOS, ptrTABLA, sizeof(tREG_GENERIC));

        if(bufferDATOS_LEIDOS.tipo == tipo || tipo == REG_generico ){
    		i=0;
    		bufferTx[i] = aux_ptrTABLA[1];																i=i+1;
    		bufferTx[i] = aux_ptrTABLA[0];																i=i+1;
    		bufferTx[i] = ' ';																			i=i+1;
    		stringCopyN(&bufferTx[i], &bufferDATOS_LEIDOS.idx, sizeof(bufferDATOS_LEIDOS.idx)); 		i=i+sizeof(bufferDATOS_LEIDOS.idx);
    		bufferTx[i] = ' ';				 															i=i+1;
    		stringCopyN(&bufferTx[i], &bufferDATOS_LEIDOS.tipo, sizeof(bufferDATOS_LEIDOS.tipo)); 		i=i+sizeof(bufferDATOS_LEIDOS.tipo);
    		bufferTx[i] = ' ';			 																i=i+1;
    		stringCopyN(&bufferTx[i], &bufferDATOS_LEIDOS.date, sizeof(bufferDATOS_LEIDOS.date));		i=i+sizeof(bufferDATOS_LEIDOS.date);
    		bufferTx[i] = ' ';			 																i=i+1;
    		stringCopyN(&bufferTx[i], &bufferDATOS_LEIDOS.chofer, sizeof(bufferDATOS_LEIDOS.chofer));	i=i+sizeof(bufferDATOS_LEIDOS.chofer);
    		bufferTx[i] = ' ';			 																i=i+1;
    		stringCopyN(&bufferTx[i], &bufferDATOS_LEIDOS.empty, sizeof(bufferDATOS_LEIDOS.empty));		i=i+sizeof(bufferDATOS_LEIDOS.empty);
    		bufferTx[i++] = 0x0D;
    		bufferTx[i++] = 0x0A;

    		print_and_wait((byte*)&bufferTx, i);
        }

	}

	void Escribir_BYTE_Rx (byte* Rx_data_ptr){
				//
				// El formato de datos de recepcion es
				//
				//    | N | CMD | DATA_1 | DATA_2 | . | . |
				//
				//
				// Si el bit mas significativo esta seteado (0x80) indica que va a indicar
				// cantidad maxima de FICHAS/PESOS.


				uint8_t 		bufferTx[2*sizeof(tREG_GENERIC)];
				tREG_GENERIC* 	ptrTABLA;

				byte N, 	cmd,i,dato, status;
				uint32_t 	aux32;
				uint32_t 	aux_TABLA;
				uint8_t*  	aux_Rx_data_ptr;
				uint8_t* 	aux_ptrTABLA;

		        uint16_t* 	aux16_ptr;

		    	uint8_t 	DATOS_LEIDO;
		    	uint8_t err;


		        N 	= *Rx_data_ptr++;               // Extraigo N
				cmd = *Rx_data_ptr++;               // Extraigo CMD

				aux_ptrTABLA = &ptrTABLA;
				//apunto al registro de tabla que quiero leer (con ptrTABLA)
				aux_ptrTABLA[3] = 0;
				aux_ptrTABLA[2] = 0;
				aux_ptrTABLA[1] = *Rx_data_ptr;
				aux_ptrTABLA[0] = *(Rx_data_ptr+1);

				//escribir dato
				//uint32_t prim = __get_PRIMASK();
				//__disable_irq();

				err = EEPROM_WriteByte_irqDisable((uint32_t) ptrTABLA, *(Rx_data_ptr+2));

				//if (!prim) {
			  	//			__enable_irq();
			  	//}
/*
				i = 0;
				aux_TABLA = (uint32_t) ptrTABLA;
				 while(i < sizeof(tREG_GENERIC)){
				 		//EEPROM_WriteByte_irqDisable((uint32_t) ptrEEPROM, 0xff);
				 		EEPROM_WriteByte(aux_TABLA, 0xff);
				 		aux_TABLA++;
				 		i++;
				  }
*/
				//leer dato
				EEPROM_ReadBuffer(&DATOS_LEIDO, ptrTABLA, 1);

				i=0;
				bufferTx[i++] = aux_ptrTABLA[1];
				bufferTx[i++] = aux_ptrTABLA[0];
				bufferTx[i++] = ' ';
				bufferTx[i++] = *(Rx_data_ptr+2);
				bufferTx[i++] = DATOS_LEIDO;
				bufferTx[i++] = err;
				bufferTx[i++] = 0x0D;
				bufferTx[i++] = 0x0A;

				print_and_wait((byte*)&bufferTx, i);
			}

	static void borrarTABLA_REPORTES_BUFFER_Rx (byte* Rx_data_ptr){

		tREG_GENERIC 	bufferDATOS_LEIDOS;
		uint8_t 		bufferTx[2*sizeof(tREG_GENERIC)];
		tREG_GENERIC* 	ptrTABLA;
        byte* 			INI_TABLA_ptr; byte* auxRX_DATA_ptr;
        uint32_t 		INI_TABLA, auxRX_DATA;

        // El formato de datos de recepcion es

        //    | N | CMD | DATA_1 | DATA_2 | . | . |


		mostrar_clear_dsplyEEPROM=1;
		mostrar_t_dsplyT=1;
		on_display_tarifa();
		print_display();

		Rx_data_ptr++;               // salto N
		Rx_data_ptr++;               // salto CMD

		//apunto a inicio de tabla en eeprom
		ptrTABLA = ADDR_EEPROM_REPORTE;
		dispararTO_lazo();
		while(ptrTABLA <= FIN_TABLA_REPORTE){
			if((NO_TXING_PRINTER && NO_RXING_PRINTER  && NO_ESTA_IMPRIMIENDO)){
				writeREG_TABLA(ptrTABLA, 0xFF);
				if(checkRANGE(ptrTABLA, FIN_TABLA_REPORTE, chkTO_lazo_F())){break;}		//cuando INI_ptr > FIN_TABLA_REPORTE sale del while
				READandPRINT(&ptrTABLA, REG_generico);
			}
		}
		detenerTO_lazo();

		mostrar_clear_dsplyEEPROM = 0;
		mostrar_t_dsplyT=0;
		//indexMenu_IniTurno=0;
		on_display_tarifa();
		off_display();

		//inicializa primer encendido
		borrarPrimerEncendido();

		//resetea el equipo
		NVIC_SystemReset();
	}


	static void setREGISTRO_TABLA_Rx (byte* Rx_data_ptr){

			uint8_t i,  dato;
			uint8_t* 	ptrTABLA;
			uint8_t* 	ptrTABLA_ptr;
			byte* Rx_ptr = Rx_data_ptr;

			Rx_data_ptr++;               // salto N
			Rx_data_ptr++;               // salt0 CMD
			dato = *Rx_data_ptr++;
			ptrTABLA_ptr = &ptrTABLA;

			//apunto al registro de tabla que quiero escribit (con ptrTABLA)
			ptrTABLA_ptr[0] = *(Rx_data_ptr+1);
			ptrTABLA_ptr[1] = *(Rx_data_ptr+0);
			ptrTABLA_ptr[2] = 0;
			ptrTABLA_ptr[3] = 0;
			writeREG_TABLA(ptrTABLA, dato);

			//apunto al registro de tabla que quiero leer (con ptrTABLA)
			ptrTABLA_ptr[0] = *(Rx_data_ptr+1);
			ptrTABLA_ptr[1] = *(Rx_data_ptr+0);
			ptrTABLA_ptr[2] = 0;
			ptrTABLA_ptr[3] = 0;
			READandPRINT_regTABLA_REPORTES (ptrTABLA, REG_generico);

		}



	static void writeREG_TABLA(byte* ptrTABLA, byte dato){

		 uint8_t i=0;
		 while(i < sizeof(tREG_GENERIC)){
				//EEPROM_WriteByte_irqDisable((uint32_t) ptrEEPROM, 0xff);
				EEPROM_WriteByte_irqDisable(ptrTABLA, dato);
				ptrTABLA++;
				i++;
		}
	}



	void Escribir_BUFFER_Rx (byte* Rx_data_ptr){
					//
					// El formato de datos de recepcion es
					//
					//    | N | CMD | DATA_1 | DATA_2 | . | . |
					//
					//
					// Si el bit mas significativo esta seteado (0x80) indica que va a indicar
					// cantidad maxima de FICHAS/PESOS.

					uint8_t 		bufferTx[2*sizeof(tREG_GENERIC)];
					tREG_GENERIC* 	ptrTABLA;

					byte N, 	cmd,i,dato;
					uint32_t 	aux32;
					uint32_t 	aux_TABLA;
					uint8_t*  	aux_Rx_data_ptr;
					uint8_t* 	aux_ptrTABLA;
			        uint 		status;
			        uint16_t* 	aux16_ptr;


			        N 	= *Rx_data_ptr++;               // Extraigo N
					cmd = *Rx_data_ptr++;               // Extraigo CMD

					aux_ptrTABLA = &ptrTABLA;
					//apunto al registro de tabla que quiero escribir (con ptrTABLA)
					aux_ptrTABLA[3] = 0;
					aux_ptrTABLA[2] = 0;
					aux_ptrTABLA[1] = *Rx_data_ptr;
					aux_ptrTABLA[0] = *(Rx_data_ptr+1);

					(void)EEPROM_WriteBuffer((Rx_data_ptr+3), (uint32_t) ptrTABLA, *(Rx_data_ptr+2) );

					i=0;
					bufferTx[i++] = aux_ptrTABLA[1];
					bufferTx[i++] = aux_ptrTABLA[0];
					bufferTx[i++] = ' ';

					uint8_t k = *(Rx_data_ptr+2);
					uint8_t j = 0;
					while(k > 0){
						bufferTx[i++] = *(Rx_data_ptr+3+j);
						k--;
						j++;
					}

					bufferTx[i++] = 0x0D;
					bufferTx[i++] = 0x0A;

					print_and_wait((byte*)&bufferTx, i);

				}

	void Enviar_BUFFER_Rx_porPuertoSerie (byte* Rx_data_ptr, byte N){

						print_and_wait(Rx_data_ptr, N);

	}



 static	void print_and_wait(byte* bufferTx, uint16_t i){

		byte status;

		status = 1;
		while(status){
			if((NO_TXING_PRINTER && NO_RXING_PRINTER  && NO_ESTA_IMPRIMIENDO)){
				statusPRINT = IMPRESION_EN_PROCESO;
				PRINT_send(bufferTx, i+1);
				while(statusPRINT !=NO_HAY_IMPRESION_EN_PROCESO){
					//espera fin de transmision
					//porque el buffer de tx esta en el stack
					//y si vuelvo al loop principal es probable que se pise el
					//stack mientras se transmite los datos desde la sci
					status=0;
				}
			}
		}
	}


 static void Leer_reporteTURNO(byte* Rx_data_ptr){

	 byte*			ptrINI_TURNO;
	 byte* 			ptrFIN_TURNO;
	 tREG_GENERIC*	ptrTABLA;
	// byte* ptrFIN;
	 byte tipo, TO_F, cantidadSESIONES;


	 tREG_SESION*	ptrsSESION[2];

	 Rx_data_ptr++;               // salto N
	 Rx_data_ptr++;               // salt0 CMD
	 tipo = *Rx_data_ptr++;


	 cantidadSESIONES = REPORTES_getSesions(ptrsSESION, 2);
	 if(cantidadSESIONES != 0xff){
		 ptrINI_TURNO = ptrsSESION[1];
		 ptrFIN_TURNO = ptrsSESION[0];


		 ptrTABLA = (tREG_GENERIC*)ptrINI_TURNO;

		 TO_F = 0;
		 dispararTO_lazo();
		 while(ptrTABLA != ptrFIN_TURNO){
			if((NO_TXING_PRINTER && NO_RXING_PRINTER  && NO_ESTA_IMPRIMIENDO)){
				if(checkRANGE(ptrTABLA, FIN_TABLA_REPORTE, TO_F )){break;}		//cuando INI_ptr > FIN_TABLA_REPORTE sale del while
				READandPRINT(&ptrTABLA, tipo);
			}
		 }
		 READandPRINT(&ptrTABLA, tipo);
		 detenerTO_lazo();
	 }
 }

 static void Leer_reporteTURNO_PARCIAL(byte* Rx_data_ptr){

	 byte*			ptrINI_TURNO;
	 byte* 			ptrFIN_TURNO;
	 tREG_GENERIC*	ptrTABLA;
	// byte* ptrFIN;
	 byte tipo, TO_F, cantidadSESIONES;


	 tREG_SESION*	ptrsSESION[2];

	 Rx_data_ptr++;               // salto N
	 Rx_data_ptr++;               // salt0 CMD
	 tipo = *Rx_data_ptr++;


	   cantidadSESIONES = REPORTES_getSesions(ptrsSESION, 2);
	 //if(cantidadSESIONES != 0xff){
		// ptrINI_TURNO = ptrsSESION[1];
		 //ptrFIN_TURNO = ptrsSESION[0];

	     ptrINI_TURNO = ptrsSESION[0];
	     ptrFIN_TURNO = REPORTE_PUTptr;
		 ptrTABLA = (tREG_GENERIC*)ptrINI_TURNO;

		 TO_F = 0;
		 dispararTO_lazo();
		 while(ptrTABLA != ptrFIN_TURNO){
			if((NO_TXING_PRINTER && NO_RXING_PRINTER  && NO_ESTA_IMPRIMIENDO)){
				if(checkRANGE(ptrTABLA, FIN_TABLA_REPORTE, TO_F )){break;}		//cuando INI_ptr > FIN_TABLA_REPORTE sale del while
				READandPRINT(&ptrTABLA, tipo);
			}
		 }
		 READandPRINT(&ptrTABLA, tipo);
		 detenerTO_lazo();
	// }
 }


static void READandPRINT(byte** ptrptrTABLA, byte tipo){
	READandPRINT_regTABLA_REPORTES (*ptrptrTABLA, tipo); 						//Lee un registro de tabla y lo manda por puerto
	incFlashRep_ptr(ptrptrTABLA);										//ptrTABLA++;
	HAL_Delay(50);													//da tiempo al docklight que no se me cuelque
}


		/* RECEPCION DE  */
		/**************************************/
		void Inicio_TURNO_Rx (byte* Rx_data_ptr){
			//
			// El formato de datos de recepcion es
			//
			//    | N | CMD | DATA_1 | DATA_2 | . | . |
			//
			//
			// Si el bit mas significativo esta seteado (0x80) indica que va a indicar
			// cantidad maxima de FICHAS/PESOS.
			byte N;
			byte cmd;
			uint16_t aux16;

			N 	= *Rx_data_ptr++;               // Extraigo N
			cmd = *Rx_data_ptr++;               // Extraigo CMD
			nroChofer = *Rx_data_ptr++;            // Extraigo DATA_1
			//informa que el turno fue iniciado correctamente
		    //prendo bandera
			if(ESTADO_RELOJ == FUERA_SERVICIO){
				//BanderaOut_On();
				aux16 = getViajes_Parcial();
				if(aux16 != 0){
					RELOJ_INTERNO_newSesion(nroChofer);
					if(!(EQUIPO_METRO_LITE_RELOJ_BANDERITA)){
						Tx_Comando_MENSAJE(SESION_CERRADA_EXITOSAMENTE);
					}
				}else{
					Tx_Comando_MENSAJE(DEBE_REALIZAR_UN_VIAJE);
					//RELOJ_INTERNO_newSesion(nroChofer);
				}

			}
		}

		/* RECEPCION DE  */
		/**************************************/
		void Entra_DESCANSO_Rx (byte* Rx_data_ptr){
			//
			// El formato de datos de recepcion es
			//
			//    | N | CMD | DATA_1 | DATA_2 | . | . |
			//
			//
			// Si el bit mas significativo esta seteado (0x80) indica que va a indicar
			// cantidad maxima de FICHAS/PESOS.
			byte N;
			byte cmd;

			N 	= *Rx_data_ptr++;               // Extraigo N
			cmd = *Rx_data_ptr++;               // Extraigo CMD
			//nroChofer = 0;
		    //apago bandera
			nroChofer = *Rx_data_ptr++;            // Extraigo DATA_1

		//	if(ESTADO_RELOJ == LIBRE){
				Pase_a_FUERA_SERVICIO();
		//	}else{
				//Tx_Conf_finTURNO(noFIN);
		//	}

		}

		void Sale_DESCANSO_Rx (byte* Rx_data_ptr){
					//
					// El formato de datos de recepcion es
					//
					//    | N | CMD | DATA_1 | DATA_2 | . | . |
					//
					//
					// Si el bit mas significativo esta seteado (0x80) indica que va a indicar
					// cantidad maxima de FICHAS/PESOS.
					byte N;
					byte cmd;

					N 	= *Rx_data_ptr++;               // Extraigo N
					cmd = *Rx_data_ptr++;               // Extraigo CMD
					//nroChofer = 0;
				    //apago bandera
					nroChofer = *Rx_data_ptr++;            // Extraigo DATA_1
					quitar_asignado;

					//if(ESTADO_RELOJ == FUERA_SERVICIO){
						//BanderaOut_Off();
						//RELOJ_INTERNO_newSesion(nroChofer);
						//Tx_Conf_finTURNO(okFIN);
						//print_ticket_turno();
						CMD_Pase_a_LIBRE.timeReint = timeReint_normal;
						if(ESTADO_RELOJ==LIBRE){
							Tx_Pase_a_LIBRE(CON_CONEXION_CENTRAL);
						}else {
							Pase_a_LIBRE(CON_CONEXION_CENTRAL);
						}

				//	}else{
						//Tx_Conf_finTURNO(noFIN);
				//	}

				}

		void Pedido_reportePARCIAL_Rx (byte* Rx_data_ptr){
						//
						// El formato de datos de recepcion es
						//
						//    | N | CMD | DATA_1 | DATA_2 | . | . |
						//
						//
						// Si el bit mas significativo esta seteado (0x80) indica que va a indicar
						// cantidad maxima de FICHAS/PESOS.
						byte N;
						byte cmd;

						N 	= *Rx_data_ptr++;               // Extraigo N
						cmd = *Rx_data_ptr++;               // Extraigo CMD
						if(ESTADO_RELOJ != OCUPADO){
							Tx_Comando_MENSAJE(RECAUDACION_PARCIAL);
						}
					}

		void UpdateSuccess_Rx (byte* Rx_data_ptr){
						//
						// El formato de datos de recepcion es
						//
						//    | N | CMD | DATA_1 | DATA_2 | . | . |
						//
						//
						// Si el bit mas significativo esta seteado (0x80) indica que va a indicar
						// cantidad maxima de FICHAS/PESOS.
						byte N;
						byte cmd;

						N 	= *Rx_data_ptr++;               // Extraigo N
						cmd = *Rx_data_ptr++;               // Extraigo CMD
			    		//reseteo el equipo
						NVIC_SystemReset();
					}



		/* RECEPCION DE  */
		/**************************************/
		void Pedido_Pase_LIBRE_Rx (byte* Rx_data_ptr){
			//
			// El formato de datos de recepcion es
			//
			//    | N | CMD | DATA_1 | DATA_2 | . | . |
			//
			//
			// Si el bit mas significativo esta seteado (0x80) indica que va a indicar
			// cantidad maxima de FICHAS/PESOS.
			byte N;
			byte cmd;

			sinCONEXION_CENTRAL=0;
			appConectada_ACentral = 1;

			if(datosSC_cntWORD == 0){
				N 	= *Rx_data_ptr++;               // Extraigo N
				cmd = *Rx_data_ptr++;               // Extraigo CMD
				SaveDatGps(Rx_data_ptr, LIB);
				if(ESTADO_RELOJ==COBRANDO || ESTADO_RELOJ==FUERA_SERVICIO){
					Pase_a_LIBRE(CON_CONEXION_CENTRAL);
				}else if (ESTADO_RELOJ==LIBRE){
					TxRta_conDATOS(CAMBIO_RELOJ_PERMITIDO);
					Tx_Pase_a_LIBRE(CON_CONEXION_CENTRAL);
				}else if (ESTADO_RELOJ==OCUPADO){
					TxRta_conDATOS(CAMBIO_RELOJ_PERMITIDO);
					Tx_Pase_a_OCUPADO(CON_CONEXION_CENTRAL);
				}
				//Tx_Resumen_VIAJE();
				//TxRta_conDATOS(0x02);
			}else{
				Tx_Comando_MENSAJE(SINCRONIZANDO_CON_CENTRAL);
			}
		}


		/* RECEPCION DE  */
		/**************************************/

		void Pedido_Pase_OCUPADO_Rx (byte* Rx_data_ptr){
			//
			// El formato de datos de recepcion es
			//
			//    | N | CMD | DATA_1 | DATA_2 | . | . |
			//
			//
			// Si el bit mas significativo esta seteado (0x80) indica que va a indicar
			// cantidad maxima de FICHAS/PESOS.
			byte N;
			byte cmd;

			sinCONEXION_CENTRAL=0;
			appConectada_ACentral = 1;

			if(datosSC_cntWORD == 0){
				if(VELCOCIDAD_PERMITE_CAMBIO_RELOJ){
					N 	= *Rx_data_ptr++;               // Extraigo N
					cmd = *Rx_data_ptr++;               // Extraigo CMD
					//TARIFA.numero = *Rx_data_ptr++;            // Extraigo DATA_1
					tarifa = *Rx_data_ptr++;            // Extraigo DATA_1
					SaveDatGps(Rx_data_ptr, OCUP);
	                if(ESTADO_RELOJ==LIBRE || ESTADO_RELOJ==FUERA_SERVICIO){
	                //normalmente pasa a ocupado edesde el estado libre por un cambio del chofer o por sensor de asiento
	                // o bien desde fuera de servicio por sensor de asiento
	                	if (seleccionManualTarifas){
	                						if(tarifa > 4){
	                						//tarifa invalida
	                							TxRta_conDATOS(CAMBIO_RELOJ_NO_PERMITIDO_OTROS);
	                							Tx_Comando_MENSAJE(TARIFA_INVALIDA);
	                						}else if(tarifa > nroTARIFA_HAB_MANUAL){
	                						//tarifa no programada
	                							TxRta_conDATOS(CAMBIO_RELOJ_NO_PERMITIDO_OTROS);
	                							Tx_Comando_MENSAJE(TARIFA_NO_PROGRAMADA);
	                						}else{
	                							TxRta_conDATOS(CAMBIO_RELOJ_PERMITIDO);
	                							paseOCUPADO_APP=1;
	                							tarifa_1_4 = tarifa;
	                							setTARIFA_MANUAL();
	                							Pase_a_OCUPADO(CON_CONEXION_CENTRAL);
	                							//envia valor de viaje para que muestre bajada de bandera
	                							Tx_Valor_VIAJE();
	                						}
	                	}else{
	                						tarifa_1_8 = TARIFA_AUTO_getNroTarifa();
	                						//set tarifa a mostrar en display
	                						if(tarifa_1_8 < 5){
	                							tarifa_1_4 = tarifa_1_8;
	                						}else{
	                							tarifa_1_4 = tarifa_1_8 - 4;
	                						}

	                						TxRta_conDATOS(CAMBIO_RELOJ_PERMITIDO);
	                						paseOCUPADO_APP=1;
	                						if(tarifa != tarifa_1_4){
	                							tarifa = tarifa_1_4;
	                							Tx_Comando_MENSAJE(TARIFA_AUTOMATICA);
	                						}

	                						Pase_a_OCUPADO(CON_CONEXION_CENTRAL);
	                						//envia valor de viaje para que muestre bajada de bandera
	                						Tx_Valor_VIAJE();
	                	}

	                }else if(ESTADO_RELOJ==COBRANDO){
						TxRta_conDATOS(CAMBIO_RELOJ_PERMITIDO);
						Tx_Pase_a_COBRANDO(CON_CONEXION_CENTRAL);
						Tx_Valor_VIAJE();
	                }else if(ESTADO_RELOJ==OCUPADO){
	 					TxRta_conDATOS(CAMBIO_RELOJ_PERMITIDO);
	 					Tx_Pase_a_OCUPADO(CON_CONEXION_CENTRAL);
	 					Tx_Valor_VIAJE();
	                }else if (ESTADO_RELOJ==LIBRE){
						TxRta_conDATOS(CAMBIO_RELOJ_PERMITIDO);
						Tx_Pase_a_LIBRE(CON_CONEXION_CENTRAL);
	                } else{
	                	TxRta_conDATOS(CAMBIO_RELOJ_PERMITIDO);
	                }
				}else{
					TxRta_conDATOS(CAMBIO_RELOJ_NO_PERMITIDO_MOV);
					Tx_Comando_MENSAJE(VEHICULO_EN_MOVIMIENTO);
				}

			}else{
				Tx_Comando_MENSAJE(SINCRONIZANDO_CON_CENTRAL);
			}
		}

		/* RECEPCION DE  */
		/*****************/
			void Pedido_Pase_COBRANDO_Rx (byte* Rx_data_ptr){
				//
				// El formato de datos de recepcion es
				//
				//    | N | CMD | DATA_1 | DATA_2 | . | . |
				//
				//
				// Si el bit mas significativo esta seteado (0x80) indica que va a indicar
				// cantidad maxima de FICHAS/PESOS.
				byte N;
				byte cmd;
				appConectada_ACentral = 1;
				if(datosSC_cntWORD == 0){
					sinCONEXION_CENTRAL=0;

						if(VELCOCIDAD_PERMITE_CAMBIO_RELOJ){
							if(ESTADO_RELOJ==OCUPADO){
								TxRta_conDATOS(CAMBIO_RELOJ_PERMITIDO);
								N 	= *Rx_data_ptr++;               // Extraigo N
								cmd = *Rx_data_ptr++;               // Extraigo CMD
								SaveDatGps(Rx_data_ptr, COBR);
								Pase_a_COBRANDO(CON_CONEXION_CENTRAL);
								Tx_Valor_VIAJE();

							}else if(ESTADO_RELOJ==COBRANDO){
								//esto puede pasar si la central estaba caida y el pase a cobrando no se registro en la central
								TxRta_conDATOS(CAMBIO_RELOJ_PERMITIDO);
								Tx_Valor_VIAJE();
								Tx_Pase_a_COBRANDO(CON_CONEXION_CENTRAL);

								//TxRta_conDATOS(CAMBIO_RELOJ_NO_PERMITIDO_OTROS);
								//Tx_Comando_MENSAJE(PASE_A_COBRANDO_NO_PERMITIDO);
							}else if (ESTADO_RELOJ==LIBRE){
								TxRta_conDATOS(CAMBIO_RELOJ_PERMITIDO);
								//parche reintento infinito de libre, porque la app esta en ocupado
								//el reloj en libre, la app queda reintentando el pase a cobrando
								//entonces le envio un pase a cobrando trucho para que se pase a cobrando
								//y luego se pueda pasar a libre

								//Tx_Pase_a_LIBRE(CON_CONEXION_CENTRAL);
								Tx_Pase_a_COBRANDO_trucho(CON_CONEXION_CENTRAL);
								Tx_Valor_VIAJE();
							}else {
								//no se en que caso puede ocurrir
								TxRta_conDATOS(CAMBIO_RELOJ_PERMITIDO);
							}

						}else{
							TxRta_conDATOS(CAMBIO_RELOJ_NO_PERMITIDO_MOV);
							Tx_Comando_MENSAJE(VEHICULO_EN_MOVIMIENTO);
						}
						//Tx_Comando_MENSAJE(RECAUDACION_PARCIAL);
				}else{
					Tx_Comando_MENSAJE(SINCRONIZANDO_CON_CENTRAL);
				}
			}



			/* RECEPCION DE  */
					/**************************************/
					void Pedido_Pase_LIBRE_SC_Rx (byte* Rx_data_ptr){
						//
						// El formato de datos de recepcion es
						//
						//    | N | CMD | DATA_1 | DATA_2 | . | . |
						//
						//
						// Si el bit mas significativo esta seteado (0x80) indica que va a indicar
						// cantidad maxima de FICHAS/PESOS.
						byte N;
						byte cmd;

						//if(VELCOCIDAD_PERMITE_CAMBIO_RELOJ && (ESTADO_RELOJ != LIBRE)){

						sinCONEXION_CENTRAL=1;
						yaTrasmitioUltimo_cmdReloj = 0;

							N 	= *Rx_data_ptr++;               // Extraigo N
							cmd = *Rx_data_ptr++;               // Extraigo CMD
							SaveDatGps(Rx_data_ptr, LIB);
							if(ESTADO_RELOJ==COBRANDO || ESTADO_RELOJ==FUERA_SERVICIO){
								Pase_a_LIBRE(SIN_CONEXION_CENTRAL);
							}else if (ESTADO_RELOJ==LIBRE){
								TxRta_conDATOS(CAMBIO_RELOJ_PERMITIDO);
								//Tx_Pase_a_LIBRE(SIN_CONEXION_CENTRAL);
							}else if (ESTADO_RELOJ==OCUPADO){
								TxRta_conDATOS(CAMBIO_RELOJ_PERMITIDO);
								Tx_Valor_VIAJE();
								//Tx_Pase_a_OCUPADO(SIN_CONEXION_CENTRAL);
							}else{
								TxRta_conDATOS(CAMBIO_RELOJ_PERMITIDO);
							}
							//Tx_Resumen_VIAJE();
							//TxRta_conDATOS(0x02);

						//}


					}

					/* RECEPCION DE  */
					/**************************************/

					void Pedido_Pase_OCUPADO_SC_Rx (byte* Rx_data_ptr){
						//
						// El formato de datos de recepcion es
						//
						//    | N | CMD | DATA_1 | DATA_2 | . | . |
						//
						//
						// Si el bit mas significativo esta seteado (0x80) indica que va a indicar
						// cantidad maxima de FICHAS/PESOS.
						byte N;
						byte cmd;

						sinCONEXION_CENTRAL=1;
						yaTrasmitioUltimo_cmdReloj = 0;

						if(VELCOCIDAD_PERMITE_CAMBIO_RELOJ){
							N 	= *Rx_data_ptr++;               // Extraigo N
							cmd = *Rx_data_ptr++;               // Extraigo CMD
							//TARIFA.numero = *Rx_data_ptr++;            // Extraigo DATA_1

							tarifa = *Rx_data_ptr++;            // Extraigo DATA_1
							SaveDatGps(Rx_data_ptr, OCUP);

							if(ESTADO_RELOJ==LIBRE || ESTADO_RELOJ==FUERA_SERVICIO){
								if (seleccionManualTarifas){
									if(tarifa > 4){
									//tarifa invalida
										TxRta_conDATOS(CAMBIO_RELOJ_NO_PERMITIDO_OTROS);
										Tx_Comando_MENSAJE(TARIFA_INVALIDA);
									}else if(tarifa > nroTARIFA_HAB_MANUAL){
									//tarifa no programada
										TxRta_conDATOS(CAMBIO_RELOJ_NO_PERMITIDO_OTROS);
										Tx_Comando_MENSAJE(TARIFA_NO_PROGRAMADA);
									}else{
										TxRta_conDATOS(CAMBIO_RELOJ_PERMITIDO);
										paseOCUPADO_APP=1;
										tarifa_1_4 = tarifa;
										setTARIFA_MANUAL();
										Pase_a_OCUPADO(SIN_CONEXION_CENTRAL);
										//envia valor de viaje para que muestre bajada de bandera
										Tx_Valor_VIAJE();
									}
								}else{
									tarifa_1_8 = TARIFA_AUTO_getNroTarifa();
									//set tarifa a mostrar en display
									if(tarifa_1_8 < 5){
										tarifa_1_4 = tarifa_1_8;
									}else{
										tarifa_1_4 = tarifa_1_8 - 4;
									}

									TxRta_conDATOS(CAMBIO_RELOJ_PERMITIDO);
									paseOCUPADO_APP=1;
									if(tarifa != tarifa_1_4){
										tarifa = tarifa_1_4;
										Tx_Comando_MENSAJE(TARIFA_AUTOMATICA);
									}

									Pase_a_OCUPADO(SIN_CONEXION_CENTRAL);
									//envia valor de viaje para que muestre bajada de bandera
									Tx_Valor_VIAJE();
								}
							}else if (ESTADO_RELOJ==OCUPADO){
								TxRta_conDATOS(CAMBIO_RELOJ_PERMITIDO);
								//Tx_Pase_a_OCUPADO(SIN_CONEXION_CENTRAL);
								Tx_Valor_VIAJE();
							}else if (ESTADO_RELOJ==COBRANDO){
								TxRta_conDATOS(CAMBIO_RELOJ_PERMITIDO);
								//Tx_Pase_a_COBRANDO(SIN_CONEXION_CENTRAL);
								Tx_Valor_VIAJE();
							}else{
								TxRta_conDATOS(CAMBIO_RELOJ_PERMITIDO);
							}
						}else{
							TxRta_conDATOS(CAMBIO_RELOJ_NO_PERMITIDO_MOV);
							Tx_Comando_MENSAJE(VEHICULO_EN_MOVIMIENTO);
						}
					}

					/* RECEPCION DE  */
					/*****************/
						void Pedido_Pase_COBRANDO_SC_Rx (byte* Rx_data_ptr){
							//
							// El formato de datos de recepcion es
							//
							//    | N | CMD | DATA_1 | DATA_2 | . | . |
							//
							//
							// Si el bit mas significativo esta seteado (0x80) indica que va a indicar
							// cantidad maxima de FICHAS/PESOS.
							byte N;
							byte cmd;

							sinCONEXION_CENTRAL=1;
							yaTrasmitioUltimo_cmdReloj = 0;

								if(VELCOCIDAD_PERMITE_CAMBIO_RELOJ){
									if(ESTADO_RELOJ==OCUPADO){
										TxRta_conDATOS(CAMBIO_RELOJ_PERMITIDO);
										N 	= *Rx_data_ptr++;               // Extraigo N
										cmd = *Rx_data_ptr++;               // Extraigo CMD
										SaveDatGps(Rx_data_ptr, COBR);
										Pase_a_COBRANDO(SIN_CONEXION_CENTRAL);
										Tx_Valor_VIAJE();
									}else if(ESTADO_RELOJ==COBRANDO){
										//esto puede pasar si la central estaba caida y el pase a cobrando no se registro en la central
										TxRta_conDATOS(CAMBIO_RELOJ_PERMITIDO);
										Tx_Valor_VIAJE();
										//Tx_Pase_a_COBRANDO(SIN_CONEXION_CENTRAL);
									}else if(ESTADO_RELOJ==LIBRE){
										//esto puede pasar si la central estaba caida y el pase a cobrando no se registro en la central
										TxRta_conDATOS(CAMBIO_RELOJ_PERMITIDO);
										//Tx_Pase_a_LIBRE(SIN_CONEXION_CENTRAL);
									} else{
										TxRta_conDATOS(CAMBIO_RELOJ_PERMITIDO);
									}



								}else{
									TxRta_conDATOS(CAMBIO_RELOJ_NO_PERMITIDO_MOV);
									Tx_Comando_MENSAJE(VEHICULO_EN_MOVIMIENTO);
								}
								//Tx_Comando_MENSAJE(RECAUDACION_PARCIAL);

						}


			void Comando_TRANSPARENTE_Rx (byte* Rx_data_ptr){
				//
				// El formato de datos de recepcion es
				//
				//    | N | CMD | DATA_1 | DATA_2 | . | . |
				//
				//
				// Si el bit mas significativo esta seteado (0x80) indica que va a indicar
				// cantidad maxima de FICHAS/PESOS.

				byte 	cmd, subcmd, N;
				byte*   data_ptr;

				data_ptr = Rx_data_ptr;

				N 		= *data_ptr++;               // Extraigo N
				cmd 	= *data_ptr++;               // Extraigo CMD
				subcmd	= *data_ptr++;              // Extraigo sub CMD


			    if(subcmd< 0x80){
			     //comandos transparentes
			    	procesar_CMDS_TRANSPARENTES(Rx_data_ptr);
			    }else{
			    //resùesta a comandos transparentes
			    	procesar_respCMDS_TRANSPARENTES(Rx_data_ptr);
			    }
			}



		/* RECEPCION DE  TARIFAS */
		/*************************/
		void ConsultaTarifas (byte* Rx_data_ptr){
			//
			// El formato de datos de recepcion es
			//
			//    | N | CMD | DATA_1 | DATA_2 | . | . |
			//
			//
			// Si el bit mas significativo esta seteado (0x80) indica que va a indicar
			// cantidad maxima de FICHAS/PESOS.
			byte N;
			byte cmd;

			N 	= *Rx_data_ptr++;                   // Extraigo N
			cmd = *Rx_data_ptr++;               	// Extraigo CMD
			Tx_TARIFAS();
		}


		void Inicio_EPERA_Rx (byte* Rx_data_ptr){
			//
			// El formato de datos de recepcion es
			//
			//    | N | CMD | DATA_1 | DATA_2 | . | . |
			//
			//
			// Si el bit mas significativo esta seteado (0x80) indica que va a indicar
			// cantidad maxima de FICHAS/PESOS.
			byte N;
			byte cmd;
			byte DATA_1;
			byte DATA_2;
			uint16_t maxValue;
			byte* dirNEXT_ptr;


			N 	= *Rx_data_ptr++;               // Extraigo N
			cmd = *Rx_data_ptr++;               // Extraigo CMD
			//RELOJ_toggleEspera();
			//no_contar_espera = 0;           // Empiezo a contar espera

		}

		void Fin_EPERA_Rx (byte* Rx_data_ptr){
			//
			// El formato de datos de recepcion es
			//
			//    | N | CMD | DATA_1 | DATA_2 | . | . |
			//
			//
			// Si el bit mas significativo esta seteado (0x80) indica que va a indicar
			// cantidad maxima de FICHAS/PESOS.
			byte N;
			byte cmd;
			byte DATA_1;
			byte DATA_2;
			uint16_t maxValue;
			byte* dirNEXT_ptr;


			N 	= *Rx_data_ptr++;               // Extraigo N
			cmd = *Rx_data_ptr++;               // Extraigo CMD

			//RELOJ_toggleEspera();
			//no_contar_espera = 1;               // Detengo a contar espera

		}


		/* RECEPCION DE  */
		/**************************************/
		void Impresion_Rx (byte* Rx_data_ptr){
			//
			// El formato de datos de recepcion es
			//
			//    | N | CMD | DATA_1 | DATA_2 | . | . |
			//
			//
			// Si el bit mas significativo esta seteado (0x80) indica que va a indicar
			// cantidad maxima de FICHAS/PESOS.
			byte N;
			byte cmd;
			byte DATA_1;
			byte DATA_2;
			uint16_t maxValue;
			byte* dirNEXT_ptr;


			N 	= *Rx_data_ptr++;               // Extraigo N
			cmd = *Rx_data_ptr++;               // Extraigo CMD
			DATA_1 = *Rx_data_ptr++;            // Extraigo DATA_1
			DATA_2 = *Rx_data_ptr++;            // Extraigo DATA_2
			dirNEXT_ptr = Rx_data_ptr;

			//imprime ticket de viaje ?
			if(DATA_1 == IMPRESION_TICKET_VIAJE){
				print_ticket_viaje();
			}

		 #ifdef VISOR_REPORTES
			//imprime ticket de turno o parcial ?
			if(DATA_1 == IMPRESION_TICKET_TURNO && REPORTES_HABILITADOS){
					print_ticket_turno(0);
			}else if(DATA_1 == IMPRESION_TICKET_PARCIAL && REPORTES_HABILITADOS){
					print_ticket_parcial();
			}
		 #endif

		}

		/* RECEPCION DE ESTADO */
		/*************************/
		void ConsultaEstado (byte* Rx_data_ptr){
			//
			// El formato de datos de recepcion es
			//
			//    | N | CMD | DATA_1 | DATA_2 | . | . |
			//
			//
			// Si el bit mas significativo esta seteado (0x80) indica que va a indicar
			// cantidad maxima de FICHAS/PESOS.
			byte N;
			byte cmd;

			N 	= *Rx_data_ptr++;                   // Extraigo N
			cmd = *Rx_data_ptr++;               	// Extraigo CMD
			Tx_Status_RELOJ();
			if(ESTADO_RELOJ==OCUPADO || ESTADO_RELOJ==COBRANDO){
				Tx_Valor_VIAJE();
			}
			//Tx_respVersion_Equipo();

		}


		/* App Conectada */
		/*****************/
		void Rx_appConectada_aCentral (byte* Rx_data_ptr){
			//
			// El formato de datos de recepcion es
			//
			//    | N | CMD | DATA_1 | DATA_2 | . | . |
			//
			//
			// Si el bit mas significativo esta seteado (0x80) indica que va a indicar
			// cantidad maxima de FICHAS/PESOS.
			byte N;
			byte cmd;

			N 	= *Rx_data_ptr++;                   // Extraigo N
			cmd = *Rx_data_ptr++;               	// Extraigo CMD

			appConectada_ACentral = 1;
			//prueba
			//Tx_Comando_MENSAJE(SINCRONIZANDO_CON_CENTRAL);

			if(sinCONEXION_CENTRAL && !yaTrasmitioUltimo_cmdReloj){
				yaTrasmitioUltimo_cmdReloj = 1;
				if(ESTADO_RELOJ==COBRANDO){
					//Tx_Pase_a_COBRANDO(CON_CONEXION_CENTRAL);
				}else if (ESTADO_RELOJ==LIBRE){
					//Tx_Pase_a_LIBRE(CON_CONEXION_CENTRAL);
				}else if (ESTADO_RELOJ==OCUPADO){
					//Tx_Pase_a_OCUPADO(CON_CONEXION_CENTRAL);
				}
			}
		}

		/* App DesConectada */
		/*******************/
		void Rx_appDesconectada_deCentral (byte* Rx_data_ptr){
			//
			// El formato de datos de recepcion es
			//
			//    | N | CMD | DATA_1 | DATA_2 | . | . |
			//
			//
			// Si el bit mas significativo esta seteado (0x80) indica que va a indicar
			// cantidad maxima de FICHAS/PESOS.
			byte N;
			byte cmd;

			N 	= *Rx_data_ptr++;                   // Extraigo N
			cmd = *Rx_data_ptr++;               	// Extraigo CMD

			appConectada_ACentral = 0;

		}


		/* RECEPCION DE PASE A ASIGNADO*/
		/*******************************/
		void ASIGNAR_Rx (byte* Rx_data_ptr){
			//
			// El formato de datos de recepcion es
			//
			//    | N | CMD | DATA_1 | DATA_2 | . | . |
			//
			//
			// Si el bit mas significativo esta seteado (0x80) indica que va a indicar
			// cantidad maxima de FICHAS/PESOS.
			byte N;
			byte cmd;

			N 	= *Rx_data_ptr++;               // Extraigo N
			cmd = *Rx_data_ptr++;               // Extraigo CMD

			BanderaOut_Off();					//apago bandera
			if(RELOJ_LIBRE){
				asignar_libre;
			}else if(RELOJ_OCUPADO || RELOJ_COBRANDO){
				asignar_ocupado;
			}
		}


		/* RECEPCION DE PASE A ASIGNADO*/
		/*******************************/
		void quitarASIGNADO_Rx (byte* Rx_data_ptr){
			//
			// El formato de datos de recepcion es
			//
			//    | N | CMD | DATA_1 | DATA_2 | . | . |
			//
			//
			// Si el bit mas significativo esta seteado (0x80) indica que va a indicar
			// cantidad maxima de FICHAS/PESOS.
			byte N;
			byte cmd;

			N 	= *Rx_data_ptr++;               // Extraigo N
			cmd = *Rx_data_ptr++;               // Extraigo CMD

			BanderaOut_On();					//apago bandera
			quitar_asignado;
		}

		//PROCESO DE COMANDOS TRANSPARENTES
	static	void procesar_CMDS_TRANSPARENTES(byte* Rx_data_ptr){


			byte			cmd, subcmd, N, i,k,dataBYTE,statusSESION,cantidad_de_sesion;
			byte			status;  //0: encontro viaje // 1: no encontro viaje
			uint16_t		dataWORD;
			byte			buff_aux[400];
			tREG_LIBRE*	ptrREG_LIBRE;
			tREG_OCUPADO*	ptrREG_OCUPADO;
			tREG_A_PAGAR*	ptrREG_APAGAR;
			uint8_t* 		Rx_data_ptr_aux;
			byte prueba;
			byte* prueba_ptr;

			N 		= *Rx_data_ptr++;              // Extraigo N
			cmd 	= *Rx_data_ptr++;              // Extraigo CMD
			subcmd	= *Rx_data_ptr++;              // Extraigo sub CMD

			dataBYTE  = *Rx_data_ptr;
			Rx_data_ptr_aux = &dataWORD;
			Rx_data_ptr_aux[1]  = Rx_data_ptr[0];
			Rx_data_ptr_aux[0]  = Rx_data_ptr[1];

			switch (subcmd){
			      case subCMD_ECO:	//ECO
			    	  	  	i=0;
			    	  	  	buff_aux[i++] = subCMD_ECO + 0x80;
			    	  	    bufferNcopy(&buff_aux[i] ,Rx_data_ptr	,N); i = i + N;
			    	  	  	N = i+1;
				    	    Tx_cmdTRANSPARENTE(N, buff_aux );
			      	  	  	break;
			      case subCMD_VERSION:  //ESTADO DE RELOJ
                            i=0;

                            getDate();
							buff_aux[i++] = subCMD_VERSION + 0x80;
							buff_aux[i++] = 0;   				//fuente de hora
							buff_aux[i++] = RTC_Date.hora[0];   // HORA
							buff_aux[i++] = RTC_Date.hora[1];   // MINUTOS
							buff_aux[i++] = RTC_Date.hora[2];   // SEGUNDOS
							buff_aux[i++] = RTC_Date.fecha[0];  // DIA
							buff_aux[i++] = RTC_Date.fecha[1];  // MES
							buff_aux[i++] = RTC_Date.fecha[2];  // AÑO
                            //copia tipo de equipo
							k = string_copy_returnN(&buff_aux[i],"  Equipo: ");
							i=i+k;
							switch (TIPO_DE_EQUIPO){
							      case 0:
									k = string_copy_returnN(&buff_aux[i],"metroblue (PB7=0, PB6=0)");
									i=i+k;
									break;
							      case 1:
										k = string_copy_returnN(&buff_aux[i],"metrolite (PB7=1, PB6=1)");
										i=i+k;
									break;
							      default :
										k = string_copy_returnN(&buff_aux[i],"  DESCONOCIDO");
										i=i+k;
							    	  break;
							}
                            //copia version
							k = string_copy_returnN(&buff_aux[i],", Firmware: ");
							i=i+k;
							k = string_copy_returnN(&buff_aux[i],versionFIRMWARE);
							i=i+k;
							k = string_copy_returnN(&buff_aux[i],fechaFIRMWARE);
							i=i+k;

							//prepara comando transparente
                            N = i+1;
			    	        Tx_cmdTRANSPARENTE(N, buff_aux );


			               break;
			      case subCMD_consultaVIAJE:
			    		//NRO VIAJE:  1  |  1 |  2 |  2 |  2 |  3 |  3 |  3 | 4
			    		// 			-----+----+----+----+----+----+----+----+----
			    		//ESTADO:	  C1 | L1 | O2 | C2 | L2 | O3 | C3 | L3 | O4
			    		//		    -----+----+----+----+----+----+----+----+----

			    		// NormalmeNte cuando se emite el comando L3 los datos que contiene son del L2,O3,C3
			    		// por lo tanto cuando se va a emitir un L3 que no se emitio por falta de conexion
			    		// tendra los datos de L2,O3,C3

			    		// igualmente cuando hay una consulta por numero de viaje, si piden los datos
			    		// del numero de viaje 2, se enviara los datos de L1, O2, C2

			    		// Ejemplo2:   NroViaje: 2 -> se arma datos de L1,O2,C2
			    		// Ejemplo1:   NroViaje: 3 -> se arma datos de L2,O3,C3

							i=0;

							buff_aux[i++] = subCMD_consultaVIAJE + 0x80;  //subcomando
							buff_aux[i++] = 0;   						  //fuente de hora

							getDate();
							buff_aux[i++] = RTC_Date.hora[0];   		  //HORA
							buff_aux[i++] = RTC_Date.hora[1];   		  //MINUTOS
							buff_aux[i++] = RTC_Date.hora[2];   		  //SEGUNDOS
							buff_aux[i++] = RTC_Date.fecha[0];  		  //DIA
							buff_aux[i++] = RTC_Date.fecha[1];  		  //MES
							buff_aux[i++] = RTC_Date.fecha[2];  		  //AÑO

							byte nroViaje = dataBYTE;

							if(nroViaje == 0){
								ptrREG_LIBRE = get_regLIBRE_by_anyPTR(REPORTE_PUTptr, 0xff);
							}else{
								ptrREG_LIBRE = get_regLIBRE_by_anyPTR(REPORTE_PUTptr, nroViaje-1);
							}
							ptrREG_OCUPADO = get_regOCUPADO_by_anyPTR(REPORTE_PUTptr, nroViaje);
							ptrREG_APAGAR  = get_regAPAGAR_by_anyPTR(REPORTE_PUTptr, nroViaje);

							i = armar_buff_viaje((uint8_t*)&buff_aux, i);
							N = i+1;
							if(ptrREG_LIBRE == NULL || ptrREG_OCUPADO == NULL || ptrREG_APAGAR == NULL ){
								status = 1;
								buff_aux[8] = status;
							}
							Tx_cmdTRANSPARENTE(N, buff_aux );

			               break;
			      case subCMD_consultaTURNO:
							cantidad_de_sesion = REPORTES_getTurno(sesion_ptrs, dataWORD, 2);        				// Obtengo punteros a todos las sesiones
							if(cantidad_de_sesion ==0xff){
								//no hay viajes
								statusSESION = 1;
							}else if(cantidad_de_sesion ==0xfe){
								//ERROR
								statusSESION = 2;
							}else{
								statusSESION = 0;
							}

							i=0;
							getDate();
							buff_aux[i++] = subCMD_consultaTURNO + 0x80;   //subcomando
							buff_aux[i++] = 0;   					  	   //fuente de hora
							buff_aux[i++] = RTC_Date.hora[0];   	       //HORA
							buff_aux[i++] = RTC_Date.hora[1];   		   //MINUTOS
							buff_aux[i++] = RTC_Date.hora[2];   		   //SEGUNDOS
							buff_aux[i++] = RTC_Date.fecha[0];  		   //DIA
							buff_aux[i++] = RTC_Date.fecha[1];  		   //MES
							buff_aux[i++] = RTC_Date.fecha[2];  		   //AÑO

							if(REPORTE_NRO_TURNO>=1 && !statusSESION){
				    	  	    //armo y transmito reporte de turno
								status = 0;
								buff_aux[i++] = status;
								N = consulta_ticket_turno(&buff_aux[i], dataWORD);
								N = N+i+1;
								Tx_cmdTRANSPARENTE(N, buff_aux );
							}else{
								if(statusSESION == 2){
									//ERROR EN REPORTE DE TURNO
									status = 1;
									buff_aux[i++] = status;
	                                N = i+1;
					    	        Tx_cmdTRANSPARENTE(N, buff_aux );
								}else{
									//TODAVIA NO FINALIZO NINGUN TURNO
									status = 1;
									buff_aux[i++] = status;
	                                N = i+1;
					    	        Tx_cmdTRANSPARENTE(N, buff_aux );
								}
							}


			               break;
			      case subCMD_progAIR:
							i=0;
							//respuesta al comando
							buff_aux[i++] = subCMD_progAIR + 0x80;  		//subcomando
							buff_aux[i++] = 0;   						  	//fuente de hora
							getDate();
							buff_aux[i++] = RTC_Date.hora[0];   		  	//HORA
							buff_aux[i++] = RTC_Date.hora[1];   		  	//MINUTOS
							buff_aux[i++] = RTC_Date.hora[2];   		  	//SEGUNDOS
							buff_aux[i++] = RTC_Date.fecha[0];  		  	//DIA
							buff_aux[i++] = RTC_Date.fecha[1];  		  	//MES
							buff_aux[i++] = RTC_Date.fecha[2];  		  	//AÑO
							buff_aux[i++] = *(Rx_data_ptr);  		  		//id 01
							buff_aux[i++] = *(Rx_data_ptr+1);  		  		//id 02

							N = i+1;
							Tx_cmdTRANSPARENTE(N, buff_aux );
							prueba_ptr = Rx_data_ptr+2;
							prueba = *(Rx_data_ptr+2);
							if(*(Rx_data_ptr+2) == 0x03){
								//grabacion de param. reloj comun
								AIR_UPDATE_RxTransparente(Rx_data_ptr-3);
			    	  	    }else if(*(Rx_data_ptr+2) == 0x04){
								//grabacion de param. tarifa
								AIR_UPDATE_RxTransparente(Rx_data_ptr-3);
							}
			               break;

			  default:
				   	   	   break;


		}
}

	static void procesar_respCMDS_TRANSPARENTES(byte* Rx_data_ptr){

		byte 	cmd, subcmd, N, nro_viaje_rx, nro_viaje_buff, cntBYTE_BACK;
		byte* 	get_BACK;

		N 		= *Rx_data_ptr++;              // Extraigo N
		cmd 	= *Rx_data_ptr++;              // Extraigo CMD
		subcmd	= *Rx_data_ptr++;              // Extraigo sub CMD
		//central -> reloj
		/*
			#define subCMD_ECO				0x01
			#define subCMD_VERSION			0x02
			#define subCMD_consultaVIAJE	0x03
			#define subCMD_consultaTURNO	0x04
			#define subCMD_progAir			0x05
		*/

		if(subcmd == subCMD_datosSC + 0x80){
			nro_viaje_rx	= *Rx_data_ptr++;

			//guardo puntero get y cntBYTE
			get_BACK = datosSC.get;
			cntBYTE_BACK = datosSC.cntWORD;
			//tomo numero de viaje
			nro_viaje_buff = getBUFCIR(&datosSC);
			if(nro_viaje_rx == nro_viaje_buff){
			 //respuesta a comando a enviado
				Tx_datosSC = 0;						//habilita tx del sgte dato en buffer
				timeoutTx_datosSC = 0;
			}else{
			 //llego dos veces la resp. a un comando
			 //no avanzo el puntero -> lo restauro
			 //y hacemos que aca no paso nada
				datosSC.get = get_BACK;
				datosSC.cntWORD = cntBYTE_BACK;
			}
		}else if(subcmd == subCMD_progSuccess + 0x80){
			//reseteo el equipo
			NVIC_SystemReset();
		}
	}




	uint8_t armar_buff_viaje(uint8_t* buff_aux, uint8_t i){
		//doy vuelta los bytes
		convert_bigINDIAN_to_litleINDIAN (&regVIAJE.fichasDist, 3);
		convert_bigINDIAN_to_litleINDIAN (&regVIAJE.fichasTime, 3);
		convert_bigINDIAN_to_litleINDIAN (&regVIAJE.importe, 4);
		convert_bigINDIAN_to_litleINDIAN (&regVIAJE.kmLIBRE			, 2);
		convert_bigINDIAN_to_litleINDIAN (&regVIAJE.kmOCUPADO		, 2);
		convert_bigINDIAN_to_litleINDIAN (&regVIAJE.segMarchaLIBRE	, 2);
		convert_bigINDIAN_to_litleINDIAN (&regVIAJE.segMarchaOCUPADO, 2);
		convert_bigINDIAN_to_litleINDIAN (&regVIAJE.segParadoLIBRE	, 2);
		convert_bigINDIAN_to_litleINDIAN (&regVIAJE.segParadoOCUPADO, 2);
		buff_aux[i++] = 0;						//status
		buff_aux[i++] = regVIAJE.nroViaje;
		buff_aux[i++] = regVIAJE.chofer;
		buff_aux[i++] = regVIAJE.tarifa;
		//if(regVIAJE.velMaxLIBRE>255){regVIAJE.velMaxLIBRE=255;}
		//if(regVIAJE.velMaxOCUPADO>255){regVIAJE.velMaxOCUPADO=255;}
		//if(regVIAJE.minutosEspera>255){regVIAJE.minutosEspera=255;}

		buff_aux[i++] = regVIAJE.velMaxLIBRE;
		buff_aux[i++] = regVIAJE.velMaxOCUPADO;
		buff_aux[i++] = regVIAJE.sensor;
		buff_aux[i++] = regVIAJE.minutosEspera;
		buff_aux[i++] = regVIAJE.fichaPesos;
		buff_aux[i++] = regVIAJE.puntoDecimal;
		buff_aux[i++] = regVIAJE.estadoConexion_LIBRE;
		buff_aux[i++] = regVIAJE.estadoConexion_OCUPADO;
		buff_aux[i++] = regVIAJE.estadoConexion_COBRANDO;
		buff_aux[i++] = regVIAJE.fichasDist[2];
		buff_aux[i++] = regVIAJE.fichasDist[1];
		buff_aux[i++] = regVIAJE.fichasDist[0];
		buff_aux[i++] = regVIAJE.fichasTime[2];
		buff_aux[i++] = regVIAJE.fichasTime[1];
		buff_aux[i++] = regVIAJE.fichasTime[0];
		bufferNcopy(&buff_aux[i] ,(byte*)&regVIAJE.importe			,sizeof(regVIAJE.importe));			 i = i + sizeof(regVIAJE.importe);
		bufferNcopy(&buff_aux[i] ,(byte*)&regVIAJE.kmLIBRE			,sizeof(regVIAJE.kmLIBRE));			 i = i + sizeof(regVIAJE.kmLIBRE);
		bufferNcopy(&buff_aux[i] ,(byte*)&regVIAJE.kmOCUPADO		,sizeof(regVIAJE.kmOCUPADO));		 i = i + sizeof(regVIAJE.kmOCUPADO);
		bufferNcopy(&buff_aux[i] ,(byte*)&regVIAJE.segMarchaLIBRE	,sizeof(regVIAJE.segMarchaLIBRE));	 i = i + sizeof(regVIAJE.segMarchaLIBRE);
		bufferNcopy(&buff_aux[i] ,(byte*)&regVIAJE.segMarchaOCUPADO	,sizeof(regVIAJE.segMarchaOCUPADO)); i = i + sizeof(regVIAJE.segMarchaOCUPADO);
		bufferNcopy(&buff_aux[i] ,(byte*)&regVIAJE.segParadoLIBRE	,sizeof(regVIAJE.segParadoLIBRE));	 i = i + sizeof(regVIAJE.segParadoLIBRE);
		bufferNcopy(&buff_aux[i] ,(byte*)&regVIAJE.segParadoOCUPADO	,sizeof(regVIAJE.segParadoOCUPADO)); i = i + sizeof(regVIAJE.segParadoOCUPADO);
		//hora
		bufferNcopy(&buff_aux[i] ,(byte*)&regVIAJE.dateLIBRE+4	,3); i = i + 3;
		//fecha
		bufferNcopy(&buff_aux[i] ,(byte*)&regVIAJE.dateLIBRE		,3); i = i + 3;
		//hora
		bufferNcopy(&buff_aux[i] ,(byte*)(&regVIAJE.dateOCUPADO)+4	,3); i = i + 3;
		//fecha
		bufferNcopy(&buff_aux[i] ,(byte*)&regVIAJE.dateOCUPADO		,3); i = i = i + 3;
		//hora
		bufferNcopy(&buff_aux[i] ,(byte*)(&regVIAJE.dateA_PAGAR)+4  ,3); i = i + 3;
		//fecha
		bufferNcopy(&buff_aux[i] ,(byte*)&regVIAJE.dateA_PAGAR	    ,3); i = i + 3;
		//Posicion libre
		buff_aux[i++] = regVIAJE.sgnLatLonLibre;
		bufferNcopy(&buff_aux[i] ,(byte*)&regVIAJE.latitudLIBRE	    ,3); i = i + 3;
		bufferNcopy(&buff_aux[i] ,(byte*)&regVIAJE.longitudLIBRE    ,3); i = i + 3;
		buff_aux[i++] = regVIAJE.velgpsLIBRE;

		//Posicion ocupado
		buff_aux[i++] = regVIAJE.sgnLatLonOcupado;
		bufferNcopy(&buff_aux[i] ,(byte*)&regVIAJE.latitudOCUPADO   ,3); i = i + 3;
		bufferNcopy(&buff_aux[i] ,(byte*)&regVIAJE.longitudOCUPADO   ,3); i = i + 3;
		buff_aux[i++] = regVIAJE.velgpsOCUPADO;

		//Posicion cobrando
		buff_aux[i++] = regVIAJE.sgnLatLonCobrando;
		bufferNcopy(&buff_aux[i] ,(byte*)&regVIAJE.latitudCOBRANDO  ,3); i = i + 3;
		bufferNcopy(&buff_aux[i] ,(byte*)&regVIAJE.longitudCOBRANDO  ,3); i = i + 3;
		buff_aux[i++] = regVIAJE.velgpsCOBRANDO;


		return(i);
	}

/*
	uint8_t armar_buff_viajeSC(uint8_t* buff_aux, uint8_t i){
		//doy vuelta los bytes
		convert_bigINDIAN_to_litleINDIAN (&regVIAJE.fichasDist, 3);
		convert_bigINDIAN_to_litleINDIAN (&regVIAJE.fichasTime, 3);
		convert_bigINDIAN_to_litleINDIAN (&regVIAJE.importe, 4);
		convert_bigINDIAN_to_litleINDIAN (&regVIAJE.kmLIBRE			, 2);
		convert_bigINDIAN_to_litleINDIAN (&regVIAJE.kmOCUPADO		, 2);
		convert_bigINDIAN_to_litleINDIAN (&regVIAJE.segMarchaLIBRE	, 2);
		convert_bigINDIAN_to_litleINDIAN (&regVIAJE.segMarchaOCUPADO, 2);
		convert_bigINDIAN_to_litleINDIAN (&regVIAJE.segParadoLIBRE	, 2);
		convert_bigINDIAN_to_litleINDIAN (&regVIAJE.segParadoOCUPADO, 2);
		buff_aux[i++] = 0;						//status
		buff_aux[i++] = regVIAJE.nroViaje;
		buff_aux[i++] = regVIAJE.chofer;
		buff_aux[i++] = regVIAJE.tarifa;
		//if(regVIAJE.velMaxLIBRE>255){regVIAJE.velMaxLIBRE=255;}
		//if(regVIAJE.velMaxOCUPADO>255){regVIAJE.velMaxOCUPADO=255;}
		//if(regVIAJE.minutosEspera>255){regVIAJE.minutosEspera=255;}

		buff_aux[i++] = regVIAJE.velMaxLIBRE;
		buff_aux[i++] = regVIAJE.velMaxOCUPADO;
		buff_aux[i++] = regVIAJE.sensor;
		buff_aux[i++] = regVIAJE.minutosEspera;
		buff_aux[i++] = regVIAJE.fichaPesos;
		buff_aux[i++] = regVIAJE.puntoDecimal;
		buff_aux[i++] = regVIAJE.estadoConexion_LIBRE;
		buff_aux[i++] = regVIAJE.estadoConexion_OCUPADO;
		buff_aux[i++] = regVIAJE.estadoConexion_COBRANDO;
		buff_aux[i++] = regVIAJE.fichasDist[2];
		buff_aux[i++] = regVIAJE.fichasDist[1];
		buff_aux[i++] = regVIAJE.fichasDist[0];
		buff_aux[i++] = regVIAJE.fichasTime[2];
		buff_aux[i++] = regVIAJE.fichasTime[1];
		buff_aux[i++] = regVIAJE.fichasTime[0];
		bufferNcopy(&buff_aux[i] ,(byte*)&regVIAJE.importe			,sizeof(regVIAJE.importe));			 i = i + sizeof(regVIAJE.importe);
		bufferNcopy(&buff_aux[i] ,(byte*)&regVIAJE.kmLIBRE			,sizeof(regVIAJE.kmLIBRE));			 i = i + sizeof(regVIAJE.kmLIBRE);
		bufferNcopy(&buff_aux[i] ,(byte*)&regVIAJE.kmOCUPADO		,sizeof(regVIAJE.kmOCUPADO));		 i = i + sizeof(regVIAJE.kmOCUPADO);
		bufferNcopy(&buff_aux[i] ,(byte*)&regVIAJE.segMarchaLIBRE	,sizeof(regVIAJE.segMarchaLIBRE));	 i = i + sizeof(regVIAJE.segMarchaLIBRE);
		bufferNcopy(&buff_aux[i] ,(byte*)&regVIAJE.segMarchaOCUPADO	,sizeof(regVIAJE.segMarchaOCUPADO)); i = i + sizeof(regVIAJE.segMarchaOCUPADO);
		bufferNcopy(&buff_aux[i] ,(byte*)&regVIAJE.segParadoLIBRE	,sizeof(regVIAJE.segParadoLIBRE));	 i = i + sizeof(regVIAJE.segParadoLIBRE);
		bufferNcopy(&buff_aux[i] ,(byte*)&regVIAJE.segParadoOCUPADO	,sizeof(regVIAJE.segParadoOCUPADO)); i = i + sizeof(regVIAJE.segParadoOCUPADO);
		//hora
		bufferNcopy(&buff_aux[i] ,(byte*)&regVIAJE.dateLIBRE+4	,3); i = i + 3;
		//fecha
		bufferNcopy(&buff_aux[i] ,(byte*)&regVIAJE.dateLIBRE		,3); i = i + 3;
		//hora
		bufferNcopy(&buff_aux[i] ,(byte*)(&regVIAJE.dateOCUPADO)+4	,3); i = i + 3;
		//fecha
		bufferNcopy(&buff_aux[i] ,(byte*)&regVIAJE.dateOCUPADO		,3); i = i = i + 3;
		//hora
		bufferNcopy(&buff_aux[i] ,(byte*)(&regVIAJE.dateA_PAGAR)+4  ,3); i = i + 3;
		//fecha
		bufferNcopy(&buff_aux[i] ,(byte*)&regVIAJE.dateA_PAGAR	    ,3); i = i + 3;
		//Posicion libre
		buff_aux[i++] = regVIAJE.sgnLatLonLibre;
		bufferNcopy(&buff_aux[i] ,(byte*)&regVIAJE.latitudLIBRE	    ,3); i = i + 3;
		bufferNcopy(&buff_aux[i] ,(byte*)&regVIAJE.longitudLIBRE    ,3); i = i + 3;
		buff_aux[i++] = regVIAJE.velgpsLIBRE;

		//Posicion ocupado
		buff_aux[i++] = regVIAJE.sgnLatLonOcupado;
		bufferNcopy(&buff_aux[i] ,(byte*)&regVIAJE.latitudOCUPADO   ,3); i = i + 3;
		bufferNcopy(&buff_aux[i] ,(byte*)&regVIAJE.longitudOCUPADO   ,3); i = i + 3;
		buff_aux[i++] = regVIAJE.velgpsOCUPADO;

		return(i);
	}
*/

	void Tx_respVersion_Equipo(void){
		byte	buff_aux[100];
		byte i, k, N;
        i=0;

            getDate();
			buff_aux[i++] = subCMD_VERSION + 0x80;
			buff_aux[i++] = 0;   				//fuente de hora
			buff_aux[i++] = RTC_Date.hora[0];   // HORA
			buff_aux[i++] = RTC_Date.hora[1];   // MINUTOS
			buff_aux[i++] = RTC_Date.hora[2];   // SEGUNDOS
			buff_aux[i++] = RTC_Date.fecha[0];  // DIA
			buff_aux[i++] = RTC_Date.fecha[1];  // MES
			buff_aux[i++] = RTC_Date.fecha[2];  // AÑO
          //copia tipo de equipo
			k = string_copy_returnN(&buff_aux[i],"  Equipo: ");
			i=i+k;
			switch (TIPO_DE_EQUIPO){
			      case 0:
					k = string_copy_returnN(&buff_aux[i],"metroblue (PB7=0, PB6=0)");
					i=i+k;
					break;
			      case 1:
						k = string_copy_returnN(&buff_aux[i],"metrolite (PB7=1, PB6=1)");
						i=i+k;
					break;
			      default :
						k = string_copy_returnN(&buff_aux[i],"  DESCONOCIDO");
						i=i+k;
			    	  break;
			}
          //copia version
			k = string_copy_returnN(&buff_aux[i],", Firmware: ");
			i=i+k;
			k = string_copy_returnN(&buff_aux[i],versionFIRMWARE);
			i=i+k;
			k = string_copy_returnN(&buff_aux[i],fechaFIRMWARE);
			i=i+k;

			//prepara comando transparente
            N = i+1;
	        Tx_cmdTRANSPARENTE(N, buff_aux );
	}

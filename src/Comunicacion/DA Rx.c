#include "DA Rx.h"
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

	static errorTYPE DA_chkRx (void);
	static errorTYPE DA_chkRxLength (void);
	static errorTYPE DA_extractRxData (byte* buffer);
	static	void DA_RxData_procesado (void);
	static void DA_newGET_inGPA (byte* GETptr);
	static void DA_incGPAidx (byte* idxPTR);


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
		Pase_LIBRE_Rx,
		Pase_OCUPADO_Rx,
		Pase_COBRANDO_Rx,
		Inicio_EPERA_Rx,
		Fin_EPERA_Rx,
		Impresion_Rx,
		ConsultaTarifas,
		ConsultaEstado,
		Rx_comando_0B,
		Entra_DESCANSO_Rx,
		Sale_DESCANSO_Rx,
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
		Rx_comando_0B,
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

					Rx_cmd = RxDA_buffer[1];        // Extraigo comando
					if(EQUIPO_METRO_LITE_RELOJ_INTERNO ||
					  (((EQUIPO_METRO_BLUE) || (EQUIPO_METRO_LITE_RELOJ_BANDERITA)) && !CMD_ACTIONS)){
						  if (Rx_cmd < 0x80){
							  // Recibí un comando de la Central -> Debo Transmitir Respuesta
							  // Asumo Rta Simple (sin datos) => Seteo N y Buffer.
							  // Si no fuera Rta simple, la rutina de Rx del comando en cuestión debe
							  // setear la longitud y el buffer de Rta
							  N_CMD_A_RESP = N_CMD;         						// Longitud de Respuesta Simple
							  BUFFER_CMD_A_RESP = NULL;     						// No lleva Buffer (Rta Simple)
							  Tabla_RxDA[Rx_cmd](RxDA_buffer);      			// Proceso Comando
							  CMD_a_RESP = Rx_cmd;        							// Qué voy a Responder
							  if(Rx_cmd != cmdConsultaTarifas){
							  TX_F_CMD_A_RESP = 1;        							// Levanto Bandera de Tx Respuesta
							  }
						  }else{
							// Recibí la respuesta a un comando
							pauseTx = 0;
							Rx_cmd -= 0x80;
							anularTx_cmd(Rx_cmd);           						// Anulo la transmisión del comando
							Tabla_RxRTA_DA[Rx_cmd](RxDA_buffer);  			// Proceso Respuesta
						  }
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
	/*************************************/
	static	void DA_RxData_procesado (void){
		DA_incGPAidx(&DA_RxGPAidxGET);    // Avanzo GET del GPA de Rx
		DA_decRx_cnt;                      // Decremento Contador de Datos Recibidos, xq fueron procesados
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

			N 	= *Rx_data_ptr++;               // Extraigo N
			cmd = *Rx_data_ptr++;               // Extraigo CMD
			nroChofer = *Rx_data_ptr++;            // Extraigo DATA_1
			//informa que el turno fue iniciado correctamente
		    //prendo bandera
			if(ESTADO_RELOJ == FUERA_SERVICIO){
				BanderaOut_On();
				RELOJ_INTERNO_newSesion(nroChofer);
				//Tx_Conf_inicioTURNO(okINICIO);
			}else{
				//Tx_Conf_inicioTURNO(noINICIO);
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

			if(ESTADO_RELOJ == LIBRE){
				Pase_a_FUERA_SERVICIO();
			}else{
				//Tx_Conf_finTURNO(noFIN);
			}

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

					if(ESTADO_RELOJ == FUERA_SERVICIO){
						//BanderaOut_Off();
						//RELOJ_INTERNO_newSesion(nroChofer);
						//Tx_Conf_finTURNO(okFIN);
						//print_ticket_turno();
						Pase_a_LIBRE();
					}else{
						//Tx_Conf_finTURNO(noFIN);
					}

				}

		/* RECEPCION DE  */
		/**************************************/
		void Pase_LIBRE_Rx (byte* Rx_data_ptr){
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

				N 	= *Rx_data_ptr++;               // Extraigo N
				cmd = *Rx_data_ptr++;               // Extraigo CMD

				Pase_a_LIBRE();

				//Tx_Resumen_VIAJE();
				//TxRta_conDATOS(0x02);

			//}


		}

		/* RECEPCION DE  */
		/**************************************/

		void Pase_OCUPADO_Rx (byte* Rx_data_ptr){
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

			if(VELCOCIDAD_PERMITE_CAMBIO_RELOJ){
				N 	= *Rx_data_ptr++;               // Extraigo N
				cmd = *Rx_data_ptr++;               // Extraigo CMD
				//TARIFA.numero = *Rx_data_ptr++;            // Extraigo DATA_1
				tarifa = *Rx_data_ptr++;            // Extraigo DATA_1
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
					Pase_a_OCUPADO();
					//envia valor de viaje para que muestre bajada de bandera
					Tx_Valor_VIAJE();
				}

			}else{
				TxRta_conDATOS(CAMBIO_RELOJ_NO_PERMITIDO_MOV);
				Tx_Comando_MENSAJE(VEHICULO_EN_MOVIMIENTO);
			}
		}

		/* RECEPCION DE  */
		/*****************/
			void Pase_COBRANDO_Rx (byte* Rx_data_ptr){
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

					if(VELCOCIDAD_PERMITE_CAMBIO_RELOJ){
						TxRta_conDATOS(CAMBIO_RELOJ_PERMITIDO);
						N 	= *Rx_data_ptr++;               // Extraigo N
						cmd = *Rx_data_ptr++;               // Extraigo CMD
						Pase_a_COBRANDO();
						Tx_Valor_VIAJE();

					}else{
						TxRta_conDATOS(CAMBIO_RELOJ_NO_PERMITIDO_MOV);
						Tx_Comando_MENSAJE(VEHICULO_EN_MOVIMIENTO);
					}
					Tx_Comando_MENSAJE(RECAUDACION_PARCIAL);

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
				byte N;
				byte cmd;

				//if(VELCOCIDAD_PERMITE_CAMBIO_RELOJ && (ESTADO_RELOJ != COBRANDO)){
					N 	= *Rx_data_ptr++;               // Extraigo N
					cmd = *Rx_data_ptr++;               // Extraigo CMD
				//}
					//ENVIA UN ECO
					Tx_Comando_TRANSPARENTE(N, Rx_data_ptr );
					//ENVIA AL CEL PARA QUE MUESTRE POR PANTALLA
					Tx_Comando_MENSAJE_fromBUFFER(N, Rx_data_ptr, COMANDO_TRANSPARENTE);
					//CON UN BEEP INDICA RECEPCION DE COMANDO TRANSPARENTE
					Buzzer_On(BEEP_TECLA_LARGA);
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
					print_ticket_turno();
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
			if(ESTADO_RELOJ==COBRANDO){
				Tx_Valor_VIAJE();
			}


		}

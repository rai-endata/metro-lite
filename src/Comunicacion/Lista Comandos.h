#ifndef _LISTA_COMANDOS_
  #define _LISTA_COMANDOS_



/* ********************************************************
 * COMANDOS DE TRANSMISION DEL RELOJ AL DISPOSITIVO ANDROID
 ******************************************************* */
	#define cmdEncendido_EQUIPO					0x01
	#define cmdBoton_EMERGENCIA					0x02
	#define cmdValor_BAJADA_BANDERA				0x03
	#define cmdValor_VIAJE						0x04

	//#define cmdResumen_VIAJE					0x06
	//#define cmdPosicion						0x07
	#define cmdStatus_RELOJ						0x08
	#define cmdTARIFAS							0x09
	#define cmdConf_inicioTURNO					0x0A
	//#define cmdConf_finTURNO					0x0B
//#define cmdDistancia_Velocidad			0x0C
	#define CMD_AIR_UPDATE_DONE                 0x0B
	#define CMD_AIR_READ                    	0x0C


	#define CMD_RELOJ_Pase_a_LIBRE              0x18
	#define CMD_RELOJ_Pase_a_OCUPADO            0x19
	#define CMD_RELOJ_Pase_a_COBRANDO           0x1A
    #define CMD_RELOJ_Pase_a_FUERA_SERVICIO     0x1B
	#define CMD_RELOJ_Pase_a_OCUPADO_SA		    0x1C
	#define CMD_RELOJ_Pase_a_OCUPADO_BANDERA    0x1D
	#define CMD_RELOJ_Pase_a_OCUPADO_APP	    0x1E

	//#define CMD_RELOJ_Pase_a_LIBRE_SC           0x28
	//#define CMD_RELOJ_Pase_a_OCUPADO_SA_SC      0x2C
	//#define CMD_RELOJ_Pase_a_OCUPADO_APP_SC     0x2E
	//#define CMD_RELOJ_Pase_a_COBRANDO_SC        0x2A

	#define CMD_RELOJ_Pase_a_LIBRE_SC           0x18
	#define CMD_RELOJ_Pase_a_OCUPADO_SA_SC      0x1C
	#define CMD_RELOJ_Pase_a_OCUPADO_APP_SC     0x1E
	#define CMD_RELOJ_Pase_a_COBRANDO_SC        0x1A

	#define CMD_comandoTRANSPARENTE		0x40

		//SUBCOMANDOS

		//central -> reloj
		#define subCMD_ECO				0x01
		#define subCMD_VERSION			0x02
		#define subCMD_consultaVIAJE	0x03
		#define subCMD_consultaTURNO	0x04
		#define subCMD_progAIR			0x05
		#define subCMD_updateHora		0x06


		//reloj -> central
		#define subCMD_progSuccess		0x0B
		#define subCMD_datosSC			0x3F

		#define CMD_comandoMENSAJE			0x50

	#define CMD_RELOJ  ((CMD_a_Tx == CMD_RELOJ_Pase_a_LIBRE)			|| \
						(CMD_a_Tx == CMD_RELOJ_Pase_a_OCUPADO)			|| \
						(CMD_a_Tx == CMD_RELOJ_Pase_a_OCUPADO_SA)		|| \
						(CMD_a_Tx == CMD_RELOJ_Pase_a_OCUPADO_BANDERA)	|| \
						(CMD_a_Tx == CMD_RELOJ_Pase_a_OCUPADO_APP)		|| \
						(CMD_a_Tx == CMD_RELOJ_Pase_a_COBRANDO)			|| \
						(CMD_a_Tx == CMD_RELOJ_Pase_a_LIBRE_SC)			|| \
						(CMD_a_Tx == CMD_RELOJ_Pase_a_OCUPADO_SA_SC)	|| \
						(CMD_a_Tx == CMD_RELOJ_Pase_a_OCUPADO_APP_SC)	|| \
						(CMD_a_Tx == CMD_RELOJ_Pase_a_COBRANDO_SC)		|| \
						(CMD_a_Tx == CMD_RELOJ_Pase_a_FUERA_SERVICIO)	   \
						)




/* *************************************************************
 * COMANDOS DE RECEPCION DEL RELOJ-UNIK DEL DISPOSITIVO ANDROID
 *************************************************************** */

	#define cmdInicio_TURNO						0x01
	#define cmdPedido_Pase_LIBRE				0x03
	#define cmdPedido_Pase_OCUPADO				0x04
	#define cmdPedido_Pase_COBRANDO				0x05
	#define cmdInicio_EPERA						0x06
	#define cmdFin_ESPERA						0x07
	#define cmdImpresion						0x08
	#define cmdConsultaTarifas					0x09
	#define cmdConsultaEstado					0x0A
	#define cmdPase_fueraServicio				0x0B
    #define cmdEntra_DESCANSO					0x0C
	#define cmdSale_DESCANSO					0x0D
    #define cmdREPORTE_PARCIAL_Rx				0x0E

    #define cmdApp_CON_CONEXION_Rx				0x10
	#define cmdApp_SIN_CONEXION_Rx				0x11

	#define cmdPedido_Pase_LIBRE_SC				0x23
	#define cmdPedido_Pase_OCUPADO_SC			0x24
	#define cmdPedido_Pase_COBRANDO_SC			0x25

	#define cmdAsignado							0x30


#endif

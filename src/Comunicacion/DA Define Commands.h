
#ifndef _DA_DEFINE_COMMANDS_
  #define _DA_DEFINE_COMMANDS_

	#include "DA Communication.h"


#define okINICIO	0
#define noINICIO	1

#define okFIN		0
#define noFIN		1
#define		dim_comandoTRANSPARENTE_Buffer 		600
#define		dimMAX   							dim_comandoTRANSPARENTE_Buffer

	// Estructura de Puntero a Funciones de Transmisión
	typedef void (*typeTABLA_Tx)(void);

	extern typeTxCMD* const Tabla_ComandosTx[129];
	extern const typeTABLA_Tx Tabla_TxDA[];

	extern typeTxCMD CMD_NULL;

	extern typeTxCMD CMD_Encendido_EQUIPO;
	extern typeTxCMD CMD_Boton_EMERGENCIA;
	extern typeTxCMD CMD_Bajada_BANDERA;
	extern typeTxCMD CMD_Cuenta_FICHAS;
	//extern typeTxCMD CMD_PedidoPase_OCUPADO;
	extern typeTxCMD CMD_Resumen_VIAJE;
	extern typeTxCMD CMD_Velocidad_Posicion;
	extern typeTxCMD CMD_Status_RELOJ;
	extern typeTxCMD CMD_TRANSPARENTE;

	extern void Tx_minutos_ESPERA (void);
	//extern void Tx_Conf_finTURNO  (byte status);
	extern void Tx_Conf_inicioTURNO  (byte status);
	void Tx_Comando_MENSAJE(byte subCMD);


	extern void Tx_BUFFER_getN (void);
	extern void Tx_BUFFER (void);

	extern void Tx_NONE (void);

	extern void Tx_Encendido_EQUIPO  (void);
	extern void Tx_Valor_VIAJE (void);




	extern void Tx_Boton_EMERGENCIA (void);

	//extern void Tx_PedidoPase_OCUPADO (void);
	//extern void Tx_Distancia_Velocidad  (void);
	//extern void Tx_Velocidad_Posicion (void);
	//extern void Tx_Resumen_VIAJE  (void);

	extern void Tx_Status_RELOJ (void);
	extern void Tx_Comando_MENSAJE_fromBUFFER(byte N, byte* buffer, byte subCMD);


	//extern void Tx_Comando_TRANSPARENTE (void);
	void Tx_cmdTRANSPARENTE(byte N, byte* buffer);
	void Tx_Comando_TRANSPARENTE_prueba(void);
	void TxRta_conDATOS (byte dato);

#endif

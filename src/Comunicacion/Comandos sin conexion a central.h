/*
 * comandos sin conexions a central.h
 *
 *  Created on: 21/11/2019
 *      Author: Rai
 */

#ifndef RELOJ_COMANDOS_SIN_CONEXION_A_CENTRAL_H_
#define RELOJ_COMANDOS_SIN_CONEXION_A_CENTRAL_H_

#include "Definicion de tipos.h"
#include "Manejo de Buffers.h"

typedef union {
	byte Byte;
	struct{
		byte SC 							:1;  // 0: indica que se recibio comando de reloj con conexion, 1: indica que se recibio comando de reloj sin conexion
		byte TxDatosSC						:1;  //no hay transmision de datos tx en proceso;
		byte appConectada					:1;  //0: indica app desconectada de la central, 1:indica app conectada a central
		byte yaTrasmitioUltimo				:1;  //0:aun no transmitio ultimo comando 1: ya transmitio ultimo comando.
		byte esperoRepuesta_cmdReloj		:1;  //bloque transmision de comando guardos hasta recibir respuesta
	}Bits;
}t_cmdSC;

	typedef	struct{
		byte nro_viaje;
		byte estado_reloj
	}sc_data;


t_cmdSC	_f_SC;
	#define f_SC							_f_SC.Byte
	#define sinCONEXION_CENTRAL				_f_SC.Bits.SC
	#define Tx_datosSC						_f_SC.Bits.TxDatosSC
	#define appConectada_ACentral			_f_SC.Bits.appConectada
    #define yaTrasmitioUltimo_cmdReloj		_f_SC.Bits.yaTrasmitioUltimo
	#define esperarRespuesta_cmdReloj		_f_SC.Bits.esperoRepuesta_cmdReloj


extern void ini_acumular_cmdSC (void);
extern void check_datosSC(void);
extern void firstIniSc(void);


//extern void procesar_datosSC(void);
//extern void reTx_datosSC (void);


//extern void pruebaSC();


#define dim_bufferSC 80
extern buffWordTYPE	 datosSC;		//estructura de datos sin conexion para acumular  comandos sin condexion
extern word bufferSC[dim_bufferSC]; //buffer circular que acumula datos sin conexion


	#define datosSC_iniBUFFER	 datosSC.iniBUFFER
	#define datosSC_GETptr       datosSC.get
	#define datosSC_PUTptr       datosSC.put
	#define datosSC_cntWORD      datosSC.cntWORD
	#define datosSC_cntDATOS     datosSC.cntDATOS
	#define datosSC_SizeBUFFER   datosSC.sizeBUFFER
	#define checkCMDS_SC(X)	 	 check_cntBYTE_BUFCIR(X)

extern 	byte timeoutTx_datosSC;
#define TIMEOUT_datosSC 			15


extern void push_dataSC(sc_data data);
extern sc_data pull_dataSC();
extern void push_dataSC_inEeprom(sc_data dataSC);
extern byte load_regViaje(byte nro_viaje);



#endif /* RELOJ_COMANDOS_SIN_CONEXION_A_CENTRAL_H_ */

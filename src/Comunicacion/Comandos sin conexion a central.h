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
		byte SC 					:1;  //indica que no hay conexion con la central
		byte guardar_cmdSC 			:1;  //indica guadar comandos sin conexion con la central
		byte TxDatosSC			:1;   //no hay transmision de datos tx en proceso;
	}Bits;
}t_cmdSC;


t_cmdSC	_f_SC;
	#define f_SC							_f_SC.Byte
	#define sinCONEXION_CENTRAL				_f_SC.Bits.SC
	#define guardarCMD_sinCONEXION_CENTRAL	_f_SC.Bits.guardar_cmdSC
	#define Tx_datosSC						_f_SC.Bits.TxDatosSC

extern void acumular_cmdSC(byte nro_viaje);
extern void ini_acumular_cmdSC (void);
extern void procesar_datosSC(void);
extern void reTx_datosSC (void);

#define dim_bufferSC 100
extern buffcirTYPE	 datosSC;					//estructura de datos sin conexion para acumular  comandos sin condexion
extern byte 		 bufferSC[dim_bufferSC];    //buffer circular que acumula datos sin conexion

	#define datosSC_iniBUFFER	 datosSC.iniBUFFER
	#define datosSC_GETptr       datosSC.get
	#define datosSC_PUTptr       datosSC.put
	#define datosSC_cntBYTE      datosSC.cntBYTE
	#define datosSC_cntDATOS     datosSC.cntDATOS
	#define datosSC_SizeBUFFER   datosSC.sizeBUFFER
	#define checkCMDS_SC(X)	 	 check_cntBYTE_BUFCIR(X)

extern 	byte timeoutTx_datosSC;
#define TIMEOUT_datosSC 			15


#endif /* RELOJ_COMANDOS_SIN_CONEXION_A_CENTRAL_H_ */

/*
 * Rx-VA.h
 *
 *  Created on: 15/9/2017
 *      Author: Rai
 */

#ifndef COMUNICACION_CX___VISOR_ANDROID_RX_VA_H_
#define COMUNICACION_CX___VISOR_ANDROID_RX_VA_H_



#endif /* COMUNICACION_CX___VISOR_ANDROID_RX_VA_H_ */


// Fin de datos de Air Buffer
#define fin_datos_msb     0xDF
#define fin_datos_lsb     0x0A


extern void guardarRxDA_BaxFORMAT (void);
extern void RxDA_BaxFORMAT_toAIR_RxBuffer (void);

extern void VA_UART_ini (void);
extern void rxVA_baxFORMAT_TimeOut (void);


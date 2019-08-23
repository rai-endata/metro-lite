/*
 * Ticket Common.h
 *
 *  Created on: 14/3/2018
 *      Author: Rai
 */

#ifndef _TICKET_COMMON_H_
#define _TICKET_COMMON_H_

#include "stddef.h"
#include "stddef.h"
#include "calculos.h"
#include "DA Tx.h"
//#include "Param Reloj.h"
#include "Parametros Reloj.h"
#include "Grabaciones en EEPROM.h"

#include "Textos.h"

/*
 Set the character print mode: 0x1b 0x21 n
n										0 			1
0 font normal 						  Shall    character     00: shall  01: normal
1 undefined
2 undefined
3 bold 								  cancel    setting     00: cancel  0x04: setting
4 double high 						  cancel    setting		00: cancel  0x08: setting
5 double width 						  cancel 	setting		00: cancel  0x10: setting
6 undefined
7 underline 						  cancel 	setting		00:cancel	0x80: setting
 * */

typedef enum{
	NO_HAY_IMPRESION_EN_PROCESO,
	IMPRESION_EN_PROCESO
}tESTADO_IMPRESION;


extern uint8_t to_print_cnt;
extern tESTADO_IMPRESION statusPRINT;

extern void IniPRINT(uint8_t** ptrDouble);
extern void printCABECERA(uint8_t** ptrDouble, byte plusPRINT);
extern void printEMPRESA(uint8_t** ptrDouble);
extern void printLINE(ptrDouble);
extern void add_LF(uint8_t** ptrDouble);
extern void TICKET_importeToString (dword importe, byte* buffer);
extern byte TICKET_pasarSiguienteTarifa (byte* tarifaPTR);
extern void to_printing (void);
extern void habilitar_negrita(uint8_t** ptrDouble);
extern void deshabilitar_negrita(uint8_t** ptrDouble);
extern void printFONT_B(uint8_t** ptrDouble);
extern void enableFONT_ITALIC(uint8_t** ptrDouble);
extern void disableFONT_ITALIC(uint8_t** ptrDouble);
extern void enableBOLD(uint8_t** ptrDouble);
extern void disableBOLD(uint8_t** ptrDouble);
extern void georgianFONT(uint8_t** ptrDouble);
void Ajuste_Interlinea(uint8_t** ptrDouble, uint8_t n);
void setCharCode_spain(uint8_t** ptrDouble);


#define ESTA_IMPRIMIENDO     (statusPRINT == IMPRESION_EN_PROCEOS)
#define NO_ESTA_IMPRIMIENDO  (statusPRINT == NO_HAY_IMPRESION_EN_PROCESO)

#endif /* IMPRESORA_TICKET_COMMON_H_ */

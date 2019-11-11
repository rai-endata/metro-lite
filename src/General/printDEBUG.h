/*
 * printDEBUG.h
 *
 *  Created on: 30/11/2018
 *      Author: Rai
 */

#include "- metroBLUE Config -.h"

#ifndef PRINTDEBUG_H_
#define PRINTDEBUG_H_

#include "Manejo de Buffers.h"
#include "stdint.h"
#include "Print UART.h"
#include "Common Ticket.h"
#include "Reportes.h"


extern	void PRINT_DEBUG_encolarBYTE(uint8_t data);
extern	void PRINT_DEBUG_encolarBUFFER(uint8_t* buffer);
extern	void PRINT_DEBUG_imprimir (void);
extern	void printPRUEBA(uint8_t n);

#ifdef VISOR_REPORTES
	extern	void printCabecera_REGISTER_TAB_REPORTES(tREG_GENERIC* newReg_GETptr);
	extern void printLiOcFs_REGISTER_TAB_REPORTES(tREG_GENERIC* newReg_GETptr);
	//extern void printA_PAGAR_REGISTER_TAB_REPORTES(tREG_GENERIC* Reg_GETptr);
	extern void printA_PAGAR_REGISTER_TAB_REPORTES(tREG_A_PAGAR* regAPagar, uint8_t* ptr_espera);

	extern void printSESIONS_REGISTER_TAB_REPORTES(tREG_GENERIC* Reg_GETptr);
#endif

extern void printVARIABLE(uint32_t valor, byte* buffer, byte espacio, byte fin);
extern void printVARIABLE_VECTOR(byte* ptr_valor, byte* buffer, byte espacio, byte fin);
extern buffcirTYPE		printDEBUG;
	#define printDEBUG_BUFFERptr 	printDEBUG.iniBUFFER
	#define printDEBUG_GETptr      	printDEBUG.get
	#define printDEBUG_PUTptr      	printDEBUG.put
	#define printDEBUG_BYTEcnt    	printDEBUG.cntBYTE
	#define printDEBUG_DATOScnt   	printDEBUG.cntDATOS
	#define printDEBUG_BUFFERsize  	printDEBUG.sizeBUFFER

#endif /* PRINTDEBUG_H_ */

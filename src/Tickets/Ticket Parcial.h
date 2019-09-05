/*
 * Ticket Parcial.h
 *
 *  Created on: 14/3/2018
 *      Author: Rai
 */

#ifndef _TICKET_PARCIAL_H_
#define _TICKET_PARCIAL_H_
//#include "- Reloj-Blue Config -.h"
#include "- metroBLUE Config -.h"
#include "Reportes.h"

#define TO_PRINTING_TURNO		5
#define MENU_REPORTE_TURNO_index	0
#define max_turnosReporte           3        // Maximo de turnos a mostrar


#define PROG_TICKET_percentChofer     ((const) EEPROM_PROG_TICKET_RECAUD.chofer)

#ifdef VISOR_PRINT_TICKET
	extern void print_ticket_parcial(void);
	extern void TICKET_PARCIAL_setIni (tREG_SESION* addr_iniSESION, tREG_SESION* reg_iniSESION );
	extern uint16_t getViajes_Parcial(void);
	extern uint32_t getRecaudacion_Parcial(void);

#endif


#endif /* TICKET_PARCIAL_H_ */

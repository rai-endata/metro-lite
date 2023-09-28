/*
 * Ticket Turno.h
 *
 *  Created on: 14/3/2018
 *      Author: Rai
 */

#ifndef _TICKET_TURNO_H_
#define _TICKET_TURNO_H_
//#include "- Reloj-Blue Config -.h"
#include "- metroBLUE Config -.h"
#include "Reportes.h"

#define TO_PRINTING_TURNO		5
#define MENU_REPORTE_TURNO_index	0
#define max_turnosReporte           2        // Maximo de turnos a mostrar


#define PROG_TICKET_percentChofer     ((const) EEPROM_PROG_TICKET_RECAUD.chofer)

extern const byte TICKET_TURNO_estados[3];

extern tREG_SESION iniTURNO;
extern tREG_SESION finTURNO;
extern tREG_SESION* iniTURNO_ptr;      // Puntero a inicio de Turno del tipo SESION
extern tREG_SESION* finTURNO_ptr;      // Puntero a final de Turno del tipo SESION
extern tREG_SESION* sesion_ptrs[max_turnosReporte];
extern byte estadosTurno;


#ifdef VISOR_REPORTES
	extern void print_ticket_turno(uint8_t nroTurno);
	extern void TICKET_TURNO_setIni (tREG_SESION* addr_iniSESION, tREG_SESION* reg_iniSESION );
	extern void TICKET_TURNO_setFin (tREG_SESION* addr_finSESION, tREG_SESION* reg_finSESION );
	void REPORTES_calVarios (tREG_GENERIC* INI_ptr, tREG_GENERIC* FIN_ptr, uint32_t* datosMOV);

#endif

	extern byte TURNO_getVelMax_estado(byte tipo);
    extern uint32_t TURNO_getKM_estado(byte tipo);
    extern void TURNO_getTiempoMarcha_estado(byte* buffer, byte tipo);
    extern void TURNO_getTiempoParado_estado(byte* buffer, byte tipo);

    extern uint16_t TURNO_getCantViajes_tarifa(byte nroTarifa);
    extern uint32_t TURNO_getRecaudacion_tarifa(byte* buffer, byte nroTarifa);

    extern uint16_t TURNO_getCantViajes_turno(void);
    extern uint16_t TURNO_getCantViajesAsiento_turno(void);
    extern uint16_t TURNO_getCantErroresAsiento_turno(void);

    extern uint32_t TURNO_getRecaudacion_turno(byte* buffer);
    extern uint32_t TURNO_calcKmTotales_turno(void);
    extern uint32_t TURNO_calcRecaudacionChofer(byte* buffer);

    extern uint32_t TURNO_calcRecaudacionPorKm_turno(byte* buffer);
    extern uint32_t TURNO_calcPercentKmOcupado_turno(byte* buffer);

    extern uint16_t TURNO_getCantDesconexionesAlim_turno(void);
    extern uint32_t TURNO_calcTiempoDesconexionAlim_turno(byte* buffer);
    extern uint32_t TURNO_calcTiempoDesconexionAlim_turno_HEX(byte* buffer);
    extern uint32_t TURNO_getImportePerdido_tarifa(byte* buffer, byte nroTarifa);

    extern uint32_t TURNO_calcImportePerdido_turno(byte* buffer);
    extern uint16_t TURNO_getCantDesconexionesAcc_turno(void);
    extern uint32_t TURNO_calcTiempoDesconexionAcc_turno(byte* buffer);
    extern uint32_t TURNO_getImportePerdidoAcc_tarifa(byte* buffer, byte nroTarifa);
    extern uint32_t TURNO_calcImportePerdidoAcc_turno(byte* buffer);
    extern uint16_t TURNO_getCantMovSinPulsos_turno(void);

 #ifdef    DESCONEXIONES_TOTALES_EN_TICKET_TURNO
    extern uint16_t TURNO_getCantDesconexionesAlim_TOTAL(void);
    extern uint32_t TURNO_calcTiempoDesconexionAlim_TOTAL(byte* buffer);
    extern uint32_t TURNO_getImportePerdido_tarifa_TOTAL(byte* buffer, byte nroTarifa);
    extern uint32_t TURNO_calcImportePerdido_TOTAL(byte* buffer);

    extern uint16_t  TURNO_getCantDesconexionesAcc_TOTAL(void);
    extern uint32_t TURNO_calcTiempoDesconexionAcc_TOTAL(byte* buffer);
    extern uint32_t TURNO_getImportePerdidoAcc_TOTAL_tarifa(byte* buffer, byte nroTarifa);
    extern uint32_t TURNO_calcImportePerdidoAcc_TOTAL(byte* buffer);

    extern uint16_t TURNO_getCantMovSinPulsos_TOTAL(void);
 #endif


#endif /* IMPRESORA_TICKET_TURNO_H_ */

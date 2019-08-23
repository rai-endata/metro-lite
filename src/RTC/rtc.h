/*
 * rtc.h
 *
 *  Created on: 4/10/2017
 *      Author: Rai
 */

#ifndef _RTC_H_
#define _RTC_H_

typedef union {
	struct{
		uint32_t rtc_backup_register0		:32;	// registro para backup del rtc
		uint32_t rtc_backup_register1		:32;	// registro para backup del rtc
		uint32_t rtc_backup_register2		:32;	// registro para backup del rtc
		uint32_t rtc_backup_register3		:32;	// registro para backup del rtc
		uint32_t rtc_backup_register4		:32;	// registro para backup del rtc
	}registers;

	struct {
		//register0
		uint32_t hora         		  :8;		// Agrupacion de
		uint32_t minuto       	      :8;		// Agrupacion de
		uint32_t segundo     		  :8;		// Agrupacion de
		uint32_t dia	      		  :8;		// Agrupacion de
		//register1
		uint32_t mes	      		  :8;		// Agrupacion de
		uint32_t anio	      		  :8;		// Agrupacion de
		uint32_t tarifa	      		  :4;		// Agrupacion de
		uint32_t estado_reloj 		  :4;		// Agrupacion de
		uint32_t dif_espera			  :8;		// Agrupacion de
		//register2
		uint32_t fichas_pulsos  	  :16;	// Agrupacion de
		uint32_t fichas_tiempo  	  :16;	// Agrupacion de
		//register3
		uint32_t contador_pulsos  	  :32;	// Agrupacion de
		//register4
		uint32_t contador_pulsos_old  :32;	// Agrupacion de
	}backupRTC;

}tBACKUP_RTC;


typedef struct {
	uint8_t 		tiempo_corte_alimentacion;					//menos de 15 seg
	uint8_t 		tarifa;
	uint8_t 		estado_reloj;
	uint8_t 		BYTE_TO_FILL1;
	uint16_t		velocidad_maxima;
	uint16_t		segundos_espera;
	uint16_t		segundos_tarifacion;
	uint16_t		WORD_TO_FILL1;
	uint32_t 		fichas_pulsos;
	uint32_t 		fichas_tiempo;
	uint32_t 		contador_pulsos;
	uint32_t 		contador_pulsos_old;
	uint32_t		distancia_ocupado;
	tDATE			ocupado_date;
	tDATE			cobrando_date;
	tDATE   		date;
}backupEEPROM;



#include "Definicion de tipos.h"

extern tDATE RTC_Date;
extern tDATE RTC_Date;
extern tDATE DateApagado;                        // Fecha y Hora de APAGADO

extern tDATE* DATE_ptr;                 // Puntero para fuente de FECHA y HORA
extern tDATE HoraApagado;

extern tDATE getDate (void);
extern uint32_t rtc_read_backup_reg(uint32_t BackupRegister);
extern void rtc_write_backup_reg(uint32_t BackupRegister, uint32_t data);


extern void read_backup_registers(void);
extern void write_backup_registers(void);
extern void test_backup_registers(void);
extern void write_backup_rtc(void);
extern void clean_backup_rtc(void);
extern void get_backup_HoraApagado(void);
extern void write_backup_rtcCONTADOR_PULSOS(void);
extern void RTC_updateDate (byte* new_date);

#endif /* RTC_RTC_H_ */

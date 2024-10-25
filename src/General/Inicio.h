#ifndef _INICIO_
 #define _INICIO_

#include "DA Communication.h"

// BANDERAS GENERALES 1
typedef union {
  byte Byte;
  struct{
	byte microCorte        :1;        // Indica que ocurrio un microCorte de Alimentacion
	byte firstDate         :1;        // Indica que obtuve el primer Date
	byte programacion      :1;        // Reset por programacion
	byte firstChk          :1;    // indica preimer chequeo si reloj esta bloqueado
	byte pvd_off		       :1;	  //indica que ya llego la interrupcion de PVD (Programmable voltage detector)
  }Bits;
}inicioFLAGS1;

extern byte corteALIMENTACION;
extern byte viajeInconcluso;

extern inicioFLAGS1             	_inicio_F1;
     #define inicio_F1             	_inicio_F1.Byte
     #define INICIO_microCorte     	_inicio_F1.Bits.microCorte
     #define INICIO_firstDate      	_inicio_F1.Bits.firstDate
     #define INICIO_programacion    _inicio_F1.Bits.programacion
     #define firstChk_bloqueoReloj 	_inicio_F1.Bits.firstChk
	 #define PVD_OFF			 	_inicio_F1.Bits.pvd_off


#define valorPrimerEncendido    0x55AA
#define EEPROM_PRIMER_ENCENDIDO ADDR_EEPROM_PRIMER_ENCENDIDO

#define rxCorteLargoPrevio           0x0000
#define noRxCorteLargoPrevio         0x0100
#define noRxCorteCortoPrevio         0x0200
#define EEPROM_YA_HUBO_CORTE_LARGO 	ADDR_EEPROM_YA_HUBO_CORTE_LARGO

#define EEPROM_libreDATE 	ADDR_EEPROM_LIBRE_DATE


extern tDATE HoraApagado;
extern byte motivo_reset;
extern byte	corteALIMENTACION;

extern void firstDATE (void);
extern void SOURCE_DATE_Ini(void);
extern byte dateValid (tDATE date);
extern void actualizarPrimerEncendido (void);
extern void borrarPrimerEncendido (void);
extern void determinePrimerEncendido (void);

extern void check_corte_alimentacion(void);
extern void set_rxCorteNormal(void);
extern void set_noRxCorteLargoPrevio(byte estado_reloj);
extern void set_noRxCorteCortoPrevio(byte estado_reloj);

#endif

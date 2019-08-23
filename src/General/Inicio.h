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

extern byte 		corteALIMENTACION;
extern inicioFLAGS1             	_inicio_F1;
     #define inicio_F1             	_inicio_F1.Byte
     #define INICIO_microCorte     	_inicio_F1.Bits.microCorte
     #define INICIO_firstDate      	_inicio_F1.Bits.firstDate
     #define INICIO_programacion    _inicio_F1.Bits.programacion
     #define firstChk_bloqueoReloj 	_inicio_F1.Bits.firstChk
	 #define PVD_OFF			 	_inicio_F1.Bits.pvd_off


#define valorPrimerEncendido      0x55AA
//const uint16_t FLASH_PRIMER_ENCENDIDO = {NULL};
//const uint16_t FLASH_PRIMER_ENCENDIDO[] = {0x0000};
#define EEPROM_PRIMER_ENCENDIDO   ADDR_EEPROM_PRIMER_ENCENDIDO


extern tDATE HoraApagado;
extern byte motivo_reset;
extern void firstDATE (void);
extern void SOURCE_DATE_Ini(void);
extern byte dateValid (tDATE date);
extern void actualizarPrimerEncendido (void);
extern void borrarPrimerEncendido (void);
extern void determinePrimerEncendido (void);

extern void check_corte_alimentacion(void);

#endif

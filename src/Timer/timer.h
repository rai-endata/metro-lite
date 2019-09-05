
#ifndef _TIMER_
  #define _TIMER_

#include "Definicion de tipos.h"



/*********************************************************************************************/
/* ESTRUCTURAS */
/***************/

// TIMER GENERAL FLAGS
typedef union {
  byte Byte;
  struct {
	byte  lazo_hab    :1;	// Habilitación del TO del LAZO
	byte  lazo_F      :1;   // Bandera del TO de LAZO
	byte  _1mseg      :1;   // Se Cumplieron 1mseg
	byte  _10mseg     :1;   // Se Cumplieron 10mseg
	byte  _16mseg     :1;   // Se Cumplieron 16mseg
	byte  _100mseg    :1;   // Se Cumplieron 100mseg
	byte  _500mseg    :1;   // Se Cumplieron 500mseg
	byte  _1seg       :1;   // Se Cumplió 1seg
  } Bits;

  struct {
	byte          :8;
  } MergedBits;
} tmr_gral_flags;

extern byte   checkTIME;


   /* TIMEOUT DE LAZO */
   /*******************/
  extern void dispararTO_lazo (void);
  extern void detenerTO_lazo (void);
  extern byte chkTO_lazo_F (void);

#endif  

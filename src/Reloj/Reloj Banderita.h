/*
 * Reloj Banderita.h
 *
 *  Created on: 12/11/2018
 *      Author: Rai
 */

/* File: <Reloj Banderita.h> */



#ifndef _RELOJ_BANDERITA_
  #define _RELOJ_BANDERITA_

#define PIN_BANDERITA                  GPIO_PIN_4
#define PUERTO_BANDERITA               GPIOB
#define BANDERITA_CLK_ENABLE()       __GPIOB_CLK_ENABLE()
#define BANDERITA_CLK_DISABLE()      __GPIOB_CLK_DISABLE()

  /* INCLUDE */
  /***********/
    #include "Programacion Parametros.h"
    #include "Reloj.h"


	// ESTADOS DE RELOJ
	typedef enum{
		LIBRE_BANDERITA,
		OCUPADO_BANDERITA,
		COBRANDO_BANDERITA
	}tRELOJ_BANDERITA;

  /* RUTINAS */
  /***********/
    extern void check_relojBANDERITA (void);
    extern GPIO_PinState check_pressBANDERITA(void);
    extern void BANDERITA_actualizar (void);
    //extern byte LOGICA_BANDERITA;                // No es un parametro de programacion, pero se deduce de el
    extern void Ini_portBANDERITA (void);

  /* TIMER */
  /*********/
    extern void timer_muestreo_banderita (void);


#endif

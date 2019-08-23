/* File: <Calculo Velocidad ACUMM.h> */

#ifndef _CALCULO_VELOCIDAD_ACUMM_
  #define _CALCULO_VELOCIDAD_ACUMM_

	#include "Definicion de tipos.h"

/*********************************************************************************************/
/* ESTRUCTURAS */
/***************/
  // VELOCIDAD FLAG
/*
typedef union {
        byte Byte;
        struct {
          byte do_calc_vel        :1;   // Indica que se debe calcular la velocidad, para no calcular si no hay pulsos
          byte timer_calc_vel_F   :1;   // Bandera que habilita el calculo de la velocidad
          byte dont_calc_vel      :1;   // Inhibe calculo de la velocidad y reinicia calculo de periodo de pulsos
          byte almacenado_done    :1;   // Indica que se ya se almaceno la cantidad necesaria de periodos
          byte ya_llego_unPULSO	  :1;   // Apenas se mueve descarto el primer pulso, en el calculo de Velocidad.
        } Bits;
  }VELOCIDAD_FLAGS;
*/

  /* VARIABLES */
  /*************/

//  VELOCIDAD_FLAGS                  _velocidad_F;  // Banderas de Calculo de Velocidad
  /*
     #define velocidad_F                   	_velocidad_F.Byte
     #define do_calculo_velocidad          	_velocidad_F.Bits.do_calc_vel
     #define timer_calculo_velocidad_F     	_velocidad_F.Bits.timer_calc_vel_F
	 #define no_calcular_velocidad         	_velocidad_F.Bits.dont_calc_vel
     #define frecuencias_almacenadas_done  	_velocidad_F.Bits.almacenado_done
     #define yaLLEGO_unPULSO				_velocidad_F.Bits.ya_llego_unPULSO

		extern word VELOCIDAD;
		extern word  VELOCIDAD_NEW;
		extern word VELOCIDAD_MAX_NEW;

		#define VELCOCIDAD_PERMITE_CAMBIO_RELOJ   (VELOCIDAD < 1000)  // Si la velocidad es menor a 10.00km/h se permiten los cambios de Reloj
		
		extern word VELOCIDAD_MAX;
		  #define resetVELOCIDAD_MAX              (VELOCIDAD_MAX = 0)
		
		#define decimalesVelocidad        2       //esta en km/h con 2 decimales

		extern word pulsos_cnt;
  */
  /* RUTINAS */
  /***********/    
  /*
  extern void VELOCIDAD_iniCalculo (void);
    extern void calculoVELOCIDAD (void);

    extern void VELOCIDAD_inhibirCalculo (void);

    extern 	void VELOCIDAD_almacenarFreq (dword frecuencia);
    extern  byte VELOCIDAD_chkRuidoMedicion (dword frecuencia_actual);
    extern  dword VELOCIDAD_promediarFreq (void);
    extern  void VELOCIDAD_borrarFreqAlmacenadas (void);
    extern  void VELOCIDAD_reiniCalculoPeridoPulsos (void);
    extern  void VELOCIDAD_calculo_porPeriodo (void);
    extern  void VELOCIDAD_redispararTimeOutPulsos(void);
    extern  void determineVELOCIDAD_MAX (void);
    extern  void cal_freq_pulsos(void);

*/
  
  /* TIMER */
  /*********/    
  /*
  extern void time_out_calculo_periodo (void);
    extern void timer_calculo_periodo (void);    
*/
#endif

//File: <Odometro.h>

#ifndef _ODOMETRO_
  #define _ODOMETRO_

 	#include "stm32f0xx_hal_tim.h"
	#include "Definicion de tipos.h"
	#include "Calculo Velocidad.h"
 
 
  /* ESTRUCTURAS */
  /***************/    
  
  typedef union {
        byte Byte;
        struct {
          byte do_calc_vel        :1;   // Indica que se debe calcular la velocidad, para no calcular si no hay pulsos
          byte dism_vel           :1;   // Indica que disminuyo la velocidad por debajo del valor de HISTERESIS
          byte do_calc_acel       :1;   // Indica que ya se puede calcular la aceleración
          byte dont_calc_vel      :1;   // Inhibe calculo de la velocidad y reinicia calculo de periodo de pulsos
          byte almacenado_done    :1;   // Indica que se ya se almaceno la cantidad necesaria de periodos
          byte timer_calc_vel_F   :1;   // Bandera que habilita el calculo de la velocidad
          byte espaciar_IRQs      :1;   // Para el calculo periódico de velocidad
          byte :1;
        } Bits;
  } vel_F;

 


  typedef union {
        byte Byte;
        struct {
          byte typeVISOR        :1;   // indica si el proyecto que usa el modulo es visor compact o visot PLUS
        } Bits;
  }odometro_F;


  /* VARIABLES */
  /*************/

  //extern UART_HandleTypeDef huart1;
	extern uint32_t DISTANCIAm;
	extern uint16_t  DISTANCIAkm;
	extern uint32_t DISTANCIAm_OCUPADO;
	extern uint32_t DISTANCIAm_LIBRE;

	extern TIM_HandleTypeDef pulsoACCUM;

	extern uint32_t PULSE_ACCUM_CNT;
		  

 //ODOMETRO
   extern odometro_F   _odometro_F;          // Banderas de Calculo de Velocidad 
    #define odometro_F                   _odometro_F.Byte
    #define useVISOR                    _odometro_F.Bits.typeVISOR
      #define     visorCOMPACT        0
      #define     visorPLUS           1

   #define VELCOCIDAD_PERMITE_CAMBIO_RELOJ   (VELOCIDAD < 1000)  // Si la velocidad es menor a 10.00km/h se permiten los cambios de Reloj


//	#define PULSOS_x_KM          100
//	#define CARRERA_BANDERA		  10

    //extern uint16_t PULSOS_x_KM;
	//extern uint16_t CARRERA_BANDERA;


	extern uint32_t frecuencia;
	extern uint32_t t_pulsos;
	extern uint32_t t_pulsos_anterior;
	extern uint16_t  VELOCIDAD;										  	      // Velocidad del equipo en km/h, con 2 decimales
	extern uint16_t  VELOCIDAD_MAX;									        // Velocidad MAXIMA del equipo en km/h, con 2 decimales
		#define resetVELOCIDAD_MAX              (VELOCIDAD_MAX = 0)

	extern uint16_t HISTERESIS_VT;                   // Histeresis de la Velocidad de Transicion
	extern uint16_t VELOCIDAD_TRANSICION;            // Velocidad de Transision en km/h, con 2 deciamles

	extern byte timeOut_FRECUENCIA_cnt;

    #define  pin_inputCAPTURE                1
    #define  pin_PULSE_ACCUM                 0
  /* RUTINAS */
  /***********/    
  

  extern void PULSOS_ini (void);

  //extern byte ODOMETRO_sendBackupData (void);
  //extern void ODOMETRO_getBackupData (void);
  extern  uint16_t calculoDISTANCIA_PULSE_ACCUM (byte dist_type);
	#define calcularDISTANCIA_entreEstados    calculoDISTANCIA_PULSE_ACCUM(1)
	#define calcularDISTANCIA_acumulada       calculoDISTANCIA_PULSE_ACCUM(0)

  //extern  uint16_t calcularDISTANCIA_acumulada (void);
  //extern  uint16_t calcularDISTANCIA_entreEstados (void);
  void DISTANCIA_setPulsosCntOld (uint32_t value);
  void timeOut_FRECUENCIA (void);
  extern uint32_t pulsos_cnt_old;

  extern void DISTANCIA_iniCalculo_PULSE_ACCUM (void);

  //extern  void CALC_CONST_resetPulsos (void);
  extern void resetPULSOS (void);
  extern uint16_t cal_dist (void);
	
/* TIMER */
  /*********/    
    extern void timer_calculo_periodo (void);
    extern void MX_TIM2_Init(void);
    extern void MX_TIM2_Init1(void);
    extern void MX_TIM2_Init3(void);
    extern void MX_TIM2_Init4(void);

    extern TIM_HandleTypeDef htim2;

#endif  

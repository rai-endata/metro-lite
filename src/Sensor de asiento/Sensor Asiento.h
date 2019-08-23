/* File <sensor_asiento.h> */


 #ifndef _SENSOR_ASIENTO_
 #define _SENSOR_ASIENTO_
 #include "definicion de tipos.h"
 #include "Parametros Reloj.h"


/* INCLUDE */
/***********/

/* RUTINAS */
/***********/
//#define TIPO_SENSOR_ASIENTO       ((const)EEPROM_PROG_relojCOMUN.varios1.MergedBits.tipo_sensor)
/*
#define TIPO_SENSOR_ASIENTO      1
#define SIN_SENSOR_ASIENTO      (TIPO_SENSOR_ASIENTO == 0)
#define SENSOR_ASIENTO_NORM     (TIPO_SENSOR_ASIENTO == 1)
#define SENSOR_ASIENTO_INTEL    (TIPO_SENSOR_ASIENTO == 2)
*/

//define PIN PULSADOR_IMPRESION
#define SENSOR_ASIENTO_PIN                 		   GPIO_PIN_8
#define SENSOR_ASIENTO_GPIO_PORT                   GPIOB
#define SENSOR_ASIENTO_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOB_CLK_ENABLE()
#define SENSOR_ASIENTO_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOB_CLK_DISABLE()
#define SENSOR_ASIENTO_EXTI_LINE                   GPIO_PIN_8
#define SENSOR_ASIENTO_EXTI_IRQn                   EXTI4_15_IRQHandler


  #define asiento_LOW             0     // Por los inversores del orto
  #define asiento_HIGH            1
  
  #define dim_SerialAsiento         8

  extern void ASIENTO_ini (void);
  extern void ini_puerto_sensor_asiento(void);
  //extern void ASIENTO_ocuparseAfterTurnOn (void);
  extern void ASIENTO_consultaSerial (void);
  extern void ASIENTO_consultaSensor (void);
  extern byte ASIENTO_getPressStatus (void);
  

/* RUTINAS EEPROM */
/******************/


/* RUTINAS FLASH */
/*****************/
  extern void ASIENTO_turnOnByAsiento_enFLASH (byte forced);
  


/* TIMER */
/*********/
  extern void ASIENTO_chkSerial_timer (void); 
  extern void ASIENTO_chkSensor_timer (void);
  extern void ASIENTO_desrebote_timer (void);
  extern void ASIENTO_chkTiempo_sensor (void);

#endif

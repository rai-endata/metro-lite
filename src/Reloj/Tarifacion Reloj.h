/* File: <Tarifacion Reloj.h> */

#ifndef _TARIFACION_RELOJ_
  #define _TARIFACION_RELOJ_
	#define chkSIN_PULSOS   (SinPulsos == 1)
	#define SinPulsos  1

  #define TARIFA_BACKUP				0
  #define FICHAS_xPULSOS_BACKUP		1          // Cantidad de Fichas que cayeron por PULSOS
  #define FICHAS_xTIEMPO_BACKUP     2     // Cantidad de Fichas que cayeron por TIEMPO

  /* INCLUDES */
  /************/
	//#include "Tarifacion Reloj.h"
	//#include "Param Reloj.h"
	#include "Parametros Reloj.h"


  /* VARIABLES */
  /*************/
    extern tTARIFA   TARIFA;
    extern ttTARIFA	  TARIFA1,TARIFA2,TARIFA3,TARIFA4;
    extern ttEQPESOS EqPesosDiurno;
    extern ttEQPESOS EqPesosNocturno;
    extern tEQPESOS	EqPesosD;
    extern tEQPESOS	EqPesosN;

    extern uint32_t VALOR_VIAJE;
    extern uint32_t VALOR_VIAJE_PACTADO;
    extern uint8_t puntoDecimal_PACTADO;
    extern uint32_t VALOR_VIAJE_mostrar;			   // Valor del Viaje a mostrar en la app del celular
    extern uint32_t fichas_xPulsos;          // Cantidad de Fichas que cayeron por PULSOS
    extern uint32_t fichas_xTiempo;          // Cantidad de Fichas que cayeron por TIEMPO
    extern uint32_t fichas_Total;
    extern byte tarifa;

		extern uint32_t pulsosTarifacion;
		extern uint16_t segundosTarifacion;
		extern uint16_t segundosEspera;
		extern uint8_t  dif_espera;

		extern uint16_t TIEMPO_FICHA_INICIAL;
      
  /* RUTINAS */
  /***********/
    extern void RELOJ_iniTarifacion (void);
    
    extern uint32_t RELOJ_calcularImporte (uint32_t fichasT, uint32_t fichasD, byte viajes, byte nroTarifa, byte* buffer);
    
    extern void RELOJ_setTarifa (byte nro);
    extern void RELOJ_setTarifa_kate (byte nro);
    extern void RELOJ_setNUMTarifa (byte nro);
    
    extern void RELOJ_calcCantidadPulsosFicha(void);
    extern void RELOJ_calcCantidadPulsosBajada(void);
    extern void RELOJ_calcTiempoFichaInicial (void);
    
    extern void RELOJ_tarifacion (void);
    extern void tarifarRELOJ (void);
    extern void RELOJ_iniPulsos (void);
    
    extern uint32_t TARIFACION_getFichasT (void);
    extern uint32_t TARIFACION_getFichasD (void);
    
    // VELOCIDAD DE TRANSICION
    extern void RELOJ_chkVelTransicion (void);
    
    // TARIFACION POR TIEMPO
    extern void RELOJ_toggleEspera (void);
    
    // BACKUP EN RTC
    extern byte TARIFACION_sendBackupData (void);
    //extern void TARIFACION_getBackupData (void);

    extern void set_tarifPulsos (void);

    extern	uint32_t recuperar_valor_viaje_backup(void);
    extern  uint32_t recuperar_fichas_xtiempo_backup(void);
    extern  uint32_t recuperar_fichas_xpulsos_backup(void);
    extern  uint8_t  recuperar_tarifa_backup(void);
    extern  void 	 guardar_fichas_xtiempo_backup(uint32_t fichas_tiempo);
    extern  void 	 guardar_fichas_xpulsos_backup(uint32_t fichas_pulsos);
    extern  void 	 guardar_tarifa_backup(tarifa);
    extern void 	 RedondeoValorViaje_3(void);
    extern void      RedondeoValorViaje_2(void);
    extern uint8_t   tarifando_tiempo(void);


  /* TIMER */
  /*********/
    extern void timerCHK_pulsosTarifacion (void);
    extern void timer_TarifacionTiempo (void);
   // extern tTARIFA* prgRELOJ_getTarifa_pointer (byte nro);
#endif

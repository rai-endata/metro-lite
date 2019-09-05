/* File: <- metroBLUE Config -.h> */

#ifndef _metroBLUE_CONFIG_
  #define _metroBLUE_CONFIG_

	//*****  Equipo  ************************************************
	#define metroBLUE

	/* ******** Versionamiento Semantico **************************
    vX.Y.Z

	X: Major, version_major	((uint16_t)0x01) //se incrementa cuando cambia compatibilidad
	y: Major, version_minor	((uint16_t)0x00) //se incrementa cuando cambia funcionalidad vuelve a cero cuando se incrementa major
	Z: Major, se incrementa cuando se arregla algun/os errores, vuelve a cero cuando se incrementa minor o major
    */

    #define version_firmware   v1.0.0


	//*****  Localidad  *********************************************

    /* MONTEVIDEO */        #define _TAXI_141_



	//*****  MODULOS POR LOCALIDAD  *********************************

#ifdef _TAXI_141_

   	#define     STM32F091RC

	#define		VISOR_REPORTES
	//#define 	VISOR_REPORTE_ACUMULATIVO
	#define		VISOR_PRINT_TICKET_FULL
	#define		VISOR_PRINT_TICKET
	#define		VISOR_PROG_TICKET
	#define		_ABONAR_SEGUN_TARIFA_HORA_A_PAGAR_
	#define		DESCONEXIONES_TOTALES_EN_TICKET_TURNO
	//#define		MOSTRAR_IMPORTE   1				//muestra importe
	#define		MOSTRAR_IMPORTE	  0			//muestra fichas
	#define		RELOJ_HOMOLOGADO
	#define		VISOR_CHK_JUMPER_PROG

	#define	    HACER_REDONDEO
	//#define		RELOJ_DEBUG
	#define		HABILITA_REINTENTOS

#endif



	//******************* CONTROL DE COMPILACION *******************

	// Al menos se debe seleccionar una localidad
	#ifndef VISOR_LOCALIDAD
//	  #error "Debe elegir una localidad para la compilacion"
	#endif
	
	

#endif

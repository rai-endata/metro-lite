/* File: <Reporte 30 Dias.h> */


#ifndef _REPORTE_30DIAS_
  #define _REPORTE_30DIAS_
  
  /* INCLUDES */
  /************/
  #include "- metroBLUE Config -.h"
#ifdef VISOR_REPORTE_30DIAS


  
 #include "Reportes.h"

  /* ESTRUCTURAS */
  /***************/
  // TIPO DE DATOS RECAUDADO
    typedef struct {
      byte    viajes;                   // Cantidad de Viajes
      byte    fichasDist[3];            // Cantidad de Fichas de Distancia
      byte    fichasTime[3];            // Cantidad de Fichas de Tiempo
      dword   importe;                  // Importe
    }tREC;
  
  
  // TIPO DE REPORTE 30 DIAS POR CHOFER
  // debe tener longitud PAR. Me parece que no es necesario que sea par
    typedef struct {
      byte    viajesAsiento;            // Cantidad de Viajes por Sensor de Asiento
      byte    erroresAsiento;           // Cantidad de Errores de Sensor de Asiento
      dword   KM_libre;                 // KM Libre con 1decimal
      dword   KM_ocupado;               // KM Ocupado con 1decimal
      dword   KM_fserv;                 // KM Fuera Servicio con 1decimal
      byte    cantDesconAlim;           // Cantidad de Desconexiones de Alimentacion
      byte    segDesconAlim[3];         // Segundos de Alimentacion desconectado
      byte    cantDesconAcc;            // Cantidad de Desconexiones de Accesorio
      byte    segDesconAcc[3];          // Segundos de Accesorio Desconectado
      dword   importePerdido;           // Importe Perdido
      dword   importePerdidoAcc;        // Importe Perdido por Desconexion de Accesorio
      tREC    tarifa[9];                // Datos de cada tarifa +1 por tarifa especial
    }tREG_CHOFER_30DIAS;  

    
  // TIPO DE REPORTE 30 DIAS
  // debe tener longitud PAR
    typedef struct {
      word    idx;                      // Indice
      byte    fecha[3];                 // Fecha
      tREG_CHOFER_30DIAS    chofer[4];  // Datos del Chofer
      byte    empty;                    // Relleno para longitud PAR
    }tREG_30DIAS;


    #define dim_REPORTE_30DIAS    30    // Cantidad de registros (eventos)
  
  
  /* RUTINAS */
  /***********/
    extern byte REPORTE_30DIAS_chkIntigrity (void);
    extern void REPORTE_30DIAS_ini (byte reset);
    extern void REPORTE_30DIAS_addData (tREG_GENERIC* regPTR);
    
    extern tREG_30DIAS*far REPORTE_30DIAS_getPUT_ptr (void);
    extern byte REPORTE_30DIAS_determineFin (tREG_30DIAS*far* ptr_ptr);
    
    extern void incFlashRep30_ptr (tREG_30DIAS*far* ptr_ptr);
    extern void decFlashRep30_ptr (tREG_30DIAS*far* ptr_ptr);

  #endif
#endif

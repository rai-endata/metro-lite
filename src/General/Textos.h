/* File: <Textos.h> */


#ifndef _TEXTOS_
  #define _TEXTOS_
  /* INCLUDES */
  /************/
#include "- metroBLUE Config -.h"
#include "Definicion de tipos.h"
  /* CONSTANTES */
  /**************/
  
    /* CONFIGURACION INICIAL */
    /*************************/
      extern const byte  _INICIANDO_EQUIPO_POR_PRIMERA_VEZ[];
      extern const byte  _POR_FAVOR_ESPERE[];
    
    /* AVISOS */
    /**********/
      extern const byte  _Espere_conexion[];
      extern const byte  _con_la_Central[];
      
      extern const byte  _Direccion[];
      extern const byte  _no_Disponible[];
      
      extern const byte  _Para_DESLOGUEARSE[];
      extern const byte  _pase_a_Libre_o_a[];
      extern const byte  _Fuera_de_Servicio[];
      
      extern const byte  _Logueandose[];
      extern const byte  _Por_favor_espere[];
      
      extern const byte  _Reloj_Desconectado[];
      
      extern const byte  _Sincronizando[];
      
      extern const byte  _Despacho[];
      extern const byte  _Suspendido[];
      
      extern const byte  _Hora_RTC_INVALIDA[];
      extern const byte  _comuniquese_con[];
      extern const byte  _Servicio_Tecnico[];
      
      extern const byte  _RTC_NO_RESPONDE[];
      extern const byte  _se_utilizara_la[];
      extern const byte  _hora_del_GPS[];
      
      extern const byte  _Volumen_de_Buzzer[];
		  extern const byte  _seteado_en[];
		  extern const byte  _ALTO[];
		  extern const byte  _BAJO[];
      
      extern const byte  _Remate[];
		  extern const byte  _Viaje_en[];
		  
		#ifdef VISOR_TARJETA
		  extern const byte  _Por_favor_pase_la[];
      extern const byte  _tarjeta_nuevamente[];
    #endif
      
    #ifdef VISOR_TARJETA
      extern const byte  _Consultando_tarjeta[];
      extern const byte  _Validando_tarjeta[];
    #endif
    
    #ifdef VISOR_REPORTES
      extern const byte  _Esta_a_punto_de[];
      extern const byte  _iniciar_un_nuevo[];
      extern const byte  _turno_Continuar[];
    #endif
    
    #ifdef VISOR_REPORTES
      extern const byte _Ha_iniciado_un[];
      extern const byte _nuevo_turno_con[];
      extern const byte _el_chofer[];
    #endif
    
    #ifdef VISOR_TARJETA
      extern const byte _La_tarjeta_ha[];
    	extern const byte _sido_RECHAZADA[];
    	extern const byte _Motivo[];
    	extern const byte _SIN_SALDO[];
    	extern const byte _INVALIDA[];
    	extern const byte _ROBADA[];
    	extern const byte _EXPIRADA[];
    	extern const byte _EXCEDIDA[];
    	extern const byte _DESCONOCIDO[];
    	extern const byte _No_se_puede[];
		  extern const byte _recargar_desde[];
    #endif
    
    #ifdef VISOR_TARJETA
		  extern const byte _Ha_alcanzado_la[];
  		extern const byte  _cantidad_maxima[];
  		extern const byte  _de_transacciones[];
  		extern const byte  _permitidas[];
    #endif
    
    #ifdef VISOR_TARJETA
  		extern const byte _Numero_de[];
  		extern const byte _tarjeta_incorrecto[];
    #endif

    #ifdef VISOR_TARJETA
  		extern const byte _Codigo_de[];
  		extern const byte _Seguridad[];
  		extern const byte _incorrecto[];
    #endif

    #ifdef VISOR_TARJETA
  		extern const byte _Ha_alcanzado_el[];
  		extern const byte _limite_de_consumo[];
  		extern const byte _permitido[];
    #endif

    #ifdef VISOR_TARJETA
  		extern const byte  _El_viaje_ya_ha[];
		  extern const byte _sido_pagado_con[];
		  extern const byte _tarjeta[];
    #endif
    
    #ifdef VISOR_REPORTES	
    	extern const byte  _Memoria_de_Reportes[];
		  extern const byte  _proxima_a_llenarse[];
		  extern const byte  _Cierre_el_turno_o[];
		  extern const byte  _perdera_los_datos[];
		  extern const byte  _Cerrarlo_ahora[];
    #endif
    
    #ifdef VISOR_REPORTES
		  extern const byte _Todavia_no_finalizo[];
		  extern const byte  _ningun_Turno[];
    #endif
		  
		  extern const byte  _Error_al_comprobar[];
		  extern const byte  _datos_de[];
		  extern const byte  _almohadilla[];
		  extern const byte  _Verifiquela[];
		  
		  extern const byte  _Datos_de[];
		  extern const byte  _Almohadilla[];
		  extern const byte  _Registrados[];
		  
		  extern const byte  _El_Microfono[];
		  extern const byte  _quedo_PTTeado[];
		  
		  extern const byte  _La_seleccion_manual[];
		  extern const byte  _de_tarifa_no[];
		  extern const byte  _esta_habilitada[];

    #ifdef VISOR_IMPRESORA
		  extern const byte far _Imprimir_Duplicado[];
    #endif
		  
		  extern const byte  _El_viaje_ha_sido[];
		  extern const byte  _rechazado[];
		  extern const byte  _redespachado[];
		  extern const byte  _anulado[];

    #ifdef VISOR_PRINT_TICKET
		  extern const byte  _Todavia_no_se_han[];
		  extern const byte  _realizado_viajes[];
    #endif

    #ifdef VISOR_TARJETA
		  extern const byte far _No_se_ha_completado[];
      extern const byte  _la_transaccion[];
      extern const byte  _Por_favor_intente[];
      extern const byte  _nuevamente[];
    #endif

    #ifdef VISOR_PRINT_TICKET_FULL
      extern const byte  _No_se_han_detectado[];
      extern const byte  _desconexiones[];
    #endif
      
      extern const byte  _No_puede_realizar[];
      extern const byte  _cambios_de_reloj[];
      extern const byte  _con_el_vehiculo[];
      extern const byte  _en_movimiento[];
      
      extern const byte  _Desea_salir_del[];
      extern const byte  _sistema[];
      
      extern const byte  _Valor_programado[];
      extern const byte  _con_exito[];
      
      extern const byte  _ERROR_AL_PROGRAMAR[];
      extern const byte  _LA_CONSTANTE[];
      
      extern const byte  _YA_FUE_PROGRAMADA[];
      
      //extern const byte  _CONST[];
      extern const byte  _Se_reiniciara[];
      extern const byte  _el_equipo[];
      
      extern const byte  _La_CENTRAL_ha[];
      extern const byte  _sido_notificada[];
      
      extern const byte  _Reportes[];
      extern const byte  _no_Habilitados[];
      
      extern const byte  _No_ha_salido[];
      extern const byte  _favorecido_en_el[];
      extern const byte  _remate_de_viaje[];

    #ifdef VISOR_REPORTES
      extern const byte  _Para_evitar_errores[];
      extern  const byte _en_la_memoria[];
      extern const byte  _se_han_reseteado[];
      extern const byte  _los_reportes[];
    #endif
    
    
    #ifdef VISOR_TARJETA_OFFLINE
      extern const byte  _Desea_realizar[];
      extern const byte  _una_transaccion[];
      extern const byte  _offline[];
    #endif
    
    #ifdef VISOR_PRINT_TICKET
      extern const byte  _Que_desea_hacer[];
      extern const byte  _IMPRIMIRLO[];
      extern const byte  _VERLO[];
    #endif
      
		  
    /* VARIOS */
    /**********/
      extern const byte  _MOV_[];
      extern const byte  _REV[];
      extern const byte  _ERROR[];
      extern const byte  _FECHA[];
      extern const byte  _HORA[];
      extern const byte  _ASTERISK[];
      extern const byte  _SPACE[];
      extern const byte  _BARRA[];
      extern const byte  _2PUNTOS[];
      extern const byte  _20[];
      extern const byte  _FICHAS[];
      extern const byte  _sgnPESOS[];
      extern const byte  _BARRA_VERTICAL[];
      extern const byte  _EQUIS[];
      extern const byte  _switchSheldon[];
      extern const byte  _switchProggy[];
      extern const byte  _switchGohu[];
      extern const byte  _switchDina[];
      extern const byte  _EMPTY[];
      extern const byte  _y[];
      extern const byte  _ENDATA[];
      extern const byte  _closePARENT[];
      extern const byte  _km2[];
    
    
    /* PANTALLA LOGO */
    /*****************/
      extern const byte _D_SUSP[];
    
    
    /* PANTALLA LOGUEO */
    /*******************/
      extern const byte  _CHOFER[];
      extern const byte  _CLAVE[];
      extern const byte  _logueo_invalido[];
      extern const byte  _chofer_invalido[];
      extern const byte  _pasword_invalido[];
      extern const byte  _chofer_inhabilitado[];
      extern const byte  _movil_inhabilitado[];
      extern const byte  _vencimientos_pendientes[];
      extern const byte  _liquidaciones_pendientes[];
      extern const byte  _movil_inexistente[];
      extern const byte  _chofer_logueado[];
    
    
    /* PANTALLA PRINCIPAL */
    /**********************/
      extern const byte  _ZONA[];
      extern const byte  _BASE[];
      extern const byte  _Mv[];
      extern const byte  _Pos[];
      extern const byte  _FUT[];
      extern const byte  _VIRT[];
      extern const byte  _ASIG[];
      extern const byte  _4GUIONES[];
      extern const byte  _2GUIONES[];
      extern const byte  _sgnMAS[];
      extern const byte  _sgnMENOS[];
    
    
    /* PANTALLA MENSAJE */
    /********************/
      extern const byte  _Buzon_Vacio[];
      extern const byte  _SI[];
      extern const byte  _NO[];
      
    
    /* PANTALLA ACEPTA VIAJE */
    /*************************/
      extern const byte  _ACEPTA_VIAJE[];
      extern const byte  _con_Tarjeta[];
      extern const byte  _con_Cta_Cte[];
      extern const byte  _con_Cambio[];
      extern const byte  _con_Animal[];
      extern const byte  _a_mas_de_5_min[];
      extern const byte  _a_mas_de_3_km[];
      extern const byte  _larga_distancia[];
      extern const byte  _con_fichas[];
      extern const byte  _VIAJE_ACEPTADO[];
      extern const byte  _VIAJE_RECHAZADO[];
      extern const byte  _con_hasta_[];
      extern const byte  _en_[];
    
    
    /* PANTALLA DIRECCION */
    /**********************/
      extern const byte  _ENTRE_[];
      extern const byte  _y_[];
      extern const byte  _PACTADO_S_[];
      extern const byte  _CC_[];
      extern const byte  _TARJETA_[];
      extern const byte  _DEMORA_[];
      extern const byte  _min_[];
      extern const byte  _z_[];
      extern const byte  _b_[];
      extern const byte  _N_V_[];


    /* PANTALLA MENU RELOJ */
    /***********************/
      extern const byte  _1_[];
      extern const byte  _2_[];
      extern const byte  _3_[];
      extern const byte  _4_[];
      extern const byte  _5_[];
      extern const byte  _6_[];
      extern const byte  _7_[];
      extern const byte  _T1[];
      extern const byte  _T2[];
      extern const byte  _T3[];
      extern const byte  _T4[];
      extern const byte  _Fuera_Serv[];
      extern const byte  _Nuevo_Turno[];
      extern const byte  _Menu_Tickets[];
      
  
    /* PANTALLA MENU POWER */
    /***********************/ 
      extern const byte  _REINICIAR[];
      extern const byte  _APAGAR[];
                
    
    /* PANTALLA MENU */
    /*****************/   
      extern const byte  _ENVIAR_MSJ[];
      extern const byte  _CONTRASTE[];
      extern const byte  _TARJETA_MANUAL[];
      extern const byte  _CUPON[];
      extern const byte  _RECAUDADO[];
      
      
    /* PANTALLA MENSAJES PREGRABADOS */
    /*********************************/
      extern const byte  _OK[];
      
    
    /* TABLA DE MESES */
    /******************/
      extern const byte*  MESES[];
    
    
    /* PANTALLA TARJETA */
    /********************/
    #ifdef VISOR_TARJETA
      extern const byte  _Nro_de_Tarjeta[];
      extern const byte  _Fecha_Expiracion[];
      extern const byte  _Codigo_Seguridad[];
      extern const byte  _Codigo_Autorizacion[];
      extern const byte  _SIN_VENCIMIENTO[];
      extern const byte  _SALIR[];
      extern const byte  _CAMBIAR[];
      
      extern const byte  _Menu_Tarjeta[];
      extern const byte  _Pago[];
      extern const byte  _Saldo[];
      extern const byte  _Recarga[];
      
      extern const byte  _El_Saldo_Es[];
      extern const byte  _Punto_Decimal[];
      
      extern const byte  _MAYOR_999[];
    #endif
    
    #if defined(VISOR_TARJETA) || defined(VISOR_IMPORTE_MANUAL)
      extern const byte  _Ingrese_el_Monto[];
      extern const byte  _El_Monto_Es[];
    #endif
    
    
    /* TICKETS */
    /***********/
      extern const byte  _PRUEBA_IMPRESORA[];
      extern const byte  _NA[];
      extern const byte  _X[];
      
    #ifdef VISOR_PROG_TICKET
      extern const byte  _Localidad_Desconocida[];
      extern const byte  _Tipo_de_Servicio_Desconocido[];
      extern const byte  _Titular_Desconocido[];
      extern const byte  _CUIT_Desconocido[];
      extern const byte  _Coche_Desconocido[];
      extern const byte  _Patente_Desconocida[];
      extern const byte  _Licencia_Desconocida[];
    #endif
    
      extern const byte  _Chofer[];
      extern const byte  _GUION_[];
      extern const byte  _INICIO[];
      extern const byte  _FIN[];
      extern const byte  _Distancia[];
      extern const byte  _km[];
      extern const byte  _Vel_Max[];
      extern const byte  _kmh[];
      extern const byte  _Tarifa[];
      extern const byte  _D[];
      extern const byte  _N[];
    #ifdef VISOR_PRINT_TICKET
      extern const byte  _Ticket_de_Viaje[];
      extern const byte  _CUIT[];
      extern const byte  _RUC[];
      extern const byte  _Patente[];
      extern const byte  _Licencia[];
      extern const byte  _IVA[];
      extern const byte  _Bajada_Bandera[];
      extern const byte  _F_Dist[];
      extern const byte  _F_Tiempo[];
      extern const byte  _F_Total[];
      extern const byte  _Nro_Movil[];
      extern const byte  _Nro_Recibo[];
      extern const byte  _Espera[];
      extern const byte  _A_PAGAR[];
      extern const byte  _GRACIAS[];
      extern const byte  _Eq[];
    #endif
    
    #ifdef VISOR_REPORTES
      extern const byte  _Recaudacion_Parcial[];
      extern const byte  _Impreso_el[];
      extern const byte  _RECAUDACION[];
      extern const byte  _Inicio_Turno[];
    #endif
    
    #ifdef VISOR_TICKET_TURNO
      extern const byte  _REPORTE_DE_TURNO[];
      extern const byte  _REPORTE_TURNO[];
      extern const byte  _Turno_Nro[];
      extern const byte  _Fin_Turno[];
      extern const byte  _LIBRE[];
      extern const byte  _OCUPADO[];
      extern const byte  _FUERA_DE_SERVICIO[];
      extern const byte  _Tiempo_Marcha[];
      extern const byte  _Tiempo_Parado[];
      extern const byte  _TARIFA[];
      extern const byte  _TARIFA_ESPECIAL[];
      extern const byte  _Diurna[];
      extern const byte  _Nocturna[];
      extern const byte  _Viajes[];
      extern const byte  _Viajes_por_asiento[];
      extern const byte  _Por_Asiento[];
      extern const byte  _Errores_asiento[];
      extern const byte  _Err_Asiento[];
      extern const byte  _Recaudado[];
      extern const byte  _Fichas[];
      extern const byte  _Tiempo[];
      extern const byte  _Totales[];
      extern const byte  _RINDEX[];
      extern const byte  _KM_Totales[];
      extern const byte  _KM_Total[];
      extern const byte  _Pesos_x_km[];
      extern const byte  _Percent_km_ocupado[];
      extern const byte  _km_ocup[];
      extern const byte  _Desconex_Alim[];
      extern const byte  _DESCONEX_ALIM[];
      extern const byte  _Cantidad[];
      extern const byte  _Desconex_Acc[];
      extern const byte  _DESCONEX_ACC[];
      extern const byte  _Pesos_Perdidos[];
      extern const byte  _Mov_sPulsos[];
    #endif
    
    #ifdef VISOR_PRINT_TICKET_TARJETA
      extern const byte  _COMPROBANTE_TARJETA[];
      extern const byte  _Nro_Transaccion[];
      extern const byte  _Nro_Tarjeta[];
      extern const byte  _PAGO[];
      extern const byte  _RECARGA[];
      extern const byte  _SALDO[];
      
      extern const byte  _REPORTE_TARJETA[];
    #endif
    
    #ifdef VISOR_TICKET_30_DIAS
      extern const byte  _REPORTE_DESCONEXIONES[];
      extern const byte  _REPORTE_30_DIAS[];
      extern const byte  _REPORTE_DEL_DIA[];
      extern const byte  _DIA[];
      extern const byte  _Turnos[];
      extern const byte  _Dist_Ocup[];
      extern const byte  _km_Libre[];
      extern const byte  _km_Ocupado[];
      extern const byte  _km_FServ[];
    #endif
    
    #ifdef VISOR_TICKET_CONTROL_PARAMETROS
      extern const byte  _CONTROL_DE_PARAMETROS[];
      extern const byte  _Parametros_de_Movil[];
      extern const byte  _Parametros_de_Reloj[];
      extern const byte  _Tipo_Servicio[];
      extern const byte  _TAXI[];
      extern const byte  _REMIS[];
      extern const byte  _Constante[];
      extern const byte  _RTC[];
      extern const byte  _Tecla_Espera[];
      extern const byte  _Sin_Espera[];
      extern const byte  _Espera_Acum[];
      extern const byte  _Tarifa_Manual[];
      extern const byte  _Choferes[];
      extern const byte  _Cant_Tarifas[];
      extern const byte  _t_Gracia[];
      extern const byte  _Dist_Inicial[];
      extern const byte  _Dist_Ficha[];
      extern const byte  _Hora_Inicio[];
      extern const byte  _t_Ficha[];
      extern const byte  _Asiento[];
      extern const byte  _Sin_Sensor[];
      extern const byte  _Normal[];
      extern const byte  _Inteligente[];
      extern const byte  _Car_Bandera[];
      extern const byte  _seg[];
      extern const byte  _DLMMJVS_CAL[];
    #endif
    
    #ifdef VISOR_TICKET_DETALLADO
      extern const byte  _REPORTE_DETALLADO[];
      extern const byte  _L[];
      extern const byte  _O[];
      extern const byte  _C[];
      extern const byte  _F[];
      extern const byte  _T[];
      extern const byte  _V[];
      extern const byte  _Inicia[];
      extern const byte  _FD[];
      extern const byte  _FT[];
      extern const byte  _PROG_TALLER[];
      extern const byte  _PROG_AIRE[];
      extern const byte  _AYUDA[];
      extern const byte  _L_Libre[];
      extern const byte  _O_Ocupado[];
      extern const byte  _C_Cobrando[];
      extern const byte  _F_Fuera_Servicio[];
      extern const byte  _T_Turno[];
      extern const byte  _D_Desconex_Alim[];
      extern const byte  _X_Desconex_Acc[];
      extern const byte  _V_Varios[];
      extern const byte  _HH[];
      extern const byte  __MM[];
      extern const byte  _km_[];
      extern const byte  _vel[];
      extern const byte  _tSTOP[];
      extern const byte  _tarif[];
      extern const byte  _PESOS_fichas[];
      extern const byte  _tDESC[];
      extern const byte  _error_sens_Asiento[];
      extern const byte  _ayuda_sens_Asiento[];
    #endif


    /* PANTALLA MENU CHOFER */
    /************************/
    #ifdef VISOR_REPORTES
      extern const byte  _CHOFER_1[];
      extern const byte  _CHOFER_2[];
      extern const byte  _CHOFER_3[];
      extern const byte  _CHOFER_4[];
    #endif
    
    
    /* PANTALLA MENU TICKET */
    /************************/
    #ifdef VISOR_PROG_TICKET
      extern const byte  _Recaud_Parcial[];
      extern const byte  _Ultimo_Viaje[];
      extern const byte  _Reporte_Turno[];
      extern const byte  _Reporte_Tarjeta[];
      extern const byte  _Ultimo_Comprobante[];
      extern const byte  _Reporte_Descon[];
      extern const byte  _Reporte_30_Dias[];
      extern const byte  _Control_Param[];
      extern const byte  _Reporte_por_Dia[];
      extern const byte  _Reporte_Detallado[];
    #endif
    
    
    /* PANTALLA MENU REPORTE TURNO */
    /*******************************/
    #ifdef VISOR_PROG_TICKET
      extern const byte  _ID[];
      extern const byte  _CHF[];
    #endif
    
    
    /* PANTALLA DISTANCIA y VELOCIDAD */
    /**********************************/
      extern const byte  _Dist[];
      extern const byte  _Vel[];
      extern const byte  _kmh2[];
      extern const byte  _mts[];

    
    /* MODO TEST */
    /*************/
      extern const byte  _GPS[];
      extern const byte  _ACCESORIO[];
      extern const byte  _RADIO[];
      extern const byte  _IMPRESORA[];
      extern const byte  _TARJETA[];
      extern const byte  _TECLADO[];
      
      extern const byte  _SELECCIONE_MODULO[];
      
      extern const byte  _COMUNICACION[];
      extern const byte  _PREG[];
      extern const byte  _POSICION[];
      extern const byte  _Valida[];
      extern const byte  _No_Valida[];
      extern const byte  _SINCRONISMO[];
      extern const byte  _LATITUD[];
      extern const byte  _LONGITUD[];
    
    #ifdef VISOR_PROG_TICKET
      extern const byte  _IMPRIMIENDO_TICKET[];
    #endif
      extern const byte  _EQUIPO_SIN_IMPRESORA[];
      
      extern const byte  _0[];
      extern const byte  _1[];
      extern const byte  _2[];
      extern const byte  _3[];
      extern const byte  _4[];
      extern const byte  _5[];
      extern const byte  _6[];
      extern const byte  _7[];
      extern const byte  _8[];
      extern const byte  _9[];
      
      extern const byte  _BANDERITA[];
      extern const byte  _ASIENTO[];
      extern const byte  _EMERGENCIA[];
      extern const byte  _PULSOS[];
      extern const byte  _DIV[];
      extern const byte  _VEL[];
      extern const byte  _SERIE[];
      extern const byte  _verificar_BANDERA_y_PARLANTE[];
      extern const byte  _0_micOculto[];
      
      extern const byte  _SALIDA_PTT[];
      extern const byte  _PORTADORA[];
      extern const byte  _PTT_EXTERNO[];
      extern const byte  _PTT_VISOR[];
      extern const byte  _CANAL_DATOS[];
      extern const byte  _T1_tonos_de_prueba[];
      extern const byte  _verificar_CAMBIO_DE_CANAL[];
    
    #ifdef VISOR_TARJETA
      extern const byte _PASE_LA_TARJETA[];
    #endif
      extern const byte  _EQUIPO_SIN_TARJETA[];

    
    /* MOVILES POR ZONA */
    /********************/
      extern const byte  _MOVILES_EN_ZONA[];
      extern const byte  _MOVILES_EN_BASE[];
      extern const byte  _Informacion[];
      extern const byte  _de_Zonas[];
      extern const byte  _de_Bases[];
      extern const byte  _no_disponible[];
      
    
    /* CALCULO DE CONSTANTE */
    /************************/
      extern const byte  _CONSTANTE[];
      extern const byte  _RESET[];
      
    
    /* MENU REPORTE DETALLADO */
    /**************************/
    #ifdef VISOR_TICKET_DETALLADO
      extern const byte  _SELECCIONE_DIA[];
      extern const byte  _SELECCIONE_HORA[];
      extern const byte  _00[];
    #endif
      
      
    /* PANTALLA TARJETA MANUAL */
    /***************************/
      extern const byte  _Ingrese_el_Numero[];
      extern const byte  _CONTINUAR[];
    #ifdef VISOR_TARJETA
      extern const byte  _de_Tarjeta[];
      
      extern const byte  _Ingrese_el_Codigo[];
      extern const byte  _de_Seguridad[];
      extern const byte  _de_Autorizacion[];
    #endif


    /* PANTALLA PARAMETROS */
    /***********************/
    #ifdef VISOR_PANTALLA_PARAMETROS
      extern const byte  _DESCONEX_RLJ[];
      extern const byte  _ZONAS[];
      extern const byte  _BASES[];
      extern const byte  _RLJ_VIEJO[];
      extern const byte  _RLJ_NUEVO[];
      extern const byte  _REPORTES[];
      extern const byte  _GPRS[];
      extern const byte  _DECIMALES[];
      extern const byte  _CODIGO[];
      extern const byte  __OCULTOS[];
      extern const byte  _HOMOLOGADO[];
      extern const byte  _TICKET[];
      extern const byte  _GEOFENCE[];
      extern const byte  _MENSAJES[];
      extern const byte  _NO_Tx_CH_[];
    #endif
      
    
    /* PANTALLA RECAUDADO */
    /**********************/
    #ifdef VISOR_REPORTES
      extern const byte _VIAJES[];
    #endif
     
     
    /* PANTALLA CUPON */
    /******************/
      extern const byte  _de_Cupon[];
      extern const byte  _de_Cuenta[];
      extern const byte  _ENVIAR[];
    
    
    /* TO SPEECH */
    /*************/
    #ifdef VISOR_PRINT_TICKET_FULL
      extern const byte  _mil[];
      extern const byte  _dos_mil[];
      extern const byte  _tres_mil[];
      extern const byte  _cuatro_mil[];
      extern const byte  _cinco_mil[];
      extern const byte  _seis_mil[];
      extern const byte  _siete_mil[];
      extern const byte  _ocho_mil[];
      extern const byte  _nueve_mil[];
    
      extern const byte _cien[];
      
      extern const byte  _ciento[];
      extern const byte  _doscientos[];
      extern const byte  _trescientos[];
      extern const byte  _cuatrocientos[];
      extern const byte  _quinientos[];
      extern const byte  _seisientos[];
      extern const byte  _setecientos[];
      extern const byte  _ochocientos[];
      extern const byte  _novecientos[];
    
      extern const byte  _dieci[];
      extern const byte  _veinti[];
      extern const byte  _treinta[];
      extern const byte  _cuarenta[];
      extern const byte  _cincuenta[];
      extern const byte  _sesenta[];
      extern const byte  _setenta[];
      extern const byte  _ochenta[];
      extern const byte  _noventa[];

      extern const byte  _cero[];
      extern const byte  _un[];
      extern const byte  _dos[];
      extern const byte  _tres[];
      extern const byte  _cuatro[];
      extern const byte  _cinco[];
      extern const byte  _seis[];
      extern const byte  _siete[];
      extern const byte  _ocho[];
      extern const byte  _nueve[];
    
      extern const byte  _veinte[];
    
      extern const byte  _diez[];
      extern const byte  _once[];
      extern const byte  _doce[];
      extern const byte  _trece[];
      extern const byte  _catorce[];
      extern const byte  _quince[];
      extern const byte  _dieciseis[];
      extern const byte  _diecisiete[];
      extern const byte  _dieciocho[];
      extern const byte  _diecinueve[];
    #endif
    
    
    /* PREMIUM */
    /***********/
    #ifdef _PREMIUM_
      extern const byte  _legal1[];
      extern const byte  _legal2[];
      extern const byte  _legal3[];
      extern const byte  _legal4[];
      extern const byte  _legal5[];
      extern const byte  _centavos[];
      extern const byte  _FIRMA[];
      extern const byte  _ACLARACION[];
      extern const byte  _ACLARACION_dots[];
      extern const byte  _DNI[];
      extern const byte  _Nro_Viaje[];
      extern const byte  _Nro_Autoriz[];
      extern const byte  _PremiumCard[];
      extern const byte  _DESDE[];
      extern const byte  _HASTA[];
      extern const byte  _Coodenadas_GPS[];
      extern const byte  _DESDEdots[];
    #endif
        


  
  
  /* RUTINAS */
  /***********/
    extern void string_FAR_copy (byte* dest_ptr, byte*  orig_ptr);
    extern void string_FAR_concat (byte* dest_ptr, byte*  orig_ptr);
    extern byte string_FAR_length (byte* orig_ptr);

#endif

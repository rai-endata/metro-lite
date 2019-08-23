/* File: <Textos.c> */


/* INCLUDES */
/************/
  #include "- metroBLUE Config -.h"
  #include "Manejo de Buffers.h"
  
 // #include "Fonts.h"


/*********************************************************************************************/
/* CONSTANTES */
/**************/
#pragma CONST_SEG   PAGE_TEXT
 
  /* CONFIGURACION INICIAL */
  /*************************/
    const byte  _INICIANDO_EQUIPO_POR_PRIMERA_VEZ[] = {"INICIANDO EQUIPO POR PRIMERA VEZ"};
    const byte  _POR_FAVOR_ESPERE[] = {"POR FAVOR ESPERE"};
 
  /* AVISOS */
  /**********/
    const byte  _Espere_conexion[] = {"Espere conexion"};
    const byte  _con_la_Central[] = {"con la Central"};
    
    const byte  _Espere_sincronismo[] = {"Espere sincronismo"};
    
    const byte  _Direccion[] = {"Direccion"};
    const byte  _no_Disponible[] = {"no Disponible"};
    
    const byte  _Para_DESLOGUEARSE[] = {"Para DESLOGUEARSE"};
    const byte  _pase_a_Libre_o_a[] = {"pase a Libre o a"};
    const byte  _Fuera_de_Servicio[] = {"Fuera de Servicio"};
    
    
    const byte  _Logueandose[] = {"Logueandose."};
    const byte  _Por_favor_espere[] = {"Por favor espere"};
    
    const byte  _Reloj_Desconectado[] = {"Reloj Desconectado"};
    
    const byte  _Sincronizando[] = {"Sincronizando"};
    
    const byte  _Despacho[] = {"Despacho"};
    const byte  _Suspendido[] = {"Suspendido"};

    const byte  _Hora_RTC_INVALIDA[] = {"Hora RTC INVALIDA"};
    const byte  _comuniquese_con[] = {"comuniquese con"};
    const byte  _Servicio_Tecnico[] = {"Servicio Tecnico"};
    
    const byte  _RTC_NO_RESPONDE[] = {"RTC NO RESPONDE"};
    const byte  _se_utilizara_la[] = {"se utilizaara la"};
    const byte  _hora_del_GPS[] = {"hora del GPS"};

		const byte  _Volumen_de_Buzzer[] = {"Volumen de Buzzer"};
		const byte  _seteado_en[] = {"seteado en "};
		const byte  _ALTO[] = {"ALTO"};
		const byte  _BAJO[] = {"BAJO"};
		
		const byte  _Remate[] = {"Remate:"};
		const byte  _Viaje_en[] = {"Viaje en:"};

  #ifdef VISOR_TARJETA
		const byte  _Por_favor_pase_la[] = {"Por favor pase la"};
    const byte  _tarjeta_nuevamente[] = {"tarjeta nuevamente"};
  #endif
  
  #ifdef VISOR_TARJETA
    const byte  _Consultando_tarjeta[] = {"Consultando tarjeta"};
    const byte  _Validando_tarjeta[] = {"Validando tarjeta"};
  #endif

  #ifdef VISOR_REPORTES
    const byte  _Esta_a_punto_de[] = {"Esta a punto de"};
    const byte  _iniciar_un_nuevo[] = {"iniciar un nuevo"};
    const byte  _turno_Continuar[] = {"turno. Continuar?"};
  #endif
    
  #ifdef VISOR_REPORTES
    const byte  _Ha_iniciado_un[] = {"Ha iniciado un"};
    const byte  _nuevo_turno_con[] = {"nuevo turno con"};
    const byte  _el_chofer[] = {"el CHOFER "};
  #endif
	
	#ifdef VISOR_TARJETA
		const byte  _La_tarjeta_ha[] = {"La tarjeta ha"};
		const byte  _sido_RECHAZADA[] = {"sido RECHAZADA."};
		const byte  _Motivo[] = {"Motivo: "};
		const byte  _SIN_SALDO[] = {"SIN SALDO"};
		const byte  _INVALIDA[] = {"INVALIDA"};
		const byte  _ROBADA[] = {"ROBADA"};
		const byte  _EXPIRADA[] = {"EXPIRADA"};
		const byte  _EXCEDIDA[] = {"EXCEDIDA"};
		const byte  _DESCONOCIDO[] = {"DESCONOCIDO"};
  #endif
  
  #ifdef VISOR_TARJETA
		const byte  _No_se_puede[] = {"No se puede"};
		const byte  _recargar_desde[] = {"RECARGAR desde"};
  #endif
		
  #ifdef VISOR_TARJETA
		const byte  _Ha_alcanzado_la[] = {"Ha alcanzado la"};
		const byte  _cantidad_maxima[] = {"cantidad maxima"};
		const byte  _de_transacciones[] = {"de transacciones"};
		const byte  _permitidas[] = {"permitidas"};
  #endif
  
  #ifdef VISOR_TARJETA		
		const byte  _Numero_de[] = {"Numero de"};
		const byte  _tarjeta_incorrecto[] = {"Tarjeta incorrecto"};
	#endif
		
	#ifdef VISOR_TARJETA
		const byte  _Codigo_de[] = {"Codigo de"};
		const byte  _Seguridad[] = {"Seguridad"};
		const byte  _incorrecto[] = {"incorrecto"};
  #endif
  
  #ifdef VISOR_TARJETA		
		const byte  _Ha_alcanzado_el[] = {"Ha alcanzado el"};
		const byte  _limite_de_consumo[] = {"limite de consumo"};
		const byte  _permitido[] = {"permitido"};
  #endif

  #ifdef VISOR_TARJETA
		const byte  _El_viaje_ya_ha[] = {" El viaje ya ha"};
		const byte  _sido_pagado_con[] = {"sido pagado con"};
		const byte  _tarjeta[] = {"tarjeta"};
  #endif
  
  #ifdef VISOR_REPORTES
		const byte  _Memoria_de_Reportes[] = {"Memoria de Reportes"};
		const byte  _proxima_a_llenarse[] = {"proxima a llenarse"};
		const byte  _Cierre_el_turno_o[] = {"Cierre el turno o"};
		const byte  _perdera_los_datos[] = {"perdera los datos."};
		const byte  _Cerrarlo_ahora[] = {"Cerrarlo ahora?"};
  #endif		
		
	#ifdef VISOR_REPORTES
		const byte  _Todavia_no_finalizo[] = {"Todavia no finalizo"};
		const byte  _ningun_Turno[] = {"ningun Turno"};
  #endif
		
		const byte  _Error_al_comprobar[] = {"Error al comprobar"};
		const byte  _datos_de[] = {"datos de"};
		const byte  _almohadilla[] = {"almohadilla."};
		const byte  _Verifiquela[] = {"Verifiquela"};
		
		const byte  _Datos_de[] = {"Datos de"};
		const byte  _Almohadilla[] = {"almohadilla"};
		const byte  _Registrados[] = {"Registrados."};
		
		const byte  _El_Microfono[] = {"El Microfono"};
		const byte  _quedo_PTTeado[] = {"quedo PTTeado"};
		
		const byte  _La_seleccion_manual[] = {"La seleccion manual"};
		const byte  _de_tarifa_no[] = {"de tarifa no"};
		const byte  _esta_habilitada[] = {"esta habilitada"};

  #ifdef VISOR_IMPRESORA
		const byte  _Imprimir_Duplicado[] = {"Imprimir Duplicado?"};
	#endif
		
		const byte  _El_viaje_ha_sido[] = {"El viaje ha sido"};
		const byte  _rechazado[] = {"rechazado"};
		const byte  _redespachado[] = {"redespachado"};
		const byte  _anulado[] = {"anulado"};

  #ifdef VISOR_PRINT_TICKET
		const byte  _Todavia_no_se_han[] = {"Todavia no se han"};
		const byte  _realizado_viajes[] = {"realizado viajes"};
  #endif
	
	#ifdef VISOR_TARJETA
	const byte  _No_se_ha_completado[] = {"No se ha completado"};
    const byte  _la_transaccion[] = {"la transaccion"};
    const byte  _Por_favor_intente[] = {"Por favor intente"};
    const byte  _nuevamente[] = {"nuevamente"};
  #endif
  
  #ifdef VISOR_PRINT_TICKET_FULL  
    const byte  _No_se_han_detectado[] = {"No se han detectado"};
    const byte  _desconexiones[] = {"desconexiones"};
  #endif
    
    const byte  _No_puede_realizar[] = {"No puede realizar"};
    const byte  _cambios_de_reloj[] = {"cambios de reloj"};
    const byte  _con_el_vehiculo[] = {"con el vehiculo"};
    const byte  _en_movimiento[] = {"en movimiento"};
    
    const byte  _Desea_salir_del[] = {"Desea salir del"};
    const byte  _sistema[] = {"sistema?"};
    
    const byte  _Valor_programado[] = {"Valor programado"};
    const byte  _con_exito[] = {"con exito."};
    
    const byte  _ERROR_AL_PROGRAMAR[] = {"ERROR AL PROGRAMAR"};
    const byte  _LA_CONSTANTE[] = {"LA CONSTANTE"};
    
    const byte  _YA_FUE_PROGRAMADA[] = {"YA FUE PROGRAMADA"};
    
    const byte  _CONST[] = {"CONST:"};
    const byte  _Se_reiniciara[] = {"Se reiniciara"};
    const byte  _el_equipo[] = {"el equipo"};
    
    const byte  _La_CENTRAL_ha[] = {"La CENTRAL ha"};
    const byte  _sido_notificada[] = {"sido notificada"};
    
    const byte  _Reportes[] = {"Reportes"};
    const byte  _no_Habilitados[] = {"no Habilitados"};
    
    const byte  _No_ha_salido[] = {"No ha salido"};
    const byte  _favorecido_en_el[] = {"favorecido en el"};
    const byte  _remate_de_viaje[] = {"remate de viaje"};

  #ifdef VISOR_REPORTES
    const byte  _Para_evitar_errores[] = {"Para evitar errores"};
    const byte  _en_la_memoria[] = {"en la memoria"};
    const byte  _se_han_reseteado[] = {"se han reseteado"};
    const byte  _los_reportes[] = {"los reportes"};
  #endif
  
  #ifdef VISOR_TARJETA_OFFLINE
    const byte  _Desea_realizar[] = {"Desea realizar"};
    const byte  _una_transaccion[] = {"una transaccion"};
    const byte  _offline[] = {"offline?"};
  #endif
  
  #ifdef VISOR_PRINT_TICKET
    const byte  _Que_desea_hacer[] = {"Que desea hacer?"};
    const byte  _IMPRIMIRLO[] = {"IMPRIMIRLO"};
    const byte  _VERLO[] = {"VERLO"};
  #endif
		
		
		 
  /* VARIOS */
  /**********/
    /*const byte far _MOV_[] = {"!#$%&'()*+,-./0123456789:;"};
    const byte  _ERROR[] = {"<=>?@ABCDEFGHIJKLMNOPQRSTU"};
    const byte  _REV[] = {"VWXYZ[/]^_`abcdefghijklmnop"};
    const byte  _FECHA[] = {"qrstuvwxyz{}~ñÑ"};
    */
    const byte  _MOV_[] = {"Mov:"};
    const byte  _REV[] = {"rev"};
    const byte  _ERROR[] = {"ERROR"};
    const byte  _FECHA[] = {"FECHA"};
    const byte  _HORA[] = {"HORA"};
    const byte  _ASTERISK[] = {"*"};
    const byte  _SPACE[] = {" "};
    const byte  _BARRA[] = {"/"};
    const byte  _2PUNTOS[] = {":"};
    const byte  _20[] = {"20"};
    const byte  _FICHAS[] = {"FICHAS"};
    const byte  _sgnPESOS[] = {"$"};
    const byte  _BARRA_VERTICAL[] = {"|"};
    const byte  _EQUIS[] = {"X"};
//    const byte  _switchSheldon[] = {switchFontSheldon, 0x00};
//    const byte  _switchProggy[] = {switchFontProggy, 0x00};
//    const byte  _switchGohu[] = {switchFontGohu, 0x00};
//    const byte  _switchDina[] = {switchFontDina, 0x00};
    const byte  _EMPTY[] = {""};
    const byte  _y[] = {"y"};
    const byte  _ENDATA[] = {"ENDATA"};
    const byte  _closePARENT[] = {")"};
    const byte  _km2[] = {"km"};
  
  
  /* PANTALLA LOGO */
  /*****************/
    const byte _D_SUSP[] = {"D.SUSP"};
  
  
  /* PANTALLA LOGUEO */
  /*******************/
    const byte  _CHOFER[] = {"CHOFER:"};
    const byte  _CLAVE[] = {"CLAVE:"};
    const byte  _logueo_invalido[] = {"LOGUEO INVALIDO"};
    const byte  _chofer_invalido[] = {"CHOFER INVALIDO"};
    const byte  _pasword_invalido[] = {"PASSWORD INVALIDO"};
    const byte  _chofer_inhabilitado[] = {"CHOFER INHABILITADO"};
    const byte  _movil_inhabilitado[] = {"MOVIL INHABILITADO"};
    const byte  _vencimientos_pendientes[] = {"VENCIMIENTOS PEND."};
    const byte  _liquidaciones_pendientes[] = {"LIQUIDACIONES PEND."};
    const byte  _movil_inexistente[] = {"MOVIL INEXISTENTE"};
    const byte  _chofer_logueado[] = {"CHOFER LOGUEADO"};
    

  
  
  /* PANTALLA PRINCIPAL */
  /**********************/
    const byte  _ZONA[] = {"ZONA:"};
    const byte  _BASE[] = {"BASE:"};
    const byte  _Mv[] = {"Mv:"};
    const byte  _Pos[] = {"Ps:"};
    const byte  _FUT[] = {"FUT:"};
    const byte  _VIRT[] = {"VIRT"};
    const byte  _ASIG[] = {"ASIG"};
    const byte  _4GUIONES[] = {"----"};
    const byte  _2GUIONES[] = {"--"};
    const byte  _sgnMAS[] = {"+"};
    const byte  _sgnMENOS[] = {"-"};
  
  
  /* PANTALLA MENSAJE */
  /********************/
    const byte  _Buzon_Vacio[] = {"Buzon Vacio"};
    const byte  _SI[] = {"SI"};
    const byte  _NO[] = {"NO"};
  
  
  /* PANTALLA ACEPTA VIAJE */
  /*************************/
    const byte  _ACEPTA_VIAJE[] = {"ACEPTA VIAJE?"};
    const byte  _con_Tarjeta[] = {"(con Tarjeta)"};
    const byte  _con_Cta_Cte[] = {"(con Cta. Cte.)"};
    const byte  _con_Cambio[] = {"(con Cambio)"};
    const byte  _con_Animal[] = {"(con Animal)"};
    const byte  _a_mas_de_5_min[] = {"(a mas de 5 min)"};
    const byte  _a_mas_de_3_km[] = {"(a mas de 3 km)"};
    const byte  _larga_distancia[] = {"(larga distancia)"};
    const byte  _con_fichas[] = {"(con fichas)"};
    const byte  _VIAJE_ACEPTADO[] = {"VIAJE ACEPTADO"};
    const byte  _VIAJE_RECHAZADO[] = {"VIAJE RECHAZADO"};
    const byte  _con_hasta_[] = {"(hasta "};
    const byte  _en_[] = {" en "};
  
  
  /* PANTALLA DIRECCION */
  /**********************/
    const byte  _ENTRE_[] = {"Entre "};
    const byte  _y_[] = {" y "};
    const byte  _PACTADO_S_[] = {"PACTADO: $"};
    const byte  _CC_[] = {"CC: "};
    const byte  _TARJETA_[] = {"TARJETA: "};
    const byte  _DEMORA_[] = {"DEMORA: "};
    const byte  _min_[] = {" min "};
    const byte  _z_[] = {"z: "};
    const byte  _b_[] = {"b: "};
    const byte  _N_V_[] = {"N.V."};
  
  
  /* PANTALLA MENU RELOJ */
  /***********************/
    const byte  _1_[] = {"1- "};
    const byte  _2_[] = {"2- "};
    const byte  _3_[] = {"3- "};
    const byte  _4_[] = {"4- "};
    const byte  _5_[] = {"5- "};
    const byte  _6_[] = {"6- "};
    const byte  _7_[] = {"7- "};
    const byte  _T1[] = {"T1"};
    const byte  _T2[] = {"T2"};
    const byte  _T3[] = {"T3"};
    const byte  _T4[] = {"T4"};
    const byte  _Fuera_Serv[] = {"Fuera Serv."};
    const byte  _Nuevo_Turno[] = {"Nuevo Turno"};
    const byte  _Menu_Tickets[] = {"Menu Tickets"};

  
  /* PANTALLA MENU POWER */
  /***********************/
    const byte  _REINICIAR[] = {"REINICIAR"};
    const byte  _APAGAR[] = {"APAGAR"};


  /* PANTALLA MENU */
  /*****************/
    const byte  _TARJETA_MANUAL[] = {"TARJ. MANUAL"};
    const byte  _CUPON[] = {"CUPON"};
    const byte  _RECAUDADO[] = {"RECAUDADO"};
    const byte  _ENVIAR_MSJ[] = {"ENVIAR MSJ"};
    const byte  _CONTRASTE[] = {"CONTRASTE"};
        
  
  /* PANTALLA MENSAJE PREGRABADO */
  /*******************************/
    const byte _OK[] = {"OK"};
          
    
  /* TABLA DE MESES */
  /******************/
    const byte _ENE[] = {"ENE"};
    const byte _FEB[] = {"FEB"};
    const byte _MAR[] = {"MAR"};
    const byte _ABR[] = {"ABR"};
    const byte _MAY[] = {"MAY"};
    const byte _JUN[] = {"JUN"};
    const byte _JUL[] = {"JUL"};
    const byte _AGO[] = {"AGO"};
    const byte _SEP[] = {"SEP"};
    const byte _OCT[] = {"OCT"};
    const byte _NOV[] = {"NOV"};
    const byte _DIC[] = {"DIC"};
    
    const byte* MESES[12] = {
      _ENE,
      _FEB,
      _MAR,
      _ABR,
      _MAY,
      _JUN,
      _JUL,
      _AGO,
      _SEP,
      _OCT,
      _NOV,
      _DIC
    };


  /* PANTALLA TARJETA */
  /********************/
  #ifdef VISOR_TARJETA
    const byte  _Nro_de_Tarjeta[] = {"TARJETA:"};
    const byte  _Fecha_Expiracion[] = {"EXPIRACION:"};
    const byte  _Codigo_Seguridad[] = {"CODIGO:"};
    const byte  _Codigo_Autorizacion[] = {"AUTORIZACION:"};
    const byte  _SIN_VENCIMIENTO[] = {"SIN VENCIMIENTO"};
    const byte  _SALIR[] = {"SALIR"};
    const byte  _CAMBIAR[] = {"CAMBIAR"};
            
    const byte  _Menu_Tarjeta[] = {"MENU TARJETA"};
    const byte  _Pago[] = {"Pago"};
    const byte  _Saldo[] = {"Saldo"};
    const byte  _Recarga[] = {"Recarga"};
    
    const byte  _El_Saldo_Es[] = {"EL SALDO ES:"};
    const byte  _Punto_Decimal[] = {"Punto Decimal"};
    
    const byte  _MAYOR_999[] = {"> 999"};
  #endif
  
  #if defined(VISOR_TARJETA) || defined(VISOR_IMPORTE_MANUAL)
    const byte  _Ingrese_el_Monto[] = {"INGRESE EL MONTO:"};
    const byte  _El_Monto_Es[] = {"EL MONTO ES:"};
  #endif
    

  /* TICKETS */
  /***********/
    const byte  _PRUEBA_IMPRESORA[] = {"PRUEBA IMPRESORA"};
    const byte  _NA[] = {"N/A"};
    const byte  _X[] = {"X"};        // Desconexion Accesorio y uso la X para otras cosas
  
  #ifdef VISOR_PROG_TICKET
    const byte  _Localidad_Desconocida[] = {"Localidad"};
    const byte  _Tipo_de_Servicio_Desconocido[] = {"Tipo de Servicio"};
    const byte  _Titular_Desconocido[] = {"Titular Desconocido"};
    const byte  _CUIT_Desconocido[] = {"00-0000000-0"};
    const byte  _Coche_Desconocido[] = {"Coche Desconocido"};
    const byte  _Patente_Desconocida[] = {"xxx xxx"};
    const byte  _Licencia_Desconocida[] = {"xxxxxxx"};
  #endif
  
  const byte  _Chofer[] = {"Chofer:"};
  const byte  _GUION_[] = {"-"};
  const byte  _INICIO[] = {"INICIO:"};
  const byte  _FIN[] = {"FIN:"};
  const byte  _Distancia[] = {"Distancia:"};
  const byte  _km[] = {" km"};
  const byte  _Vel_Max[] = {"Vel.Max:"};
  const byte  _kmh[] = {" km/h"};
  const byte  _Tarifa[] = {"Tarifa"};
  const byte  _D[] = {"D"};
  const byte  _N[] = {"N"};
  #ifdef VISOR_PRINT_TICKET
    const byte  _Ticket_de_Viaje[] = {"TICKET DE VIAJE"};
    const byte  _CUIT[] = {"CUIT:"};
    const byte  _RUC[] = {"RUC:"};
    const byte  _Patente[] = {"Patente:"};
    const byte  _Licencia[] = {"Licencia:"};
    const byte  _IVA[] = {"IVA:"};
    const byte  _Bajada_Bandera[] = {"Bajada:"};
    const byte  _F_Dist[] = {"F.Dist:"};
    const byte  _F_Tiempo[] = {"F.Tiempo:"};
    const byte  _F_Total[] = {"F.TOTAL:"};
    const byte  _Nro_Movil[] = {"Nro Movil:"};
    const byte  _Nro_Recibo[] = {"Nro Recibo:"};
    const byte  _Espera[] = {"Espera:"};
    const byte  _A_PAGAR[] = {"A PAGAR:"};
    const byte  _GRACIAS[] = {"GRACIAS"};
    const byte  _Eq[] = {"Eq."};
  #endif

  #ifdef VISOR_REPORTES
    const byte  _Recaudacion_Parcial[] = {"RECAUDACION PARCIAL"};
    const byte  _Impreso_el[] = {"Impreso el:"};
    const byte  _RECAUDACION[] = {"RECAUDACION:"};
    const byte  _Inicio_Turno[] = {"Inicio Turno:"};
  #endif
  
  #ifdef VISOR_TICKET_TURNO
    const byte  _REPORTE_DE_TURNO[] = {"REPORTE DE TURNO"};
    const byte  _REPORTE_TURNO[] = {"REPORTE TURNO"};
    const byte  _Turno_Nro[] = {"Turno Nro:"};
    const byte  _Fin_Turno[] = {"Fin Turno:"};
    const byte  _LIBRE[] = {"LIBRE"};
    const byte  _OCUPADO[] = {"OCUPADO"};
    const byte  _FUERA_DE_SERVICIO[] = {"FUERA DE SERVICIO"};
    const byte  _Tiempo_Marcha[] = {"T. Marcha:"};
    const byte  _Tiempo_Parado[] = {"T. Parado:"};
    const byte  _TARIFA[] = {"TARIFA"};
    const byte  _TARIFA_ESPECIAL[] = {"TARIFA ESPECIAL"};
    const byte  _Diurna[] = {"Diurna"};
    const byte  _Nocturna[] = {"Nocturna"};
    const byte  _Viajes[] = {"Viajes:"};
    const byte  _Viajes_por_asiento[] = {"Viajes asiento:"};
    const byte  _Por_Asiento[] = {"Por Asiento:"};
    const byte  _Errores_asiento[] = {"Errores asiento:"};
    const byte  _Err_Asiento[] = {"Err.Asiento:"};
    const byte  _Recaudado[] = {"Recaudado:"};
    const byte  _Fichas[] = {"Fichas:"};
    const byte  _Tiempo[] = {"Tiempo:"};
    const byte  _Totales[] = {"TOTALES"};
    const byte  _RINDEX[] = {"RINDEX"};
    const byte  _KM_Totales[] = {"KM Totales:"};
    const byte  _KM_Total[] = {"KM Total:"};
    const byte  _Pesos_x_km[] = {"$ x km:"};
    const byte  _Percent_km_ocupado[] = {"%km Ocupado:"};
    const byte  _km_ocup[] = {"KM Ocup:"};
    const byte  _Desconex_Alim[] = {"Desconex. Alim:"};
    const byte  _DESCONEX_ALIM[] = {"DESCONEX ALIM."};
    const byte  _Cantidad[] = {"Cantidad:"};
    const byte  _Desconex_Acc[] = {"Desconex. Acc:"};
    const byte  _DESCONEX_ACC[] = {"DESCONEX ACC."};
    const byte  _Pesos_Perdidos[] = {"$ Perdidos:"};
    const byte  _Mov_sPulsos[] = {"Mov. s/Pulsos"};
  #endif
  
  #ifdef VISOR_PRINT_TICKET_TARJETA
    const byte  _COMPROBANTE_TARJETA[] = {"COMPROBANTE TARJETA"};
    const byte  _Nro_Transaccion[] = {"Transac.:"};
    const byte  _Nro_Tarjeta[] = {"Nro Tarjeta:"};
    const byte  _PAGO[] = {"PAGO:"};
    const byte  _RECARGA[] = {"RECARGA:"};
    const byte  _SALDO[] = {"SALDO:"};
    
    const byte  _REPORTE_TARJETA[] = {"REPORTE TARJETA"};
  #endif
  
  #ifdef VISOR_TICKET_30_DIAS
    const byte  _REPORTE_DESCONEXIONES[] = {"REPORTE DESCONEXIONES"};
    const byte  _REPORTE_30_DIAS[] = {"REPORTE 30 DIAS"};
    const byte  _REPORTE_DEL_DIA[] = {"REPORTE DEL DIA"};
    const byte  _DIA[] = {"DIA"};
    const byte  _Turnos[] = {"Turnos:"};
    const byte  _Dist_Ocup[] = {"Dist.Ocup:"};
    const byte  _km_Libre[] = {"km Libre:"};
    const byte  _km_Ocupado[] = {"km Ocupado:"};
    const byte  _km_FServ[] = {"km F.Serv:"};
  #endif
  
  #ifdef VISOR_TICKET_CONTROL_PARAMETROS  
    const byte  _CONTROL_DE_PARAMETROS[] = {"CONTROL DE PARAMETROS"};
    const byte  _Parametros_de_Movil[] = {"Parametros de Movil"};
    const byte  _Parametros_de_Reloj[] = {"Parametros de Reloj"};
    const byte  _Tipo_Servicio[] = {"Tipo Servicio:"};
    const byte  _TAXI[] = {"TAXI"};
    const byte  _REMIS[] = {"REMIS"};
    const byte  _Constante[] = {"Constante:"};
    const byte  _RTC[] = {"RTC:"};
    const byte  _Tecla_Espera[] = {"Tecla Espera:"};
    const byte  _Sin_Espera[] = {"Sin Espera:"};
    const byte  _Espera_Acum[] = {"Espera Acum.:"};
    const byte  _Tarifa_Manual[] = {"Tarifa Manual:"};
    const byte  _Choferes[] = {"Choferes:"};
    const byte  _Cant_Tarifas[] = {"Cant. Tarifas:"};
    const byte  _t_Gracia[] = {"t Gracia:"};
    const byte  _Dist_Inicial[] = {"Dist.Ini.:"};
    const byte  _Dist_Ficha[] = {"Dist.Ficha:"};
    const byte  _Hora_Inicio[] = {"Hora Inicio:"};
    const byte  _t_Ficha[] = {"t Ficha:"};
    const byte  _Asiento[] = {"Asiento:"};
    const byte  _Sin_Sensor[] = {"Sin Sensor"};
    const byte  _Normal[] = {"Normal"};
    const byte  _Inteligente[] = {"Inteligente"};
    const byte  _Car_Bandera[] = {"Car.Bandera:"};
    const byte  _seg[] = {"seg"};
    const byte  _DLMMJVS_CAL[] = {" D L M M J V S  CAL"};
  #endif
  
  #ifdef VISOR_TICKET_DETALLADO
    const byte  _REPORTE_DETALLADO[] = {"REPORTE DETALLADO"};
    const byte  _L[] = {"L"};        // Libre
    const byte  _O[] = {"O"};        // Ocupado
    const byte  _C[] = {"C"};        // Cobrando (A Pagar)
    const byte  _F[] = {"F"};        // Fuera Servicio
    const byte  _T[] = {"T"};        // Turno
    const byte  _V[] = {"V"};        // Varios
    const byte  _Inicia[] = {"Inicia"};
    const byte  _FD[] = {"d"};
    const byte  _FT[] = {"t"};
    const byte  _PROG_TALLER[] = {"PROG.EN TALLER"};
    const byte  _PROG_AIRE[] = {"PROG.POR AIRE"};
    const byte  _AYUDA[] = {"AYUDA"};
    const byte  _L_Libre[] = {"Libre"};
    const byte  _O_Ocupado[] = {"Ocupado"};
    const byte  _C_Cobrando[] = {"Cobrando"};
    const byte  _F_Fuera_Servicio[] = {"Fuera Servicio"};
    const byte  _T_Turno[] = {"Turno"};
    const byte  _D_Desconex_Alim[] = {"Desconex. Alim."};
    const byte  _X_Desconex_Acc[] = {"Desconex. Acc."};
    const byte  _V_Varios[] = {"Varios"};
    const byte  _HH[] = {"HH"};
    const byte  __MM[] = {":MM"};
    const byte  _km_[] = {"km"};
    const byte  _vel[] = {"vel"};
    const byte  _tSTOP[] = {"tSTOP"};
    const byte  _tarif[] = {"tarif"};
    const byte  _PESOS_fichas[] = {"$/fichas"};
    const byte  _tDESC[] = {"tDESC"};
    const byte  _error_sens_Asiento[] = {"error sens.Asiento"};
    const byte  _ayuda_sens_Asiento[] = {"\x07""*O\x05 -> ocupado por sensor asiento"};
  #endif    


  /* PANTALLA MENU CHOFER */
  /************************/
  #ifdef VISOR_REPORTES
    const byte  _CHOFER_1[] = {"CHOFER 1"};
    const byte  _CHOFER_2[] = {"CHOFER 2"};
    const byte  _CHOFER_3[] = {"CHOFER 3"};
    const byte  _CHOFER_4[] = {"CHOFER 4"};
  #endif
  
  
  /* PANTALLA MENU TICKET */
  /************************/
  #ifdef VISOR_PROG_TICKET
    const byte  _Recaud_Parcial[] = {"Recaud. Parcial"};
    const byte  _Ultimo_Viaje[] = {"Ultimo Viaje"};
    const byte  _Reporte_Turno[] = {"Reporte Turno"};
    const byte  _Reporte_Tarjeta[] = {"Reporte Tarjeta"};
    const byte  _Ultimo_Comprobante[] = {"Ultimo Comprob."};
    const byte  _Reporte_Descon[] = {"Reporte Descon."};
    const byte  _Reporte_30_Dias[] = {"Reporte 30 Dias"};
    const byte  _Control_Param[] = {"Control Param."};
    const byte  _Reporte_por_Dia[] = {"Reporte por Dia"};
    const byte  _Reporte_Detallado[] = {"Rep. Detallado"};
  #endif


  /* PANTALLA MENU REPORTE TURNO */
  /*******************************/
  #ifdef VISOR_PROG_TICKET
    const byte  _ID[] = {"ID: "};
    const byte  _CHF[] = {"CHF: "};
  #endif
  
  
  /* PANTALLA DISTANCIA y VELOCIDAD */
  /**********************************/
    const byte  _Dist[] = {"Dist:"};
    const byte  _Vel[] = {"Vel:"};
    const byte  _kmh2[] = {"km/h"};
    const byte  _mts[] = {"mts"};


  /* MODO TEST */
  /*************/
    const byte  _GPS[] = {"GPS"};
    const byte  _ACCESORIO[] = {"ACCESORIO"};
    const byte  _RADIO[] = {"RADIO"};
    const byte  _IMPRESORA[] = {"IMPRESORA"};
    const byte  _TARJETA[] = {"TARJETA"};
    const byte  _TECLADO[] = {"TECLADO"};
    
    const byte  _SELECCIONE_MODULO[] = {"SELECCIONE MODULO"};
    
    const byte  _COMUNICACION[] = {"COMUNICACION: "};
    const byte  _PREG[] = {"?"};
    const byte  _POSICION[] = {"POSICION: "};
    const byte  _Valida[] = {"Valida"};
    const byte  _No_Valida[] = {"No Valida"};
    const byte  _SINCRONISMO[] = {"SINCRONISMO: "};
    const byte  _LATITUD[] = {"LATITUD: "};
    const byte  _LONGITUD[] = {"LONGITUD: "};
    
  #ifdef VISOR_IMPRESORA
    const byte  _IMPRIMIENDO_TICKET[] = {"IMPRIMIENDO TICKET"};
  #endif
    const byte  _EQUIPO_SIN_IMPRESORA[] = {"EQUIPO SIN IMPRESORA"};
    
    const byte  _0[] = {"0"};
    const byte  _1[] = {"1"};
    const byte  _2[] = {"2"};
    const byte  _3[] = {"3"};
    const byte  _4[] = {"4"};
    const byte  _5[] = {"5"};
    const byte  _6[] = {"6"};
    const byte  _7[] = {"7"};
    const byte  _8[] = {"8"};
    const byte  _9[] = {"9"};
    
    const byte  _BANDERITA[] = {"BANDERITA"};
    const byte  _ASIENTO[] = {"ASIENTO"};
    const byte  _EMERGENCIA[] = {"EMERGENCIA"};
    const byte  _PULSOS[] = {"PULSOS: "};
    const byte  _DIV[] = {"DIV x"};
    const byte  _VEL[] = {"VEL: "};
    const byte  _SERIE[] = {"SERIE:"};
    const byte  _verificar_BANDERA_y_PARLANTE[] = {"verificar BANDERA y PARLANTE"};
    const byte  _0_micOculto[] = {"#0 => Micrifono Oculto"};
    
    const byte  _SALIDA_PTT[] = {"SALIDA PTT: "};
    const byte  _PORTADORA[] = {"PORTADORA"};
    const byte  _PTT_EXTERNO[] = {"PTT EXTERNO"};
    const byte  _PTT_VISOR[] = {"PTT VISOR"};
    const byte  _CANAL_DATOS[] = {"CANAL DATOS: "};
    const byte  _T1_tonos_de_prueba[] = {"#T1 => Tx tonos de prueba"};
    const byte  _verificar_CAMBIO_DE_CANAL[] = {"verificar CAMBIO DE CANAL"};
  
  #ifdef VISOR_TARJETA
    const byte  _PASE_LA_TARJETA[] = {"PASE LA TARJETA"};
  #endif
    const byte  _EQUIPO_SIN_TARJETA[] = {"EQUIPO SIN TARJETA"};
    
    
    /* MOVILES POR ZONA/BASE */
    /*************************/
    const byte  _MOVILES_EN_ZONA[] = {"MOVILES EN ZONA"};
    const byte  _MOVILES_EN_BASE[] = {"MOVILES EN BASE"};
    const byte  _Informacion[] = {"Informacion"};
    const byte  _de_Zonas[] = {"de Zonas"};
    const byte  _de_Bases[] = {"de Bases"};
    const byte  _no_disponible[] = {"no disponible"};
    
    
    /* CALCULO DE CONSTANTE */
    /************************/
    const byte  _CONSTANTE[] = {"CONSTANTE"};
    const byte  _RESET[] = {"RESET"};
    
    
    /* MENU REPORTE DETALLADO */
    /**************************/
  #ifdef VISOR_TICKET_DETALLADO
    const byte  _SELECCIONE_DIA[] = {"SELECCIONE DIA"};
    const byte  _SELECCIONE_HORA[] = {"SELECCIONE HORA"};
    const byte  _00[] = {":00"};
  #endif
        
    
    /* PANTALLA TARJETA MANUAL */
    /***************************/
    const byte  _Ingrese_el_Numero[] = {"Ingrese el Numero"};
    const byte  _CONTINUAR[] = {"CONTINUAR"};
  #ifdef VISOR_TARJETA
    const byte far _de_Tarjeta[] = {"de Tarjeta:"};
    
    const byte  _Ingrese_el_Codigo[] = {"Ingrese el Codigo"};
    const byte  _de_Seguridad[] = {"de Seguridad:"};
    const byte  _de_Autorizacion[] = {"de Autorizacion:"};
  #endif
    
    
    /* PANTALLA PARAMETROS */
    /***********************/
  #ifdef VISOR_PANTALLA_PARAMETROS
    const byte  _DESCONEX_RLJ[] = {"DESCONEX RLJ"};
    const byte  _ZONAS[] = {"ZONAS"};
    const byte  _BASES[] = {"BASES"};
    const byte  _RLJ_VIEJO[] = {"RLJ VIEJO"};
    const byte  _RLJ_NUEVO[] = {"RLJ NUEVO"};
    const byte  _REPORTES[] = {"REPORTES"};
    const byte  _GPRS[] = {"GPRS"};
    const byte  _DECIMALES[] = {"DECIMALES"};
    const byte  _CODIGO[] = {"CODIGO"};
    const byte  __OCULTOS[] = {" OCULTOS"};
    const byte  _HOMOLOGADO[] = {"HOMOLOGADO"};
    const byte  _TICKET[] = {"TICKET"};
    const byte  _GEOFENCE[] = {"GEOFENCE"};
    const byte  _MENSAJES[] = {"MENSAJES"};
    const byte  _NO_Tx_CH_[] = {"NO Tx CH "};
  #endif
    
    
    /* PANTALLA RECAUDADO */
    /**********************/
  #ifdef VISOR_REPORTES
    const byte  _VIAJES[] = {"VIAJES"};
  #endif
    
    
    /* PANTALLA CUPON */
    /******************/
    const byte  _de_Cupon[] = {"de Cupon:"};
    const byte  _de_Cuenta[] = {"de Cuenta:"};
    const byte  _ENVIAR[] = {"ENVIAR"};
    
    
    /* TO SPEECH */
    /*************/
  #ifdef VISOR_PRINT_TICKET_FULL    
    const byte  _mil[] = {"mil"};
    const byte  _dos_mil[] = {"dos mil"};
    const byte  _tres_mil[] = {"tres mil"};
    const byte  _cuatro_mil[] = {"cuatro mil"};
    const byte  _cinco_mil[] = {"cinco mil"};
    const byte  _seis_mil[] = {"seis mil"};
    const byte  _siete_mil[] = {"siete mil"};
    const byte  _ocho_mil[] = {"ocho mil"};
    const byte  _nueve_mil[] = {"nueve mil"};
    
    const byte  _cien[] = {"cien"};
    
    const byte  _ciento[] = {"ciento"};
    const byte  _doscientos[] = {"doscientos"};
    const byte  _trescientos[] = {"trescientos"};
    const byte  _cuatrocientos[] = {"cuatrocientos"};
    const byte  _quinientos[] = {"quinientos"};
    const byte  _seisientos[] = {"seisientos"};
    const byte  _setecientos[] = {"setecientos"};
    const byte  _ochocientos[] = {"ochocientos"};
    const byte  _novecientos[] = {"novecientos"};
    
    const byte  _dieci[] = {"dieci"};
    const byte  _veinti[] = {"veinti"};
    const byte  _treinta[] = {"treinta"};
    const byte  _cuarenta[] = {"cuarenta"};
    const byte  _cincuenta[] = {"cincuenta"};
    const byte  _sesenta[] = {"sesenta"};
    const byte  _setenta[] = {"setenta"};
    const byte  _ochenta[] = {"ochenta"};
    const byte  _noventa[] = {"noventa"};
    
    const byte  _cero[] = {"cero"};
    const byte  _un[] = {"un"};
    const byte  _dos[] = {"dos"};
    const byte  _tres[] = {"tres"};
    const byte  _cuatro[] = {"cuatro"};
    const byte  _cinco[] = {"cinco"};
    const byte  _seis[] = {"seis"};
    const byte  _siete[] = {"siete"};
    const byte  _ocho[] = {"ocho"};
    const byte  _nueve[] = {"nueve"};
    
    const byte  _veinte[] = {"veinte"};
    
    const byte  _diez[] = {"diez"};
    const byte  _once[] = {"once"};
    const byte  _doce[] = {"doce"};
    const byte  _trece[] = {"trece"};
    const byte  _catorce[] = {"catorce"};
    const byte  _quince[] = {"quince"};
    const byte  _dieciseis[] = {"dieciseis"};
    const byte  _diecisiete[] = {"diecisiete"};
    const byte  _dieciocho[] = {"dieciocho"};
    const byte  _diecinueve[] = {"diecinueve"};
  #endif
    
    
    /* PREMIUM */
    /***********/
  #ifdef _PREMIUM_
    const byte  _legal1[] = {"\x05""Buenos Aires \x06"};
    const byte  _legal2[] = {"\x05 a la vista pagare sin protesto (art.50 Decr. ley 5965/63)"};
    const byte  _legal3[] = {"a Servicios Premium S.A. la suma de pesos \x06"};
    const byte  _legal4[] = {"\x05 ("};
    const byte  _legal5[] = {" por igual valor recibido en servicio de taxi, a mi entera satisfaccion, pagaderos en Parana 426 2B, Ciudad de Buenos Aires."};
    const byte  _centavos[] = {"centavos)"};
    const byte  _FIRMA[] = {"\x07""FIRMA:\x05"".............."};
    const byte  _ACLARACION[] = {"\x07""ACLARACION:\x05""........."};
    const byte  _ACLARACION_dots[] = {"..................."};
    const byte  _DNI[] = {"\x07""DNI:\x05""................"};
    const byte  _Nro_Viaje[] = {"Nro Viaje:"};
    const byte  _Nro_Autoriz[] = {"Nro Autoriz.:"};
    const byte  _PremiumCard[] = {"PremiumCard Nro:"};
    const byte  _DESDE[] = {"\x07""DESDE: "};
    const byte  _HASTA[] = {"\x07""HASTA: "};
    const byte  _Coodenadas_GPS[] = {"Coords GPS  "};
    const byte  _DESDEdots[] = {"............"};
  #endif
   
    

#pragma CONST_SEG   DEFAULT
/**************************************************************************************************/
/* RUTINAS */
/***********/

  /* STRING FAR COPY */
  /*******************/
    void string_FAR_copy (byte* dest_ptr, byte* orig_ptr){
      while (*orig_ptr != 0){
        *dest_ptr++ = *orig_ptr++;
      }
      
      *dest_ptr = 0;
    }


  /* STRING FAR CONCAT */
  /*********************/
    void string_FAR_concat (byte* dest_ptr, byte* orig_ptr){
      while (*dest_ptr != 0){
        dest_ptr++;
      }
      
      string_FAR_copy(dest_ptr, orig_ptr);
    }


  /* STRING FAR LENGTH */
  /*********************/
    byte string_FAR_length (byte* orig_ptr){
      byte len;
      
      len = 0;
      while (*orig_ptr++ != 0){
        len++;
      }
      
      return(len);
    }



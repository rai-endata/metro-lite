
#include "Timer.h"
#include "Tarifacion Reloj.h"
#include "Reloj.h"
#include "Calculo Velocidad.h"
#include "Odometro.h"
#include "stm32f0xx_hal_uart.h"
#include "main.h"
#include "seriePROG.h"

  


    /*********************************************************************************************/
    /* PROTOTIPOS */
    /**************/
      // NO PRECISAS
      static void tmr_1mseg (void);
	  static void tmr_10mseg (void);
      static void tmr_16mseg (void);
      static void tmr_100mseg (void);
      static void tmr_500mseg (void);
      static void tmr_1seg (void);


      // PRECISAS
      static void tmr_1mseg_precisas (void);
      static void tmr_10mseg_precisas (void);
      static void tmr_16mseg_precisas (void);
      static void tmr_100mseg_precisas (void);
      static void tmr_500mseg_precisas (void);
      static void tmr_1seg_precisas (void);

      // TIME OUT LAZO
      static void TO_lazo (void);


    /*********************************************************************************************/
    /* VARIABLES */
    /*************/
      static tmr_gral_flags           tmr_gral_F1;  // Banderas Timer General
        #define _tmr_gral_F1          tmr_gral_F1.Byte
        #define TO_lazo_hab           tmr_gral_F1.Bits.lazo_hab
        #define TO_lazo_F             tmr_gral_F1.Bits.lazo_F
		#define TMR_GRAL_1mseg        tmr_gral_F1.Bits._1mseg
        #define TMR_GRAL_10mseg       tmr_gral_F1.Bits._10mseg
        #define TMR_GRAL_16mseg       tmr_gral_F1.Bits._16mseg
        #define TMR_GRAL_100mseg      tmr_gral_F1.Bits._100mseg
        #define TMR_GRAL_500mseg      tmr_gral_F1.Bits._500mseg
        #define TMR_GRAL_1seg         tmr_gral_F1.Bits._1seg

      static uint16_t base_ms;						    // Base de tiempos en miliseg.
      static byte ini_cnt=4;						//	detecta los primeros tres segundos
      byte   checkTIME=5;;
      static byte TO_lazo_cnt;                      // Contador de TIME OUT DE LAZO
        #define x100mseg_TO_lazo        100           // x100mseg a esperar en un lazo (FOR - WHILE)


    /*********************************************************************************************/
    /* ATENCION TIMER GENERAL DESDE LOOP */
    /************************************/
      byte seg_prueba;
      byte seg2_prueba;

      void TMR_GRAL_LOOP (void){
        // Atención de las temporizaciones NO PRECISAS desde el LOOP principal

		if (TMR_GRAL_1mseg){
			TMR_GRAL_1mseg = 0;
			tmr_1mseg();
		}

       if (TMR_GRAL_10mseg){
          TMR_GRAL_10mseg = 0;
          tmr_10mseg();
        }

        if (TMR_GRAL_16mseg){
          TMR_GRAL_16mseg = 0;
          tmr_16mseg();
        }


        if (TMR_GRAL_100mseg){
          TMR_GRAL_100mseg = 0;
          tmr_100mseg();
        }


        if (TMR_GRAL_500mseg){
          TMR_GRAL_500mseg = 0;
          tmr_500mseg();
        }

        if (TMR_GRAL_1seg){
          TMR_GRAL_1seg = 0;
          tmr_1seg();
          seg_prueba++;
        }
      }


    /*********************************************************************************************/
    /* TIMER GENERAL ISR */
    /*********************/
      void HAL_SYSTICK_Callback(void)
     {
          // Interrumpe c/ 1 mseg

          base_ms++;
          t_pulsos++;
          if (base_ms == 1001){
            base_ms = 0;
          }

            tmr_1mseg_precisas();             // Rutinas Ejecutadas cada 1 mseg

            if (((base_ms % 10) == 0) && (base_ms != 0)){
              TMR_GRAL_10mseg = 1;						// Se cumplieron 10mseg
              tmr_10mseg_precisas();          // Rutinas Ejecutadas cada 10 mseg
            }

            if (((base_ms % 16) == 0) && (base_ms != 0)){
              TMR_GRAL_16mseg = 1;						// Se cumplieron 16mseg
              tmr_16mseg_precisas();          // Rutinas Ejecutadas cada 16 mseg
            }

            if (((base_ms % 100) == 0) && (base_ms != 0)){
              TMR_GRAL_100mseg = 1;						// Se cumplieron 100mseg
              tmr_100mseg_precisas();         // Rutinas Ejecutadas cada 100 mseg
            }

            if (((base_ms % 500) == 0) && (base_ms != 0)){
              TMR_GRAL_500mseg = 1;						// Se cumplieron 500mseg
              tmr_500mseg_precisas();         // Rutinas Ejecutadas cada 500 mseg
            }

            if (((base_ms % 1000) == 0) && (!base_ms != 0)){
              TMR_GRAL_1seg = 1;              // Se cumplió 1seg
              tmr_1seg_precisas();            // Rutinas Ejecutadas cada 1 seg
            }
     }




    /*********************************************************************************************/
    /* RUTINAS INTERNAS */
    /********************/

      /* NO PRECISAS */
      /***************/

          /* Timer 10mseg LOOP */
          /**********************/

		  static void tmr_1mseg (void){

		  }

          static void tmr_10mseg (void){
        	  ASIENTO_desrebote_timer();
        	  rxVA_baxFORMAT_TimeOut();
        	  UART_RX_TimeOut(&huart1);
        	  UART_RX_TimeOut(&huart7);
        	  UART_TX_TimeOut(&huart1);
        	  UART_TX_TimeOut(&huart7);
        	  PRG_TO_serieRx();

          }

          /* Timer 16mseg LOOP */
          /**********************/

          static void tmr_16mseg (void){

          }

          /* Timer 100mseg LOOP */
          /**********************/

          static void tmr_100mseg (void){

        	  if(ini_cnt > 1){
        		//  watchDOG ();
        	  }
        	to_test_display();
			to_toggle_display();
			to_toggleTARIFA_display();

			update_ADC();
			finPRG_TO_RESET();
			Bluetooth_TimeOut();
			//RELOJ_timerTarifaTiempo();      // Toggle Indicacion tarifando por tiempo



        	  //PANTALLA_timerActualizar();     // Timer de Actualización de Pantalla
/*
 *
            #ifndef VISOR_USES_GPRS
              time_out_PTT_datos();         // Time Out PTT de RADIO para DATOS

              timer_chkPTTexterno();        // Chequeo del pin de PTT EXTERNO (MICROFONO)

             MDM_timeOut;                  // Time Out Modem Visor
             MDM_keepAlive;                // Mantener vivo al Modem si IRQ permanece en bajo

              timer_CAMBIOcanal();          // Temporización del cambio de canal
              timer_blockCAMBIOcanal();     // Temporización del BLOQUEO de cambio de canal

              PTT_VISOR_periodicControl_timer();  // Control Periódico de PTT VISOR
            #endif

            timer_relojDesconectado();      // TimeOut de Reloj Desconectado

            #ifdef VISOR_256
              RELOJprotocolo_TO_consulta();   // Reintentos Consulta de estado al reloj con protocolo
            #endif

            BANDERA_parpadeo_timer();       // Timer para parpadeo de bandera

            MENSAJE_delayEEPROM();          // Delay de grabación MSJ GRAL en EEPROM

            timer_calculo_velocidad();      // Temporización del calculo de velocidad
            VISOR_GPRS_reset_timer();   	// Timer Reset GPRS
  */
        	//  PRINT_Rx_TO();

          }


        /* Timer 500mseg LOOP */
        /**********************/
          static void tmr_500mseg (void){
           // antena_TO();                    // TimeOut de Flecha de Antenas
          }


        /* Timer 1seg LOOP */
        /*******************/
          static void tmr_1seg (void){

        	  if(prog_mode){
            		  HAL_GPIO_TogglePin(BANDERA_OUT_PORT, BANDERA_OUT_PIN);
           	  }

        	  Send_Tx_Valor_VIAJE();
        	  Toggle_BANDERA ();
        	  ini_cnt--;
        	  checkTIME--;
        	  if(ini_cnt == 0){
        		  ini_cnt = 1;
        		 // watchDOG ();
        	  }
        	  ASIENTO_chkSensor_timer();
        	  //rtc__actDATE();
        	  timeOut_FRECUENCIA();
        	  RELOJ_chkVelTransicion();
        	  RELOJ_a_pagar_to_libre_timer();
        	  cambioRELOJ_timer();
        	  to_printing();
        	  TARIFA_TimeOut();
        	  //PRUEBA
        	  //if(ESTADO_RELOJ == LIBRE){
        	  //	  Tx_Encendido_EQUIPO();
        	  // }

		#ifdef HABILITA_REINTENTOS
        	  reTx_timer();                   // Time Out de Retransmisiones
        	  //reTx_datosSC();
		#endif

        	  /*
            #ifdef VISOR_USES_GPRS
              //  VISOR_GPRS_keepAlive_TO_timer();  // Keep Alive de GPRS
                GPRS_PULSO_reint_timer();   // Timer reint pulso de reset gprs
       	   	   */
              //  PRINT_newCMD_TO_timer();   // TimeOut Rx AT CMD
               // PRINT_sendTO_timer();      // TimeOut Tx AT CMD

                /*
                VISOR_GPRS_noRxTO_timer();  // TimeOut no Rx
            #else
              MDM_transmitiendo_TO;         // Time Out de Modem Transmitiendo
              MDM_timeOutNoRxTx;            // Time Out de No Rx/Tx de Modem

              time_out_PTT_voz();           // Time Out PTT de RADIO para VOZ

              timer_BSQDcanal();			// Temporización de Búsqueda de Canal
              timeout_chNoActivity();       // TimeOut de Canal sin Actividad
              timeout_BSQDcanalEncendido(); // TimeOut para Buscar Canal si no Responde al Encendido Visor

              avoidPORTADORA_timer();       // Temporizador para no darle bola a la portadora al Tx, si esta se cuelga

              timer_maxTIMEvoz();           // Tiempo de Permanencia Máximo en Canal de Voz
              time_out_chVOZ();             // TimeOut en Canal de Voz (inactivo)
            #endif

            //PANTALLA_timeOut();             // TimeOut de Pantalla

            #ifndef VISOR_EXTERNAL_GPS
              fakeGPS_Cx_TO();                  // TimeOut de NO Comunicación con el GPS
              //GPS_autoBaud_timer();         // Auto-BaudRate Timer
              RTP_timer();                  // Temporización de Envió de Posición y Velocidad
              RTP_periodicTimer();          // Timer de envió de PyV periódico
              RTPperiodic_TO_timer();       // TimeOut de programación de Periodicidad de envió de PyV

              #ifdef VISOR_PROG_GEO_FENCE
                GF_chkTimer();              // Chequeo de Geo Fence
              #endif
            #else
              externalGPS_Cx_TO();          // TimeOut de NO Comunicación con el GPS externo
            #endif

            EntradaBaseVirtualAutomatica_timer(); // Timer Ingreso en Base Virtual Automáticamente

            //zonaNav_timeOut();              // TimeOut Navegación Zona

            //timer_salir_alRespoderViaje();  // Salir de Pantalla Acepta Viaje al Responder la Invitacion

            timeOut_Asignado();             // Time Out Asignado



            //BACKLIGHT_Teclado_timer();      // Temporización Backlight Teclado
            BACKLIGHT_Display_timer();      // Temporización Backlight Display

            RTC_timerConsulta();            // Timer de consulta periódica a RTC
            RTC_timerError();               // Timer de error en RTC
            RTC_periodicBackup_timer();     // Timer grabacion periodica de backup

            #ifdef VISOR_TARJETA
              TARJETA_blockTx_timer();      // Timer de Bloqueo de Tx de Tarjeta
              TARJETA_inhibitTimer();       // Timer de Inhibición de Lectura de Tarjeta
              TARJETA_notValid_TO();        // Time Out Tarjeta No Validada
            #endif

            ASIENTO_chkSerial_timer();      // Timer para leer Serial
            ASIENTO_chkSensor_timer();      // Timer para determinar sensor presionado o no
            ASIENTO_chkTiempo_sensor();     // Sensor Asiento por Tiempo

            BUZZER_timerRepeatTone();       // Temporización de Repetición de Tono

            RELOJ_INTERNO_timerMarchaParado();  // Temporización de Marcha/Parado en los estados de Reloj

            MovIrregular_timerChkSinPulsos();   // Timer de chequeo de movimiento sin pulsos

          #ifdef VISOR_TARJETA
            PagarConTarjeta_timerBloqueo();     // Timer de Bloqueo
          #endif

            ProntoLiberarse_timerBloqueo();     // Timer de Bloqueo
            RedespacharViaje_timerBloqueo();    // Timer de Bloqueo
            ReenviarDireccion_timerBloqueo();   // Timer de Bloqueo
            ReenviarDireccion_timerWindow();    // Timer de Ventana
            EnPuerta_timerBloqueo();            // Timer de Bloqueo
            UsarTelefonico_timerBloqueo();      // Timer de Bloqueo

            RELOJ_a_pagar_to_libre_timer(); // Timer para pasar A PAGAR -> LIBRE (Reloj HOMOLOGADO)
  */
        /*
            TEST_ingreso_TO();              // TimeOut de ingreso en MODO TEST
            TEST_toggleBandera_timer();     // Toggle Bandera en MODO TEST
            TEST_toggleCambioCanal_timer(); // Toggle Cambio de Canal en MODO TEST
            TEST_parlante_timer();          // Apagar parlante al probar AUDIO TONE GENERATOR
            TEST_micOculto_timer();         // Timer de Prueba del Micrófono Oculto
         */
/*
        	  HORA_APAGADO_periodicBKP_timer(); // Grabación PERIODICA de la HORA DE APAGADO

            REMATE_TO_timer();              // TimeOut de duración de un REMATE

            ACEPTA_VIAJE_waitingDIR_TO();   // TimeOut de ESPERANDO DIRECCION

            #ifndef VISOR_EXTERNAL_GPS
            #ifdef VISOR_ZONAS_EN_CENTRAL
              RTP_PyV_TO_timer ();          // TimeOut de Posición y Velocidad (reintentos)
            #endif
            #endif

            #ifdef VISOR_REPORTE_ACUMULATIVO
              REPACUM_timer();              // Chequeo de REPORTE ACUMULATIVO
            #endif

            #ifdef VISOR_REPORTES
              REPORTES_delayFLASH_update_timer(); // Delay de grabación de los reportes
            #endif

              sendGPS_to_serieVISOR();
              timeGPS_received();
              EMERGENCIA_enCorto();             // Pulsador de Emergencia en Corto
              timeout_EMERGinhib();           // TimeOut Inhibición Emergencia
              timeCHECK_SISR();
              TIMER_timeout_setBAUDRATE();
              motorOFF_TO();                     //motor apagado
              ultimoENVIO_TO_cnt_TO();
        */
             //   timeout_PRINT_send_TIMER();

          }





      /* PRECISAS */
      /************/

        /* Rutinas Timer 1mseg */
        /***********************/

          uint16_t CNT_PRUEBA;
          uint16_t CNT_PRUEBA_SEG;

          static void tmr_1mseg_precisas (void){
              TMR_GRAL_1mseg = 1;			// Se cumplieron 1mseg
        	  to_display_on();

        	  //TECLADO_count_TC();			// Temporización del Teclado

        	 //pulsos_cnt++;
        	 //if( pulsos_cnt == 1080){
        	 //	  pulsos_cnt=0;
        	 //	  cal_freq_pulsos();
        	 //}


            #ifndef VISOR_USES_GPRS
       ///       delay_PTT_timer();            // Temporización del Delay de PTT

       ///       MDM_SPI_timeOut;              // TimeOut de Recepción de datos por la SPI del Modem

       ///       huecoRTA_timer();             // Hueco para la respuesta

       ///       TxOverPortadora_timer();      // Timer de Transmission sobre la portadora de la Central

       ///       timer_portadoraVoz();         // Temporización de portadora de VOZ

       ///       PTT_VISOR_debounce_timer();     // Desrebote del PTT VISOR
            #endif

            #ifdef VISOR_IMPRESORA
              PRINTER_pulsosMotor();        // Pulsos para control del motor de impresora
            #endif

        ///    PRG_TO_serieRx();               // TimeOut Rx Serie Reloj

            #ifdef VISOR_256
              RLJ_TO_serieRx();             // TimeOut Rx Serie Programador
            #endif

         ///   ASIENTO_desrebote_timer();      // Desrebote del Sensor de Asiento
            //gprs
         ///   timeout_waitTxSOCKET_TIMER();
         ///   serieVISOR_TimeOut();

          }


        /* Rutinas Timer 10mseg */
        /***********************/
          static void tmr_10mseg_precisas (void){
				timer_muestreo_banderita();     // Temporizacion del Muestreo de Banderita
				timerCHK_pulsosTarifacion();    // Timer para la tarifacion por Pulsos
				timer_TarifacionTiempo();       // Temporizacion de la tarifacion por tiempo
				CONTADOR_tiempoPULSADO_timerT1();
				CONTADOR_tiempoPULSADO_timerT2();
				buzzer_TIMEOUT();
				buzzer_TIMEOUT_toOFF();
				pauseTONE_TIMEOUT();
				pauseTx_TIMEOUT();

       // TECLADO_timer();                // Timer para el Barrido del Teclado

       ///     timer_muestreo_banderita();     // Temporización del Muestreo de Banderita

       ///    time_out_calculo_periodo();			// Time Out Calculo de Velocidad por Periodo de Pulsos

       ///     BUZZER_timerTono();             // Temporización de Tonos: Duración y Pausa

            #ifndef VISOR_USES_GPRS
       ///       RetardoCelda_timer();         // Temporización del Retardo de Celda
       ///       timer_chkPortadora();         // Chequeo constante de portadora

       ///       timer_delayTxBSQDcanal();     // Delay de transmisión de Búsqueda, luego del cambio de canal
            #endif


            #ifdef VISOR_TARJETA
              TARJETA_readTO();             // TimeOut de Lectura de Datos de Tarjeta
            #endif


          }


        /* Rutinas Timer 16mseg */
        /***********************/
          static void tmr_16mseg_precisas (void){
      ///      BUZZER_timerTono();             // Temporización de Tonos del Buzzer
          }


        /* Rutinas Timer 100mseg */
        /***********************/
          static void tmr_100mseg_precisas (void){

        	  TO_lazo();                      // Time Out LAZO
            //cal_freq_pulsos();
            //timer_calculo_periodo();        // Inhibe el calculo de velocidad en el arranque del equipo

        //    TEST_update_timer();

        ///    WATCHDOG_timer();               // Timer de parpadeo de WatchDog

            #ifdef VISOR_TARIFACION_ESPECIAL
              RELOJ_timerTarifaEspecial();  // Timer Tarifacion especial (actualización pantalla)
            #endif

          ///    timer_LED(&led1);


          }


        /* Rutinas Timer 500mseg */
        /***********************/
          static void tmr_500mseg_precisas (void){
          }


        /* Rutinas Timer 1seg */
        /***********************/
          static void tmr_1seg_precisas (void){
     ///       PLL_crystalMonitor_timer();
        	  seg2_prueba++;
        	  RELOJ_INTERNO_timerMarchaParado();  // Temporizacion de Marcha/Parado en los estados de Reloj
        	  checkUART_ERROR();
        	  //cal_freq_pulsos();
          }


    /*********************************************************************************************/
    /* AUXILIARES */
    /**************/

      /* DISPARAR TIME OUT LAZO */
      /**************************/
        void dispararTO_lazo (void){
          TO_lazo_F = 0;                  // Bajo bandera de Time Out Lazo
          TO_lazo_hab = 1;                // Habilito Time Out de Lazo
        }


      /* DETENER TIME OUT LAZO */
      /*************************/
        void detenerTO_lazo (void){
          TO_lazo_F = 0;                  // Bajo bandera de Time Out Lazo
          TO_lazo_hab = 0;                // Deshabilito Time Out de Lazo
        }


      /* CHEQUEAR BANDERA DE TIME OUT LAZO */
      /*************************************/
        byte chkTO_lazo_F (void){
          return(TO_lazo_F);
        }


      /* TIME OUT DE LAZO */
      /********************/
        static void TO_lazo (void){
          // Ejecutada cada 100mseg
          // Time-Out para los lazos:
          //  - For
          //  - While
          if (TO_lazo_hab){
            TO_lazo_cnt++;
            if (TO_lazo_cnt == x100mseg_TO_lazo){
              TO_lazo_F = 1;
              TO_lazo_hab = 0;
              TO_lazo_cnt = 0;
            }
          }else{
            TO_lazo_cnt = 0;              // Si no esta habilitado, mantengo reseteado el CNT (cosa que cuando habilito empieza en cero)
          }
        }





      /* INHIBIR TIMER GENERAL */
      /*************************/
        void inhibirTimerGral (void){
        }


      /* DESINHIBIR TIMER GENERAL */
      /****************************/
        void desinhibirTimerGral (void){
        }

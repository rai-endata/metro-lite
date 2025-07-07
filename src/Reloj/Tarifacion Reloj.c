/* File: <Tarifacion Reloj.c> */

/* INCLUDES */
/************/

	#include "Reloj.h"
	//#include "Param Reloj.h"

	#include "stddef.h"
	#include "Textos.h"
	#include "Odometro.h"
	#include "Calculo Velocidad.h"
	#include "stm32f0xx_hal_tim.h"
	#include "Calculos.h"
	#include "Tarifacion Reloj.h"
	#include "Variables en flash.h"
//	#include "- Reloj-Blue Config -.h"
	#include "- metroBLUE Config -.h"
	#include "display-7seg.h"
	#include "tipo de equipo.h"
	//#include "Parametros Reloj.h"
	//#include "Grabaciones en EEPROM.h"

/*********************************************************************************************/
    /* ESTRUCTURAS */
    /***************/
      // BANDERAS DE TARIFACION
      typedef union{
        byte Byte;
        struct{
          byte bajada_byPulsos  :1;       // Indica que paso la bajada de bandera por pulsos
          byte bajada_byTiempo  :1;       // Indica que paso la bajada de bandera por tiempo
          byte tarifTiempo      :1;       // Bandera que indica que estoy tarifando por tiempo
          byte tarifPulsos      :1;       // Bandera que indica que estoy tarifando por pulsos
          byte no_espera        :1;       // Indica que no se deben contar fichas de espera
          byte chkTarifPulsos   :1;       // Bandera del Timer de chequeo de Tarifacion por Pulsos
          byte chkTarifTiempo   :1;       // Bandera del Timer de chequeo de Tarifacion por Tiempo
          byte act_valorviaje   :1;       // Indica que se debe actualizar el valor del viaje
        }Bits;

        struct{
          byte bajada_done      :2;       // Indica que ya pase la bajada de bandera, ya sea por tiempo/pulsos
        }MergedBits;

      }tTARIFACION_FLAGS;


/*********************************************************************************************/
/* PROTOTIPOS */
/**************/
  // VELOCIDAD TRANSICION
  static void calculoVELOCIDAD_TRANSICION (void);


  // TARIFACION POR DISTANCIA
  //static void RELOJ_iniPulsos (void);
  static void RELOJ_tarifarPulsos (void);
  static uint32_t RELOJ_convertirTiempo_to_Pulsos (uint16_t tiempo);
  static uint32_t getPulsos_whileTarifandoTiempo (void);
  
  // TARIFACION POR TIEMPO
  static void RELOJ_iniTiempo (void);
  static void RELOJ_esperaIni (void);
  static void RELOJ_tarfifarTiempo (void);
  void RedondeoValorViaje_3(void);
  void RedondeoValorViaje_2(void);

/*********************************************************************************************/
/* VARIABLES */
/*************/
  tTARIFA	TARIFA;                     // Tarifa activa
  ttTARIFA	TARIFA1,TARIFA2,TARIFA3,TARIFA4;
  ttEQPESOS	EqPesosDiurno;
  ttEQPESOS	EqPesosNocturno;
  tEQPESOS	EqPesosD;
  tEQPESOS	EqPesosN;

  
  static tTARIFACION_FLAGS                _tarifacion_F;     // Banderas de Tarifacion
    #define tarifacion_F                  _tarifacion_F.Byte
    #define bajadaBandera_done            _tarifacion_F.MergedBits.bajada_done
      #define bajadaBandera_byPulsos      _tarifacion_F.Bits.bajada_byPulsos
      #define bajadaBandera_byTiempo      _tarifacion_F.Bits.bajada_byTiempo
    #define tarifandoTiempo               _tarifacion_F.Bits.tarifTiempo
    #define tarifandoPulsos               _tarifacion_F.Bits.tarifPulsos
    #define no_contar_espera              _tarifacion_F.Bits.no_espera
    #define timerCHK_pulsosTarifacion_F        _tarifacion_F.Bits.chkTarifPulsos
    #define timerCHK_tiempoTarifacion_F        _tarifacion_F.Bits.chkTarifTiempo
    #define tarifacion_actValorViaje          _tarifacion_F.Bits.act_valorviaje



    uint32_t pulsosTarifacion;               // Valor del Pulse Accumulator para la tarifacion por pulsos
  // EDIT 19/04/2013
  //  Cuando va por debajo de la velocidad de transicion, los pulsos siguen entrando => Estos no se
  // deben tener en cuenta a la hora de tarifar (para TAXI. Los REMISES si lo tienen en cuenta). Para
  // lograr esto, debo guardar el valor de comenzar a tarifar por tiempo. Luego al volver a tarifar
  // por distancia le "quito" esa diferencia de pulsos
  uint32_t pulsos_whenIniTiempo;           // Valor del Pulse Accumulator al iniciar tarifacion por TIEMPO
  uint32_t segTarifacion_to_pulsos;        // Segundos de tarifacion excedentes convertidos a tiempo
  uint16_t  segundosTarifacion;             // Cantidad de segundos de tarifacion
  uint16_t  segundosEspera;                 // Segundos de Espera
  uint8_t  dif_espera;                 // Segundos de Espera

  /*
    Aclaracion: Los "segundosEspera" es un contador de segundos que se incrementa mientras esta
    tarifando por tiempo. No se resetea nunca.
    "segundosTarifacion", en cambio, tiene los segundos equivalentes a una cantidad entera de fichas.
    Por eso, el tiempo convertido a pulsos es: los segundos totales, menos los segundos equivalentes
    a una cantidad entera de fichas.
    
    Con pulsos tarifacion es algo parecido. Como pueden entrar pulsos mientras esta por debajo de la
    velocidad de transicion, apenas supera la misma guardo los pulsos acumulados hasta el momento en
    pulsosTarifacion, Luego, a los pulsos debo restarle esta cantidad para que sean los pulsos desde
    que esta tarifando por tiempo. Y con cada ficha, le sumo su equivalente en pulsos.
  */

  uint16_t CANTIDAD_PULSOS_FICHA;           // Equivalente en PULSOS de la Distancia de Ficha
  uint16_t CANTIDAD_PULSOS_BAJADA;          // Equivalente en PULSOS de la Distancia Inicial (Bajada Bandera)
  uint16_t TIEMPO_FICHA_INICIAL;            // Equivalente en TIEMPO de la Distancia Inicial (Bajada Bandera)
  

  uint32_t fichas_xPulsos;          // Cantidad de Fichas que cayeron por PULSOS
  uint32_t fichas_xTiempo;          // Cantidad de Fichas que cayeron por TIEMPO
  uint32_t fichas_Total;
  uint32_t VALOR_VIAJE;                    // Valor del Viaje
  uint32_t VALOR_VIAJE_PACTADO;            // Valor del Viaje recibido desde la central
  uint8_t puntoDecimal_PACTADO;            // punto decimal recibido desde la central

  uint32_t VALOR_VIAJE_mostrar;			   // Valor del Viaje a mostrar en la app del celular
  byte tarifa;
  static byte timerCHK_pulsosTarifacion_cnt;   // Contador del Timer para Chequear Tarifacion por Pulsos
    #define x100mseg_chk_pulsosTarifacion   2   // x10mseg (20mseg) para chequear los pulsos de tarifacion
  
  static byte timer_TarifacionTiempo_cnt;      // Contador de tiempo para la tarifacion por tiempo, como la base es 100mseg, cuenta hasta 10 para contar 1seg
    #define x100mseg_timer_TarifacionTiempo 100  // x10mseg (1seg) para contar 1seg de tarifacion por tiempo

  /*********************************************************************************************/
  /* MACROS */
  /**********/


  #define setTIMER_tarifTiempo        (timer_TarifacionTiempo_cnt = x100mseg_timer_TarifacionTiempo)
  #define clearTIMER_tarifTiempo      (timer_TarifacionTiempo_cnt = 0)
  #define setTIMER_tarifPulsos        (timerCHK_pulsosTarifacion_cnt = x100mseg_chk_pulsosTarifacion)
  #define clearTIMER_tarifPulsos      (timerCHK_pulsosTarifacion_cnt = 0)

/*********************************************************************************************/
/* RUTINAS */
/***********/

   /* INICIALIZACION DE TARIFACION */
  /********************************/

    void RELOJ_iniTarifacion (void){
      // Se llama al pasar a OCUPADO. Antes de hacer nada, debo inicializar la tarifa
      // La tarifa se inicializa:
      //  - Al seleccionar la tarifa en MENU RELOJ
      //  - Al presionar T1 largo (de acuerdo a la vigencia) NO IMPLEMENTADO AUN
      RELOJ_calcCantidadPulsosFicha();        // Pulsos equivalentes a 1 Ficha
      RELOJ_calcCantidadPulsosBajada();       // Pulsos equivalentes a la Bajada de Bandera
      RELOJ_calcTiempoFichaInicial();         // Equivalente en Tiempo de la Distancia de Bajada de Bandera

      calculoVELOCIDAD_TRANSICION();              // Calculo de la Velocidad de Transición

      tarifandoPulsos = 0;                    // No estoy tarifando pulsos, xq el cambio de reloj se hace detenido
      // Realizo las misma acciones que cuando esta por debajo de la velocidad de transición
      tarifandoTiempo = 1;                    // Como pasa a OCUPADO detenido, comienza tarifando por tiempo


      setTIMER_tarifTiempo;                   // Disparo timer Tarifacion por Tiempo
      //tarifacion_actValorViaje = 1;           // Actualizar valor de viaje
      dispararIndicacionTarifaTiempo();       // Disparar Toggle Indicación Tarifando por Tiempo
      //actualizarTarifaTiempo;                 // Actualizar Indicación Tarifando por Tiempo
      //mostrarTarifandoTiempo;                 // Mostrar Indicación Tarifando por Tiempo

      // Reseteo variables de tarifacion
      bajadaBandera_done = 0;
      fichas_xTiempo = 0;
      fichas_xPulsos = 0;
      guardar_fichas_xpulsos_backup(fichas_xPulsos);
      guardar_fichas_xtiempo_backup(fichas_xTiempo);

      if(TARIFA_PESOS){
               //valor en pesos
         	  VALOR_VIAJE  = TARIFA.bajadaBandera;                             // Bajada Bandera
      }else{

    	  if(EqPESOS_hab){
        	  //valor en fichas
    		  if(TARIFA.diaNoche==0){
    			//tarifa diurna
    			VALOR_VIAJE  = EqPESOS_DIURNA.bajadaBandera;                             // Bajada Bandera
    		  }else{
    			 //tarifa Nocturna
    			VALOR_VIAJE  = EqPESOS_NOCTURNA.bajadaBandera;                             // Bajada Bandera
    		  }
    	  }else{

    		  VALOR_VIAJE  = TARIFA.bajadaBandera * TARIFA.valorFicha;
    	  }

		 // RedondeoValorViaje_3(); //valor enviar app

     }
      //valor mostar en display

      if(TARIFA_PESOS){
    	  //muestra importe
    	  update_valor_display(VALOR_VIAJE);
      }else{
    	  if(EqPESOS_hab){
    		  fichas_Total = fichas_xTiempo + fichas_xPulsos;
    	  }else{
    		  fichas_Total = TARIFA.bajadaBandera + fichas_xTiempo + fichas_xPulsos;
    	  }
		  update_valor_display(fichas_Total);
      }


      //  EDIT 13/047/12
      //  Como el REMIS toma TODOS los pulsos como pulsos de tarifacion => Al pasar a ocupado guardo el
      //  valor del contador de pulsos, para que siempre "compare" contra este valor y de esta manera
      //  nunca se coma pulsos y absolutamente todos sean pulsos de tarifacion.
      //  Al servicio de TAXI no le afecta, porque comienza tarifando por tiempo, y al iniciar la
      //  tarifacion por pulsos, ahi "congela" el valor del contador de pulsos
        //pulsosTarifacion = ODOMETRO_getPulsosReales(); // Valor de contador de pulsos al pasar a OCUPADO
      pulsosTarifacion = __HAL_TIM_GET_COUNTER(&pulsoACCUM);

      // EDIT 19/04/2013
      //  Al comenzar la tarifacion, como el movil esta detenido, comeinza tarifando por tiempo => debo
      // actualizar el valor de los pulsos al comenzar la tarifacion por tiempo
      pulsos_whenIniTiempo = pulsosTarifacion;          // Inicializo pulsos cuando comeinza a tarifar por tiempo
      segTarifacion_to_pulsos = 0;
      segundosTarifacion = 0;
      segundosEspera = 0;
      dif_espera = 0;
      minutosEspera = 0;
      minutosEspera_anterior = 0;
      segundosEspera = 0;
      RELOJ_esperaIni();                                    // Inicializacion de la espera

		if(paseOCUPADO_PACTADO){
			VALOR_VIAJE = VALOR_VIAJE_PACTADO;
			PUNTO_DECIMAL = puntoDecimal_PACTADO;
		}
    }


  /* CALCULAR IMPORTE y CONVERTIR A STRING */
  /*****************************************/
    uint32_t RELOJ_calcularImporte (uint32_t fichasT, uint32_t fichasD, byte viajes, byte nroTarifa, byte* buffer){
      // Rutina que calcula el IMPORTE segun:
      //  - Fichas T
      //  - Fichas D
      //  - Cantidad de Viajes
      //  - Nro Tarifa.
      //
      // La conversion a string es opcional. Si el buffer es NULL no lo convierte a string
      tTARIFA tempTarifa;
      tEQPESOS eqPesos;

      uint32_t importe;
      
      if (TARIFA_PESOS){
        // PESOS
        if (nroTarifa!= 0){
    
          tempTarifa = prgRELOJ_getTarifa(nroTarifa);          //toma la direccion de la tarifa usada
          importe = (viajes * tempTarifa.bajadaBandera);
          importe += (fichasT * tempTarifa.valorFichaTiempo);
          importe += (fichasD * tempTarifa.valorFicha);
          
        
          if (buffer != NULL){
            *buffer++ = '$';                        // Agrego signo $
            convert_to_string_with_decimals(buffer, importe, 0xFF, PUNTO_DECIMAL, base_DECIMAL);
          }
        
        }else{
          // Si al recuperar tiene tarifa 0 => muestro N/A
          if (buffer != NULL){
            string_FAR_copy(buffer, _NA);
          }
          importe = 0;
        }
      
      }else{
        // FICHAS
        if (EqPESOS_hab != 0){
          // Con Equivalencia en Pesos => La imprimo
          if (nroTarifa > 4){
            // NOCTURNA
            eqPesos = EqPESOS_NOCTURNA;
        	//  eqPesos = EqPesosN;
          }else{
            // DIURNA
            eqPesos = EqPESOS_DIURNA;
        	//  eqPesos = EqPesosD;
          }
          importe = (viajes * eqPesos.bajadaBandera);
          importe += (fichasT * eqPesos.valorFichaTiempo);
          importe += (fichasD * eqPesos.valorFicha);
          
          if (buffer != NULL){
            *buffer++ = '$';                        // Agrego signo $
            convert_to_string_with_decimals(buffer, importe, 0xFF, 3, base_DECIMAL);
            //convert_to_string_with_decimals(buffer, importe, 0xFF, PUNTO_DECIMAL, base_DECIMAL);
          }
        
        }else{
          // Sin Equivalencia en Pesos => N/A
          if (buffer != NULL){
            string_FAR_copy(buffer, _NA);
          }
          importe = 0;
        }
      }
      
      return(importe);
    }


    

  /* LEVANTAR TARIFA <NRO> */
  /*************************/
    void RELOJ_setTarifa (byte nro){
      // Rutina que setea la tarifa, extrayendola de la programacion de EEPROM
      // segun el numero de la misma

    	TARIFA = prgRELOJ_getTarifa(nro);
    }


//#ifdef  _REMISES_KATE_
#ifdef VISOR_TARIFACION_KATE
    void RELOJ_setTarifa_kate (byte nro){
      // Rutina que setea la tarifa, extrayendola de la programacion de EEPROM
      // segun el numero de la misma
      TARIFA_KATE = prgRELOJ_getTarifa_Kate(nro);
    }

#endif

  /* SETEAR SOLO EL NRO DE TARIFA */
  /********************************/
    void RELOJ_setNUMTarifa (byte nro){
      // Para tarifacion especial, xq al mandar el dato al RTC (BKP), lo saca de aca
      TARIFA.numero = nro;
    }    
  
    

  /* CALCULAR CANTIDAD DE PULSOS POR FICHA */
  /*****************************************/
    void RELOJ_calcCantidadPulsosFicha(void){
      //                                                                 PULSOS_x_KM
      //      CANTIDAD_PULSOS_FICHA = --------------- x DISTANCIA_FICHA
      //                                    1000
      uint32_t temp;
      
      temp = PULSOS_x_KM; 
      temp *= TARIFA.distFicha;
      temp /= 1000;
      
      CANTIDAD_PULSOS_FICHA = (word) temp;
    }



  /* CALCULAR CANTIDAD DE PULSOS DE LA BAJADA DE BANDERA */
  /*******************************************************/
    void RELOJ_calcCantidadPulsosBajada(void){
      // Calcula el equivalente en pulsos de la DISTANCIA INICIAL (distancia durante la cual
      // vale la Bajada de Bandera)
      //
      //                                 PULSOS_x_KM
      //      CANTIDAD_PULSOS_BAJADA = --------------- x DISTANCIA_INICIAL
      //                                    1000
      uint32_t temp;
      
      temp = PULSOS_x_KM;
      temp *= TARIFA.distInicial;
      temp /= 1000;
      
      CANTIDAD_PULSOS_BAJADA = (word) temp;
    }

          
  /* CALCULAR TIEMPO DE FICHA INICIAL */
  /**************************************/
    void RELOJ_calcTiempoFichaInicial (void){
      // El TIEMPO DE FICHA INICIAL es el tiempo equivalente a la DISTANCIA INICIAL
      //
      //                                                           DISTANCIA_INICIAL        
      //      TIEMPO_FICHA_INICIAL = ---------------------  x TIEMPO_FICHA
      //                                DISTANCIA_FICHA
     
      uint32_t temp;
      
      temp = (uint32_t)(TARIFA.distInicial) * (uint32_t)(TARIFA.tiempoFicha);
      temp /= TARIFA.distFicha;
      
      TIEMPO_FICHA_INICIAL = (word) temp;
    }


    void tarifarRELOJ (void){
		if(ESTADO_RELOJ==OCUPADO){
			RELOJ_tarifacion();
		}
		if(paseOCUPADO_PACTADO){
			VALOR_VIAJE = VALOR_VIAJE_PACTADO;
			PUNTO_DECIMAL = puntoDecimal_PACTADO;
		}
	}

  /* TARIFACION - CALCULO DEL VALOR DEL VIAJE */
  /********************************************/
    void RELOJ_tarifacion (void){
      // Se llama desde el LOOP, sólo si esta ocupado y se solicito la actualizacion del valor
      // del viaje. El valor de viaje se actualiza por:
      //  - Inicio de Tarifacion (acaba de pasar a ocupado)
      //  - Nueva ficha por pulsos
      //  - Nueva ficha por tiempo
      byte tarifar;
      
      tarifar = 0;
      #ifdef VISOR_TARIFACION_ESPECIAL
        if (TARIFA.numero != nroTARIFA_ESPECIAL){
          tarifar = 1;
        }
      #else
        tarifar = 1;
      #endif
      
      RELOJ_tarifarPulsos();                        // Tarifación por pulsos (lo idea es que este inmediatamente debajo del calculo velocidad)
      RELOJ_tarfifarTiempo();                        // Tarifación por tiempo
      
      if (tarifacion_actValorViaje && tarifar){
        tarifacion_actValorViaje = 0;                // Bajo bandera
        
        
     //#ifdef  _REMISES_KATE_

      if(TARIFA_PESOS){
          //valor en pesos
    	  VALOR_VIAJE  = TARIFA.bajadaBandera;                             // Bajada Bandera
          VALOR_VIAJE += (fichas_xPulsos * TARIFA.valorFicha);            // Fichas por Pulsos
          VALOR_VIAJE += (fichas_xTiempo * TARIFA.valorFichaTiempo);      // Fichas por Tiempo
      }else{
    	  if(EqPESOS_hab){
              //valor en fichas
             if(TARIFA.diaNoche==0){
    			//tarifa diurna
            	 VALOR_VIAJE  = EqPESOS_DIURNA.bajadaBandera;
            	 VALOR_VIAJE += (fichas_xPulsos * EqPESOS_DIURNA.valorFicha);            // Fichas por Pulsos
            	 VALOR_VIAJE += (fichas_xTiempo * EqPESOS_DIURNA.valorFichaTiempo);      // Fichas por Tiempo
             }else{
            	 //tarifa Nocturna
     			VALOR_VIAJE  = EqPESOS_NOCTURNA.bajadaBandera;                             // Bajada Bandera
     			VALOR_VIAJE += (fichas_xPulsos * EqPESOS_NOCTURNA.valorFicha);            // Fichas por Pulsos
     			VALOR_VIAJE += (fichas_xTiempo * EqPESOS_NOCTURNA.valorFichaTiempo);      // Fichas por Tiempo

             }
    	  }else{
              //valor en pesos
        	  VALOR_VIAJE  = TARIFA.bajadaBandera * TARIFA.valorFicha;                             // Bajada Bandera
              VALOR_VIAJE += (fichas_xPulsos * TARIFA.valorFicha);            // Fichas por Pulsos
              VALOR_VIAJE += (fichas_xTiempo * TARIFA.valorFichaTiempo);      // Fichas por Tiempo
    	  }
        // RedondeoValorViaje_3();
      }
        // EDIT 16/01/13
        //  Tengo que hacer el decremento en un WHILE, porque sino la 2da y sucesivas
        // veces que sea mayor a ese valor (2000.00) solo me va a restar 1000.00 y se
        // me va a tildar, xq no puede mostrar ese importe en pantalla

      if(TARIFA_PESOS){
    	  if(EQUIPO_METRO_BLUE){
        	  //valor en pesos
              while (VALOR_VIAJE > 99999){
                  // Si me voy de rango, sigo contando, llevando la diferencia
                  VALOR_VIAJE -= 100000;
                }
    	  }
      }else{
    	  //valor en fichas
          //while (VALOR_VIAJE > 99999){
              // Si me voy de rango, sigo contando, llevando la diferencia
              //VALOR_VIAJE -= 100000;
            //}
      }


      if(TARIFA_PESOS){
    	  //muestra importe
    	  update_valor_display(VALOR_VIAJE);
      }else{
    	  if(EqPESOS_hab){
    		  fichas_Total = fichas_xTiempo + fichas_xPulsos;
    	  }else{
    		  fichas_Total = TARIFA.bajadaBandera + fichas_xTiempo + fichas_xPulsos;
    	  }
		  update_valor_display(fichas_Total);
      }

      //para que actualice apenas cambia una ficha(y no espere a que se cumpla el timer)
      Tx_Valor_VIAJE();
      cntSendValorViaje=3; //para que espere 3 segundos para enviar desde el timer

      //actualizarRELOJ;                // Actualizar zona del display donde va el importe
      }
    }


  /* EXTRAER CANTIDAD DE FICHAS TARIFADAS POR TIEMPO */
  /***************************************************/
    uint32_t TARIFACION_getFichasT (void){
      return(fichas_xTiempo);
    }


  /* EXTRAER CANTIDAD DE FICHAS TARIFADAS POR DISTANCIA (PULSOS) */
  /***************************************************************/
    uint32_t TARIFACION_getFichasD (void){
      return(fichas_xPulsos);
    }    
      
    

/*********************************************************************************************/
/* VELOCIDAD TRANSISION */
/************************/

  /* CALCULO DE LA VELOCIDAD DE TRANSICION */
  /*****************************************/
    static void calculoVELOCIDAD_TRANSICION (void){
      // Utilizamos una histeresis. Por lo tanto, no es el mismo el valor que hay que superar
      // cuando se esta por debajo, que el valor que hay que disminuir cuando se esta por
      // arriba de la velocidad de transision.
      // La ecuacion que determina el valor exacto de la velocidad de transicion es:
      //        VELOCIDAD_TRANSICION = 3.6 * DISTANCIA_FICHA / TIEMPO_FICHA [km/h]
      //
      // DISTANCIA_FICHA -> [mts]
      // TIEMPO_FICHA -> [seg]
      //
      // EDIT 06/07/2012
      //  Para compatibilizar el funcionamiento como TAXI y como REMIS, cuando el movil
      //  trabaje como remis, la velocidad de transicion va a ser 0km/h, de modo que
      //  mientras se mueva SIEMPRE cuente pulsos, y solo cuente tiempo cuando se detenga
      uint32_t VelocidadTransicion;
      uint32_t TiempoFicha;
      
      if (SERVICIO_TAXI){
        TiempoFicha = TARIFA.tiempoFicha;
        VelocidadTransicion = 0;
        VelocidadTransicion = (((uint32_t) TARIFA.distFicha) * 360) / TiempoFicha;
       // VelocidadTransicion = VelocidadTransicion*100;	//le agrego dos decimales
        // Histeresis de 1%, ancho de ventana 2%  
        HISTERESIS_VT = (2 * VelocidadTransicion / 100);
        VELOCIDAD_TRANSICION = (VelocidadTransicion - (HISTERESIS_VT/2));
      
      }else{
        HISTERESIS_VT = 0;          // Histeresis Nula
        VELOCIDAD_TRANSICION = 0;   // Velocidad de Transicion 0km/h
      }
    }    


  /* CHEQUEO SI SUPERE LA VELOCIDAD DE TRANSICION */
  /************************************************/
    void RELOJ_chkVelTransicion (void){
      // Se llama cada vez que calculo la velocidad; y en el TimeOut de Velocidad
      // ¿Y si la llamo desde el LOOP luego de calcular la velocidad?
      // AHORA LA LLAMO DESDE EL LOOP
      //
      // EDIT 06/07/2012
      //  Para homogeneizar el funcionamiento de TAXI y REMIS, cuando el movil
      //  trabaje como remis, la velocidad de transicion va a ser 0km/h, de modo que
      //  mientras se mueva SIEMPRE cuente pulsos, y solo cuente tiempo cuando se detenga
      
      if (RELOJ_OCUPADO /*&& !chkSIN_PULSOS*/){
        if (VELOCIDAD > VELOCIDAD_TRANSICION){
          // ESTOY POR ENCIMA DE LA VELOCIDAD DE TRANSICION
          // - Estoy por encima de la velocidad de transicion => Puedo decir que me estan ingresando pulsos de reloj
                 //
                // Las tareas a realizar son:
                //  A- Chequear si recien deje de tarifar por tiempo
          if (tarifandoTiempo){
            // A- Chequear si recien deje de tarifar por tiempo.
            tarifandoTiempo = 0;          // Dejo de tarifar por Tiempo
            tarifandoPulsos = 1;          // Comienzo a tarifar por Pulsos
            
            RELOJ_iniPulsos();            // Inicializacion de tarifacion por pulsos
          }else{
        	  //programacion defensiva
        	  //tarifandoPulsos = 1;          // Comienzo a tarifar por Pulsos
          }
        
        }else{
          // ESTOY POR DEBAJO DE LA VELOCIDAD DE TRANSICION
          // - Estoy por debajo de la velocidad de transicion => Debo tarifar por tiempo
                //
                // Las tareas a realizar son:
                //  A- Chequear si recien deje de tarifar por pulsos
                //  B- No acumular espera
          //  C- Como estoy por debajo de la VelTransicion, cambiar el valor de la misma, por el tema de la histeresis
          //  D- Indicar Tarifando por Tiempo
          //  E- Inicializar la Espera
          if (tarifandoPulsos){
            // A- Chequear si recien deje de tarifar por pulsos.
            tarifandoPulsos = 0;          // Dejo de tarifar por Pulsos 
            tarifandoTiempo = 1;          // Comienzo a tarifar por Tiempo
            
            RELOJ_iniTiempo();            // Inicializacion de tarifacion por tiempo
          }else{
        	  //programacion defensiva
        	  //tarifandoTiempo = 1;          // Comienzo a tarifar por Tiempo
          }
        }
      }
    }

  

/*********************************************************************************************/
/* TARIFACION POR DISTANCIA */
/****************************/
/*
  La tarifacion por distancia o pulsos, funciona de la siguiente manera:
  Como ahora nos valemos del contador de pulsos de hardware, el micro funciona libremente mientras
  los pulsos ingresan si que este se entere.
  Al dejar de tarifar por TIEMPO y empezar a tarifar por PULSOS, se "congela" el valor del PULSE
  ACCUMULATOR en una variable. Luego, se va haciendo la diferencia entre este valor congelado
  y el valor actual del ACUMULADOR DE PULSOS, cada vez que se supera el valor de una ficha, ya
  sea una ficha comun o la bajada de bandera, se congela el nuevo valor del acumulador de pulsos
  decontandole el equivalente en pulsos de esa ficha y quedandome con el excedente respecto del valor
  congelado anteriormente. De esta manera, el valor de la diferencia entre los pulsos actuales y los
  congelados es SIEMPRE MENOR A UNA FICHA. Cabe aclarar que por cada vez que se supera el valor de 1
  ficha, se incrementa el contador de FICHAS POR DISTANCIA.
  Luego, cuando vuelve a tarifar por TIEMPO, la cantidad pulsos que ingresaron mientras tarifaba por
  pulsos (diferencia entre el valor congelado y el valor a la hora de volver a tarifar por tiempo)
  es guardado en otra variable, para ir llevando una cuenta de la cantidad de pulsos que no superan una
  ficha y tampoco son convertidos a tiempo. Esta cantidad de pulsos, hace las veces de un "offset" para
  la proxima vez que se vuelva a tarifar por pulsos.
*/

  /* INICIALIZAR TARIFACION POR PULSOS */
  /*************************************/
     void RELOJ_iniPulsos (void){
      // Como su nombre lo indica inicializa la tarifacion por distancia. Se llama cada vez que se
      // supera la velocidad de transision
      //
      // Las tareas a realizar son:
          //  B- Congelo los pulsos, xq a partir de ahora ya cuentan como pulsos de reloj
          //     EDIT 13/07/12
          //     En el caso de que trabaje como remis, TODOS los pulsos cuentan como pulsos
          //     de tarifacion, por lo tanto no tengo que congelar NUNCA los pulsos de tarifacion
          //     Sino que al iniciar la tarifacion, es decir al pasar a ocupado, los congelo.
          //     De esta manera, garantizo que TODOS, absolutamente TODOS los pulsos que entren
          //     sean pulsos de tarifacion
          //
      //  C- Como estoy por encima de la VelTransicion, cambiar el valor de la misma, por el tema de la histeresis
      //  D- Chequear si esta programado para acumular espera para convertir TIEMPO --> PULSOS
      //  *E*- Si es REMIS no convierte TIEMPO --> PULSOS
      //     EDIT 12/07/12
      //      La conversion del TIEMPO -> PULSOS no depende del tipo de servicio, sino de la programacion
      //     de la espera, mas precisamente, de la ESPERA NO ACUMULATIVA
      //     Por lo tanto, el punto (E) desaparece
      //     
      //     EDIT 10/12/12
      //      Para REMIS, la ACUMULACION DE ESPERA, no tiene que ver con la conversion TIEMPO -> PULSOS, sino con
      //     no resetear el contador de tiempo
      //
      //  F- Convertir TIEMPO --> PULSOS, si dio "positivo" alguno de los chequeos
      //  G- Dejar de Indicar Tarifando por Tiempo
      clearTIMER_tarifTiempo;                     // Detengo timer Tarifacion por Tiempo
      setTIMER_tarifPulsos;                       // Disparo timer Check Tarifacion por Pulsos
      
      // B- Congelo los pulsos, xq a partir de ahora ya cuentan como pulsos de reloj
      // Todos los pulsos que entraron mientras tarifaba por tiempo no los tengo que tener en cuenta, son solo
      // para la distancia recorrida, pero no para la tarifación. Por eso, como ahora deje de tarifar por tiempo,
      // comienzo a contar los pulsos desde el valor actual.
      //
      // EDIT 19/04/2013
      //  Lo de arriba esta MAL: Si comienzo a contar los pulsos desde el valor actual => Pierdo la "historia" de
      // los pulsos. En caso de que avance, frene, avance, frene... Si comienzo desde el valor actual los avances
      // intermedios los pierdo
      // => Para mantener la historia, a los pulsos tarifacion, le debo sumar los pulsos que no fueron tenidos en
      // cuenta para la tarifacion, es decir, los q se hicieron por debajo de la Vt. Esto es la diferencia entre el
      // valor actual y pulsos_whenIniTiempo. Graficamente:
      //
      // PUL TARIFACION       3000 -> Cuando arranca la tarifacion por pulsos, esta en 3000 el contador
      // PUL REALES           3200 -> Actualmente el contador esta en 3200 => tengo 200 pulsos de diferencia
      //
      // Al comenzar a tarifar por debajo de Vt, guardo ese valor
      // pulsos_whenIniTiempo 3200
      // PUL REALES           3500 -> Recorro 300 pulsos mas por debajo de Vt
      //
      // Al volver a cobrar por tiempo, le sumo a PUL TARIFACION la diferencia entre PUL REALES y pulsos_whenIniTiempo
      //
      // PUL TARIFACION       3000 + (3500 - 3200) = 3300
      // PUL REALES           3500 -> Mantengo la diferencia anterior (la "historia") de 200 pulsos
      //
      //
      // EDIT 19/04/2013
      //  Ademas de eso, para el caso con acumulador de pulsos (VISOR COMPACT) no discriminaba REMIS de TAXI
      if (SERVICIO_TAXI){
        // Para los TAXIS, no debo contar los pulsos que recorri por debajo de la Vt (es decir mientras tarifaba
        // por tiempo); por eso se los sumo.
        pulsosTarifacion += getPulsos_whileTarifandoTiempo();
      }
      
      // C- Como RECIEN estoy por encima de la VelTransicion, cambiar el valor de la misma, por el tema de la histeresis
      VELOCIDAD_TRANSICION -= HISTERESIS_VT;      // Cambio valor de la velocidad de transicion
      
      // D- Chequear si esta programado para acumular espera para convertir TIEMPO --> PULSOS
      // *E*- [NO VALIDO] Si es REMIS no convierte TIEMPO --> PULSOS
      // F- Convertir TIEMPO --> PULSOS, si dio "positivo" alguno de los chequeos
      //    El tiempo que no completo una ficha por tiempo es convertido a pulsos de tarifacion
      //
      // EDIT 10/12/12
      //  Para REMIS, la ACUMULACION DE ESPERA, no tiene que ver con la conversion TIEMPO -> PULSOS, sino con
      // no resetear el contador de tiempo
      if (SERVICIO_TAXI){
        if (NoAcumularEspera){
          // Taxi SIN Espera Acumulativa => NO Convierte TIEMPO -> PULSOS
          segTarifacion_to_pulsos = 0;
        }else{
          // Taxi con Espera Acumulativa => Convierte TIEMPO -> PULSOS
          segTarifacion_to_pulsos = RELOJ_convertirTiempo_to_Pulsos(segundosEspera - segundosTarifacion);
        }
      }else{
        // SERVICIO DE REMIS => La acumulacion de espera no tiene que ver con la conversion tiempo a 
        // pulsos, sino con no resetear el contador de espera
        segTarifacion_to_pulsos = 0;
      }
      
      // G- Dejar de Indicar Tarifando por Tiempo
      detenerIndicacionTarifaTiempo();            // Detener Toggle Indicacion Tarifando por Tiempo
      //actualizarTarifaTiempo;                     // Actualizar Indicacion Tarifando por Tiempo
      //ocultarTarifandoTiempo;                     // Ocultar Indicacion Tarifando por Tiempo
    }
  
  /* TARIFACION POR PULSOS */
  /*************************/
    static void RELOJ_tarifarPulsos (void){
      // Rutina que se llama desde el LOOP y solo tiene sentido cuando el reloj esta en OCUPADO
      // Ya tengo los pulsos congelados cuando apenas supere la velocidad de transicion, es decir que
      // comence a tarifar por pulsos.
      // Luego, desde el LOOP y cada tanto (timer) me voy fijando la diferencia entre estos pulsos y el
      // valor actual del acumulador de pulsos.
      // Si la diferencia supera el valor de una ficha (en pulsos); sumo una ficha por Pulsos y al
      // valor que tenia congelado le sumo la cantidad de pulsos equivalente a la ficha que acabo de
      // contar
      uint32_t dif_pulsos;
      uint32_t pulse_accum;
      
      if (RELOJ_OCUPADO && timerCHK_pulsosTarifacion_F && tarifandoPulsos){
        timerCHK_pulsosTarifacion_F = 0;                  // Bajo Bandera
        
        // La mejor forma de trabajar con el tiempo convertido a pulsos, es restarselo a la variable pulsosTarifacion;
        // ya que al restarle este valor, a la hora de calcular la diferencia con el acumulador de pulsos, la misma
        // va a ser mas grande (porque pulsosTarifacion es mas chico).
        // Pero esto no siempre puede hacerse, ya que si pulsosTarifacion es mas chico que el tiempo convertido a 
        // pulsos, la resta daría la vuelta (pasaría  0xFFFFxxx) y por lo tanto la diferencia sería avismal.
        // Entonces voy a trabajar el tiempo convertido de 2 formas:
        //  1) Si lo puedo restar (pulsosTarifacion es mayor o igual) lo resto, y me olvido de eso; funciona perfectamente
        //  2) Si no lo puedo restar lo sumo luego de calcular la diferencia, es decir, agrando la diferencia; hasta que
        //     la diferencia sea lo suficientemente grande como para igualar o superar una ficha.
        //
        // El problema de (2) es que si no alcanzo una ficha, y vuelve a tarifar por tiempo; el tiempo antes convertido
        // se pierde por completo
        if (pulsosTarifacion >= segTarifacion_to_pulsos){
          // Opcion (1) del manejo del tiempo convertido a pulsos
          pulsosTarifacion -= segTarifacion_to_pulsos;
          segTarifacion_to_pulsos = 0;                    // Como logré usarlo, lo reseteo
        }
        
         pulse_accum = __HAL_TIM_GET_COUNTER(&pulsoACCUM);
        
        
        // EDIT 10/07/2012
        // La comparacion debe ser con MAYOR e IGUAL, xq sino cuando son iguales realiza la otra operacion y la cuenta
        // se va al carajo!
        //    dif_pulsos = 0xFFFFFFFF - AAAAA + AAAAA = 0xFFFFFFFF
        if (pulse_accum >= pulsosTarifacion){
          dif_pulsos = pulse_accum - pulsosTarifacion;    // Calculo diferencia de Pulsos
        }else{
          dif_pulsos = (0xFFFFFFFF - pulsosTarifacion) + pulse_accum;    // Calculo diferencia de Pulsos
        }
        
        // Opcion (2) del manejo del tiempo convertido a pulsos. Siempre lo sumo, total, si no hay nada por sumar, el valor
        // es cero.
        dif_pulsos += segTarifacion_to_pulsos;            // A la diferencia de pulsos, le agrego el tiempo convertido a pulsos
        
        // Solo proceso los pulsos si pase la distancia inicial (bajada bandera). Mientras tanto, me 
        // voy fijando si la supere
        //
        // EDIT 13/06/2012
        // Una vez superadd el valor de la ficha correspondiente, debo restar los pulsos de la conversion
        // de tiempo, al valor de pulsosTarifacion. Esto es asi porque:
        // (asumiendo que trabajamos como antes, sin restar)
        // 
        // diff_pulsos = 1150
        // segTarifacion_to_pulsos = 100
        //
        // => pulsos totales = 1250 si 1250 es la CANTIDAD_PULSOS_BAJADA => se cumple
        // la condición de que paso la bajada de bandera y:
        //
        // pulsosTarifacion = CANTIDAD_PULSOS_BAJADA = 1250
        //
        // Y aca hay un error, xq pulsosTarifacion > diff_pulsos (1250 > 1150) => si la ficha de
        // distancia cae cada 500 pulsos, la misma va a caer cuando haya una diferencia de 500
        // entre diff_pulsos y pulsosTarifacion. Como pulsosTarifacion es 100 pulsos menor, para que
        // haya una diferencia de 500 pulsos, deberan caer (100 + 500). Esto es lo que hace que
        // se deban restar los pulsos de la conversion de tiempo, de modo que cuando cae la ficha
        // pulsosTarifacion ~ diff_pulsos (el aproximado es por si dif_pulsos > CANTIDAD_PULSOS_XX)
        //
        // EDIT 26/06/2012
        // Al superar la DISTANCIA_INICIAL (bajada bandera), no debo hacer que los "pulsosTarifacion"
        // sean iguales a CANTIDAD_PULSOS_BAJADA, xq pierdo todo el "offset" que tenia de los pulsos.
        // Un poco mas explicado: pulsosTarifacion, al superar la Vel Transicion, se iguala al valor
        // actual del contador de pulsos. Este contador, por su naturaleza, NO SE RESETEA NUNCA.
        // Ahora supongamos que antes de pasar a OCUPADO, recorrio XXXXX mts en LIBRE, esto equivaldría
        // a PPPPPPP pulsos. "Este" valor (PPPPPPP) es aproximadamente el que se carga en pulsosTarifacion
        // al superar VelTrans. Si luego, al recorrer la distancia de la bajada de bandera, hago que
        // pulsosTarifacion = CANTIDAD_PULSOS_BAJADA => "Pierdo" todos los otros pulsos, y las fichas
        // van a empezar a caer como laaaaaaco. Es por eso que en lugar de igualarlo a la cantidad de
        // pulsos equivalente a la bajada de bandera, se los sumo.
        //
        // EDIT 31/10/12 HALOWEEN
        //  Al cumplirse la bajada de bandera, tengo una nueva ficha por pulsos
        if (bajadaBandera_done == 0){
          // Todavia no supere la bajada de bandera => Me fijo si lo hice
          if (dif_pulsos >= CANTIDAD_PULSOS_BAJADA){
            // Se cumplio la bajada de bandera por PULSOS (distancia inicial)
            bajadaBandera_byPulsos = 1;                   // La bajada de bandera se cumplio por pulsos
            tarifacion_actValorViaje = 1;                 // Como tengo nueva ficha, actualizo valor viaje

            if (pulse_accum < pulsosTarifacion){
            	pulsosTarifacion = (pulsosTarifacion + CANTIDAD_PULSOS_BAJADA)- 0xFFFFFFFF;
            }else{
            	pulsosTarifacion += CANTIDAD_PULSOS_BAJADA;   // Avanzo los pulsos de tarifacion en la cantidad equivalente a la Distancia Inicial
            }
            pulsosTarifacion -= segTarifacion_to_pulsos;  // Debo restarle los pulsos del tiempo convertido (si es cero no resto nada)

            segTarifacion_to_pulsos = 0;                  // Ya hice uso del tiempo convertido => lo reseteo
            fichas_xPulsos++;
            guardar_fichas_xpulsos_backup(fichas_xPulsos);
            //#ifdef  _REMISES_KATE_
          #ifdef VISOR_TARIFACION_KATE
            tarifar_distancia = 1;
            #ifdef _REMISES_LOS_PINOS_
             //EL LOS PINOS LA TARIFACION ES FIJA PARA TARIFA 2
             if(TARIFA.numero ==tarifa2D){
              tarifar_distancia = 0;
             }
            #endif
          #endif
          }
          
        }else{
          // Como ya supere la bajada de bandera, proceso los pulsos para determinar si
          // ya cayo una fiha
		 if (dif_pulsos >= CANTIDAD_PULSOS_FICHA){

			while( (pulse_accum - pulsosTarifacion) >= CANTIDAD_PULSOS_FICHA){
				fichas_xPulsos++;
				pulsosTarifacion += CANTIDAD_PULSOS_FICHA;
				//if (pulse_accum > 0xffffffff){
			//		pulsosTarifacion = (pulsosTarifacion + CANTIDAD_PULSOS_FICHA)- 0xffffffff;
				//}
			}


			guardar_fichas_xpulsos_backup(fichas_xPulsos);
			//#ifdef  _REMISES_KATE_
		  #ifdef VISOR_TARIFACION_KATE
		   tarifar_distancia = 1;
		   #ifdef _REMISES_LOS_PINOS_
			 //EL LOS PINOS LA TARIFACION ES FIJA PARA TARIFA 2
			 if(TARIFA.numero ==tarifa2D){
			  tarifar_distancia = 0;
			 }
		   #endif
		  #endif
			tarifacion_actValorViaje = 1;                 // Como tengo nueva ficha, actualizo valor viaje
			pulsosTarifacion -= segTarifacion_to_pulsos;  // Debo restarle los pulsos del tiempo convertido  (si es cero no resto nada)
			segTarifacion_to_pulsos = 0;                  // Ya hice uso del tiempo convertido => lo reseteo
		}
		}
      }
    }


  /* CONVERTIR TIEMPO SOBRANTE A PULSOS */
  /**************************************/    
    static uint32_t RELOJ_convertirTiempo_to_Pulsos (uint16_t tiempo){
      // Se llama cuando deja de tarifar por tiempo y comienza a tarifar por distancia, es decir,
      // ni bien supera la velocidad de transicion
      // La conversion de TIEMPO -> PULSOS depende de si paso o no la bajada de bandera.
      uint32_t pulsos_equivalente;
      
      if (bajadaBandera_done == 0){
        // Todavia no pase la Bajada de Bandera
        //
        //                                                 CANTIDAD_PULSOS_BAJADA
        //     TIEMPO_to_PULSOS = ----------------------  x TIEMPO ESPERADO
        //                         TIEMPO_FICHA_INICIAL
        //
        pulsos_equivalente = (uint32_t)CANTIDAD_PULSOS_BAJADA * (uint32_t)tiempo;
        pulsos_equivalente /= TIEMPO_FICHA_INICIAL;
        
      }else{
        // Ya pase la Bajada de Bandera
        //
        //                         CANTIDAD DE PULSOS POR FICHA
        //     TIEMPO_to_PULSOS = ----------------------------    x   TIEMPO SOBRANTE
        //                            TIEMPO DE UNA FICHA
        //
        pulsos_equivalente = CANTIDAD_PULSOS_FICHA * tiempo;
        pulsos_equivalente /= TARIFA.tiempoFicha;
      }
      
      return(pulsos_equivalente);
    }


  /* OBTENER CANTIDAD DE PULSOS RECORRIDOS POR DEBAJO DE LA VELOCIDAD TRANSICION */
  /*******************************************************************************/
    static uint32_t getPulsos_whileTarifandoTiempo (void){
      // Esta rutina devuelve la cantidad de pulsos que ingresaron mientras tarifaba
      // por tiempo. Este valor es la diferencia entre los pulsos actuales y los
      // pulsos cuando comenzo la tarifacion por tiempo
      // Debo tener en cuenta el OVERFLOW del contador
      uint32_t pulsosActuales;
      uint32_t deltaPulsos;
      
      #ifdef ODOMETRO_ACUMULADOR_PULSOS
        pulsosActuales = freezePULSE_ACCUM();           // Obtengo valor actual de los pulsos
      #else
        //pulsosActuales = ODOMETRO_getPulsosReales();    // Obtengo valor actual de los pulsos
        pulsosActuales = __HAL_TIM_GET_COUNTER(&pulsoACCUM);
      #endif 
      
      if (pulsosActuales >= pulsos_whenIniTiempo){
        deltaPulsos = pulsosActuales - pulsos_whenIniTiempo;
      }else{
        deltaPulsos = (0xFFFFFFFF - pulsos_whenIniTiempo) + pulsosActuales;
      }
      
      return(deltaPulsos);
    }
    
    

/*********************************************************************************************/
/* TARIFACION POR TIEMPO */
/*************************/
/*
  La Tarifacion por Tiempo funciona de la siguiente manera:
  En primer lugar, solo tarifa por tiempo cuando se encuentra por debajo de la velocidad de
  transición. Y si venia de tarifar por pulsos, NO convierte los pulsos que no completaron
  una ficha en segundos.
  Luego de detectar que esta por debajo de la velocidad de transición y mientras se mantenga
  esta condición, se implementará un contador de tiempo (contador de segundos con base de 100mseg)
        [NOTA: la base de 100mseg es para tener un error de 100mseg en la tarifacion; ya que si
        tengo una base de 1seg, para contar 1seg, el error es de 1seg. Error propio de la base]
  Este contador, es un contador de timer; y dentro de la rutina de tarifacion por tiempo, cuando
  se alcance el tiempo necesario para completar una ficha, se incrementa la cantidad de fichas
  por tiempo; y se dispara la actualizacion del valor de viaje.
  
  Adicionalmente, la tarifacion por tiempo tiene 3 parametros programables:
    - TECLA ESPERA: Para habilitar/deshabilitar la espera
    
    - ESPERA ACUMULATIVA: Para que al dejar de tarifar por tiempo y empezar a tarifar por 
      distancia convierta el tiempo sobrante (que no llego a completar una ficha) en pulsos.
      De lo contrario ese tiempo se pierde
    
    - ESPERA HABILITADA: Esta asociada a la tecla espera. Si la espera está habilitada, al
      pasar por debajo de la velocidad de transición, comienza a tarifar por tiempo. Si en
      cambio no esta habilitada, no hace nada; y si uno quisiera, con la tecla espera puede
      habilitarla
*/

  /* INICIALIZAR TARIFACION POR TIEMPO */
  /*************************************/
    static void RELOJ_iniTiempo (void){
      // Como su nombre lo indica inicializa la tarifacion por tiempo. Se llama cada vez que se
      // pasa por debajo de la velocidad de transision.
      // Tambien se llama en "toggle espera" cuando se presiona la tecla espera, para comenzar/detener
      // la espera
      //
      // Las tareas a realizar son:
        //  B- No acumular espera
      //  C- Como estoy por debajo de la VelTransicion, cambiar el valor de la misma, por el tema de la histeresis
      //  D- Indicar Tarifando por Tiempo
      //  E- Inicializar la Espera
      //
      // B- No acumular espera
      // El tiempo que fue convertido a pulsos no lo tengo que tener en cuenta, por lo tanto, igualo
      // los segundosEspera con los segundosTarifacion.
      //
      // EDIT 10/12/12
      //  Para REMIS, la ACUMULACION DE ESPERA, no tiene que ver con la conversion TIEMPO -> PULSOS, sino con
      // no resetear el contador de tiempo
      //
      // EDIT 19/04/2013
      //  Cuando va por debajo de la velocidad de transicion, los pulsos siguen entrando => Estos no se
      // deben tener en cuenta a la hora de tarifar (para TAXI. Los REMISES si lo tienen en cuenta). Para
      // lograr esto, debo guardar el valor de comenzar a tarifar por tiempo. Luego al volver a tarifar
      // por distancia le "quito" esa diferencia de pulsos
      //
      //  EDIT 23/10/13
      //    Ahora, si esta con remis, y espera NO ACUMULATIVA, debo resetear el tiempo de gracia. Para hacerlo
      // vuelvo a cargar la tarifa
      if (!(SERVICIO_REMIS && !NoAcumularEspera)){
        segundosTarifacion = segundosEspera;        // Los segundos de tarifacion los voy a contar desde "cero"
        dif_espera = 0;
      }
      
      // C- Como RECIEN estoy por encima de la VelTransicion, cambiar el valor de la misma, por el tema de la histeresis
      VELOCIDAD_TRANSICION += HISTERESIS_VT;    // Cambio valor de la velocidad de transicion
      
      // D- Indicar Tarifando por Tiempo
      dispararIndicacionTarifaTiempo();           // Disparar Toggle Indicacion Tarifando por Tiempo
      //actualizarTarifaTiempo;                     // Actualizar Indicacion Tarifando por Tiempo
      //mostrarTarifandoTiempo;                     // Mostrar Indicacion Tarifando por Tiempo
      
      clearTIMER_tarifPulsos;                     // Detengo timer Check Tarifacion por Pulsos
      setTIMER_tarifTiempo;                       // Disparo timer Tarifacion por Tiempo
      
      // E- Inicializar la Espera
      RELOJ_esperaIni();                          // Inicializo la espera
      
      // F- Tiempo de Gracia (no acumulativo).
      // Si esta en la tarifa especial, el tiempo de gracia no existe
      if (SERVICIO_REMIS && NoAcumularEspera){
        
        #ifdef VISOR_TARIFACION_NORMAL
          RELOJ_setTarifa(TARIFA.numero);         // Vuelvo a cargar la tarifa, para resetear el tiempo de gracia
       
    //#ifdef _REMISES_KATE_
    #ifdef VISOR_TARIFACION_KATE
          RELOJ_setTarifa_kate(TARIFA.numero);         // Vuelvo a cargar la tarifa, para resetear el tiempo de gracia
    #endif
       
       
        #endif
        
        #ifdef VISOR_TARIFACION_ESPECIAL
          if (TARIFA.numero != nroTARIFA_ESPECIAL){
            RELOJ_setTarifa(TARIFA.numero);       // Vuelvo a cargar la tarifa, para resetear el tiempo de gracia
 
        //#ifdef _REMISES_KATE_
        #ifdef VISOR_TARIFACION_KATE
            RELOJ_setTarifa_kate(TARIFA.numero);         // Vuelvo a cargar la tarifa, para resetear el tiempo de gracia
        #endif
              
          }
        #endif
      }
      
      // Guardo el valor actual del contador de pulsos, para que cuando vuelva a tarifar por PULSOS, no
      // tenga en cuenta los pulsos que ingresan por debajo de la velocidad de transicion
      #ifdef ODOMETRO_ACUMULADOR_PULSOS
        pulsos_whenIniTiempo = freezePULSE_ACCUM();        // Valor actual del Acumulador de Pulsos
      #else  
        //pulsos_whenIniTiempo = ODOMETRO_getPulsosReales(); // Valor actual del Acumulador de Pulsos
        pulsos_whenIniTiempo = __HAL_TIM_GET_COUNTER(&pulsoACCUM);
      #endif
      
    }
  
  
  /* INICIALIZACION DE ESPERA */
  /****************************/
    static void RELOJ_esperaIni (void){
      // La inicializacion de la espera se llama cuando:
      //  - Se inicializa la tarifa (al pasar a ocupado)
      //  - Se pasa por debajo de la velocidad de transision
      //
      // La espera tiene asociada 2 variables de programacion:
      //  1. Sin Espera
      //  2. Tecla Espera
      //
      // Si esta SIN ESPERA, significa que al pasar por debajo de la velocidad
      // de transision, no cuenta fichas de tiempo.
      // Y la TECLA ESPERA es para invertir el estado ACTUAL de la espera, si+
      // contaba fichas deja de contarlas, y viceversa
      //
      // Como esto es la INICIALIZACION, no tiene en cuenta la tecla, porque la
      // misma todavia no fue pulsada.
      if (sinEspera){
        no_contar_espera = 1;           // No contar fichas de Tiempo
        
        // EDIT 14/11/12
        //  Como no esta programado para que espere => detengo indicacion de tarifando
        // por tiempo. Ademas, detengo el timer de tarifacion por tiempo
        clearTIMER_tarifTiempo;         // Detengo timer Tarifacion por Tiempo

        detenerIndicacionTarifaTiempo();// Detener Toggle Indicacion Tarifando por Tiempo
        //actualizarTarifaTiempo;         // Actualizar Indicacion Tarifando por Tiempo
        //ocultarTarifandoTiempo;         // Ocultar Indicacion Tarifando por Tiempo
      
      }else{
        no_contar_espera = 0;           // Contar fichas de Tiempo
      }
    }

uint8_t tarifando_tiempo(void){
	//devuelve 0 si no cuenta tiempo
	//devuelve 1 si esta contando tiempo
	uint8_t status;
	if(tarifandoTiempo){
		status=1;
	}else{
		status=0;
	}
	return(status);
}

  /* TOGGLE ESPERA */
  /*****************/
    void RELOJ_toggleEspera (void){
      // Toggle Espera se llama cuando se presiona la Tecla de Espera.
      // Lo que hace es invertir el estado de la espera, si estaba
      // contando deja de contar, y viceversa
      //
      // EDIT 14/11/12
      //  El toggle de la espera solo tiene sentido, si estoy tarifando por
      // tiempo, sino no hace nada
      if (tarifandoTiempo){
        if (no_contar_espera){
          // No estaba esperando => Comienzo a esperar
          RELOJ_iniTiempo();              // Inicializo cuenta de tiempo
          // "no_contar_espera" se debe modificar luego del llamado a la inicializacion
          // de la cuenta de tiempo, ya que en ella se llama a iniEspera la cual modifica
          // el valor de la variable antes mencionada
          no_contar_espera = 0;           // Empiezo a contar espera
          
          // EDIT 14/11/12
          //  Como en iniTiempo se llama a esperaIni; si esta programado SIN espera => se detuvo
          // la indicacion de traifando por tiempo. Ahora lo que hago es volver a encenderla.
          // Y ademas disparo el timer de tarifacion por tiempo
          setTIMER_tarifTiempo;                       // Disparo timer Tarifacion por Tiempo
          dispararIndicacionTarifaTiempo();           // Disparar Toggle Indicacion Tarifando por Tiempo
          
        }else{
          // Estaba esperando => Detengo la espera
          // Las acciones a realizar son similares a las que se hacen cuando se pasa
          // por encima de la velocidad de transision y se deja de contar tiempo para
          // contar pulsos
          no_contar_espera = 1;           // Detengo la cuenta de espera
          
          clearTIMER_tarifTiempo;         // Detengo timer Tarifacion por Tiempo
          detenerIndicacionTarifaTiempo();// Detener Toggle Indicacion Tarifando por Tiempo
          
        }
       // actualizarTarifaTiempo;         // Actualizar Indicacion Tarifando por Tiempo
       // ocultarTarifandoTiempo;         // Ocultar Indicacion Tarifando por Tiempo
      }
    }
  

  /* TARIFACION POR TIEMPO */
  /*************************/
    static void RELOJ_tarfifarTiempo (void){
      // Rutina que se llama desde el LOOP y solo tiene sentido cuando el reloj esta en OCUPADO
      // Con un timer voy contando el tiempo que lleva esperando; y luego, desde el LOOP y cada
      // tanto (timer) me voy fijando si la cantidad de tiempo transcurrido son suficientes para
      // completar una ficha. De ser así, sumo una ficha por Tiempo; y actualizo el valor del viaje
      //
      // Ademas, debo fijarme que NO este programado SIN ESPERA
      //
      // no_contar_espera, es una variable que esta asociada a:
      //  - Programacion SIN ESPERA
      //  - Tecla Espera (pulsada o no)
      //
      // Cuando recien pasa por debajo de la velocidad de transicion (o sea cuando deberia empezar
      // a contar por tiempo) setea el valor INICIAL de esta variable, y luego, al pulsar
      // la tecla de espera se va cambiando.
      word dif_tiempo;
      byte bajadaDONE;
      
      if (!no_contar_espera){
        if (RELOJ_OCUPADO && timerCHK_tiempoTarifacion_F && tarifandoTiempo ){
          timerCHK_tiempoTarifacion_F = 0;                  // Bajo Bandera
          
          dif_tiempo = segundosEspera - segundosTarifacion; // Calculo diferencia de Tiempo
          dif_espera = dif_tiempo;
          
          // EDIT 11/07/2012
          // Cuando funciona como REMIS, la tarifacion por TIEMPO es independiente de la tarifacion
          // por PULSOS, y la BAJADA DE BANDERA solo existe para la tarifacion por pulsos, cuando
          // tarifa por TIEMPO, las fichas caen igual, independientemente de si se supero o no la
          // bajada de bandera
          bajadaDONE = 0;                                     // Asumo que no supere la bajada de bandera
          if (SERVICIO_TAXI){
            // Solo proceso el tiempo si pase el tiempo inicial (bajada bandera). Mientras tanto, me 
            // voy fijando si lo supere
            if (bajadaBandera_done == 0){
              // Todavia no supere la bajada de bandera => Me fijo si lo hice
              if (dif_tiempo >= TIEMPO_FICHA_INICIAL){
                // Se cumplio la bajada de bandera por TIEMPO (tiempo inicial)
                bajadaBandera_byTiempo = 1;                   // La bajada de bandera se cumplio por tiempo
                tarifacion_actValorViaje = 1;                 // Como tengo nueva ficha, actualizo valor viaje
                segundosTarifacion += TIEMPO_FICHA_INICIAL;   // Avanzo los segundos de tarifacion en la cantidad equivalente al Tiempo Inicial
                // EDIT 07/10/2013
                //  No caia una ficha cuando se cumplia la bajada de bandera por tiempo
                fichas_xTiempo++;                             // Tengo una nueva ficha por Tiempo
                guardar_fichas_xtiempo_backup(fichas_xTiempo);

          //#ifdef  _REMISES_KATE_
          #ifdef VISOR_TARIFACION_KATE
               tarifar_tiempo = 1;
               #ifdef _REMISES_LOS_PINOS_
                 //EL LOS PINOS LA TARIFACION ES FIJA PARA TARIFA 2
                 if(TARIFA.numero ==tarifa2D){
                  tarifar_tiempo = 0;
                 }
               #endif
          #endif
          
              }
            }else{
              // TAXI: Ya supere la bajada de bandera
              bajadaDONE = 1;
            }
          
          }else if (SERVICIO_REMIS){
            // REMIS: Simulo que "supere" la bajada de bandera (que no existe para la tarifacion
            // por tiempo, pero de esta manera unifico funcionamiento
            bajadaDONE = 1;
          }
          
          
           
          if (bajadaDONE){
            // TAXI: Como ya supere la bajada de bandera, proceso el tiempo para determinar si
            // ya cayo una ficha
            //
            // REMIS: Simulo que "supere" la bajada de bandera (que no existe para la tarifacion
            // por tiempo, pero de esta manera unifico funcionamiento
            if (dif_tiempo >= TARIFA.tiempoFicha){
              fichas_xTiempo++;                             // Tengo una nueva ficha por Tiempo
              guardar_fichas_xtiempo_backup(fichas_xTiempo);

          		//write_backup_eeprom();
              	//read_backup_eeprom();

          //#ifdef  _REMISES_KATE_
          #ifdef VISOR_TARIFACION_KATE
              tarifar_tiempo = 1;
              #ifdef _REMISES_LOS_PINOS_
                //EL LOS PINOS LA TARIFACION ES FIJA PARA TARIFA 2
                if(TARIFA.numero ==tarifa2D){
                 tarifar_tiempo = 0;
                }
              #endif
              
          #endif
              tarifacion_actValorViaje = 1;                 // Como tengo nueva ficha, actualizo valor viaje
              segundosTarifacion += TARIFA.tiempoFicha;     // Avanzo los segundos de tarifacion en 1 ficha

	  		  uint16_t MINUTOS = segundosTarifacion/60;
	  		  if(MINUTOS > 255){MINUTOS=255;}
	  		  minutosEspera = MINUTOS;
            }
          }
        } 
      }
    }


/**************************************************************************************************************/
/* BACKUP EN RTC */
/*****************/    
  
  //#ifdef  _REMISES_KATE_
  #ifdef VISOR_TARIFACION_KATE
    #define TARIFACION_backupSize       15
  #else
    #define TARIFACION_backupSize       10
  #endif
  
  /* ENVIAR DATOS A RTC */
  /**********************/
/*
 *
6 fecha y hora
1 tarifa numero , estado reloj
1 velocidad max

2 fichas pulsos
2 fichas tiempo

4 contador_pulsos
4 contador_pulsos old
 *
 *
 */

    void guardar_tarifa_backup(tarifa){
    	//rtc_write_backup_reg((uint32_t)TARIFA_BACKUP, tarifa);
    }

    void guardar_fichas_xpulsos_backup(uint32_t fichas_pulsos){
    	//rtc_write_backup_reg((uint32_t)FICHAS_xPULSOS_BACKUP, fichas_pulsos);
    }


    void guardar_fichas_xtiempo_backup(uint32_t fichas_tiempo){
    	//rtc_write_backup_reg((uint32_t)FICHAS_xTIEMPO_BACKUP, fichas_tiempo);
    }


    uint8_t recuperar_tarifa_backup(void){
    	rtc_read_backup_reg((uint32_t)TARIFA_BACKUP);
    }

    uint32_t recuperar_fichas_xpulsos_backup(void){
    	return(rtc_read_backup_reg((uint32_t)FICHAS_xPULSOS_BACKUP));
    }

    uint32_t recuperar_fichas_xtiempo_backup(void){
    	return(rtc_read_backup_reg((uint32_t)FICHAS_xTIEMPO_BACKUP));
    }

    uint32_t recuperar_valor_viaje_backup(void){
      	//Reloj_iniTarifa(tarifa);
        if(TARIFA_PESOS){
            //valor en pesos
       	    VALOR_VIAJE  = TARIFA.bajadaBandera;                             // Bajada Bandera
            VALOR_VIAJE += (fichas_xPulsos * TARIFA.valorFicha);            // Fichas por Pulsos
            VALOR_VIAJE += (fichas_xTiempo * TARIFA.valorFichaTiempo);      // Fichas por Tiempo
        }else{
        	 if(EqPESOS_hab){
                 //valor en fichas
                   //valor en fichas
                 if(TARIFA.diaNoche==0){
           			//tarifa diurna
                   	 VALOR_VIAJE  = EqPESOS_DIURNA.bajadaBandera;
                   	 VALOR_VIAJE += (fichas_xPulsos * EqPESOS_DIURNA.valorFicha);            // Fichas por Pulsos
                   	 VALOR_VIAJE += (fichas_xTiempo * EqPESOS_DIURNA.valorFichaTiempo);      // Fichas por Tiempo
                    }else{
                   	 //tarifa Nocturna
            			VALOR_VIAJE  = EqPESOS_NOCTURNA.bajadaBandera;                             // Bajada Bandera
            			VALOR_VIAJE += (fichas_xPulsos * EqPESOS_NOCTURNA.valorFicha);            // Fichas por Pulsos
            			VALOR_VIAJE += (fichas_xTiempo * EqPESOS_NOCTURNA.valorFichaTiempo);      // Fichas por Tiempo

                    }
        	 }else{
                 //valor en pesos
            	 VALOR_VIAJE  = TARIFA.bajadaBandera * TARIFA.valorFicha;                             // Bajada Bandera
                 VALOR_VIAJE += (fichas_xPulsos * TARIFA.valorFicha);            // Fichas por Pulsos
                 VALOR_VIAJE += (fichas_xTiempo * TARIFA.valorFichaTiempo);      // Fichas por Tiempo

        	 }
        }
      }



    byte TARIFACION_sendBackupData (void){
      // Se llama desde la funcion saveBackup del RTC
      // Rutina que envia datos de TARIFACION al RTC, para backup
      byte* data_ptr;
      
      if ((!RELOJ_OCUPADO) ){
        // Si no esta OCUPADO y no esta en COBRANDO => Guardo CEROS ya que no hay importe perdido
        // (ni importe para recuperar en caso de que al desconexion sea <15seg)
        fichas_xPulsos = 0;
        fichas_xTiempo = 0;
        TARIFA.numero = 0;
      }else{
        // En cambio, si esta OCUPADO o A PAGAR, guardo la bandera de actualizacion de
        // valor de tarifacion levantada, para que actualice al re-encenderse
        tarifacion_actValorViaje = 1;
      }
      
      RTC_sendBackupData(TARIFA.numero);
      
      // EDIT 07/10/2013
      //  Si la tarifa es la especial => reemplazo el valor de de FICHA PULSOS por VALOR VIAJE
      #ifdef VISOR_TARIFACION_NORMAL
        data_ptr = (byte*) &fichas_xPulsos;            // Puntero a fichas por Tiempo
      
      #endif
      
      #ifdef VISOR_TARIFACION_ESPECIAL
        if (TARIFA.numero == nroTARIFA_ESPECIAL){
          data_ptr = (byte*) &VALOR_VIAJE;              // Puntero a valor de viaje
        }else{
          data_ptr = (byte*) &fichas_xPulsos;          // Puntero a fichas por Tiempo
        }
      #endif
      
      RTC_sendBackupData(*data_ptr++);
      RTC_sendBackupData(*data_ptr++);
      RTC_sendBackupData(*data_ptr++);
      RTC_sendBackupData(*data_ptr++);
      
      // EDIT 07/10/2013
      //  Si la tarifa es la especial => reemplazo el valor de de FICHA T por SEGUNDOS ESPERA
      data_ptr = (byte*) &fichas_xTiempo;            // Puntero a fichas por Tiempo
      
      RTC_sendBackupData(*data_ptr++);
      RTC_sendBackupData(*data_ptr++);
      RTC_sendBackupData(*data_ptr++);
      RTC_sendBackupData(*data_ptr++);

      RTC_sendBackupData(tarifacion_F);
      
      return(TARIFACION_backupSize);
    }
  
  

    void RedondeoValorViaje_3(void){
    	uint32_t aux1,aux2;

    #ifdef HACER_REDONDEO
        	aux1 = VALOR_VIAJE/1000;
        	aux2 = VALOR_VIAJE - aux1*1000;
        	if(aux2<500){
        		VALOR_VIAJE = aux1*1000;
        	}else{
        		VALOR_VIAJE = (aux1+1)*1000;
        	}
	#endif

    }

  void RedondeoValorViaje_2(void){
       	uint32_t aux1,aux2;

       #ifdef HACER_REDONDEO
           	aux1 = VALOR_VIAJE_mostrar/100;
           	aux2 = VALOR_VIAJE_mostrar - aux1*100;
           	if(aux2<500){
           		VALOR_VIAJE_mostrar = aux1*100;
           	}else{
           		VALOR_VIAJE_mostrar = (aux1+1)*100;
           	}
   	#endif

       }

/*********************************************************************************************/
/* TIMER */
/*********/

  /* TEMPORIZACION DE TARIFACION POR PULSOS */
  /******************************************/
    void timerCHK_pulsosTarifacion (void){
      // Ejecutada cada 100mseg.
      // Para no estar permanentemente en cada vuelta del LOOP chequeando los pulsos, lo hago
      // cada tanto, con la cadencia de este timer, que no se detiene nunca
      if (timerCHK_pulsosTarifacion_cnt != 0){
        timerCHK_pulsosTarifacion_cnt--;
        if (timerCHK_pulsosTarifacion_cnt == 0){
          timerCHK_pulsosTarifacion_F = 1;  // Bandera para procesar tarifacion pulsos en LOOP
          setTIMER_tarifPulsos;             // Re-Disparo timer Check Tarifacion Pulsos
        }
      }
    }


  /* TEMPORIZACION DE TARIFACION POR TIEMPO */
  /******************************************/
    void timer_TarifacionTiempo (void){
      // Ejecutada cada 100mseg.
      // Contador de segundos, para la tarifacion por tiempo. Cada vez que se cumple 1seg
      // incrementa el valor del tiempo de espera
      if ((timer_TarifacionTiempo_cnt != 0) && RELOJ_OCUPADO){
        timer_TarifacionTiempo_cnt--;
        if (timer_TarifacionTiempo_cnt == 0){
          timerCHK_tiempoTarifacion_F = 1;  // Bandera para procesar tarifacion tiempo en LOOP
          
          // EDIT 23/10/13
          //  Para cuando se trata de un REMIS, antes de contar la espera, puedo tener tiempo de gracia
          // => Primero agoto el tiempo de gracia, luego tarifo
          if (SERVICIO_TAXI){
            // Taxi: Tarifo espera normalmente
            segundosEspera++;               // Incremento tiempo de espera
          
          }else if (SERVICIO_REMIS){
            // Remis: Debo agotar el tiempo de gracia. El mismo puede ser acumulativo o no. Si no
            // es acumulativo, se resetea en ini tarifacion tiempo
            if (TARIFA.tiempoGracia == 0){
              segundosEspera++;             // Incremento tiempo de espera
            }else{
              TARIFA.tiempoGracia--;        // Decremento tiempo de gracia
            }
          }
            
          setTIMER_tarifTiempo;             // Re-Dispara timer Tarifacion Tiempo
        }
      }
    }    


//prueba
       void set_tarifPulsos (void){
    	   tarifandoPulsos = 1;
       }


       /* EXTRAER PUNTERO A TARIFA SEGUN NUMERO */
       /*****************************************/
/*
       tTARIFA* prgRELOJ_getTarifa_pointer (byte nro){

        	 uint32_t* FLASH_ptr;

        	 FLASH_ptr = (uint32_t*) (&FLASH_PROG_relojT1D) + ((FLASHsize_PRG_relojTARIFA/4) * (nro - 1));

           return((tTARIFA*)FLASH_ptr);
         }
*/

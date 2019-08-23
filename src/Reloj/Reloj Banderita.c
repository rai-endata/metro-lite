/*
 * Reloj Banderita.c
 *
 *  Created on: 12/11/2018
 *      Author: Rai
 */


/* File: <Reloj Banderita.c> */


/* INCLUDE */
/***********/
  #include "Reloj.h"
  #include "Reloj Banderita.h"
  #include "Parametros Reloj.h"
  #include "display-7seg.h"
  #include "tipo de equipo.h"

/*********************************************************************************************/
/* ESTRUCTURAS */
/***************/
  // BANDERAS DE RELOJ CON BANDERITA
  typedef union {
      byte Byte;
      struct {
        byte finMuestreo_F      :1;   // Indica que se deben procesar las muestras de banderita
        byte estable            :1;   // Indica si el reloj ya tiene un estado estable o no
        byte arranque           :1;   // Indica que ya informó el arranque
      } Bits;
  }BANDERITA_FLAGS;


  // ESTADO DE MUESTRAS
  typedef enum{
    MUESTRAS_ALTO,
    MUESTRAS_BAJO,
    MUESTRAS_MEDIO
  }tESTADO_MUESTRAS;



/*********************************************************************************************/
/* PROTOTIPOS */
/**************/
  // RUTINAS INTERNAS
  static byte BANDERITA_muestrear (void);
  static void CambioReloj_BANDERITA (tRELOJ estado_nuevo);
  static void paseLIBRE_banderita (void);
  byte RELOJ_chkDesconectado (void);

/*********************************************************************************************/
/* CONSTANTES */
/**************/
  // TABLA DE ESTDOS DE BANDERITA SEGUN MUESTRAS Y LOGICA DE BANDERITA
    static const tRELOJ_BANDERITA ESTADOS_BANDERITA[2][3]={   // [logica][estado_muestras]
    //  ALTO  -   BAJO  -  MEDIO
      OCUPADO_BANDERITA , LIBRE_BANDERITA   , COBRANDO_BANDERITA,     // OCUPADO en ALTO
      LIBRE_BANDERITA   , OCUPADO_BANDERITA , COBRANDO_BANDERITA      // LIBRE en ALTO
    };


  // TABLA DE ACTUALIZACION DE ESTADOS DE BANDERITA
  //ESTA TABLA TIENE EN CUENTA EL HECHO QUE:
  // DE LIBRE SOLO SE PUEDE PASAR A OCUPADO (si el estado nuevo me da cobrando me quedo en libre)
  // DE OCUPADO SE PUEDE PASAR A LIBRE O A COBRANDO
   //DE COBRANDO SOLO SE PUEDE PASAR A LIBRE( si el estado nuevo me da ocupado me quedo en cobrando)
    static const tRELOJ_BANDERITA ACTUALIZACION_BANDERITA[3][3]={  // [estado_estable][estado_nuevo]
      /*         ESTADO NUEVO          */
      //  LIBRE  -  OCUPADO  - A PAGAR
          LIBRE_BANDERITA  ,  OCUPADO_BANDERITA  ,  LIBRE_BANDERITA  ,  // LIBRE
          LIBRE_BANDERITA  ,  OCUPADO_BANDERITA  , COBRANDO_BANDERITA ,  // OCUPADO    /* ESTADO ESTABLE */
          LIBRE_BANDERITA  ,  COBRANDO_BANDERITA  , COBRANDO_BANDERITA    // A PAGAR
    };

    static const tRELOJ  TAB_EQUIV_RELOJ_BANDERITA[3] = {
    		LIBRE , OCUPADO  ,  COBRANDO
    };

/*********************************************************************************************/
/* VARIABLES */
/*************/
  #define cantidad_fin_muestreo       25
  #define muestreo_ALTO               ((cantidad_fin_muestreo * 8)/10)    // 80% del total
  #define muestreo_BAJO               ((cantidad_fin_muestreo * 2)/10)    // 10% del total
  #define max_cnt_muestreos           2

  static BANDERITA_FLAGS     _banderita_F;           // Banderas de Reloj con Banderita
    #define banderita_F               _banderita_F.Byte
    #define fin_muestreo_banderita_F  _banderita_F.Bits.finMuestreo_F
    #define BANDERITA_estable         _banderita_F.Bits.estable
    #define BANDERITA_arranque        _banderita_F.Bits.arranque

  static byte BANDERITA_muestras;                    // Muestras Tomadas
  static byte BANDERITA_muestras_cnt;                // Cantidad de Muestras Tomadas

  static tRELOJ_BANDERITA BANDERITA_estados[max_cnt_muestreos];// Estado de Reloj con Banderita en cada muestreo
  static byte BANDERITA_cnt_muestreos;               // Cantidad de Muestreos realizados

  static tRELOJ_BANDERITA BANDERITA_ESTADO_ESTABLE;            // Estado de Reloj con Banderita ESTABLE

  static tRELOJ BANDERITA_ESTADO_ANTERIOR;           // Estado de Reloj con Banderita ANTERIOR al actual

  static tESTADO_MUESTRAS BANDERITA_estadoMuestras;

  static byte timer_muestreo_banderita_cnt;          // Contador para el Timer de Muestreo de Banderita
    #define x10mseg_muestreo_band       4

  #define nuevoEstado_segunMuestras      (BANDERITA_estados[BANDERITA_cnt_muestreos] = ESTADOS_BANDERITA[LOGICA_BANDERITA][BANDERITA_estadoMuestras])

  //byte LOGICA_BANDERITA;                // No es un parametro de programacion, pero se deduce de el
  GPIO_PinState pinBANDERITA;
  tRELOJ ESTADO_RELOJ_EQUIV;

/*********************************************************************************************/
/* RUTINAS */
/***********/
void Ini_portBANDERITA (void){

	GPIO_InitTypeDef  GPIO_InitStruct;

        //Config. como entrada
	    BANDERITA_CLK_ENABLE();
		GPIO_InitStruct.Pin = PIN_BANDERITA;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		//GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(PUERTO_BANDERITA, &GPIO_InitStruct);
		//levanta la salida de bandera
		HAL_GPIO_WritePin(BANDERA_OUT_PORT, BANDERA_OUT_PIN, GPIO_PIN_SET);
}

  /* CHEQUEO RELOJ BANDERITA */
  /***************************/
    void check_relojBANDERITA (void){
      byte i, coinciden;
      tRELOJ_BANDERITA estado_anterior;
      byte cambiar_reloj;
      tRELOJ estadoEQUIV;

      if (RELOJ_BANDERITA && fin_muestreo_banderita_F && !RELOJ_chkDesconectado()){
        fin_muestreo_banderita_F = 0;   // Bajo bandera
        nuevoEstado_segunMuestras;      // Nuevo estado segun las muestras

        BANDERITA_cnt_muestreos++;      // Incremento cantidad de muestreos

        if (BANDERITA_cnt_muestreos >= max_cnt_muestreos){
          BANDERITA_cnt_muestreos = 0;  // Comienzo nueva secuencia de muestreos

          coinciden = 1;                // Asumo que coinciden los estados de las muestras
          for (i=0; i<max_cnt_muestreos-1; i++){
            if (BANDERITA_estados[i] != BANDERITA_estados[i+1]){
              coinciden = 0;            // No coincidieron
            }
          }

          cambiar_reloj = 0;            // Asumo que no va a haber cambio de reloj
          if (coinciden && BANDERITA_estable){

        	// Ya puedo ver posibles cambios de reloj
            estado_anterior = BANDERITA_ESTADO_ESTABLE;
            BANDERITA_ESTADO_ESTABLE = ACTUALIZACION_BANDERITA[BANDERITA_ESTADO_ESTABLE][BANDERITA_estados[0]];
            if (BANDERITA_ESTADO_ESTABLE != estado_anterior){
              // Como el nuevo estado es distinto del anterior, hago el cambio de reloj
              cambiar_reloj = 1;
            }

          }else if(coinciden){
            // Si coincidieron, pero el reloj todavia no estaba estable, lo estabilizo.
            // Esto se hace por unica vez al arrancar el equipo
            BANDERITA_estable = 1;
            BANDERITA_ESTADO_ESTABLE = BANDERITA_estados[0];  // Actualizo estado estable
            cambiar_reloj = 1;
          }

          if (cambiar_reloj){

        	 estadoEQUIV = TAB_EQUIV_RELOJ_BANDERITA[BANDERITA_ESTADO_ESTABLE];
            CambioReloj_BANDERITA(estadoEQUIV);  // Paso al Estado Estable
          }
        }
      }
      if (RELOJ_chkDesconectado()){
        // Al estar el RELOJ DESCONECTADO, reseteo las variables para que al conectarse
        // detecte el cambio de reloj
        BANDERITA_ESTADO_ESTABLE = 0;
        BANDERITA_ESTADO_ANTERIOR = 0;

        BANDERITA_muestras = 0;
        BANDERITA_muestras_cnt = 0;

        BANDERITA_estadoMuestras = 0;
        BANDERITA_estable = 0;
      }
    }





/*********************************************************************************************/
/* RUTINAS INTERNAS */
/********************/

  /* MUESTREAR BANDERITA */
  /***********************/
    static byte BANDERITA_muestrear (void){
      byte fin_muestreo;

      fin_muestreo = 0;                 // Asumo que no terminó el muestreo

      if(check_pressBANDERITA()){
        // Como se trata de un '1' incremento las muestras
        BANDERITA_muestras++;           // Incremento valor de las muestras
      }

      BANDERITA_muestras_cnt++;         // Incremento cantidad de muestras

      if (BANDERITA_muestras_cnt > cantidad_fin_muestreo){
        fin_muestreo = 1;               // Termine el Muestreo => Proceso Datos
        if (BANDERITA_muestras >= muestreo_ALTO){
          // MUESTREO ALTO -> La mayoría de las muestras fueron 1s
          BANDERITA_estadoMuestras = MUESTRAS_ALTO;

        }else if (BANDERITA_muestras <= muestreo_BAJO){
          // MUESTREO BAJO -> La mayoría de las muestras fueron 0s
          BANDERITA_estadoMuestras = MUESTRAS_BAJO;

        }else{
          // MUESTREO MEDIO -> No hay una cantidad destacada de 1s o 0s
          BANDERITA_estadoMuestras = MUESTRAS_MEDIO;
        }

        BANDERITA_muestras = 0;         // Reseteo muestras
        BANDERITA_muestras_cnt = 0;     // Reseteo cantidad de muestras
      }

      return(fin_muestreo);
    }



/*********************************************************************************************/
/* RUTINAS INTERNAS */
/********************/
    /* CAMBIO DE RELOJ CON BANDERITA */
    /*********************************/

    static void CambioReloj_BANDERITA (tRELOJ estado_nuevo){
      // Para los cambios de reloj con BANDERITA, uso las mismas rutinas que
      // el reloj interno, ya que internamente, se controla el importe del
      // viaje y demas datos de reloj

    if (RELOJ_BANDERITA){
        if (estado_nuevo == OCUPADO){
          // PASE A OCUPADO
    		tarifa_1_8 = TARIFA_AUTO_getNroTarifa();
    		if(tarifa_1_8 < 5){
    			tarifa_1_4 = tarifa_1_8;
    		}else{
    			tarifa_1_4 = tarifa_1_8 - 4;
    		}
    		//paseOCUPADO_BANDERITA = 1;
    		paseOCUPADO_BOTON = 1;
    		Pase_a_OCUPADO();

        }else if (estado_nuevo == COBRANDO){
          // PASE A A PAGAR ->
          Pase_a_COBRANDO();

        }else if (estado_nuevo == LIBRE){
          if (BANDERITA_ESTADO_ANTERIOR == OCUPADO){
            // Como viene de OCUPADO => Hago un pase a A PAGAR
        	  Pase_a_COBRANDO();                   // Paso a LIBRE desde OCUPADO => Tx A PAGAR
          }

          // Mas alla de que venga o no de OCUPADO, el reloj debe quedar en LIBRE
          //paseLIBRE_banderita();                    // Paso a LIBRE
          Pase_a_LIBRE();
        //CambioReloj();                              // Acciones Comunes a TODOS los cambios de Reloj
      }
      BANDERITA_ESTADO_ANTERIOR = estado_nuevo;   // Actualizo estado de reloj anterior
    }
    }


GPIO_PinState check_pressBANDERITA(void){

      	GPIO_PinState pinSTATE;


      	pinSTATE = HAL_GPIO_ReadPin(PUERTO_BANDERITA, PIN_BANDERITA);
      	if(!pinSTATE){
      		//conexion bletooth (flanco descendente)
      		pinBANDERITA = 1;

      	}else{
      		//desconexion bluetooth (flanco ascendente)

      		pinBANDERITA = 0;
      	}
      	return(pinBANDERITA);
 }

byte RELOJ_chkDesconectado (void){
	return(0);
}
/*********************************************************************************************/
/* TIMER */
/*********/

  /* TEMPORIZACION DE MUESTREO */
  /*****************************/

void timer_muestreo_banderita (void){
      // Ejecutada cada 10mseg precisos
	if (RELOJ_BANDERITA){
		if(statusDISPLAY != DISPLAY_CHK){
			timer_muestreo_banderita_cnt++;
			if (timer_muestreo_banderita_cnt >= x10mseg_muestreo_band){
			  timer_muestreo_banderita_cnt = 0;
			  fin_muestreo_banderita_F = BANDERITA_muestrear();
			}
		}
      }

}


    /* PASE A LIBRE CON BANDERITA */
      /******************************/
        static void paseLIBRE_banderita (void){
			paseLIBRE_interno_sinKM;
			ESTADO_RELOJ = LIBRE;      // Actualizo Estado de Reloj
		}

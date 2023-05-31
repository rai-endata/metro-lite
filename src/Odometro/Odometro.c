//File: <odometro.c>

/* File: <Calculo Distancia ACUMM.c> */

/*
  CALCULO DE DISTANCIA :
  ---------------------
    La constante del vehiculo relaciona los pulsos con la distancia, entonces, lo que hacemos es contar pulsos y luego
    transformarlos a distancia. Para ello tenemos un contador de 4 bytes, que nunca se resetea. Sino que cuenta y cuenta.
    El tamaño del contador nos garantiza que en un estado de reloj, nunca va a llegar a un overflow xq eso implicaria
    recorrer demasiados kilometros en el mismo estado de reloj.
    Este contador se incrementa con el ingreso de cada pulso. Junto con este contador hay otro contador que almacena
    la cantidad de pulsos anteriores, para poder hacer la diferencia de pulsos y conocer la distancia recorrida. De acuerdo
    a cual de ellos sea el mayor se hace la diferencia de pulsos. Con la diferencia de pulsos, queremos calcular la
    distancia recorrida en mts y sin decimales. Para ellos, multiplicamos la cantidad de pulsos por la cantidad de metros
    por pulso, informacion que nos brinda la constante del auto.

    Luego hay dos alternativas: que se desee la distancia recorrida entre 2 estados de reloj (para el ticket) o que se
    pretenda la distancia recorrida acumulada desde que se paso a ese estado, o sea la distancia recorrida en ese estado.
    La diferencia de estos dos calculos radica en que cada vez que hago un cambio de reloj,(que es cuando calculo la distancia
    entre estados) debo guardar la cantidad de pulsos hasta ese momento, para luego poder hacer la diferencia en el proximo
    estado. Y si ademas, estoy en OCUPADO y voy a pasar a A PAGAR, debo calcular la distancia recorrida en km, con 1 decimal
    para poder mostrarla en el Ticket Comun.


    CARRERA DE BANDERA:
      La carrera de bandera esta asociada al sensor de asiento, son lo metros que debe recorrer el vehiculo para pasar a OCUPADO
      automaticamente, estando el sensor presionado.
      Para no exigir tanto al micro, no convertimos la distancia a cada rato, sino que nos manejamos con pulsos y solo calculamos
      la distancia cuando es realmente necesario. Es por eso que la carrera de bandera tambien la medimos en pulsos. Al arrancar
      el micro, sacamos el equivalente de a cuantos pulsos equivale la cantidad de metros de carrera de bandera. Junto con el
      contador anterior de pulsos, hay otro contador para los pulsos del sensor, el mismo se incrementa con cada nuevo pulsos y
      se resetea cuando se detecta que se liberó el sensor de asiento.
      Mientras tanto, desde el LOOP y en la rutina del sensor de asiento se chequea si ya se cumplio la cantidad de pulsos
      correspondientes a la carrera de bandera, y si es asi pasa a OCUPADO.
      Esta rutina que está en el LOOP, se ejecuta segun una bandera del TIMER, cada 3 segundos, es por eso que puede tardar mas
      tiempo en pasar a ocupado. es decir, a una distancia mayor a la carrera de bandera.


    A PAGAR ---> LIBRE
      Si el vehículo se encuentra en A PAGAR, y comienza a moverse, debe pasar a LIBRE, si recorre una distancia mayor a 100mts
      que en realidad tampoco se mide en mts, sino en cantidad de pulsos, sino con otro contador que se incrementa con cada nuevo
      pulso y se resetea cuando se pasa a A PAGAR. Ademas de haber recorrido esos 100mts, tambien deben de haber pasado esos
      10segundos que hay para el paso de A PAGAR a LIBRE


15/04/2011
    Se hicieron modificaciones a esta rutina, para que no "moleste" tanto al micro. Aprovechamos el contador de pulsos de
    hardware del HCS12, para realizar justamente el conteo de pulsos y asi poder determinar la distancia recorrida.
    Al ser un contador de hardware, para incrementar la cuenta, no necesita la atecion del CPU, por lo tanto no se pierden
    pulsos, ni se domora el micro en contarlos, por mas que vayan a la velocidad que vayan.
    Para poder quitar el MULTIPLEXOR y el DIVISOR DE PULSOS, se decidió hacer una leve modificacion a la rutina de calculo
    de velocidad. Antes, por cada pulso que ingresaba, o mejor dicho, por cada 2 pulsos, hacia una medicion de frecuencia,
    luego esperaba a tener "cant_frecuencias_almacenadas" (5) frecuencias almacenadas, y con ellas 5 realizaba un promedio
    y asi determinaba el valor de la velocidad. Esto era hecho PERMANENTEMENTE. Imaginemos que la velocidad junto con el
    divisor de pulsos es tal que la frecuencia de los mismos es 200Hz esto implicaría que el micro es interrumpido 200 veces
    por segundo. Y como es de esperarse, se "achancha". Ademas de que impide el correcto funcionamiento de otras rutinas que
    tambien requieren de interrupciones.
*/

/* INCLUDES */
/************/
  //#include "- Visor Config -.h"
#include "HAL.h"
#include "stm32f0xx_hal_uart.h"
#include "odometro.h"
//#include "Param Reloj.h"
#include "Parametros Reloj.h"



 // #include "Programacion\Programacion Parametros.h"
  //#include "HCS12\TIMER\Timer Overflow.h"

 #ifdef _RELOJ_AVL_
  //#include "Calculo Velocidad ACUMM - rai.h"
 #else
  //#include "Calculo Velocidad ACUMM.h"
 #endif

 // #include "Reloj\Tarifacion Reloj.h"
  //#include "RTC\RTC.h"



/////////// DISTANCIA ////////////////
/* External variables --------------------------------------------------------*/
	//UART_HandleTypeDef huart1;
	TIM_HandleTypeDef htim2;

	uint16_t PULSOS_OVF_cnt;                  // Contador de Overflow de Pulsos
	TIM_HandleTypeDef pulsoACCUM;
	uint32_t PULSE_ACCUM_CNT;
	uint32_t DISTANCIAm;
	uint32_t DISTANCIA_100;
	uint32_t DISTANCIA_100_anterior;

	//uint16_t PULSOS_x_KM;
	//uint16_t CARRERA_BANDERA;
    uint16_t  DISTANCIAkm;
    uint32_t DISTANCIAm_OCUPADO;
    uint32_t DISTANCIAm_LIBRE;
    uint32_t frecuencia;
    uint32_t t_pulsos;
    uint32_t t_pulsos_anterior;
    uint16_t  VELOCIDAD;										  	      // Velocidad del equipo en km/h, con 2 decimales
    uint16_t  VELOCIDAD_MAX;									        // Velocidad MAXIMA del equipo en km/h, con 2 decimales
    uint16_t HISTERESIS_VT;                   // Histeresis de la Velocidad de Transicion
    uint16_t VELOCIDAD_TRANSICION;            // Velocidad de Transision en km/h, con 2 deciamles

    byte timeOut_FRECUENCIA_cnt;

  // Distancia recorrida en mts
  uint32_t pulsos_cnt_old;            // Contador de Pulsos Anterior


  static uint32_t pulsos_sensor_cnt;         // Contador de Pulsos para el Sensor de Asiento
  static uint32_t pulsos_carrera_bandera;    // Equivalente en Pulsos de la Carrera de Bandera

  static uint16_t  pulsos_APagar2Libre_cnt;   // Contador de Pulsos para pasar automaticamente de A Pagar a Libre
  static uint16_t  pulsos_APagar2Libre;       // Equivalente en Pulsos a 100mts, para pasar automaticamente de A PAGAR a LIBRE

  void DISTANCIA_resetPulsos_APagar2Libre (void);
  void DISTANCIA_resetPulsosSensor (void);
  void calcPulsos_APagar2Libre (void);
  void carreraBandera_to_Pulsos (void);
  void timeOut_FRECUENCIA (void);


//////////// VELOCIDAD    ////////////

  /*
  // VELOCIDAD FLAG
  typedef union {
        byte Byte;
        struct {
          byte do_calc_vel        :1;   // Indica que se debe calcular la velocidad, para no calcular si no hay pulsos
          byte timer_calc_vel_F   :1;   // Bandera que habilita el calculo de la velocidad
          byte dont_calc_vel      :1;   // Inhibe calculo de la velocidad y reinicia calculo de periodo de pulsos
          byte almacenado_done    :1;   // Indica que se ya se almaceno la cantidad necesaria de periodos
          byte espaciar_IRQs      :1;   // Para el calculo periodico de velocidad
        } Bits;
  }VELOCIDAD_FLAGS;
  */

  //static void VELOCIDAD_almacenarFreq (uint32_t frecuencia);
  //static byte VELOCIDAD_chkRuidoMedicion (uint32_t frecuencia_actual);
  //static uint32_t VELOCIDAD_promediarFreq (void);
  //static void VELOCIDAD_borrarFreqAlmacenadas (void);
  //static void VELOCIDAD_reiniCalculoPeridoPulsos (void);
  //static void VELOCIDAD_calculo_porPeriodo (void);
  //static void VELOCIDAD_redispararTimeOutPulsos(void);
  //static void determineVELOCIDAD_MAX (void);



  #define cant_frecuencias_almacenadas  5       // Cantidad de Frecuencias Almacenadas

/*
  static VELOCIDAD_FLAGS                  _velocidad_F;  // Banderas de Calculo de Velocidad
    #define velocidad_F                   _velocidad_F.Byte
    #define do_calculo_velocidad          _velocidad_F.Bits.do_calc_vel
    #define timer_calculo_velocidad_F     _velocidad_F.Bits.timer_calc_vel_F
    #define frecuencias_almacenadas_done  _velocidad_F.Bits.almacenado_done
    #define espaciar_IRQpulsos            _velocidad_F.Bits.espaciar_IRQs
 */

  //static uint32_t periodo_pulsos;                  // Periodo de los pulsos en mseg
  //static word  cnt_periodo_odometro;            // Contador del TIMER del Odometro para el calculo de periodo
  //static word  cnt_periodo_odometro_old;        // Contador del TIMER del Odometro para el calculo de periodo del valor anterior

  //static byte  ESTADO_PULSOS;                   // Estado para el calculo de velocidad con pulsos
  //#define PRIMER_PULSO              0x00
  //#define SEGUNDO_PULSO             0x01

  //static word  calculo_periodo_TO_cnt;          // Contador para el TimeOut de Pulsos, para el calculo de periodo
    // Tener en cuenta el valor del TIMER CALCULO VELOCIDAD, ya que eso hace q no se calcule siempre la velocidad
    // sino que sea algo periodico para reducir las IRQs del micro
  //#define x10mseg_calculo_periodo_TO_L  304   // x10mseg para el TimeOut de Calculo de Periodo LARGE
  // #define x10mseg_calculo_periodo_TO_M  208   // x10mseg para el TimeOut de Calculo de Periodo MEDIUM
  // #define x10mseg_calculo_periodo_TO_S  112   // x10mseg para el TimeOut de Calculo de Periodo SMALL

  //#define Velocidad_Minima              1000  // 10km/h x 100
  //#define VEL_sobreMinima               (VELOCIDAD >= Velocidad_Minima)


 // static byte  timer_calculo_velocidad_cnt;     // Contador del Timer que habilita el calculo de la velocidad
  //  #define x100mseg_calculo_velocidad        5 // 500mseg para el calculo de la velocidad

	//static uint32_t  FRECUENCIAS_ALMACENADAS[cant_frecuencias_almacenadas];
//	static uint32_t* ptr_FRECUENCIAS_ALMACENADAS;
//	static byte   frecuencias_almacenadas_cnt;    // Contador de periodos almacenados
//	static uint32_t  frecuencia_almacenada_anterior;
//	static word   frecuencias_filtradas_cnt;
  //  #define filtro_frecuencia_compact             2000  // Filtro para delta de 20.00km/h

	//static byte  timer_calculo_periodo_cnt;
  //  #define x100mseg_calculo_periodo      15    // x100mseg en el arranque, durante los cuales no calculo periodo, por el ruido del arranque

//	static byte no_calcular_velocidad;


  //#define ODOMETRO_backupSize       9


/*********************************************************************************************/
/* CALCULO DE DISTANCIA */
/************************/

  /* INICIALIZACION PARA EL CALCULO DE DISTANCIA */
  /***********************************************/
    void DISTANCIA_iniCalculo_PULSE_ACCUM (void){
      // Se llama en el inicio
      // Requiere Programacion. Por eso debe ser ejecutada despues de que se
      // levantan los parametros de programacion
      //PULSE_ACCUM_CNT = 0;                  // Reseteo Contador de Pulsos de HARDWARE
      PULSE_ACCUM_CNT = setPULSE_ACCUM_CNT(&pulsoACCUM,0);
      pulsos_cnt_old = 0;

      DISTANCIA_resetPulsos_APagar2Libre(); // Resetea Pulsos de A Pagar -> Libre
      DISTANCIA_resetPulsosSensor();        // Resetea Pulsos del Sensor de Asiento

      calcPulsos_APagar2Libre();            // Calcula la cantidad de pulsos equivalentes a la distancia para pasar A Pagar -> Libre
      carreraBandera_to_Pulsos();           // Calcula la cantidad de pulsos equivalentes a la carrera de Bandera
  	  //DISTANCIAm = 0;
  	  //DISTANCIA_100 = 0;

    }


  /* RESET CONTADOR PULSOS A PAGAR -> LIBRE */
  /******************************************/
    void DISTANCIA_resetPulsos_APagar2Libre (void){
      //Se llama cuando se pasa a A PAGAR, para que cuente los pulsos
      //necesarios para pasar automaticamente de A PAGAR a LIBRE
      pulsos_APagar2Libre_cnt = 0;
    }


  /* RESET CONTADOR PULSOS SENSOR */
  /************************************/
    void DISTANCIA_resetPulsosSensor (void){
      // Se llama cuando se presiona el Sensor
      pulsos_sensor_cnt = 0;
    }


  /* CHEQUEAR CARRERA BANDERA */
  /****************************/
    byte DISTANCIA_chkCarreraBandera (void){
      byte done;

      if (pulsos_sensor_cnt >= pulsos_carrera_bandera){
        done = 1;          // Se cumplio la Distancia
      }else{
        done = 0;          // No se cumplio la Distancia
      }

      return(done);
    }


  /* CHEQUEAR PASE A PAGAR TO LIBRE */
  /**********************************/
    byte DISTANCIA_chkAPagar2Libre (void){
      if (pulsos_APagar2Libre_cnt >= pulsos_APagar2Libre){
        return(1);          // Se cumplio la Distancia
      }else{
        return(0);          // No se cumplio la Distancia
      }
    }


  /* GET pulsos_cnt_old */
  /**********************/
    uint32_t DISTANCIA_getPulsosCntOld (void){
      return(pulsos_cnt_old);
    }


  /* SET pulsos_cnt_old */
  /**********************/
    void DISTANCIA_setPulsosCntOld (uint32_t value){
      pulsos_cnt_old = value;
    }



  /* PULSOS PARA PASAR DE A PAGAR A LIBRE */
  /****************************************/
    void calcPulsos_APagar2Libre (void){
      // Calcula la cantidad de pulsos que deben ingresar para pasar automaticamente de A PAGAR
      // a LIBRE, tras recorrer 100mts.
      // Como la constante esta en pulsos/km, divido la constante x10 para tener pulsos en 100mts
      // Pero a su vez, tb hay q dividir por el multiplicador (divisor) de pulsos
      pulsos_APagar2Libre = PULSOS_x_KM / 10;
    }

  /* CARRERA DE BANDERA EN PULSOS */
  /********************************/
    void carreraBandera_to_Pulsos (void){
      // Calcula a cuantos pulsos equivale la carrera de bandera.
      // La constante esta en pulsos/km y la carrera de bandera en mts => tengo que calcular
      // los pulsos por metro y multiplicarlo por la carrera de bandera
      uint16_t carrera_bandera;

      carrera_bandera = CARRERA_BANDERA;
      pulsos_carrera_bandera = (PULSOS_x_KM * carrera_bandera) / 1000;
    }



  /* CALCULAR DISTANCIA RECORRIDA */
  /********************************/
    uint16_t calculoDISTANCIA_PULSE_ACCUM (byte dist_type){
      // Tener en cuenta que esta rutina calcula la DISTANCIA recorrida por el equipo, y lo guarda
      // en la variable distancia, y la misma indica la distancia en METROS
      // El valor que devuelve es la distancia en KM con 2 decimales. Como devuelve un WORD,
      // el valor maximo es 65535, es decir 655.35 Km
      //
      // Cuando se hace un cambio de estado del reloj se llama a esta rutina pidiéndole calcular la
      // distancia entre estados. Es por esto que resetea el contador de pulsos para no tener en
      // cuenta la distancia que tiene acumulada.
      // Y el valor que devuelve es la distancia recorrida en el estado de reloj antes del cambio
      uint32_t delta_pulsos;
      uint16_t  km_estado;


      if (PULSE_ACCUM_CNT >= pulsos_cnt_old){
        delta_pulsos = PULSE_ACCUM_CNT - pulsos_cnt_old;                   // Calculo Diferencia de Pulsos
      }else{
        delta_pulsos = (0xFFFFFFFF - pulsos_cnt_old) + PULSE_ACCUM_CNT;      // Calculo Diferencia de Pulsos
      }


      /*** La DISTANCIA la trabajamos sin decimales y en mts ***/

      if(PULSOS_x_KM < 2000){
    	  //La distancia max que puede calcular sin que se desborde es de 2147 km
    	  //aqui el error de truncamiento es solo el redondeo del cociente
    	  //el limite de distanciaviene dado porque Distanciam es de 4 bytes
    	  //con lo cual tenemos ffffffffhex/2000dec = 2147 km de distancia max
    	  DISTANCIAm = (delta_pulsos * 1000)/PULSOS_x_KM;		            // Distancia Recorrida en METROS sin decimales
      }else{
    	  //aqui se mete un error adicional, el truncamiento que hay en el cociente
    	  //del denominador.
    	  //la peor situacion es para k=2009
    	  //el error para una distancia de 100 metros seria
    	  //distancia = (201*100)/(2009/10) = 100,5 metros
    	  // es decir hay un error del 0,5 % el cual va bajando
    	  //para constantes mayores
    	  DISTANCIAm = (delta_pulsos * 100)/(PULSOS_x_KM/10);		            // Distancia Recorrida en METROS sin decimales
      }

      DISTANCIA_100 = (DISTANCIAm/100)*100;

      //km_estado = DISTANCIAm/100;							//en km con un decimal

      if (dist_type == 1){
        // Se le pide que inicie un nuevo calculo de distancia, xq hubo un cambio en el estado de reloj.
        // Por lo tanto, la distancia que devuelva ahora es la distancia recorrida en el estado de reloj
        // anterior al cambio.
        // Y luego hace que el contador de pulsos anterior sea el valor actual ("reset del contador de
        // pulsos") para que el proximo calculo de distancia no tenga en cuenta la distancia recorrida
        // en el estado anterior
        km_estado = (uint16_t)(DISTANCIAm / 100);                          // Distancia Recorrida en km con 1 decimal

        pulsos_cnt_old = getPULSE_ACCUM_CNT(&pulsoACCUM);             // Guardo Pulsos para siguiente estado

      }else if (dist_type == 0){
        // NO modificar pulsos_cnt_old
        // En realidad lo que me interesa es el calculo de arriba de DISTANCIA
        km_estado = 0;                                                // Para que devuelva CERO, xq no me interesa
      }

      return(km_estado);
    }

    /* CALCULAR DISTANCIA RECORRIDA */
      /********************************/
        uint16_t cal_dist (void){
        	//calculo de distancia en metros sin decimales
          uint32_t delta_pulsos;
          uint32_t dist;


          if (PULSE_ACCUM_CNT >= pulsos_cnt_old){
            delta_pulsos = PULSE_ACCUM_CNT - pulsos_cnt_old;                   // Calculo Diferencia de Pulsos
          }else{
            delta_pulsos = (0xFFFFFFFF - pulsos_cnt_old) + PULSE_ACCUM_CNT;      // Calculo Diferencia de Pulsos
          }


          /*** La DISTANCIA la trabajamos sin decimales y en mts ***/
          dist = (delta_pulsos * 1000)/PULSOS_x_KM;		            // Distancia Recorrida en METROS sin decimales


          return(dist);
        }

  /* CALCULAR DISTANCIA SENSOR ASIENTO */
  /*************************************/
    void calcularDISTANCIA_sensorAsiento (void){
      uint32_t distancia_recorrida;
      //byte* dist_ptr;
      byte *dist_ptr;

      distancia_recorrida = (pulsos_sensor_cnt * 1000) / PULSOS_x_KM;		// Distancia Recorrida en mts, sin decimales

      dist_ptr = (byte*)&distancia_recorrida;
      dist_ptr++;
      //DISTANCIA_SENSOR_ASIENTO[0] = *dist_ptr++;
      //DISTANCIA_SENSOR_ASIENTO[1] = *dist_ptr++;
      //DISTANCIA_SENSOR_ASIENTO[2] = *dist_ptr;
    }


/*********************************************************************************************/
/* VELOCIDAD */
/************/

/*
    La velocidad se calcula midiendo el periodo de los pulsos. Para ello utilizamos el contador del FREE TIMER
    y la interrupcion de TIMER OVERFLOW, para saber cuantas vueltas dio ese contador. Para medir el periodo
    tomamos el valor de cuenta del contador antes mencionado entre 2 flancos de los pulsos.
    Cada vez que ingresa un pulso se llama a "calcular_periodo_pulsos", inicialmente, el ESTADO_PULSOS es
    PRIMER_PULSO, o sea que al ingresar el primer pulso (flanco) lo primero que se hace ese retener el contador
    de OVF que hizo el timer, es decir cuantas veces dio vuelta, e inmediatamente despues retengo el valor de
    cuenta del timer general (free timer). Esto es importante que lo haga apenas entre, para que el valor de
    cuenta del free timer sea lo mas exacto posible al momento del ingreso, xq sino pierdo tiempo haciendo otras
    cosas y por lo tanto este valor se ve modificado. Y el contador de OVF es mas critico, porque el paso del timer
    es de 83 nanoseg @ 48MHz, y por cualquier cosa que me demore, puede ocurrir un OVF mas, que no deberia ser
    incluido dentro de la medicion xq ocurrio despues de la interrupcion del pulso. Y ademas, guardo el valor del
    timer de input capture.
    Luego, si el valor de cuenta del timer general es menor al valor del input capture, es porque me demore
    en atender la interrupcion. Recordemos que el Input Capture congela el valor de cuenta del timer al recibir
    el pulso en la linea de entrada. El timer general sigue corriendo, y si cuando hago la comparacion, el valor
    de cuenta del timer general es menor al que congeló el input capture, entonces, ocurrio un OVF que SI debo tener
    en cuenta para la medicion. Entonces, el valor inicial del contador de OVF seria '1', si en cambio el valor
    de cuenta del free timer es mayor al del input capture, no ocurrio ningun OVF y el valor inicial del contador
    es nulo.

    Graficamente sería asi:

                        																		+-------- Valor del Input Capture
                            																|
                                                            V
                            ++----------------------------------++	   Este seria el word del contador del free timer
                            ||                |                 ||     en este caso, ocurrio un OVF, que debe ser tenido
                            ++----------------------------------++     en cuenta en la medicion de periodo => Valor inicial
                          0x0000                              0xFFFF   del contador es '1'
                          		 A
                          		 |
                          		 +----- Valor del free timer a la hora de atender la interrupcion




                        									 					+-------- Valor del Input Capture
                            							   				|
                                                    V
                            ++----------------------------------++	   Este seria el word del contador del free timer
                            ||                |                 ||     en este caso, no ocurrio ningun OVF. => Valor inicial
                            ++----------------------------------++     del contador es '0'
                          0x0000                          A   0xFFFF
                          		 											      |
              																			      +--------- Valor del free timer a la hora de atender la interrupcion


    Luego de inicializar el contador, ya puedo habilitar la IRQ de TIMER OVERFLOW, para comenzar a contar OVERFLOWS.
    Para ello, primero bajo la bandera de OVF, porque puede haber quedado levantada de antes y luego desenmascaro la
    interrupcion. Tras habilitar la interrupcion, vuelvo a retener el valor actual del contador del free timer, y lo
    vuelvo a comparar con el valor del input capture, porque se puede haber sucedido un overflow entre que hice la
    comparacion anterior y habilite la interrupcion, y es decir ante de habilitar la interrupcion. ese overflow no
    lo va a contar la interrupcion xq sucedió antes de la misma, pero debe ser tenido en cuenta para la medicion
    porque ocurrió despues de que ingreso el primer pulso. Entonces lo que hago es poner el contador en '1', no lo
    sumo, xq si en la comparacion anterior tb era menor, valdria '2' en vez de '1'.

    Graficamente:

    																							      +-------- Valor del Input Capture
    																							      |
																	                      |			+-- Valor del free timer a la hora de atender la interrupcion
                            											      |			|
                                                        V			V
                            ++----------------------------------++	   Aca vemos como ocurrion un overlfow que debo contar a
                            ||                |                 ||     la hora de calcular el periodo porque se sucedio despues
                            ++----------------------------------++     de que ingreso el pulso
                          0x0000                              0xFFFF
                          		 A
                          		 |
                          		 +----- Valor del free timer luego de habilitar la interrupcion de OVF


    Luego guardo el valor del input capture, para poder hacer la diferencia cuando llegue el proximo pulso, y paso al siguiente
    estado.

    Al ingresar el pulso (flanco) siguiente, el ESTADO_PULSO ya es SEGUNDO_PULSO. Al igual que antes, lo primero que se hace es
    retener el valor del contador de interrupciones y el del free timer. Luego se deshabilita la interrupcion por overflow.
    Despues hay 2 motivos por los cuales deberia decrementar el contador de overflow, cualquiera de los 2 motivos hace un unico
    decremento del contador. En primer lugar, nuevamente si el contador del free timer es menor al del input capture, tengo que
    descontar un overflow, xq sucedio despues del ingreso del pulso. El otro caso, seria que el valor del contador de
    overflow sea mayor al valor que habia retenido al ingresar <nota: ahora que lo pienso, puede ser que esto este al pedo, xq
    es lo mismo que el caso 1, pero por las dudas lo dejo... me acuerdo que era algo medio rebuscado>
    Despues comparo los valores de cuenta actual y anterior, para saber como hacer la resta para tener el periodo. Si el valor
    anterior es mayor al actual entonces tengo que descontar un overflow xq es como que "ya lo estoy contemplando" al hacer la
    resta al revez (max - anterior + actual).

    Luego al valor de cuenta obtenido al hacer la diferencia le sumo el valor equivalente a la cantidad de overflows que se
    sucedieron. Por cada overflow, le sumo 0xFFFF (un vuelta del contador) que equivale a poco mas de 5mseg, he aqui la importancia
    de los overflows, xq por uno que le este errando, cometeria un error de 5mseg en la medicion del periodo y es intolerable
    para la precision que buscamos.
    Despues de tener ya la cantidad total de cuentas entre un pulso y otro, debo convertir las cuentas a tiempo. Cada cuenta
    equivale a 250/3 nanosegundos:

                                        periodo = cuentas * 250 / 3

    Y como luego vamos a trabajar con la inversa del periodo (frecuencia) y queremos que tenga 2 decimales, dividimos el periodo
    x100:

                             periodo = (cuentas * 250 / 3) / 100 = cuentas * 2.5 / 3

    Y para no usar numero decimales, reemplazamos 2.5 = 5 / 2 => nos queda

                                         periodo = cuentas * 5 / 6

    A esta altura, en "periodo_pulsos" ya tengo el periodo de la señal en nano segundos, con 2 decimales
    Como ya tengo el valor del periodo, puedo hacer el calculo de la velocidad. Y disparo el mismo levantando la bandera
    do_calculo_velocidad.

    Ahora hago un par de cosas referentes a la aceleracion, pero eso lo explico mas adelante

    Finalmente, reseteo el estado de los pulsos.

    En conclusion, cada 2 pulsos tengo el valor de un periodo y lanzo un calculo de velocidad.


15/04/2011
    Se hicieron modificaciones a esta rutina, para que no "moleste" tanto al micro. Aprovechamos el contador de pulsos de
    hardware del HCS12, para realizar justamente el conteo de pulsos y asi poder determinar la distancia recorrida.
    Al ser un contador de hardware, para incrementar la cuenta, no necesita la atecion del CPU, por lo tanto no se pierden
    pulsos, ni se domora el micro en contarlos, por mas que vayan a la velocidad que vayan.
    Para poder quitar el MULTIPLEXOR y el DIVISOR DE PULSOS, se decidió hacer una leve modificacion a la rutina de calculo
    de velocidad. Antes, por cada pulso que ingresaba, o mejor dicho, por cada 2 pulsos, hacia una medicion de frecuencia,
    luego esperaba a tener "cant_frecuencias_almacenadas" (5) frecuencias almacenadas, y con ellas 5 realizaba un promedio
    y asi determinaba el valor de la velocidad. Esto era hecho PERMANENTEMENTE. Imaginemos que la velocidad junto con el
    divisor de pulsos es tal que la frecuencia de los mismos es 200Hz esto implicaría que el micro es interrumpido 200 veces
    por segundo. Y como es de esperarse, se "achancha". Ademas de que impide el correcto funcionamiento de otras rutinas que
    tambien requieren de interrupciones.
    Para alivianar esto, y dado que no es imprescindible conocer la velocidad en cada instante y mucho menos calcularla 100
    veces por segundo, pusimos un temporizador de calculo de velocidad <espaciar_IRQpulsos>, y modificamos la promediacion
    de frecuencias.
    Como concluimos anteriormente, requiero de 2 pulsos, para el calculo de 1 frecuencia. Entonces, hacemos los siguiente
    la PRIMERA vez, esperamos a tener "cant_frecuencias_almacenadas" (5) frecuencias almacenadas para efectuar el primer
    calculo. Luego, disparamos un timer de 2seg, e inhibimos el calculo de la velocidad, y la medicion del periodo de los
    pulsos (Input Capture); hasta que no expire dicho tiempo. Cuando expira, volvemos a esperar 2 pulsos, y determinamos el
    valor de la frecuencia. Con esta nueva frecuencia y las 4 anteriores realizamos una nueva promediacion y asi obtenemos
    un nuevo valor de velocidad.
    A este nuevo valor de frecuencia a promediar, se le aplica un filtro antes de meterlo en el "buffer de promediacion".
    Si tras los 2 segundos, la nueva frecuencia a almacenar es tal que la velocidad que determinaria la misma es 25 km/h,
    la filtramos y ahi si NO inhibimos la entrada de pulsos y esperamos otros 2 pulsos para obtener un nuevo valor de
    frecuenciay volvemos a aplicarle este mismo filtro.
    Cuando el vehiculo se detiene, que lleva su velocidad a cero, se repite el ciclo de calculo desde el principio, es decir
    que vuelvo a esperar a tener 5 frecuencias para calcular la 1er velocidad.
    Graficamente:

    |<x><x><x><x><x>---------------<x>---------------<x><x>---|
    +--------------++--------------+|+---------------+|  |
            |              |        |        |        |  +-------> Instantaneamente tomamos un nuevo valor de "velocidad" en este caso cae dentro del filtro
            |              |        |        |        +----------> Valor de velocidad fuera del filtro (error de medicion)
            |              |				|				 +-------------------> Pausa. Pulsos inhibidos
            |              |        +----------------------------> Nuevo valor de velocidad, completa los 5 para el calculo
            |              +-------------------------------------> Pausa. Pulsos inhibidos
            +----------------------------------------------------> 5 velocidades iniciales
*/





  /* INICIALIZACION PARA EL CALCULO DE VELOCIDAD */
  /***********************************************/
 /*
    void VELOCIDAD_iniCalculo (void){
      VELOCIDAD = 0;                                            // Reseteo Velocidad

      ptr_FRECUENCIAS_ALMACENADAS = FRECUENCIAS_ALMACENADAS;    // Inicializo puntero de frecuencias almacenados
      frecuencias_almacenadas_cnt = 0;
      frecuencias_almacenadas_done = 0;
      frecuencias_filtradas_cnt = 0;

      timer_calculo_periodo_cnt = x100mseg_calculo_periodo;     // Para no calcular la velocidad hasta un cierto tiempo dsps del arranque, por ruido
    }
   */

  /* CALCULO DE VELOCIDAD */
  /************************/
    /*
    void calculoVELOCIDAD_PULSE_ACCUM (void){
      // Se llama desde el LOOP
      // Si la bandera de no calcular la velocidad esta levantada => No solo no hago los calculos, sino que
      // ademas reseteo el calculo del periodo de los pulsos, para iniciar una nueva cuenta.
      // Solo entra si se habilita el calculo (cada 1seg)
      if (do_calculo_velocidad){
        do_calculo_velocidad = 0;               // Bajo Bandera

        if (no_calcular_velocidad){
          // INHIBICION DEL CALCULO DE VELOCIDAD
          no_calcular_velocidad = 0;            // Bajo bandera
          VELOCIDAD_reiniCalculoPeridoPulsos(); // Recalcular periodo de los pulsos

        }else{
          // CALCULO VELOCIDAD
          VELOCIDAD_calculo_porPeriodo();       // Hago el Calculo de la Velocidad
          determineVELOCIDAD_MAX();             // Determino Velocidad Maxima

          RELOJ_chkVelTransicion();             // Calculo de la velocidad de Transicion
        }
      }
    }

*/








/**************************************************************************************************************/
/* BACK UP ODOMETRO */
/********************/


  /* ENVIAR DATOS A RTC */
  /**********************/
  /*
    byte ODOMETRO_sendBackupData_PULSE_ACCUM (void){
      // Se llama desde la funcion saveBackup del RTC
      // Rutina que envia datos del ODOMETRO al RTC, para backup
      byte* data_ptr;
      uint32_t value;

      value = DISTANCIA_getPulsosCntOld();    // NOT
      data_ptr = &value;
      RTC_sendBackupData(*data_ptr++);
      RTC_sendBackupData(*data_ptr++);
      RTC_sendBackupData(*data_ptr++);
      RTC_sendBackupData(*data_ptr++);

      value = DISTANCIA_getPulsosCntOld();
      data_ptr = &value;
      RTC_sendBackupData(*data_ptr++);
      RTC_sendBackupData(*data_ptr++);
      RTC_sendBackupData(*data_ptr++);
      RTC_sendBackupData(*data_ptr++);

      RTC_sendBackupData(VELOCIDAD_MAX);

      return(ODOMETRO_backupSize);
    }
  */

  /* RECUPERAR DATOS DE RTC */
  /**************************/
    void ODOMETRO_getBackupData_PULSE_ACCUM (void){
      // Se llama desde la funcion getBackup del RTC
      // Rutina que extrae datos del backup en RTC, pertenecientes al ODOMETRO
   	/*
      byte* data_ptr;
      uint32_t value;

      data_ptr = (byte*) &value;        // NOT

      *data_ptr++ = RTC_getBackupData();
      *data_ptr++ = RTC_getBackupData();
      *data_ptr++ = RTC_getBackupData();
      *data_ptr++ = RTC_getBackupData();

      DISTANCIA_setPulsosCntOld(value);

      data_ptr = (byte*) &value;
      *data_ptr++ = RTC_getBackupData();
      *data_ptr++ = RTC_getBackupData();
      *data_ptr++ = RTC_getBackupData();
      *data_ptr++ = RTC_getBackupData();
*/
     // DISTANCIA_setPulsosCntOld(value);

      //VELOCIDAD_MAX = RTC_getBackupData();
    }




/*********************************************************************************************/
/* PULSOS */
/***********/

  /* INICIALIZACION DE PULSOS */
  /****************************/
    /*
    void PULSOS_ini_PULSE_ACCUM (void){

    	ddr_PULSOS_PULSE_ACCUM = 0;                   // Pulsos como entrada

      // Divisor de pulsos en 1
      ddr_lsb_DIVISOR_PULSOS_PULSE_ACCUM = 1;       // Set LSB Divisor de pulsos como salida
      ddr_msb_DIVISOR_PULSOS_PULSE_ACCUM = 1;				// Set MSB Divisor de pulsos como salida
      lsb_DIVISOR_PULSOS_PULSE_ACCUM = 1;
      msb_DIVISOR_PULSOS_PULSE_ACCUM = 1;

      // Input Capture
      TMR_ICOC_PULSOS = 0;              // Timer como input capture
      TMR_CTRLA_PULSOS = 0;             // Interrumpcion por flanco ascendente
      TMR_CTRLB_PULSOS = 1;             // Interrumpcion por flanco ascendente
      PULSOS_habIRQ();                  // Habilito IRQ Pulsos

      // Pulse Accumulator
      PULSE_ACCUM_E = 1;                // Pulse Accumulator Enabled
      PULSE_ACCUM_MODE = 0;             // Event Counter
      PULSE_ACCUM_EDGE = 0;             // Falling Edge
      PULSE_ACCUM_CNT = 0;              // Reseteo valor del contador de HARDWARE
      PULSE_ACCUM_OVF_IRQ_F = 1;        // Limpio bandera de IRQ
      PULSE_ACCUM_OVF_IRQ_E = 1;        // Habilito OVF IRQ

      PULSOS_OVF_cnt = 0;               // Reseteo contador de OVF

    }
*/

  /* RESETEAR PULSOS */
  /*******************/
    void PULSOS_reset (void){
      PULSE_ACCUM_CNT = setPULSE_ACCUM_CNT(&pulsoACCUM,0);              // Reseteo valor del contador de HARDWARE
    }





  /* CONGELAR VALOR DEL ACUMULADOR DE PULSOS */
  /*******************************************/
    uint32_t freezePULSE_ACCUM (void){
    	uint32_t  freezed;

    	//freezed = __HAL_TIM_GET_COUNTER(&htim2);;        // Recien dio la vuelta
    	freezed = getPULSE_ACCUM_CNT(&pulsoACCUM);

    	return(freezed);
    }


/*********************************************************************************************/
/* TIMER */
/*********/

    void MX_TIM2_Init(void){

      TIM_SlaveConfigTypeDef sSlaveConfig;
      TIM_MasterConfigTypeDef sMasterConfig;

      pulsoACCUM.Instance = TIM2;
      pulsoACCUM.Init.Prescaler = 0;
      pulsoACCUM.Init.CounterMode = TIM_COUNTERMODE_UP;
      pulsoACCUM.Init.Period = 0xFFFFFFFF;
      pulsoACCUM.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
      HAL_TIM_Base_Init(&pulsoACCUM);


      sSlaveConfig.SlaveMode = TIM_SLAVEMODE_EXTERNAL1;
      sSlaveConfig.InputTrigger = TIM_TS_TI1FP1;
      sSlaveConfig.TriggerPolarity = TIM_TRIGGERPOLARITY_RISING;
      sSlaveConfig.TriggerFilter = 15;
      HAL_TIM_SlaveConfigSynchronization(&pulsoACCUM, &sSlaveConfig);

      sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
      sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
      HAL_TIMEx_MasterConfigSynchronization(&pulsoACCUM, &sMasterConfig);

      HAL_TIM_Base_Start(&pulsoACCUM);
    }


    void MX_TIM2_Init1(void){

         TIM_SlaveConfigTypeDef sSlaveConfig;
         TIM_MasterConfigTypeDef sMasterConfig;
         TIM_ClockConfigTypeDef sClockSourceConfig; //agregado input capture
         TIM_IC_InitTypeDef sConfigIC;				//agregado input capture

         pulsoACCUM.Instance = TIM2;
         pulsoACCUM.Init.Prescaler = 0;
         pulsoACCUM.Init.CounterMode = TIM_COUNTERMODE_UP;
         pulsoACCUM.Init.Period = 0xFFFFFFFF;
         pulsoACCUM.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
		 if(HAL_TIM_Base_Init(&pulsoACCUM) != HAL_OK){
			_Error_Handler(__FILE__, __LINE__);
		 }else{
	    	 if (HAL_TIM_Base_Start(&pulsoACCUM) != HAL_OK){
 	            _Error_Handler(__FILE__, __LINE__);
	    	 }
		 }

         //agregado input capture
         sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
         if (HAL_TIM_ConfigClockSource(&pulsoACCUM, &sClockSourceConfig) != HAL_OK){
           _Error_Handler(__FILE__, __LINE__);
         }

		 if (HAL_TIM_IC_Init(&pulsoACCUM) != HAL_OK){
			 _Error_Handler(__FILE__, __LINE__);
	     }else{
	    	 if (HAL_TIM_IC_Start_IT(&pulsoACCUM,TIM_CHANNEL_1) != HAL_OK){
 	            _Error_Handler(__FILE__, __LINE__);
	    	 }
	     }

         sSlaveConfig.SlaveMode = TIM_SLAVEMODE_EXTERNAL1;
         sSlaveConfig.InputTrigger = TIM_TS_TI1FP1;
         sSlaveConfig.TriggerPolarity = TIM_TRIGGERPOLARITY_RISING;
         sSlaveConfig.TriggerFilter = 15;
         HAL_TIM_SlaveConfigSynchronization(&pulsoACCUM, &sSlaveConfig);

         sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
         sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
         HAL_TIMEx_MasterConfigSynchronization(&pulsoACCUM, &sMasterConfig);

         //agregado input capture
           sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
           sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
           sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
           sConfigIC.ICFilter = 15;

           if (HAL_TIM_IC_ConfigChannel(&pulsoACCUM, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
           {
             _Error_Handler(__FILE__, __LINE__);
           }


       }


    /* TIM2 init function */
void MX_TIM2_Init3(void)
    {

      TIM_ClockConfigTypeDef sClockSourceConfig;
      TIM_MasterConfigTypeDef sMasterConfig;
      TIM_IC_InitTypeDef sConfigIC;

      htim2.Instance = TIM2;
      htim2.Init.Prescaler = 48000;
      htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
      htim2.Init.Period = 100000;
      htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
      if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
      {
        _Error_Handler(__FILE__, __LINE__);
      }

      sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
      if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
      {
        _Error_Handler(__FILE__, __LINE__);
      }

      if (HAL_TIM_IC_Init(&htim2) != HAL_OK)
      {
        _Error_Handler(__FILE__, __LINE__);
      }

      sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
      sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
      if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
      {
        _Error_Handler(__FILE__, __LINE__);
      }

      sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
      sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
      sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
      sConfigIC.ICFilter = 15;
      if (HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
      {
        _Error_Handler(__FILE__, __LINE__);
      }

    }





	void MX_TIM2_Init4(void){

		TIM_SlaveConfigTypeDef sSlaveConfig;
		TIM_MasterConfigTypeDef sMasterConfig;
		TIM_IC_InitTypeDef sConfigIC;

		pulsoACCUM.Instance = TIM2;
		pulsoACCUM.Init.Prescaler = 0;
		pulsoACCUM.Init.CounterMode = TIM_COUNTERMODE_UP;
		pulsoACCUM.Init.Period = 0xFFFFFFFF;
		pulsoACCUM.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
        //inicializo base de temporizacion
		if(HAL_TIM_Base_Init(&pulsoACCUM) != HAL_OK){
			_Error_Handler(__FILE__, __LINE__);
		}else{
			//if (HAL_TIM_Base_Start(&pulsoACCUM) != HAL_OK){
		    //		_Error_Handler(__FILE__, __LINE__);
			//}
		}
        //seteo entrada de pulsos externos al contador CK_PSC
		sSlaveConfig.SlaveMode = TIM_SLAVEMODE_EXTERNAL1;
		sSlaveConfig.InputTrigger = TIM_TS_TI1FP1;
		sSlaveConfig.TriggerPolarity = TIM_TRIGGERPOLARITY_RISING;
		sSlaveConfig.TriggerFilter = 15;
		HAL_TIM_SlaveConfigSynchronization(&pulsoACCUM, &sSlaveConfig);
        //inicializo input capture
		if (HAL_TIM_IC_Init(&pulsoACCUM) != HAL_OK)
		{
			_Error_Handler(__FILE__, __LINE__);
		}

		sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
		sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
		if (HAL_TIMEx_MasterConfigSynchronization(&pulsoACCUM, &sMasterConfig) != HAL_OK)
		{
			_Error_Handler(__FILE__, __LINE__);
		}

		//inicializo flancos y filtro de input capture
		sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
		sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
		sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
		sConfigIC.ICFilter = 15;
		if (HAL_TIM_IC_ConfigChannel(&pulsoACCUM, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
		{
			_Error_Handler(__FILE__, __LINE__);
		}

		HAL_TIM_Base_Start(&pulsoACCUM);
	}


	 /* TIME OUT FRECUENCIA */
	 /**********************/

	void timeOut_FRECUENCIA (void){

		if (timeOut_FRECUENCIA_cnt != 0){
		  timeOut_FRECUENCIA_cnt--;
		if (timeOut_FRECUENCIA_cnt == 0){
			frecuencia = 0;
			VELOCIDAD = 0;
		}
	 }
	}



/*********************************************************************************************/
/* INTERRUPCIONES */
/******************/

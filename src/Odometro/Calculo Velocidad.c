/* File: <Calculo Velocidad.c> */

/*
  CALCULO DE VELOCIDAD :
  ---------------------
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
    hardware del HCS12, para realizar justamente el conteo de pulsos y así poder determinar la distancia recorrida.
    Al ser un contador de hardware, para incrementar la cuenta, no necesita la atención del CPU, por lo tanto no se pierden
    pulsos, ni se demora el micro en contarlos, por mas que vayan a la velocidad que vayan.
    Para poder quitar el MULTIPLEXOR y el DIVISOR DE PULSOS, se decidió hacer una leve modificación a la rutina de calculo
    de velocidad. Antes, por cada pulso que ingresaba, o mejor dicho, por cada 2 pulsos, hacia una medición de frecuencia,
    luego esperaba a tener "cant_frecuencias_almacenadas" (5) frecuencias almacenadas, y con ellas 5 realizaba un promedio
    y así determinaba el valor de la velocidad. Esto era hecho PERMANENTEMENTE. Imaginemos que la velocidad junto con el
    divisor de pulsos es tal que la frecuencia de los mismos es 200Hz esto implicaría que el micro es interrumpido 200 veces
    por segundo. Y como es de esperarse, se "achancha". Además de que impide el correcto funcionamiento de otras rutinas que
    también requieren de interrupciones.
    Para alivianar esto, y dado que no es imprescindible conocer la velocidad en cada instante y mucho menos calcularla 100
    veces por segundo, pusimos un temporizador de calculo de velocidad <espaciar_IRQpulsos>, y modificamos la promediacion
    de frecuencias.
    Como concluimos anteriormente, requiero de 2 pulsos, para el calculo de 1 frecuencia. Entonces, hacemos los siguiente
    la PRIMERA vez, esperamos a tener "cant_frecuencias_almacenadas" (5) frecuencias almacenadas para efectuar el primer
    calculo. Luego, disparamos un timer de 2seg, e inhibimos el calculo de la velocidad, y la medicion del periodo de los
    pulsos (Input Capture); hasta que no expire dicho tiempo. Cuando expira, volvemos a esperar 2 pulsos, y determinamos el
    valor de la frecuencia. Con esta nueva frecuencia y las 4 anteriores realizamos una nueva promediacion y asi obtenemos 
    un nuevo valor de velocidad.
    A este nuevo valor de frecuencia a promediar, se le aplica un filtro antes de meterlo en el "buffer de promediación".
    Si tras los 2 segundos, la nueva frecuencia a almacenar es tal que la velocidad que determinaría la misma es 25 km/h,
    la filtramos y ahí si NO inhibimos la entrada de pulsos y esperamos otros 2 pulsos para obtener un nuevo valor de
    frecuencia y volvemos a aplicarle este mismo filtro.
    Cuando el vehículo se detiene, que lleva su velocidad a cero, se repite el ciclo de calculo desde el principio, es decir
    que vuelvo a esperar a tener 5 frecuencias para calcular la 1er velocidad.
    Gráficamente:
    
    |<x><x><x><x><x>---------------<x>---------------<x><x>---|
    +--------------++--------------+|+---------------+|  |
            |              |        |        |        |  +-------> Instantáneamente tomamos un nuevo valor de "velocidad" en este caso cae dentro del filtro
            |              |        |        |        +----------> Valor de velocidad fuera del filtro (error de medición)
            |              |				|				 +-------------------> Pausa. Pulsos inhibidos
            |              |        +----------------------------> Nuevo valor de velocidad, completa los 5 para el calculo
            |              +-------------------------------------> Pausa. Pulsos inhibidos
            +----------------------------------------------------> 5 velocidades iniciales
*/  

/* INCLUDES */
/************/
/*
    #include "Calculo Velocidad.h"
	#include "Odometro.h"
	#include "Parametros Reloj.h"
	#include "Tarifacion Reloj.h"
*/

/*********************************************************************************************/
/* VARIABLES */
/*************/



/*********************************************************************************************/
/* ESTRUCTURAS */
/***************/
/*

void VELOCIDAD_redispararTimeOutPulsos(void);

// RUTINAS INTERNAS
  void VELOCIDAD_almacenarFreq (dword frecuencia);
   byte VELOCIDAD_chkRuidoMedicion (dword frecuencia_actual);
   dword VELOCIDAD_promediarFreq (void);

   void VELOCIDAD_borrarFreqAlmacenadas (void);
   void VELOCIDAD_reiniCalculoPeridoPulsos (void);
   void VELOCIDAD_calculo_porPeriodo (void);
   void determineVELOCIDAD_MAX (void);

   static void filtro_frec (void);
*/



/*********************************************************************************************/
/* VARIABLES */
/*************/
/*
   static word  calculo_periodo_TO_cnt;          // Contador para el TimeOut de Pulsos, para el calculo de periodo
    word  VELOCIDAD;										  	      // Velocidad del equipo en km/h, con 2 decimales
    word  VELOCIDAD_NEW;
    word  VELOCIDAD_MAX;									        // Velocidad MAXIMA del equipo en km/h, con 2 decimales
    word  VELOCIDAD_MAX_NEW;


    #define cant_frecuencias_almacenadas  2       // Cantidad de Frecuencias Almacenadas
  
	static word   frecuencias_filtradas_cnt;
	 // #define filtro_frecuencia             555   // Filtro para delta de 20km/h => filtro = vel*1000/3600
	  #define max_frecuencias_filtradas     5     // Cantidad de filtraciones CONSECUTIVAS para determinar q no es ruido y debo dejar de filtrar


	static dword periodo_pulsos;                  // Periodo de los pulsos en mseg
    static byte cnt_mseg;
    static dword n_pulsos_anterior;
    static dword n_pulsos_aux;

    static dword pulsosRLJ_anterior;
    static dword pulsosRLJ;


	static byte  ESTADO_PULSOS;                   // Estado para el calculo de velocidad con pulsos
		#define PRIMER_PULSO              0x00
		#define SEGUNDO_PULSO             0x01

	// Tener en cuenta el valor del TIMER CALCULO VELOCIDAD, ya que eso hace q no se calcule siempre la velocidad
    // sino que sea algo periodico para reducir las IRQs del micro
    #define x10mseg_calculo_periodo_TO_L  304   // x10mseg para el TimeOut de Calculo de Periodo LARGE
    #define x10mseg_calculo_periodo_TO_M  208   // x10mseg para el TimeOut de Calculo de Periodo MEDIUM
    #define x10mseg_calculo_periodo_TO_S  112   // x10mseg para el TimeOut de Calculo de Periodo SMALL
  
  #define Velocidad_Minima              1000  // 10km/h x 100
  #define VEL_sobreMinima               (VELOCIDAD >= Velocidad_Minima)
  
  dword filtro;
  word pulsos_cnt;

	static dword  FRECUENCIAS_ALMACENADAS[cant_frecuencias_almacenadas];
	static dword* ptr_FRECUENCIAS_ALMACENADAS;
	static byte   frecuencias_almacenadas_cnt;    // Contador de periodos almacenados
	static dword  frecuencia_almacenada_anterior;
	static word   frecuencias_filtradas_cnt;
	  //#define filtro_frecuencia             2500  // Filtro para delta de 25.00km/h
	  #define filtro_frecuencia             1000000  // Filtro para delta de 25.00km/h
	
	#define x100mseg_calculo_periodo      15    // x100mseg en el arranque, durante los cuales no calculo periodo, por el ruido del arranque

	static byte  timer_calculo_periodo_cnt = x100mseg_calculo_periodo;
	
*/


/*********************************************************************************************/
/* RUTINAS */
/***********/

/* CALCULA Y ALMACENA FRECUENCIA DE PULSOS */
/*******************************************/

  /* INICIALIZACION PARA EL CALCULO DE VELOCIDAD */
  /***********************************************/
/*
 void VELOCIDAD_iniCalculo (void){
	  VELOCIDAD = 0;                                            // Reseteo Velocidad

	  ptr_FRECUENCIAS_ALMACENADAS = FRECUENCIAS_ALMACENADAS;    // Inicializo puntero de frecuencias almacenados
	  frecuencias_almacenadas_cnt = 0;
	  frecuencias_almacenadas_done = 0;
	  frecuencias_filtradas_cnt = 0;

	  do_calculo_velocidad = 0;       							// No calcular velocidad

	  n_pulsos_anterior  = __HAL_TIM_GET_COUNTER(&pulsoACCUM);
	  pulsosRLJ_anterior = n_pulsos_anterior;

	  cnt_mseg=0;
	  filtro_frec();

	}
*/
	//dword n_pulsos_anterior;
/*
	dword pulsos_x_seg;
	dword n_pulsos;
	void cal_freq_pulsos(void){
	    //se llama a esta rutina cada 100 mseg;



		if (timer_calculo_periodo_cnt == 0){
		// El timer es para que empiece a calcular los periodos un cierto tiempo después del arranque, por
		// el ruido y eso. Sino tira cualquiera como velocidad máxima
			pulsosRLJ = __HAL_TIM_GET_COUNTER(&pulsoACCUM);
			n_pulsos_aux  = pulsosRLJ;
			if(n_pulsos_aux >= n_pulsos_anterior){
				n_pulsos = n_pulsos_aux-n_pulsos_anterior;
			}else{
				n_pulsos = (0xffffffff-n_pulsos_anterior) + n_pulsos_aux;
			}

			n_pulsos_anterior = n_pulsos_aux;

			if(pulsosRLJ != pulsosRLJ_anterior){
				if(yaLLEGO_unPULSO){
				//solo considero pulsos mayores a 2 Hz
					if(cnt_mseg >0 ){
						//tiempo entre dos pulsos 100*cnt_mseg (mseg)
						pulsos_x_seg = n_pulsos*10000;    			//convierto a pulsos x segundo (n_pulsos/100 mseg)
						pulsos_x_seg = 100*pulsos_x_seg; 		//le agrego dos decimales
						pulsos_x_seg = pulsos_x_seg/cnt_mseg;   //100*cnt_mseg = tiempo entre dos pulsos
						VELOCIDAD_almacenarFreq(pulsos_x_seg);	//almaceno frecuencia
						pulsosRLJ_anterior = pulsosRLJ;
						cnt_mseg=0;
					}else{
						//tiempo entre dos pulsos 1 seg)
						//pulsos_x_seg = n_pulsos*1;    			//convierto a pulsos x segundo (n_pulsos/1seg)
						pulsos_x_seg = n_pulsos*10000;    			//convierto a pulsos x segundo (n_pulsos/100 mseg)
						pulsos_x_seg = 100*pulsos_x_seg; 		//le agrego dos decimales
						VELOCIDAD_almacenarFreq(pulsos_x_seg);	//almaceno frecuencia
						pulsosRLJ_anterior = pulsosRLJ;
					}
					do_calculo_velocidad = 1;                   // Habilito Calculo de Velocidad
				}else{
					//filtro el primer pulso para que no me desvie la muestra, ya que el cnt_mseg esta
					//contando de 0 a 20, mientras esta quieto y el primer pulso puede ingresar en cualquier
					//momento de esta cuenta, asi descarto este error inicial, comenzando la cuenta a partir del segundo pulso.
					cnt_mseg=0;
					yaLLEGO_unPULSO=1;
					pulsosRLJ_anterior = pulsosRLJ;
					pulsos_cnt=0;
				}
			}else {
				cnt_mseg++;

				if(cnt_mseg > 1 ){
					//Si pasa por aca es porque todavia no se movio
					//O se detuvo
					//O la frecuencia de pulsos es menor a 0.5 Hz
					cnt_mseg=0;
					yaLLEGO_unPULSO=0;
					//VELOCIDAD_borrarFreqAlmacenadas();

					for(char i=0;i<cant_frecuencias_almacenadas;i++){
						//pongo en cero la velocidad
						VELOCIDAD_almacenarFreq(0);
					}
				}
			}
		}
	}

  */

  /* CALCULO DE VELOCIDAD */
  /************************/
 /*
 void calculoVELOCIDAD (void){
	// Se llama desde el LOOP
	// Si la bandera de no calcular la velocidad esta levantada => No solo no hago los calculos, sino que
	// ademas reseteo el calculo del periodo de los pulsos, para iniciar una nueva cuenta.
	// Solo entra si se habilita el calculo (cada 1seg)

		if (do_calculo_velocidad){
			do_calculo_velocidad = 0;               // Bajo Bandera
			// CALCULO VELOCIDAD
			VELOCIDAD_calculo_porPeriodo();       // Hago el Calculo de la Velocidad
			determineVELOCIDAD_MAX();             // Determino Velocidad Maxima
			RELOJ_chkVelTransicion();             // Calculo de la velocidad de Transicion
		}

    }    
*/

 /* INHIBIR CALCULO VELOCIDAD */
  /*****************************/
 /*
 void VELOCIDAD_inhibirCalculo (void){
      // Se llama al ingresar a CUALQUIER IRQ, ya que eso me bloquea las IRQs de PULSOS:
      //  - IRQ MODEM [IRQ]
      //  - IRQ GPS <Tx y Rx>
      //  - IRQ EEPROM
      //  - IRQ BUZZER
      //  - IRQ PTT (port H)
      // para que no calcule la velocidad luego de salir de la IRQ, ya que como se comió las IRQ de TIMER OVERFLOW,
      // va a dar erroneo el calculo => Inhibo el calculo, y con esa misma bandera, pero desde el LOOP [si pasa por
      // el LOOP es xq salio de la IRQ de grabacion] vuelvo a habilitar el calculo, reiniciando el calculo del 
      // periodo de los pulsos
      
      no_calcular_velocidad = 1;
      
    }    
*/
/*********************************************************************************************/
/* RUTINAS INTERNAS */
/********************/  
  
  /* ALMACENAR FRECUENCIAS */
  /*************************/
 /*
 void VELOCIDAD_almacenarFreq (dword frecuencia){
      byte almacenar;
      
      if(frecuencias_almacenadas_done){
        // Solo aplico el filtro, si ya tengo el buffer lleno, xq sino tengo problemas para realizar los calculos
        // al iniciar la marcha y de paso me ayuda para el calculo de la aceleracion brusca en el arranque, xq sino
        // el promedio amortigua la aceleracion
        almacenar = VELOCIDAD_chkRuidoMedicion(frecuencia);
      
      }else{
        //Si no esta lleno el buffer, siempre almaceno
        almacenar = 1;
      }
      //PRUEBA SACO FILTRO
      almacenar = 1;
      
      if (almacenar){
        *ptr_FRECUENCIAS_ALMACENADAS = frecuencia;                      // Guardo Frecuencia Actual
        frecuencia_almacenada_anterior = frecuencia;                      // Guardo ultimo frecuencia almacenada, para luego hacer el chequeo de ruido

        ptr_FRECUENCIAS_ALMACENADAS++;
        if (ptr_FRECUENCIAS_ALMACENADAS == (FRECUENCIAS_ALMACENADAS + cant_frecuencias_almacenadas)){
          ptr_FRECUENCIAS_ALMACENADAS = FRECUENCIAS_ALMACENADAS;          // Doy la vuelta
        }
        
        frecuencias_almacenadas_cnt++;                                    // Incremento cantidad de Frecuencias almacenadas
        if (frecuencias_almacenadas_cnt == cant_frecuencias_almacenadas){
          frecuencias_almacenadas_cnt = cant_frecuencias_almacenadas - 1; // Vuelvo a contador para sabes cuando llegue al fin del buffer
          frecuencias_almacenadas_done = 1;                               // Ya puedo calcular la velocidad xq tengo la cantidad suficiente de muestras
          
          do_calculo_velocidad = 1;                                       // Habilito calculo de velocidad en LOOP

          
        }
      }
       }

*/
  
  /* CHEQUEAR RUIDO EN MEDICION DE FRECUENCIAS */
  /*********************************************/
 /*
 byte VELOCIDAD_chkRuidoMedicion (dword frecuencia_actual){
      // Filtracion de Frecuencias
      dword delta_frecuencia;
      

      // Calculo la diferencia en frecuencia entre el valor actual y el ultimo almacenado
      if (frecuencia_actual >= frecuencia_almacenada_anterior){
        delta_frecuencia = frecuencia_actual - frecuencia_almacenada_anterior; 

      }else{
        delta_frecuencia = frecuencia_almacenada_anterior - frecuencia_actual; 
      }
      
      if (delta_frecuencia > filtro){
        frecuencias_filtradas_cnt++;
        frecuencia_almacenada_anterior = frecuencia_actual; //para que no quede clavado(RAI)
        return(0);                    // No almacenar frecuencia xq esta fuera del filtro
        
      }else{
        return(1);                    // Si almacenar frecuencia xq esta dentro del filtro
      }
    }
*/

  /* PROMEDIAR FRECUENCIAS */
  /*************************/  
 /*
 dword VELOCIDAD_promediarFreq (void){
      byte i;
      dword suma;
      dword frecuencia_promediada;
      
      suma = 0;
      if(frecuencias_almacenadas_done){
        // Ya tengo el buffer lleno => Sumo el TOTAL de muestras y Divido por el TOTAL del buffer
        for (i=0; i<cant_frecuencias_almacenadas; i++){
          suma += FRECUENCIAS_ALMACENADAS[i];
        }
        frecuencia_promediada = (suma)/cant_frecuencias_almacenadas;
      
      }else{
        // Todavia no tengo el buffer lleno => Sumo N muestras y Divido por ese N
        for (i=0; i<frecuencias_almacenadas_cnt; i++){
          suma += FRECUENCIAS_ALMACENADAS[i];
        }
        
        // Comparo con CERO, para no DIVIDIR POR CERO
        if (frecuencias_almacenadas_cnt != 0){
          frecuencia_promediada = (suma)/frecuencias_almacenadas_cnt;
        }else{
          frecuencia_promediada = 0;
        }
      }
      return(frecuencia_promediada);
    }
  */
   
  /* BORRAR FRECUENCIAS ALMACENADAS */
  /**********************************/
 /*
 void VELOCIDAD_borrarFreqAlmacenadas (void){
      byte i;
      
      for (i=0; i<cant_frecuencias_almacenadas; i++){
        FRECUENCIAS_ALMACENADAS[i] = 0;
      }
      
      ptr_FRECUENCIAS_ALMACENADAS = FRECUENCIAS_ALMACENADAS;  // Inicializo puntero de frecuencias almacenadas
      frecuencias_almacenadas_cnt = 0;
      frecuencias_almacenadas_done = 0;
    }
*/
  
  /* RE-INICIAR CALCULO PERIODO PULSOS */
  /*************************************/
/*
 void VELOCIDAD_reiniCalculoPeridoPulsos (void){
      // Cada vez que se inhibe el calculo de velocidad, se pierden interrupciones de TIMER OVERFLOW => El contador de
      // OVF esta mal, y por ende cuenta mal el periodo, lo que conlleva a un error en el calculo de la velocida. 
      // Por eso se RE-INICIA el calculo de los pulsos [con la bandera que inhibe el calculo de la velocidad], para
      // que comienze a medir el periodo de los pulsos desde cero. Eso se logra, reseteando el valor de la variable 
      // ESTADO_PULSOS al valor PRIMER_PULSO.
      // Ademas de resetear todas las banderas que habilitan los calculos.
        do_calculo_velocidad = 0;       // No calcular velocidad
		n_pulsos_anterior = __HAL_TIM_GET_COUNTER(&pulsoACCUM);
		cnt_mseg=0;

    }
  */
         
  /* CALCULO DE VELOCIDAD POR PERIODO */
  /************************************/
 /*
 void VELOCIDAD_calculo_porPeriodo (void){
      dword pulsos_x_hora;
      dword frecuencia_promedio;
      
      frecuencia_promedio = VELOCIDAD_promediarFreq();    // Hago el promedio de las frecuencias

      //pulsos_x_hora = frecuencia_promedio * (dword) 3600; // Pulsos en 3600seg (una hora)
      pulsos_x_hora = frecuencia_promedio; // Pulsos en 3600seg (una hora)
      // No sabemos bien porque, pero si primero dividimos y después multiplicamos,
      // da mas precisa la cuenta

      if((word)(pulsos_x_hora / PULSOS_x_KM)< 0xffff){
          VELOCIDAD = (word)(pulsos_x_hora / (3*PULSOS_x_KM));    // Velocidad en km/h, con 2 decimales
      }
      else{
    	  VELOCIDAD = 0xffff;
      }


      VELOCIDAD_NEW = (3600*frecuencia)/PULSOS_x_KM;

    }
*/

  
  /* REDISPARA TIME OUT PULSOS */
  /*****************************/
 /*
 void VELOCIDAD_redispararTimeOutPulsos(void){
      // Setea el valor de TIME OUT de los PULSOS, de acuerdo al valor de la constante
      if (PULSOS_x_KM <= 750){
        calculo_periodo_TO_cnt = x10mseg_calculo_periodo_TO_L; 
      
      }else if ((PULSOS_x_KM > 750) && (PULSOS_x_KM <= 2000)){
        calculo_periodo_TO_cnt = x10mseg_calculo_periodo_TO_M; 
      
      }else if (PULSOS_x_KM > 2000){
        calculo_periodo_TO_cnt = x10mseg_calculo_periodo_TO_S;
      }
    }
*/

  /* DETERMINACION DE VELOCIDAD MAXIMA */
  /*************************************/
/*
void determineVELOCIDAD_MAX (void){
      // La velocidad maxima se determina luego de efectuar el calculo de velocidad
      // (es ahi donde se llama).
      // Como nos interesa por ESTADO de RELOJ, luego de que hay un cambio de reloj
      // (o sea un pase a cualquier estado) se resetea el valor de VELOCIDAD MAXIMA.
      if (VELOCIDAD_MAX < (VELOCIDAD /100)){
        VELOCIDAD_MAX = (VELOCIDAD /100);  // Divido x100 para dejarla sin los 2 decimales
      }
      if (VELOCIDAD_MAX_NEW < (VELOCIDAD_NEW /100)){
        VELOCIDAD_MAX_NEW = (VELOCIDAD_NEW /100);  // Divido x100 para dejarla sin los 2 decimales
      }

    }
*/
/*
static void filtro_frec (void){
    filtro  = filtro_frecuencia;
    filtro *= PULSOS_x_KM;
    filtro /= 3600;
}
*/
/*********************************************************************************************/
/* TIMER */
/*********/    
  
  /* TIME OUT CALCULO PERIODO */
  /****************************/
  /*
    void time_out_calculo_periodo (void){
      // Se ejecuta cada 10mseg. Es para que deje de medir el periodo de los pulsos
      // cuando ya no hay mas pulsos. Xq no tenemos forma de saber que dejaron de
      // entrar pulsos
      if (calculo_periodo_TO_cnt != 0){
        calculo_periodo_TO_cnt--;
        if (calculo_periodo_TO_cnt == 0){
          ESTADO_PULSOS = PRIMER_PULSO;				// Reseteo Estado Pulsos
          //TMR_OVF_IRQE = 0;                   // Deshabilito IRQ de Timer OverFlow
          periodo_pulsos = 0;                 // Reseteo Periodod de pulsos
          
          VELOCIDAD = 0;                      // Si se extinguieron los pulsos => Velocidad Nula
          RELOJ_chkVelTransicion();						// Calculo de la velocidad de Transision
          VELOCIDAD_borrarFreqAlmacenadas();  // Reseteo todo lo referente al almacenamiento de frecuencia
        }
      }
    }
  */
    
    
    
  /* TIMER CALCULO PERIODO */
  /*************************/
 /*
    void timer_calculo_periodo (void){
      // Cada 100mseg - Para inhibir el calculo en el arranque, por el ruido. Deja el contador
      // en cero y eso significa que expiro el tiempo
      if (timer_calculo_periodo_cnt != 0){
        timer_calculo_periodo_cnt--;
        if (timer_calculo_periodo_cnt == 0){
        	VELOCIDAD_iniCalculo();
        }
      }
    }
*/




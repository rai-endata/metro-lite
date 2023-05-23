/* File <sensor_asiento.c> */

/* 
  La consulta del sensor de asiento INTELIGENTE la dispara el TIMER cada 3seg. Solo lo hace si el movil esta
  trabajando con RELOJ INTERNO, y ademas esta LIBRE o FUERA DE SERVCIO.
  Cuando el timer dispara la consulta, se inicia el proceso. El mismo requiere que el master envie un pulso
  negativo de al menos 480useg de duracion. Luego de eso, es preciso que el resistor (pullup) tome la linea.
  Para ello el pin del uP debe configurarse como entrada. Una vez hecho esto, se debe esperar la respuesta
  del DS2401. La misma consiste en un pulso negativo que debe ser sampleado aproximademente 60useg dpues de
  que el pull-up tomo la linea. Si se recibe dicho pulso, esta todo en orden y puede iniciarse una comunicacion.
  Hasta ahora, solo se establecio la comunicacion, pero no se inicio. Para iniciarla, el master (para nosotros
  el micro) debe hacerle un pedido al DS2401. En nuestro caso, este pedido consiste en el numero de serie.
  El comando que se debe enviar es el 0x0F o el 0x33. Se recomienda usar el 0x0F xq es compatible con versiones 
  mas viejas de dispositivos. Recordar que el comando se envia transmitiendo el LSB primero (de la misma manera
  se reciben los datos). Cabe aclarar tb que el proceso de escritura de un "0" y de un "1" son distintos y deben
  respetar el esquema de tiempos establecido en la hoja de datos, para cumplir con las normas del protocolo. 
  Brevemente es asi:
    
    - para enviar un "1" el uP debe mantener baja la linea durante 9useg. Luego el pull-up controla el estado de
      la linea hasta terminar el cilco (slot) en aproximadamente 60useg despues. Ademas se debe dejar un tiempo
      de recuperacion de aprox. 10useg. Para facilitarlo, yo lo hago al inicio.
      
    - en cambio, para enviar un "0", se debe iniciar de la misma manera, pero la linea debe ser mantenida baja
      por el master durante todo el ciclo (slot). Esto es aproximadamente 60useg. Luego le cede el control
      al pull-up y luego de un minimo tiempo de recuperacion, ya esta listo para una nueva transmision.
      
  Ni bien el DS2401 recibe el comando solicitandole el numero de identificacion, este lo entrega. Empezando por
  el bit menos significativo: y la secuencia es la siguiente:
  
                     MSB													                           LSB
                      | Cyclic Redundancy Check | SERIAL NUMBER | FAMILY CODE |
                           last byte (8th)         byte 2nd-7th  	 1st byte            64 bits en total
  																						
  El procedimiento para leer es el siguiente: Dado que solo se cuenta con solo una linea de comunicacion, es
  preciso que haya alguna forma de sincronismo. La misma se logra enviando un pulso negativo de unos 9useg, 
  luego el DS2401 controla la linea y debe ser sampleada aproximadamente 10useg dpues de que se envio el pulso
  negativo. Por ultimo, se debe finalizar el ciclo, esperando que se cumplan los tiempos especificados. No olvidarse
  del tiempo de recuperacion, que al igual que antes, lo estoy haciendo antes de recibir un dato.
  
  Una vez recibidos todos los bytes se debe chequear el CRC... Si este se calcula sin problemas y se verifica,
  entonces solo resta comparar el numero de serie leido con el almacenado en la EEPROM.
  
  Antes de salir de la rutina, la misma se fija si hubo algun error anteriormente (en realidad es si hubo 3 errores)
  En caso de que ya hallan sucedidos, se muestra un cartel en la pantalla y automaticamente se pasa a ocupado. Ademas
  de enviar unos comandos a la central para informar de la sitacion
*/

/* INCLUDES */
/************/
	#include "Sensor Asiento.h"
	#include "Manejo de Buffers.h"
	#include "Reloj.h"
	#include "Timer.h"
	#include "main.h"
	//#include "Param Reloj.h"
	#include "Parametros Reloj.h"
	#include "Grabaciones en EEPROM.h"
	#include "odometro.h"
	#include "tipo de equipo.h"


/*
        #define TIPO_SENSOR_ASIENTO       ((const)EEPROM_PROG_relojCOMUN.varios1.MergedBits.tipo_sensor)
        #define SIN_SENSOR_ASIENTO      (TIPO_SENSOR_ASIENTO == 0)
        #define SENSOR_ASIENTO_NORM     (TIPO_SENSOR_ASIENTO == 1)
        #define SENSOR_ASIENTO_INTEL    (TIPO_SENSOR_ASIENTO == 2)

    //#define SENSOR_ASIENTO_segundos   ((const)EEPROM_PROG_relojCOMUN.varios1.Bits.sensorSeg)
      //#define TIPO_SERVICIO             ((const)EEPROM_PROG_relojCOMUN.varios1.Bits.tipo_servicio)
	//#define SENSOR_ASIENTO_segundos		0

    #define TIPO_SERVICIO			TAXI

    	#define REMIS                   0
        #define TAXI                    1
        #define SERVICIO_TAXI           (TIPO_SERVICIO == TAXI)
        #define SERVICIO_REMIS          (TIPO_SERVICIO == REMIS)

*/

/*********************************************************************************************/
/* ESTRUCTURAS */
/***************/    
  // Bits desmenusados
    typedef union {
          byte Byte;
          struct {
            byte bit1      :1;      
            byte bit2      :1;
            byte bit3      :1;
            byte bit4      :1;
            byte bit5      :1;
            byte bit6      :1;
            byte bit7      :1;
            byte bit8      :1;
          } Bits;
    } byte_type;
    
    
  // Banderas de DS2401
    typedef union {
          byte Byte;
          struct {
            byte PP           :1;    // Presence Pulse Detected
            byte CRC_ok       :1;    // CRC chequeado y valido
            byte timer_F      :1;    // Bandera del TTIMER de CONSULTA (SERIAL)
            byte error_cnt0   :1;    // Contador de Errores LSB
            byte error_cnt1   :1;    // Contador de Errores MSB
            byte error_F      :1;    // Bandera de Error
            byte :1;
            byte cons_serial  :1;    // Indica que se esta consultando el SERIAL
          } Bits;
          struct {
            byte            :3;
            byte grpERR_cnt :2;
          } MergedBits;
    } DS2401_Flags;    


  // Banderas de SENSOR_ASIENTO
    typedef union {
          byte Byte;
          struct {
            byte sensor_press     :1;    // Indica Sensor Presionado
            byte :1;
            byte rst_dist         :1;    // Indica que se debe resetear la distancia del sensor de asiento
            byte do_desrebote     :1;    // Indica que se esta haciendo el desrebote
            byte wrEEPROM         :1;    // Indica que hay que grabar la EEPROM
            byte :1;
            byte segunda_lectura  :1;    // Indica que se va a efectuar la segunda lectura => Comparar ambas lecturas
            byte writeFlash       :1;    // Indica grabacion de FLASH
          } Bits;
    } SENSOR_ASIENTO;    


/*********************************************************************************************/
/* PROTOTIPOS */
/**************/
  
  // RUTINAS INTERNAS

  static void Pase_a_OCUPADO_x_sensorASIENTO (byte causa);
  static void carreraBandera_toPulsos (void);
  static byte chkCarreraBandera_distancia (void);
  static byte chkCarreraBandera_tiempo (void);
  static byte chkCarreraBandera_done (void);

  // RUTINAS DE FLASH
  static void ASIENTO_requestTurnOnByAsiento_enFLASH (void);
  
  

/*********************************************************************************************/
/* VARIABLES */
/*************/
  #define asiento_ON							1     // El pulsador tira la linea a masa, pero hay un inversor en el medio
  #define asiento_OFF             0
  
  #define SENSOR_ASIENTO_RELOJ_OK       (RELOJ_LIBRE || RELOJ_FUERA_SERVICIO)
  
  static DS2401_Flags                   _DS2401_F;               // Banderas del CHIP de Sensor de Asiento CONSULTA SERIAL
    #define DS2401_F                    _DS2401_F.Byte
    #define DS2401_PP                   _DS2401_F.Bits.PP
    #define DS2401_CRC_ok               _DS2401_F.Bits.CRC_ok
    #define DS2401_timer_F              _DS2401_F.Bits.timer_F
    #define DS2401_error_cnt            _DS2401_F.MergedBits.grpERR_cnt
    #define DS2401_error                _DS2401_F.Bits.error_F
    #define hab_DS2401_timer            _DS2401_F.Bits.timer_hab
	#define	DS2401_consulta_serial      _DS2401_F.Bits.cons_serial
  
  static SENSOR_ASIENTO                 _sensor_asiento_F;
    #define sensor_asiento_F            _sensor_asiento_F.Byte
    #define sensor_asiento_press        _sensor_asiento_F.Bits.sensor_press
    #define reset_sensor                _sensor_asiento_F.Bits.rst_dist
    #define doing_desrebote_asiento     _sensor_asiento_F.Bits.do_desrebote
    #define asiento_2nd_read            _sensor_asiento_F.Bits.segunda_lectura
    #define asiento_grabarEEPROM_F      _sensor_asiento_F.Bits.wrEEPROM
    #define asiento_writeFlash          _sensor_asiento_F.Bits.writeFlash
  
  
  static dword ASIENTO_offsetPulsos;        // Valor del contador de pulsos al detectar almohadilla presionada
  static dword PULSOS_CARRERA_BANDERA;      // Equivalente en Pulsos de la Carrera de Bandera
  
  static word ASIENTO_segundosSensor;       // Contador para el sensor de asiento funcionando por tiempo
  
  static word DS2401_timer_cnt;             // Contador del timer del DS2401 (SERIAL)
    #define seg_DS2401_timer        10
  
  static byte chk_sensor_cnt;               // Contador del timer del DS2401 (SENSOR)
    #define seg_chk_sensor          3
    
  static byte desrebote_sensor_cnt;         // Contador del timer de desrebote
    #define mseg_desrebote_sensor   10

  
  // VARIABLES DE FLASH
#pragma CONST_SEG FLASH_DATA_PAGE
  static const word   FLASH_TURN_ON_BY_ASIENTO;
#pragma CONST_SEG DEFAULT  
  

/*********************************************************************************************/
/* RUTINAS */
/***********/
  
  /* INICIALIZACION DE SENSOR DE ASIENTO */
  /***************************************/
    void ASIENTO_ini (void){
  		carreraBandera_toPulsos();        // Convierto la carrera de bandera a Pulsos
        reset_sensor = 1;                 // Para q al detectar SENSOR ASIENTO, resetee la distancia/tiempo del sensor
  		
  		if (SENSOR_ASIENTO_INTEL){
  		  DS2401_timer_cnt = seg_DS2401_timer - 2;    // para que en 2seg consulte al DS2401
  		}
  		
  		// Si se trata de un primer encendido => Borro el serial del sensor.
  		// Si bien en el primer encendido (de fabrica) esto ya esta borrado, hay situaciones
  		// como ser la programacion en la que vuelvo a "activar" el primer encendido; y debo
  		// asegurar que ciertas cosas esten como de fabrica
  		if (VISOR_firstBoot && !SIN_SENSOR_ASIENTO){
        //ASIENTO_deleteSerialEEPROM();
  		}
    }

void ini_puerto_sensor_asiento(void){
	GPIO_InitTypeDef gpioinitstruct;
	//habilita Clock del pin sensor asiento
	SENSOR_ASIENTO_GPIO_CLK_ENABLE();

	gpioinitstruct.Pin = SENSOR_ASIENTO_PIN;
	gpioinitstruct.Pull = GPIO_PULLDOWN;
	gpioinitstruct.Speed = GPIO_SPEED_FREQ_HIGH;

	// Configure pin sensor asiento como entrada
	gpioinitstruct.Mode = GPIO_MODE_INPUT;

	HAL_GPIO_Init(SENSOR_ASIENTO_GPIO_PORT, &gpioinitstruct);

	GPIO_PinState estado_sa_pin;
	estado_sa_pin = HAL_GPIO_ReadPin(SENSOR_ASIENTO_GPIO_PORT, SENSOR_ASIENTO_PIN);
}


  /* OCUPARSE TRAS ENCENDIDO POR SENSOR DE ASIENTO */
  /*************************************************/
  /*
   void ASIENTO_ocuparseAfterTurnOn (void){
      // En caso de que el equipo haya estado a apagado, y se enciende por sensor de
      // asiento, al encenderse debe ocuparse
      // 
      // Esta rutina se llama en el INICIO del equipo
      word turnOnByAsiento;
      
      turnOnByAsiento = FLASH_TURN_ON_BY_ASIENTO;
      
      if (turnOnByAsiento == 1){
        ASIENTO_requestTurnOnByAsiento_enFLASH();     // Limpio Flash
        
        Pase_a_OCUPADO_x_sensorASIENTO(sensorAsiento_pressed);      // Pasa a Ocupado por el sensor de asiento presionado
      }
    }
  */

  /* CONSULTA SERIAL AL SENSOR DE ASIENTO */
  /****************************************/

  
  /* CONSULTA AL SENSOR DE ASIENTO */
  /*********************************/
    void ASIENTO_consultaSensor (void){
    	 // FUNCION LLAMDA ESDE EL LOOP
         // Trabajo de la siguiente manera:
         //  - Al detectar sensor presionado guardo valor del contador de pulsos
         //  - Luego, mientras se mantenga presionado calculo la diferencia entre
         //    el valor actual y el valor guardado.
         //  - Cuando supere la carrera de bandera, me ocupo.

    	GPIO_PinState pin_ASIENTO_IN;

     		if (!SIN_SENSOR_ASIENTO && SENSOR_ASIENTO_RELOJ_OK){
     		  if (sensor_asiento_press){
     		    if (RELOJ_INTERNO && SENSOR_ASIENTO_RELOJ_OK){
				   if (reset_sensor){
					 reset_sensor = 0;                                     				// Bajo bandera

					 if (SENSOR_ASIENTO_segundos){
					   ASIENTO_segundosSensor = CARRERA_BANDERA;          				 // La "Carrera Bandera" es el tiempo del sensor
					 }else{
						 ASIENTO_offsetPulsos = __HAL_TIM_GetCounter(&pulsoACCUM);       // Guardo valor actual del contador
					 }
				   }

				   if (chkCarreraBandera_done() ){ // Me fijo si no esta haciendo CONSULTA SERIAL xq debe chequear que siga presiondao y debe setearlo como entrada
					 if (SENSOR_ASIENTO_RELOJ_OK){                         // Si el reloj lo permite
						 pin_ASIENTO_IN = HAL_GPIO_ReadPin(SENSOR_ASIENTO_GPIO_PORT, SENSOR_ASIENTO_PIN);
						 if(pin_ASIENTO_IN == asiento_ON){                  // ... y sigue presionado
							 Pase_a_OCUPADO_x_sensorASIENTO(sensorAsiento_pressed);          // Pasa a Ocupado por el sensor de asiento presionado
						 }
					     reset_sensor = 1;                                   // Reset reset_sensor
					 }
				   }
             }
     		  }
     		}
       }

  /* OBTENER STATUS DE PRESION DEL SENSOR DE ASIENTO */
  /***************************************************/

    byte ASIENTO_getPressStatus (void){
      return(sensor_asiento_press);
    }
    
    

/*********************************************************************************************/
/* RUTINAS INTERNAS */
/********************/

  /* INITIALIZATION PROCEDURE */
  /****************************/
  


  
  
  
  



  
 
  /* PASAR A OCUPADO POR SENSOR DE ASIENTO */
  /*****************************************/
  static void Pase_a_OCUPADO_x_sensorASIENTO (byte causa){
    // Pasa el RELOJ INTERNO a OCUPADO, por el SENSOR de ASIENTO. Como el pasaje puede ser desde LIBRE o F.SERVICIO
    // dependiendo del estado, las acciones a tomar son distintas.
    // Si esta en LIBRE: Hace un cambio de reloj (presiona tecla T1 larga) y listo, ya esta.
    // Si esta en F. SERVICIO: Debe hacer 2 cambios de reloj (2 teclas T1 larga) la primera para salir de F. SERVICIO
    // (y quedaria en LIBRE) y la segunda para pasar a OCUPADO. Pero se debe levantar una bandera que indique que
    // debe grabar la TABLA de LIBRE con todos CEROS ya que vino de F. SERVICIO y debe pasar directamente a OCUPADO.
    //
    // EDIT 02/11/12
    //  Se agrego la causa del pase a OCUPADO por sensor de asiento, puede ser normal (presionado)
    // o por error
    
    if (chkVISOR_powerOFF){
      // El equipo esta apagado => Debo encenderlo, por sensor de asiento
      ASIENTO_requestTurnOnByAsiento_enFLASH();   // Solicito grabacion de FLASH
      ASIENTO_turnOnByAsiento_enFLASH(1);         // Fuerzo grabacion de FLASH
      
      MOTIVO_RESET = RST_TURN_ON_byASIENTO;       // Reset por SENSOR DE ASIENTO
      resetVISOR;                       // Resetear Visor
    
    }else{
      // El equipo esta encendido => Funcionamiento normal
      if (RELOJ_FUERA_SERVICIO){
    	  paseLIBRE_SENSOR_ASIENTO = 1;
    	  cambio_de_reloj_x_sensor_asiento();          // Paso a Libre (salgo de F.SERV)
      }
      
      RELOJ_INTERNO_setAsiento(causa);  // Pase a ocupado por sensor de asiento
      //tarifa = TARIFA_AUTO_getNroTarifa();

		tarifa_1_8 = TARIFA_AUTO_getNroTarifa();
		if(tarifa_1_8 < 5){
			tarifa_1_4 = tarifa_1_8;
		}else{
			tarifa_1_4 = tarifa_1_8 - 4;
		}

	  paseOCUPADO_SENSOR_ASIENTO = 1;
      cambio_de_reloj_x_sensor_asiento();            // Paso a ocupado
    }
  }

  
  /* CARRERA DE BANDERA EN PULSOS */
  /********************************/
    static void carreraBandera_toPulsos (void){
      // Calcula a cuantos pulsos equivale la carrera de bandera.
      // La constante esta en pulsos/km y la carrera de bandera en mts => tengo que calcular
      // los pulsos por metro y multiplicarlo por la carrera de bandera
      //
      //                           CARRERA_BANDERA * PULSOS_x_KM
      // PULSOS_CARRERA_BANDERA = -------------------------------  x1000
      //                                     DIVISOR
      //
      // NOTA: Como voy a trabajar con pulsos REALES, ya tengo el divisor incluido en los pulsos
      //
      // => PULSOS_CARRERA_BANDERA = CARRE_BANDERA * PULSOS_x_KM x1000
      PULSOS_CARRERA_BANDERA = CARRERA_BANDERA;
      PULSOS_CARRERA_BANDERA *= PULSOS_x_KM;
      PULSOS_CARRERA_BANDERA /= 1000;

    }
 

  /* CHEQUEAR CARRERA BANDERA FINALIZADA POR DISTANCIA */
  /*****************************************************/
    static byte chkCarreraBandera_distancia (void){
      // Esta rutina chequea si se cumplio la carrera de bandera
      //  0: No se cumplio
      //  1: Se cumplio
      dword pulsosActuales;
      dword difPulsos;
       
        pulsosActuales = __HAL_TIM_GetCounter(&pulsoACCUM);       // Guardo valor actual del contador

      if (pulsosActuales >= ASIENTO_offsetPulsos){
        difPulsos = pulsosActuales - ASIENTO_offsetPulsos;
      }else{
        difPulsos = ASIENTO_offsetPulsos - pulsosActuales;
      }
      
      if (difPulsos >= PULSOS_CARRERA_BANDERA){
        return(1);          // Se cumplio la Distancia
      }else{
        return(0);          // No se cumplio la Distancia
      }
    }    


  /* CHEQUEAR CARRERA BANDERA FINALIZADA POR TIEMPO */
  /*****************************************************/
    static byte chkCarreraBandera_tiempo (void){
      // Esta rutina chequea si se cumplio la carrera de bandera
      // pero por tiempo. Si el contador de segundos es nulo, es
      // xq se cumplio
      //  0: No se cumplio
      //  1: Se cumplio
       
      if (ASIENTO_segundosSensor == 0){
        return(1);          // Se cumplio la Distancia
      }else{
        return(0);          // No se cumplio la Distancia
      }
    }        


  /* CHEQUEAR CARRERA BANDERA FINALIZADA */
  /***************************************/
    static byte chkCarreraBandera_done (void){
      // Se fija si finaliza la carrera de bandera ya sea por
      // tiempo o distancia, de acuerdo a como este programado
      
      if (SENSOR_ASIENTO_segundos){
        return(chkCarreraBandera_tiempo());
      }else{
        return(chkCarreraBandera_distancia());
      }
    }     



      

/*********************************************************************************************/
/* RUTINAS DE FLASH */
/********************/

  /* SOLICITAR GRABACION DE ENCENDIDO POR SENSOR DE ASIENTO EN FLASH */
  /*******************************************************************/
    static void ASIENTO_requestTurnOnByAsiento_enFLASH (void){
      asiento_writeFlash = 1;
    }

 
  /* GRABACION DE ENCENDIDO POR SENSOR DE ASIENTO EN FLASH */
  /*********************************************************/
    void ASIENTO_turnOnByAsiento_enFLASH (byte forced){
      byte data[2];
      tFLASH_ERROR error;
      
      if (asiento_writeFlash && (/*FLASH_chkCanUpdate() ||*/ forced)){
        asiento_writeFlash = 0;
        
        if (chkVISOR_powerOFF){
          // Si esta apagado, grabo que lo encienda
          data[0] = 0;
          data[1] = 1;
        }else{
          // Si esta encendido, reseteo la variable
          data[0] = 0;
          data[1] = 0;
        }
        error = FLASH_updateSector((word*) &FLASH_TURN_ON_BY_ASIENTO, (byte*) &data, (word) 2);
      }
    }
    
    
    

/*********************************************************************************************/
/* TIMER */
/*********/
  
  /* TIMER DE CONSULTA SERIAL */
  /****************************/
    void ASIENTO_chkSerial_timer (void){
  		// Me fijo si fue programado con SENSOR INTELIGENTE
  		// Ejecutada cada 1seg
  		if (SENSOR_ASIENTO_INTEL){
        if (RELOJ_INTERNO && SENSOR_ASIENTO_RELOJ_OK){
          if (DS2401_timer_cnt != 0){
            DS2401_timer_cnt--;
            if (DS2401_timer_cnt == 0){
              DS2401_timer_F = 1;
              DS2401_timer_cnt = seg_DS2401_timer;    // Re-Disparo Timer
            }
          }
        }
  		}
    }
  
  
  
  /* TIMER CHK SENSOR ASIENTO */
  /****************************/
  void ASIENTO_chkSensor_timer (void){
    // Ejecutada cada 1seg
    // levanta sensor_asiento_press si es que esta presionado, para luego procesarlo
    // en LOOP principal. Si ya detecto que esta presionado, sale de la rutina
		// Me fijo si NO fue programado SIN SENSOR y si no esta consultando el serial
		if (!DS2401_consulta_serial){
      chk_sensor_cnt++;
      if (chk_sensor_cnt == seg_chk_sensor){
        chk_sensor_cnt = 0;
        if (doing_desrebote_asiento == 0){
          // Si no estoy haciendo el desrebote, lo hago. La bandera la bajo en el
          // desrebote
          desrebote_sensor_cnt = mseg_desrebote_sensor;   // Disparo desrebote
          doing_desrebote_asiento = 1;                    // Estoy haciendo desrebote
        }
      }
		}
  }


  /* TIMER DESREBOTE SENSOR ASIENTO */
  /**********************************/
  void ASIENTO_desrebote_timer (void){

  	GPIO_PinState pin_ASIENTO_IN;

  	// Ejecutada cada 10mseg
  	if (desrebote_sensor_cnt != 0 ){
  		desrebote_sensor_cnt--;
  		if (desrebote_sensor_cnt == 0 ){
  			//tomo estado del pin del sensor de asiento
  			pin_ASIENTO_IN = HAL_GPIO_ReadPin(SENSOR_ASIENTO_GPIO_PORT, SENSOR_ASIENTO_PIN);
  			if (pin_ASIENTO_IN == asiento_ON){                  //Me fijo si esta presionado (lo tira a masa)
  				sensor_asiento_press = 1;                       //se presiono
  				doing_desrebote_asiento = 0;                    //Ya no estoy haciendo desrebote
  				chk_sensor_cnt = 0;								//espera para volver a checkear el sensor
  			}else{
  				sensor_asiento_press = 0;                       //no se presiono o se soltó
  				doing_desrebote_asiento = 0;                    //Ya no estoy haciendo desrebote
  				reset_sensor = 1;                               //Reset distancia/tiempo, para que vuelva a contar la distancia cuando vuelta a ser presionado
  				chk_sensor_cnt = 0;								//espera para volver a checkear el sensor
  			}
  		}
  	}
  }


  /* TIMER DE SENSOR DE ASIENTO POR TIEMPO */
  /*****************************************/
    void ASIENTO_chkTiempo_sensor (void){
  		// Ejecutada cada 1seg
  		// Me fijo si fue programado con SENSOR ALGUN SENSOR DE ASIENTO
  		// y esta programado para funcionar por tiempo en lugar de distancia
  		//  => Voy decrementando la "carrera de bandera" que en realidad son
  		//     segundos
  		if (!SIN_SENSOR_ASIENTO && SENSOR_ASIENTO_segundos){
        if (RELOJ_INTERNO && SENSOR_ASIENTO_RELOJ_OK){
          if (ASIENTO_segundosSensor != 0){
            ASIENTO_segundosSensor--;
          }
        }
  		}
    }


#include "Inicio.h"
#include "DA Communication.h"
#include "Lista Comandos.h"
#include "Constantes.h"
//#include "- Reloj-Blue Config -.h"
#include "- metroBLUE Config -.h"
#include "rtc.h"
#include "gps.h"
//#include "Param Reloj.h"
#include "Parametros Reloj.h"

#include "stddef.h"
#include "flash.h"
#include "main.h"
#include "Reportes.h"
#include "Odometro.h"
#include "Tarifacion Reloj.h"
#include "reloj.h"
#include "eeprom.h"
#include "Comandos sin conexion a central.h"

tDATE HoraApagado;
byte motivo_reset;
inicioFLAGS1   _inicio_F1;
byte 		corteALIMENTACION;
byte viajeInconcluso;

static void corteLargo(uint16_t centralRx_cmdRLJ_cortePrevio);
static byte corteCorto(uint16_t centralRx_cmdRLJ_cortePrevio);

static byte tarifaPerdida;
static dword fichasDPerdida;
static dword fichasTPerdida;
static dword dineroPerdido;
tRELOJ estadoReloj_en_corteAlimentacion;


// VARIABLES DE FLASH

void SOURCE_DATE_Ini (void){
    // Inicializacion de la FUENTE de la fecha y la hora.
    // Si esta con GPS, sera del GPS, sino del RTC
    if (HORA_GPS){
      DATE_ptr = &GPS.Date;

    }else if (HORA_RTC){
      DATE_ptr = &RTC_Date;
    }
    __NOP();
    __NOP();
  }

/* DETERMINAR FECHA VALIDA */
 /***************************/
   byte dateValid (tDATE date){
     // Indica si la fecha es valida o no:
     //  0: No valida
     //  1: Valida
     byte valid;

     if ((date.hora[0] <= 0x23)   &&
   	    (date.hora[1] <= 0x59)   &&
   	    (date.hora[2] <= 0x59)   &&
   	    (date.fecha[0] <= 0x31)  &&
   	    (date.fecha[1] <= 0x12)  &&
   	    (date.fecha[2] >= 0x12)){
   	  valid = 1;

   	}else{
   	  valid = 0;
     }
     return(valid);
   }



   /* ACTUALIZAR PRIMER ENCENDIDO */
   /*******************************/

   /* ACTUALIZAR PRIMER ENCENDIDO */
   /*******************************/
     void actualizarPrimerEncendido (void){
       // Dado que la determinacion se debe hacer en el arranque, ni bien se termina de inicializar
       // el micro, par asi poder mostrar el aviso del primer encendido. No puedo actualizar
       // dicha variable en FLASH en ese momento, xq si mientras esta en la inicializacion se
       // resetea el equipo, se pierde esta indicacion.
       //
       // EDIT 28/06/2012
       // Ahora se actualiza en onFirstDate
    volatile tFLASH_ERROR error;
       uint16_t data;
       EEPROM_ReadBuffer(&data,EEPROM_PRIMER_ENCENDIDO,SIZE_EEPROM_PRIMER_ENCENDIDO);
       if (data != valorPrimerEncendido){
         // Se trata del primer encendido
    	 error = EEPROM_write(EEPROM_PRIMER_ENCENDIDO, valorPrimerEncendido); // Grabar Word en EEPROM

       }
     }

     /* BORRAR PRIMER ENCENDIDO */
       /***************************/
       void borrarPrimerEncendido (void){
           volatile tEEPROM_ERROR error;

           error = EEPROM_write(EEPROM_PRIMER_ENCENDIDO, 0xffff); // Grabar Word en EEPROM

       }

   /* ACCIONES A REALIZAR AL OBTENER DATE POR PRIMERA VEZ */
     /*******************************************************/
       void firstDATE (void){
         // Aca se agrupan todas las acciones que requieren del DATE, cuando arranca el equipo.
         // Ellas son:
         //  - Setear Date Cambio Reloj (para el estado del arranque)
         //  - Grabar un inicio de sesion

         // - chequear estado de bloqueo de reloj
         //
         // EDIT 29/01/13
         //  Cambio la forma de determinar FIRST DATE. Antes lo hacia con fecha[0] != 0, es
         // decir, DIA != 0.
         // El tema es que si quisiera usar esto con el GPS, no me serviría de mucho, xq
         // inicialmente me carga la fecha de produccion/fabricacion del modulo GPS => Para cuando
         // la hora es de GPS, ademas debo tener posicion valida
    	  byte buffer_backup[1];

    	  getDate();
    	  //if (!INICIO_firstDate && (DATE_ptr->fecha[0] != 0) && ((HORA_GPS && GPS.valid) || HORA_RTC) /*&& !PROGRAMADOR_chkReset()*/){
   		  if (!INICIO_firstDate && ((HORA_GPS && GPS.valid) || HORA_RTC) /*&& !PROGRAMADOR_chkReset()*/){
    		  //if (!INICIO_firstDate ){
           // Si todavia no tengo el primer date y la fecha NO ES cero => Ya tengo el primer
           // date
           INICIO_firstDate = 1;                         // Seteo Bandera
           //guarda estado inicial de bloqueo de equipo

           //if (!INICIO_microCorte){
           //  RELOJ_tomarDateCambioReloj_Arranque();      // Hora de cambio de Reloj (solo si NO ES uCorte)
           //}

           if (VISOR_firstBoot){
             // EDIT 22/02/13
             //En caso de un reset forzado de Reportes,
        	 //tambien debo iniciar la primer sesion del chofer
             CHOFER_firstBoot();                         // En el primer arranque debo grabar un inicio de Sesion con CHF #1
           }

           if (REPORTES_HABILITADOS){
        	   if (!VISOR_firstBoot){
        	   //if (!VISOR_firstBoot && !INICIO_microCorte){
        		   if(corteALIMENTACION==1){
        			   // DESCONEXION
        			   (void)REPORTE_queueDesconexionAlim(HoraApagado, RELOJ_INTERNO_getChofer(), tarifaPerdida, fichasDPerdida, fichasTPerdida, dineroPerdido);
						//borro corteALIMENTACION
						borroCorteALIMENTACION();
        		   }
        	   }
           }

           if (VISOR_firstBoot){
             // Lo correcto es RECIEN AHORA actualizar el valor del 1º encendido
             actualizarPrimerEncendido();         // Como ya termine las inicializaciones, puedo cambiar el valor del 1º encendido en FLASH
           }
         }
       }

/* DETERMINAR SI SE TRATA DEL PRIMER ENCENDIDO */
/***********************************************/

       void determinePrimerEncendido (void){
       	// Esta rutina se llama en el arranque, ni bien terminamos con la inicializacion del
       	// HCS12, para saber si se trata de un primer encendido o no y asi saber como se
       	// deben inicializar el resto de los modulos

       	uint16_t primerEncendido, huboCorteLargo;
       	uint16_t PRUEBA;

       	EEPROM_ReadBuffer(&primerEncendido,EEPROM_PRIMER_ENCENDIDO,SIZE_EEPROM_PRIMER_ENCENDIDO);
       	if ( primerEncendido == valorPrimerEncendido ){
       		// No se trata del primer encendido
       	  VISOR_firstBoot = 0;
       	}else{
       	  // Se trata del primer encendido
       	  VISOR_firstBoot = 1;
       	  //lo programo recien cuando voy a guardar sesion inicial
       	  //porque si lo borro aca, y se desconecta el equipo antes de que se guarde
       	  //el inicio de sesion, cuando vuelve a encender ya no guarda el inicio de sesion
       	  //y ya no se puede cerrar turno
       	  //EEPROM_write(EEPROM_PRIMER_ENCENDIDO, valorPrimerEncendido); // Grabar Word en EEPROM

       	  //inicializa corte largo
       	  EEPROM_write(EEPROM_YA_HUBO_CORTE_LARGO, rxCorteLargoPrevio); // Grabar Word en EEPROM
       	  iniPactado();	//inicializa ocupado pactado

       	  //EEPROM_ReadBuffer(&PRUEBA,EEPROM_PRIMER_ENCENDIDO,SIZE_EEPROM_PRIMER_ENCENDIDO);
       	  //RELOJ_INTERNO_reOpenTurno();

		#ifdef VISOR_REPORTES
		   if (HORA_RTC){
			  REPORTES_HABILITADOS = 1;
			}else{
			  REPORTES_HABILITADOS = 0;
			}
		  #else
			REPORTES_HABILITADOS = 0;
		  #endif

	      ini_newReg_ptr();					// Inicializo puntero PUT y GET de cola
       	  RELOJ_INTERNO_newSesion(1);

       	}
       }

      void set_noRxCorteLargoPrevio(byte estado_reloj){
    	   uint16_t  aux, aux1, aux2;
    	   aux1 = estado_reloj;
    	   aux2 = noRxCorteLargoPrevio & 0xff00;
    	   //aux = noRxCorteLargoPrevio | aux1;
    	   aux = aux2 | aux1;
    	   //guardo que ya hubo corte largo previo y estado de reloj
    	   EEPROM_write(EEPROM_YA_HUBO_CORTE_LARGO, aux); // Grabar Word en EEPROM
       }

      void set_noRxCorteCortoPrevio(byte estado_reloj){
    	   uint16_t  aux, aux1, aux2;
    	   aux1 = estado_reloj;
    	   aux2 = noRxCorteCortoPrevio & 0xff00;
    	   //aux = noRxCorteCortoPrevio | aux1;
    	   aux = aux2 | aux1;
    	   //guardo que ya hubo corte largo previo y estado de reloj
    	   EEPROM_write(EEPROM_YA_HUBO_CORTE_LARGO, aux); // Grabar Word en EEPROM
       }

      void set_rxCorteNormal(void){
    	   //guardo que ya hubo corte largo y estado de reloj
    	   EEPROM_write(EEPROM_YA_HUBO_CORTE_LARGO, rxCorteLargoPrevio); // Grabar Word en EEPROM
       }



	void check_corte_alimentacion(void){

			byte* addressFLASH_cobrandoDATE;
			byte* addressFLASH_ocupadoDATE;
			byte* address_ocupadoDATE;
			byte* address_cobrandoDATE;
			byte buffer_backup[1];
			tREG_LIBRE*	ptrREG_LIBRE;
			byte statusSC; 				// 0: Encontro registro a pagar en tabla, 1: No encontro registro a pagar en tabla
			byte error_noEsperado;
			uint16_t centralRx_cmdRLJ_cortePrevio;                 //00xx: la central recibio comandos de reloj despues del corte largo anterior
																	//01xx: la central no recibio comandos de reloj despues del corte largo anterior
			INICIO_microCorte = 0;

			//check corte largo previo
			//huboCorteLargo sucede cuando, si, previo a este encendido hubo un corte lago y antes que se transmitiera el comando de reloj,
			//volvio a haber otro corte largo sin que se pudiera informar a la central el estado previo al corte.
			//el corte previo se hace usando esta funcion: set_noRxCorteLargoPrevio
			EEPROM_ReadBuffer(&centralRx_cmdRLJ_cortePrevio, EEPROM_YA_HUBO_CORTE_LARGO, SIZE_YA_HUBO_CORTE_LARGO);
			//leer hora de apagado
			read_horaAPAGADO_eeprom();
			//toma fecha y hora de encendido
			//getDate();
			//tomas datos guardos en corte de alimentacion
			RELOJ_setTarifa(tarifa_1_8);
			RELOJ_iniTarifacion();
			read_backup_eeprom();
			readEepromDATE(&libreDATE);
			EEPROM_ReadBuffer(&VELOCIDAD_MAX_VIAJE, ADDR_EEPROM_VELOCIDAD_MAX_VIAJE, SIZE_VELOCIDAD_MAX_VIAJE);
			//calculo tiempo transcurrido desde que se apago hasta que se encendio
			INICIO_microCorte = diferenciaHoraria(horaEncendido.hora, HoraApagado.hora, 16);

			//ACCIONES SEGUN TIPO DE CORTE
			if(INICIO_microCorte){
			  // CORTE CORTO
			  //ME FIJO SI EL CORTE CORTO VINO LUEGO DE UN CORTE LARGO NORMAL
			  //corte  anormal seria:si despues de un corte, que se transmite comandos encendido, cambios de reloj .., y antes de que estos se transmitan
			  //o sean recibidos vuelve a haber un corte.
			  if(!(centralRx_cmdRLJ_cortePrevio&0xff00)){
				  //viene de corte  normal (largo)
				  estadoReloj_en_corteAlimentacion = ESTADO_RELOJ;
				  set_noRxCorteCortoPrevio(ESTADO_RELOJ);
				  corteCorto(centralRx_cmdRLJ_cortePrevio);
			  }else if((centralRx_cmdRLJ_cortePrevio>>8) == 0x0002){
				  //viene de un corte corto
				  estadoReloj_en_corteAlimentacion = (byte)centralRx_cmdRLJ_cortePrevio&0x00ff;
				  set_noRxCorteCortoPrevio(estadoReloj_en_corteAlimentacion);
				  corteCorto(centralRx_cmdRLJ_cortePrevio);
			  }else{
				  //viene de corte largo previo
				  corteLargo(centralRx_cmdRLJ_cortePrevio);
			  }
			  //borro corteALIMENTACION, para que no guarde desconexion de alim. en reportes
			  borroCorteALIMENTACION();
			}else{
			  //CORTE LARGO
			  corteLargo(centralRx_cmdRLJ_cortePrevio);
			}
	}



static void corteLargo(uint16_t centralRx_cmdRLJ_cortePrevio){

		 byte error_noEsperado;
		 byte statusSC;

		//CORTE LARGO
		RELOJ_setTarifa(tarifa_1_8);
		recuperar_valor_viaje_backup();
		tarifaPerdida = tarifa_1_8;

		fichasDPerdida = fichas_xPulsos;
		fichasTPerdida = fichas_xTiempo;;
		dineroPerdido = VALOR_VIAJE;

		//Configuración de contador de pulsos
		//DISTANCIA_setPulsosCntOld(0);
		//__HAL_TIM_SET_COUNTER(&pulsoACCUM,0);
		//PULSE_ACCUM_CNT = __HAL_TIM_GetCounter(&pulsoACCUM);
		DISTANCIA_iniCalculo_PULSE_ACCUM();

		// si no hubo corte previo, gurda estado de reloj ACTUAL
		// si hubo corte previo, recupera estado de reloj en corte previo
		//huboCorteLargo sucede cuando, si, previo a este encendido hubo un corte lago y antes que se transmitiera el comando de reloj,
		//volvio a haber otro corte largo sin que se pudiera informar a la central el estado previo al corte.
		//el corte previo se hace usando esta funcion: set_noRxCorteLargoPrevio

		iniPactado();

		if(!(centralRx_cmdRLJ_cortePrevio&0xff00)){
		  //corte normal
			estadoReloj_en_corteAlimentacion = ESTADO_RELOJ;
			set_noRxCorteLargoPrevio(ESTADO_RELOJ);
		}else{
			//doble corte
			//recupero estado de reloj, ya que como hubo corte previo en al apagarde guardo libre
			estadoReloj_en_corteAlimentacion = (byte)centralRx_cmdRLJ_cortePrevio&0x00ff;
			fichasDPerdida = 0;
			fichasTPerdida = 0;
			dineroPerdido = 0;
			set_noRxCorteLargoPrevio(estadoReloj_en_corteAlimentacion);

		}

		if(estadoReloj_en_corteAlimentacion != OCUPADO){
			//solo si hubo corte largo en ocupado
			//hubo importe perdido
			fichasDPerdida = 0;
			fichasTPerdida = 0;
			dineroPerdido = 0;
		}

		if(datosSC_cntWORD == 0){
			//no hay comandos sin conexion guardados en el apagado
			error_noEsperado = 0;
			if(estadoReloj_en_corteAlimentacion == COBRANDO){
				//SE APAGO EN COBRANDO y no hay comandos acumulados sin conexion, por lo tanto se debe enviar
				//el ultimo libre para que el sistema pueda cerrar el viaje
				statusSC = load_regViaje(nroCorrelativo_INTERNO);	//levantar datos de la tabla
				if(!statusSC){
					rearmar_y_TX_cmdLibre(nroCorrelativo_INTERNO);
					corregir_dateLIBRE();
					libreDATE = RTC_Date;
					VALOR_VIAJE = regVIAJE.importe;

				}else{
					error_noEsperado = 1;
				}
				tarifaPerdida = tarifa_1_8;
				fichasDPerdida = 0;
				fichasTPerdida = 0;
				dineroPerdido = 0;

			}else{
				//SI SE APAGO EN OCUPADO,  no hay comandos acumulados sin conexion, por lo tanto no se tiene informacion
				//de como concluyo el viaje. Se transmite un libre sin datos, y es particularmente importante que el importe
				//enviado en este caso sea cero, para no asignar un importe erroneo a un viaje no concluido
				//EN OTRA CIRCUNSTANCIA tambien envia libre sin datos
				VALOR_VIAJE = 0;
				ResetDatGps(LIB);
				Pase_a_LIBRE(CI_ENCENDIDO_EQUIPO);
				if(estadoReloj_en_corteAlimentacion != OCUPADO){
					//para impresion de ticket en libre recupero valor de viaje
					recuperar_valor_viaje_backup();
				}else{
					//para impresion de ticket
					//recupera el valor de viaje para imprimir ticket, porque a pesar de estar el
					//viaje inconcluso y no sabemos cuanto fue realmente el valor de viaje, lo que puede pasar
					//con mas frecuencia es que el viaje termine en ocupado pero porque se colgo la aplicacion
					//esta no lo pudo transmitir y apagaron el equipo, de este modo se imprime el ultimo tikect
					//de un viaje que no paso nunca a cobrando, pero probablemente haya terminado el viaje
					recuperar_valor_viaje_backup();
					viajeInconcluso = 1;
				}

			}
			if(error_noEsperado){
				VALOR_VIAJE = 0;
				ResetDatGps(LIB);
				Pase_a_LIBRE(CI_ENCENDIDO_EQUIPO);
			}
			quitar_asignado;
			ESTADO_RELOJ=LIBRE;
		}
	}

static byte corteCorto(uint16_t centralRx_cmdRLJ_cortePrevio){
	 byte error_noEsperado;
	 byte statusSC;

    // CORTE CORTO => Recuperar
	  //__HAL_TIM_SET_COUNTER(&pulsoACCUM,PULSE_ACCUM_CNT);
	  readIniPactado();
	  if((ESTADO_RELOJ == OCUPADO) || (ESTADO_RELOJ == COBRANDO)){
			  //__HAL_TIM_SET_COUNTER(&pulsoACCUM,PULSE_ACCUM_CNT);
			  //RELOJ_setTarifa(tarifa);        // Seteo automatico de tarifa
			  RELOJ_setTarifa(tarifa_1_8);
			  RELOJ_iniTarifacion();
			  //vuelvo a recuperar porque  RELOJ_iniTarifa me modifico algunas variables
			  read_backup_eeprom();

			  recuperar_valor_viaje_backup();
		      update_valor_tarifa(tarifa);
		      on_display_all();
			  //para el calculo de frecuencia
		      t_pulsos_anterior = 0;
			  t_pulsos = 0;
			  cntIC_anterior   = 0;
			  uint16_t MINUTOS = segundosTarifacion/60;
			  if(MINUTOS > 255){MINUTOS=255;}
			  minutosEspera = MINUTOS;

		  if(TARIFA_PESOS){
				  //muetra importe
				  update_valor_display(VALOR_VIAJE);
			  }else{
				  //muestra fichas
				  update_valor_display(fichas_xTiempo + fichas_xPulsos);
			  }
		  if(ESTADO_RELOJ==COBRANDO){
			 toggle_display();
		  }
		  ini_pulsador_impresion();

		  //si hubo corte largo previo debo transmitir estado de reloj
		  if((centralRx_cmdRLJ_cortePrevio>>8) == 0x0001){
				//doble corte
				fichasDPerdida = 0;
				fichasTPerdida = 0;
				dineroPerdido = 0;
				//recupero estado de reloj, ya que como hubo corte previo en al apagar guardo libre
				estadoReloj_en_corteAlimentacion = (byte)centralRx_cmdRLJ_cortePrevio&0x00ff;
				//seteo como que no recibio, para borrarla cuando recibe respuesta
				set_noRxCorteLargoPrevio(estadoReloj_en_corteAlimentacion);

				if(estadoReloj_en_corteAlimentacion != OCUPADO){
					//solo si hubo corte largo en ocupado
					//hubo importe perdido
					fichasDPerdida = 0;
					fichasTPerdida = 0;
					dineroPerdido = 0;
				}

				if(datosSC_cntWORD == 0){
					//no hay comandos sin conexion guardados en el apagado
					error_noEsperado = 0;
					if(estadoReloj_en_corteAlimentacion == COBRANDO){
						//SE APAGO EN COBRANDO y no hay comandos acumulados sin conexion, por lo tanto se debe enviar
						//el ultimo libre para que el sistema pueda cerrar el viaje
						statusSC = load_regViaje(nroCorrelativo_INTERNO);	//levantar datos de la tabla
						if(!statusSC){
							rearmar_y_TX_cmdLibre(nroCorrelativo_INTERNO);
							corregir_dateLIBRE();
							libreDATE = RTC_Date;
							VALOR_VIAJE = regVIAJE.importe;

						}else{
							error_noEsperado = 1;
						}
						tarifaPerdida = tarifa_1_8;
						fichasDPerdida = 0;
						fichasTPerdida = 0;
						dineroPerdido = 0;
					}else{
						//SI SE APAGO EN OCUPADO,  no hay comandos acumulados sin conexion, por lo tanto no se tiene informacion
						//de como concluyo el viaje. Se transmite un libre sin datos, y es particularmente importante que el importe
						//enviado en este caso sea cero, para no asignar un importe erroneo a un viaje no concluido
						//EN OTRA CIRCUNSTANCIA tambien envia libre sin datos
						VALOR_VIAJE = 0;
						ResetDatGps(LIB);
						Pase_a_LIBRE(CI_ENCENDIDO_EQUIPO);
						if(estadoReloj_en_corteAlimentacion != OCUPADO){
							//para impresion de ticket en libre recupero valor de viaje
							//ocupado dejo VALOR_VIAJE = 0;
							recuperar_valor_viaje_backup();

						}else{
							//para que imprima todo ceros en el ticket de viaje
							//fichas_xPulsos = 0;
							//fichas_xTiempo = 0;
							//DISTANCIAm_OCUPADO = 0;
							//VALOR_VIAJE = 0;

							//para impresion de ticket
							//recupera el valor de viaje para imprimir ticket, porque a pesar de estar el
							//viaje inconcluso y no sabemos cuanto fue realmente el valor de viaje, lo que puede pasar
							//con mas frecuencia es que el viaje termine en ocupado pero porque se colgo la aplicacion
							//esta no lo pudo transmitir y apagaron el equipo, de este modo se imprime el ultimo tikect
							//de un viaje que no paso nunca a cobrando, pero probablemente haya terminado el viaje
							recuperar_valor_viaje_backup();
							viajeInconcluso = 1;
						}

					}
					if(error_noEsperado){
						VALOR_VIAJE = 0;
						ResetDatGps(LIB);
						Pase_a_LIBRE(CI_ENCENDIDO_EQUIPO);
					}
				}
		  }
	  }else{
		  if(ESTADO_RELOJ == LIBRE){
			  BanderaOut_On();
			  //para impresion de ticket en libre recupero valor de viaje
			  read_backup_eeprom();
				  recuperar_valor_viaje_backup();

		  }
	  }
}


void writeEepromDATE(tDATE date){
	 byte auxDate[7];

	 auxDate[0] = date.fecha[0];
	 auxDate[1] = date.fecha[1];
	 auxDate[2] = date.fecha[2];
	 auxDate[3] = date.fecha[3];
	 auxDate[4] = date.hora[0];
	 auxDate[5] = date.hora[1];
	 auxDate[6] = date.hora[2];

	 EEPROM_WriteBuffer((uint8_t*) &auxDate, EEPROM_libreDATE, sizeof(tDATE));
}


void readEepromDATE(tDATE* date){

	byte auxDate[7];

	 EEPROM_ReadBuffer(&auxDate, EEPROM_libreDATE, sizeof(tDATE));

	 date->fecha[0] = auxDate[0];
	 date->fecha[1] = auxDate[1];
	 date->fecha[2] = auxDate[2];
	 date->fecha[3] = auxDate[3];
	 date->hora[0]  = auxDate[4];
	 date->hora[1]  = auxDate[5];
	 date->hora[2]  = auxDate[6];
}

void borroCorteALIMENTACION(void){

	byte buffer_backup[1];

	//borro corteALIMENTACION
	buffer_backup[0]=0;
	EEPROM_WriteBuffer((uint8_t*) &buffer_backup,ADDRESS_BACKUP_EEPROM,1);
}

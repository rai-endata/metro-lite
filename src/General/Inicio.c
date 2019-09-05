
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

tDATE HoraApagado;
byte motivo_reset;
inicioFLAGS1   _inicio_F1;
byte 		corteALIMENTACION;

static byte tarifaPerdida;
static dword fichasDPerdida;
static dword fichasTPerdida;
static dword dineroPerdido;


// VARIABLES DE FLASH

void SOURCE_DATE_Ini (void){
    // Inicializacion de la FUENTE de la fecha y la hora.
    // Si esta con GPS, sera del GPS, sino del RTC
    if (HORA_GPS){
      DATE_ptr = &GPS.Date;

    }else if (HORA_RTC){
      DATE_ptr = &RTC_Date;
    }
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
    	  getDate();
    	  if (!INICIO_firstDate && (DATE_ptr->fecha[0] != 0) && ((HORA_GPS && GPS.valid) || HORA_RTC) /*&& !PROGRAMADOR_chkReset()*/){
           // Si todavia no tengo el primer date y la fecha NO ES cero => Ya tengo el primer
           // date
           INICIO_firstDate = 1;                         // Seteo Bandera
           //guarda estado inicial de bloqueo de equipo

           //if (!INICIO_microCorte){
           //  RELOJ_tomarDateCambioReloj_Arranque();      // Hora de cambio de Reloj (solo si NO ES uCorte)
           //}

         #ifdef VISOR_REPORTES
           if (VISOR_firstBoot || REPORTES_chkForcedReset()){
             // EDIT 22/02/13
             //  En caso de un reset forzado de Reportes, tambien debo iniciar la primer sesion del chofer
             CHOFER_firstBoot();                         // En el primer arranque debo grabar un inicio de Sesion con CHF #1
           }
         #else
           if (VISOR_firstBoot){
             // EDIT 22/02/13
             //  En caso de un reset forzado de Reportes, tambien debo iniciar la primer sesion del chofer
             CHOFER_firstBoot();                         // En el primer arranque debo grabar un inicio de Sesion con CHF #1
           }
         #endif

       #ifdef VISOR_REPORTES
         #ifndef VISOR_TUNEADO_NO_BORRA_REPORTES
           if (REPORTES_HABILITADOS){
             if (!VISOR_firstBoot && !INICIO_microCorte){
              /*
               if (INICIO_programacion){
                 // PROGRAMACION
                 (void)REPORTE_queueVarios(getDate(), RELOJ_INTERNO_getChofer(), 0, progTaller); // Encolo Programacion en TALLER
                  //inicializa desconexiones
               }else{
                 // DESCONEXION
                 (void)REPORTE_queueDesconexionAlim(HoraApagado, RELOJ_INTERNO_getChofer(), tarifaPerdida, fichasDPerdida, fichasTPerdida, dineroPerdido);

              }
              */
       	  	  if(corteALIMENTACION==1){
                  // DESCONEXION
                  (void)REPORTE_queueDesconexionAlim(HoraApagado, RELOJ_INTERNO_getChofer(), tarifaPerdida, fichasDPerdida, fichasTPerdida, dineroPerdido);
       	  	  }
             }
           }
         #endif
       #endif

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

       	uint16_t primerEncendido;
       	uint16_t PRUEBA;

       	EEPROM_ReadBuffer(&primerEncendido,EEPROM_PRIMER_ENCENDIDO,SIZE_EEPROM_PRIMER_ENCENDIDO);
       	if ( primerEncendido == valorPrimerEncendido ){
       		// No se trata del primer encendido
       	  VISOR_firstBoot = 0;
       	}else{
       	  // Se trata del primer encendido
       	  VISOR_firstBoot = 1;
       	  EEPROM_write(EEPROM_PRIMER_ENCENDIDO, valorPrimerEncendido); // Grabar Word en EEPROM
       	  EEPROM_ReadBuffer(&PRUEBA,EEPROM_PRIMER_ENCENDIDO,SIZE_EEPROM_PRIMER_ENCENDIDO);

       	}
       }

/*
void inicio(void){

    byte* addressFLASH_cobrandoDATE;
    byte* addressFLASH_ocupadoDATE;
    byte* address_ocupadoDATE;
    byte* address_cobrandoDATE;

		  INICIO_microCorte = 0;
		  get_backup_HoraApagado();
		  INICIO_microCorte = diferenciaHoraria(RTC_Date.hora, HoraApagado.hora,15);
	      // ACCIONES SEGUN TIPO DE CORTE
	      if (INICIO_microCorte){
	        // CORTE CORTO => Recuperar
	    	  get_backup();   //para saber en que estado esta                       							//Extraigo backup de RTC
	    	  __HAL_TIM_SET_COUNTER(&pulsoACCUM,PULSE_ACCUM_CNT);
	    	  if((ESTADO_RELOJ == OCUPADO) || (ESTADO_RELOJ == COBRANDO)){

	    		  Reloj_iniTarifa(tarifa);
				  RELOJ_iniTarifacion();
				  get_backup();					//vuelvo a recuperar porque  RELOJ_iniTarifa me modifico algunas variables
				  recuperar_valor_viaje_backup();
		    	  update_valor_tarifa(tarifa);
		    	  on_display_all();
				  //para el calculo de frecuencia
		    	  t_pulsos_anterior = 0;
				  t_pulsos = 0;
		  		  cntIC_anterior   = 0;
		  		  //recupera segundos de espera y de tarifacion
		  		segundosTarifacion = fichas_xTiempo*TIEMPO_FICHA_INICIAL;
		  		segundosEspera =	segundosTarifacion +  dif_espera; //estimo a la mitad
		  		minutosEspera = segundosTarifacion/60;

		          addressFLASH_ocupadoDATE = &FLASH_ocupadoDATE;
		          address_ocupadoDATE = &ocupadoDATE;

		          *(address_ocupadoDATE+0) = *(addressFLASH_ocupadoDATE+0);
		          *(address_ocupadoDATE+1) = *(addressFLASH_ocupadoDATE+1);
		          *(address_ocupadoDATE+2) = *(addressFLASH_ocupadoDATE+2);
		          *(address_ocupadoDATE+3) = *(addressFLASH_ocupadoDATE+3);
		          *(address_ocupadoDATE+4) = *(addressFLASH_ocupadoDATE+4);
		          *(address_ocupadoDATE+5) = *(addressFLASH_ocupadoDATE+5);
		          *(address_ocupadoDATE+6) = *(addressFLASH_ocupadoDATE+6);

		    	  if(MOSTRAR_IMPORTE){
					  //muetra importe
					  update_valor_display(VALOR_VIAJE);
				  }else{
					  //muetra fichas
					  update_valor_display(fichas_xTiempo + fichas_xPulsos);
				  }
	    	  }
	    	  if(ESTADO_RELOJ==COBRANDO){
	    		 toggle_display();

	    		 addressFLASH_cobrandoDATE = &FLASH_cobrandoDATE;
		         address_cobrandoDATE = &cobrandoDATE;

		         *(address_cobrandoDATE+0) = *(addressFLASH_cobrandoDATE+0);
		         *(address_cobrandoDATE+1) = *(addressFLASH_cobrandoDATE+1);
		         *(address_cobrandoDATE+2) = *(addressFLASH_cobrandoDATE+2);
		         *(address_cobrandoDATE+3) = *(addressFLASH_cobrandoDATE+3);
		         *(address_cobrandoDATE+4) = *(addressFLASH_cobrandoDATE+4);
		         *(address_cobrandoDATE+5) = *(addressFLASH_cobrandoDATE+5);
		         *(address_cobrandoDATE+6) = *(addressFLASH_cobrandoDATE+6);
	    	  }



	      }else{
	        // CORTE LARGO

	      }
}
*/


void check_corte_alimentacion(void){

    byte* addressFLASH_cobrandoDATE;
    byte* addressFLASH_ocupadoDATE;
    byte* address_ocupadoDATE;
    byte* address_cobrandoDATE;
    byte buffer_backup[1];


		  INICIO_microCorte = 0;

		  read_horaAPAGADO_eeprom();
		  //rtc__actDATE();
	      getDate();
		  read_backup_eeprom();
		  INICIO_microCorte = diferenciaHoraria(RTC_Date.hora, HoraApagado.hora,16);
	      //ACCIONES SEGUN TIPO DE CORTE
	      if(INICIO_microCorte){
	          // CORTE CORTO => Recuperar
	    	  __HAL_TIM_SET_COUNTER(&pulsoACCUM,PULSE_ACCUM_CNT);
	    	  if((ESTADO_RELOJ == OCUPADO) || (ESTADO_RELOJ == COBRANDO)){

	    		  //Reloj_iniTarifa(tarifa);
				  //RELOJ_iniTarifacion();
	    		  __HAL_TIM_SET_COUNTER(&pulsoACCUM,PULSE_ACCUM_CNT);
	    		  //RELOJ_setTarifa(tarifa);        // Seteo automatico de tarifa
	    		  RELOJ_setTarifa(tarifa_1_8);
				  RELOJ_iniTarifacion();
		  	      //RLJ_INI();
				  //vuelvo a recuperar porque  RELOJ_iniTarifa me modifico algunas variables
				  read_backup_eeprom();

				  recuperar_valor_viaje_backup();
		    	  update_valor_tarifa(tarifa);
		    	  on_display_all();
				  //para el calculo de frecuencia
		    	  t_pulsos_anterior = 0;
				  t_pulsos = 0;
		  		  cntIC_anterior   = 0;
		  		  minutosEspera = segundosTarifacion/60;

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

	    	  }else{
					//viaje perdido
					tarifaPerdida = 0;
					fichasDPerdida = 0;
					fichasTPerdida = 0;
					dineroPerdido = 0;
	    		  //arranco como si fuera CORTE LARGO
	  	  		//ESTADO_RELOJ=FUERA_SERVICIO;
	  	  		ESTADO_RELOJ=LIBRE;
	  	  		Pase_a_LIBRE();
	  	  		setTEST_DISPLAY();
	    	  }
	      }else{
	        // CORTE LARGO
	  		//ESTADO_RELOJ=FUERA_SERVICIO;
			//viaje perdido
			//viaje perdido
	    	RELOJ_setTarifa(tarifa_1_8);
	    	recuperar_valor_viaje_backup();
			tarifaPerdida = tarifa_1_8;
			fichasDPerdida = fichas_xPulsos;
			fichasTPerdida = fichas_xTiempo;;
			dineroPerdido = VALOR_VIAJE;

	    	ESTADO_RELOJ=LIBRE;
	  		Pase_a_LIBRE();
	  		setTEST_DISPLAY();
	      }
	      //leo valor de corteALIMETACION
	      //EEPROM_ReadBuffer((uint8_t*) &corteALIMENTACION,ADDRESS_BACKUP_EEPROM,1);
	      if(corteALIMENTACION!=0){
		      //borro valor de corte de alimentacion (este byte lo uso para saber si hubo o no corte)
		      //corteALIMENTACION = 0 //reset sin corte de alimentacion
		      //corteALIMENTACION = 1 //reset luego de un corte de alimentacion
		      buffer_backup[0]=0;
		      EEPROM_WriteBuffer((uint8_t*) &buffer_backup,ADDRESS_BACKUP_EEPROM,1);
	      }
}

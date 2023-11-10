/*
 * rtc.c
 *
 *  Created on: 4/10/2017
 *      Author: Rai
 */
#include "stm32f0xx_hal_def.h"
#include "stm32f0xx_hal_rtc.h"

#include "Definicion de tipos.h"
//#include "stm32f429i_discovery.h"
//#include "- Reloj-Blue Config -.h"
#include "- metroBLUE Config -.h"
#include "main.h"
#include "rtc.h"
//#include "Param Reloj.h"
#include "Parametros Reloj.h"
#include "Tarifacion Reloj.h"
#include "Reloj.h"
#include "Odometro.h"
#include "Timer.h"

tDATE RTC_Date;
tDATE DateApagado;                        // Fecha y Hora de APAGADO

tDATE* DATE_ptr;                 // Puntero para fuente de FECHA y HORA
tDATE HoraApagado;

	/* HORA VALIDA ? */
	/*****************/
	void checkTime(void){
		if(checkTIME == 0){
			  checkTIME = 60;
			  getDate();
			  if(RTC_Date.fecha[2]==0x00){
				 set_TIMEandDATE();
				 getDate();
			  }
		}
	}

/* EXTRAER DATE */
 /****************/
   tDATE getDate (void){
     // Devuelve la fecha y hora (DATE) actual dependiendo de la fuente que se haya
     // seleccionado en la programacion
	   if (HORA_RTC){
		   rtc__actDATE();
	   }
	   return(*DATE_ptr);
   }


void rtc__actDATE (void){
	RTC_TimeTypeDef stm32_time;
	RTC_DateTypeDef stm32_date;
    uint16_t aux;

	if(HAL_RTC_GetTime(&hrtc, &stm32_time, RTC_FORMAT_BCD)==HAL_OK){
		RTC_Date.hora[0] = stm32_time.Hours;       // Actualizo HORA
		RTC_Date.hora[1] = stm32_time.Minutes;     // Actualizo MINUTOS
		RTC_Date.hora[2] = stm32_time.Seconds;     // Actualizo SEGUNDOS
	}else{
		RTC_Date.hora[0] = 0xff;       // Actualizo HORA
		RTC_Date.hora[1] = 0xff;     // Actualizo MINUTOS
		RTC_Date.hora[2] = 0xff;     // Actualizo SEGUNDOS
	}
	if(HAL_RTC_GetDate(&hrtc, &stm32_date, RTC_FORMAT_BCD)==HAL_OK){
		RTC_Date.fecha[0] =	stm32_date.Date;      // Actualizo dia del mes
		RTC_Date.fecha[1] =	stm32_date.Month;     // Actualizo MES
		RTC_Date.fecha[2] =	stm32_date.Year;      // Actualizo AÑO
		RTC_Date.fecha[3] =	stm32_date.WeekDay;   // Actualizo dia de semana
	}else{
		RTC_Date.fecha[0] =	0xff;      // Actualizo dia del mes
		RTC_Date.fecha[1] =	0xff;     // Actualizo MES
		RTC_Date.fecha[2] =	0x00;      // Actualizo AÑO
		RTC_Date.fecha[3] =	0xff;   // Actualizo dia de semana
	}

}



void test_backup_registers(void){

	write_backup_registers();
	read_backup_registers();

}


void write_backup_registers(void){
	uint32_t j;
	uint32_t k;

	//guarda 5 valores
	j=0; k=1000;
	while(j<5){
		rtc_write_backup_reg(j, k);
		j++; k++;
	}

}

void write_backup_rtc(void){

	uint32_t V[5];
	tBACKUP_RTC* ptr_backup;

		ptr_backup =(tBACKUP_RTC*) &V;

		//guarda hora apagado
		ptr_backup->backupRTC.dia 	= RTC_Date.fecha[0];
	 	ptr_backup->backupRTC.mes   = RTC_Date.fecha[1];
		ptr_backup->backupRTC.anio 	= RTC_Date.fecha[2];
		ptr_backup->backupRTC.hora    = RTC_Date.hora[0];
		ptr_backup->backupRTC.minuto  = RTC_Date.hora[1];
		ptr_backup->backupRTC.segundo = RTC_Date.hora[2];

		ptr_backup->backupRTC.tarifa = tarifa;
		ptr_backup->backupRTC.estado_reloj = ESTADO_RELOJ;
		ptr_backup->backupRTC.dif_espera = dif_espera;

		ptr_backup->backupRTC.fichas_pulsos = fichas_xPulsos;
		ptr_backup->backupRTC.fichas_tiempo = fichas_xTiempo;

		ptr_backup->backupRTC.contador_pulsos = PULSE_ACCUM_CNT;
		ptr_backup->backupRTC.contador_pulsos_old = pulsos_cnt_old;

		rtc_write_backup_reg(0, V[0]);
		rtc_write_backup_reg(1, V[1]);
		rtc_write_backup_reg(2, V[2]);
		rtc_write_backup_reg(3, V[3]);
		//en cobrando la variacion de distancia solo me sirve para la carrera de bandera y esto
		//puedo calcularlo guaradandoo solo PULSE_ACCUM_CNT y haciendo pulsos_cnt_old = 0 y PULSE_ACCUM_CNT = 0
		//en el pase a cobrando. De este modo me queda un lugar para guardar la distancia en caso de microtcorte
		if(ESTADO_RELOJ!=COBRANDO){
			rtc_write_backup_reg(4, DISTANCIAm_OCUPADO);
		}

		if(ESTADO_RELOJ==COBRANDO){
			rtc_write_backup_reg(4, DISTANCIAm_OCUPADO);
		}else{
			rtc_write_backup_reg(4, V[4]);
		}
}


void get_backup(void){

	uint32_t V[5];
	tBACKUP_RTC* ptr_backup;

    //paso los datos guardadados de los registros del rtc
	//a la memoria ram del micro
	V[1]= rtc_read_backup_reg(1);
	V[2]= rtc_read_backup_reg(2);
	V[3]= rtc_read_backup_reg(3);
	V[4]= rtc_read_backup_reg(4);
    //paso cada dato a su correspondiente variable
	ptr_backup =(tBACKUP_RTC*) &V;			//apunto al vector que contiene los datos
	tarifa = ptr_backup->backupRTC.tarifa;
	ESTADO_RELOJ = ptr_backup->backupRTC.estado_reloj;
	ESTADO_RELOJ_X_PULSADOR = ESTADO_RELOJ;
	dif_espera = (uint16_t)ptr_backup->backupRTC.dif_espera;

	fichas_xPulsos = ptr_backup->backupRTC.fichas_pulsos;
	fichas_xTiempo = ptr_backup->backupRTC.fichas_tiempo;
	PULSE_ACCUM_CNT = ptr_backup->backupRTC.contador_pulsos;
	if(ESTADO_RELOJ==COBRANDO){
		DISTANCIAm_OCUPADO = ptr_backup->backupRTC.contador_pulsos_old;
		pulsos_cnt_old = 0;
	}else{
		pulsos_cnt_old = ptr_backup->backupRTC.contador_pulsos_old;
	}
}


void write_backup_rtcCONTADOR_PULSOS(void){

	uint32_t V[5];
	tBACKUP_RTC* ptr_backup;
	uint32_t prim;

		prim = __get_PRIMASK();
		__disable_irq();

		ptr_backup =(tBACKUP_RTC*) &V;

		ptr_backup->backupRTC.contador_pulsos = PULSE_ACCUM_CNT;
		ptr_backup->backupRTC.contador_pulsos_old = pulsos_cnt_old;

		rtc_write_backup_reg(3, V[3]);
		rtc_write_backup_reg(4, V[4]);

		if (!prim) {
			__enable_irq();
		}

}

void clean_backup_rtc(void){
		rtc_write_backup_reg(0, 0x00000000);
		rtc_write_backup_reg(1, 0x00000000);
		rtc_write_backup_reg(2, 0x00000000);
		rtc_write_backup_reg(3, 0x00000000);
		rtc_write_backup_reg(4, 0x00000000);
}


void read_backup_registers(void){
	uint32_t j;
	uint32_t V[5];
	byte A[10];
	uint32_t B[10];
	tBACKUP_RTC* ptr_backup;
	//recupera los 5 valores

	ptr_backup =(tBACKUP_RTC*) &V;


	V[0]= rtc_read_backup_reg(0);
	V[1]= rtc_read_backup_reg(1);
	V[2]= rtc_read_backup_reg(2);
	V[3]= rtc_read_backup_reg(3);
	V[4]= rtc_read_backup_reg(4);


/*
//tiempo (dia, segundo, minuto hora, )
           01   02        03    04
//vel-max  tarifa - estado-reloj  año  mes
   05         07     04            07  08



//fichas-pulsos
0203
//fichas-tiempo
0901
contador
04050607
contador old
08090102
 */

	HoraApagado.fecha[0] = ptr_backup->backupRTC.dia;;
	HoraApagado.fecha[1] = ptr_backup->backupRTC.mes;;
	HoraApagado.fecha[2] = ptr_backup->backupRTC.anio;
	HoraApagado.hora[0] = ptr_backup->backupRTC.hora;
	HoraApagado.hora[1] = ptr_backup->backupRTC.minuto;
	HoraApagado.hora[2] = ptr_backup->backupRTC.segundo;
	/*
	A[0] = ptr_backup->backupRTC.hora;
	A[1] = ptr_backup->backupRTC.minuto;
	A[2] = ptr_backup->backupRTC.segundo;
	A[3] = ptr_backup->backupRTC.dia;
	A[4] = ptr_backup->backupRTC.mes;
	A[5] = ptr_backup->backupRTC.anio;

	A[6] = ptr_backup->backupRTC.tarifa;
	A[7] = ptr_backup->backupRTC.estado_reloj;
	A[8] = ptr_backup->backupRTC.velocidad_max;

	B[0] = ptr_backup->backupRTC.fichas_pulsos;
	B[1] = ptr_backup->backupRTC.fichas_tiempo;
	B[2] = ptr_backup->backupRTC.contador_pulsos;
	B[3] = ptr_backup->backupRTC.contador_pulsos_old;
*/
}

void get_backup_HoraApagado(void){

	uint32_t V[2];
	tBACKUP_RTC* ptr_backup;

	V[0]= rtc_read_backup_reg(0);
	V[1]= rtc_read_backup_reg(1);

	ptr_backup =(tBACKUP_RTC*) &V;

	HoraApagado.fecha[0] = ptr_backup->backupRTC.dia;;
	HoraApagado.fecha[1] = ptr_backup->backupRTC.mes;;
	HoraApagado.fecha[2] = ptr_backup->backupRTC.anio;
	HoraApagado.hora[0] = ptr_backup->backupRTC.hora;
	HoraApagado.hora[1] = ptr_backup->backupRTC.minuto;
	HoraApagado.hora[2] = ptr_backup->backupRTC.segundo;
}


uint32_t rtc_read_backup_reg(uint32_t BackupRegister) {
    RTC_HandleTypeDef RtcHandle;
    RtcHandle.Instance = RTC;
    return HAL_RTCEx_BKUPRead(&RtcHandle, BackupRegister);
}

void rtc_write_backup_reg(uint32_t BackupRegister, uint32_t data) {
    RTC_HandleTypeDef RtcHandle;
    RtcHandle.Instance = RTC;
    HAL_PWR_EnableBkUpAccess();
    HAL_RTCEx_BKUPWrite(&RtcHandle, BackupRegister, data);
    HAL_PWR_DisableBkUpAccess();
}

/* ACTUALIZAR FECHA y HORA DE RTC */
 /**********************************/
   void RTC_updateDate (byte* new_date){
		// Rutina que actualiza los valores de HORA y FECHA en el RTC.
		// El formato de new_date es:;
		//    DIA - MES - AÑO - HORA - MIN - SEG
		byte aux;
		RTC_TimeTypeDef sTime;
		RTC_DateTypeDef sDate;

		aux = new_date[3];             // Extraigo la Nueva Hora

        //if (aux != RTC_Date.hora[0]){

     	sDate.Date = new_date[0];
     	sDate.Month = new_date[1];
     	sDate.Year = new_date[2];

     	//CALCULO DIA SEMANA
      	sDate.WeekDay = determineDiaSemana(&new_date[0]);
   		if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
 		{
 			//_Error_Handler(__FILE__, __LINE__);
 			//MX_RTC_Init();
 		}

     	sTime.Hours = new_date[3];
     	sTime.Minutes = new_date[4];
     	sTime.Seconds = new_date[5];
     	sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
     	sTime.StoreOperation = RTC_STOREOPERATION_RESET;
		if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
 		{
 			//_Error_Handler(__FILE__, __LINE__);
 			//MX_RTC_Init();
 		}




   //  }

   }



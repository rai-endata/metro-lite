/*
 * eeprom.c
 *
 *  Created on: 5/5/2018
 *      Author: Rai
 */
//#include "eeprom_aux.h"
#include "eeprom.h"

#include "rtc.h"
#include "Tarifacion Reloj.h"
#include "Reloj.h"
#include "Odometro.h"
#include "main.h"
#include "DTE - Tx.h"
#include "Ticket VIaje.h"
#include "inicio.h"

//tEEPROM dataEE;


void write_backup_eeprom(byte motivoBackup){

	//motivoBackup
	//0: se colgo el equipo (HardFault_Handler)
	//1: se desconecto la alimentacion del equipo ( HAL_PWR_PVDCallback)
    //1 guarda desconexion de alimentacion para reportar en ticket de turno
    //motivo backup lo guardo en primer LSB
	//ultimo viaje inconcluos lo guardo en segundo LSB

	byte* ptrDIR;
	uint8_t buffer_backup[128];
	uint8_t aux;

    uint8_t i=0;

    aux = motivoBackup;
    if(viajeInconcluso){
      aux = aux | 0x2;
    }
    //guardo corteALIMENTACION
	buffer_backup[i] = aux; i++;
	buffer_backup[i] = tarifa_1_8; i++;
	buffer_backup[i] = ESTADO_RELOJ; i++;

	//BYTE_TO_FILL1
	i++;

	ptrDIR = (uint8_t*)&VELOCIDAD_MAX;
	buffer_backup[i] = *(ptrDIR+1); i++;
	buffer_backup[i] = *(ptrDIR+0); i++;

	ptrDIR = (uint8_t*)&segundosEspera;
	buffer_backup[i] = *(ptrDIR+1); i++;
	buffer_backup[i] = *(ptrDIR+0); i++;

	ptrDIR = (uint8_t*)&segundosTarifacion;
	buffer_backup[i] = *(ptrDIR+1); i++;
	buffer_backup[i] = *(ptrDIR+0); i++;

	//WORD_TO_FILL1
	i++;  i++;

	ptrDIR = (uint8_t*)&fichas_xPulsos;
	buffer_backup[i] = *(ptrDIR+3); i++;
	buffer_backup[i] = *(ptrDIR+2); i++;
	buffer_backup[i] = *(ptrDIR+1); i++;
	buffer_backup[i] = *(ptrDIR+0); i++;

	ptrDIR = (uint8_t*)&fichas_xTiempo;
	buffer_backup[i] = *(ptrDIR+3); i++;
	buffer_backup[i] = *(ptrDIR+2); i++;
	buffer_backup[i] = *(ptrDIR+1); i++;
	buffer_backup[i] = *(ptrDIR+0); i++;

	ptrDIR = (uint8_t*)&PULSE_ACCUM_CNT;
	buffer_backup[i] = *(ptrDIR+3); i++;
	buffer_backup[i] = *(ptrDIR+2); i++;
	buffer_backup[i] = *(ptrDIR+1); i++;
	buffer_backup[i] = *(ptrDIR+0); i++;

	ptrDIR = (uint8_t*)&pulsos_cnt_old;
	buffer_backup[i] = *(ptrDIR+3); i++;
	buffer_backup[i] = *(ptrDIR+2); i++;
	buffer_backup[i] = *(ptrDIR+1); i++;
	buffer_backup[i] = *(ptrDIR+0); i++;

	ptrDIR = (uint8_t*)&DISTANCIAm_OCUPADO;
	buffer_backup[i] = *(ptrDIR+3); i++;
	buffer_backup[i] = *(ptrDIR+2); i++;
	buffer_backup[i] = *(ptrDIR+1); i++;
	buffer_backup[i] = *(ptrDIR+0); i++;

	buffer_backup[i] = ocupadoDATE.fecha[0];  i++;
	buffer_backup[i] = ocupadoDATE.fecha[1];  i++;
	buffer_backup[i] = ocupadoDATE.fecha[2];  i++;
	buffer_backup[i] = ocupadoDATE.fecha[3];  i++;
	buffer_backup[i] = ocupadoDATE.hora[0];   i++;
	buffer_backup[i] = ocupadoDATE.hora[1];   i++;
	buffer_backup[i] = ocupadoDATE.hora[2];   i++;

	buffer_backup[i] = cobrandoDATE.fecha[0];  i++;
	buffer_backup[i] = cobrandoDATE.fecha[1];  i++;
	buffer_backup[i] = cobrandoDATE.fecha[2];  i++;
	buffer_backup[i] = cobrandoDATE.fecha[3]; i++;
	buffer_backup[i] = cobrandoDATE.hora[0];   i++;
	buffer_backup[i] = cobrandoDATE.hora[1];   i++;
	buffer_backup[i] = cobrandoDATE.hora[2];   i++;

	//guarda hora apagado
	getDate();
	buffer_backup[i] = RTC_Date.fecha[0]; i++;
	buffer_backup[i] = RTC_Date.fecha[1]; i++;
	buffer_backup[i] = RTC_Date.fecha[2]; i++;
	buffer_backup[i] = RTC_Date.fecha[3]; i++;
	buffer_backup[i] = RTC_Date.hora[0]; i++;
	buffer_backup[i] = RTC_Date.hora[1]; i++;
	buffer_backup[i] = RTC_Date.hora[2]; i++;

	EEPROM_WriteBuffer((uint8_t*) &buffer_backup,ADDRESS_BACKUP_EEPROM,SIZE_BACKUP_EEPROM);
}


void read_backup_eeprom(void){

	uint8_t* ptrDIR;
	uint8_t buffer_backup[128];
    uint8_t i=0;
    uint8_t aux;

    uint32_t addr;
    uint32_t address_eeprom;
    address_eeprom = ADDRESS_BACKUP_EEPROM;

    	//me fijo si hubo corte de alimentacion
    	EEPROM_ReadBuffer((uint8_t*) &buffer_backup,ADDRESS_BACKUP_EEPROM,SIZE_BACKUP_EEPROM);

    	aux = buffer_backup[i]; i++;
    	corteALIMENTACION = aux & 0x1;
    	aux = aux & 0x2;
    	if(aux){
    	   viajeInconcluso = 1;
    	}
    	tarifa_1_8		  = buffer_backup[i]; i++;

    	if(tarifa_1_8 < 5){
		 tarifa_1_4 = tarifa_1_8;
		}else{
		 tarifa_1_4 = tarifa_1_8 - 4;
		}
		tarifa = tarifa_1_4;

    	update_valor_tarifa(tarifa);

    	ESTADO_RELOJ = buffer_backup[i]; i++;
    	ESTADO_RELOJ_X_PULSADOR = ESTADO_RELOJ;
    	//BYTE_TO_FILL1
   		i++;

    	ptrDIR = (uint8_t*)&VELOCIDAD_MAX;
    	*(ptrDIR+1) = buffer_backup[i]; i++;
    	*(ptrDIR+0) = buffer_backup[i]; i++;

    	ptrDIR = (uint8_t*)&segundosEspera;
    	*(ptrDIR+1) = buffer_backup[i]; i++;
    	*(ptrDIR+0) = buffer_backup[i]; i++;

    	ptrDIR = (uint8_t*)&segundosTarifacion;
    	*(ptrDIR+1) = buffer_backup[i]; i++;
    	*(ptrDIR+0) = buffer_backup[i]; i++;

    	//WORD_TO_FILL1
    	i++;  i++;

    	ptrDIR = (uint8_t*)&fichas_xPulsos;
    	*(ptrDIR+3) = buffer_backup[i]; i++;
    	*(ptrDIR+2) = buffer_backup[i]; i++;
    	*(ptrDIR+1) = buffer_backup[i]; i++;
    	*(ptrDIR+0) = buffer_backup[i]; i++;

    	ptrDIR = (uint8_t*)&fichas_xTiempo;
    	*(ptrDIR+3) = buffer_backup[i]; i++;
    	*(ptrDIR+2) = buffer_backup[i]; i++;
    	*(ptrDIR+1) = buffer_backup[i]; i++;
    	*(ptrDIR+0) = buffer_backup[i]; i++;

    	ptrDIR = (uint8_t*)&PULSE_ACCUM_CNT;
    	*(ptrDIR+3) = buffer_backup[i]; i++;
    	*(ptrDIR+2) = buffer_backup[i]; i++;
    	*(ptrDIR+1) = buffer_backup[i]; i++;
    	*(ptrDIR+0) = buffer_backup[i]; i++;

    	ptrDIR = (uint8_t*)&pulsos_cnt_old;
    	*(ptrDIR+3) = buffer_backup[i]; i++;
    	*(ptrDIR+2) = buffer_backup[i]; i++;
    	*(ptrDIR+1) = buffer_backup[i]; i++;
    	*(ptrDIR+0) = buffer_backup[i]; i++;

    	ptrDIR = (uint8_t*)&DISTANCIAm_OCUPADO;
    	*(ptrDIR+3) = buffer_backup[i]; i++;
    	*(ptrDIR+2) = buffer_backup[i]; i++;
    	*(ptrDIR+1) = buffer_backup[i]; i++;
    	*(ptrDIR+0) = buffer_backup[i]; i++;

    	ocupadoDATE.fecha[0] = buffer_backup[i];  i++;
    	ocupadoDATE.fecha[1] = buffer_backup[i];  i++;
    	ocupadoDATE.fecha[2] = buffer_backup[i];  i++;
    	ocupadoDATE.fecha[3] = buffer_backup[i];  i++;
    	ocupadoDATE.hora[0]  = buffer_backup[i];  i++;
    	ocupadoDATE.hora[1]  = buffer_backup[i];  i++;
    	ocupadoDATE.hora[2]  = buffer_backup[i];  i++;

    	cobrandoDATE.fecha[0] = buffer_backup[i];  i++;
    	cobrandoDATE.fecha[1] = buffer_backup[i];  i++;
    	cobrandoDATE.fecha[2] = buffer_backup[i];  i++;
    	cobrandoDATE.fecha[3] = buffer_backup[i];  i++;
    	cobrandoDATE.hora[0]  = buffer_backup[i];  i++;
    	cobrandoDATE.hora[1]  = buffer_backup[i];  i++;
    	cobrandoDATE.hora[2]  = buffer_backup[i];  i++;

    	//leo hora apagado
    	HoraApagado.fecha[0] = buffer_backup[i];  i++;
    	HoraApagado.fecha[1] = buffer_backup[i];  i++;
    	HoraApagado.fecha[2] = buffer_backup[i];  i++;
    	HoraApagado.fecha[3] = buffer_backup[i];  i++;
    	HoraApagado.hora[0]  = buffer_backup[i];  i++;
    	HoraApagado.hora[1]  = buffer_backup[i];  i++;
    	HoraApagado.hora[2]  = buffer_backup[i];  i++;
}




void read_horaAPAGADO_eeprom(void){

	uint8_t buffer_backup[SIZE_DATE_APAGADO_EEPROM];
    uint8_t i=0;

	//leo hora apagado desde eeprom
	EEPROM_ReadBuffer((uint8_t*) &buffer_backup,ADDRESS_DATE_APAGADO_EEPROM,SIZE_DATE_APAGADO_EEPROM);

	//guardo hora apagado en ram
	HoraApagado.fecha[0] = buffer_backup[i];  i++;
	HoraApagado.fecha[1] = buffer_backup[i];  i++;
	HoraApagado.fecha[2] = buffer_backup[i];  i++;
	HoraApagado.fecha[3] = buffer_backup[i];  i++;
	HoraApagado.hora[0]  = buffer_backup[i];  i++;
	HoraApagado.hora[1]  = buffer_backup[i];  i++;
	HoraApagado.hora[2]  = buffer_backup[i];
}

/*
void test_size(void){

	uint32_t addr1,addr2,addr3,addr4,addr5,addr6;
	uint32_t addr7,addr8,addr9,addr10,addr11,addr12;
	uint32_t addr13,addr14;

	uint16_t num1,num2,num3,num4,num5,num6,num7,num8,num9,num10,num11,num12,num13,num14,num15,num16;

	uint32_t size;

	uint32_t suma;

	num1  = SIZE_DATE_APAGADO_EEPROM;
	num2  = SIZE_TARIFA_EEPROM;
	num3  = SIZE_ESTADO_RELOJ_EEPROM;
	num4  = SIZE_FICHAS_PULSOS_EEPROM;
	num5  = SIZE_FICHAS_TIEMPO_EEPROM;
	num6  = SIZE_CONTADOR_PULSOS_EEPROM;
	num7  = SIZE_CONTADOR_PULSOS_OLD_EEPROM;
	num8  = SIZE_SEG_ESPERA_EEPROM;
	num9  = SIZE_SEG_TARIF_EEPROM;
	num10 = SIZE_DIS_OCUP_EEPROM;
	num11 = SIZE_OCUP_DATE_EEPROM;
	num12 = SIZE_COB_DATE_EEPROM;
	num13 = SIZE_VEL_MAX_EEPROM;
	num14 = SIZE_CORTE_ALIM_EEPROM;
	num15 = SIZE_BYTE_FILL1_EEPROM;
	num16 = SIZE_WORD_FILL1_EEPROM;

	size  = SIZE_BACKUP_EEPROM;
	suma = num1+num2+num3+num4+num5+num6+num7+num8+num9+num10+num11+num12+num13+num14+num15+num16;

	addr1 =	ADDRESS_DATE_APAGADO_EEPROM;
	addr2 = ADDRESS_TARIFA_EEPROM;
	addr3 = ADDRESS_ESTADO_RELOJ_EEPROM;
	addr4 = ADDRESS_FICHAS_PULSOS_EEPROM;
	addr5 = ADDRESS_FICHAS_TIEMPO_EEPROM;
	addr6 = ADDRESS_CONTADOR_PULSOS_EEPROM;
	addr7 = ADDRESS_CONTADOR_PULSOS_OLD_EEPROM;
	addr8 = ADDRESS_SEG_ESPERA_EEPROM;
	addr9 = ADDRESS_SEG_TARIF_EEPROM;
	addr10 =ADDRESS_DIS_OCUP_EEPROM;
	addr11 =ADDRESS_OCUP_DATE_EEPROM;
	addr12 =ADDRESS_COB_DATE_EEPROM;
	addr13 =ADDRESS_VEL_MAX_EEPROM;
	addr14 =ADDRESS_CORTE_ALIM_EEPROM;

}
*/
//RUTINAS IMPORTADAS DE PROYECTO ANTERIOR PARA COMPATIBILIDAD DE USO DE RUTINAS
//******************************************************************************

/*********************************************************************************************/
/* PROTOTIPOS */
/**************/
  // RUTINAS INTERNAS

/*********************************************************************************************/
/* VARIABLES */
/*************/
  static byte   EEPROM_request;         // Solicitud de grabacion de EEPROM

/*********************************************************************************************/
/* RUTINAS */
/***********/

  /* INICIALIZACION DE EEPROM */
  /****************************/
  /*
    void EEPROM_ini (byte initee){
    }
*/

  /* INICIAR GRABACION DE EEPROM */
  /*******************************/
    void EEPROM_iniGrabacion (void){
      // EDIT 15/06/2012
      // Ahora cuando pido iniciar una grabacion de EEPROM, no levanto las
      // banderas de habilitacion de IRQ (para grabar en el MISMO momento
      // que se solicita; sino que levanto una bandera interna y luego,
      // desde el LOOP y cuando se den las condiciones, disparo la grabacion
      // de EEPROM
      EEPROM_request = 1;
    }


  /* CHEQUEO DE GRABACION DE EEPROM */
  /**********************************/
    void EEPROM_chkRequest (byte force){
      // Llamada desde el LOOP, se fija si hay una solicitud de grabacion, y si se
      // cumplen las condiciones, habilita la grabacion de EEPROM
      //
      // EDIT 29/06/2012
      //  Se le agrego la posibilidad de forzar el grabado, sin tener que chequear
      //  las trabas
      //
      // EDIT 01/11/12
      //  Se agrego BUZZER_ringing como traba para grabar la EEPROM
      byte buzzerRinging;
      tEEPROM_ERROR error;

		if( (!DA_Txing ) && (huart1.Rx_TO_cnt == 0) && (huart7.Rx_TO_cnt == 0)){

			  if (EEPROM_request && (force )){
				EEPROM_request = 0;

			   // ECNFG_CCIE = 1;
			   //ECNFG_CBEIE = 1;
				//EEPROM_grabaciones();
				error = PROGRAMADOR_grabarEEPROM();         // Grabacion de Programacion en EEPROM

			  }
		}
    }
  //         word *prueba_1;
  //         byte prueba_2;

  /* GRABACION DE UN BYTE EN EEPROM */
  /**********************************/
 /*
    tEEPROM_ERROR grabar_byte_EEPROM (byte dato, uint16_t* EEPROM_ptr, byte mask){
      // Rutina que dada una variable en EEPROM, (4bytes) graba solamente 1 byte
      // de esa variable y el resto lo deja como esta. La mascara indica que
      // byte modificar.
      // El tipo de datos "tEEPROM_DATA" se compone de la siguiente manera, y
      // vemos al lado el valor de mascara asociado a cada uno:
      //
      //  byte1 --> mask = 1
      //  byte2 --> mask = 2
      //  byte3 --> mask = 3
      //  byte4 --> mask = 4
      tEEPROM_DATA valor_EEPROM;
      tEEPROM_ERROR error;

       tEEPROM_DATA valor_EEPROM_post;
       byte *ptr_byte;

      prueba_1=EEPROM_ptr;
      prueba_2=*EEPROM_ptr;

      error = EEPROM_OK;                    // Asumo que no hay error

      if ((mask > 4) || (mask == 0)){
        error = EEPROM_ERROR_MASK;          // Error en la mascara de Grabacion
      }

      if (!error){
          valor_EEPROM = *((tEEPROM_DATA*) EEPROM_ptr); // Leo el valor actual de la variable.

        if (mask == 1){
          valor_EEPROM.Bits.byte1 = dato;   // Actualizo Valor

        }else if (mask == 2){
          valor_EEPROM.Bits.byte2 = dato;   // Actualizo Valor

        }else if (mask == 3){
          valor_EEPROM.Bits.byte3 = dato;   // Actualizo Valor

        }else if (mask == 4){
          valor_EEPROM.Bits.byte4 = dato;   // Actualizo Valor
        }

        error = EEPROM_clear(EEPROM_ptr);                       // Borro Direccion de EEPROM donde voy a grabar los datos. Borro 2 WORDS

        ptr_byte = EEPROM_ptr;
        valor_EEPROM_post.Bits.byte1=*(ptr_byte+0);
        valor_EEPROM_post.Bits.byte2=*(ptr_byte+1);
        valor_EEPROM_post.Bits.byte3=*(ptr_byte+2);
        valor_EEPROM_post.Bits.byte4=*(ptr_byte+3);

        error = EEPROM_write(EEPROM_ptr, valor_EEPROM.MergedBits.word1);    // Grabar Word en EEPROM
        error = EEPROM_write(EEPROM_ptr + 1, valor_EEPROM.MergedBits.word2);// Grabar Word en EEPROM
      }

      return(error);
    }

*/

  /* GRABACION DE WORD EN EEPROM */
  /*******************************/

  /*
    tEEPROM_ERROR grabar_word_EEPROM (uint16_t dato, uint16_t* EEPROM_ptr, byte mask){
      // Rutina que dada una variable en EEPROM, (4bytes) graba solamente 2 bytes
      // (word) de esa variable y el resto lo deja como esta. La mascara indica que
      // word modificar.
      // El tipo de datos "tEEPROM_DATA" se compone de la siguiente manera, y
      // vemos al lado el valor de mascara asociado a cada uno:
      //
      //  word1 --> mask = 1
      //  word2 --> mask = 2
      tEEPROM_DATA valor_EEPROM;
      tEEPROM_ERROR error;

      error = EEPROM_OK;                          // Asumo que no hay error

      if ((mask > 2) || (mask == 0)){
        error = EEPROM_ERROR_MASK;                // Error en la mascara de Grabacion
      }

      if(!error){
          valor_EEPROM = *((tEEPROM_DATA*) EEPROM_ptr); // Leo el valor actual de la variable.

        if (mask == 1){
          valor_EEPROM.MergedBits.word1 = dato;   // Actualizo Valor

        }else if (mask == 2){
          valor_EEPROM.MergedBits.word2 = dato;   // Actualizo Valor
        }

        error = EEPROM_clear(EEPROM_ptr);                       // Borro Direccion de EEPROM donde voy a grabar los datos. Borro 2 WORDS

        error = EEPROM_write(EEPROM_ptr, valor_EEPROM.MergedBits.word1);      // Grabar Word en EEPROM
        error = EEPROM_write(EEPROM_ptr, valor_EEPROM.MergedBits.word2);      // Grabar Word en EEPROM
      }

      return(error);
    }
*/

  /* GRABACION DE UN BUFFER EN EEPROM */
  /************************************/
    tEEPROM_ERROR grabar_buffer_EEPROM (uint16_t* ptrRam, uint16_t* ptrEeprom, uint16_t max_size){
      // Graba un buffer en EEPROM. Para hacerlo, se le debe decir que buffer va a grabar
      // y en que direccion. Ademas, se le debe pasar la cantidad de bytes a grabar.
      // Esta cantidad debe ser multiplo de 4
      tEEPROM_ERROR error;
      uint16_t EEPROMsize_max;
      byte fin_datos;
      byte* fin_ptr;
      //byte page;
      //uint16_t copia[300];
      //word j;



        error = EEPROM_ACCESS_ERROR;                            // Asumo que no hay error

        EEPROMsize_max = EEPROM_PAGE_SIZE_BYTES;              // cantidad de bytes de una pagina

        fin_datos = 0;                              // Reseteo Variable
        fin_ptr = (byte*) ptrRam;
        while ((EEPROMsize_max != 0) && !(fin_datos)){

          EEPROMsize_max--;                         // Voy decrementando cantidad de 4bytes a grabar

          // chequeo condicion |DF|0A|XX|XX|
          if ((*fin_ptr == finEEPROM_H) && (*(fin_ptr+1) == finEEPROM_L)){
            fin_datos = 1;                          // Llegue al fin => Lo grabo y salgo
          }

          //error = EEPROM_clear(ptrEeprom);                 // Borro Direccion de EEPROM donde voy a grabar los datos. Borro 2 WORDS
          //page = (word)ptrEeprom/128;
          error = EEPROM_write(ptrEeprom, *ptrRam);   // Grabar Word en EEPROM

          //j = ((word)ptrEeprom - (128*3 + 172))/2;
          //copia[j] = *ptrRam;

          ptrEeprom++;                             		// Avanzo puntero de EEPROM
          ptrRam++;                            		// Avanzo Puntero de Datos


          if (!fin_datos){
            // chequeo condicion |XX|DF|0A|XX|
            fin_ptr++;
            if ((*fin_ptr == finEEPROM_H) && (*(fin_ptr+1) == finEEPROM_L)){
              fin_datos = 1;                        // Llegue al fin => Lo grabo y salgo
            }

            // chequeo condicion |XX|XX|DF|0A|
            fin_ptr++;
            if ((*fin_ptr == finEEPROM_H) && (*(fin_ptr+1) == finEEPROM_L)){
              fin_datos = 1;                        // Llegue al fin => Lo grabo y salgo
            }
            error = EEPROM_write(ptrEeprom, *ptrRam); // Grabar Word en EEPROM

            //j = ((word)ptrEeprom - (128*3 + 172))/2;
            //copia[j] = *ptrRam;

            ptrEeprom++;                           // Avanzo puntero de EEPROM
            ptrRam++;                          // Avanzo Puntero de Datos

            // chequeo condicion |XX|XX|XX|DF||0A|
            fin_ptr++;
            if ((*fin_ptr == finEEPROM_H) && (*(fin_ptr+1) == finEEPROM_L)){
              // Llegue al fin=> no hago nada. Recien grabé |XX|DF| => dejo el puntero
              // donde esta para que detecte el fin, grabe |0A|XX|00|00| y salga
            }else{
              // Como no encontre fin, avanzo puntero para seguir buscandolo
              fin_ptr++;
            }

          }else{
            // Si llegue al fin de datos, en el word anterior, ahora graba CEROS para rellenar el
            // word que falta para completar los 4 bytes
            error = EEPROM_write(ptrEeprom,(word) 0x0000); // Grabar Word en EEPROM

           // j = (((word)ptrEeprom - (128*3 + 172)))/2;
           // copia[j] = *ptrRam;

          }
        }

        __NOP();
        __NOP();
        __NOP();
      return(error);
    }


    /* GRABACION DE UN BUFFER EN EEPROM */
      /************************************/
        tEEPROM_ERROR grabar_buffer_EEPROM_TICKET (uint16_t* ptrRam, uint16_t* ptrEeprom, uint16_t max_size){
          // Graba un buffer en EEPROM. Para hacerlo, se le debe decir que buffer va a grabar
          // y en que direccion. Ademas, se le debe pasar la cantidad de bytes a grabar.
          // Esta cantidad debe ser multiplo de 4
          tEEPROM_ERROR error1;
          error_t error;

          //uint16_t EEPROMsize_max;
          //byte fin_datos;
          //byte* fin_ptr;
          //byte page;
          //uint16_t copia[300];
          //word j;



			error = EEPROM_WriteBuffer((uint8_t*) ptrRam, ptrEeprom, max_size);
			if(error == errNone){
				error1 = EEPROM_OK;
			}else{
				error1 = EEPROM_ACCESS_ERROR;
			}



			return(error1);
        }

/*********************************************************************************************/
/* RUTINAS INTERNAS */
/********************/

  /* ESCRITURA DE 1 WORD EN EEPROM */
  /*********************************/

    tEEPROM_ERROR EEPROM_write (word* ptrEeprom, word dato){
      // Programa 1 word en la EEPROM, en la direccion pasada como argumento
      tEEPROM_ERROR error;
      uint16_t bufferWord[1];
      uint32_t addressEeprom;
      error_t err;

        error = EEPROM_OK;                // Asumo que no hay error

  		bufferWord[0] = dato;

  		err = EEPROM_WriteBuffer((uint8_t*) &bufferWord, (uint32_t)ptrEeprom, (uint16_t)2);
        if(err != errNone){
        	//para debug
       // 	uint8_t stop=1;
        //	while(stop){
        //	}
        }
  		return(error);
    }


    tEEPROM_ERROR EEPROM_writeDouble (uint32_t* EEPROM_ptr, uint32_t dato){
        // Programa 1 word en la EEPROM, en la direccion pasada como argumento
        tEEPROM_ERROR error;
        uint32_t buffer_backup[1];
        uint32_t address_eeprom;

        error = EEPROM_OK;                // Asumo que no hay error

    		buffer_backup[0] = dato;
    		address_eeprom = EEPROM_ptr;

    		EEPROM_WriteBuffer((uint8_t*) &buffer_backup,address_eeprom,(uint16_t)4);

    		return(error);
      }

    uint32_t EEPROM_readDouble (uint32_t* EEPROM_ptr){

    	 uint32_t dato;
         uint32_t* ptr_dato;

            ptr_dato = &dato;
     		EEPROM_ReadBuffer(ptr_dato,EEPROM_ptr,4);
     		return(dato);
       }

  /* BORRADO DE EEPROM */
  /*********************/
    tEEPROM_ERROR EEPROM_clear (word* EEPROM_ptr){
      // Borra cuatro bytes de la EEPROM
      tEEPROM_ERROR error;
      uint16_t buffer_backup[1];

  	  buffer_backup[0] = 0xffff;
      error = EEPROM_WriteBuffer((uint8_t*) &buffer_backup, EEPROM_ptr, (uint16_t)2);

      error = EEPROM_OK;                // Asumo que no hay error

 		return(error);
    }


    void levantar_variablesEEPROM (void){
    	nroCorrelativo_INTERNO = Read_nroCorrelativo_from_EEPROM();
    	nroTICKET = EEPROM_readDouble ((uint32_t*) EEPROM_NRO_TICKET);
    }


    void borrar_EEPROM (void){
    	 uint32_t aux_TABLA;
    	 uint32_t i, k;
         byte dato;
    		i = 0;
    		k = 20;
    		 aux_TABLA = (uint32_t) 0x0000;
    		 while(i < 64*1024){
    		 		//EEPROM_WriteByte_irqDisable((uint32_t) ptrEEPROM, 0xff);
    		 		EEPROM_WriteByte(aux_TABLA, 0xff);
    		 		//dato = EEPROM_ReadByte(aux_TABLA);
    		 		//if(dato != 0xff){
    		 			//__NOP();
    		 		//}
    		 		aux_TABLA++;
    		 		i++;
    		 		if(i == k){
    		 			k = i +20;
    		 			HAL_GPIO_TogglePin(BANDERA_OUT_PORT, BANDERA_OUT_PIN);
    		 		}
    		 }
   			 //resetea el equipo
   			 //NVIC_SystemReset();

      }


    uint32_t buffAddress[100];

    void testAddress(void){

       		buffAddress[0] = ADDRESS_BACKUP_EEPROM;
       		buffAddress[1] = ADDRESS_CORTE_ALIM_EEPROM;
       		buffAddress[2] = ADDRESS_TARIFA_EEPROM;
       		buffAddress[3] = ADDRESS_ESTADO_RELOJ_EEPROM;
       		buffAddress[4] = ADDRESS_BYTE_FILL1_EEPROM;
       		buffAddress[5] = ADDRESS_VEL_MAX_EEPROM;
       		buffAddress[6] = ADDRESS_SEG_ESPERA_EEPROM;
       		buffAddress[7] = ADDRESS_SEG_TARIF_EEPROM;
       		buffAddress[8] = ADDRESS_WORD_FILL1_EEPROM;
       		buffAddress[9] = ADDRESS_FICHAS_PULSOS_EEPROM;
       		buffAddress[10] = ADDRESS_FICHAS_TIEMPO_EEPROM;
       		buffAddress[11] = ADDRESS_CONTADOR_PULSOS_EEPROM;
       		buffAddress[12] = ADDRESS_CONTADOR_PULSOS_OLD_EEPROM;
       		buffAddress[13] = ADDRESS_DIS_OCUP_EEPROM;
       		buffAddress[14] = ADDRESS_OCUP_DATE_EEPROM;
       		buffAddress[15] = ADDRESS_COB_DATE_EEPROM;
       		buffAddress[16] = ADDRESS_DATE_APAGADO_EEPROM;
       		buffAddress[17] = ADDRESS_BACKUP_EEPROM + SIZE_BACKUP_EEPROM;

           	buffAddress[18] = ADDR_EEPROM_PRIMER_ENCENDIDO;
           	buffAddress[19] = ADDR_EEPROM_PRIMER_ENCENDIDO + SIZE_EEPROM_PRIMER_ENCENDIDO;
           	buffAddress[20] = EEPROM_NRO_CORRELATIVO;			//ADDR_EEPROM_PAGE_12 (10)
           	buffAddress[21] = EEPROM_NRO_CORRELATIVO + 1;			//ADDR_EEPROM_PAGE_12 (10)
           	buffAddress[22] = EEPROM_NRO_TICKET;			    //ADDR_EEPROM_PAGE_12 (11)
           	buffAddress[23] = EEPROM_NRO_TICKET + SIZE_EEPROM_NRO_TICKET;			    //ADDR_EEPROM_PAGE_12 (11)
           	buffAddress[24] = ADDR_EEPROM_CHOFER;
           	buffAddress[25] = ADDR_EEPROM_CHOFER + SIZE_EEPROM_CHOFER;

       		buffAddress[26] = ADDRESS_PROG1;
           	buffAddress[27] = ADDRESS_PROG_relojCOMUN + SIZE_PROG_relojCOMUN;
           	buffAddress[28] = ADDRESS_PROG_relojT1D;
           	buffAddress[29] = ADDRESS_PROG_relojT1D + SIZE_PROG_relojT1D;
           	buffAddress[30] = ADDRESS_PROG_relojT2D;
           	buffAddress[31] = ADDRESS_PROG_relojT2D + SIZE_PROG_relojT2D;
           	buffAddress[32] = ADDRESS_PROG_relojT3D;
           	buffAddress[33] = ADDRESS_PROG_relojT3D + SIZE_PROG_relojT3D;
           	buffAddress[34] = ADDRESS_PROG_relojT4D;
           	buffAddress[35] = ADDRESS_PROG_relojT4D + SIZE_PROG_relojT4D;

           	buffAddress[36] = ADDRESS_PROG_relojT1N;
           	buffAddress[37] = ADDRESS_PROG_relojT1N + SIZE_PROG_relojT1N;
           	buffAddress[38] = ADDRESS_PROG_relojT2N;
           	buffAddress[39] = ADDRESS_PROG_relojT2N + SIZE_PROG_relojT2N;
           	buffAddress[40] = ADDRESS_PROG_relojT3N;
           	buffAddress[41] = ADDRESS_PROG_relojT3N + SIZE_PROG_relojT3N;
           	buffAddress[42] = ADDRESS_PROG_relojT4N;
           	buffAddress[43] = ADDRESS_PROG_relojT4N + SIZE_PROG_relojT4N;
           	buffAddress[44] = ADDRESS_PROG_relojEqPESOS;
           	buffAddress[45] = ADDRESS_PROG_relojEqPESOS + SIZE_PROG_relojEqPESOS;
           	buffAddress[46] = ADDRESS_PROG_relojCALEND;
           	buffAddress[47] = ADDRESS_PROG_relojCALEND + SIZE_PROG_relojCALEND;

           	buffAddress[48] = ADDRESS_PROG_MOVIL;
           	buffAddress[49] = ADDRESS_PROG_MOVIL + SIZE_PROG_MOVIL;

           	buffAddress[50] = ADDRESS_PROG2;
           	buffAddress[51] = ADDRESS_PROG3;
           	buffAddress[52] = ADDRESS_PROG4;

           	buffAddress[53] = ADDRESS_PROG_TICKET_PAGE1;
           	buffAddress[54] = ADDRESS_PROG_TICKET_PAGE2;
           	buffAddress[55] = ADDRESS_PROG_TICKET_PAGE1 + SIZE_PROG_TICKET;

           	buffAddress[56] = ADDRESS_PROG_TICKET_RECAUD;
           	buffAddress[57] = ADDRESS_PROG_TICKET_RECAUD + SIZE_PROG_TICKET_RECAUD;

   			buffAddress[58] = ADDR_EEPROM_REPORTE_PUT;
       		buffAddress[59] = ADDR_EEPROM_REPORTE_PUT + SIZE_EEPROM_REPORTE_PUT;
       		buffAddress[60] = ADDR_EEPROM_REPORTE_INDEX;
       		buffAddress[61] = ADDR_EEPROM_REPORTE_INDEX + SIZE_EEPROM_REPORTE_INDEX;
       		buffAddress[62] = ADDR_EEPROM_REPORTE_NRO_VIAJE;
       		buffAddress[63] = ADDR_EEPROM_REPORTE_NRO_VIAJE + SIZE_EEPROM_REPORTE_NRO_VIAJE;
       		buffAddress[64] = ADDR_EEPROM_REPORTE_NRO_TURNO;
       		buffAddress[65] = ADDR_EEPROM_REPORTE_NRO_TURNO + SIZE_EEPROM_REPORTE_NRO_TURNO;
       		buffAddress[66] = ADDR_INDEX_LAST_SESION;
       		buffAddress[67] = ADDR_INDEX_LAST_SESION + SIZE_INDEX_LAST_SESION;

           	buffAddress[68] = ADDR_BASE_TABLA_REPORTE;
           	buffAddress[69] = ADDR_NEXT_REPORTE;

           	buffAddress[70] = ADDR_DATOS_SC_GET_PTR;
           	buffAddress[71] = ADDR_EEPROM_VELOCIDAD_MAX_VIAJE;

           }


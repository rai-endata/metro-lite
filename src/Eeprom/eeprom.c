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


void write_backup_eeprom(void){

	byte* ptrDIR;
	uint8_t buffer_backup[128];
    uint8_t i=0;

	buffer_backup[i] = 1; i++;
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

    uint32_t addr;
    uint32_t address_eeprom;
    address_eeprom = ADDRESS_BACKUP_EEPROM;

    	//me fijo si hubo corte de alimentacion
    	EEPROM_ReadBuffer((uint8_t*) &buffer_backup,ADDRESS_BACKUP_EEPROM,SIZE_BACKUP_EEPROM);

    	corteALIMENTACION = buffer_backup[i]; i++;
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
    void EEPROM_ini (byte initee){
    }


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
           word *prueba_1;
           byte prueba_2;

  /* GRABACION DE UN BYTE EN EEPROM */
  /**********************************/
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


  /* GRABACION DE WORD EN EEPROM */
  /*******************************/
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


  /* GRABACION DE UN BUFFER EN EEPROM */
  /************************************/
    tEEPROM_ERROR grabar_buffer_EEPROM (uint16_t* data_buffer, uint16_t* EEPROM_ptr, uint16_t max_size){
      // Graba un buffer en EEPROM. Para hacerlo, se le debe decir que buffer va a grabar
      // y en que direccion. Ademas, se le debe pasar la cantidad de bytes a grabar.
      // Esta cantidad debe ser multiplo de 4
      tEEPROM_ERROR error;
      uint16_t EEPROMsize_max;
      byte fin_datos;
      byte* fin_ptr;

      error = EEPROM_OK;                            // Asumo que no hay error

        EEPROMsize_max = EEPROM_PAGE_SIZE_BYTES;              // cantidad de bytes de una pagina

        fin_datos = 0;                              // Reseteo Variable
        fin_ptr = (byte*) data_buffer;
        while ((EEPROMsize_max != 0) && !(fin_datos)){

          EEPROMsize_max--;                         // Voy decrementando cantidad de 4bytes a grabar

          // chequeo condicion |DF|0A|XX|XX|
          if ((*fin_ptr == finEEPROM_H) && (*(fin_ptr+1) == finEEPROM_L)){
            fin_datos = 1;                          // Llegue al fin => Lo grabo y salgo
          }

          error = EEPROM_clear(EEPROM_ptr);                 // Borro Direccion de EEPROM donde voy a grabar los datos. Borro 2 WORDS

          error = EEPROM_write(EEPROM_ptr, *data_buffer);   // Grabar Word en EEPROM
          EEPROM_ptr++;                             		// Avanzo puntero de EEPROM
          data_buffer++;                            		// Avanzo Puntero de Datos


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
            error = EEPROM_write(EEPROM_ptr, *data_buffer); // Grabar Word en EEPROM
            EEPROM_ptr++;                           // Avanzo puntero de EEPROM
            data_buffer++;                          // Avanzo Puntero de Datos

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
            error = EEPROM_write(EEPROM_ptr,(word) 0x0000); // Grabar Word en EEPROM
          }
        }

      return(error);
    }

/*********************************************************************************************/
/* RUTINAS INTERNAS */
/********************/

  /* ESCRITURA DE 1 WORD EN EEPROM */
  /*********************************/

    tEEPROM_ERROR EEPROM_write (word* EEPROM_ptr, word dato){
      // Programa 1 word en la EEPROM, en la direccion pasada como argumento
      tEEPROM_ERROR error;
      uint16_t buffer_backup[1];
      uint32_t address_eeprom;

      error = EEPROM_OK;                // Asumo que no hay error

  		buffer_backup[0] = dato;
  		address_eeprom = EEPROM_ptr;

  		EEPROM_WriteBuffer((uint8_t*) &buffer_backup,address_eeprom,(uint16_t)2);

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

      error = EEPROM_OK;                // Asumo que no hay error

 		return(error);
    }

    void testEEPROM(void ){

    	byte EEPROM_AUX[20];
        error_t status_error;

    	status_error = EEPROM_WriteByte(ADDR_DATA1_EEPROM, 1 );
        status_error = EEPROM_WriteByte(ADDR_DATA2_EEPROM, 2 );
        status_error = EEPROM_WriteByte(ADDR_DATA3_EEPROM, 3 );
        status_error = EEPROM_WriteByte(ADDR_DATA4_EEPROM, 4 );
        status_error = EEPROM_WriteByte(ADDR_DATA5_EEPROM, 5 );
        status_error = EEPROM_WriteByte(ADDR_DATA6_EEPROM, 6 );
        status_error = EEPROM_WriteByte(ADDR_DATA7_EEPROM, 7 );
        status_error = EEPROM_WriteByte(ADDR_DATA8_EEPROM, 8 );
        status_error = EEPROM_WriteByte(ADDR_DATA9_EEPROM, 9 );
        status_error = EEPROM_WriteByte(ADDR_DATA10_EEPROM, 10);

        EEPROM_WriteByte(EEPROM_NRO_CORRELATIVO, nroCorrelativo_INTERNO);

        EEPROM_AUX[0]  = EEPROM_ReadByte(ADDR_DATA1_EEPROM);
        EEPROM_AUX[1]  = EEPROM_ReadByte(ADDR_DATA2_EEPROM);
        EEPROM_AUX[2]  = EEPROM_ReadByte(ADDR_DATA3_EEPROM);
        EEPROM_AUX[3]  = EEPROM_ReadByte(ADDR_DATA4_EEPROM);
        EEPROM_AUX[4]  = EEPROM_ReadByte(ADDR_DATA5_EEPROM);
        EEPROM_AUX[5]  = EEPROM_ReadByte(ADDR_DATA6_EEPROM);
        EEPROM_AUX[6]  = EEPROM_ReadByte(ADDR_DATA7_EEPROM);
        EEPROM_AUX[7]  = EEPROM_ReadByte(ADDR_DATA7_EEPROM);
        EEPROM_AUX[8]  = EEPROM_ReadByte(ADDR_DATA8_EEPROM);
        EEPROM_AUX[9]  = EEPROM_ReadByte(ADDR_DATA9_EEPROM);
        EEPROM_AUX[10] = EEPROM_ReadByte(ADDR_DATA10_EEPROM);

        EEPROM_ReadBuffer(&EEPROM_AUX,ADDR_DATA1_EEPROM,10);
    }

    void levantar_variablesEEPROM (void){
    	nroCorrelativo_INTERNO = Read_nroCorrelativo_from_EEPROM();
    	nroTICKET = EEPROM_readDouble ((uint32_t*) EEPROM_NRO_TICKET);
    }

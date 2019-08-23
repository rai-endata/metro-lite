/*
 * Flash.c
 *
 *  Created on: 21/2/2018
 *      Author: Rai
 */
#include "stm32f0xx_hal_flash.h"
#include "Flash.h"
#include "Definicion de tipos.h"
#include "driver_eeprom.h"

tFLASH_FLAGS 	_flash_F;

 static uint16_t* FLASH_bkpSector (uint16_t* far_address);
 static uint16_t* FLASH_determineSectorIni (uint16_t* far_address);
 static uint16_t FLASH_sectorsToUpdate(uint16_t offset, uint16_t data_length);
 static tFLASH_ERROR FLASH_updateBackup(uint16_t offset, byte* data_ptr, uint16_t sizeToUpdate);
 static tFLASH_ERROR FLASH_writeSector(uint16_t* far_address, uint16_t* data_ptr);

 uint16_t FLASH_bkp[FLASH_sectorSize/2];     // BackUp del Sector a Borrar
//  static byte FLASH_dontTouchIRQ;         // Indica que no se deben tocar las IRQ al grabar la FLASH


 void grabar_enFLASH (void){
 //	cobrandoDATE_enFLASH(0);
 //	ocupadoDATE_enFLASH(0);
 }


/* BORRAR SECTOR DE FLASH - A PARTIR DIRECCION DE INICIO */
/*********************************************************/

   tFLASH_ERROR FLASH_eraseSector(uint16_t* Address){

	//ESTA RUTINA: Borra la pagina que contiene Address

	tFLASH_ERROR error;
	FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t PageError = 0;

    error = FLASH_ERR_NONE;                   // Asumo que no hay error

	EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.PageAddress = (uint32_t)Address;
	EraseInitStruct.NbPages = 1;
	if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK){
		error = FLASH_ERR_ACCESS;
	}

	return (error);
   }

   /* ACTUALIZACION DE FLASH */
   /**************************/
   /*
   extern tFLASH_ERROR FLASH_updateSector (uint16_t* far_flash_address, uint8_t* data_ptr, uint16_t data_length){
       // Rutina que actualiza la FLASH. Para ello:
       //  - Realiza un backup del/de los sector/es a actualizar
       //  - Borra el/los sector/es involucrados
       //  - Actualiza (en RAM) los datos a grabar en FLASH
       //  - Graba en flash el/los sector/es con sus valores actualizados
       //
       // En caso de que la longitud de datos requiera que se trabaje sobre mas de un sector
       // se trabajará de a un sector a la vez.
       //
       // Hay que tener en cuenta que para evitar problemas, la longitud debe ser PAR, ya que
       // la FLASH se escribe a WORDS.
       //
       //
       volatile tFLASH_ERROR error;
       volatile uint16_t numberOfSectors;
       volatile uint16_t* far_iniSector_address;
       volatile uint16_t offset;
       volatile uint16_t writeLength;
       uint32_t prim;
       error = FLASH_ERR_NONE;                           // Asumo que no hay error


       if((!IS_FLASH_PROGRAM_ADDRESS((uint32_t)far_flash_address))){
    	   error = FLASH_ERR_NOT_FLASH_ADDRESS;			//no es una direccion de flash valida
       };

       if (data_length == 0){
         error = FLASH_ERR_NULL_DATA_LENGTH;             // Error de Longitud de datos nula
       }

       if(data_length & 0x0001){
     	  error = FLASH_ERR_ODD_LENGTH;                   // Longitud Impar
     	}

     	if (error == FLASH_ERR_NONE){
       	// Ahora tengo que determinar el offset de la direcion a grabar respecto del inicio del
       	// sector, ya que ese mismo offset se ve reflejado en el buffer de backup, y asi se a
       	// partir de que direccion tengo que actualizar los datos en el backup
     	offset = (uint16_t)((uint32_t)far_flash_address) % FLASH_sectorSize;  // Offset de la direccion a grabar respecto del inicio del sector

       	// Determino la cantidad de sectores a actualizar y la cantidad de datos a escribir
       	// en la primer grabacion
       	numberOfSectors = FLASH_sectorsToUpdate(offset, data_length);
       	if (numberOfSectors > 1){
       	  writeLength = FLASH_sectorSize - offset;      // Escribo la cantidad de bytes necesarios para completar el sector inicial
       	}else{
       	  writeLength = data_length;                    // Escribo la totalidad de los datos
       	}

       	while ((numberOfSectors > 0) && (error == FLASH_ERR_NONE)){
			far_iniSector_address = FLASH_bkpSector (far_flash_address);      // Hago BackUp del Sector
			error = FLASH_updateBackup(offset, data_ptr, writeLength);        // Actualizo backup (RAM) con los datos nuevos

			prim = __get_PRIMASK();
			__disable_irq();
			HAL_FLASH_Unlock();
			error = FLASH_eraseSector((uint16_t*) far_iniSector_address);                 // Borro el Sector
			error = FLASH_writeSector((uint16_t*) far_iniSector_address, FLASH_bkp);      // Actulizo la FLASH
			HAL_FLASH_Lock();
			if (!prim) {
				__enable_irq();
			}

			data_length -= writeLength;                                       // Descuento la cantidad de datos que ya grabe
			data_ptr += writeLength;                                          // Avanzo el puntero a datos tantos lugares como datos grabe
			far_flash_address = (uint16_t*) far_iniSector_address + (FLASH_sectorSize/2); // Paso a inicio del siguiente sector
			offset = 0;                                                       // A partir de ahora grabo sectores desde el inicio
			numberOfSectors--;                                                // Decremento Contador
			// Si todavia quedan sectores por actualizar, me fijo si voy a actualizar un sector entero o lo
			// que resta para completar la cantidad total de datos
			if (data_length > FLASH_sectorSize){
			  writeLength = FLASH_sectorSize;                                 // Escribo un sector entero
			}else{
			  writeLength = data_length;                                      // Escribo la cantidad de datos que faltan para completar la grabacion
			}
       	}
     	}

     	//FLASH_dontTouchIRQ = 0;                                               // Dejo la variable en estado de reposo

     	if (error != FLASH_ERR_NONE){
        // BUZZER_play(RING_error);
       }

     	return (error);
     }
*/

   extern tFLASH_ERROR FLASH_updateSector (uint16_t* far_flash_address, uint8_t* data_ptr, uint16_t data_length){
	   error_t status;

	   status = EEPROM_WriteBuffer(data_ptr, far_flash_address, data_length);
	   if(status == errNone){
		   return(FLASH_ERR_NONE);
	   }else{
		   return(FLASH_ERR_ACCESS);
	   }
   }


 /*********************************************************************************************/
 /* RUTINAS INTERNAS */
 /********************/

   /* BACKUP DE SECTOR DE FLASH */
   /*****************************/
   uint16_t* FLASH_bkpSector (uint16_t* far_address){
       // Esta rutina hace un backup del sector en "FLASH_bkp".
       // Se le pasa como argumento la direccion FAR de FLASH, con ella determina a que sector
       // pertenece, realiza el backup del sector completo, y devuelve la direccion de inicio
       // de dicho sector. Esto es util, ya que a la hora de borrar necesito la direccion de
       // inicio.
	   uint16_t* sector_ini_address;
	   uint16_t i;

       sector_ini_address = FLASH_determineSectorIni(far_address); // Determino sector de inicio

       for (i=0; i<(FLASH_sectorSize/2); i++){
         FLASH_bkp[i] = *(sector_ini_address + i);                 // Hago BackUp de a WORD
       }

       return (sector_ini_address);
     }


   /* DETERMINAR DIRECCION DE INICIO DE SECTOR DE FLASH */
   /*****************************************************/
   uint16_t* FLASH_determineSectorIni (uint16_t* far_address){
       // Funcion que dada una direccion de flash, devuelve la direccion de inicio del
       // sector al que pertenece
	  // uint16_t* address;
	   uint16_t offset;
       uint32_t ini_sector;


       offset = (uint16_t)((uint32_t) far_address - (uint32_t) FLASH_sectorIni);// Calculo el offset desde el inicio de Flash

       // Ahora, al offset, le voy restando el tamaño de un sector (si puedo) y esa es la
       // diferencia que hay entre la direccion y el inicio del sector al que pertenece
       while (offset >= FLASH_sectorSize){
         offset -= FLASH_sectorSize;             // Resto tamaño de sector
       }

       ini_sector = (uint32_t) far_address -  offset;
       far_address = ((uint16_t*) ini_sector);

       return (far_address);                     // Devuelvo direccion de inicio
     }


   /* DETERMINAR CANTIDAD DE SECTORES A ACTUALIZAR */
   /************************************************/
   uint16_t FLASH_sectorsToUpdate(uint16_t offset, uint16_t data_length){
       // Esta rutina nos dice cuantos sectores se van a ver modificados al actualizar.
       // Depende de la direccion de inicio de la actualizacion (no inicio de sector)
       // y de la cantidad de datos a actualizar.
	   uint16_t iniSector_rest;
       byte numberOfSectors;

       // Ahora calculo cuanto espacio tengo en el sector donde comienza la actualizacion
       iniSector_rest = FLASH_sectorSize - offset;

       // Luego, si la longitud de datos es mayor a lo que me resta del sector inicial
       // es porque va a ocupar mas de un sector
       numberOfSectors = 1;              // De movida voy a ocupar un sector
       if(data_length > iniSector_rest){
         data_length -= iniSector_rest;  // Le quito lo que voy a grabar en el resto

         numberOfSectors += data_length / FLASH_sectorSize; // Agrego cantidad de sectores enteros
         if ((data_length % FLASH_sectorSize) != 0){
           numberOfSectors++;            // Incremento cantidad
         }
       }

       return(numberOfSectors);
     }


   /* ACTUALIZAR EL BACKUP DEL SECTOR */
   /***********************************/
     tFLASH_ERROR FLASH_updateBackup(uint16_t offset, byte* data_ptr, uint16_t sizeToUpdate){
       // Rutina que agrega los datos nuevos al backup hecho del sector.
       // La longitud debe ser menor o igual a la longitud del sector
       tFLASH_ERROR error;
       byte* bkp_ptr;
       uint16_t size_left;

       error = FLASH_ERR_NONE;                 // Asumo que no hay error

       size_left = FLASH_sectorSize - offset;  // Espacio disponible en el sector
       if (sizeToUpdate > size_left){
         error = FLASH_ERR_SIZE_TOO_LARGE;     // El tamaño de los datos a grabar es mayor al espacio disponible

       }else{
         bkp_ptr =(byte*) &FLASH_bkp;          // Puntero a inicio del backup
         bkp_ptr += offset;                    // Le sumo el offset

         while  (sizeToUpdate > 0){
           *bkp_ptr++ = *data_ptr++;           // Actualizo backup
           sizeToUpdate--;                     // Decremento longitud de datos
         }
       }

       return(error);
     }




   /* GRABAR SECTOR EN FLASH */
   /**************************/
     tFLASH_ERROR FLASH_writeSector(uint16_t* far_address, uint16_t* data_ptr){
       // La direccion de FLASH pasada como argumento, debe ser la direccion de
       // INICIO del sector.
       // Esto tiene que ser así ya que graba tantos uint16_ts, como uint16_ts tenga un sector
       // y si no se trata de la direccion de inicio, graba mitad en un sector y mitad
       // en el siguiente
       //
       // Como la  rutina que graba WORD cheque si la direccion es IMPAR o si la flash
       // no esta borrada, aqui lo unico que chequeo es que la direccion sea el
       // inicio del sector
    	 uint16_t* address;
       tFLASH_ERROR error;

       uint16_t word_count;

       error = FLASH_ERR_NONE;                   // Asumo que no hay error

       address = (uint16_t*)far_address;             // Le quito la informacion de PPAGE

       if((uint32_t)address % FLASH_sectorSize !=0){
     	  error = FLASH_ERR_NOT_START_OF_SECTOR;  // La direccion a Borrar no es el Inicio del Sector
     	}

       word_count = 0;                           // Reseteo Cuenta
     	while ((word_count < (FLASH_sectorSize/2) ) && (error == FLASH_ERR_NONE)){
     	  // Mientras no haya escrito tantos words como los que tiene el sector y no haya
     	  // errores, escribo la FLASH
     	  error = FLASH_writeWord(far_address++, *data_ptr++);

     	  word_count++;                           // Incremento Cuenta
     	}

     	return (error);
     }

     /* GRABACION DE UN BYTE EN EEPROM */
       /**********************************/

     tFLASH_ERROR grabar_byte_FLASH (uint8_t dato,    uint32_t* FLASH_ptr,      uint8_t mask){
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
           tFLASH_DATA valor_FLASH;
           tFLASH_ERROR error;

           error = FLASH_ERR_NONE;                    // Asumo que no hay error

           if ((mask > 4) || (mask == 0)){
             error = FLASH_ERR_ACCESS;          // Error en la mascara de Grabacion
           }

           if (!error){
               valor_FLASH = *( (tFLASH_DATA*)FLASH_ptr ); // Leo el valor actual de la variable.

             if (mask == 1){
               valor_FLASH.Bytes.byte1 = dato;   // Actualizo Valor

             }else if (mask == 2){
               valor_FLASH.Bytes.byte2 = dato;   // Actualizo Valor

             }else if (mask == 3){
               valor_FLASH.Bytes.byte3 = dato;   // Actualizo Valor

             }else if (mask == 4){
               valor_FLASH.Bytes.byte4 = dato;   // Actualizo Valor
             }

             error = FLASH_updateSector((uint16_t*) FLASH_ptr, (uint8_t*)&valor_FLASH, (uint16_t) 4);

           }

            return(error);
           }


       /* GRABACION DE WORD EN EEPROM */
       /*******************************/
         tFLASH_ERROR grabar_word_FLASH (uint16_t dato, uint16_t* FLASH_ptr, uint8_t mask){
           // Rutina que dada una variable en EEPROM, (4bytes) graba solamente 2 bytes
           // (word) de esa variable y el resto lo deja como esta. La mascara indica que
           // word modificar.
           // El tipo de datos "tEEPROM_DATA" se compone de la siguiente manera, y
           // vemos al lado el valor de mascara asociado a cada uno:
           //
           //  word1 --> mask = 1
           //  word2 --> mask = 2
           tFLASH_DATA valor_FLASH;
           tFLASH_ERROR error;

           error = FLASH_ERR_NONE;                          // Asumo que no hay error

           if ((mask > 2) || (mask == 0)){
             error = FLASH_ERR_ACCESS;                // Error en la mascara de Grabacion
           }

           if(!error){
               valor_FLASH = *((tFLASH_DATA*) FLASH_ptr); // Leo el valor actual de la variable.

             if (mask == 1){
               valor_FLASH.MergedBytes.word1 = dato;   // Actualizo Valor

             }else if (mask == 2){
               valor_FLASH.MergedBytes.word2 = dato;   // Actualizo Valor
             }

             error = FLASH_updateSector((uint16_t*) FLASH_ptr, (uint8_t*) &valor_FLASH, (uint16_t) 4);
           }

           return(error);
         }

         /* GRABAR WORD(2 bytes) EN FLASH */
          /********************************/

         tFLASH_ERROR FLASH_writeWord(uint16_t* Address, __IO uint16_t data){
         	// Rutina que grabar un word en la direccion de FLASH establecida. La direccion
         	// a grabar debe ser par, ya que la FLASH se organiza de a WORDS-ALINEADOS, es
         	// necesario que la direccion sea PAR, sino se grabaria mitad de un word y mitad
         	// del siguiente.
         	//
         	// ESTA RUTINA:
         	// - No graba si la direccion es IMPAR
         	// - No realiza el previo "borrado" de la flash

         	//word* address;
         	tFLASH_ERROR error;

         	//direccion par ?
         	if( (uint32_t) Address & 0x0001){
         	  error = FLASH_ERR_ODD_ACCESS;     // Not-Aligned Word (Direccion Impar)
         	}
         	if( error != FLASH_ERR_ODD_ACCESS){
         		//grabacion correcta ?
         		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, (uint32_t)Address, data) == HAL_OK){
         		  error = FLASH_ERR_NONE;
         		}else{
         		  error = FLASH_ERR_ACCESS;
         		}
         	}

         	return (error);
         }

         /* CHEQUEAR SI PUEDO ACTUALIZAR LA FLASH */
           /*****************************************/
         uint8_t FLASH_chkCanUpdate (void){
                uint8_t status;
            	status = 0;
            	//por ahora grabo siempre
            	force_saveFLASH = 1;
            	if(force_saveFLASH){
                	status = 1;
                }
                return(status);
             }

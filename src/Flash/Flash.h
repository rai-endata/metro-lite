/*
 * Flash.h
 *
 *  Created on: 21/2/2018
 *      Author: Rai
 */

#ifndef _FLASH_H_
#define _FLASH_H_
#include "stm32f0xx_hal_flash.h"

  /* ESTRUCTURAS */
  /***************/

    // Tipo de Datos de Errores de Flash
    typedef enum{
      FLASH_ERR_NONE,                   // NO ERROR => OK
      FLASH_ERR_ODD_ACCESS,             // Direccion de Borrado/Escritura IMPAR
      FLASH_ERR_NOT_ERASED,             // Direccion no BORRADA => No se puede programar
      FLASH_ERR_ACCESS,                 // Error de ACCESO (propio de HCS12)
      FLASH_ERR_VIOLATION,              // Error de VIOLACION (propio de HCS12)
      FLASH_ERR_NOT_START_OF_SECTOR,    // No se trata del Inicio del Sector (borrado de sector)
      FLASH_ERR_NULL_DATA_LENGTH,       // La longitud de datos a grabar es NULA
      FLASH_ERR_ODD_LENGTH,             // Longitud de datos IMPAR
      FLASH_ERR_SIZE_TOO_LARGE,          // El tamño de los datos es demasiado grande
	  FLASH_ERR_NOT_FLASH_ADDRESS		//la direccion no es una direccion de flash valida
    }tFLASH_ERROR;

    // TIPO DE DATOS DE EEPROM
      typedef union{
        uint32_t LWord;
        struct{
			uint32_t byte1 :8;
			uint32_t byte2 :8;
			uint32_t byte3 :8;
			uint32_t byte4 :8;
        }Bytes;

        struct{
        	uint32_t word1 :16;
        	uint32_t word2 :16;
        }MergedBytes;
      }tFLASH_DATA;


      // BANDERAS DE FLASH
        typedef union {
          uint8_t Byte;
          struct{
        	  uint8_t aux1			    :1;     // Indica que se debe HACER esa pantalla
        	  uint8_t force_saveFSH    :1;     // Bandera auxiliar 1
        	  uint8_t aux_2            :1;			// Bandera auxiliar 2
        	  uint8_t aux_3            :1;			// Bandera auxiliar 3
        	  uint8_t aux_4            :1;			// Bandera auxiliar 4
        	  uint8_t aux_5            :1;			// Bandera auxiliar 5
        	  uint8_t aux_6            :1;			// Bandera auxiliar 6
        	  uint8_t aux_7            :1;     // Bandera auxiliar 7
          }Bits;
        }tFLASH_FLAGS;

// BANDERAS DE PANTALLA LOGUEO
extern tFLASH_FLAGS 	_flash_F;

	#define flash_F		              _flash_F.Byte
	#define force_saveFLASH           _flash_F.Bits.force_saveFSH     // Indica que se debe forzar grabacion en FLASH

    /* DEFINICION DE MACROS */
    /************************/

  #define STM32F09x_flashPageSize     	2048    		// 1024bytes Tamaño de cada sector de Flash del HCS12X
  #define STM32F09x_flashPageIni       	0x08000000  	// Direccion de Inicio de Sector de Flash

  #define FLASH_sectorSize            	STM32F09x_flashPageSize
  #define FLASH_sectorIni             	STM32F09x_flashPageIni
  //#define __far							//lo define como vacio por compatibilidad de libreria con el hcs12


    /* DEFINICION DE PROTOTIPOS */
    /****************************/
    extern tFLASH_ERROR FLASH_writeWord(uint16_t* Address, __IO uint16_t data);
    extern tFLASH_ERROR FLASH_eraseSector(uint16_t* Address);
    extern tFLASH_ERROR FLASH_updateSector (uint16_t* far_flash_address, uint8_t* data_ptr, uint16_t data_length);
    extern tFLASH_ERROR grabar_byte_FLASH (uint8_t dato,    uint32_t* FLASH_ptr,      uint8_t mask);
    extern uint8_t FLASH_chkCanUpdate (void);
    extern void grabar_enFLASH (void);
#endif

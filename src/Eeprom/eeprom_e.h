/*
 * eeprom_e.h
 *
 *  Created on: 31/5/2018
 *      Author: Rai
 */

#ifndef EEPROM_EEPROM_E_H_
#define EEPROM_EEPROM_E_H_

 // ERRORES DE EEPROM

	typedef enum{
         EEPROM_OK,                        // No hay error
         EEPROM_ACCESS_ERROR,              // Error de Acceso
         EEPROM_PROTECTION_VIOLATION,      // Error de Violacion de Proteccion
         EEPROM_noMult4,                   // Error longitud a grabar no multiplo de 4
         EEPROM_ERROR_MASK,                // Error en la mascara de grabacion
		 EEPROM_CHECK_SUM              	   // Error de CHKSUM
       }tEEPROM_ERROR;

#endif /* EEPROM_EEPROM_E_H_ */

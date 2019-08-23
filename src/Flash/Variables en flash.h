/*
 * Variables en flash.h
 *
 *  Created on: 27/2/2018
 *      Author: Rai
 */

#ifndef FLASH_VARIABLES_EN_FLASH_H_
#define FLASH_VARIABLES_EN_FLASH_H_

#include "stdint.h"
#include "eeprom_aux.h"

// Banderas de SENSOR_ASIENTO
    typedef union {
          uint8_t Byte;
          struct {
            uint8_t ocupadoDATE        :1;    // Indica grabacion de FLASH fecha y hora que se ocupo
            uint8_t cobrandoDATE       :1;    // Indica grabacion de FLASH fecha y hora que se paso a cobrando
          } Bits;
    } flagFLASH;

#include "flash.h"
#include "Sensor Asiento.h"
//#include "Param Reloj.h"
#include "Parametros Reloj.h"
#include "Parametros Ticket.h"

#define FLASHsize_PRG_relojCOMUN       (sizeof(tPARAM_RELOJ_COMUNES))
#define FLASHsize_PRG_relojTARIFA      (sizeof(tPARAM_RELOJ_TARIFA))

#define FLASHsize_PRG_relojEqPESOS     (sizeof(tPARAM_RELOJ_EQPESOS))
#define FLASHsize_PRG_relojCALEND      (sizeof(tPARAM_RELOJ_CALEND) * dim_Feriados)
#define FLASHsize_PRG_MOVIL            (sizeof(tPARAM_MOVIL))
#define FLASHsize_HORA_APAGADO         8
#define FLASHsize_PRG_TICKET           (sizeof(tPRG_TICKET))

//extern const tPARAM_MOVIL FLASH_PROG_MOVIL;
extern const byte FLASH_HORA_APAGADO[FLASHsize_HORA_APAGADO];
extern const tPARAM_RELOJ_COMUNES FLASH_PROG_relojCOMUN;
extern const tPARAM_RELOJ_TARIFA FLASH_PROG_relojT1D;
extern const tPARAM_RELOJ_TARIFA FLASH_PROG_relojT2D;
extern const tPARAM_RELOJ_TARIFA FLASH_PROG_relojT3D;
extern const tPARAM_RELOJ_TARIFA FLASH_PROG_relojT4D;
extern const tPARAM_RELOJ_TARIFA FLASH_PROG_relojT1N;
extern const tPARAM_RELOJ_TARIFA FLASH_PROG_relojT2N;
extern const tPARAM_RELOJ_TARIFA FLASH_PROG_relojT3N;
extern const tPARAM_RELOJ_TARIFA FLASH_PROG_relojT4N;
extern const tPARAM_RELOJ_EQPESOS FLASH_PROG_relojEqPESOS;
extern const tPARAM_RELOJ_CALEND FLASH_PROG_relojCALEND[dim_Feriados];


extern const byte FLASH_SERIAL_ASIENTO[dim_SerialAsiento];
extern const tFLASH_DATA FLASH_VARS1;

	  #define FLASH_RESET                FLASH_VARS1.Bytes.byte1           // Indica MOTIVO de RESET
	  #define FLASH_RESET_msk            1                                 // Mascara de MOTIVO RESET dentro de los 4bytes
	  #define FLASH_NRO_CORRELATIVO      FLASH_VARS1.Bytes.byte2           // Indica NUMERO CORRELATIVO DE VIAJE
	  #define FLASH_NRO_CORRELATIVO_msk  2                                 // Mascara de NUMERO CORRELATIVO DE VIAJE dentro de los 4bytes

extern const tFLASH_DATA FLASH_VARS2;
extern const uint32_t FLASH_VARIOS_DESCONEXION_ACC;
extern const uint32_t FLASH_IMPORTE_DESCONEXION_ACC;
extern const uint32_t FLASH_DATE_DESCONEXION_ACC[2];

//const uint32_t EEPROM_LOCALIDAD @0xD90;
extern const uint32_t FLASH_LOCALIDAD;
extern const tPRG_TICKET FLASH_PROG_TICKET;
extern const tPRG_TICKET_RECAUD FLASH_PROG_TICKET_RECAUD;

 extern flagFLASH _writeFLASH;

#define writeFLASH 					_writeFLASH.flagFLASH.Byte
#define ocupadoDATE_writeFLASH 		_writeFLASH.Bits.ocupadoDATE
#define cobrandoDATE_writeFLASH 	_writeFLASH.Bits.cobrandoDATE



#endif /* FLASH_VARIABLES_EN_FLASH_H_ */

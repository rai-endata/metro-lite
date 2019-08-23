

#include "stdint.h"
#include "Variables en flash.h"
#include "flash.h"
#include "Sensor Asiento.h"
//#include "Param Reloj.h"
#include "Parametros Reloj.h"

#include "Parametros Ticket.h"


// VARIABLES DE FLASH

//const tPARAM_MOVIL FLASH_PROG_MOVIL={NULL};
const byte FLASH_HORA_APAGADO[FLASHsize_HORA_APAGADO]={NULL};
const tPARAM_RELOJ_COMUNES FLASH_PROG_relojCOMUN={NULL};
const tPARAM_RELOJ_TARIFA FLASH_PROG_relojT1D={NULL};
const tPARAM_RELOJ_TARIFA FLASH_PROG_relojT2D={NULL};
const tPARAM_RELOJ_TARIFA FLASH_PROG_relojT3D={NULL};
const tPARAM_RELOJ_TARIFA FLASH_PROG_relojT4D={NULL};
const tPARAM_RELOJ_TARIFA FLASH_PROG_relojT1N={NULL};
const tPARAM_RELOJ_TARIFA FLASH_PROG_relojT2N={NULL};
const tPARAM_RELOJ_TARIFA FLASH_PROG_relojT3N={NULL};
const tPARAM_RELOJ_TARIFA FLASH_PROG_relojT4N={NULL};
const tPARAM_RELOJ_EQPESOS FLASH_PROG_relojEqPESOS={};
const tPARAM_RELOJ_CALEND FLASH_PROG_relojCALEND[dim_Feriados];

const byte FLASH_SERIAL_ASIENTO[dim_SerialAsiento]={NULL};

//const tFLASH_DATA FLASH_VARS1 __attribute__ ((section ("_const_data")))={0x00,0x00,0x00,0x00};

//__asm (".SECTION .reportdata");

const tFLASH_DATA FLASH_VARS1 ={NULL};
//const tFLASH_DATA FLASH_VARS1={0x00000000};;


const tFLASH_DATA FLASH_VARS2={NULL};
const uint32_t FLASH_VARIOS_DESCONEXION_ACC={NULL};
const uint32_t FLASH_IMPORTE_DESCONEXION_ACC={NULL};
const uint32_t FLASH_DATE_DESCONEXION_ACC[2]={NULL};

//const uint32_t EEPROM_LOCALIDAD @0xD90;
const uint32_t FLASH_LOCALIDAD={NULL};

const tPRG_TICKET FLASH_PROG_TICKET={NULL};
const tPRG_TICKET_RECAUD FLASH_PROG_TICKET_RECAUD={NULL};

flagFLASH _writeFLASH;


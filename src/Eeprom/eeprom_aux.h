
#ifndef __PRUEBA__
#define	__PRUEBA__
#include "Parametros Reloj.h"
#include "rtc.h"
#include "driver_eeprom.h"
/*
 Cuando hay inclusiones cruzadas me da  Errores de compilacion.
 * Ej:
 * en file1 tengo:
 * //file1.h
 * #include file2.h
 *
 * y en file2 tengo
 * //file2.h
 * #include file1.h
 *
 * Tambien da error, con inclusiones transitivas cruzadas
 *
*ejemplo:

 * en file1 tengo:
 * //file1.h
 * #include file3.h


* //file2
 * #include file1.h

 * //file3
 * #include file2.h


 *
 * como file3 inlcluye a file2 y este a file1, y file1 incluye a file3, esto hace que file1 y  file3  estan cruzadas.
 * estan cruzados en forma transitiva a travez de file3.

 * */



typedef struct {
	backupEEPROM	dataBACKUP;
	tPROG			progEEPROM;
   }tEEPROM;


   typedef union
   {
    	uint8_t 		page[EEPROM_PAGE_SIZE_BYTES][EEPROM_SIZE/EEPROM_PAGE_SIZE_BYTES];			//access by page
    	uint8_t			byte[EEPROM_SIZE];			//access by byte
    	uint16_t		word16[EEPROM_SIZE/2];		//access by word 16
    	uint32_t		word32[EEPROM_SIZE/4];		//access by word 32
    	backupEEPROM	dataBACKUP;				    //access by user struct
    }eepromDATA;


//#define	ADDRESS_EEPROM  				    offsetof(tEEPROM,tPROG)

//#define	ADDRESS_PROG_relojCOMUN_EEPROM 		offsetof(tEEPROM,progEEPROM.PROG_relojCOMUN)
//#define	SIZE_PROG_relojCOMUN_EEPROM   	    sizeof(((tEEPROM *)0)->progEEPROM.PROG_relojCOMUN)








#endif

/*
 * test_eeprom.h
 *
 *  Created on: 30/4/2018
 *      Author: Rai
 */

#ifndef _TEST_EEPROM_H_
#define _TEST_EEPROM_H_

#include "stddef.h"
#include "stdint.h"

#include "driver_eeprom.h"

 typedef struct
  {
	uint8_t 	idString[20];
	uint8_t 	someBytes[1024];
 	uint16_t 	someInts[1024];
  }eeprom_user;

  typedef union
  {
   	uint8_t 		page[128][512];			//access by page
   	uint8_t			byte[65536];			//access by byte
   	uint16_t		userInt16[32768];
   	uint32_t		userUint16[16384];
   	eeprom_user		userStructEeprom;		//finally the user struct.
   }eeprom_u;


 //extern error_t LC_EepromTest(void);

 extern uint8_t sent1[];
 extern uint8_t sent2[];
 extern uint8_t sent3[];
 extern uint8_t sent4[];
 extern uint8_t sent5[];

 extern uint8_t received1[EEPROM_PAGE_SIZE_BYTES];
 extern uint8_t received2[EEPROM_PAGE_SIZE_BYTES];
 extern uint8_t received3[EEPROM_PAGE_SIZE_BYTES];


#endif /* EEPROM_EEPROM_H_ */

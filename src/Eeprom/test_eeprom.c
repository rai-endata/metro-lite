

#include "test_eeprom.h"
#include "driver_eeprom.h"


eeprom_u		*eeprom_64;

#define member_size(type, member) sizeof(((type *)0)->member)

error_t LC_EepromTest(void);

uint8_t received1[EEPROM_PAGE_SIZE_BYTES];
uint8_t received2[EEPROM_PAGE_SIZE_BYTES];
uint8_t received3[EEPROM_PAGE_SIZE_BYTES];


//If stack space is a problem, put these variables outside of this function
//so that they are not allocated on the stack.
//userString will only take 4 bytes as it will point to the constant string
//"LC640 buffer" in stm32 FLASH
uint8_t sent1[EEPROM_PAGE_SIZE_BYTES]=
{1,2,3,4,5,6,7,8,9,10
,1,2,3,4,5,6,7,8,9,20
,1,2,3,4,5,6,7,8,9,30
,1,2,3,4,5,6,7,8,9,40
,1,2,3,4,5,6,7,8,9,50
,1,2,3,4,5,6,7,8,9,60
,1,2,3,4
};

uint8_t sent2[EEPROM_PAGE_SIZE_BYTES]=
{'A','B','C','D','E','F','G','H','I','J'
,'A','B','C','D','E','F','G','H','I','J'
,'A','B','C','D','E','F','G','H','I','J'
,'A','B','C','D','E','F','G','H','I','J'
,'A','B','C','D','E','F','G','H','I','J'
,'A','B','C','D','E','F','G','H','I','J'
,'A','B','C','D'
};

uint8_t sent3[EEPROM_PAGE_SIZE_BYTES]=
{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25
,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,50
,1,2,3,4,5,6,7,8,9,10,11,12,13,14
};

uint8_t sent4[EEPROM_PAGE_SIZE_BYTES]=
{'A','A','A','A','A','A','A','A','A','A','A','A','A','A','A','A','A','A','A','A','A','A','A','A','A'
,'A','A','A','A','A','A','A','A','A','A','A','A','A','A','A','A','A','A','A','A','A','A','A','A','A'
,'A','A','A','A','A','A','A','A','A','A'
,'A','A','A','A'
};

uint8_t sent5[EEPROM_PAGE_SIZE_BYTES]=
{'B','B','B','B','B','B','B','B','B','B','B','B','B','B','B','B','B','B','B','B','B','B','B','B','B'
,'B','B','B','B','B','B','B','B','B','B','B','B','B','B','B','B','B','B','B','B','B','B','B','B','B'
,'B','B','B','B','B','B','B','B','B','B'
,'B','B','B','B'
};


error_t LC_EepromTest(void){
uint16_t i;
uint8_t userString[20]="Prueba de EEPROM";


uint8_t checkString[20]="";
uint16_t address_eeprom;

uint16_t address_eeprom1;
uint16_t address_eeprom2;
uint16_t address_eeprom3;
uint16_t address_eeprom4;
uint16_t address_eeprom5;
uint16_t address_eeprom6;
uint16_t address_eeprom7;

volatile statusReg_t sr;
error_t rv=errNone;




	EEPROM_ReadBuffer(received1,0,64);  //read 15 byte to the array received[] from  offset 28 of eeprom
 	EEPROM_ReadBuffer(received2,64,64);  //read 15 byte to the array received[] from  offset 28 of eeprom
 	EEPROM_ReadBuffer(received3,128,64);  //read 15 byte to the array received[] from  offset 28 of eeprom


    //test ubicacion de datos
	address_eeprom = offsetof(eeprom_user,idString);
	address_eeprom1 = offsetof(eeprom_user,someBytes);
	address_eeprom2 = offsetof(eeprom_user,someInts);
	address_eeprom3 = offsetof(eeprom_u,page);
	address_eeprom4 = offsetof(eeprom_u,byte);
	address_eeprom5 = offsetof(eeprom_u,userInt16);
	address_eeprom6 = offsetof(eeprom_u,userUint16);
	address_eeprom7 = offsetof(eeprom_u,userStructEeprom);

	sr.byte=EEPROM_ReadStatusReg();						//read status reg to see if we have an
														//protection set etc.

	rv=EEPROM_WriteBuffer(userString,					//the element to write
			address_eeprom,								//the address in the eeprom to write it
			sizeof(((eeprom_user *)0)->idString));		//the size of the element to write



//The EEPROM_WriteBuffer() will write accross page boundaries
//it will return errFail if there is a problem

	if(rv)
	{
	//	assert(1);
		//EEPROM_WriteBuffer(sent,28,15);
	}
	EEPROM_ReadBuffer(checkString,					//now read it back to check
			offsetof(eeprom_user,idString),
			sizeof(((eeprom_user *)0)->idString));


    //

	//clear_receivedBUFFER();
	//uint8_t *array,uint16_t addr, uint16_t num_bytes
	//EEPROM_Protect(EEPROM_ProtectNone);
	EEPROM_WriteBuffer(sent1,0,64);		//write 15 byte from the array sent[] to offset 28 of eeprom
	EEPROM_WriteBuffer(sent2,64,64);		//write 15 byte from the array sent[] to offset 28 of eeprom
	EEPROM_WriteBuffer(sent3,128,64);		//write 15 byte from the array sent[] to offset 28 of eeprom

	//EEPROM_WriteBuffer(sent4,0,64);		//write 15 byte from the array sent[] to offset 28 of eeprom
	//EEPROM_WriteBuffer(sent4,64,64);		//write 15 byte from the array sent[] to offset 28 of eeprom
	//EEPROM_WriteBuffer(sent4,128,64);		//write 15 byte from the array sent[] to offset 28 of eeprom


	//EEPROM_Protect(EEPROM_ProtectAll);

	EEPROM_ReadBuffer(received1,0,64);  //read 15 byte to the array received[] from  offset 28 of eeprom
	EEPROM_ReadBuffer(received2,64,64);  //read 15 byte to the array received[] from  offset 28 of eeprom
	EEPROM_ReadBuffer(received3,128,64);  //read 15 byte to the array received[] from  offset 28 of eeprom

	WriteBuffer_EEPROM(sent5,0,64);

}

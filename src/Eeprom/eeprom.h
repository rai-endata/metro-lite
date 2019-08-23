/*
 * eeprom.h
 *
 *  Created on: 5/5/2018
 *      Author: Rai
 */

#ifndef __EEPROM__

#define __EEPROM__

#include "stddef.h"
#include "stdint.h"

#include "driver_eeprom.h"
#include "Definicion de tipos.h"
#include "rtc.h"
#include "tipos - parametros reloj.h"

#include "eeprom_aux.h"
#include "eeprom address.h"

/*
	typedef enum{
		 EEPROM_OK,                        // No hay error
		 EEPROM_ACCESS_ERROR,              // Error de Acceso
		 EEPROM_PROTECTION_VIOLATION,      // Error de Violacion de Proteccion
		 EEPROM_noMult4,                   // Error longitud a grabar no multiplo de 4
		 EEPROM_ERROR_MASK                 // Error en la mascara de grabacion
	}tEEPROM_ERROR;
*
  typedef union
   {
    	uint8_t 		page[EEPROM_PAGE_SIZE_BYTES][EEPROM_SIZE/EEPROM_PAGE_SIZE_BYTES];			//access by page
    	uint8_t			byte[EEPROM_SIZE];			//access by byte
    	uint16_t		word16[EEPROM_SIZE/2];		//access by word 16
    	uint32_t		word32[EEPROM_SIZE/4];		//access by word 32
    	backupEEPROM	dataBACKUP;				    //access by user struct
    }eepromDATA;

    typedef struct {
    	backupEEPROM	dataBACKUP;
    	tPROG			progEEPROM;
       }tEEPROM;


*/


/************************ ADDRES SIZE PROG RELOJ *******************************************************/
#define	ADDR_BASE_PROG_PART1				ADDR_EEPROM_PAGE_1
#define	ADDR_BASE_PROG_PART2				ADDR_EEPROM_PAGE_2
#define	ADDR_BASE_PROG_PART3				ADDR_EEPROM_PAGE_3
#define	ADDR_BASE_PROG_PART4				ADDR_EEPROM_PAGE_4

#define	ADDR_BASE_VARIABLES			     	ADDR_EEPROM_PAGE_10			//AQUI GUARDO VARIABLES DE DISTINTO TIPO

#define	ADDR_DATA1_EEPROM 					ADDR_BASE_VARIABLES
#define	ADDR_DATA2_EEPROM 					ADDR_BASE_VARIABLES + 1
#define	ADDR_DATA3_EEPROM 					ADDR_BASE_VARIABLES + 2
#define	ADDR_DATA4_EEPROM 					ADDR_BASE_VARIABLES + 3
#define	ADDR_DATA5_EEPROM 					ADDR_BASE_VARIABLES + 4
#define	ADDR_DATA6_EEPROM 					ADDR_BASE_VARIABLES + 5
#define	ADDR_DATA7_EEPROM 					ADDR_BASE_VARIABLES + 6
#define	ADDR_DATA8_EEPROM 					ADDR_BASE_VARIABLES + 7
#define	ADDR_DATA9_EEPROM 					ADDR_BASE_VARIABLES + 8
#define	ADDR_DATA10_EEPROM 					ADDR_BASE_VARIABLES + 9

#define	ADDR_EEPROM_PRIMER_ENCENDIDO		ADDR_BASE_VARIABLES + 0
#define	SIZE_EEPROM_PRIMER_ENCENDIDO		sizeof(uint16_t)

#define	ADDR_BASE_VAR_REPORTES				ADDR_EEPROM_PAGE_20
#define	ADDR_BASE_TABLA_REPORTE				ADDR_EEPROM_PAGE_21

#define	ADDRESS_PROG_relojCOMUN     		ADDR_BASE_PROG_PART1
#define	SIZE_PROG_relojCOMUN        	    sizeof(((tEEPROM *)0)->progEEPROM.PROG_relojCOMUN)
#define	ADDRESS_PROG_relojT1D 				ADDR_BASE_PROG_PART1 + SIZE_PROG_relojCOMUN
#define	SIZE_PROG_relojT1D   	    		sizeof(((tEEPROM *)0)->progEEPROM.PROG_relojT1D)
#define	ADDRESS_PROG_relojT2D 				ADDR_BASE_PROG_PART1 + SIZE_PROG_relojCOMUN + SIZE_PROG_relojT1D
#define	SIZE_PROG_relojT2D   	    		sizeof(((tEEPROM *)0)->progEEPROM.PROG_relojT2D)
#define	ADDRESS_PROG_relojT3D 				ADDR_BASE_PROG_PART1 + SIZE_PROG_relojCOMUN + SIZE_PROG_relojT1D + SIZE_PROG_relojT2D
#define	SIZE_PROG_relojT3D   	    		sizeof(((tEEPROM *)0)->progEEPROM.PROG_relojT3D)
#define	ADDRESS_PROG_relojT4D 				ADDR_BASE_PROG_PART1 + SIZE_PROG_relojCOMUN + SIZE_PROG_relojT1D + SIZE_PROG_relojT2D + SIZE_PROG_relojT3D
#define	SIZE_PROG_relojT4D   	    		sizeof(((tEEPROM *)0)->progEEPROM.PROG_relojT4D)

#define	ADDRESS_PROG_relojT1N 				ADDR_BASE_PROG_PART2
#define	SIZE_PROG_relojT1N   	    		sizeof(((tEEPROM *)0)->progEEPROM.PROG_relojT1N)
#define	ADDRESS_PROG_relojT2N 				ADDR_BASE_PROG_PART2 + SIZE_PROG_relojT1N
#define	SIZE_PROG_relojT2N   	    		sizeof(((tEEPROM *)0)->progEEPROM.PROG_relojT2N)
#define	ADDRESS_PROG_relojT3N 				ADDR_BASE_PROG_PART2 + SIZE_PROG_relojT1N + SIZE_PROG_relojT2N
#define	SIZE_PROG_relojT3N   	    		sizeof(((tEEPROM *)0)->progEEPROM.PROG_relojT3N)
#define	ADDRESS_PROG_relojT4N 				ADDR_BASE_PROG_PART2 + SIZE_PROG_relojT1N + SIZE_PROG_relojT2N + SIZE_PROG_relojT3N
#define	SIZE_PROG_relojT4N   	    		sizeof(((tEEPROM *)0)->progEEPROM.PROG_relojT4N)

#define	ADDRESS_PROG_relojEqPESOS 			ADDR_BASE_PROG_PART2 + SIZE_PROG_relojT1N + SIZE_PROG_relojT2N + SIZE_PROG_relojT3N  + SIZE_PROG_relojT4N
#define	SIZE_PROG_relojEqPESOS   	    	sizeof(((tEEPROM *)0)->progEEPROM.PROG_relojEqPESOS)

#define	ADDRESS_PROG_relojCALEND 			ADDRESS_PROG_relojEqPESOS + SIZE_PROG_relojEqPESOS
#define	SIZE_PROG_relojCALEND   	    	sizeof(((tEEPROM *)0)->progEEPROM.PROG_relojCALEND)


#define	SIZE_PROG_RELOJ_EEPROM 	(SIZE_PROG_relojCOMUN+					\
								 SIZE_PROG_relojT1D+					\
								 SIZE_PROG_relojT2D+					\
								 SIZE_PROG_relojT3D+					\
								 SIZE_PROG_relojT4D+					\
								 SIZE_PROG_relojT1N+					\
								 SIZE_PROG_relojT2N+					\
								 SIZE_PROG_relojT3N+					\
								 SIZE_PROG_relojT4N+					\
								 SIZE_PROG_relojEqPESOS+				\
								 SIZE_PROG_relojCALEND)


/* *********************** ADDRES SIZE BACKUP ***********************************************************/

#define	ADDRESS_BACKUP_EEPROM  				offsetof(tEEPROM,dataBACKUP)
#define	ADDRESS_DATE_APAGADO_EEPROM  		offsetof(tEEPROM,dataBACKUP.date)
#define	SIZE_DATE_APAGADO_EEPROM  			sizeof(((tEEPROM *)0)->dataBACKUP.date)
#define	ADDRESS_TARIFA_EEPROM  				offsetof(tEEPROM,dataBACKUP.tarifa)
#define	SIZE_TARIFA_EEPROM  				sizeof(((tEEPROM *)0)->dataBACKUP.tarifa)
#define	ADDRESS_ESTADO_RELOJ_EEPROM  		offsetof(tEEPROM,dataBACKUP.estado_reloj)
#define	SIZE_ESTADO_RELOJ_EEPROM  			sizeof(((tEEPROM *)0)->dataBACKUP.estado_reloj)
#define	ADDRESS_FICHAS_PULSOS_EEPROM  		offsetof(tEEPROM,dataBACKUP.fichas_pulsos)
#define	SIZE_FICHAS_PULSOS_EEPROM  			sizeof(((tEEPROM *)0)->dataBACKUP.fichas_pulsos)
#define	ADDRESS_FICHAS_TIEMPO_EEPROM  		offsetof(tEEPROM,dataBACKUP.fichas_tiempo)
#define	SIZE_FICHAS_TIEMPO_EEPROM  			sizeof(((tEEPROM *)0)->dataBACKUP.fichas_tiempo)
#define	ADDRESS_CONTADOR_PULSOS_EEPROM  	offsetof(tEEPROM,dataBACKUP.contador_pulsos)
#define	SIZE_CONTADOR_PULSOS_EEPROM  		sizeof(((tEEPROM *)0)->dataBACKUP.contador_pulsos)
#define	ADDRESS_CONTADOR_PULSOS_OLD_EEPROM  offsetof(tEEPROM,dataBACKUP.contador_pulsos_old)
#define	SIZE_CONTADOR_PULSOS_OLD_EEPROM  	sizeof(((tEEPROM *)0)->dataBACKUP.contador_pulsos_old)
#define	ADDRESS_SEG_ESPERA_EEPROM  				offsetof(tEEPROM,dataBACKUP.segundos_espera)
#define	SIZE_SEG_ESPERA_EEPROM 					sizeof(((tEEPROM *)0)->dataBACKUP.segundos_espera)
#define	ADDRESS_SEG_TARIF_EEPROM  				offsetof(tEEPROM,dataBACKUP.segundos_tarifacion)
#define	SIZE_SEG_TARIF_EEPROM 					sizeof(((tEEPROM *)0)->dataBACKUP.segundos_tarifacion)
#define	ADDRESS_DIS_OCUP_EEPROM  				offsetof(tEEPROM,dataBACKUP.distancia_ocupado)
#define	SIZE_DIS_OCUP_EEPROM 					sizeof(((tEEPROM *)0)->dataBACKUP.distancia_ocupado)
#define	ADDRESS_OCUP_DATE_EEPROM  				offsetof(tEEPROM,dataBACKUP.ocupado_date)
#define	SIZE_OCUP_DATE_EEPROM 					sizeof(((tEEPROM *)0)->dataBACKUP.ocupado_date)
#define	ADDRESS_COB_DATE_EEPROM  				offsetof(tEEPROM,dataBACKUP.cobrando_date)
#define	SIZE_COB_DATE_EEPROM 					sizeof(((tEEPROM *)0)->dataBACKUP.cobrando_date)
#define	ADDRESS_VEL_MAX_EEPROM  				offsetof(tEEPROM,dataBACKUP.velocidad_maxima)
#define	SIZE_VEL_MAX_EEPROM 					sizeof(((tEEPROM *)0)->dataBACKUP.velocidad_maxima)
#define	ADDRESS_CORTE_ALIM_EEPROM  				offsetof(tEEPROM,dataBACKUP.tiempo_corte_alimentacion)
#define	SIZE_CORTE_ALIM_EEPROM 					sizeof(((tEEPROM *)0)->dataBACKUP.tiempo_corte_alimentacion)
#define	ADDRESS_BYTE_FILL1_EEPROM  				offsetof(tEEPROM,dataBACKUP.BYTE_TO_FILL1)
#define	SIZE_BYTE_FILL1_EEPROM 					sizeof(((tEEPROM *)0)->dataBACKUP.BYTE_TO_FILL1)
#define	ADDRESS_WORD_FILL1_EEPROM  				offsetof(tEEPROM,dataBACKUP.WORD_TO_FILL1)
#define	SIZE_WORD_FILL1_EEPROM 					sizeof(((tEEPROM *)0)->dataBACKUP.WORD_TO_FILL1)

#define	SIZE_BACKUP_EEPROM 	(SIZE_DATE_APAGADO_EEPROM +			\
 	 	 	 	 	 	 	 SIZE_TARIFA_EEPROM +				\
							 SIZE_ESTADO_RELOJ_EEPROM +			\
							 SIZE_FICHAS_PULSOS_EEPROM +		\
							 SIZE_FICHAS_TIEMPO_EEPROM +		\
							 SIZE_CONTADOR_PULSOS_EEPROM +		\
							 SIZE_CONTADOR_PULSOS_OLD_EEPROM +	\
							 SIZE_SEG_ESPERA_EEPROM +			\
							 SIZE_SEG_TARIF_EEPROM + 			\
							 SIZE_DIS_OCUP_EEPROM + 			\
 	 	 	 	 	 	 	 SIZE_OCUP_DATE_EEPROM + 			\
							 SIZE_COB_DATE_EEPROM + 			\
							 SIZE_VEL_MAX_EEPROM +				\
							 SIZE_BYTE_FILL1_EEPROM +			\
							 SIZE_WORD_FILL1_EEPROM +			\
 	 	 	 	 	 	 	 SIZE_CORTE_ALIM_EEPROM)


//el tamaño de PROG_TICKET ES DE 220 POR LO TANTO OCUPA 2 PAGINAS (220 -128 = 92)
#define	ADDRESS_PROG_TICKET     		ADDR_BASE_PROG_PART3+174 							//SUMO 174 PORQUE A PARTIR DE AHI NO FUNCIONA, NO SE PORQUE QUIZAS ESTE FALLANDO LA EEPROM. ESTO ME LLEVA A PAGINA  PAGINA 4
#define	SIZE_PROG_TICKET        	    sizeof(((tEEPROM *)0)->progEEPROM.PROG_TICKET)

//ADDRESS_PROG_TICKET_RECAUD ya esta en PAGINA 6
#define	ADDRESS_PROG_TICKET_RECAUD 		ADDRESS_PROG_TICKET + SIZE_PROG_TICKET
#define	SIZE_PROG_TICKET_RECAUD    	    sizeof(((tEEPROM *)0)->progEEPROM.PROG_TICKET_RECAUD)


#define	ADDRESS_PROG_MOVIL     		ADDRESS_PROG_TICKET_RECAUD + SIZE_PROG_TICKET_RECAUD
#define	SIZE_PROG_MOVIL    	        sizeof(((tEEPROM *)0)->progEEPROM.PROG_MOVIL)


#define	EEPROM_NRO_CORRELATIVO		ADDR_BASE_VARIABLES + 10
#define	EEPROM_NRO_TICKET			ADDR_BASE_VARIABLES + 11




 /* ESTRUCTURAS */
 /***************/

// BANDERAS DE EEPROM
   typedef union{
     byte Byte;
     struct{
       byte reset_F      :1;           // Indica que se debe grabar el MOTIVO DE RESET
       byte bootloader_F :1;           // Indica que se debe grabar el ENTER BOOTLOADER
     }Bits;
   }tEEPROM_FLAG;


   // TIPO DE DATOS DE EEPROM
   typedef union{
     dword LWord;
     struct{
       dword byte1 :8;
       dword byte2 :8;
       dword byte3 :8;
       dword byte4 :8;
     }Bits;

     struct{
       dword word1 :16;
       dword word2 :16;
     }MergedBits;
   }tEEPROM_DATA;



 /* VARIABLES & MACROS */
 /**********************/
   // Fin de datos de EEPROM tanto para escribir como para leer
   #define finEEPROM_H   0xDF
   #define finEEPROM_L   0x0A
   #define finEEPROM_HL  ((finEEPROM_H << 8) | finEEPROM_L)


 /* RUTINAS */
 /***********/
   extern void EEPROM_ini (byte initee);
   extern void EEPROM_iniGrabacion (void);
   extern void EEPROM_chkRequest (byte force);
     #define EEPROM_chkRequest_normal      (EEPROM_chkRequest(0))
     #define EEPROM_chkRequest_forced      (EEPROM_chkRequest(1))

   extern tEEPROM_ERROR grabar_byte_EEPROM (byte dato, uint16_t* EEPROM_ptr, byte mask);
   extern tEEPROM_ERROR grabar_word_EEPROM (uint16_t dato, uint16_t* EEPROM_ptr, byte mask);
   extern tEEPROM_ERROR grabar_buffer_EEPROM (uint16_t* data_buffer, uint16_t* EEPROM_ptr, uint16_t max_size);

  void read_backup_eeprom(void);
 void write_backup_eeprom(void);
 extern void test_size(void);
 extern void testEEPROM(void );
 extern void levantar_variablesEEPROM (void);


 uint32_t EEPROM_readDouble (uint32_t* EEPROM_ptr);
 tEEPROM_ERROR EEPROM_writeDouble (uint32_t* EEPROM_ptr, uint32_t dato);
 tEEPROM_ERROR EEPROM_write (word* EEPROM_ptr, word dato);
 tEEPROM_ERROR EEPROM_clear (word* EEPROM_ptr);



#endif /* EEPROM_EEPROM_H_ */

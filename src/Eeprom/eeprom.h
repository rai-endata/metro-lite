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

//******************* ASIGNACION DE DIRECCIONES DE EEPROM **********************
//******************************************************************************

#define	ADDRESS_VARIOS     				ADDR_EEPROM_PAGE_0  // (pag0)
#define	ADDRESS_PROG1     				ADDR_EEPROM_PAGE_1  // (pag1, pag2, Y pag3)     //Datos de programacion
#define	ADDRESS_PROG2     				ADDR_EEPROM_PAGE_4  // (pag4, pag5, Y pag6)     //copia 1 datos de programacion
#define	ADDRESS_PROG3     				ADDR_EEPROM_PAGE_7  // (pag7, pag8, Y pag9)	    //copia 2 datos de programacion
#define	ADDRESS_PROG4     				ADDR_EEPROM_PAGE_10 // (pag10, pag11, Y pag12)  //copia 3 datos de programacion

#define	ADDRESS_PROG_TICKET_PAGE1			ADDR_EEPROM_PAGE_13
#define	ADDRESS_PROG_TICKET_PAGE2			ADDR_EEPROM_PAGE_13 + 128
#define	ADDRESS_PROG_TICKET_RECAUD			ADDRESS_PROG_TICKET_PAGE1 + sizeof(tPRG_TICKET)

#define	ADDRESS_PROG_TICKET_PAGE1_bck1		ADDR_EEPROM_PAGE_15
#define	ADDRESS_PROG_TICKET_PAGE2_bck1		ADDRESS_PROG_TICKET_PAGE1_bck1 + 128
#define	ADDRESS_PROG_TICKET_RECAUD_bck1		ADDRESS_PROG_TICKET_PAGE1_bck1 + sizeof(tPRG_TICKET)

#define	ADDRESS_PROG_TICKET_PAGE1_bck2		ADDR_EEPROM_PAGE_17
#define	ADDRESS_PROG_TICKET_PAGE2_bck2		ADDRESS_PROG_TICKET_PAGE1_bck2 + 128
#define	ADDRESS_PROG_TICKET_RECAUD_bck2		ADDRESS_PROG_TICKET_PAGE1_bck2 + sizeof(tPRG_TICKET)

#define	ADDRESS_PROG_TICKET_PAGE1_bck3		ADDR_EEPROM_PAGE_19
#define	ADDRESS_PROG_TICKET_PAGE2_bck3		ADDRESS_PROG_TICKET_PAGE1_bck3 + 128
#define	ADDRESS_PROG_TICKET_RECAUD_bck3		ADDRESS_PROG_TICKET_PAGE1_bck3 + sizeof(tPRG_TICKET)

#define	ADDRESS_REPORTES   					ADDR_EEPROM_PAGE_21
#define	ADDR_BASE_TABLA_REPORTE				ADDR_INDEX_LAST_SESION + SIZE_INDEX_LAST_SESION

#define	ADDR_NEXT_REPORTE 					ADDR_BASE_TABLA_REPORTE + SIZE_EEPROM_REPORTE


//  ************       Definicion de datos varios*******************************
// ******************************************************************************

#define	ADDRESS_BACKUP_EEPROM  				ADDRESS_VARIOS

	#define	ADDRESS_CORTE_ALIM_EEPROM  				ADDRESS_BACKUP_EEPROM + offsetof(tEEPROM,dataBACKUP.tiempo_corte_alimentacion)
	#define	ADDRESS_TARIFA_EEPROM  					ADDRESS_BACKUP_EEPROM + offsetof(tEEPROM,dataBACKUP.tarifa)
	#define	ADDRESS_ESTADO_RELOJ_EEPROM  			ADDRESS_BACKUP_EEPROM + offsetof(tEEPROM,dataBACKUP.estado_reloj)
	#define	ADDRESS_BYTE_FILL1_EEPROM  				ADDRESS_BACKUP_EEPROM + offsetof(tEEPROM,dataBACKUP.BYTE_TO_FILL1)
	#define	ADDRESS_VEL_MAX_EEPROM  				ADDRESS_BACKUP_EEPROM + offsetof(tEEPROM,dataBACKUP.velocidad_maxima)
	#define	ADDRESS_SEG_ESPERA_EEPROM  				ADDRESS_BACKUP_EEPROM + offsetof(tEEPROM,dataBACKUP.segundos_espera)
	#define	ADDRESS_SEG_TARIF_EEPROM  				ADDRESS_BACKUP_EEPROM + offsetof(tEEPROM,dataBACKUP.segundos_tarifacion)
	#define	ADDRESS_WORD_FILL1_EEPROM  				ADDRESS_BACKUP_EEPROM + offsetof(tEEPROM,dataBACKUP.WORD_TO_FILL1)
	#define	ADDRESS_FICHAS_PULSOS_EEPROM  			ADDRESS_BACKUP_EEPROM + offsetof(tEEPROM,dataBACKUP.fichas_pulsos)
	#define	ADDRESS_FICHAS_TIEMPO_EEPROM  			ADDRESS_BACKUP_EEPROM + offsetof(tEEPROM,dataBACKUP.fichas_tiempo)
	#define	ADDRESS_CONTADOR_PULSOS_EEPROM  		ADDRESS_BACKUP_EEPROM + offsetof(tEEPROM,dataBACKUP.contador_pulsos)
	#define	ADDRESS_CONTADOR_PULSOS_OLD_EEPROM  	ADDRESS_BACKUP_EEPROM + offsetof(tEEPROM,dataBACKUP.contador_pulsos_old)
	#define	ADDRESS_DIS_OCUP_EEPROM  				ADDRESS_BACKUP_EEPROM + offsetof(tEEPROM,dataBACKUP.distancia_ocupado)
	#define	ADDRESS_OCUP_DATE_EEPROM  				ADDRESS_BACKUP_EEPROM + offsetof(tEEPROM,dataBACKUP.ocupado_date)
	#define	ADDRESS_COB_DATE_EEPROM  				ADDRESS_BACKUP_EEPROM + offsetof(tEEPROM,dataBACKUP.cobrando_date)
	#define	ADDRESS_DATE_APAGADO_EEPROM  			ADDRESS_BACKUP_EEPROM + offsetof(tEEPROM,dataBACKUP.date)

	#define	SIZE_CORTE_ALIM_EEPROM 				sizeof(((tEEPROM *)0)->dataBACKUP.tiempo_corte_alimentacion)
	#define	SIZE_TARIFA_EEPROM  				sizeof(((tEEPROM *)0)->dataBACKUP.tarifa)
	#define	SIZE_ESTADO_RELOJ_EEPROM  			sizeof(((tEEPROM *)0)->dataBACKUP.estado_reloj)
	#define	SIZE_BYTE_FILL1_EEPROM 				sizeof(((tEEPROM *)0)->dataBACKUP.BYTE_TO_FILL1)
	#define	SIZE_VEL_MAX_EEPROM 				sizeof(((tEEPROM *)0)->dataBACKUP.velocidad_maxima)
	#define	SIZE_SEG_ESPERA_EEPROM				sizeof(((tEEPROM *)0)->dataBACKUP.segundos_espera)
	#define	SIZE_SEG_TARIF_EEPROM  				sizeof(((tEEPROM *)0)->dataBACKUP.segundos_tarifacion)
	#define	SIZE_WORD_FILL1_EEPROM 				sizeof(((tEEPROM *)0)->dataBACKUP.WORD_TO_FILL1)
	#define	SIZE_FICHAS_PULSOS_EEPROM  			sizeof(((tEEPROM *)0)->dataBACKUP.fichas_pulsos)
	#define	SIZE_FICHAS_TIEMPO_EEPROM  			sizeof(((tEEPROM *)0)->dataBACKUP.fichas_tiempo)
	#define	SIZE_CONTADOR_PULSOS_EEPROM  		sizeof(((tEEPROM *)0)->dataBACKUP.contador_pulsos)
	#define	SIZE_CONTADOR_PULSOS_OLD_EEPROM  	sizeof(((tEEPROM *)0)->dataBACKUP.contador_pulsos_old)
	#define	SIZE_DIS_OCUP_EEPROM 				sizeof(((tEEPROM *)0)->dataBACKUP.distancia_ocupado)
	#define	SIZE_OCUP_DATE_EEPROM 				sizeof(((tEEPROM *)0)->dataBACKUP.ocupado_date)
	#define	SIZE_COB_DATE_EEPROM 				sizeof(((tEEPROM *)0)->dataBACKUP.cobrando_date)
	#define	SIZE_DATE_APAGADO_EEPROM  			sizeof(((tEEPROM *)0)->dataBACKUP.date)

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


#define	ADDR_EEPROM_PRIMER_ENCENDIDO	ADDRESS_BACKUP_EEPROM + SIZE_BACKUP_EEPROM 			//AQUI GUARDO VARIABLES DE DISTINTO TIPO
#define	SIZE_EEPROM_PRIMER_ENCENDIDO	sizeof(uint16_t)

#define	EEPROM_NRO_CORRELATIVO			ADDR_EEPROM_PRIMER_ENCENDIDO + SIZE_EEPROM_PRIMER_ENCENDIDO
#define	SIZE_EEPROM_NRO_CORRELATIVO		sizeof(uint8_t)

#define	EEPROM_NRO_TICKET			    EEPROM_NRO_CORRELATIVO + SIZE_EEPROM_NRO_CORRELATIVO
#define	SIZE_EEPROM_NRO_TICKET		    sizeof(uint32_t)

#define	ADDR_EEPROM_CHOFER			    EEPROM_NRO_TICKET + SIZE_EEPROM_NRO_TICKET
#define	SIZE_EEPROM_CHOFER			    sizeof(uint16_t)

// Direccion reportes
#define	ADDR_EEPROM_REPORTE_PUT			ADDRESS_REPORTES
#define	SIZE_EEPROM_REPORTE_PUT			sizeof(uint32_t)

#define	ADDR_EEPROM_REPORTE_INDEX 		ADDR_EEPROM_REPORTE_PUT + SIZE_EEPROM_REPORTE_PUT
#define	SIZE_EEPROM_REPORTE_INDEX		sizeof(uint16_t)

#define	ADDR_EEPROM_REPORTE_NRO_VIAJE	ADDR_EEPROM_REPORTE_INDEX + SIZE_EEPROM_REPORTE_INDEX
#define	SIZE_EEPROM_REPORTE_NRO_VIAJE	sizeof(uint16_t)

#define	ADDR_EEPROM_REPORTE_NRO_TURNO	ADDR_EEPROM_REPORTE_NRO_VIAJE + SIZE_EEPROM_REPORTE_NRO_VIAJE
#define	SIZE_EEPROM_REPORTE_NRO_TURNO	sizeof(uint16_t)

#define	ADDR_INDEX_LAST_SESION 			ADDR_EEPROM_REPORTE_NRO_TURNO + SIZE_EEPROM_REPORTE_NRO_TURNO
#define	SIZE_INDEX_LAST_SESION			sizeof(uint16_t)

//************  Definicion de datos de programacion - ocupa 3 paginas  **************
//***********************************************************************************

//datos de programacion del reloj
#define	ADDRESS_PROG_relojCOMUN     		ADDRESS_PROG1
#define	SIZE_PROG_relojCOMUN        	    sizeof(((tEEPROM *)0)->progEEPROM.PROG_relojCOMUN)
#define	ADDRESS_PROG_relojT1D 				ADDRESS_PROG1 + SIZE_PROG_relojCOMUN
#define	SIZE_PROG_relojT1D   	    		sizeof(((tEEPROM *)0)->progEEPROM.PROG_relojT1D)
#define	ADDRESS_PROG_relojT2D 				ADDRESS_PROG1 + SIZE_PROG_relojCOMUN + SIZE_PROG_relojT1D
#define	SIZE_PROG_relojT2D   	    		sizeof(((tEEPROM *)0)->progEEPROM.PROG_relojT2D)
#define	ADDRESS_PROG_relojT3D 				ADDRESS_PROG1 + SIZE_PROG_relojCOMUN + SIZE_PROG_relojT1D + SIZE_PROG_relojT2D
#define	SIZE_PROG_relojT3D   	    		sizeof(((tEEPROM *)0)->progEEPROM.PROG_relojT3D)
#define	ADDRESS_PROG_relojT4D 				ADDRESS_PROG1 + SIZE_PROG_relojCOMUN + SIZE_PROG_relojT1D + SIZE_PROG_relojT2D + SIZE_PROG_relojT3D
#define	SIZE_PROG_relojT4D   	    		sizeof(((tEEPROM *)0)->progEEPROM.PROG_relojT4D)

//12 bytes libres

//cambio a la pagina 2 para que ADDRESS_PROG_relojT4D no quede partida en dos paginas (0 y 1)(ya que generaria error en las rutinas de lectura y escritura de la spi)
#define	ADDRESS_PROG_relojT1N 				ADDRESS_PROG1 + 128
#define	SIZE_PROG_relojT1N   	    		sizeof(((tEEPROM *)0)->progEEPROM.PROG_relojT1N)
#define	ADDRESS_PROG_relojT2N 				ADDRESS_PROG1 + 128 + SIZE_PROG_relojT1N
#define	SIZE_PROG_relojT2N   	    		sizeof(((tEEPROM *)0)->progEEPROM.PROG_relojT2N)
#define	ADDRESS_PROG_relojT3N 				ADDRESS_PROG1 + 128 + SIZE_PROG_relojT1N + SIZE_PROG_relojT2N
#define	SIZE_PROG_relojT3N   	    		sizeof(((tEEPROM *)0)->progEEPROM.PROG_relojT3N)
#define	ADDRESS_PROG_relojT4N 				ADDRESS_PROG1 + 128 + SIZE_PROG_relojT1N + SIZE_PROG_relojT2N + SIZE_PROG_relojT3N
#define	SIZE_PROG_relojT4N   	    		sizeof(((tEEPROM *)0)->progEEPROM.PROG_relojT4N)

#define	ADDRESS_PROG_relojEqPESOS 			ADDRESS_PROG1 + 128 + SIZE_PROG_relojT1N + SIZE_PROG_relojT2N + SIZE_PROG_relojT3N  + SIZE_PROG_relojT4N
//#define	SIZE_PROG_relojEqPESOS   	    	sizeof(((tEEPROM *)0)->progEEPROM.PROG_relojEqPESOS)
#define	SIZE_PROG_relojEqPESOS   	    	sizeof(((tPARAM_RELOJ_EQPESOS *)0)->hab)      +  \
											sizeof(((tPARAM_RELOJ_EQPESOS *)0)->diurna)   +  \
											sizeof(((tPARAM_RELOJ_EQPESOS *)0)->nocturna) +  \
											sizeof(((tPARAM_RELOJ_EQPESOS *)0)->checksum) +  \
											sizeof(((tPARAM_RELOJ_EQPESOS *)0)->finDATA)
// quedan 2 bytes linres en pagina ..

//cambio a la pagina 3 para que PROG_relojCALEND no quede partida en dos paginas (2 y 3)(ya que generaria error en las rutinas de lectura y escritura de la spi)
#define	ADDRESS_PROG_relojCALEND 			ADDRESS_PROG1 + 2*128
#define	SIZE_PROG_relojCALEND   	    	sizeof(((tEEPROM *)0)->progEEPROM.PROG_relojCALEND)

//datos de programacion del movil
#define	ADDRESS_PROG_MOVIL     		ADDRESS_PROG_relojCALEND + SIZE_PROG_relojCALEND
#define	SIZE_PROG_MOVIL    	        sizeof(((tEEPROM *)0)->progEEPROM.PROG_MOVIL)

#define ADDRESS_PROG_relojCOMUN_bck1  	ADDRESS_PROG2
#define ADDRESS_PROG_relojCOMUN_bck2  	ADDRESS_PROG3
#define ADDRESS_PROG_relojCOMUN_bck3  	ADDRESS_PROG4

#define	ADDRESS_PROG_relojEqPESOS_bck1	ADDRESS_PROG2 + 128 + SIZE_PROG_relojT1N + SIZE_PROG_relojT2N + SIZE_PROG_relojT3N  + SIZE_PROG_relojT4N
#define	ADDRESS_PROG_relojEqPESOS_bck2	ADDRESS_PROG3 + 128 + SIZE_PROG_relojT1N + SIZE_PROG_relojT2N + SIZE_PROG_relojT3N  + SIZE_PROG_relojT4N
#define	ADDRESS_PROG_relojEqPESOS_bck3	ADDRESS_PROG4 + 128 + SIZE_PROG_relojT1N + SIZE_PROG_relojT2N + SIZE_PROG_relojT3N  + SIZE_PROG_relojT4N

#define	ADDRESS_PROG_relojCALEND_bck1	ADDRESS_PROG2 + 2*128
#define	ADDRESS_PROG_MOVIL_bck1    		ADDRESS_PROG_relojCALEND_bck1 + SIZE_PROG_relojCALEND

#define	ADDRESS_PROG_relojCALEND_bck2	ADDRESS_PROG3 + 2*128
#define	ADDRESS_PROG_MOVIL_bck2    		ADDRESS_PROG_relojCALEND_bck2 + SIZE_PROG_relojCALEND

#define	ADDRESS_PROG_relojCALEND_bck3	ADDRESS_PROG4 + 2*128
#define	ADDRESS_PROG_MOVIL_bck3    		ADDRESS_PROG_relojCALEND_bck3 + SIZE_PROG_relojCALEND



//quedan 74 bytes libres en pagina ..

//  ************       Definicion de datos de ticket en paginas 3 paginas ***********************
// ***********************************************************************************************
#define	SIZE_PROG_TICKET        	    sizeof(((tEEPROM *)0)->progEEPROM.PROG_TICKET)
#define	SIZE_PROG_TICKET_RECAUD    	    sizeof(((tEEPROM *)0)->progEEPROM.PROG_TICKET_RECAUD)


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
   extern tEEPROM_ERROR grabar_buffer_EEPROM_TICKET (uint16_t* data_buffer, uint16_t* EEPROM_ptr, uint16_t max_size);

	 void read_backup_eeprom(void);
	 void write_backup_eeprom(void);
	 extern void test_size(void);

	 extern void levantar_variablesEEPROM (void);


	 uint32_t EEPROM_readDouble (uint32_t* EEPROM_ptr);
	 tEEPROM_ERROR EEPROM_writeDouble (uint32_t* EEPROM_ptr, uint32_t dato);
	 tEEPROM_ERROR EEPROM_write (word* EEPROM_ptr, word dato);
	 tEEPROM_ERROR EEPROM_clear (word* EEPROM_ptr);

	 extern void borrar_EEPROM (void);
	 extern void testAddress(void);

#endif /* EEPROM_EEPROM_H_ */

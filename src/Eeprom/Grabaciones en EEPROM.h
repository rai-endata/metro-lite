/* File: <Grabaciones en EEPROM.h> */

#ifndef _GRABACIONES_EEPROM_
#define _GRABACIONES_EEPROM_

  /* INCLDUE */
  /***********/

    #include "eeprom.h"
    #include "Programacion Parametros.h"
	#include "Parametros Ticket.h"
	//#include "Parametros Reloj.h"


  
    


  #ifdef VISOR_PROG_TICKET    
    extern const tPRG_TICKET EEPROM_PROG_TICKET;
    extern const tPRG_TICKET_RECAUD EEPROM_PROG_TICKET_RECAUD;
  #endif

  

    extern  byte EEPROM_SERIAL_ASIENTO[];
    
  //  extern tEEPROM_DATA             EEPROM_VARS1;
     // #define EEPROM_BUZON_MSJ            EEPROM_VARS1.Bits.byte1           // Buzon de Mensajes
     // #define EEPROM_BUZON_MSJ_msk        1                                 // Mascara de Buzon de Mensajes dentro de los 4bytes
     // #define EEPROM_RESET                EEPROM_VARS1.Bits.byte2           // Indica MOTIVO de RESET
    //  #define EEPROM_RESET_msk            2                                 // Mascara de MOTIVO RESET dentro de los 4bytes
     // #define EEPROM_NRO_CORRELATIVO      EEPROM_VARS1.Bits.byte3           // Indica NUMERO CORRELATIVO DE VIAJE
     // #define EEPROM_NRO_CORRELATIVO_msk  3                                 // Mascara de NUMERO CORRELATIVO DE VIAJE dentro de los 4bytes
     // #define EEPROM_VOLUMEN              EEPROM_VARS1.Bits.byte4           // Indica VOLUMEN
     // #define EEPROM_VOLUMEN_msk          4                                 // Mascara de VOLUMEN dentro de los 4bytes
    
    extern tEEPROM_DATA             EEPROM_VARS2;
     // #define EEPROM_BUZZER_VOLUME        EEPROM_VARS2.Bits.byte1           // Volumen de Buzzer
     // #define EEPROM_BUZZER_VOLUME_msk    1                                 // Mascara de Volumen de Buzzer dentro de los 4bytes
     // #define EEPROM_CONTRASTE            EEPROM_VARS2.Bits.byte2           // Contraste de Display
     // #define EEPROM_CONTRASTE_msk        2                                 // Mascara de Contraste de Display dentro de los 4bytes
     // #define EEPROM_MAGNIFY              EEPROM_VARS2.Bits.byte3           // Magnificar Fuente
     // #define EEPROM_MAGNIFY_msk          3                                 // Mascara de Magnificar Fuente dentro de los 4bytes

   // extern  dword EEPROM_VARIOS_DESCONEXION_ACC;
   // extern  dword EEPROM_IMPORTE_DESCONEXION_ACC;
   // extern  dword EEPROM_DATE_DESCONEXION_ACC[];
   // extern  dword EEPROM_LOCALIDAD;
   // extern  dword EEPROM_DESCONEXIONES_TOTALES[5];


    /*
    EEPROM_DESCONEXIONES_TOTALES[5]
      desconexiones 2 bytes
      tarifaPerdida 1 byte
      fichasDPerdid 4 bytes
      fichasTPerdid 4 bytes
      dineroPerdido 4 bytes
      0xdf0a        2 byte
      ----------------------
      total        17 bytes
    */

  
  /* VARIABLES & MACROS */
  /**********************/
    // BANDERAS DE EEPROM
    extern tEEPROM_FLAG                   _EEPROM_F;
   //   #define EEPROM_F                    _EEPROM_F.Byte
   //   #define EEPROM_grabarRESET_F        _EEPROM_F.Bits.reset_F
      

  extern byte no_grabo_nada;      

  
  /* RUTINAS */
  /***********/
    extern tEEPROM_ERROR EEPROM_grabaciones (void);
    

#endif

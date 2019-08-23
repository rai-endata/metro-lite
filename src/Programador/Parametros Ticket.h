/* File: <Parametros Ticket.h> */

#ifndef _PARAMETROS_TICKET_
  #define _PARAMETROS_TICKET_
  
  /* INCLUDE */
  /***********/
    #include "- metroBLUE Config -.h"
   // #include "EEPROM.h"


 
  
  /* ESTRUCTURAS */
  /***************/
    // PARAMETROS DE TICKET
    // Los buffers comienzan con el ID de campo, seguido del indicador de FONT
    // LA LONGITUD TOTAL DE LA ESTRUCTURA DEBE SER MULTIPLO DE 4
    // Tamaño total, 220
    //
    // EDIT 21/12/12 BAKTUN
    //  La cantidad maxima de caracteres que entran en el TICKET son 19
    // Ademas, se agrego un byte mas para el ID del campo
    typedef struct{
      byte LEYENDA_1[19 + 1 + 1 + 1];       // ID + FONT + Leyenda 1 + fin_cadena
      byte LEYENDA_2[19 + 1 + 1 + 1];       // ID + FONT + Leyenda 2 + fin_cadena
      byte LEYENDA_3[19 + 1 + 1 + 1];       // ID + FONT + Leyenda 3 + fin_cadena
      byte LOCALIDAD[19 + 1 + 1 + 1];       // ID + FONT + Localidad + fin_cadena
      byte SERVICIO[19 + 1 + 1 + 1];        // ID + FONT + Tipo Servicio + fin_cadena

      byte TITULAR[20 + 1 + 1 + 1];         // ID + FONT + Titular + fin_cadena
      byte CUIT[14 + 1 + 1 + 1];            // ID + FONT + CUIT + fin_cadena
      byte MARCA_MODELO[19 + 1 + 1 + 1];    // ID + FONT + Marca y Modelo + fin_cadena
      byte PATENTE[10 + 1 + 1 + 1];         // ID + FONT + Patente + fin_cadena
      byte LICENCIA[10 + 1 + 1 + 1];        // ID + FONT + Licencia + fin_cadena
      byte IVA[15 + 1 + 1 + 1];             // ID + FONT + IVA + fin_cadena
      byte finEEPROM[2];                    // Fin de EEPROM
      byte makeItMod4[2];                   // Para completar tamaño multiplo de 4
    }tPRG_TICKET;   
    
    
    // PARAMETROS DE TICKET RECAUDACION
    // Los buffers comienzan con el indicador de FONT
    // LA LONGITUD TOTAL DE LA ESTRUCTURA DEBE SER MULTIPLO DE 4
    typedef struct{
      byte chofer;
      byte finEEPROM[2];                    // Fin de EEPROM
      byte makeItMod4[1];                   // Para completar tamaño multiplo de 4
    }tPRG_TICKET_RECAUD;   
   
    
  /* VARIABLES */
  /*************/
    #define TICKET_fontNormal     0xC7
    #define TICKET_fontNegrita    0xFC
    
    #define TICKET_noRecaud       0x00
    
    #define PROG_TICKET_getLeyenda_1      ((byte*) &EEPROM_PROG_TICKET.LEYENDA_1[1])
    #define PROG_TICKET_getLeyenda_2      ((byte*) &EEPROM_PROG_TICKET.LEYENDA_2[1])
    #define PROG_TICKET_getLeyenda_3      ((byte*) &EEPROM_PROG_TICKET.LEYENDA_3[1])
    #define PROG_TICKET_getLocalidad      ((byte*) &EEPROM_PROG_TICKET.LOCALIDAD[1])
    #define PROG_TICKET_getServicio       ((byte*) &EEPROM_PROG_TICKET.SERVICIO[1])
    #define PROG_TICKET_getTitular        ((byte*) &EEPROM_PROG_TICKET.TITULAR[1])
    #define PROG_TICKET_getCUIT           ((byte*) &EEPROM_PROG_TICKET.CUIT[1])
    #define PROG_TICKET_getMarcaModelo    ((byte*) &EEPROM_PROG_TICKET.MARCA_MODELO[1])
    #define PROG_TICKET_getPatente        ((byte*) &EEPROM_PROG_TICKET.PATENTE[1])
    #define PROG_TICKET_getLicencia       ((byte*) &EEPROM_PROG_TICKET.LICENCIA[1])
    #define PROG_TICKET_getIVA            ((byte*) &EEPROM_PROG_TICKET.IVA[1])
                                          


    #define EEPROMsize_PROG_TICKET       		(sizeof(tPRG_TICKET))
	#define EEPROMsize_PROG_TICKET_RECAUD		(sizeof(tPRG_TICKET_RECAUD))

  /* RUTINAS */
  /***********/      
    extern void prgTICKET_ini (void);
      
    extern void PROG_readTICKET (byte N);
    extern void PROG_writeTICKET (byte N);
    
    extern void PROG_readTICKET_RECAUD (byte N);
    extern void PROG_writeTICKET_RECAUD (byte N);
    
  
  /* EEPROM */
  /**********/
    // PARAMETROS TICKET
    extern void PROG_TICKET_to_EEPROM (byte iniGrabacion);
    extern tEEPROM_ERROR PROG_TICKET_grabarEEPROM (void);
    
    // PARAMETROS TICKET RECAUDACION
    extern void PROG_TICKET_RECAUD_to_EEPROM (byte iniGrabacion);
    extern tEEPROM_ERROR PROG_TICKET_RECAUD_grabarEEPROM (void);
#endif

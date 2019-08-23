/* File: <Grabaciones en EEPROM.c> */


/* INCLUDE */
/***********/
  #include "main.h"
  #include "eeprom.h"
  #include "Grabaciones en EEPROM.h"
  #include "Programacion Parametros.h"
  #include "Parametros Reloj.h"
  #include "Reloj.h"
  #include "Sensor Asiento.h"
  
  /*********************************************************************************************/
/* VARIABLES */
/*************/
  tEEPROM_FLAG        _EEPROM_F;        // Banderas de EEPROM
  byte no_grabo_nada;
  

#ifdef VISOR_PROG_TICKET  
  const tPRG_TICKET EEPROM_PROG_TICKET;
  const tPRG_TICKET_RECAUD EEPROM_PROG_TICKET_RECAUD;
#endif


//  byte EEPROM_SERIAL_ASIENTO[dim_SerialAsiento];
 // tEEPROM_DATA EEPROM_VARS1;
 // tEEPROM_DATA EEPROM_VARS2;
 // dword EEPROM_VARIOS_DESCONEXION_ACC;
 // dword EEPROM_IMPORTE_DESCONEXION_ACC;
 // dword EEPROM_DATE_DESCONEXION_ACC[2];

//  dword EEPROM_GPS_BAUD_RATE;
 
 //const dword EEPROM_LOCALIDAD @0xD90;
  //dword EEPROM_LOCALIDAD;
 

/*********************************************************************************************/
/* RUTINAS GRABACIONES EN EEPROM */
/*********************************/  
  tEEPROM_ERROR EEPROM_grabaciones (void){
    tEEPROM_ERROR error;
    
    no_grabo_nada=1;
    
    // Motivo Reset
   // error = EEPROM_grabarRESET();               // Grabacion de Motivo Reset en EEPROM

    // Programación de Parámetros
    error = PROGRAMADOR_grabarEEPROM();         // Grabacion de Programacion en EEPROM
    
    // Nro Correlativo de Viaje
    //error = RELOJ_nroCorrelativo_grabarEEPROM();// Grabacion de Nro Correlativo en EEPROM

    // Sensor Asiento
    //error = EEPROM_grabarSerialASIENTO();       // Grabacion de Serial de Asiento en EEPROM
    
    // Hora de Apagado (HORA GPS)
    //error = EEPROM_grabarHORA_APAGADO();        // Grabacion de Hora de Apagado en EEPROM
    
    // Datos de Desconexion Accesorio
    //error = EEPROM_grabarDESCON_ACC();          // Grabacion de Desconexion de Accesorio en EEPROM


    // Codigo Localidad
    //error = EEPROM_grabarCODELOC();             // Grabacion de Codigo de Localidad en EEPROM


    if(no_grabo_nada){
       //hago una grabacion trucha de volumen para que haya una grabacion
       //y pueda salir de la irq
       //error = grabar_byte_EEPROM(VOLUMEN, (word*)&EEPROM_VARS1, EEPROM_VOLUMEN_msk);
    }

    return(error);

  }

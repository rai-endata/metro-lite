/* File: <Parametros Movil.h> */

#ifndef _PARAMETROS_MOVIL_
  #define _PARAMETROS_MOVIL_
 #include	"stdint.h"
#include	"Definicion de tipos.h"
 #include "eeprom_e.h"

  /* INCLUDE */
  /***********/

  /* ESTRUCTURAS */
  /***************/
    // TIPO DE DATOS DE PARAMETROS DE MOVIL
    // debe tener un tamaño total multiplo de 4
    //
    //  +----|----|----|----++----|----|----|----+
    //  | Nro.Mov | Rx | Tx ||tRLJ|	DF | 0A | xx |
    //  +----|----|----|----++----|----|----|----+
    //
    // - Nro.Mov ---> Numero de Movil
    // - Rx --------> Address Rx
    // - Tx --------> Address Tx
    // - tRLJ ------> Tipo Reloj
    typedef struct{
      uint16_t mov;             // Numero de Movil
      uint8_t addressRx;       // Address (grupo) de Recepcion del Modem
      uint8_t addressTx;       // Address (grupo) de Transmision del Modem
      uint8_t tipoReloj;       // Tipo de Reloj
      uint16_t checksum;     // nro que se DEBE incrementaR secuencialmente EN LA BASE DE DATOS con los cambios de tarifa (para saber que tarifa tiene grabado)
      uint16_t finDATA;     // chksum recibido por aire (igual a suma desde el comando hasta CP2 incluidos estos)
    }tPARAM_MOVIL;

    typedef union{
        byte Byte;
        struct{
          byte                :1;
          byte portDPX        :1;         // Define si en DUPLEX se va a trabajar con portadora o pseudo-portadora
          byte logicaPortad   :1;         // Define la logica de la portadora, es decir el nivel logico q representa que HAY portadora
          byte flanco_des     :1;         // Indica si el cambio de canal es Flanco Ascendente o Descendente
          byte spxDPX         :1;         // Indica si es Simplex o Duplex
          byte _PTTexterno_   :1;         // [RESERVADO] PTT Externo
          byte sinPulsos      :1;         // Indica que no tiene conectado los pulsos
          byte auto_login     :1;         // Logueo Automatico al Sistema
        }Bits;
      }PRG_RADIO;

    // TIPO DE DATOS DE PARAMETROS DE RADIO
       // debe tener un tamaño total multiplo de 4
       //
       //  +----|----|----|----++----|----|----|----++----|----|----|----++----|----|----|----|
       //  |tRAD|dPTT|cDAT|cVOZ||chDS|tVOZ|sbRx|sbTx|| va |nMIC|SbTL| DF || 0A | xx | xx | xx |
       //  +----|----|----|----++----|----|----|----++----|----|----|----++----|----|----|----|
       //
       // - tRAD ---> Tipo Radio
       // - dPTT ---> Delay de PTT
       // - cDAT ---> Canal de Datos
       // - cVOZ ---> Canal de Voz
       // - chDS ---> Canal de Despacho Suspendido
       // - tVOZ ---> Tiempo en Canal de Voz
       // - sbRx ---> Subtono de Rx
       // - sbTx ---> Subtono de Tx
       // - va -----> Varios
       // - nMIC ---> Nivel de Microfono
       // - SbTL ---> SubTone Level de Tx (0-100%)
       typedef struct{
         byte tipoRadio;       // Tipo de Radio
         byte delayPTT;        // Retardo de PTT de la Radio
         byte chDatos;         // Numero del canal de datos en la Radio
         byte chVoz;           // Numero del canal de voz en la Radio
         byte chDS;            // Numero del canal para Despacho Suspendido
         byte tVoz;            // Tiempo en Canal de Voz (TimeOut inactividad)
         byte subToneRx;       // Subtono de Recepcion
         byte subToneTx;       // Subtono de Transmision
         PRG_RADIO varios;     // Banderas de Programacion Varias
         byte micLevel;        // Nivel del Microfono
         byte CTCSS_TxLevel;   // Nivel de Tx de subtono CTCSS
         byte fin[2];          // 2byte finEEPROM
         byte empty[3];        // tamaño multiplo de 4
       }tPARAM_RADIO;
    
  /* VARIABLES */
  /*************/
    extern tPARAM_MOVIL EEPROM_PROG_MOVIL;
    extern tPARAM_RADIO EEPROM_PROG_RADIO;

#define TIPO_RELOJ_VALIDO ((TIPO_RELOJ == BANDERITA_LIBRE_ALTO) || (TIPO_RELOJ == BANDERITA_OCUPADO_ALTO) || (TIPO_RELOJ == INTERNO))

    extern uint8_t LOGICA_BANDERITA;
		  #define OCUPADO_enALTO      0
		  #define LIBRE_enALTO        1

  
    // PARAMETROS DE MOVIL
    #define NRO_MOVIL             0x0001
      #define NRO_MOVIL_H         ((NRO_MOVIL & 0xFF00) >> 8)
      #define NRO_MOVIL_L         (NRO_MOVIL & 0x00FF)
    
    #define ADDRESS_RX            ((const)EEPROM_PROG_MOVIL.addressRx)
    #define ADDRESS_TX            ((const)EEPROM_PROG_MOVIL.addressTx)
    
    #define TIPO_RELOJ            ((tTIPORELOJ)(EEPROM_PROG_MOVIL.tipoReloj))
		
  
  /* RUTINAS */
  /***********/      
    extern void prgMOVIL_ini (void);
      
    extern void PROG_readMOVIL (uint8_t N);
    extern void PROG_writeMOVIL (uint8_t N);
    extern void PROG_writeRADIO (uint8_t N);
    extern void PROG_readRADIO (uint8_t N);

    

  /* EEPROM */
  /**********/
    // PARAMETROS MOVIL
    extern void PROG_MOVIL_to_EEPROM (uint8_t iniGrabacion);
    extern tEEPROM_ERROR PROG_MOVIL_grabarEEPROM (void);
    extern void read_MOVIL_eeprom(uint8_t* buffer_backup);
  
  /* AIR UPDATE */
  /**************/
  #ifdef VISOR_AIR_UPDATE  
    extern void PROG_saveMOVIL_air (byte* data, byte N);
    extern byte PROG_writeMOVIL_air (byte N);
  #endif
  
#endif

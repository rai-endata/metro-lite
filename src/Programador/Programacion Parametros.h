/* File <Programacion Parametros.h> */


#ifndef _PROGRAMACION_PARAMETROS_
  #define _PROGRAMACION_PARAMETROS_
  
  /* INCLUDE */
  /***********/
    #include "Protocolo BAX.h"
	//#include "eeprom.h"
 	#include "seriePROG.h"
	#include "eeprom_e.h"


  /* MACROS */
  /**********/
//EEPROMsize_PRG_relojCOMUN
	//EEPROMsize_PRG_relojTARIFA
	//EEPROMsize_PRG_relojEqPESOS
	//EEPROMsize_PRG_relojCALEND

#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define size_prog1 MAX(EEPROMsize_PRG_relojCOMUN, EEPROMsize_PRG_relojTARIFA)
#define size_prog2 MAX(EEPROMsize_PRG_relojEqPESOS, EEPROMsize_PRG_relojCALEND)
#define EEPROMsize_PRG_MAX   MAX(size_prog1, size_prog2)

  /* PUERTOS */
  /***********/

#define JUMPER_PROG_PIN						GPIO_PIN_11
#define JUMPER_PROG_PORT					GPIOC
#define JUMPER_PROG_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOC_CLK_ENABLE()
#define JUMPER_PROG_GPIO_CLK_DISABLE()       __HAL_RCC_GPIOC_CLK_DISABLE()
#define JUMPER_PROG_EXTI_LINE                 GPIO_PIN_11
#define JUMPER_PROG_EXTI_IRQn                 EXTI4_15_IRQn


#define jumperON		1
#define prog_modeON		1

  /* ESTRUCTURAS */
  /***************/
    // ESTRUCTURA DE GRABACION DE PARAMETROS 1 - Estructura DUAL, para levantar la 
    // bandera temporal (parametro recibido correctamente) que una vez finalizado
    // el proceso de grabación, comanda que se levanta la bandera de grabacion de
    // EEPROM
    typedef union{
      byte Byte;
      struct{
        byte wr_comunes       :1;       // Indica que se deben grabar los parametros comunes en EEPROM
        byte wr_EqPesos       :1;       // Indica que se debe grabar la equivalencia en pesos en EEPROM
        byte wr_cal           :1;       // Indica que se debe grabar el calendario en EEPROM
        byte wr_mov           :1;       // Indica que se debe grabar los parametros de movil en EEPROM
        byte wr_rad           :1;       // Indica que se debe grabar los parametros de radio en EEPROM
        byte wr_tkt           :1;       // Indica que se debe grabar los parametros de ticket en EEPROM
        byte wr_tkt_rec       :1;       // Indica que se debe grabar los parametros de ticket recaudacion en EEPROM
        byte wr_kate          :1;       // Indica que se debe grabar los parametros de kate en eeprom
      }Bits;
    }PRG_FLAGS1;


    // ESTRUCTURA DE GRABACION DE PARAMETROS 2 - Estructura DUAL, para levantar la 
    // bandera temporal (parametro recibido correctamente) que una vez finalizado
    // el proceso de grabación, comanda que se levanta la bandera de grabacion de
    // EEPROM
    typedef union{
      byte Byte;
      struct{
        byte wr_T1D           :1;       // Indica que se debe grabar tarifa 1 diurna en EEPROM
        byte wr_T2D           :1;       // Indica que se debe grabar tarifa 2 diurna en EEPROM
        byte wr_T3D           :1;       // Indica que se debe grabar tarifa 3 diurna en EEPROM
        byte wr_T4D           :1;       // Indica que se debe grabar tarifa 4 diurna en EEPROM
        byte wr_T1N           :1;       // Indica que se debe grabar tarifa 1 nocturna en EEPROM
        byte wr_T2N           :1;       // Indica que se debe grabar tarifa 2 nocturna en EEPROM
        byte wr_T3N           :1;       // Indica que se debe grabar tarifa 3 nocturna en EEPROM
        byte wr_T4N           :1;       // Indica que se debe grabar tarifa 4 nocturna en EEPROM
      }Bits;
    }PRG_FLAGS2;
    
    // ESTRUCTURA DE GRABACION DE PARAMETROS 3 - Estructura DUAL, para levantar la 
    // bandera temporal (parametro recibido correctamente) que una vez finalizado
    // el proceso de grabación, comanda que se levanta la bandera de grabacion de
    // EEPROM
    typedef union{
      byte Byte;
      struct{
    	byte resetAFTER_txRTA           :1;       // Indica que se debe grabar tarifa 1 diurna en EEPROM    byte wr_MSJ2          :1;       // Indica que se debe grabar MSJ PREGRABADO 2 en EEPROM
        byte  jumper					:1;		  // se presiono pulsador de programacion
        byte  prog						:1;		  // se presiono pulsador de programacion
        byte  error_eeprom				:1;		  // error al levantar datos de eeprom
      }Bits;
    }PRG_FLAGS3;
    
    
    // ESTRUCTURA DE GRABACION DE PARAMETROS 4 - Estructura DUAL, para levantar la 
    // bandera temporal (parametro recibido correctamente) que una vez finalizado
    // el proceso de grabación, comanda que se levanta la bandera de grabacion de
    // EEPROM
    typedef union{
      byte Byte;
      struct{
        byte wr_GF1_2         :1;       // Indica que se debe grabar GEO FENCE 1-2 en EEPROM
        byte wr_GF3_4         :1;       // Indica que se debe grabar GEO FENCE 3-4 en EEPROM
        byte wr_GF5_6         :1;       // Indica que se debe grabar GEO FENCE 5-6 en EEPROM
        byte wr_GF7_8         :1;       // Indica que se debe grabar GEO FENCE 7-8 en EEPROM
        byte wr_GF9_10        :1;       // Indica que se debe grabar GEO FENCE 9-10 en EEPROM
        byte wr_GF11_12       :1;       // Indica que se debe grabar GEO FENCE 11-12 en EEPROM
        byte wr_GF13_14       :1;       // Indica que se debe grabar GEO FENCE 13-14 en EEPROM
        byte wr_GF15_16       :1;       // Indica que se debe grabar GEO FENCE 15-16 en EEPROM
      }Bits;
    }PRG_FLAGS4;
    
    
    // BANDERAS DE PROGRAMACION
    typedef union{
      byte Byte;
      struct{
        byte RQST             :1;       // Indica que hubo pedido de grabacion de parametros de EEPROM (PROGRAMACION)
        byte FIN              :1;       // Indica fin de Programación
        byte RST              :1;       // Indica que se debe resetear por PROGRAMACION (al finalizar tonos buzzer)
        byte prgSKYPATROL     :1;       // Indica a protocolo Bax que tiene que armar comandos de programacion
      }Bits;
    }PRG_FLAGS;
    

    // ERRORES DE PROGRAMACION
    typedef enum{
      PRG_OK,                           // Sin Error
      PRG_ERROR_TO,                     // Error de Time Out
      PRG_ERROR_N,		  				// Error en el N
      PRG_NULL_POINTER,
      PRG_NO_JUMPER                     // Sin Jumper de programacion
    }tPRG_ERROR;
  

    typedef struct{
		uint32_t 	dirRljCmn;
		uint32_t	dirEqPesos;
		uint32_t	dirCalend;
		uint32_t	dirMovil;
     }tDirProg;


    typedef union{
         byte Byte;
         struct{
			byte 		addressProg1           :1;       //
			byte 		addressProg2           :1;       //
			byte 		addressProg3           :1;       //
			byte 		addressProg4           :1;       //
         }Bits;
		struct{
			byte addressProg          :4;        // Tarifas Diurnas Habilitadas
		}MergedBits;


       }checkEEPROM_PROG;

/*----------------------------------------------------------------------------------*/
/* BANDERAS DE PARAMETROS CORRECTOS */
/*----------------------------------*/    
    // BANDERAS DE PARAMETRO CORRECTO 1 - Si recibi el parametro correctamente
    // y lo duplique en RAM sin errores, levanto esta bandera para luego
    // comandar la grabación de EEPROM cuando finaliza el proceso de grabación
    extern PRG_FLAGS1                       _prgOK_F1;
      #define prgOK_F1                      _prgOK_F1.Byte
      #define prgRELOJ_COMUNES_OK_F 		    _prgOK_F1.Bits.wr_comunes
      #define prgRELOJ_EQPESOS_OK_F 		    _prgOK_F1.Bits.wr_EqPesos
      #define prgRELOJ_CALEND_OK_F 		      _prgOK_F1.Bits.wr_cal
      #define prgMOVIL_OK_F 		            _prgOK_F1.Bits.wr_mov
      #define prgRADIO_OK_F 		            _prgOK_F1.Bits.wr_rad
      #define prgTICKET_OK_F 		            _prgOK_F1.Bits.wr_tkt
      #define prgTICKET_RECAUD_OK_F         _prgOK_F1.Bits.wr_tkt_rec
      #define prgRELOJ_KATE_OK_F            _prgOK_F1.Bits.wr_kate

     
    // BANDERAS DE PARAMETRO CORRECTO 2 - Si recibi el parametro correctamente
    // y lo duplique en RAM sin errores, levanto esta bandera para luego
    // comandar la grabación de EEPROM cuando finaliza el proceso de grabación
    extern PRG_FLAGS2                       _prgOK_F2;
      #define prgOK_F2                      _prgOK_F2.Byte
      #define prgRELOJ_T1D_OK_F			        _prgOK_F2.Bits.wr_T1D
      #define prgRELOJ_T2D_OK_F			        _prgOK_F2.Bits.wr_T2D
      #define prgRELOJ_T3D_OK_F			        _prgOK_F2.Bits.wr_T3D
      #define prgRELOJ_T4D_OK_F			        _prgOK_F2.Bits.wr_T4D
      #define prgRELOJ_T1N_OK_F			        _prgOK_F2.Bits.wr_T1N
      #define prgRELOJ_T2N_OK_F			        _prgOK_F2.Bits.wr_T2N
      #define prgRELOJ_T3N_OK_F			        _prgOK_F2.Bits.wr_T3N
      #define prgRELOJ_T4N_OK_F			        _prgOK_F2.Bits.wr_T4N


    // BANDERAS DE PARAMETRO CORRECTO 3 - Si recibi el parametro correctamente
    // y lo duplique en RAM sin errores, levanto esta bandera para luego
    // comandar la grabación de EEPROM cuando finaliza el proceso de grabación
    extern PRG_FLAGS3                    _prgOK_F3;
      #define prgOK_F3                   _prgOK_F3.Byte
      #define resetAFTER_txRTA_F         _prgOK_F3.Bits.resetAFTER_txRTA
	  #define jumper_pressed             _prgOK_F3.Bits.jumper
	  #define prog_mode		             _prgOK_F3.Bits.prog
	  #define error_eepromDATA           _prgOK_F3.Bits.error_eeprom

    void finPRG_TO_RESET (void);
    
    
    // BANDERAS DE PARAMETRO CORRECTO 4 - Si recibi el parametro correctamente
    // y lo duplique en RAM sin errores, levanto esta bandera para luego
    // comandar la grabación de EEPROM cuando finaliza el proceso de grabación
    extern PRG_FLAGS4                       _prgOK_F4;
      #define prgOK_F4                      _prgOK_F4.Byte
      #define prgGEO_FENCE_1_2_OK_F         _prgOK_F4.Bits.wr_GF1_2
      #define prgGEO_FENCE_3_4_OK_F         _prgOK_F4.Bits.wr_GF3_4
      #define prgGEO_FENCE_5_6_OK_F         _prgOK_F4.Bits.wr_GF5_6
      #define prgGEO_FENCE_7_8_OK_F         _prgOK_F4.Bits.wr_GF7_8
      #define prgGEO_FENCE_9_10_OK_F        _prgOK_F4.Bits.wr_GF9_10
      #define prgGEO_FENCE_11_12_OK_F       _prgOK_F4.Bits.wr_GF11_12
      #define prgGEO_FENCE_13_14_OK_F       _prgOK_F4.Bits.wr_GF13_14
      #define prgGEO_FENCE_15_16_OK_F       _prgOK_F4.Bits.wr_GF15_16
    
    extern checkEEPROM_PROG		CheckProg_status;
      #define prgOK_ADDRESS		CheckProg_status.Byte
      #define blckPROG1_OK		CheckProg_status.Bits.addressProg1
	  #define blckPROG2_OK	 	CheckProg_status.Bits.addressProg2
	  #define blckPROG3_OK	 	CheckProg_status.Bits.addressProg3
	  #define blckPROG4_OK	 	CheckProg_status.Bits.addressProg4
	  #define blckPROG_OK	 	CheckProg_status.MergedBits.addressProg

    extern tDirProg		dirProg;
	 #define	addressReloj	dirProg.dirRljCmn
	 #define	addressEqPesos	dirProg.dirEqPesos
	 #define	addressCalend	dirProg.dirCalend
	 #define	addressMovil	dirProg.dirMovil

/*----------------------------------------------------------------------------------*/
/* BANDERAS DE GRANACION DE PARAMETROS */
/*-------------------------------------*/    
    // BANDERAS DE GRABACION DE PARAMETROS EN EEPROM 1
    extern PRG_FLAGS1                       _prgEEPROM_F1;
      #define prgEEPROM_F1                  _prgEEPROM_F1.Byte
      #define prgRELOJ_COMUNES_EEPROM_F 		_prgEEPROM_F1.Bits.wr_comunes
      #define prgRELOJ_EQPESOS_EEPROM_F 		_prgEEPROM_F1.Bits.wr_EqPesos
      #define prgRELOJ_CALEND_EEPROM_F 		  _prgEEPROM_F1.Bits.wr_cal
      #define prgMOVIL_EEPROM_F 		        _prgEEPROM_F1.Bits.wr_mov
      #define prgRADIO_EEPROM_F 		        _prgEEPROM_F1.Bits.wr_rad
      #define prgTICKET_EEPROM_F 		        _prgEEPROM_F1.Bits.wr_tkt
      #define prgTICKET_RECAUD_EEPROM_F     _prgEEPROM_F1.Bits.wr_tkt_rec
      
     
     // BANDERAS DE GRABACION DE PARAMETROS EN EEPROM 2
     extern PRG_FLAGS2                      _prgEEPROM_F2;
      #define prgEEPROM_F2                  _prgEEPROM_F2.Byte
      #define prgRELOJ_T1D_EEPROM_F			    _prgEEPROM_F2.Bits.wr_T1D
      #define prgRELOJ_T2D_EEPROM_F			    _prgEEPROM_F2.Bits.wr_T2D
      #define prgRELOJ_T3D_EEPROM_F			    _prgEEPROM_F2.Bits.wr_T3D
      #define prgRELOJ_T4D_EEPROM_F			    _prgEEPROM_F2.Bits.wr_T4D
      #define prgRELOJ_T1N_EEPROM_F			    _prgEEPROM_F2.Bits.wr_T1N
      #define prgRELOJ_T2N_EEPROM_F			    _prgEEPROM_F2.Bits.wr_T2N
      #define prgRELOJ_T3N_EEPROM_F			    _prgEEPROM_F2.Bits.wr_T3N
      #define prgRELOJ_T4N_EEPROM_F			    _prgEEPROM_F2.Bits.wr_T4N


    // BANDERAS DE GRABACION DE PARAMETROS EN EEPROM 3
     extern PRG_FLAGS3                      _prgEEPROM_F3;
      #define prgEEPROM_F3                  _prgEEPROM_F3.Byte
      #define prgMSJ_PREGRAB_1_EEPROM_F	    _prgEEPROM_F3.Bits.wr_MSJ1
      #define prgMSJ_PREGRAB_2_EEPROM_F	    _prgEEPROM_F3.Bits.wr_MSJ2
      #define prgMSJ_PREGRAB_3_EEPROM_F	    _prgEEPROM_F3.Bits.wr_MSJ3
      #define prgMSJ_PREGRAB_4_EEPROM_F	    _prgEEPROM_F3.Bits.wr_MSJ4
      #define prgMSJ_PREGRAB_5_EEPROM_F	    _prgEEPROM_F3.Bits.wr_MSJ5
      #define prgMSJ_PREGRAB_6_EEPROM_F	    _prgEEPROM_F3.Bits.wr_MSJ6
      #define prgMSJ_PREGRAB_7_EEPROM_F	    _prgEEPROM_F3.Bits.wr_MSJ7
      #define prgMSJ_PREGRAB_8_EEPROM_F	    _prgEEPROM_F3.Bits.wr_MSJ8

    
    // BANDERAS DE GRABACION DE PARAMETROS EN EEPROM 4 - Solo uso 1bit
    extern PRG_FLAGS4                       _prgEEPROM_F4;
      #define prgEEPROM_F4                  _prgEEPROM_F4.Byte
      #define prgGEO_FENCE_1_2_EEPROM_F     _prgEEPROM_F4.Bits.wr_GF1_2
      #define prgGEO_FENCE_3_4_EEPROM_F     _prgEEPROM_F4.Bits.wr_GF3_4
      #define prgGEO_FENCE_5_6_EEPROM_F     _prgEEPROM_F4.Bits.wr_GF5_6
      #define prgGEO_FENCE_7_8_EEPROM_F     _prgEEPROM_F4.Bits.wr_GF7_8
      #define prgGEO_FENCE_9_10_EEPROM_F    _prgEEPROM_F4.Bits.wr_GF9_10
      #define prgGEO_FENCE_11_12_EEPROM_F   _prgEEPROM_F4.Bits.wr_GF11_12
      #define prgGEO_FENCE_13_14_EEPROM_F   _prgEEPROM_F4.Bits.wr_GF13_14
      #define prgGEO_FENCE_15_16_EEPROM_F   _prgEEPROM_F4.Bits.wr_GF15_16


/*----------------------------------------------------------------------------------*/

    
    // BANDERAS DE PROGRAMACION
    extern PRG_FLAGS                        _prog_F;
      #define prog_F                        _prog_F.Byte  
      #define prgREQUEST_F                  _prog_F.Bits.RQST
      #define prgFIN_F 		                  _prog_F.Bits.FIN
      #define prgRESET_F 		                _prog_F.Bits.RST
      #define PROG_SKYPATROL                _prog_F.Bits.prgSKYPATROL

    
    // COMANDOS y subCOMANDOS
      #define CMD_PROGRAMAR           0x7F
      #define CMD_LECTURA             0x7E
      #define CMD_INTERNO             0x6F
      #define N_lectura               0
      
      // RELOJ  
        #define subCMD_RELOJkate      0x84
        #define N_RELOJkate           6 
        #define subCMD_RELOJcomun     0x85
        #define N_RELOJcomun          8 // Esto son solo los PARAMETROS no contempla el DATE (6 bytes mas)
        #define subCMD_RELOJtarifa1D  0x86
        #define subCMD_RELOJtarifa2D  0x87
        #define subCMD_RELOJtarifa3D  0x88
        #define subCMD_RELOJtarifa4D  0x89
        #define subCMD_RELOJtarifa1N  0x8C
        #define subCMD_RELOJtarifa2N  0x8D
        #define subCMD_RELOJtarifa3N  0x8E
        #define subCMD_RELOJtarifa4N  0x8F
        #define N_RELOJtarifa         17
        #define subCMD_RELOJcalendar  0x90
        #define N_RELOJcalendar       40
        #define subCMD_RELOJeqPesos   0x99
        #define N_RELOJeqPesos        13
      
      // MOVIL + RADIO
        #define subCMD_MOVIL          0x80
        #define N_MOVIL               5
        #define subCMD_RADIO          0x81
        #define N_RADIO               11
      
      // FIN PROGRAMACION
        #define subCMD_FIN_PROG       0x8A

      // TICKET
        #define subCMD_TICKETtext     0x8B
        #define subCMD_TICKETsel      0x9A
        #define N_TICKETsel           1  
        #define subCMD_TICKETracaud   0x9B
        #define N_TICKETrecaud        1  
      
#define    FIN_PRG_TO					30

  /* VARIABLES */
  /*************/
  
 extern byte longTarifa;
 extern uint8_t finPRG_TO_RESET_TO_cnt;
  
  /* RUTINAS */
  /***********/
    extern void PROGRAMADOR_ini (void);
    extern void PROGRAMADOR_chkRx (void);
    extern void PROGRAMADOR_startTx(byte CMD, byte subCMD, byte* DATA_buffer);
    extern void PROGRAMADOR_startTxRTA(byte CMD);

    extern void iniDataProg(void);
    extern void checkDataProg(void);
    extern byte checkSectorProg(uint32_t blockProg);
    extern uint32_t getDirProgOk(void);
    extern void loadDirProg(uint32_t dir);
    extern void restoreEepromProg(void);
    extern void restoreSectoProg(uint32_t dir_Ok, uint16_t* dir_Wrong);
    
    extern tPRG_ERROR PROGRAMADOR_chkN (byte N_Rx, byte N_expect);
    
    extern byte* PROGRAMADOR_addRxData (tBAX_SCI* BAX_SCI_ptr);
    extern byte* PROGRAMADOR_addByte (byte dato);
    extern byte PROGRAMADOR_getProgData (byte** ptr_ptr);
    
    extern tPRG_ERROR PROGRAMADOR_saveSTRING (byte* RAMbuffer, tBAX_SCI* BAX_SCI_ptr);

    extern tEEPROM_ERROR PROGRAMADOR_grabarEEPROM (void);
    
    extern void PROGRAMADOR_fin (void);
    extern void PROGRAMADOR_reset (void);
    
    extern byte PROGRAMADOR_chkJumper (void);
    extern void PROGRAMADOR_TxNoJumper (void);
    
    extern byte PROGRAMADOR_chkReset (void);
    extern void  JUMPER_PROG_Init(void);

    extern void check_jumperPROG(void);
    extern void chkProgMode(void);
    void chkEepromDATA(void);

  /* TIMER */
  /*********/
    extern void PROGRAMADOR_reset_timer (void);
        

  /* PARAMETROS */
  /**************/
    
  
#endif

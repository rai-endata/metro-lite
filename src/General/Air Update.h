/* File: <Air Update.h> */



  /* INCLUDES */
  /************/
 // #include "- Visor Config -.h"
 // #include "Air Communication.h"
    
#include "Definicion de tipos.h"
#include "DA Communication.h"

/*********************************************************************************************/
/* ESTRUCTURAS */
/***************/


typedef union{
    byte Byte;
    struct{
      byte update_rqst          :1;     // Indica REQUEST de UPDATE
      byte reset_rqst           :1;     // Indica Request de RESET (tras programacion de parametros por aire)
      byte finRta_LOCAL         :1;     // Indica que finalizo la Rta al Comando de Update
      byte finRta_TRANSPARENTE  :1;     // Indica que finalizo la Rta al Comando de Update
      byte ok_airUpdate_txRqst  :1;     // Indica que se graba correctamente y indica informar a la central
      byte txUpdateSuccess		:1;		// espera a que se transmita la respuesta para luego enviar el comando
    }Bits;
  }tAirUpdate_FLAGS;


 // ACCIONES
  typedef enum{
    ACC_updateZona,
    ACC_updateBase,
    ACC_updateProgRadio,
    ACC_updateProgReloj_comun,
    ACC_updateProgReloj_tarifa,
    ACC_updateProgReloj_eqpesos,
    ACC_updateProgReloj_calendario
  }tACCIONES;
  

  //SUBCOMANDO PROGRAMACION POR AIRE TARIFA KATE
  #define AIR_UPDATE_subCMDtarifa            0x0C

  //SUBCOMANDO PROGRAMACION POR AIRE TARIFA KATE
  #define AIR_UPDATE_subCMDrelojCM           0x03

	#define cantReint           10

  /* VARIABLES */
  /*************/
    extern typeTxCMD CMD_AIRUPDATE_SUCCESS;
    extern typeTxCMD CMD_AirRead;
  

    extern tAirUpdate_FLAGS             _AIR_UPDATE_F;
    extern tACCIONES                    AIR_UPDATE_action;
    extern byte AIR_UPDATE_ID[2];

      #define AIR_UPDATE_F              _AIR_UPDATE_F.Byte
      #define updateRequest             _AIR_UPDATE_F.Bits.update_rqst
      #define updateFinRTA_LOCAL        _AIR_UPDATE_F.Bits.finRta_LOCAL
	  #define updateFinRTA_TRANSPARENTE _AIR_UPDATE_F.Bits.finRta_TRANSPARENTE
      #define resetRequest              _AIR_UPDATE_F.Bits.reset_rqst
      #define OK_airUpdate_txRequest    _AIR_UPDATE_F.Bits.ok_airUpdate_txRqst
	  #define waitToTx_upDateSuccess    _AIR_UPDATE_F.Bits.txUpdateSuccess

  
  /* RUTINAS */
  /***********/
    extern void AIR_UPDATE_Rx (byte* Rx_data_ptr);
    extern void AIR_UPDATE_RxTransparente (byte* Rx_data_ptr);

    extern void AIR_READ_Rx (byte* Rx_data_ptr);

    extern void AIR_UPDATE_update (void);
    extern void AIR_UPDATE_success_Tx_LOCAL (void);
    extern void AIR_UPDATE_success_Tx_TRANSPARENTE(void);

    extern byte AIR_UPDATE_chkCRC (byte* data, byte N);
    


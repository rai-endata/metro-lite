#ifndef _DA_RX_
  #define _DA_RX_
#include "Definicion de tipos.h"
#include "stdint.h"

/* RUTINAS */
/***********/
#define IMPRESION_TICKET_VIAJE 		1
#define IMPRESION_TICKET_TURNO 		2

extern	byte CMD_a_RESP;                            // Comando a Responder
		#define TX_F_CMD_A_RESP                 (Tabla_ComandosTx[OFFSET_RTA]->Tx_F)
		#define N_CMD_A_RESP                    (Tabla_ComandosTx[OFFSET_RTA]->N)
		#define BUFFER_CMD_A_RESP               (Tabla_ComandosTx[OFFSET_RTA]->buffer)

//#define 	CMD_ACTIONS		((Rx_cmd == cmdInicio_TURNO) || \
//							 (Rx_cmd == cmdFin_TURNO))

#define 	CMD_ACTIONS		((Rx_cmd == cmdInicio_TURNO)   		||	\
							 (Rx_cmd == cmdFin_TURNO)			||	\
							 (Rx_cmd == cmdPase_LIBRE)			||  \
							 (Rx_cmd == cmdPase_OCUPADO)		||  \
							 (Rx_cmd == cmdPase_Cobrando)		||  \
							 (Rx_cmd == cmdInicio_EPERA)		||  \
							 (Rx_cmd == cmdFin_ESPERA)			||  \
							 (Rx_cmd == cmdImpresion)			||  \
							 (Rx_cmd == cmdPase_fueraServicio))




extern byte Rx_DA_chkTxRta_F (void);
	#define Tx_RTA_F          (Rx_DA_chkTxRta_F())

extern byte Rx_DA_getCMDRta (void);
	#define getCMD_A_RESP     (Rx_DA_getCMDRta())

extern byte* Rx_DA_getBufferRta (void);
	#define getBUFFER_A_RESP  (Rx_DA_getBufferRta())

extern byte tarifa;
extern byte RxDA_buffer[255];
extern byte CMD_a_RESP;;

extern void Rx_DA_clrTxRta_F (void);
	#define clrTx_RTA_F       (Rx_DA_clrTxRta_F())

	extern void DA_putByte_RxBuffer (byte dato);
	extern void DA_addRxPUT_toRxGPA (void);
	extern void DA_incRxcnt (void);
	extern void DA_saveRxPUTptr_BKP (void);
	extern uint16_t chkSpace_onDA_RxBuffer (void);
	extern void DA_iniRx (void);
	extern void RxDA_process (void);

	extern void Inicio_TURNO_Rx (byte* Rx_data_ptr);
	extern void Fin_TURNO_Rx (byte* Rx_data_ptr);
	extern void Pase_LIBRE_Rx (byte* Rx_data_ptr);
	extern void Pase_OCUPADO_Rx (byte* Rx_data_ptr);
	extern void Pase_COBRANDO_Rx (byte* Rx_data_ptr);
	extern void ConsultaTarifas (byte* Rx_data_ptr);
	extern void Inicio_EPERA_Rx (byte* Rx_data_ptr);
	extern void Fin_EPERA_Rx (byte* Rx_data_ptr);
	extern void Impresion_Rx (byte* Rx_data_ptr);
	extern void ConsultaEstado (byte* Rx_data_ptr);
	extern void Comando_TRANSPARENTE_Rx (byte* Rx_data_ptr);


#endif

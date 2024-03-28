/* File: <Air Communication.h> */

#ifndef _DA_COMMUNICATION_
  #define _DA_COMMUNICATION_
  
  /* INCLUDE */
  /***********/
   
   #include "Lista Comandos.h"
   #include "DA Tx.h"
   #include "DA Rx.h"
 
/* VARIABLES & MACROS */
  /**********************/
    // Reintentos
	#define reintINFINITOS      0xFF
	#define reint_3      		0x03
	#define reint_0      		0x00



  	#define OFFSET_RTA          0x80    // Offset en Tabla_ComandosTx
  // Longitudes de comandos estándar
    //#define N_CMD               (3)           // Longitud de COMANDO solo
    #define N_CMD               (1)           // Longitud de COMANDO solo (LE SAQUE NRO MOVIL)
    #define N_CMD_DAT           (N_CMD+1)     // Longitud de COMANDO + DATO
    #define N_POS               (7)			//Longitud de Posición
    #define N_CMD_POS           (N_CMD+N_POS) // Longitud de COMANDO + POSICION



   // Fin de datos de Air Buffer
  #define fin_datos_msb     0xDF
  #define fin_datos_lsb     0x0A


  #define MENSAJE1		0x01		//Vehiculo en movimiento
  #define MENSAJE2		0x02		//Espere unos segundos para el pase a LIBRE
  #define MENSAJE3		0x03		//Ya puede pasar a LIBRE
  #define MENSAJE4		0x04		//tarifa invalida
  #define MENSAJE5		0x05		//tarifa no programada
  #define MENSAJE6		0x06		//comando transparente
  #define MENSAJE7		0x07		//Recaudacion Parcial
  #define MENSAJE8		0x08		//Aun no tiene viajes en el turno empezado
  #define MENSAJE9		0x09		//SESION CERRADA EXITOSAMENTE
  #define MENSAJE10		0x0A		//cambio a cobrando no permitido
  #define MENSAJE11		0x0B		//Pase a Ocupado con Tarifa automatica
  #define MENSAJE12		0x0C		//Se esta quedando sin espacio para guardar reportes, CIERRE EL TURNO
  #define MENSAJE13		0x0D		//Se ha cerrado el turno, AUTOMATICAMENTE, por falta de espacio para reportes
  #define MENSAJE14		0x0E		//Sincronizando el Reloj
  #define MENSAJE15		0x0F		//Tipo de equipo o reloj desconocido, verifique programacion de reloj
  #define MENSAJE16		0x10		//reloj desconocido, verifique programacion de reloj
  #define MENSAJE17		0x11		//Verifique la programacion del reloj y del movil
  #define MENSAJE18		0x12		//Verifique la programacion del reloj
  #define MENSAJE19		0x13		//Verifique la programacion del movil
  #define MENSAJE20		0x14		//El equipo esta en modo PROGRAMACION
  /* ESTRUCTURAS */
  /***************/  


	typedef struct{
		byte Tx_F;
		byte cmd;
		byte Reintentos;
		byte timeReint;
		byte N;
		byte* buffer;
	}typeTxCMD;




    extern typeTxCMD CMD_RTA;
    extern typeTxCMD CMD_RTA;

    extern typeTxCMD* const Tabla_ComandosTx[];
    extern typeTxCMD CMD_NULL;
    
    
    
#endif

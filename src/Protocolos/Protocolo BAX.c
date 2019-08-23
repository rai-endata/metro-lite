/* File: <Protocolo BAX.c> */

/* INCLUDES */
/************/
  //#include "- Reloj-Blue Config -.h"
  #include "- metroBLUE Config -.h"
  #include "Definiciones Generales.h"
  #include "stddef.h"
  #include "BAX Config.h"
  #include "Manejo de Buffers.h"
 // #include "General\HAL.h"
  
//  #include "- Odometro Selection -.h"
  
  #include "Protocolo BAX.h"
  
//  #include "Temporizaciones\Timer General.h"
  #include "Timer.h"
 // #include "calculos.h"

/**************************************************************************************************/
/* PROTOTIPOS */
/**************/
  // RUTINAS INTERNAS
  static tBAX BAX_stageSTART (byte data, tBAX_SCI* BAX_SCI_ptr);
  static tBAX BAX_stageN (byte data, tBAX_SCI* BAX_SCI_ptr);
  static tBAX BAX_stageCHKSUM (byte data, tBAX_SCI* BAX_SCI_ptr);
  static tBAX BAX_stageDATA (byte data, tBAX_SCI* BAX_SCI_ptr);
  static tBAX BAX_stageSTOP (byte data, tBAX_SCI* BAX_SCI_ptr);
  
 // static byte* BAX_goToNextPackage (byte* ptr, byte* buffer_ini, uint16_t dim);
    
#ifdef  VISOR_USES_SKYPATROL

  static byte check_eco(void);

#endif
   
/**************************************************************************************************/
/* CONSTANTES */
/**************/
  static const BAXptr_type   BAX_tablaETAPAS_Rx[]={
    BAX_stageSTART,
    BAX_stageN,
    BAX_stageCHKSUM,
    BAX_stageDATA,
    BAX_stageSTOP
  };


/**************************************************************************************************/
/* VARIABLES */
/*************/
  #ifdef VISOR_USES_SATELITECH
    #define mseg_TO_BAX_sciRx   200       // Milisegundos de TimeOut de Rx SCI de BAX
  #elif defined(VISOR_USES_SKYPATROL)
    #define mseg_TO_BAX_sciRx   200       // Milisegundos de TimeOut de Rx SCI de BAX
  #else
    #define mseg_TO_BAX_sciRx   10        // Milisegundos de TimeOut de Rx SCI de BAX
  #endif
  
  static byte BAX_chksum_Rx;            // ChekSum recibido por la SCI
  static byte BAX_chksum_calc;          // ChekSum calculado
  
  static byte BAX_N_Rx;                 // N recibido por la SCI
  static byte BAX_N_calc;               // N calculado
  
  static byte ENDATA_RxOutOfSpace_cnt;  // Contador de buffer de Rx sin lugar
  static byte ENDATA_TxOutOfSpace_cnt;  // Contador de buffer de Tx sin lugar
    
 #ifdef  VISOR_USES_SKYPATROL
 //  static byte asciiCONT;             //cuenta si es el primer o segundo byte en ascii(0: primer byte, 1:segundo byte)
 //  static byte asciiTAB[2];           //guarda los dos byte en ascii, que representan un Hex
   byte buff_ECHO_CONTROL[2];
 #endif

/**************************************************************************************************/
/* RUTINAS */
/***********/

  /* RECEPCION DE DATOS CON PROTOCOLO BAX */
  /****************************************/
    void BAX_Rx_data (byte data, tBAX* stage_ptr, tBAX_SCI* BAX_SCI_ptr){
      // Realizo la accion dependiendo del STAGE en el que me encuentre. Cada etapa,
      // ademas de procesar el dato, actualiza el valor de la etapa
      // 
      // Los datos que voy recibiendo los guardo de la siguiente manera:
      //
      //                    | N | ...DATOS... |
      //
      // No guardo ninguno de los caracteres de control como ser el START, el CHKSUM
      // y el STOP.
      *stage_ptr = BAX_tablaETAPAS_Rx[*stage_ptr](data, BAX_SCI_ptr);
    }
	


  /* TRANSMISION DE DATOS CON PROTOCOLO BAX */
  /******************************************/
   
 #ifndef  VISOR_USES_SKYPATROL

  /* protocolo bax: 01 N CHK datos 03 */
  /************************************/

    byte BAX_Tx_data (byte N, byte* DATA_buffer, tBAX_SCI* BAX_SCI_ptr){
      // Rutina que inicia la transmisión de datos por SCI con protocolo BAX.
      // Los datos a transmitir deben terminar con DF0A.
      // Se pasa el buffer de datos a transmitir y el tipo BAX_SCI asociado
      //
      // EDIT 30/11/11
      //  Se agrego control de N junto con DF0A. Ahora el N es un argumento
      //
      // EDIT 05/09/12
      //  Ya no se trabaja mas en forma LINEAL, sino que se hace en forma CIRCULAR
      // => No inicializo puntero al ingresar en la rutina, y en lugar de eso, guardo
      // un BKP del puntero PUT para reestablecerlo en caso de error.
      // Ademas, si se logra encolar con exito, incrementa el contador de transmisiones
      // pendientes.
      // 
      // IMPORTANTE: DATA_buffer SI debe ser lineal
      //
      // EDIT 28/09/12
      //  Antes de agregar nuevos datos a ser transmitidos, debo verificar que en el buffer
      // haya lugar para los mismos y no pisar transmisiones previas. Esto es, que el PUT no
      // pise al GET
      byte TO_F, chk, exit, error;
      byte* CHK_ptr;
      byte* PUTptr_bkp;
      volatile uint16_t availableSpace;

      PUTptr_bkp = BAX_SCI_ptr->TxPUT;              // Hago backup de datos, por si hay error
      
      availableSpace = chkSpace_onBAX_TxBuffer(BAX_SCI_ptr);
      
      // Al hacer la comparacion de si hay lugar o no, al N a encolar le tengo q sumar 2 btyes
      // mas, ya que incluye a los 2 bytes de fin de SCI
      if (availableSpace >= (N + 2)){
        // Hay lugar => puedo transmitir los nuevos datos
        error = 0;
        BAX_SCI_ptr->TxFull = 0;                  //agregado por rai 06/08/2014
      }else{
        // No hay lugar => Descarto la transmision
        error = 1;
        BAX_SCI_ptr->TxFull = 1;                    // Buffer de Tx lleno
        ENDATA_TxOutOfSpace_cnt++;
      }
      
      if (!error){
        // Solo agrego datos si hay lugar (no error) ya si no hay lugar puede que no
        // entre ni un byte mas, y de ser ese el caso, pierdo lo que estaba transmitiendo
        put_byte(&BAX_SCI_ptr->TxPUT, BAX_SCI_ptr->start, BAX_SCI_ptr->TxBuffer, dim_BAX); // Agrego START
        put_byte(&BAX_SCI_ptr->TxPUT, N+1, BAX_SCI_ptr->TxBuffer, dim_BAX); // Agrego N y le sumo 1 x el chksum
        
        CHK_ptr = BAX_SCI_ptr->TxPUT;               // Dejo lugar el CheckSum
        //BAX_SCI_ptr->TxPUT++;                       // Avanzo Puntero
        inc_ptr(&BAX_SCI_ptr->TxPUT, BAX_SCI_ptr->TxBuffer, dim_BAX);
        
        chk = 0;                                    // Reseteo CheckSum
        TO_F = 0;                                   // Limpio Bandera
        exit = 0;                                   // Reseteo variable
        //dispararTimeOut_lazo();                          // Disparo TimeOut Lazo
        dispararTO_lazo();
        while (!exit && !TO_F){
          //TO_F = chkTimeOut_lazo_F();                    // Chequeo bandera de time out de lazo
          TO_F = chkTO_lazo_F();                    // Chequeo bandera de time out de lazo
          
          chk ^= *DATA_buffer;                      // Voy calculando CheckSum
          
          put_byte(&BAX_SCI_ptr->TxPUT, *DATA_buffer++, BAX_SCI_ptr->TxBuffer, dim_BAX);  // Agrego datos
          if (N>0){
            N--;                                    // Voy decrementando N
          }else{
            exit = 1;                               // Salgo por error
            error = 1;
          }
          
          if ((*DATA_buffer == finSCI_H) && (*(DATA_buffer+1) == finSCI_L)){
            // Encontre un DF0A => Si N=0 termine de extraer los datos. Sino
            // continuo justamente xq el DF0A son datos y no fin
            if (N == 0){
              exit = 1;                             // Salgo por fin
            }
          }
        }
        //detenerTimeOut_lazo();                           // Detener TimeOut Lazo
        detenerTO_lazo();                           // Detener TimeOut Lazo
      }
      
      if (!error){
        put_byte(&BAX_SCI_ptr->TxPUT, BAX_stop, BAX_SCI_ptr->TxBuffer, dim_BAX);  // Agrego STOP
        put_byte(&BAX_SCI_ptr->TxPUT, finSCI_H, BAX_SCI_ptr->TxBuffer, dim_BAX);  // Agrego fin H
        put_byte(&BAX_SCI_ptr->TxPUT, finSCI_L, BAX_SCI_ptr->TxBuffer, dim_BAX);  // Agrego fin L
        
        BAX_SCI_ptr->Tx_cnt++;                      // Nueva transmision pendiente
        
        *CHK_ptr = chk;                             // Agrego CheckSum en espacio reservado
      
      }else{
        // Reestablezco el puntero PUT, y hago como que no paso nada
        BAX_SCI_ptr->TxPUT = PUTptr_bkp;            // Reestablezo puntero
      }
      
      return(error);
    }
 #else

  /* Protocolo: comandos AT skypatrol */
  /************************************/

    byte BAX_Tx_data (uint16_t N, byte* DATA_buffer, tBAX_SCI* BAX_SCI_ptr){
      // Rutina que FORMATEA LOS datos A TRANSMITIR por SCI con protocolo BAX.
      // Los datos a transmitir deben terminar con DF0A.
      // Se pasa el buffer de datos a transmitir y el tipo BAX_SCI asociado
      //
      // EDIT 30/11/11
      //  Se agrego control de N junto con DF0A. Ahora el N es un argumento
      //
      // EDIT 05/09/12
      //  Ya no se trabaja mas en forma LINEAL, sino que se hace en forma CIRCULAR
      // => No inicializo puntero al ingresar en la rutina, y en lugar de eso, guardo
      // un BKP del puntero PUT para reestablecerlo en caso de error.
      // Ademas, si se logra encolar con exito, incrementa el contador de transmisiones
      // pendientes.
      // 
      // IMPORTANTE: DATA_buffer SI debe ser lineal
      //
      // EDIT 28/09/12
      //  Antes de agregar nuevos datos a ser transmitidos, debo verificar que en el buffer
      // haya lugar para los mismos y no pisar transmisiones previas. Esto es, que el PUT no
      // pise al GET
      byte TO_F, exit, error;
      byte* PUTptr_bkp;
      byte* N_ptr;
      volatile uint16_t availableSpace;

      N_ptr = (byte*)&N;
      
      PUTptr_bkp = BAX_SCI_ptr->TxPUT;              // Hago backup de datos, por si hay error
      
      availableSpace = chkSpace_onBAX_TxBuffer(BAX_SCI_ptr);
      
      // Al hacer la comparacion de si hay lugar o no, al N a encolar le tengo q sumar 2 btyes
      // mas, ya que incluye a los 2 bytes de fin de SCI
      if (availableSpace >= (N + 2)){
        // Hay lugar => puedo transmitir los nuevos datos
        error = 0;
        BAX_SCI_ptr->TxFull = 0;                  //agregado por rai 06/08/2014
      }else{
        // No hay lugar => Descarto la transmision
        error = 1;
        BAX_SCI_ptr->TxFull = 1;                    // Buffer de Tx lleno
        ENDATA_TxOutOfSpace_cnt++;
      }
      
      if (!error){
        //agrego N parte alta
        put_byte(&BAX_SCI_ptr->TxPUT, *N_ptr++, BAX_SCI_ptr->TxBuffer, dim_BAX); 
        //agrego N parte baja
        put_byte(&BAX_SCI_ptr->TxPUT, *N_ptr, BAX_SCI_ptr->TxBuffer, dim_BAX); 
      
        // Solo agrego datos si hay lugar (no error) ya si no hay lugar puede que no
        // entre ni un byte mas, y de ser ese el caso, pierdo lo que estaba transmitiendo
        TO_F = 0;                                   // Limpio Bandera
        exit = 0;                                   // Reseteo variable
        dispararTO_lazo();                          // Disparo TimeOut Lazo
        while (!exit && !TO_F){
          TO_F = chkTO_lazo_F();                    // Chequeo bandera de time out de lazo
          
          put_byte(&BAX_SCI_ptr->TxPUT, *DATA_buffer++, BAX_SCI_ptr->TxBuffer, dim_BAX);  // Agrego datos
          if (N>0){
            N--;                                    // Voy decrementando N
          }else{
            exit = 1;                               // Salgo por error
            error = 1;
          }
          
          if ((*DATA_buffer == finSCI_H) && (*(DATA_buffer+1) == finSCI_L)){
            // Encontre un DF0A => Si N=0 termine de extraer los datos. Sino
            // continuo justamente xq el DF0A son datos y no fin
            if (N == 0){
              exit = 1;                             // Salgo por fin
            }
          }
        }
        detenerTO_lazo();                           // Detener TimeOut Lazo
      }
      
      if (!error){
        //put_byte(&BAX_SCI_ptr->TxPUT, finSCI_H, BAX_SCI_ptr->TxBuffer, dim_BAX);  // Agrego fin H
        //put_byte(&BAX_SCI_ptr->TxPUT, finSCI_L, BAX_SCI_ptr->TxBuffer, dim_BAX);  // Agrego fin L
        
        BAX_SCI_ptr->Tx_cnt++;                      // Nueva transmision pendiente
        
      }else{
        // Reestablezco el puntero PUT, y hago como que no paso nada
        BAX_SCI_ptr->TxPUT = PUTptr_bkp;            // Reestablezo puntero
      }
      
      return(error);
    }

 
 
 #endif

  /* DETERMINAR ESPACIO DISPONIBLE EN BUFFER DE Rx */
  /*************************************************/
    
    
    uint16_t chkSpace_onBAX_RxBuffer (tBAX_SCI* BAX_SCI_ptr){
      uint16_t space;
      uint16_t queue_cnt;
      byte* GET;
      byte* PUT;
      byte idxPUT,idxGET;  

      GET = BAX_SCI_ptr->RxGPA[BAX_SCI_ptr->RxGPA_GETidx];
      PUT = BAX_SCI_ptr->RxPUT;
      queue_cnt = BAX_SCI_ptr->Rx_cnt;    // Extriago cantidad de encolados
      
      
      //agregado por rai
      idxGET = BAX_SCI_ptr->RxGPA_GETidx; //
      idxPUT = BAX_SCI_ptr->RxGPA_PUTidx;
      

      
      if (((PUT == GET) && (queue_cnt != 0)) || (idxPUT==idxGET)){
        // Si los punteros GET y PUT son iguales y HAY paquetes encolados
        // => No hay mas lugar
        space = 0;

      }else if (GET > PUT){
        /*
                 +---------+
                 |         |
                 |         |
          PUT -> |         |
                 |*********|
                 |*********|
                 |*********|
                 |         | -> GET
                 |         |
                 +---------+
                 
                 
            => SPACE = GET - PUT
        */
        space = GET - PUT;
      
      }else{
        /*
                 +---------+
                 |*********|
                 |*********|
                 |         | -> GET
                 |         |
                 |         |
                 |         |
          PUT -> |         |
                 |*********|
                 +---------+
                 
                 
            => SPACE = DIM - (PUT - GET) = DIM - PUT + GET
        */
        space = dim_BAX;
        space -= (PUT - GET);
      }
      
      return(space);
    }


  /* DETERMINAR ESPACIO DISPONIBLE EN BUFFER DE Tx */
  /*************************************************/
    uint16_t chkSpace_onBAX_TxBuffer (tBAX_SCI* BAX_SCI_ptr){
      uint16_t space;
      uint16_t queue_cnt;
      byte* GET;
      byte* PUT;
      
      GET = BAX_SCI_ptr->TxGET;
      PUT = BAX_SCI_ptr->TxPUT;
      queue_cnt = BAX_SCI_ptr->Tx_cnt;    // Extriago cantidad de encolados
      if ((PUT == GET) && (queue_cnt != 0)){
        // Si los punteros GET y PUT son iguales y HAY paquetes encolados
        // => No hay mas lugar
        space = 0;
      
      }else if (GET > PUT){
        /*
                 +---------+
                 |         |
                 |         |
          PUT -> |         |
                 |*********|
                 |*********|
                 |*********|
                 |         | -> GET
                 |         |
                 +---------+
                 
                 
            => SPACE = GET - PUT
        */
        space = GET - PUT;
      
      }else{
        /*
                 +---------+
                 |*********|
                 |*********|
                 |         | -> GET
                 |         |
                 |         |
                 |         |
          PUT -> |         |
                 |*********|
                 +---------+
                 
                 
            => SPACE = DIM - (PUT - GET) = DIM - PUT + GET
        */
        space = dim_BAX;
        space -= (PUT - GET);
      }
      
      return(space);
    }


  /* AGREGAR FORMATO BAX A CADENA */
  /********************************/
    byte BAX_formatting (byte N, byte* orig, byte* dest){
      // NO ES PARA USO CON ESTRUCTURA BAX, es SOLO PARA AGREGAR
			// Rutina que agrega el formato BAX a los datos en ORIG, y los devuelve
			// formateados en DEST.
			// Devuelve indicando ERROR (1) o NO (0)
      byte TO_F, chk, exit, error;
      byte* CHK_ptr;

			error = 0;

			*dest++ = BAX_start;												// Agrego START
			*dest++ = N+1;															// Agrego N
			//N--;

			CHK_ptr = dest;               							// Dejo lugar el CheckSum
			dest++;                       							// Avanzo Puntero

			chk = 0;                                    // Reseteo CheckSum
			TO_F = 0;                                   // Limpio Bandera
			exit = 0;                                   // Reseteo variable
			dispararTO_lazo();                          // Disparo TimeOut Lazo
			while (!exit){
				TO_F = chkTO_lazo_F();                    // Chequeo bandera de time out de lazo

				chk ^= *orig;                      				// Voy calculando CheckSum
				
				*dest++ = *orig++;												// Agrego datos
				if (N>0){
					N--;                                 	// Voy decrementando N
				}else{
					exit = 1;                               // Salgo por error
					error = 1;
				}

				if ((*orig == finSCI_H) && (*(orig+1) == finSCI_L)){
					// Encontre un DF0A => Si N=0 termine de extraer los datos. Sino
					// continuo justamente xq el DF0A son datos y no fin
					if (N == 0){
						exit = 1;                             // Salgo por fin
						error = 0;
					}
				}
				
				if (TO_F){
					error = 1;
					exit = 1;                               // Salgo por error
				}
			}
			detenerTO_lazo();                           // Detener TimeOut Lazo

      if (!error){
        *CHK_ptr = chk;                           // Agrego CheckSum en espacio reservado
				*dest++ = BAX_stop;												// Agrego STOP
				*dest++ = 0x0D;
				*dest++ = 0x0A;

      }

      return(error);
    }    
    
	
/**************************************************************************************************/
/* RUTINA INTERNAS */
/*******************/



#ifdef VISOR_USES_SKYPATROL
  /* ETAPA START DEL PROTOCOLO BAX */
  /*********************************/

    static tBAX BAX_stageSTART (byte data, tBAX_SCI* BAX_SCI_ptr){
      // Etapa inicial del protocolo BAX. El dato recibido debe ser el inicio
      // de cadena; y de ser asi, puedo pasar la siguiente etapa sino me quedo
      // en la etapa inicial
      tBAX next_stage;
        
        BAX_SCI_ptr->Rx_TO_cnt = 0;                 // Detengo Time Out
        
        if (data == BAX_SCI_ptr->start){
          // CADENA BAX
          next_stage = BAX_N;                       // Paso a estado de N
          BAX_SCI_ptr->RxPUT_bkp = BAX_SCI_ptr->RxPUT;  // Guardo BackUp de puntero PUT de Rx
          BAX_SCI_ptr->Rx_TO_cnt = mseg_TO_BAX_sciRx; // Disparo Time Out
          
        }else{
          // NINGUNA CADENA
          next_stage = BAX_START;                 // Me quedo en estado START
        }

      return (next_stage);
    }


  /* ETAPA N DEL PROTOCOLO BAX */
  /*****************************/
    static tBAX BAX_stageN (byte data, tBAX_SCI* BAX_SCI_ptr){
      // EDIT 07/09/12
      //  Se agrego el control de espacio disponible en buffer.
      // En caso de no haberlo, no permite el ingreso de los datos
      byte* PUTptr;
      uint16_t availableSpace;
      tBAX next_stage;


        BAX_SCI_ptr->Rx_TO_cnt = 0;                 // Detengo Time Out
        
        BAX_N_Rx = data;                            // Guardo N para controlarlo con longitud de datos
        BAX_N_calc = 0;                             // Reseteo Calculo de N
        
        availableSpace = chkSpace_onBAX_RxBuffer(BAX_SCI_ptr);
        
        // Al hacer la comparacion de si hay lugar o no, al N a encolar le tengo q sumar 2 btyes
        // mas, ya que incluye a los 2 bytes de fin de SCI
        if (availableSpace >= (BAX_N_Rx + 2)){
          // Hay lugar => puedo pasar a siguiente estado
          next_stage = BAX_CHKSUM;
          BAX_SCI_ptr->RxFull = 0;                  //agregado por rai 06/08/2014 
        }else{
          // No hay lugar, reseteo estados
          next_stage = BAX_START;
          BAX_SCI_ptr->RxFull = 1;                  // Buffer de Rx lleno
          ENDATA_RxOutOfSpace_cnt++;
        }
        if(!(BAX_SCI_ptr->RxFull)){
          PUTptr = BAX_SCI_ptr->RxPUT;                // Extraigo puntero put, para poder tomar su direccion
          put_byte(&PUTptr, data, BAX_SCI_ptr->RxBuffer, (uint16_t) dim_BAX); // Guardo N en el buffer
          BAX_SCI_ptr->RxPUT = PUTptr;                // Actualizo puntero PUT
          BAX_SCI_ptr->Rx_TO_cnt = mseg_TO_BAX_sciRx; // Disparo Time Out
        }
      
      return (next_stage);                             // Paso a estado de CHEKSUM o START si hay error
    }
  
  
  /* ETAPA CHKSUM DEL PROTOCOLO BAX */
  /**********************************/
    static tBAX BAX_stageCHKSUM (byte data, tBAX_SCI* BAX_SCI_ptr){
      byte next_stage;

        BAX_SCI_ptr->Rx_TO_cnt = 0;                 // Detengo Time Out
      
        BAX_chksum_Rx = data;                       // Guardo CheckSum para controlarlo con el de los datos
        BAX_chksum_calc = BAX_chksum_Rx;            // Reseteo Calculo de CheckSum
        
        BAX_N_calc++;                               // Incremento N

        BAX_SCI_ptr->Rx_TO_cnt = mseg_TO_BAX_sciRx; // Disparo Time Out
        next_stage = BAX_DATA; 
      return (next_stage);                            // Paso a estado de DATOS
    }

    
  /* ETAPA DATA DEL PROTOCOLO BAX */
  /********************************/
    static tBAX BAX_stageDATA (byte data, tBAX_SCI* BAX_SCI_ptr){
      tBAX next_stage;
      byte* PUTptr;

        BAX_SCI_ptr->Rx_TO_cnt = 0;                 // Detengo Time Out
        
        PUTptr = BAX_SCI_ptr->RxPUT;                // Extraigo puntero put, para poder tomar su direccion
        put_byte(&PUTptr, data, BAX_SCI_ptr->RxBuffer, (uint16_t) dim_BAX); // Guardo dato en el buffer
        BAX_SCI_ptr->RxPUT = PUTptr;                // Actualizo puntero PUT
        
        BAX_N_calc++;                               // Incremento N
        BAX_chksum_calc ^= data;                    // Voy calculando el CheckSum
        
        if (BAX_N_calc == BAX_N_Rx){
         //Si fue un eco lo descarto
          if( !check_eco()){
           next_stage = BAX_STOP;                         // Paso a estado de STOP
           BAX_SCI_ptr->Rx_TO_cnt = mseg_TO_BAX_sciRx;  // Disparo Time Out
          }else{
           BAX_SCI_ptr->RxPUT = BAX_SCI_ptr->RxPUT_bkp;   // Reestablezco BackUp de puntero PUT de Rx
           next_stage = BAX_START;  
          }
        }else{
          next_stage = BAX_DATA;                        // Me quedo en estado de DATA
          BAX_SCI_ptr->Rx_TO_cnt = mseg_TO_BAX_sciRx;  // Disparo Time Out
        }
        
        
      return (next_stage);
    }

    
  /* ETAPA STOP DEL PROTOCOLO BAX */
  /********************************/
    static tBAX BAX_stageSTOP (byte data, tBAX_SCI* BAX_SCI_ptr){

      byte next_stage;
      
        BAX_SCI_ptr->Rx_TO_cnt = 0;                 // Detengo Time Out

        if (data == BAX_stop){
          // Recepcion de cadena BAX Exitosa:
          // - Incremento contador de recepciones exitosas
          // - Guardo backup del puntero porque la recepcion fue exitosa
          
          if (BAX_chksum_calc == 0){

            BAX_SCI_ptr->Rx_cnt++;                        // Incremento contador de Recepciones

            BAX_SCI_ptr->RxPUT_bkp = BAX_SCI_ptr->RxPUT;  // Guardo BackUp de puntero PUT de Rx

            BAX_SCI_ptr->RxGPA[BAX_SCI_ptr->RxGPA_PUTidx] = BAX_SCI_ptr->RxPUT; // Guardo nuevo GPA
            BAX_SCI_ptr->RxGPA_PUTidx++;                  // Incremento indice PUT de GPA

            if (BAX_SCI_ptr->RxGPA_PUTidx == dim_BAX_GPA){
              BAX_SCI_ptr->RxGPA_PUTidx = 0;              // Doy la vuelta al indice PUT
            }
            
            BAX_SCI_ptr->Rx_fin = 1;                      // Levanto bandera de fin de Recepcion EXITOSO
          
          }else{
            // Error al recibir cadena BAX: No coincide Checkum
            // - Reestablezco backup del puntero PUT de Rx
            BAX_SCI_ptr->RxPUT = BAX_SCI_ptr->RxPUT_bkp;  // Reestablezco BackUp de puntero PUT de Rx
          }
        }else{
          // Error al recibir cadena BAX: No coincide N con STOP
          // - Reestablezco backup del puntero PUT de Rx
          BAX_SCI_ptr->RxPUT = BAX_SCI_ptr->RxPUT_bkp;    // Reestablezco BackUp de puntero PUT de Rx
        }
      next_stage = BAX_START;  
      return (next_stage);                               // Pase lo que pase, vuelvo al START
    }    

   /*VERIFICA SI DATOS RECIBIDOS ES UN ECO DE LO TRANSMITIDO*/
   /*********************************************************/
   /*
    Esta verificacion se hace de un modo simple: Se Verifica que el N y CHksum ultimo transmitido sea igual al recibido
    si son iguales es un eco. 
   */
   

   static byte check_eco(void){
        
        byte eco;
        
        eco = 0;
        
        if((BAX_N_Rx == buff_ECHO_CONTROL[0]) && (BAX_chksum_Rx == buff_ECHO_CONTROL[1])){
         eco=1;
        }
         return(eco);
     
    }
    
  
  
  byte pick_BAX_N_Rx (void){
    
    return(BAX_N_Rx);
  }          
   
#else

  /* ETAPA START DEL PROTOCOLO BAX */
  /*********************************/
    static tBAX BAX_stageSTART (byte data, tBAX_SCI* BAX_SCI_ptr){
      // Etapa inicial del protocolo BAX. El dato recibido debe ser el inicio
      // de cadena; y de ser asi, puedo pasar la siguiente etapa sino me quedo
      // en la etapa inicial
      tBAX next_stage;

      BAX_SCI_ptr->Rx_TO_cnt = 0;                 // Detengo Time Out
      
      if (data == BAX_SCI_ptr->start){
        // CADENA BAX
        next_stage = BAX_N;                       // Paso a estado de N
        BAX_SCI_ptr->RxPUT_bkp = BAX_SCI_ptr->RxPUT;  // Guardo BackUp de puntero PUT de Rx
        BAX_SCI_ptr->Rx_TO_cnt = mseg_TO_BAX_sciRx; // Disparo Time Out
        
      }else{
        // NINGUNA CADENA
        next_stage = BAX_START;                   // Me quedo en estado START
      }
      
      return (next_stage);
    }


  /* ETAPA N DEL PROTOCOLO BAX */
  /*****************************/
    static tBAX BAX_stageN (byte data, tBAX_SCI* BAX_SCI_ptr){
      // EDIT 07/09/12
      //  Se agrego el control de espacio disponible en buffer.
      // En caso de no haberlo, no permite el ingreso de los datos
      byte* PUTptr;
      uint16_t availableSpace;
      tBAX next_stage;

      BAX_SCI_ptr->Rx_TO_cnt = 0;                 // Detengo Time Out
      
      BAX_N_Rx = data;                            // Guardo N para controlarlo con longitud de datos
      BAX_N_calc = 0;                             // Reseteo Calculo de N
      
      availableSpace = chkSpace_onBAX_RxBuffer(BAX_SCI_ptr);
      
      // Al hacer la comparacion de si hay lugar o no, al N a encolar le tengo q sumar 2 btyes
      // mas, ya que incluye a los 2 bytes de fin de SCI
      if (availableSpace >= (BAX_N_Rx + 2)){
        // Hay lugar => puedo pasar a siguiente estado
        next_stage = BAX_CHKSUM;
        BAX_SCI_ptr->RxFull = 0;                  //agregado por rai 06/08/2014 
      }else{
        // No hay lugar, reseteo estados
        next_stage = BAX_START;
        BAX_SCI_ptr->RxFull = 1;                  // Buffer de Rx lleno
        ENDATA_RxOutOfSpace_cnt++;
      }
      if(!(BAX_SCI_ptr->RxFull)){
        PUTptr = BAX_SCI_ptr->RxPUT;                // Extraigo puntero put, para poder tomar su direccion
        put_byte(&PUTptr, data, BAX_SCI_ptr->RxBuffer, (uint16_t) dim_BAX); // Guardo N en el buffer
        BAX_SCI_ptr->RxPUT = PUTptr;                // Actualizo puntero PUT
        BAX_SCI_ptr->Rx_TO_cnt = mseg_TO_BAX_sciRx; // Disparo Time Out
      }
      return (next_stage);                        // Paso a estado de CHEKSUM o START si hay error
    }
  
  
  /* ETAPA CHKSUM DEL PROTOCOLO BAX */
  /**********************************/
    static tBAX BAX_stageCHKSUM (byte data, tBAX_SCI* BAX_SCI_ptr){

      BAX_SCI_ptr->Rx_TO_cnt = 0;                 // Detengo Time Out
      
      BAX_chksum_Rx = data;                       // Guardo CheckSum para controlarlo con el de los datos
      BAX_chksum_calc = BAX_chksum_Rx;            // Reseteo Calculo de CheckSum
      
      BAX_N_calc++;                               // Incremento N

      BAX_SCI_ptr->Rx_TO_cnt = mseg_TO_BAX_sciRx; // Disparo Time Out
      
      return (BAX_DATA);                          // Paso a estado de DATOS
    }

    
  /* ETAPA DATA DEL PROTOCOLO BAX */
  /********************************/
    static tBAX BAX_stageDATA (byte data, tBAX_SCI* BAX_SCI_ptr){
      tBAX next_stage;
      byte* PUTptr;

      BAX_SCI_ptr->Rx_TO_cnt = 0;                 // Detengo Time Out
      
      PUTptr = BAX_SCI_ptr->RxPUT;                // Extraigo puntero put, para poder tomar su direccion
      put_byte(&PUTptr, data, BAX_SCI_ptr->RxBuffer, (uint16_t) dim_BAX); // Guardo dato en el buffer
      BAX_SCI_ptr->RxPUT = PUTptr;                // Actualizo puntero PUT
      
      BAX_N_calc++;                               // Incremento N
      BAX_chksum_calc ^= data;                    // Voy calculando el CheckSum
      
      if (BAX_N_calc == BAX_N_Rx){
        next_stage = BAX_STOP;                    // Paso a estado de STOP
      }else{
        next_stage = BAX_DATA;                    // Me quedo en estado de DATA
      }
      
      BAX_SCI_ptr->Rx_TO_cnt = mseg_TO_BAX_sciRx;  // Disparo Time Out
      
      return (next_stage);
    }

    
  /* ETAPA STOP DEL PROTOCOLO BAX */
  /********************************/
    static tBAX BAX_stageSTOP (byte data, tBAX_SCI* BAX_SCI_ptr){
      BAX_SCI_ptr->Rx_TO_cnt = 0;                 // Detengo Time Out

      if (data == BAX_stop){
        // Recepcion de cadena BAX Exitosa:
        // - Incremento contador de recepciones exitosas
        // - Guardo backup del puntero porque la recepcion fue exitosa
        if (BAX_chksum_calc == 0){
          BAX_SCI_ptr->Rx_cnt++;                        // Incremento contador de Recepciones
          BAX_SCI_ptr->RxPUT_bkp = BAX_SCI_ptr->RxPUT;  // Guardo BackUp de puntero PUT de Rx

          BAX_SCI_ptr->RxGPA[BAX_SCI_ptr->RxGPA_PUTidx] = BAX_SCI_ptr->RxPUT; // Guardo nuevo GPA
          
          BAX_SCI_ptr->RxGPA_PUTidx++;                  // Incremento indice PUT de GPA
          if (BAX_SCI_ptr->RxGPA_PUTidx == dim_BAX_GPA){
            BAX_SCI_ptr->RxGPA_PUTidx = 0;              // Doy la vuelta al indice PUT
          }
          
          BAX_SCI_ptr->Rx_fin = 1;                      // Levanto bandera de fin de Recepcion EXITOSO
        
        }else{
          // Error al recibir cadena BAX: No coincide Checkum
          // - Reestablezco backup del puntero PUT de Rx
          BAX_SCI_ptr->RxPUT = BAX_SCI_ptr->RxPUT_bkp;  // Reestablezco BackUp de puntero PUT de Rx
        }
        
      }else{
        // Error al recibir cadena BAX: No coincide N con STOP
        // - Reestablezco backup del puntero PUT de Rx
        BAX_SCI_ptr->RxPUT = BAX_SCI_ptr->RxPUT_bkp;    // Reestablezco BackUp de puntero PUT de Rx
      }
      
      return (BAX_START);                               // Pase lo que pase, vuelvo al START
    }    





#endif


  
  /* INCREMENTAR PUNTERO DE BAX HASTA PROXIMO PAQUETE */
  /****************************************************/
   /*
    static byte* BAX_goToNextPackage (byte* ptr, byte* buffer_ini, uint16_t dim){
      // Esta rutina incrementa un puntero a datos de BAX, hasta el proximo
      // paquete dentro del buffer de RECEPCIONES. En caso de ser el ultimo
      // paquete, el incremento sería hasta el fin del paquete.
      // Siendo un paquete que empieza con START y termina en STOP + FIN_H + FIN_L
      // el proximo-paquete/fin-paquete-actual se considera luego de los 2
      // bytes de fin (FIN_H y FIN_L)
      //
      // El puntero, al ingresar debe estar apuntando a un START, por ende, elç
      // próximo byte define la longitud del paquete.
      // No verifico que efectivamente, apunte a un START al iniciar, por que el
      // valor del byte de START depende de qué sea la que trasnmita
      //
      // Devuelvo un nuevo valor de puntero, con el valor correspondiente al fin
      // del buffer
      byte N;
      byte* end_ptr;
      byte stop, fin1, fin2;
      uint16_t ptr_diff;
      
      (void)get_byte(&ptr, buffer_ini, dim);        // Salto START
      N = get_byte(&ptr, buffer_ini, dim);          // Extraigo N y avanzo puntero
      
      ptr += N;                                     // Avanzo el puntero tantas veces como datos tenga el comando
      end_ptr = buffer_ini + dim;
      if (ptr >= end_ptr){
        // Me pase del fin del buffer => Calculo la diferencia y se la sumo al inicio
        // del mismo
        ptr_diff = ptr - end_ptr;                   // Calculo diferencia
        ptr = buffer_ini + ptr_diff;                // Se la sumo al inicio del buffer
      }
      
      // Al avanzar hasta el último dato, debo encontrarme con un STOP y fin_H y fin_L
      stop = get_byte(&ptr, buffer_ini, dim);       // Extraigo STOP y avanzo puntero
      fin1 = get_byte(&ptr, buffer_ini, dim);       // Extraigo FIN_H y avanzo puntero
      fin2 = get_byte(&ptr, buffer_ini, dim);       // Extraigo FIN_L y avanzo puntero
      
      if ((stop != BAX_stop) || (fin1 != finSCI_H) || (fin2 != finSCI_L)){
        // ERROR => devuelvo NULL
        ptr = NULL;
      }
      
      return(ptr);
    }
*/

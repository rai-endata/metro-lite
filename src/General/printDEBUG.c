/*
 * printDEBUG.c
 *
 *  Created on: 30/11/2018
 *      Author: Rai
 */

#include "printDEBUG.h"

#ifdef RELOJ_DEBUG

#include "Calculos.h"


	#define	printDEBUG_DIM		500

	uint8_t printDEBUG_BUFFER[printDEBUG_DIM];

	buffcirTYPE		printDEBUG;

	void iniPRINT_DEBUG(uint8_t data){
		printDEBUG_BUFFERptr   = printDEBUG_BUFFER;
		printDEBUG_GETptr      = printDEBUG_BUFFER;
		printDEBUG_PUTptr      = printDEBUG_BUFFER;
		printDEBUG_BUFFERsize  = printDEBUG_DIM;
		printDEBUG_BYTEcnt    = 0;
		printDEBUG_DATOScnt   = 0;
	}

	void PRINT_DEBUG_encolarBYTE(uint8_t data){
		putBUFCIR (data, &printDEBUG);
	}

	void PRINT_DEBUG_encolarBUFFER(uint8_t* buffer){
		while(*buffer){
			putBUFCIR (*buffer++, &printDEBUG);
		}
	}

	void PRINT_DEBUG_imprimir (void){
		uint8_t dato;
		uint16_t i;

		uint8_t buffer[500];

		if( printDEBUG_DATOScnt > 0 && NO_ESTA_IMPRIMIENDO && statusPRINT==NO_HAY_IMPRESION_EN_PROCESO){
			printDEBUG_DATOScnt--;
			statusPRINT = IMPRESION_EN_PROCESO;
			i=0;
			dato = getBUFCIR(&printDEBUG);
			while(dato !=0x00){
				buffer[i++] = dato;
				dato = getBUFCIR(&printDEBUG);
			}
			buffer[i] = 0x0d;
			PRINT_send(buffer, i+1);
		}
	}



#ifdef		VISOR_REPORTES
	void printCabecera_REGISTER_TAB_REPORTES(tREG_GENERIC* newReg_GETptr){
	  uint32_t valor;
	  uint8_t bufferPRINT[50];
      uint8_t tipo;

        PRINT_DEBUG_encolarBUFFER("indice = ");
        valor = (uint32_t)newReg_GETptr->idx;
        convert_to_string_with_decimals (bufferPRINT, valor, 0xFF, 0, base_DECIMAL);
        uint8_t i=0;
        while(i<5 && (bufferPRINT[i]!= 0)){
        	PRINT_DEBUG_encolarBYTE(bufferPRINT[i]);
        	i++;
        }
        while(i<5 ){
        	PRINT_DEBUG_encolarBUFFER(" ");
        	i++;
        }
        PRINT_DEBUG_encolarBUFFER(", ");

        //PRINT_DEBUG_encolarBUFFER("TIPO = ");
        //valor = (uint32_t)newReg_GETptr->tipo;
		//convert_to_string_with_decimals (bufferPRINT, valor, 0xFF, 0, base_DECIMAL);

		if(tipo == REG_libre){
			PRINT_DEBUG_encolarBUFFER("LIBRE");
			PRINT_DEBUG_encolarBUFFER(", ");
		}else if(tipo == REG_ocupado){
			PRINT_DEBUG_encolarBUFFER("OCUP.");
			PRINT_DEBUG_encolarBUFFER(", ");
		}else if(tipo == REG_fserv){
			PRINT_DEBUG_encolarBUFFER("F. SERV. ");
			PRINT_DEBUG_encolarBUFFER(", ");
		}else if(tipo == REG_desconexAlim){
			PRINT_DEBUG_encolarBUFFER("DESCONEX. ALIM. ");
			PRINT_DEBUG_encolarBUFFER(", ");
		}else if(tipo == REG_desconexAcc){
			PRINT_DEBUG_encolarBUFFER("DESCONEX. ACC. ");
			PRINT_DEBUG_encolarBUFFER(", ");
		}else if(tipo == REG_varios){
			PRINT_DEBUG_encolarBUFFER("REG. VARIOS ");
			PRINT_DEBUG_encolarBUFFER(", ");
		}else{
			PRINT_DEBUG_encolarBUFFER("TIPO = ");
			printVARIABLE((uint32_t)tipo, &bufferPRINT, 1, 0);
		}


		i=0;
        while(i<3 && (bufferPRINT[i]!= 0)){
        	PRINT_DEBUG_encolarBYTE(bufferPRINT[i]);
        	i++;
        }
        while(i<5 ){
        	PRINT_DEBUG_encolarBUFFER(" ");
        	i++;
        }
        PRINT_DEBUG_encolarBUFFER(", ");

        PRINT_DEBUG_encolarBUFFER("CHOFER = ");
        valor = (uint32_t)newReg_GETptr->chofer;
		convert_to_string_with_decimals (bufferPRINT, valor, 0xFF, 0, base_DECIMAL);
        i=0;
        while(i<10 && (bufferPRINT[i]!= 0)){
        	PRINT_DEBUG_encolarBYTE(bufferPRINT[i]);
        	i++;
        }
		//print km_h
        uint16_t KMH = 100;
        uint16_t* KMH_ptr = &KMH;
		cargaContenidoPuntero_inDATA(KMH_ptr, &bufferPRINT[0]);
        i=0;
        while(i<4 && (bufferPRINT[i]!= 0)){
        	PRINT_DEBUG_encolarBYTE(bufferPRINT[i]);
        	i++;
        }

        PRINT_DEBUG_encolarBYTE(0x00);
        printDEBUG_DATOScnt++;

	}







	void printLiOcFs_REGISTER_TAB_REPORTES(tREG_GENERIC* Reg_GETptr){

		//tREG_GENERIC* Reg_GETptr;

		uint16_t   idx;
		uint8_t    tipo;
		tDATE      date;
		uint8_t    chofer;
		uint16_t   km;
		uint8_t    velMax;
		uint16_t   segParado;
		uint16_t   segMarcha;
		uint8_t    sensor;
		uint8_t    segDESCONEX[3];
		uint8_t    importeDESCONEX;


		uint32_t valor;
		uint8_t bufferPRINT[50];
		uint8_t k;

			idx		  = ((tREG_LIBRE*) Reg_GETptr)->idx;
			tipo	  = ((tREG_LIBRE*) Reg_GETptr)->tipo;
			date	  = ((tREG_LIBRE*) Reg_GETptr)->date;
			chofer	  = ((tREG_LIBRE*) Reg_GETptr)->chofer;
			km		  = ((tREG_LIBRE*) Reg_GETptr)->km;
			velMax	  = ((tREG_LIBRE*) Reg_GETptr)->velMax;
			segParado = ((tREG_LIBRE*) Reg_GETptr)->segParado;
			segMarcha = ((tREG_LIBRE*) Reg_GETptr)->segMarcha;
			sensor	  = ((tREG_LIBRE*) Reg_GETptr)->sensor;
			segDESCONEX[3] = ((tREG_DESCONEXION_ALIM*) Reg_GETptr)->seconds[3];
			importeDESCONEX = ((tREG_DESCONEXION_ALIM*) Reg_GETptr)->importe;

/*
DESCONEXION ALIM
		      uint8_t    seconds[3];     // Segundos desconectado
		      uint8_t    tarifa;         // Tarifa
		      uint8_t    fichasDist[3];  // Cantidad de Fichas de Distancia
		      uint8_t    fichasTime[3];  // Cantidad de Fichas de Tiempo
		      uint32_t   importe;        // Importe (PESOS)
*/
	        PRINT_DEBUG_encolarBUFFER("I = ");
	        printVARIABLE((uint32_t)idx, &bufferPRINT, 5, 0);


	    	if(tipo == REG_libre){
	    			PRINT_DEBUG_encolarBUFFER("LIBRE");
	    			PRINT_DEBUG_encolarBUFFER(", ");
	    		}else if(tipo == REG_ocupado){
	    			PRINT_DEBUG_encolarBUFFER("OCUP.");
	    			PRINT_DEBUG_encolarBUFFER(", ");
	    		}else if(tipo == REG_fserv){
	    			PRINT_DEBUG_encolarBUFFER("F. SERV. ");
	    			PRINT_DEBUG_encolarBUFFER(", ");
	    		}else if(tipo == REG_desconexAlim){
	    			PRINT_DEBUG_encolarBUFFER("DESCONEX. ALIM. ");
	    			PRINT_DEBUG_encolarBUFFER(", ");
	    		}else if(tipo == REG_desconexAcc){
	    			PRINT_DEBUG_encolarBUFFER("DESCONEX. ACC. ");
	    			PRINT_DEBUG_encolarBUFFER(", ");
	    		}else if(tipo == REG_varios){
	    			PRINT_DEBUG_encolarBUFFER("REG. VARIOS ");
	    			PRINT_DEBUG_encolarBUFFER(", ");
	    		}else{
	    			PRINT_DEBUG_encolarBUFFER("TIPO = ");
	    			printVARIABLE((uint32_t)tipo, &bufferPRINT, 1, 0);
	    		}

	        PRINT_DEBUG_encolarBUFFER("CHF = ");
	        printVARIABLE((uint32_t)chofer, &bufferPRINT,2, 0);

	        PRINT_DEBUG_encolarBUFFER("KM = ");
	        printVARIABLE((uint32_t)km, &bufferPRINT, 5, 0);

	        PRINT_DEBUG_encolarBUFFER("VMAX = ");
	        printVARIABLE((uint32_t)velMax, &bufferPRINT, 3, 0);

	        PRINT_DEBUG_encolarBUFFER("segP = ");
	        printVARIABLE((uint32_t)segParado, &bufferPRINT, 5, 0);

	        PRINT_DEBUG_encolarBUFFER("segM = ");
	        printVARIABLE((uint32_t)segMarcha, &bufferPRINT, 5, 0);
            if(tipo == REG_libre){
    	        PRINT_DEBUG_encolarBUFFER("SENS = ");
    	        printVARIABLE((uint32_t)sensor, &bufferPRINT, 5, 1);
            }

	        PRINT_DEBUG_encolarBYTE(0x00);
	        printDEBUG_DATOScnt++;
	}


	    //void printA_PAGAR_REGISTER_TAB_REPORTES(tREG_GENERIC* Reg_GETptr){
		void printA_PAGAR_REGISTER_TAB_REPORTES(tREG_A_PAGAR* regAPagar, uint8_t* ptr_espera){


		//tREG_GENERIC* Reg_GETptr;

	      uint16_t   idx;            // Indice
	      uint8_t    tipo;           // Tipo de registro
	      tDATE      date;           // Date [fecha y hora]
	      uint8_t    chofer;         // Chofer
	      uint16_t   nroViaje;       // Numero Correlativo de Viaje
	      uint8_t    tarifa;         // Tarifa
	      uint8_t    fichasDist[3];  // Cantidad de Fichas de Distancia
	      uint8_t    fichasTime[3];  // Cantidad de Fichas de Tiempo
	      uint32_t   importe;        // Importe (PESOS)
	      uint32_t   espera;         // Espera (SEGUNDOS)
	      byte   espera_aux[4];         // Espera (SEGUNDOS)
	     // uint32_t* ptr_espera;
		uint32_t valor;
		uint8_t bufferPRINT[50];
		uint8_t k;
/*
		idx		  	  = ((tREG_A_PAGAR*) Reg_GETptr)->idx		  	;  // Indice
		tipo		  = ((tREG_A_PAGAR*) Reg_GETptr)->tipo		  	;  // Tipo de registro
		date		  = ((tREG_A_PAGAR*) Reg_GETptr)->date		  	;  // Date [fecha y hora]
		chofer		  = ((tREG_A_PAGAR*) Reg_GETptr)->chofer		;  // Chofer
		nroViaje	  = ((tREG_A_PAGAR*) Reg_GETptr)->nroViaje	  	;  // Numero Correlativo de Viaje
		tarifa		  = ((tREG_A_PAGAR*) Reg_GETptr)->tarifa		;  // Tarifa
		fichasDist[2] = ((tREG_A_PAGAR*) Reg_GETptr)->fichasDist[2] ;  // Cantidad de Fichas de Distancia
		fichasDist[1] = ((tREG_A_PAGAR*) Reg_GETptr)->fichasDist[1] ;  // Cantidad de Fichas de Distancia
		fichasDist[0] = ((tREG_A_PAGAR*) Reg_GETptr)->fichasDist[0] ;  // Cantidad de Fichas de Distancia
		fichasTime[2] = ((tREG_A_PAGAR*) Reg_GETptr)->fichasTime[2] ;  // Cantidad de Fichas de Tiempo
		fichasTime[1] = ((tREG_A_PAGAR*) Reg_GETptr)->fichasTime[1] ;  // Cantidad de Fichas de Tiempo
		fichasTime[0] = ((tREG_A_PAGAR*) Reg_GETptr)->fichasTime[0] ;  // Cantidad de Fichas de Tiempo
		importe		  = ((tREG_A_PAGAR*) Reg_GETptr)->importe		;  // Importe (PESOS)
		espera		  = ((tREG_A_PAGAR*) Reg_GETptr)->espera		;  // Espera (SEGUNDOS)
		//espera_aux    = Reg_GETptr->empty[]
*/

				idx		  	  = regAPagar->idx		  	;  // Indice
				tipo		  = regAPagar->tipo		  	;  // Tipo de registro
				date		  = regAPagar->date		  	;  // Date [fecha y hora]
				chofer		  = regAPagar->chofer		;  // Chofer
				nroViaje	  = regAPagar->nroViaje	  	;  // Numero Correlativo de Viaje
				tarifa		  = regAPagar->tarifa		;  // Tarifa
				fichasDist[2] = regAPagar->fichasDist[2] ;  // Cantidad de Fichas de Distancia
				fichasDist[1] = regAPagar ->fichasDist[1] ;  // Cantidad de Fichas de Distancia
				fichasDist[0] = regAPagar->fichasDist[0] ;  // Cantidad de Fichas de Distancia
				fichasTime[2] = regAPagar->fichasTime[2] ;  // Cantidad de Fichas de Tiempo
				fichasTime[1] = regAPagar->fichasTime[1] ;  // Cantidad de Fichas de Tiempo
				fichasTime[0] = regAPagar->fichasTime[0] ;  // Cantidad de Fichas de Tiempo
				importe		  = regAPagar->importe		;  // Importe (PESOS)
				//espera		  = regAPagar->espera		;  // Espera (SEGUNDOS)
				//ptr_espera    = &regAPagar[14];
				//espera_aux    = *ptr_espera;
				espera_aux[3] = *(ptr_espera+3);
				espera_aux[2] = *(ptr_espera+2);
				espera_aux[1] = *(ptr_espera+1);
				espera_aux[0] = *(ptr_espera+0);


	        PRINT_DEBUG_encolarBUFFER("I = ");
	        printVARIABLE((uint32_t)idx, &bufferPRINT, 5, 0);

	        PRINT_DEBUG_encolarBUFFER("COBR.");
	        PRINT_DEBUG_encolarBUFFER(", ");
	        //PRINT_DEBUG_encolarBUFFER("TIPO = ");
	        //printVARIABLE((uint32_t)tipo, &bufferPRINT, 1, 0);

	        PRINT_DEBUG_encolarBUFFER("CHF = ");
	        printVARIABLE((uint32_t)chofer, &bufferPRINT, 2, 0);

	        PRINT_DEBUG_encolarBUFFER("NV = ");
	        printVARIABLE((uint32_t)nroViaje, &bufferPRINT, 3, 0);
	        PRINT_DEBUG_encolarBUFFER("  ");

	        PRINT_DEBUG_encolarBUFFER("TF = ");
	        printVARIABLE((uint32_t)tarifa, &bufferPRINT, 1, 0);
	        PRINT_DEBUG_encolarBUFFER("  ");

	        PRINT_DEBUG_encolarBUFFER("fichaD = ");
	        printVARIABLE_VECTOR(&fichasDist, &bufferPRINT, 5, 0);


	        PRINT_DEBUG_encolarBUFFER("fichaT = ");
	        printVARIABLE_VECTOR(&fichasTime, &bufferPRINT, 5, 0);

	        PRINT_DEBUG_encolarBUFFER("IMPORTE = ");
	        printVARIABLE((uint32_t)importe, &bufferPRINT, 5, 0);

	        PRINT_DEBUG_encolarBUFFER("ESPERA = ");
	        //printVARIABLE((uint32_t)espera, &bufferPRINT, 5, 1);
	        printVARIABLE_VECTOR(&(espera_aux[1]), &bufferPRINT, 5, 0);

	        PRINT_DEBUG_encolarBYTE(0x00);
	        printDEBUG_DATOScnt++;
	}


	void printSESIONS_REGISTER_TAB_REPORTES(tREG_GENERIC* Reg_GETptr){

			//tREG_GENERIC* Reg_GETptr;


			uint32_t valor;
			uint8_t bufferPRINT[50];
			uint8_t k;

	          uint16_t      idx;            // Indice
	          uint8_t       tipo;           // Tipo de registro
	          tDATE   		date;           // Date [fecha y hora]
	          uint8_t       chofer;         // Chofer
	          uint16_t      nroTurno;       // Numero de Turno

				idx		  	  = ((tREG_SESION*) Reg_GETptr)->idx		  	;  // Indice
				tipo		  = ((tREG_SESION*) Reg_GETptr)->tipo		  	;  // Tipo de registro
				date		  = ((tREG_SESION*) Reg_GETptr)->date		  	;  // Date [fecha y hora]
				chofer		  = ((tREG_SESION*) Reg_GETptr)->chofer		;  // Chofer
				nroTurno	  = ((tREG_SESION*) Reg_GETptr)->nroTurno	  	;  // Numero Correlativo de Viaje

		        PRINT_DEBUG_encolarBUFFER("I = ");
		        printVARIABLE((uint32_t)idx, &bufferPRINT, 5, 0);

		        PRINT_DEBUG_encolarBUFFER("SESION");
		        PRINT_DEBUG_encolarBUFFER("  ");
		        //PRINT_DEBUG_encolarBUFFER("TIPO = ");
		        //printVARIABLE((uint32_t)tipo, &bufferPRINT, 1, 0);

		        PRINT_DEBUG_encolarBUFFER("CHF = ");
		        printVARIABLE((uint32_t)chofer, &bufferPRINT,2, 0);

		        PRINT_DEBUG_encolarBUFFER("nroTRURNO = ");
		        printVARIABLE((uint32_t)nroTurno, &bufferPRINT, 5, 0);


		        PRINT_DEBUG_encolarBYTE(0x00);
		        printDEBUG_DATOScnt++;
		}


void printDIR_REPORTES(byte *buffer){

	uint16_t* dirREPORTE_PUTptr;
	uint16_t valor;


    //PRINT_DEBUG_encolarBUFFER("ADDR_EEPROM_REPORTES = ");
    valor = (uint32_t)REPORTE_PUTptr;
    convert_to_string_with_decimals (buffer, valor, 0xFF, 0, base_DECIMAL);
    uint8_t i=0;
  /*
    //VALOR EN DECIMAL
    while(i<5 && (buffer!= 0)){
    	i++;
    }
    i--;
    //VALOR EN HEX
    *(buffer + i++) ='(';
  */

    *(buffer + i++) =0x0d;
    *(buffer + i++) ='0';
    *(buffer + i++) ='x';
    dirREPORTE_PUTptr = &REPORTE_PUTptr;
    cargaContenidoPuntero_inDATA(dirREPORTE_PUTptr, &*(buffer+i));
/*
    *(buffer + i + 4) =')';
    *(buffer + i + 5) =',';

    *(buffer + i + 6) =' ';
    *(buffer + i + 7) =' ';

    *(buffer + i + 8) =0;
*/
    *(buffer + i + 4) =' ';
    *(buffer + i + 5) =' ';

    *(buffer + i + 6) =0;

    i=0;
    while(i<13 && (*(buffer+i)!= 0)){
    	PRINT_DEBUG_encolarBYTE(*(buffer+i));
    	i++;
    }
    PRINT_DEBUG_encolarBYTE(0x00);
    printDEBUG_DATOScnt++;
}
#endif

void printVARIABLE(uint32_t valor, byte* buffer, byte espacio, byte fin){

	byte k;

    convert_to_string_with_decimals (buffer, valor, 0xFF, 0, base_DECIMAL);
    k=0;
    while(k<5 && (*(buffer+k)!= 0)){
    	PRINT_DEBUG_encolarBYTE(*(buffer+k));
    	k++;
    }
    if(!fin){
        while(k < espacio ){
        	PRINT_DEBUG_encolarBUFFER(" ");
        	k++;
        }
        PRINT_DEBUG_encolarBUFFER(", ");
    }

}

void printVARIABLE_VECTOR(byte* ptr_valor, byte* buffer, byte espacio, byte fin){

	    //N dimension del vector menor o igual a cuatro

		byte k;
		uint32_t valor;

	      valor = *(ptr_valor+0);
	      valor <<= 8;
	      valor |= *(ptr_valor+1);
	      valor <<= 8;
	      valor |= *(ptr_valor+2);

        convert_to_string_with_decimals (buffer, valor, 0xFF, 0, base_DECIMAL);
        k=0;
        while(k<5 && (*(buffer+k)!= 0)){
        	PRINT_DEBUG_encolarBYTE(*(buffer+k));
        	k++;
        }
        if(!fin){
            while(k < espacio ){
            	PRINT_DEBUG_encolarBUFFER(" ");
            	k++;
            }
            PRINT_DEBUG_encolarBUFFER(", ");
        }

	}

#endif

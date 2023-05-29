/*
 * Ticket common.c
 *
 *  Created on: 14/3/2018
 *      Author: Rai
 */
#include "Common Ticket.h"
#include "Reportes.h"

tREG_SESION* iniTURNO_ptr_aux;      // Puntero a inicio de Turno del tipo SESION
tREG_SESION* finTURNO_ptr_aux;      // Puntero a final de Turno del tipo SESION
uint8_t print_buffer[2000];
byte tarifasTurno;

uint8_t to_print_cnt;
tESTADO_IMPRESION statusPRINT;

//Normal Mode (inicializa impresora MPT II)
void IniPRINT(uint8_t** ptrDouble){
	**ptrDouble = 0x1B;
	(*ptrDouble)++;
	**ptrDouble = 0x40;
	(*ptrDouble)++;
}

void habilitar_negrita(uint8_t** ptrDouble){
	**ptrDouble = 0x1B;
	(*ptrDouble)++;
	**ptrDouble = 0x21;
	(*ptrDouble)++;
	**ptrDouble = 0x08;
	(*ptrDouble)++;
}

void printFONT_B(uint8_t** ptrDouble){

	**ptrDouble = 0x1B;
	(*ptrDouble)++;
	**ptrDouble = 0x4D;
	(*ptrDouble)++;
	**ptrDouble = 49;
	(*ptrDouble)++;


}

void printFONT_A(uint8_t** ptrDouble){
	**ptrDouble = 0x1B;
	(*ptrDouble)++;
	**ptrDouble = 0x40;
	(*ptrDouble)++;
	**ptrDouble = 48;
	(*ptrDouble)++;
}

void enableFONT_ITALIC(uint8_t** ptrDouble){
	/*
	**ptrDouble = 0x1B;
	(*ptrDouble)++;
	**ptrDouble = 0x34;
	(*ptrDouble)++;
	**ptrDouble = 1;
	(*ptrDouble)++;
	*/
	**ptrDouble = 0x1B;
	(*ptrDouble)++;
	**ptrDouble = 0x21;
	(*ptrDouble)++;
	**ptrDouble = 0x40;
	(*ptrDouble)++;

}


void enableBOLD(uint8_t** ptrDouble){
	**ptrDouble = 0x1B;
	(*ptrDouble)++;
	**ptrDouble = 0x45;
	(*ptrDouble)++;
	**ptrDouble = 0x01;
	(*ptrDouble)++;
}

void disableBOLD(uint8_t** ptrDouble){
	**ptrDouble = 0x1B;
	(*ptrDouble)++;
	**ptrDouble = 0x45;
	(*ptrDouble)++;
	**ptrDouble = 0x00;
	(*ptrDouble)++;
}


void setBOLD(uint8_t** ptrDouble){
	**ptrDouble = 0x1B;
	(*ptrDouble)++;
	**ptrDouble = 0x21;
	(*ptrDouble)++;
	**ptrDouble = 0x08;
	(*ptrDouble)++;
}

void clearBOLD(uint8_t** ptrDouble){
	**ptrDouble = 0x1B;
	(*ptrDouble)++;
	**ptrDouble = 0x21;
	(*ptrDouble)++;
	**ptrDouble = 0x00;
	(*ptrDouble)++;
}

void setCharCode_spain(uint8_t** ptrDouble){
	**ptrDouble = 0x1B;
	(*ptrDouble)++;
	**ptrDouble = 0x52;
	(*ptrDouble)++;
	**ptrDouble = 12;		//latin america
	(*ptrDouble)++;
}


void disableFONT_ITALIC(uint8_t** ptrDouble){
/*
	**ptrDouble = 0x1B;
	(*ptrDouble)++;
	**ptrDouble = 0x34;
	(*ptrDouble)++;
	**ptrDouble = 0;
	(*ptrDouble)++;
*/
	**ptrDouble = 0x1B;
	(*ptrDouble)++;
	**ptrDouble = 0x21;
	(*ptrDouble)++;
	**ptrDouble = 0x00;
	(*ptrDouble)++;

}
void deshabilitar_negrita(uint8_t** ptrDouble){
	**ptrDouble = 0x1B;
	(*ptrDouble)++;
	**ptrDouble = 0x21;
	(*ptrDouble)++;
	**ptrDouble = 0x00;
	(*ptrDouble)++;
}


void georgianFONT(uint8_t** ptrDouble){
	**ptrDouble = 0x1B;
	(*ptrDouble)++;
	**ptrDouble = 0x74;
	(*ptrDouble)++;
	**ptrDouble = 3;
	(*ptrDouble)++;
}

void Ajuste_Interlinea(uint8_t** ptrDouble, uint8_t n){
	**ptrDouble = 0x1B;
	(*ptrDouble)++;
	**ptrDouble = 0x33;
	(*ptrDouble)++;
	**ptrDouble = n;
	(*ptrDouble)++;
}


void printEMPRESA(uint8_t** ptrDouble){
/*
	string_copy_incDest(ptrDouble,"       SERVICIO DE ");
	if(EEPROM_PROG_TICKET.SERVICIO[2]!=0){
		string_copy_incDest(ptrDouble,&EEPROM_PROG_TICKET.SERVICIO[2]);
	}else{
		string_copy_incDest(ptrDouble,"TAXI");
	}
*/

	//string_copy_incDest(ptrDouble,"TAXIMETRO");
	//add_LF(ptrDouble);
	if(EEPROM_PROG_TICKET.SERVICIO[2]!=0){
	 string_copy_incDest(ptrDouble,&EEPROM_PROG_TICKET.SERVICIO[2]);
	 add_LF(ptrDouble);
	}


	if(EEPROM_PROG_TICKET.LOCALIDAD[2]!=0){
	 string_copy_incDest(ptrDouble,&EEPROM_PROG_TICKET.LOCALIDAD[2]);
	 add_LF(ptrDouble);
	}
	printLINE(ptrDouble);
	//add_LF(ptrDouble);
}

void printCABECERA(uint8_t** ptrDouble, byte plusPRINT){
	uint8_t buffer_aux[20];
    uint16_t aux32;
	if(EEPROM_PROG_TICKET.LEYENDA_1[2]!=0){
		//RT 141
		string_copy_incDest(ptrDouble,&EEPROM_PROG_TICKET.LEYENDA_1[2]);
		add_LF(ptrDouble);
	}
	if(EEPROM_PROG_TICKET.LEYENDA_2[2]!=0){
		//TEL: 4261777
		string_copy_incDest(ptrDouble,&EEPROM_PROG_TICKET.LEYENDA_2[2]);
		add_LF(ptrDouble);
	}
	if(EEPROM_PROG_TICKET.LEYENDA_3[2]!=0){
		//urquiza 2161
		string_copy_incDest(ptrDouble,&EEPROM_PROG_TICKET.LEYENDA_3[2]);
		add_LF(ptrDouble);
	}

	if(EEPROM_PROG_TICKET.TITULAR[2]!=0){
		//Avalos Fernando
		string_copy_incDest(ptrDouble,&EEPROM_PROG_TICKET.TITULAR[2]);
		add_LF(ptrDouble);
	}
	if(EEPROM_PROG_TICKET.CUIT[2]!=0){
		string_copy_incDest(ptrDouble,"CUIT: ");
		//CUIT: 20187609224
		string_copy_incDest(ptrDouble,&EEPROM_PROG_TICKET.CUIT[2]);
		add_LF(ptrDouble);
	}
	if(EEPROM_PROG_TICKET.MARCA_MODELO[2]!=0){
		string_copy_incDest(ptrDouble,"MODELO:: ");
		//PALIO: M: 2009
		string_copy_incDest(ptrDouble,&EEPROM_PROG_TICKET.MARCA_MODELO[2]);
		add_LF(ptrDouble);
	}
	if(EEPROM_PROG_TICKET.PATENTE[2]!=0){
		string_copy_incDest(ptrDouble,"PATENTE: ");
		//Patente:  IIW 421
		string_copy_incDest(ptrDouble,&EEPROM_PROG_TICKET.PATENTE[2]);
		add_LF(ptrDouble);
	}
	if(EEPROM_PROG_TICKET.LICENCIA[2]!=0){
		string_copy_incDest(ptrDouble,"LICENCIA: ");
		//Licencia: 246
		string_copy_incDest(ptrDouble,&EEPROM_PROG_TICKET.LICENCIA[2]);
		add_LF(ptrDouble);
	}
	if(EEPROM_PROG_TICKET.IVA[2]!=0){
		string_copy_incDest(ptrDouble,"IVA: ");
		//IVA
		string_copy_incDest(ptrDouble,&EEPROM_PROG_TICKET.IVA[2]);
		add_LF(ptrDouble);
}

	printLINE(ptrDouble);
	//add_LF(ptrDouble);
	if(plusPRINT){
		//print numero de Movil

		string_copy_incDest(ptrDouble,"Nro Movil:                 ");
		aux32 = (((NRO_MOVIL_H & 0x000000ff)<<8) | (NRO_MOVIL_L & 0x0000ffff));
		convert_to_string(buffer_aux, aux32, 0, base_BCD);
		string_copy_incDest(ptrDouble,&buffer_aux);
		add_LF(ptrDouble);

		string_copy_incDest(ptrDouble,"Chofer:	              1-xxxxxx");
		add_LF(ptrDouble);
		string_copy_incDest(ptrDouble,"Nro Recibo:             000012");
		add_LF(ptrDouble);
		printLINE(ptrDouble);
		add_LF(ptrDouble);
	}
}


void printCABECERA_TURNO(uint8_t** ptrDouble, byte plusPRINT){
	uint8_t buffer_aux[20];
    uint16_t aux32;

	if(EEPROM_PROG_TICKET.TITULAR[2]!=0){
		//Avalos Fernando
		string_copy_incDest(ptrDouble,&EEPROM_PROG_TICKET.TITULAR[2]);
		add_LF(ptrDouble);
	}
	if(EEPROM_PROG_TICKET.CUIT[2]!=0){
		string_copy_incDest(ptrDouble,"CUIT: ");
		//CUIT: 20187609224
		string_copy_incDest(ptrDouble,&EEPROM_PROG_TICKET.CUIT[2]);
		add_LF(ptrDouble);
	}
	if(EEPROM_PROG_TICKET.MARCA_MODELO[2]!=0){
		string_copy_incDest(ptrDouble,"MODELO:: ");
		//PALIO: M: 2009
		string_copy_incDest(ptrDouble,&EEPROM_PROG_TICKET.MARCA_MODELO[2]);
		add_LF(ptrDouble);
	}
	if(EEPROM_PROG_TICKET.PATENTE[2]!=0){
		string_copy_incDest(ptrDouble,"PATENTE: ");
		//Patente:  IIW 421
		string_copy_incDest(ptrDouble,&EEPROM_PROG_TICKET.PATENTE[2]);
		add_LF(ptrDouble);
	}
	if(EEPROM_PROG_TICKET.LICENCIA[2]!=0){
		string_copy_incDest(ptrDouble,"LICENCIA: ");
		//Licencia: 246
		string_copy_incDest(ptrDouble,&EEPROM_PROG_TICKET.LICENCIA[2]);
		add_LF(ptrDouble);
	}

	printLINE(ptrDouble);
	//add_LF(ptrDouble);
	if(plusPRINT){
		//print numero de Movil

		string_copy_incDest(ptrDouble,"Nro Movil:                 ");
		aux32 = (((NRO_MOVIL_H & 0x000000ff)<<8) | (NRO_MOVIL_L & 0x0000ffff));
		convert_to_string(buffer_aux, aux32, 0, base_BCD);
		string_copy_incDest(ptrDouble,&buffer_aux);
		add_LF(ptrDouble);

		string_copy_incDest(ptrDouble,"Chofer:	              1-xxxxxx");
		add_LF(ptrDouble);
		string_copy_incDest(ptrDouble,"Nro Recibo:             000012");
		add_LF(ptrDouble);
		printLINE(ptrDouble);
		add_LF(ptrDouble);
	}
}

void printLINE(uint8_t** ptrDouble){
	string_copy_incDest(ptrDouble,"-------------------------------");
	add_LF(ptrDouble);
}

//agrega line feed (0x0A)
void add_LF(uint8_t** ptrDouble){
	**ptrDouble = 0x0A;
	(*ptrDouble)++;
}



/* CONVERTIR IMPORTE/RECAUDACION A STRING PARA TICKET */
/******************************************************/
  void TICKET_importeToString (dword importe, byte* buffer){
    if (buffer != NULL){
      if (TARIFA_PESOS){
        // PESOS
        *buffer++ = '$';                        // Agrego signo $
        convert_to_string_with_decimals(buffer, importe, 0xFF, PUNTO_DECIMAL, base_DECIMAL);

      }else{
        // FICHAS
        if (EqPESOS_hab != 0){
          // Con Equivalencia en Pesos => La imprimo
          *buffer++ = '$';                        // Agrego signo $
          //convert_to_string_with_decimals(buffer, importe, 0xFF, PUNTO_DECIMAL_EqPesos, base_DECIMAL);
          convert_to_string_with_decimals(buffer, importe, 0xFF, PUNTO_DECIMAL, base_DECIMAL);
        }else{
          // Sin Equivalencia en Pesos => N/A
          string_FAR_copy(buffer, _NA);
        }
      }
    }
  }


  /* PASAR A SIGUIENTE TARIFA */
    /****************************/
      byte TICKET_pasarSiguienteTarifa (byte* tarifaPTR){
        // Rutina que incrementa el contador de tarifas, hasta la proxima tarifa habilitada
        byte doNextTarifa;
        byte tarifaHAB;

        (*tarifaPTR)++;
        if (*tarifaPTR <= cantidadTarifasProgramables){
          if (*tarifaPTR <= prgRELOJ_determineCantTarifasD_MANUAL()){
            doNextTarifa = 1;
          }else{
            while (*tarifaPTR < tarifa1N){
              (*tarifaPTR)++;
            }

            tarifaHAB = prgRELOJ_determineTarifaHab(*tarifaPTR);
            while(((*tarifaPTR - tarifa1N + 1) <= prgRELOJ_determineCantTarifasN_MANUAL()) && !tarifaHAB){
              (*tarifaPTR)++;
              tarifaHAB = prgRELOJ_determineTarifaHab(*tarifaPTR);
            }

            if (tarifaHAB){
              doNextTarifa = 1;
            }else{
              doNextTarifa = 0;
            }
          }

        }else{
          doNextTarifa = 0;
        }

        return(doNextTarifa);
      }


      void to_printing (void){
        // Ejecutada cada 1seg no preciso.
        // Este timer se dispara cuando se pasa a A PAGAR y
        // estando el RELOJ HOMOLOGADO, se deben esperar 15seg
        // para pasar de A PAGAR -> LIBRE
        if (to_print_cnt != 0){
        	to_print_cnt--;
        }
      }

/*
 * display-7seg.c
 *
 *  Created on: 22/3/2018
 *      Author: Rai
 */

#include "stm32f0xx_hal_def.h"

#include "display-7seg.h"
#include "stm32f0xx_hal.h"
#include "calculos.h"
#include "RELOJ.h"
#include "Parametros Reloj.h"
#include "Tarifacion Reloj.h"
#include "teclas.h"

#define SEG_A_MASK		0b01000000
#define SEG_B_MASK		0b00100000
#define SEG_C_MASK		0b00010000
#define SEG_D_MASK		0b00001000
#define SEG_E_MASK		0b00000100
#define SEG_F_MASK		0b00000010
#define SEG_G_MASK		0b00000001

#define ON_DIGITO		1
#define OFF_DIGITO		0

#define TO_DISPLAY_ON	1

#define DISPLAY_OFF			0
#define DISPLAY_ON_IMPORTE	1
#define DISPLAY_ON_TARIFA	2
#define DISPLAY_ON_ALL		3
#define DISPLAY_OFF_TARIFA	4





#define TO_DISPLAY_TOGGLE 5

#define DIGITO_TARIFA   5

static uint8_t toggle_dsply;

typedef struct{
uint8_t	digito_0;		//digito 0
uint8_t	digito_1;		//digito 1
uint8_t	digito_2;		//digito 2
uint8_t	digito_3;		//digito 3
uint8_t	digito_4;		//digito 4
uint8_t	digito_T;		//numero de tarifa
uint8_t	punto;			//punto
}tDISPLAY;

static GPIO_PinState getSEG (uint8_t valor, uint8_t segmento );
static void print_digito (uint32_t valor, uint8_t digito, GPIO_PinState control);
static void ini_catodos_display_7seg (void);
static void ini_segmentos_display_7seg (void);
static void mostrar_prog(void);
static void mostrar_err(void);
static void mostrarTEXTO(byte* ptr_textDISPLAY);

static byte digito_encendido;
static uint8_t to_dsply_on;
static test_display_TO;
#define TEST_DISPLAY_TO	 50


tDISPLAY 				   displayDATA;
t_displayFLAGS             _displayFLAGS;

uint8_t status_display;
uint8_t statusDISPLAY;
static uint8_t auxPUNTO_DECIMAL;

uint8_t toggle_dsplyTARIFA;

uint8_t cnt_1;
uint8_t cnt_2;
uint8_t cnt_3;

#define  	DG0_7SEG	displayDATA.digito_0
#define		DG1_7SEG    displayDATA.digito_1
#define		DG2_7SEG    displayDATA.digito_2
#define		DG3_7SEG    displayDATA.digito_3
#define		DG4_7SEG    displayDATA.digito_4
#define		DGT_7SEG    displayDATA.digito_T
#define		P_7SEG      displayDATA.punto


static const byte tabDIGITOS[] = {
	// abcdefg
	0b01111110,					//0
	0b00110000,					//1
	0b01101101,					//2
	0b01111001,					//3
	0b00110011,					//4
	0b01011011,					//5
	0b01011111,					//6
	0b01110000,					//7
	0b01111111,					//8
	0b01111011					//9
};

/*
          a
     +--------+       			              	              			+--------+
     |		  |        			              	              			|		 |
   f |        | b      			              				          f |        | b
     |   g    |        		g	    			    g          			|   g    |
     +--------+        	+--------+              +--------+              +--------+
     |		          	|             			|		 |                		 |
   e |               e	|              		e	|        | c                     |
     |            	    |              			|        |                       | c
     +			       	+              			+--------+              +--------+
	                                            d                        d
      a
  +--------+
  |
  |
  |   g      	                               		    g
  +--------+                    	+          	    +--------+
  |		    	                	|          		|		 |
  |          	                    | c        	e	|        | c
  |          	                    |          		|        |

                                                        a
                                                    +--------+
                                                    |
                                                  f |
	    g       	    g      		    g           |   g
    +--------+	    +--------+ 	    +--------+      +--------+
	|		    	|		 | 		|		 |      |
e	|         	e	|        | 	e	|        | c  e |
	|          		|        | 		|        |      |
	+--------+  	+--------+
    d               d


	+
	|
 f	|
	|   g
	+--------+
	|
e	|
	|   d
	+--------+
	+
																		|
																	f	|
	    g        				    g        		            		|   g
    +--------+				    +--------+          		+           +--------+
	|		    				|		 |   				|   		|
e	|         				e	|        | c 		        | c 	e	|
	|          					|        |   		        |   		|   d
	+--------+   				+        +   		        +   		+--------+
    d            			    d


*/

static const byte tabDIG_ESPECIAL[] = {
	// abcdefg
	0b00001110,					//L grande
	0b01100111,					//P
	0b00000101,					//r chica
	0b01000010,					//r alta subida
	0b01000110,					//r chica alta
	0b01111110,					//O grande
	0b00011101,					//o chica
	0b01100011,					//o chica subida
	0b01011110,					//G grande
	0b01111011,					//g chica
	0b01001111,					//E grande
	0b00001101,					//c
	0b00010101,					//n
	0b00010000,					//i
	0b01000111,					//F
	0b00001111,					//t
	0b00011100,					//u
	0b00001100					//l chica
//     abcdefg
};


static const byte tabCLEAR[] = {
	// abcdefg
	0b00001101,					//c
	0b00001100,					//l chica
	0b00000101,					//r chica
	0b10000000,					//bit 7 en 1 -> va un numero
	0b10000000					//bit 7 en 1 -> va un numero
};

static const byte tabPROG[] = {
	// abcdefg
	0b01100111,					//P
	0b00000101,					//r chica
	0b00011101,					//o chica
	0b01111011,					//g chica
	0b00000000					//apagado
};

static const byte tabERR[] = {
     //abcdefg
	0b01001111,					//E grande
	0b00000101,					//r chica
	0b00000101,					//r chica
	0b10000000,					//bit 7 en 1 -> va un numero
	0b10000000					//bit 7 en 1 -> va un numero
};

static const byte tabCONF[] = {
     //abcdefg
	0b00001101,					//c
	0b00011101,					//o chica
	0b00010101,					//n
	0b01000111,					//F
	0b00000000					//apagado
};

static const byte tabINI[] = {
    //abcdefg
	0b00010000,					//i
	0b00010101,					//n
	0b00010000,					//i
	0b00000000,					//apagado
	0b10000000					//bit 7 en 1 -> va un numero
};

static const byte tabFIN[] = {
    //abcdefg
	0b01000111,					//F
	0b00010000,					//i
	0b00010101,					//n
	0b00000000,					//apagado
	0b10000000					//bit 7 en 1 -> va un numero
};


static const byte tabTURN[] = {
    //abcdefg
	0b00001111,					//t
	0b00011100,					//u
	0b00000101,					//r chica
	0b00010101,					//n
	0b00000000					//apagado
};

static const byte tabTEST[] = {
    //abcdefg
	0b01000111,					//F
	0b00010000,					//i
	0b00010101,					//n
	0b00000000,					//apagado
	0b10000000					//bit 7 en 1 -> va un numero
};

static const uint32_t tabDIR_mostrarTEXTO[] = {
	&tabPROG,
	&tabERR,
	&tabCONF,
	&tabINI,
	&tabFIN,
	&tabTURN,
	&tabCLEAR
};


#define	MOSTRAR_PROG	0
#define	MOSTRAR_ERR		1
#define	MOSTRAR_CONF	2
#define	MOSTRAR_INI		3
#define	MOSTRAR_FIN		4
#define	MOSTRAR_TURN	5
#define	MOSTRAR_CLEAR	6

#define  Numero_0		tabDIGITOS[0];
#define  Numero_1		tabDIGITOS[1];
#define  Numero_2		tabDIGITOS[2];
#define  Numero_3		tabDIGITOS[3];
#define  Numero_4		tabDIGITOS[4];
#define  Numero_5		tabDIGITOS[5];
#define  Numero_6		tabDIGITOS[6];
#define  Numero_7		tabDIGITOS[7];
#define  Numero_8		tabDIGITOS[8];
#define  Numero_9		tabDIGITOS[9];

#define  Letra_L		tabDIG_ESPECIAL[0]
#define  Letra_P		tabDIG_ESPECIAL[1]
#define  Letra_r_chica	tabDIG_ESPECIAL[2]
#define  Letra_r_subida	tabDIG_ESPECIAL[3]
#define  Letra_r_alta	tabDIG_ESPECIAL[4]
#define  Letra_O_grande	tabDIG_ESPECIAL[5]
#define  Letra_o_chica	tabDIG_ESPECIAL[6]
#define  Letra_o_subida	tabDIG_ESPECIAL[7]
#define  Letra_G_grande	tabDIG_ESPECIAL[8]
#define  Letra_g_chica	tabDIG_ESPECIAL[9]
#define  Letra_E 	    tabDIG_ESPECIAL[10]

#define  Letra_c 	    tabDIG_ESPECIAL[11]
#define  Letra_n 	    tabDIG_ESPECIAL[12]
#define  Letra_i 	    tabDIG_ESPECIAL[13]
#define  Letra_F 	    tabDIG_ESPECIAL[14]
#define  Letra_t 	    tabDIG_ESPECIAL[15]

void update_valor_display (uint32_t valor){

    uint32_t bcdDATA;
    byte DIGITOS[10];
    byte printDISPLAY;

    if(statusDISPLAY == DISPLAY_CHK){
     	 valor = 8888888;
    }

    uint32_t_HEX_a_BCD(valor,&DIGITOS);

    DG4_7SEG = DIGITOS[9];
    DG3_7SEG = DIGITOS[8];
    DG2_7SEG = DIGITOS[7];
    DG1_7SEG = DIGITOS[6];
    DG0_7SEG = DIGITOS[5];

    P_7SEG = PUNTO_DECIMAL+1; //sumo uno porque el primer decimal corresponde al digito 2

}

void update_valor_tarifa (uint8_t valor){
	if(statusDISPLAY != DISPLAY_CHK){
		if(seleccionManualTarifas){
			if(valor > nroTARIFA_HAB_MANUAL){
			   valor = valor - nroTARIFA_HAB_MANUAL;
			}
		}else{
			if(valor > nroTARIFA_HAB_AUTOMATICA){
			   valor = valor - nroTARIFA_HAB_AUTOMATICA;
			}
		}
	}
    DGT_7SEG = valor;
}


void print_display (void){
	byte printDISPLAY;
	byte exit;

	   exit = 0;

		print_display_off(digito_encendido);

		if(digito_encendido == 0 ){
			digito_encendido = DIGITO_TARIFA;
		}else{
			digito_encendido--;
		}
		if(prog_mode){
			exit = 1;
			if(error_eepromDATA){
			 if(jumper_pressed){
				 //mostrar_prog();
				 mostrarTEXTO((byte*)tabDIR_mostrarTEXTO[0]);
			 }else{
				 mostrar_err();
			 }
			}else{
				//mostrar_prog();
				mostrarTEXTO((byte*)tabDIR_mostrarTEXTO[0]);
			}
		}

		if(mostrar_ini_dsplyIMPORTE){
			exit = 1;
			mostrarTEXTO((byte*)tabDIR_mostrarTEXTO[MOSTRAR_INI]);
			//apaga punto logica invertida igual que segmentos
			HAL_GPIO_WritePin(PUERTO_SEGMENTO_PUNTO, PIN_SEGMENTO_PUNTO, GPIO_PIN_SET);
		}
		if(mostrar_clear_dsplyEEPROM){
			exit = 1;
			mostrarTEXTO((byte*)tabDIR_mostrarTEXTO[MOSTRAR_CLEAR]);
			//apaga punto logica invertida igual que segmentos
			HAL_GPIO_WritePin(PUERTO_SEGMENTO_PUNTO, PIN_SEGMENTO_PUNTO, GPIO_PIN_SET);
		}



		if(mostrar_conF_dsplyIMPORTE){
			exit = 1;
			mostrarTEXTO((byte*)tabDIR_mostrarTEXTO[MOSTRAR_CONF]);
			//apaga punto logica invertida igual que segmentos
			HAL_GPIO_WritePin(PUERTO_SEGMENTO_PUNTO, PIN_SEGMENTO_PUNTO, GPIO_PIN_SET);
		}

		if(mostrar_turn_dsplyIMPORTE){
			exit = 1;
			mostrarTEXTO((byte*)tabDIR_mostrarTEXTO[MOSTRAR_TURN]);
			//apaga punto logica invertida igual que segmentos
			HAL_GPIO_WritePin(PUERTO_SEGMENTO_PUNTO, PIN_SEGMENTO_PUNTO, GPIO_PIN_SET);
		}

	    if(!exit){
			if(digito_encendido == DIGITO_TARIFA){
			//display tarifa
				if((status_display == DISPLAY_ON_TARIFA) || (status_display == DISPLAY_ON_ALL)){
					if(ESTADO_RELOJ==LIBRE){
						if(!tarifa_to_cnt){
							printDISPLAY = tabDIG_ESPECIAL[0];
						}else{
							printDISPLAY = tabDIGITOS[DGT_7SEG];
						}
					}else if(ESTADO_RELOJ==OCUPADO){
						printDISPLAY = tabDIGITOS[DGT_7SEG];
					}else if(ESTADO_RELOJ==COBRANDO){
						printDISPLAY = tabDIG_ESPECIAL[1];
					}
					print_digito (printDISPLAY, digito_encendido, ON_DIGITO);
				}
			}else{
			//display importe
				if((status_display == DISPLAY_ON_IMPORTE) || (status_display == DISPLAY_ON_ALL)){
					switch(digito_encendido){
						case  4:
							printDISPLAY = tabDIGITOS[DG4_7SEG];
							print_digito (printDISPLAY, digito_encendido, ON_DIGITO);
							break;
						case  3 :
							if(DG3_7SEG!=0 || DG2_7SEG!=0 || DG1_7SEG!=0 || DG0_7SEG!=0 || PUNTO_DECIMAL == 1 || PUNTO_DECIMAL == 2 || PUNTO_DECIMAL == 3 || PUNTO_DECIMAL == 4 ){
								printDISPLAY = tabDIGITOS[DG3_7SEG];
								print_digito (printDISPLAY, digito_encendido, ON_DIGITO);
							}
							break;
						case 2 :
							if(DG2_7SEG!=0 || DG1_7SEG!=0 || DG0_7SEG!=0 || PUNTO_DECIMAL == 2 || PUNTO_DECIMAL == 3 || PUNTO_DECIMAL == 4){
								printDISPLAY = tabDIGITOS[DG2_7SEG];
								print_digito (printDISPLAY, digito_encendido, ON_DIGITO);
							}
							break;
						case 1 :
							if(DG1_7SEG!=0 || DG0_7SEG!=0 || PUNTO_DECIMAL == 3 || PUNTO_DECIMAL == 4 ){
								printDISPLAY = tabDIGITOS[DG1_7SEG];
								print_digito (printDISPLAY, digito_encendido, ON_DIGITO);
							}
							break;
						case  0 :
							if(DG0_7SEG!=0 || PUNTO_DECIMAL == 4){
								printDISPLAY = tabDIGITOS[DG0_7SEG];
								print_digito (printDISPLAY, digito_encendido, ON_DIGITO);
							}
							break;

						default:
							break;
					}

				}
	    	}
	    }
}



void print_display_off (uint8_t digito){

	print_digito (0, digito, OFF_DIGITO);

}

uint8_t prueba;

static void print_digito (uint32_t valor, uint8_t digito, GPIO_PinState control){

	//CONTROL DE SEGMENTOS
	if(control){
		//activa segmentos que correspondan (cuando enciende, cuando apaga estos valores son irrelevantes
		//el if solo es para que no pierda tiempo aca
		HAL_GPIO_WritePin(PUERTO_SEGMENTO_A, PIN_SEGMENTO_A, getSEG(valor,SEG_A_MASK));
		HAL_GPIO_WritePin(PUERTO_SEGMENTO_B, PIN_SEGMENTO_B, getSEG(valor,SEG_B_MASK));
		HAL_GPIO_WritePin(PUERTO_SEGMENTO_C, PIN_SEGMENTO_C, getSEG(valor,SEG_C_MASK));
		HAL_GPIO_WritePin(PUERTO_SEGMENTO_D, PIN_SEGMENTO_D, getSEG(valor,SEG_D_MASK));
		HAL_GPIO_WritePin(PUERTO_SEGMENTO_E, PIN_SEGMENTO_E, getSEG(valor,SEG_E_MASK));
		HAL_GPIO_WritePin(PUERTO_SEGMENTO_F, PIN_SEGMENTO_F, getSEG(valor,SEG_F_MASK));
		HAL_GPIO_WritePin(PUERTO_SEGMENTO_G, PIN_SEGMENTO_G, getSEG(valor,SEG_G_MASK));
	}

	if(statusDISPLAY == DISPLAY_CHK){
		//enciende logica invertida igual que segmentos
		HAL_GPIO_WritePin(PUERTO_SEGMENTO_PUNTO, PIN_SEGMENTO_PUNTO, GPIO_PIN_RESET);
	}else{
		//CONTROL DE PUNTO
		//solo prende el digito 2,3 y 4 (hardware)
		//el digito dos enciende con el catodo 3  corresponde un decimal
		//el digito tres enciende con el catodo 2 corresponde a dos decimales
		//el digito cuatro enciende con el catodo 1 corresponde a tres decimales
		if(digito == (5 - P_7SEG)){
			if(!prog_mode){
				//enciende logica invertida igual que segmentos
				HAL_GPIO_WritePin(PUERTO_SEGMENTO_PUNTO, PIN_SEGMENTO_PUNTO, GPIO_PIN_RESET);
			}
		}else{
			//apaga logica invertida igual que segmentos
			HAL_GPIO_WritePin(PUERTO_SEGMENTO_PUNTO, PIN_SEGMENTO_PUNTO, GPIO_PIN_SET);
		}
		//enciendo o p agago punto de digito indicador de tarifa
		//para indicar si esta contando pulso o tiempo
		if(ESTADO_RELOJ == OCUPADO){
			if(tarifando_tiempo()){
				//contando tiempo
				if(digito == DIGITO_TARIFA){
					if(!prog_mode){
						//enciende logica invertida igual que segmentos
						HAL_GPIO_WritePin(PUERTO_SEGMENTO_PUNTO, PIN_SEGMENTO_PUNTO, GPIO_PIN_RESET);
					}
				}
			}else{
				//no contando tiempo
				if(digito == DIGITO_TARIFA){
					//apaga logica invertida igual que segmentos
					HAL_GPIO_WritePin(PUERTO_SEGMENTO_PUNTO, PIN_SEGMENTO_PUNTO, GPIO_PIN_SET);
				}
			}
		}else{
			if(digito == DIGITO_TARIFA){
				//apaga logica invertida igual que segmentos
				HAL_GPIO_WritePin(PUERTO_SEGMENTO_PUNTO, PIN_SEGMENTO_PUNTO, GPIO_PIN_SET);
			}
		}
	}



	//CONTROL DE CATODOS
	switch(digito){

		//habilito catodo que corresponda (enciende segmentos activos)
		case 0 : HAL_GPIO_WritePin(PUERTO_CATODO_DIGITO_0, PIN_CATODO_DIGITO_0, control); break;
		case 1 : HAL_GPIO_WritePin(PUERTO_CATODO_DIGITO_1, PIN_CATODO_DIGITO_1, control); break;
		case 2 : HAL_GPIO_WritePin(PUERTO_CATODO_DIGITO_2, PIN_CATODO_DIGITO_2, control); break;
		case 3 : HAL_GPIO_WritePin(PUERTO_CATODO_DIGITO_3, PIN_CATODO_DIGITO_3, control); break;
		case 4 : HAL_GPIO_WritePin(PUERTO_CATODO_DIGITO_4, PIN_CATODO_DIGITO_4, control); break;

/*
		//habilito catodo que corresponda (enciende segmentos activos)
		case 0 : HAL_GPIO_WritePin(PUERTO_CATODO_DIGITO_4, PIN_CATODO_DIGITO_4, control); break;
		case 1 : HAL_GPIO_WritePin(PUERTO_CATODO_DIGITO_3, PIN_CATODO_DIGITO_3, control); break;
		case 2 : HAL_GPIO_WritePin(PUERTO_CATODO_DIGITO_2, PIN_CATODO_DIGITO_2, control); break;
		case 3 : HAL_GPIO_WritePin(PUERTO_CATODO_DIGITO_1, PIN_CATODO_DIGITO_1, control); break;
		case 4 : HAL_GPIO_WritePin(PUERTO_CATODO_DIGITO_0, PIN_CATODO_DIGITO_0, control); break;
*/
		case 5 : HAL_GPIO_WritePin(PUERTO_CATODO_DIGITO_5, PIN_CATODO_DIGITO_5, control); break;
		default:  break;
	}
}

static  GPIO_PinState getSEG (uint8_t valor, uint8_t segmento ){
  byte mask;
    mask = segmento;
    valor = valor & mask;
	//return (valor);
	return (!valor);
}


extern void ini_display_7seg (void){

	//56789
	//update_valor_display(1023456789);

	digito_encendido = 0;
	to_dsply_on=TO_DISPLAY_ON;

	ini_catodos_display_7seg();
	ini_segmentos_display_7seg();
}

void ini_catodos_display_7seg (void){

	GPIO_InitTypeDef  GPIO_InitStruct;

        //Config. PIN_CATODO_DIGITO_0
	    CATODO_DIGITO_0_CLK_ENABLE();
		GPIO_InitStruct.Pin = PIN_CATODO_DIGITO_0;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(PUERTO_CATODO_DIGITO_0, &GPIO_InitStruct);
		HAL_GPIO_WritePin(PUERTO_CATODO_DIGITO_0, PIN_CATODO_DIGITO_0, GPIO_PIN_RESET);

        //Config. PIN_CATODO_DIGITO_1
	    CATODO_DIGITO_1_CLK_ENABLE();
		GPIO_InitStruct.Pin = PIN_CATODO_DIGITO_1;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(PUERTO_CATODO_DIGITO_1, &GPIO_InitStruct);
		HAL_GPIO_WritePin(PUERTO_CATODO_DIGITO_1, PIN_CATODO_DIGITO_1, GPIO_PIN_RESET);

        //Config. PIN_CATODO_DIGITO_2
	    CATODO_DIGITO_2_CLK_ENABLE();
		GPIO_InitStruct.Pin = PIN_CATODO_DIGITO_2;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(PUERTO_CATODO_DIGITO_2, &GPIO_InitStruct);
		HAL_GPIO_WritePin(PUERTO_CATODO_DIGITO_2, PIN_CATODO_DIGITO_2, GPIO_PIN_RESET);

        //Config. PIN_CATODO_DIGITO_3
	    CATODO_DIGITO_3_CLK_ENABLE();
		GPIO_InitStruct.Pin = PIN_CATODO_DIGITO_3;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(PUERTO_CATODO_DIGITO_3, &GPIO_InitStruct);
		HAL_GPIO_WritePin(PUERTO_CATODO_DIGITO_3, PIN_CATODO_DIGITO_3, GPIO_PIN_RESET);

        //Config. PIN_CATODO_DIGITO_4
	    CATODO_DIGITO_4_CLK_ENABLE();
		GPIO_InitStruct.Pin = PIN_CATODO_DIGITO_4;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(PUERTO_CATODO_DIGITO_4, &GPIO_InitStruct);
		HAL_GPIO_WritePin(PUERTO_CATODO_DIGITO_4, PIN_CATODO_DIGITO_4, GPIO_PIN_RESET);

        //Config. PIN_CATODO_DIGITO_5
	    CATODO_DIGITO_5_CLK_ENABLE();
		GPIO_InitStruct.Pin = PIN_CATODO_DIGITO_5;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(PUERTO_CATODO_DIGITO_5, &GPIO_InitStruct);
		HAL_GPIO_WritePin(PUERTO_CATODO_DIGITO_5, PIN_CATODO_DIGITO_5, GPIO_PIN_RESET);

	}


void ini_segmentos_display_7seg (void){

	GPIO_InitTypeDef  GPIO_InitStruct;

		//Config. SEGMENTO_A
	    SEGMENTO_A_CLK_ENABLE();
		GPIO_InitStruct.Pin = PIN_SEGMENTO_A;
		//GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		//GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(PUERTO_SEGMENTO_A, &GPIO_InitStruct);
		HAL_GPIO_WritePin(PUERTO_SEGMENTO_A, PIN_SEGMENTO_A, GPIO_PIN_RESET);
		//Config. SEGMENTO_B
	    SEGMENTO_B_CLK_ENABLE();
		GPIO_InitStruct.Pin = PIN_SEGMENTO_B;
		//GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		//GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(PUERTO_SEGMENTO_B, &GPIO_InitStruct);
		HAL_GPIO_WritePin(PUERTO_SEGMENTO_B, PIN_SEGMENTO_B, GPIO_PIN_RESET);
		//Config. SEGMENTO_C
	    SEGMENTO_C_CLK_ENABLE();
		GPIO_InitStruct.Pin = PIN_SEGMENTO_C;
		//GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		//GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(PUERTO_SEGMENTO_C, &GPIO_InitStruct);
		HAL_GPIO_WritePin(PUERTO_SEGMENTO_C, PIN_SEGMENTO_C, GPIO_PIN_RESET);
		//Config. SEGMENTO_D
	    SEGMENTO_D_CLK_ENABLE();
		GPIO_InitStruct.Pin = PIN_SEGMENTO_D;
		//GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		//GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(PUERTO_SEGMENTO_D, &GPIO_InitStruct);
		HAL_GPIO_WritePin(PUERTO_SEGMENTO_D, PIN_SEGMENTO_D, GPIO_PIN_RESET);
		//Config. SEGMENTO_E
	    SEGMENTO_E_CLK_ENABLE();
		GPIO_InitStruct.Pin = PIN_SEGMENTO_E;
		//GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		//GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(PUERTO_SEGMENTO_E, &GPIO_InitStruct);
		HAL_GPIO_WritePin(PUERTO_SEGMENTO_E, PIN_SEGMENTO_E, GPIO_PIN_RESET);
		//Config. SEGMENTO_F
	    SEGMENTO_F_CLK_ENABLE();
		GPIO_InitStruct.Pin = PIN_SEGMENTO_F;
		//GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		//GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(PUERTO_SEGMENTO_F, &GPIO_InitStruct);
		HAL_GPIO_WritePin(PUERTO_SEGMENTO_F, PIN_SEGMENTO_F, GPIO_PIN_RESET);
		//Config. SEGMENTO_G
	    SEGMENTO_G_CLK_ENABLE();
		GPIO_InitStruct.Pin = PIN_SEGMENTO_G;
		//GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		//GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(PUERTO_SEGMENTO_G, &GPIO_InitStruct);
		HAL_GPIO_WritePin(PUERTO_SEGMENTO_G, PIN_SEGMENTO_G, GPIO_PIN_RESET);

		//Config. SEGMENTO_PUNTO
	    SEGMENTO_PUNTO_CLK_ENABLE();
		GPIO_InitStruct.Pin = PIN_SEGMENTO_PUNTO;
		//GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		//GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(PUERTO_SEGMENTO_PUNTO, &GPIO_InitStruct);
		HAL_GPIO_WritePin(PUERTO_SEGMENTO_PUNTO, PIN_SEGMENTO_PUNTO, GPIO_PIN_RESET);

}


void TST_segmentos_display_7seg (void){

	GPIO_InitTypeDef  GPIO_InitStruct;


		//Config. SEGMENTO_B
		SEGMENTO_B_CLK_ENABLE();
		GPIO_InitStruct.Pin = PIN_SEGMENTO_B;
		//GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		//GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(PUERTO_SEGMENTO_B, &GPIO_InitStruct);
		HAL_GPIO_WritePin(PUERTO_SEGMENTO_B, PIN_SEGMENTO_B, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(PUERTO_SEGMENTO_B, PIN_SEGMENTO_B, GPIO_PIN_SET);

	    //Config. SEGMENTO_A
	    SEGMENTO_A_CLK_ENABLE();
		GPIO_InitStruct.Pin = PIN_SEGMENTO_A;
		//GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		//GPIO_InitStruct.Pull = GPIO_PULLUP;

		//GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
		//GPIO_InitStruct.Pull = GPIO_NOPULL;

		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_PULLUP;

		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(PUERTO_SEGMENTO_A, &GPIO_InitStruct);
		//HAL_GPIO_WritePin(PUERTO_SEGMENTO_A, PIN_SEGMENTO_A, GPIO_PIN_RESET);
		//HAL_GPIO_WritePin(PUERTO_SEGMENTO_A, PIN_SEGMENTO_A, GPIO_PIN_SET);


		//Config. SEGMENTO_C
	    SEGMENTO_C_CLK_ENABLE();
		GPIO_InitStruct.Pin = PIN_SEGMENTO_C;
		//GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		//GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(PUERTO_SEGMENTO_C, &GPIO_InitStruct);
		HAL_GPIO_WritePin(PUERTO_SEGMENTO_C, PIN_SEGMENTO_C, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(PUERTO_SEGMENTO_C, PIN_SEGMENTO_C, GPIO_PIN_SET);

		//Config. SEGMENTO_D
	    SEGMENTO_D_CLK_ENABLE();
		GPIO_InitStruct.Pin = PIN_SEGMENTO_D;
		//GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		//GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(PUERTO_SEGMENTO_D, &GPIO_InitStruct);
		HAL_GPIO_WritePin(PUERTO_SEGMENTO_D, PIN_SEGMENTO_D, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(PUERTO_SEGMENTO_D, PIN_SEGMENTO_D, GPIO_PIN_SET);

		//Config. SEGMENTO_E
	    SEGMENTO_E_CLK_ENABLE();
		GPIO_InitStruct.Pin = PIN_SEGMENTO_E;
		//GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		//GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(PUERTO_SEGMENTO_E, &GPIO_InitStruct);
		HAL_GPIO_WritePin(PUERTO_SEGMENTO_E, PIN_SEGMENTO_E, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(PUERTO_SEGMENTO_E, PIN_SEGMENTO_E, GPIO_PIN_SET);

		//Config. SEGMENTO_F
	    SEGMENTO_F_CLK_ENABLE();
		GPIO_InitStruct.Pin = PIN_SEGMENTO_F;
		//GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		//GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(PUERTO_SEGMENTO_F, &GPIO_InitStruct);
		HAL_GPIO_WritePin(PUERTO_SEGMENTO_F, PIN_SEGMENTO_F, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(PUERTO_SEGMENTO_F, PIN_SEGMENTO_F, GPIO_PIN_SET);

		//Config. SEGMENTO_G
	    SEGMENTO_G_CLK_ENABLE();
		GPIO_InitStruct.Pin = PIN_SEGMENTO_G;
		//GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		//GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(PUERTO_SEGMENTO_G, &GPIO_InitStruct);
		HAL_GPIO_WritePin(PUERTO_SEGMENTO_G, PIN_SEGMENTO_G, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(PUERTO_SEGMENTO_G, PIN_SEGMENTO_G, GPIO_PIN_SET);

		//Config. SEGMENTO_PUNTO
	    SEGMENTO_PUNTO_CLK_ENABLE();
		GPIO_InitStruct.Pin = PIN_SEGMENTO_PUNTO;
		//GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		//GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(PUERTO_SEGMENTO_PUNTO, &GPIO_InitStruct);
		HAL_GPIO_WritePin(PUERTO_SEGMENTO_PUNTO, PIN_SEGMENTO_PUNTO, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(PUERTO_SEGMENTO_PUNTO, PIN_SEGMENTO_PUNTO, GPIO_PIN_SET);

	    ini_catodos_display_7seg();
	    HAL_GPIO_WritePin(PUERTO_CATODO_DIGITO_4, PIN_CATODO_DIGITO_4, GPIO_PIN_SET);
	    //HAL_GPIO_WritePin(PUERTO_CATODO_DIGITO_4, PIN_CATODO_DIGITO_4, GPIO_PIN_RESET);
	    //HAL_GPIO_WritePin(PUERTO_CATODO_DIGITO_3, PIN_CATODO_DIGITO_3, GPIO_PIN_SET);
	    //HAL_GPIO_WritePin(PUERTO_CATODO_DIGITO_3, PIN_CATODO_DIGITO_3, GPIO_PIN_RESET);
	    //HAL_GPIO_WritePin(PUERTO_CATODO_DIGITO_2, PIN_CATODO_DIGITO_2, GPIO_PIN_SET);
	    //HAL_GPIO_WritePin(PUERTO_CATODO_DIGITO_2, PIN_CATODO_DIGITO_2, GPIO_PIN_RESET);
	    //HAL_GPIO_WritePin(PUERTO_CATODO_DIGITO_1, PIN_CATODO_DIGITO_1, GPIO_PIN_SET);
	    //HAL_GPIO_WritePin(PUERTO_CATODO_DIGITO_1, PIN_CATODO_DIGITO_1, GPIO_PIN_RESET);


}

void TEST_DISPLAY_7SEG(void){

	status_display = DISPLAY_CHK;


	//APAGAR
	HAL_GPIO_WritePin(PUERTO_SEGMENTO_A, PIN_SEGMENTO_A, GPIO_PIN_SET);
	HAL_GPIO_WritePin(PUERTO_SEGMENTO_B, PIN_SEGMENTO_B, GPIO_PIN_SET);
	HAL_GPIO_WritePin(PUERTO_SEGMENTO_C, PIN_SEGMENTO_C, GPIO_PIN_SET);
	HAL_GPIO_WritePin(PUERTO_SEGMENTO_D, PIN_SEGMENTO_D, GPIO_PIN_SET);
	HAL_GPIO_WritePin(PUERTO_SEGMENTO_E, PIN_SEGMENTO_E, GPIO_PIN_SET);
	HAL_GPIO_WritePin(PUERTO_SEGMENTO_F, PIN_SEGMENTO_F, GPIO_PIN_SET);
	HAL_GPIO_WritePin(PUERTO_SEGMENTO_G, PIN_SEGMENTO_G, GPIO_PIN_SET);

	//ENCENDER catodo
	HAL_GPIO_WritePin(PUERTO_CATODO_DIGITO_4, PIN_CATODO_DIGITO_4, GPIO_PIN_SET);

	HAL_GPIO_WritePin(PUERTO_SEGMENTO_A, PIN_SEGMENTO_A, GPIO_PIN_RESET);
	//RETARDO
	HAL_Delay(500);
	HAL_GPIO_WritePin(PUERTO_SEGMENTO_B, PIN_SEGMENTO_B, GPIO_PIN_RESET);
	//RETARDO
	HAL_Delay(500);
	HAL_GPIO_WritePin(PUERTO_SEGMENTO_C, PIN_SEGMENTO_C, GPIO_PIN_RESET);
	//RETARDO
	HAL_Delay(500);
	HAL_GPIO_WritePin(PUERTO_SEGMENTO_D, PIN_SEGMENTO_D, GPIO_PIN_RESET);
	//RETARDO
	HAL_Delay(500);
	HAL_GPIO_WritePin(PUERTO_SEGMENTO_E, PIN_SEGMENTO_E, GPIO_PIN_RESET);
	//RETARDO
	HAL_Delay(500);
	HAL_GPIO_WritePin(PUERTO_SEGMENTO_F, PIN_SEGMENTO_F, GPIO_PIN_RESET);
	//RETARDO
	HAL_Delay(500);
	HAL_GPIO_WritePin(PUERTO_SEGMENTO_G, PIN_SEGMENTO_G, GPIO_PIN_RESET);
	//RETARDO
	HAL_Delay(500);

}

void to_display_on (void ){

	if(to_dsply_on!=0){
		to_dsply_on--;
		if(to_dsply_on==0){
			print_display();
			to_dsply_on=TO_DISPLAY_ON;
		}
	}
}

void toggle_display (void ){
	toggle_dsply = TO_DISPLAY_TOGGLE;
}

void toggle_displayTARIFA (void ){
	toggle_dsplyTARIFA = TO_DISPLAY_TOGGLE;
}

/*
void display_off_toggle (void ){
	status_display = DISPLAY_ON;
	toggle_dsply = 0;
}
*/
void off_display (void ){
	status_display = DISPLAY_OFF;
	toggle_dsply = 0;
	toggle_dsplyTARIFA =0;
	HAL_GPIO_WritePin(PUERTO_CATODO_DIGITO_0, PIN_CATODO_DIGITO_0, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(PUERTO_CATODO_DIGITO_1, PIN_CATODO_DIGITO_1, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(PUERTO_CATODO_DIGITO_2, PIN_CATODO_DIGITO_2, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(PUERTO_CATODO_DIGITO_3, PIN_CATODO_DIGITO_3, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(PUERTO_CATODO_DIGITO_4, PIN_CATODO_DIGITO_4, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(PUERTO_CATODO_DIGITO_5, PIN_CATODO_DIGITO_5, GPIO_PIN_RESET);
}

void on_display_importe (void ){
	status_display = DISPLAY_ON_IMPORTE;
	toggle_dsply = 0;
	toggle_dsplyTARIFA =0;

}
void on_display_tarifa (void ){
	status_display = DISPLAY_ON_TARIFA;
	toggle_dsply = 0;
	toggle_dsplyTARIFA =0;
}

void on_display_all (void ){
	status_display = DISPLAY_ON_ALL;
	toggle_dsply = 0;
	toggle_dsplyTARIFA =0;

}

void to_toggle_display (void ){

	if(toggle_dsply !=0){
		toggle_dsply--;
		if(toggle_dsply==0){
			toggle_dsply = TO_DISPLAY_TOGGLE;
			if(status_display==DISPLAY_OFF){
				status_display = DISPLAY_ON_ALL;
			}else{
				status_display = DISPLAY_OFF;
			}
		}
	}
}



void to_toggleTARIFA_display (void ){

	if(toggle_dsplyTARIFA !=0){
		toggle_dsplyTARIFA--;
		if(toggle_dsplyTARIFA==0){
			toggle_dsplyTARIFA = TO_DISPLAY_TOGGLE;
			if(status_display==DISPLAY_OFF_TARIFA){
				status_display = DISPLAY_ON_TARIFA;
			}else{
				status_display = DISPLAY_OFF_TARIFA;
			}
		}
	}
}

void to_test_display (void ){

	if(test_display_TO !=0){
		test_display_TO--;
		if(test_display_TO==0){
			statusDISPLAY = DISPLAY_NORMAL;
			off_display();
			on_display_tarifa();
			tarifa_to_cnt = 0;
			PUNTO_DECIMAL = auxPUNTO_DECIMAL;
			ini_pulsador_impresion();
			chkEepromDATA();
			check_jumperPROG();
		}
	}
}

void setTEST_DISPLAY(void){
	off_display();
	statusDISPLAY = DISPLAY_CHK;
	update_valor_display(888888);
	test_display_TO = TEST_DISPLAY_TO;
	toggle_display();
	tarifa_to_cnt = 8; //para que no muestre digito especial
	update_valor_tarifa(8);
	auxPUNTO_DECIMAL = PUNTO_DECIMAL;
	PUNTO_DECIMAL = 4;
}



static void mostrar_prog(void){

	byte printDISPLAY;
/*
Letra_P
Letra_r_alta
Letra_r_chica
Letra_r_subida
Letra_O_grande
Letra_o_chica
Letra_o_subida
Letra_G_grande
Letra_g_chica
*/
	switch(digito_encendido){
		case  0:
			//printDISPLAY = tabDIGITOS[DG4_7SEG];
			printDISPLAY = Letra_P;
			print_digito (printDISPLAY, digito_encendido, ON_DIGITO);
			break;
		case  1 :
			    cnt_1=2;
				if(cnt_1==1){
					printDISPLAY = Letra_r_alta;
				}else if(cnt_1==2){
					printDISPLAY = Letra_r_chica;
				}else{
					printDISPLAY = Letra_r_subida;
				};

				print_digito (printDISPLAY, digito_encendido, ON_DIGITO);
			break;
		case 2 :
			cnt_2=2;
			if(cnt_2==1){
				printDISPLAY = Letra_O_grande;
			}else if(cnt_2==2){
				printDISPLAY = Letra_o_chica;

			}else{
				printDISPLAY = Letra_o_subida;
			};


				print_digito (printDISPLAY, digito_encendido, ON_DIGITO);
			break;
		case 3 :
			cnt_3=2;
			if(cnt_3==1){
				printDISPLAY = Letra_G_grande;
			}else if(cnt_3==2){
				printDISPLAY = Letra_g_chica;
			}

			print_digito (printDISPLAY, digito_encendido, ON_DIGITO);
			break;
		case  4 :
				print_digito (printDISPLAY, digito_encendido, OFF_DIGITO);
			break;

		default:
			break;
	}
}



static void mostrar_err(void){

	byte printDISPLAY;
/*
Letra_P
Letra_r_alta
Letra_r_chica
Letra_r_subida
Letra_O_grande
Letra_o_chica
Letra_o_subida
Letra_G_grande
Letra_g_chica
*/
	switch(digito_encendido){
		case  0:
			//printDISPLAY = tabDIGITOS[DG4_7SEG];
			printDISPLAY = Letra_E;
			print_digito (printDISPLAY, digito_encendido, ON_DIGITO);
			break;
		case  1 :
				printDISPLAY = Letra_r_chica;
				print_digito (printDISPLAY, digito_encendido, ON_DIGITO);
			break;
		case 2 :
				printDISPLAY = Letra_r_chica;
				print_digito (printDISPLAY, digito_encendido, ON_DIGITO);
			break;
		case 3 :
				printDISPLAY = Numero_0;
				print_digito (printDISPLAY, digito_encendido, ON_DIGITO);
			break;
		case  4 :
				printDISPLAY = Numero_1;
				print_digito (printDISPLAY, digito_encendido, ON_DIGITO);
			break;

		default:
			break;
	}
}



void mostrar_ini(void){

	byte printDISPLAY;
/*
Letra_P
Letra_r_alta
Letra_r_chica
Letra_r_subida
Letra_O_grande
Letra_o_chica
Letra_o_subida
Letra_G_grande
Letra_g_chica
*/
	switch(digito_encendido){
		case  0:
			//printDISPLAY = tabDIGITOS[DG4_7SEG];
			printDISPLAY = Letra_i;
			print_digito (printDISPLAY, digito_encendido, ON_DIGITO);
			break;
		case  1 :
				printDISPLAY = Letra_n;
				print_digito (printDISPLAY, digito_encendido, ON_DIGITO);
			break;
		case 2 :
				printDISPLAY = Letra_i;
				print_digito (printDISPLAY, digito_encendido, ON_DIGITO);
			break;

		default:
			break;
	}
}


static void mostrarTEXTO(byte* ptr_textDISPLAY){

	byte printDISPLAY;

    byte segmentos;
    byte digito0,digito1,digito2,digito3,digito4;
    byte exit;
    digito0 = 1;
    digito1 = 2;
    digito2 = 3;
    digito3 = 4;
    digito4 = indexMenu_IniTurno;

    exit = 0;

	if(digito_encendido == DIGITO_TARIFA){
		exit =1;
		if(ESTADO_RELOJ==FUERA_SERVICIO){
		   if(mostrar_ini_dsplyIMPORTE){
				printDISPLAY = Letra_t;
				print_digito (printDISPLAY, digito_encendido, ON_DIGITO);
			}else if (mostrar_conF_dsplyIMPORTE){
				printDISPLAY = tabDIGITOS[indexMenu_IniTurno];
				print_digito (printDISPLAY, digito_encendido, ON_DIGITO);
		   }else if(mostrar_turn_dsplyIMPORTE){
				printDISPLAY = Letra_t;
				print_digito (printDISPLAY, digito_encendido, ON_DIGITO);
		   }

	  }
	}
    if(!exit){

        segmentos 		= ptr_textDISPLAY[digito_encendido];

        if(segmentos == 0){
    		print_digito (printDISPLAY, digito_encendido, OFF_DIGITO);
    	}else{
    		if( (segmentos&0x80) == 0){
    		   //letra
    			printDISPLAY = ptr_textDISPLAY[digito_encendido];
    			print_digito (printDISPLAY, digito_encendido, ON_DIGITO);
    		}else{
    			if(mostrar_clear_dsplyEEPROM){
    				byte DIGITOS[10];
    				uint32_t_HEX_a_BCD(indexMenu_IniTurno,&DIGITOS);
    				digito4 = DIGITOS[9];
    			    digito3 = DIGITOS[8];
    			}


    			//numero
    			switch(digito_encendido){
    					case  0:
    						//printDISPLAY = tabDIGITOS[DG4_7SEG];
    						printDISPLAY = tabDIGITOS[digito0];
    						break;
    					case  1 :
    							printDISPLAY = tabDIGITOS[digito1];
    						break;
    					case 2 :
    							printDISPLAY = tabDIGITOS[digito2];
    						break;
    					case 3 :
    							printDISPLAY = tabDIGITOS[digito3];
    						break;
    					case  4 :
    							printDISPLAY = tabDIGITOS[digito4];
    						break;

    					default:
    						break;
    				}
    				print_digito (printDISPLAY, digito_encendido, ON_DIGITO);
    		}
    	}
    }
}

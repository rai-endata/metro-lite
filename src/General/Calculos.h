/* File: <Calculos.h> */

#ifndef _CALCULOS_
  #define _CALCULOS_

  /* INCLUDES */
  /************/
#include "Definicion de tipos.h"
//#include "- Reloj-Blue Config -.h"
#include "- metroBLUE Config -.h"
#include "stdint.h"
 // #include "General\Inicio.h"
  
#ifndef VISOR_EXTERNAL_GPS
  // GPS propio del Equipo
 // #include "GPS\GPS.h"
#else
  // GPS externo
  //#include "GPS\GPS externo.h"
#endif
    
  /* FUNCIONES MATEMATICAS */
  /*************************/
      extern uint16_t potencia (byte x, byte y);
      extern uint32_t potenciaDWORD (uint16_t x, byte y);
      extern uint32_t SQRT_aprox (uint32_t xval);
      extern uint32_t Pitagoras (uint16_t x, uint16_t y);
      extern word BCDtoHEX_2BYTES (byte* num_bcd);

  /* CALCULOS CON COORDENADAS */
  /****************************/
    // Tabla de Equivalencia en Metros de Longitud a distintas Latitudes
    // Ejemplo:
    // Supongamos que la equivalencia es:
    //
    //    1min = 1253 mts
    //
    // Como trabajo con minutos con decimales, 1min = 100 decimas
    //
    //    1min = 100decimas = 1253mts
    //
    // Por tanto cada decima equivale a:
    //
    //    1 decima = 12.53mts
    //
    // La tabla de internet:
    // http://sites.google.com/site/antoniopadillac/Home/pesosymedidas/meridianosyparalelos
    // 
    // tiene equivalencias en MINUTOS, como yo trabajo en decimas, divido x100 el valor  
    //
    // Para no gastar memoria y definir toda la tabla, solo uso el valor que le corresponde
    // de acuerdo a la localidad
      #define EqLng_00lat      185542   // 1' = 1855.42mts (como trabajo con 2 decimales => x100) @ 00º Latitud
      #define EqLng_05lat      184836   // 1' = 1848.36mts (como trabajo con 2 decimales => x100) @ 05º Latitud
      #define EqLng_10lat      182723   // 1' = 1827.23mts (como trabajo con 2 decimales => x100) @ 10º Latitud
      #define EqLng_15lat      179219   // 1' = 1792.19mts (como trabajo con 2 decimales => x100) @ 15º Latitud
      #define EqLng_20lat      174352   // 1' = 1743.52mts (como trabajo con 2 decimales => x100) @ 20º Latitud
      #define EqLng_25lat      168158   // 1' = 1681.58mts (como trabajo con 2 decimales => x100) @ 25º Latitud
      #define EqLng_30lat      160684   // 1' = 1606.84mts (como trabajo con 2 decimales => x100) @ 30º Latitud
      #define EqLng_35lat      151987   // 1' = 1519.87mts (como trabajo con 2 decimales => x100) @ 35º Latitud
      #define EqLng_40lat      142133   // 1' = 1421.33mts (como trabajo con 2 decimales => x100) @ 40º Latitud
      #define EqLng_45lat      131198   // 1' = 1311.98mts (como trabajo con 2 decimales => x100) @ 45º Latitud
      #define EqLng_50lat      119264   // 1' = 1192.64mts (como trabajo con 2 decimales => x100) @ 50º Latitud
      #define EqLng_55lat      106422   // 1' = 1064.22mts (como trabajo con 2 decimales => x100) @ 55º Latitud
      #define EqLng_60lat       92771   // 1' = 927.71mts (como trabajo con 2 decimales => x100) @ 60º Latitud
      #define EqLng_65lat       78413   // 1' = 784.13mts (como trabajo con 2 decimales => x100) @ 65º Latitud
      #define EqLng_70lat       63459   // 1' = 634.59mts (como trabajo con 2 decimales => x100) @ 70º Latitud
      #define EqLng_75lat       48022   // 1' = 480.22mts (como trabajo con 2 decimales => x100) @ 75º Latitud
      #define EqLng_80lat       32219   // 1' = 322.19mts (como trabajo con 2 decimales => x100) @ 80º Latitud
      #define EqLng_85lat       16171   // 1' = 161.71mts (como trabajo con 2 decimales => x100) @ 85º Latitud
            
      // En cambio, la equivalencia en LATITUD no varia significativamente a medida
      // que nos desplazamos sobre los distintos meridianos (longitud)
      #define LatEqMts         185542   // 1' = 1855.42mts (como trabajo con 2 decimales => x100)
      
	  #define ALTA		1
	  #define BAJA		0

      extern uint32_t convert_toLongCoord (byte* coord);
      extern uint32_t convert_toLongCoord_DECIMAL (byte* coord);
      extern uint16_t REFDiff_GraMinDec (byte* coord, byte latlng, tGeoPOINT POSICION_REF);
      extern uint16_t ABSDiff_GraMinDec (byte* coord1, byte* coord2);
      //extern void coordDiff_GGMMDD (byte* coord_1, byte* coord_2, byte* diff);

  #ifndef VISOR_EXTERNAL_GPS      
      extern byte calc_CuadranteAngulo (tGeoPOINT* pos1, tGeoPOINT* pos2);
      extern uint32_t calculo_distancia_cualitativo (tGeoPOINT P1, tGeoPOINT P2);
  #endif  
      extern uint32_t calculo_distancia_aproximado (tGeoPOINT P1, tGeoPOINT P2);
  

  /* CONVERSIONES DE FORMATO */
  /***************************/
      extern byte ASCIItoHEX (byte ascii);
      extern byte BCDtoHEX (byte num_bcd);
      extern uint32_t dword_BCDtoHEX (uint32_t num_bcd);
      extern byte HEXtoBCD (byte num_dec);
      extern uint16_t uint16_t_HEXtoBCD (uint16_t num_dec);
      extern uint16_t BCDtoASCII (byte num_BCD);
      
  
  /* CONVERSIONES NUMERICAS -> STRING */
  /************************************/    
      #define MAX_LENGTH    32+1

      // Bases
      #define base_BCD      16
      #define base_HEX      16
      #define base_DECIMAL  10

      extern byte calcCantDigitos (uint32_t num, byte base);
      extern void convert_to_string(byte* buf, uint32_t i, byte cant_ceros, int base);
      extern void convert_to_string_with_decimals (byte* buf, uint32_t i, byte cant_ceros, byte decimales, int base);
      extern void convert_to_string_with_decimals_poneCOMA (byte* buf, uint32_t i, byte cant_ceros, byte decimales, int base);
      extern void date_to_string(byte* buffer, tDATE date);
      extern void shortDate_to_string(byte* buffer, tDATE date);
    
    #ifdef VISOR_PRINT_TICKET_FULL
      extern void DDMMAA_to_string(byte* buffer, byte* fecha);
      extern void DDMM_to_string(byte* buffer, byte* fecha);
      extern void HHMM_to_string(byte* buffer, byte* hora);
    #endif
      
    #ifdef VISOR_PLUS
    #ifdef VISOR_PRINT_TICKET_FULL
      extern void numberDecimals_to_speech (byte* buffer, byte* num, byte* middleText);
      extern void number_to_speech (byte* buffer, byte* num);
    #endif
    #endif
  
  
  /* CALCULOS DE FECHA Y HORA */
  /****************************/
    typedef enum{
      FECHA_IGUAL,
      FECHA_MAYOR,
      FECHA_MENOR
    }tCOMPARE_FECHA;
  
    extern void correccionHoraria (byte* fecha, byte* hora, byte uso, byte signo);
    extern byte chkBiciesto (byte anio);
    extern void seconds_to_HMS (byte* HMS_ptr, uint32_t seg);
    extern void seconds_to_HMS_HEX (byte* HMS_buffer, uint32_t seg);
    extern uint32_t restaHoraria (byte* minuendo, byte* sustraendo);
    extern byte diferenciaHoraria (byte* minuendo, byte* sustraendo,uint16_t dif_pedida);
    extern uint32_t restaDate (tDATE date2, tDATE date1);
    extern uint32_t diferenicaDias_toSeconds(uint32_t difDias, byte* hora2, byte* hora1);
    extern byte determineDiaSemana (byte* fecha);
  
  #ifndef VISOR_RELOJ_VIEJO  
    extern uint32_t compressDATE (tDATE date, byte* buffer);
  #endif
  
    extern tCOMPARE_FECHA compareFECHA (tDATE date_1, tDATE date_2);
  
  #ifdef VISOR_PRINT_TICKET_FULL
    extern tDATE getNextDate (tDATE date);
  #endif
  
  #ifdef VISOR_GPS_Tx_NAV_INIT
    extern void date_to_GPS_weekSeconds (byte* week_ptr, byte* secs_ptr, tDATE date);
  #endif

#endif

 /*  ROTAR  A LA IZQUIERDA *   
  **************************/

extern byte  rotarUnBitIzq(byte *a,byte bit);
extern byte  rotarBuffIzq(byte *buff,uint16_t N, byte BIT);

 #ifdef VISOR_USES_SKYPATROL
  extern byte cal_chk(byte N, byte* Buffer_ptr);
  extern byte string2byte_to_hex(byte* buffer_ptr);
  extern byte char_to_hex (byte dato);
 #endif
 
  extern uint32_t uint32_t_HEXtoBCD(uint32_t num);
  extern void uint32_t_HEX_a_BCD( uint32_t num, uint8_t* ptrDATA);

  extern void convert_bigINDIAN_to_litleINDIAN (uint8_t* buffer, uint8_t n);
  extern void  cargaContenidoPuntero_inDATA (uint16_t* ptrDIR, byte* DATA);
  extern byte checkRANGE(byte* ini_ptr, byte*  fin_ptr, byte timeOUT );
  extern uint8_t esbisiesto (uint16_t anio);

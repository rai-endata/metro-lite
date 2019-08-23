/* File: <Calculos.c> */

/* INCLUDES */
/************/
//#include "- Reloj-Blue Config -.h"
#include "- metroBLUE Config -.h"
#include "Calculos.h"
#include "Definicion de tipos.h"
#include "Definiciones Generales.h"
#include "Textos.h"
#include "stddef.h"


  //#include "General\Inicio.h"
#include "Manejo de Buffers.h"

  //#include "GPS\Localidades\- Localidad -.h"
  //#include "GPS\Zonas y Bases.h"
  
  //#include "Programacion\Programacion Parametros.h"
  
  //#include "Temporizaciones\Timer General.h"

/*********************************************************************************************/
/* CONSTANTES */
/**************/

	uint32_t LngEqMts;                         // Para una localidad generica, este valor es VARIABLE

/*********************************************************************************************/
/* CONSTANTES */
/**************/
  // Tabla con los dias en que termina cada mes
  const byte Tabla_FinMeses[]={
    0,        // Nada
    31,       // Enero
    28,       // Febrero
    31,       // Marzo
    30,       // Abril
    31,       // Enero
    30,       // Junio
    31,       // Julio
    31,       // Agosto
    30,       // Septiembre
    31,       // Octubre
    30,       // Noviembre
    31        // Diciembre
  };
  
  
  // Tabla con la clave del mes para el calculo del dia de la semana
  const byte Tabla_ClaveMeses[]={
    0,        // Nada
    0,        // Enero
    3,        // Febrero
    3,        // Marzo
    6,        // Abril
    1,        // Enero
    4,        // Junio
    6,        // Julio
    2,        // Agosto
    5,        // Septiembre
    0,        // Octubre
    3,        // Noviembre
    5         // Diciembre
  };

  /*
  // Tabla de Equivalencia de Lng
   static const uint32_t GENERIC_LOCATION_TablaEqLng[]={
     EqLng_00lat,
     EqLng_05lat,
     EqLng_10lat,
     EqLng_15lat,
     EqLng_20lat,
     EqLng_25lat,
     EqLng_30lat,
     EqLng_35lat,
     EqLng_40lat,
     EqLng_45lat,
     EqLng_50lat,
     EqLng_55lat,
     EqLng_60lat,
     EqLng_65lat,
     EqLng_70lat,
     EqLng_75lat,
     EqLng_80lat,
     EqLng_85lat
   };
  */

#ifdef VISOR_PLUS
#if defined(VISOR_PRINT_TICKET_FULL) && defined(_PREMIUM_)
  // Tabla para la conversion de unidades de MIL
  const byte*far Tabla_Miles[]={
    _EMPTY,
    _mil,
    _dos_mil,
    _tres_mil,
    _cuatro_mil,
    _cinco_mil,
    _seis_mil,
    _siete_mil,
    _ocho_mil,
    _nueve_mil
  };
  
  
  // Tabla para la conversion de CENTENAS
  const byte*far Tabla_Centenas[]={
    _EMPTY,
    _ciento,
    _doscientos,
    _trescientos,
    _cuatrocientos,
    _quinientos,
    _seisientos,
    _setecientos,
    _ochocientos,
    _novecientos
  };
  
  
  // Tabla para la conversion de DECENAS
  const byte*far Tabla_Decenas[]={
    _EMPTY,
    _dieci,
    _veinti,
    _treinta,
    _cuarenta,
    _cincuenta,
    _sesenta,
    _setenta,
    _ochenta,
    _noventa
  };
  
  
  // Tabla para la conversion de UNIDADES
  const byte*far Tabla_Unidades[]={
    _EMPTY,
    _un,
    _dos,
    _tres,
    _cuatro,
    _cinco,
    _seis,
    _siete,
    _ocho,
    _nueve
  };
  
  // Tabla para la conversion de DIEZ
  const byte*far Tabla_diez[]={
    _diez,
    _once,
    _doce,
    _trece,
    _catorce,
    _quince,
    _dieciseis,
    _diecisiete,
    _dieciocho,
    _diecinueve
  };
#endif  
#endif  
  
  // DATE de compresion/referencia
  const tDATE DATE_REFERENCIA= {0x27,0x09,0x12,0x00,0x00,0x00};
  
  // DATE de REFERENCIA PARA GPS
  const tDATE DATE_REFERENCIA_GPS = {0x06,0x01,0x80,0x00,0x00,0x00};

  
/*********************************************************************************************/
/* FUNCIONES MATEMATICAS */
/*************************/
  
  /* POTENCIA */
  /************/
    uint16_t potencia (byte x, byte y){
      // X a la Y
      volatile uint16_t resultado;
      
      resultado=x;
      
      if (y==0){
        resultado=1;
      }else{
        while (y>1){
          y--;
          resultado=resultado*x;
        }
      }
      
      return(resultado);
    }


  /* POTENCIA - DWORD */
  /********************/
    uint32_t potenciaDWORD (uint16_t x, byte y){
      // X a la Y
      uint32_t resultado;
      
      resultado=x;
      
      if (y==0){
        resultado=1;
      }else{
        while (y>1){
          y--;
          resultado = resultado*x;
        }
      }
      
      return(resultado);
    }    
								 

  /* RAIZ CUADRADA - Aproximacion */
  /********************************/
    uint32_t SQRT_aprox (uint32_t xval){
      uint32_t mask, temp, sar;
      uint16_t ix;
      
      mask = 0x8000;                    // Initialize mask for making bit value guessing
      sar = 0;
      for (ix = 0; ix < 16; ix++){
        temp = sar | mask;
        if((temp * temp) <= xval)
          sar = temp;
        mask >>= 1;
      }
      if ((xval - (sar * sar)) > ((sar+1)*(sar+1) - xval))
        sar += 1;
      
      return sar;
    }


  /* PITAGORAS */
  /*************/    
    uint32_t Pitagoras (uint16_t x, uint16_t y){
      // Aplica el Teorema de Pitagoras, para calcula la hipotenusa del triangulo
      // cuyos catetos son X e Y.
      uint32_t x_2, y_2, H, H_2;
      
      x_2 = potenciaDWORD (x,2);        // Calculo x^2
      y_2 = potenciaDWORD (y,2);        // Calculo y^2
      
      H_2 = x_2 + y_2;                  // Calculo H^2
      
      H = SQRT_aprox(H_2);              // Calculo Raiz(H^2)
      
      return (H);
    }
  

/*********************************************************************************************/
/* CALCULOS CON COORDENADAS */
/****************************/

  /* CONVETIR A COORDENADAS EN FORMATO LARGO */
  /*******************************************/
    uint32_t convert_toLongCoord (byte* coord){
      // Convierte a coordenada en formato largo.
      volatile uint32_t longCoord;
      
      longCoord = (((uint32_t)*coord) << 16) | (((uint32_t)*(coord + 1)) << 8) | ((uint32_t)*(coord + 2));
      
      return(longCoord);
      
    }


  /* CONVETIR A COORDENADAS EN FORMATO LARGO EN DECIMAL */
  /*****************************************************/
    uint32_t convert_toLongCoord_DECIMAL (byte* coord){
      // Convierte a coordenada en formato largo, pero en DECIMAL
      uint32_t longCoord;
      byte gra, min, dec;
      
      gra = BCDtoHEX(*coord++);
      min = BCDtoHEX(*coord++);
      dec = BCDtoHEX(*coord++);
      
      longCoord = ((uint32_t)gra * 10000) + ((uint32_t)min * 100) + (uint32_t)dec;
      
      return(longCoord);
    }    

   
  /* DIFERENCIA EN MINUTOS DE COORDENADAS */
  /****************************************/
    static uint16_t coordDiff_GraMinDec (byte* coord_1, byte* coord_2){
      // Las coordenadas tienen el siguiente formato:
      //
      //  GRA | MIN | DEC | POLO |
      //   |		 |		 |		 |
      //   |		 |		 |		 +-------> Polo: N-S-E-W
      //   |		 |		 +-------------> Decimas de Minutos
      //   |		 +-------------------> Minutos
      //   +-------------------------> Grados
      //
      // Tanto GRA,MIN como DEC estan en BCD
      // 
      // Se debe tener en cuenta que si no estan en el mismo
      // polo, no se deben restar las coordenadas sino que deben
      // sumarse
      //
      // De todos modos, el primer paso es convertir las coordenadas
      // a MINUTOS con DECIMALES
      //
      // NO PERMITO LA OPTIMIZACION DE LAS VARIABLES <por eso uso volatile>
      // PORQUE A VECES AL OPTIMIZAR, EL MICRO CALCULA MAL
      //
      // EDIT: 23/06/11
      //  NO FUNCIONA SI LA LONGITUD ES MAYOR A 99º59'59"
      //
      // EDIT: 10/01/12
      //  Devuelve un WORD => Como Maximo --> 655.35' = 10º 55.35'
      //
      // EDIT: 16/01/13
      //  Debo trabajar con dwords. Al finalizar, si es mayor que un word,
      // devuelvo valor maximo
      volatile byte gra, min, dec;
      volatile uint32_t min_1;
      volatile uint32_t min_2;
      volatile uint32_t diff;
      
      // Coordenada 1 a minutos con decimales
      gra = BCDtoHEX(*coord_1++);   // Grados de Coord_1 a Hex
      min = BCDtoHEX(*coord_1++);		// Minutos de Coord_1 a Hex
      dec = BCDtoHEX(*coord_1++);		// Decimaas de Coord_1 a Hex
      min_1 = gra * 60;             // Grados a Minutos
      min_1 += min;                 // Sumo Minutos
      min_1 *= 100;                 // x100 para luego agregar decimales
      min_1 += dec;                 // Agrego Decimales
      
      // Coordenada 2 a minutos con decimales
      gra = BCDtoHEX(*coord_2++);   // Grados de Coord_2 a Hex
      min = BCDtoHEX(*coord_2++);		// Minutos de Coord_2 a Hex
      dec = BCDtoHEX(*coord_2++);		// Decimaas de Coord_2 a Hex
      min_2 = gra * 60;             // Grados a Minutos
      min_2 += min;                 // Sumo Minutos
      min_2 *= 100;                 // x100 para luego agregar decimales
      min_2 += dec;                 // Agrego Decimales
      
      // Hasta aca tengo las 2 coordenadas en minutos con decimales y 
      // ademas tengo el puntero de cada coordenada apuntando al polo
      // de la misma

      if (*coord_1 == *coord_2){
        // Estan en el mismo POLO => La diferencia entre ambas es la resta
        // del mayor y el menor
        if (min_1 > min_2){
          diff = min_1 - min_2;         // Resto Mayor - Menor
        }else{
          diff = min_2 - min_1;         // Resto Mayor - Menor
        }
        
      }else{
        // Estan en distinto POLO => La diferencia entre ambos es la suma
        // de coordenadas
        diff = min_1 + min_2;           // Sumo coordenadas
      }
      
      // Trabajo internamente con dwords, pero debo devolver un word
      if (diff > 0xFFFF){
        diff = 0xFFFF;
      }
      
      return((uint16_t)diff);
    }


  /* CALCULAR DIFERENCIA EN GRADOS, MINUTOS y DECIMALES ENTRE POSICION y REFERENCIA */
  /**********************************************************************************/
    uint16_t REFDiff_GraMinDec (byte* coord, byte latlng, tGeoPOINT POS_REF){
      // EDIT 16/01/13
      //  Debo verificar que no me valla mas alla de la referencia, de modo que la diferencia
      // sea negativa, porque sino no puedo comprimir
      uint32_t big_ref, big_coord;
      uint16_t diff;
      
      big_coord = convert_toLongCoord(coord);
      
      #ifdef NO_COMPRIMIR_POSICION
        // Como no esta habilitada la compresion, la diferencia es 0xFFFF
        diff = 0xFFFF;
        
      #else
        if (latlng == 0){
          // LATITUD
          big_ref = convert_toLongCoord(POS_REF.lat);
          if (big_ref <= big_coord){
            diff = coordDiff_GraMinDec(coord, POS_REF.lat);
          }else{
            // Como esta fuera del rango de compresion, no puedo hacer la diferencia
            // y devuelvo fuera de rango
            diff = 0xFFFF;
          }
        
        }else{
          // LONGITUD
          big_ref = convert_toLongCoord(POS_REF.lon);
          if (big_ref <= big_coord){
            diff = coordDiff_GraMinDec(coord, POS_REF.lon);
          }else{
            // Como esta fuera del rango de compresion, no puedo hacer la diferencia
            // y devuelvo fuera de rango
            diff = 0xFFFF;
          }
        }
      #endif
      
      return(diff);
    }


  /* CALCULAR DIFERENCIA EN GRADOS, MINUTOS y DECIMALES en VALOR ABSOLUTO */
  /************************************************************************/
    uint16_t ABSDiff_GraMinDec (byte* coord1, byte* coord2){
      // Como la funcion de diferencia (coordDiff_GraMinDec) hace siempre: coord1 - coord2
      // La idea de usar esa misma funcion, pero cambiando el valor de coord1 y coord2, para
      // que la resta sea siempre positiva; es decir en valor absoluto
      uint32_t long_coord1, long_coord2; // "long" de largo no de longitud
      uint16_t diff;
      
      long_coord1 = convert_toLongCoord(coord1);
      long_coord2 = convert_toLongCoord(coord2);
      
      if (long_coord1 >= long_coord2){
        diff = coordDiff_GraMinDec(coord1, coord2);
      }else{
        diff = coordDiff_GraMinDec(coord2, coord1);
      }
      
      return (diff);
    }


  /* DIFERENCIA DE COORDENADAS EN GGº MM.DD' */
  /*******************************************/
/*
    void coordDiff_GGMMDD (byte* coord_1, byte* coord_2, byte* diff){
      // Las coordenadas tienen el siguiente formato:
      //
      //  GRA | MIN | DEC |
      //   |		 |		 |	
      //   |		 |		 +-------------> Decimas de Minutos
      //   |		 +-------------------> Minutos
      //   +-------------------------> Grados
      //
      // Tanto GRA,MIN como DEC estan en BCD
      // 
      // No tenemos en cuenta el POLO, y la diferencia es SIEMPRE en valor absoluto
      //
      // De todos modos, el primer paso es convertir las coordenadas
      // a MINUTOS con DECIMALES
      //
      // Lo que hace es devolver la diferencia en formato GGº MM.DD'
      byte gra1, min1, dec1;
      byte gra2, min2, dec2;
      byte graDIFF, minDIFF, decDIFF;
      uint32_t long_coord1, long_coord2; // "long" de largo no de longitud

      // Obtengo las coordenadas en formato largo
      long_coord1 = convert_toLongCoord(coord_1);
      long_coord2 = convert_toLongCoord(coord_2);

      // Para hacer que SIEMPRE la resta sea (coord1 - coord2), pero a su vez que la
      // diferencia sea en VALOR ABSOULTO, cargo las variales _1 y _2 de acuerdo a
      // cual sea la mayor
      if (long_coord1 >= long_coord2){
        gra1 = BCDtoHEX(*coord_1++);    // Grados de Coord_1 a Hex
        min1 = BCDtoHEX(*coord_1++);		// Minutos de Coord_1 a Hex
        dec1 = BCDtoHEX(*coord_1++);		// Decimaas de Coord_1 a Hex
        
        gra2 = BCDtoHEX(*coord_2++);    // Grados de Coord_2 a Hex
        min2 = BCDtoHEX(*coord_2++);		// Minutos de Coord_2 a Hex
        dec2 = BCDtoHEX(*coord_2++);		// Decimaas de Coord_2 a Hex

      }else{
        gra2 = BCDtoHEX(*coord_1++);    // Grados de Coord_1 a Hex
        min2 = BCDtoHEX(*coord_1++);		// Minutos de Coord_1 a Hex
        dec2 = BCDtoHEX(*coord_1++);		// Decimaas de Coord_1 a Hex
        
        gra1 = BCDtoHEX(*coord_2++);    // Grados de Coord_2 a Hex
        min1 = BCDtoHEX(*coord_2++);		// Minutos de Coord_2 a Hex
        dec1 = BCDtoHEX(*coord_2++);		// Decimaas de Coord_2 a Hex
      }

      // Diferencia de GRADOS
      graDIFF = gra1 - gra2;            

      // Diferencia de MINUTOS
      if (min1 >= min2){
        minDIFF = min1 - min2;
      }else{
        graDIFF--;
        minDIFF = (min1 + 60) - min2;
      }

      // Diferencia de DECIMAS
      if (dec1 >= dec2){
        decDIFF = dec1 - dec2;
      }else{
        minDIFF--;
        decDIFF = (dec1 + 100) - dec2;
      }

      // Convierto al diferencia en GGº MM.DD'
      gra1 = HEXtoBCD(graDIFF);
      min1 = HEXtoBCD(minDIFF);
      dec1 = HEXtoBCD(decDIFF);
      
      *diff++ = gra1;
      *diff++ = min1;
      *diff++ = dec1;
    }
*/    


#ifndef VISOR_USES_SATELITECH
#ifndef VISOR_USES_SKYPATROL  
  /* CALCULAR EL CUADRANTE AL QUE PERTENCE EL ANGULO FORMADO POR 2 POSICIONES */
  /****************************************************************************/
    byte calc_CuadranteAngulo (tGeoPOINT* pos1, tGeoPOINT* pos2){
      /* Como trabajamos en el 3er cuadrante de la tierra, los ejes son:
      
      
                   |
                   |
              2    |		1
                   |
         LON       |
         <---------*---------
      						 |
                   |
                   |
              3    |    4
                   |
                   V LAT
      */
      uint32_t lng_coord1, lng_coord2;
      uint32_t lat_coord1, lat_coord2;
      byte Q;
      
      lng_coord1 = pos1->lon[0];
      lng_coord1 <<= 8;
      lng_coord1 |= pos1->lon[1];
      lng_coord1 <<= 8;
      lng_coord1 |= pos1->lon[2];
      
      lng_coord2 = pos2->lon[0];
      lng_coord2 <<= 8;
      lng_coord2 |= pos2->lon[1];
      lng_coord2 <<= 8;
      lng_coord2 |= pos2->lon[2];
      
      lat_coord1 = pos1->lat[0];
      lat_coord1 <<= 8;
      lat_coord1 |= pos1->lat[1];
      lat_coord1 <<= 8;
      lat_coord1 |= pos1->lat[2];
      
      lat_coord2 = pos2->lat[0];
      lat_coord2 <<= 8;
      lat_coord2 |= pos2->lat[1];
      lat_coord2 <<= 8;
      lat_coord2 |= pos2->lat[2];
      
      if (lng_coord2 > lng_coord1){
        // 2 o 3
        if (lat_coord2 > lat_coord1){
          // 3
          Q = 3;
        }else{
          // 2
          Q = 2;
        }
      }else{
        // 1 o 4
        if (lat_coord2 > lat_coord1){
          // 4
          Q = 4;
        }else{
          // 1
          Q = 1;
        }
      }
      
      return(Q);
      
    }    

  
  /* CALCULO DE DISTANCIA CUALITATIVO */
  /************************************/
    uint32_t calculo_distancia_cualitativo (tGeoPOINT P1, tGeoPOINT P2){
      // Esta rutina NO CALCULA LA DISTANCIA, sino que devuelve un valor para
      // tener una idea cualitativa de la distancia a la que se encuentra.
      // La distancia segun pitagoras se calcula como:
      //
      //    dsit ^2 = x^2 + y^2
      //
      // Lo que hacemos es calcular X e Y.
      //
      // Luego como el cuadrado de la suma es mayor a la suma de los cuadrados
      // 
      //    (x + y) ^2 > x^2 + y^2
      //    (x + y) ^2 > dist^2
      // =>
      //     x + y > dist
      //
      // Por lo tanto, para ahorrarme el calculo de potencias y raices, trabajo
      // con la suma de X e Y.
      // De este modo obtengo una DISTANCIA CUALITATIVA, y luego como lo uso para
      // determinar en que zona se encuentra el equipo, la zona en cuestion sera
      // aquella cuya distancia cualitativa sea menor
      //
      // La distancia es en minutos con 2 decimales. Tener en cuenta que no es un valor
      // exacto, sino cualitativo
      uint32_t delta_lat, delta_lon;
      
      uint32_t dist_coord;
      
      delta_lat = ABSDiff_GraMinDec(P1.lat, P2.lat);		// Diferencia en LATITUD
      delta_lon = ABSDiff_GraMinDec(P1.lon, P2.lon);		// Diferencia en LONGITUD
      
      if ((delta_lat > 0xFFF0) || (delta_lon > 0xFFF0)){
        // Si alguna de las 2 diferencia es mayor a ese valor, al hacer el cuadrado
        // me va a dar un numero GIGANTE!!
        // NOTA: Ahora no hago el cuadrado, sino la suma, pero igual el valor sería
        // gigante
        // Por eso lo tomo como error, y devuelvo 0xFFFFFFFF como valor de distancia
        dist_coord = 0xFFFFFFFF;
        
      }else{
        dist_coord = delta_lat + delta_lon;
      }
      
      return(dist_coord);
    }
#endif
#endif

  /* CALCULO DE DISTANCIA APROXIMADO */
  /***********************************/
    uint32_t calculo_distancia_aproximado (tGeoPOINT P1, tGeoPOINT P2){
      // Esta rutina CALCULA LA DISTANCIA, y es aproximada, ya que utiliza un metodo
      // de aproximacion para el calculo de la raiz cuadrada.
      //
      //  1- Extraer diferencia de coordenadas en LATITUD y LONGITUD (minutos con 2 decimales)
      //  2- Convertir diferencia de coordenadas a metros
      //  3- Quito los decimales (centimetros) para no complicarla al elevar al cuadrado
      //  4- Calcular el cuadrado de cada una
      //  5- Sumarlas
      //  6- Calcular la Raiz Cuadrada Aproximada
      //
      // 4,5, 6 => PITAGORAS
      //
      // EL RESULTADO ES EN METROS SIN DECIMALES
      uint32_t delta_lat, delta_lon;
      uint32_t dist;
      
      delta_lat = ABSDiff_GraMinDec(P1.lat, P2.lat);		// Diferencia en LATITUD
      delta_lon = ABSDiff_GraMinDec(P1.lon, P2.lon);		// Diferencia en LONGITUD
      
      // La diferencia de coordenadas es en minutos con 2 decimales.
      // Luego, la convierto a metros.
      // Tener en cuenta que la equivalencia va a estar multiplicada x100, para no perder
      // decimales.
      delta_lat *= LatEqMts;            // Convierto diferencia de LATITUD a Metros
      delta_lat /= 100;                 // Quito decimales inservibles
      delta_lon *= LngEqMts;          	// Convierto diferencia de LONGITUD a Metros (depende de la localidad)
      delta_lon /= 100;                 // Quito decimales inservibles
      
      // Ahora ya tengo la distancia de cada coordenada en METROS y con 2 decimales.
      // Como son decimales de METROS, tampoco me importan, asi que los quito, xq sino
      // al aplicar pitagoras, se me va a hacer un lio terrible con los decimales
      delta_lat /= 100;                 // Redondeo a METROS SIN DECIMALES
      delta_lon /= 100;                 // Redondeo a METROS SIN DECIMALES
      
      dist = Pitagoras((uint16_t)delta_lat, (uint16_t)delta_lon); // Aplico el Teorema de Pitagoras para calcular la distancia
      
      return(dist);
    }    


/*********************************************************************************************/
/* CONVERSION DE FORMATO */
/*************************/

  /* ASCII to HEX */
  /****************/
    byte ASCIItoHEX (byte ascii){
      return(ascii - 0x30);
    }


  /* BCD to HEX - 1 byte */
  /***********************/
    byte BCDtoHEX (byte num_bcd){
      // Convierte de BCD a HEX, un numero menor a 99
      byte high, low;
      
      high=num_bcd>>4;
      low=num_bcd & 0x0F;
      high=high*10;
      
      return(high+low);
    }
    
    
  /* BCD to HEX - 4 bytes */
  /************************/
    uint32_t dword_BCDtoHEX (uint32_t num_bcd){
      // Convierte de BCD a HEX, un numero menor a 99
      byte _4bits, i, len, exit;
      byte* ptr;
      uint32_t hex, multiplicador;
      byte TO_F;
      
      len = 8;                          // Asumo longitud maxima (uint32_t)
      ptr = (byte*) &num_bcd;           // Inicializo puntero en MSB
      exit = 0;                         // Reseteo Bandera
      
      TO_F = 0;                         // Reseteo Bandera de TimeOut
      dispararTO_lazo();                // Disparo Time Out de Lazo
      while((exit == 0) && (len > 0) && !TO_F){
        TO_F = chkTO_lazo_F();          // Chequeo bandera de time out de lazo 
        if ((*ptr & 0xF0) != 0){
          exit = 1;                     // No es CERO => Comienza en nº => Salgo
        }else{
          len--;                        // Decremento longitud xq es un CERO a la izquierda
        }
        
        if (!exit){
          if ((*ptr & 0x0F) != 0){
            exit = 1;                     // No es CERO => Comienza en nº => Salgo
          }else{
            len--;                        // Decremento longitud xq es un CERO a la izquierda
          }
        }
        
        ptr++;                          // Paso a siguiente byte
      }
      detenerTO_lazo();                 // Detengo Time Out de Lazo
      
      if (!TO_F){
        ptr = (byte*) (&num_bcd) + 3;     // Puntero apuntando a LSB
        hex = 0;                          // Reseteo variables
        for (i = 0; i < len; i++){
          if ((i % 2) != 0){
            _4bits = *ptr & 0xF0;         // Posiciones IMPARES => extraigo parte ALTA del byte
            _4bits >>= 4;                 // Lo llevo a parte baja
            ptr--;                        // Como ya extraje la parte alta RETROCEDO puntero para luego extraer parte baja del siguiente
            
          }else{
            _4bits = *ptr & 0x0F;         // Posiciones PARES => extraigo parte BAJA del byte
          }
          
          multiplicador = potenciaDWORD(10, i);  // Multiplicador segun posicion 
          
          hex += _4bits * multiplicador;
        }
      }else{
        hex = 0;
      }
      
      return(hex);
    }


  /* HEX to BCD */
  /**************/    
    byte HEXtoBCD (byte num_dec){
      // Convierte un numero de 1byte menor o igual a 99 a BCD
      // Lo hace de la siguiente manera. Yo quiero pasar un numero YZ en decimal
      // a BCD. O sea que de YZ decimal pasa a 0xYZ BCD. Notar que el numero en BCD
      // es como si fuera el decimal en hexa. No que es la representacion hexa de 
      // ese decimal. Es decir el numero decimal 64 en hexa es 0x40, pero en BCD es 0x64.
      // El algoritmo de trabajo es el sgte: Tomo en numero de 2 cifras y separao sus
      // cifras en high y low. Si el nro en cuestion es YZ, entonces high=Y=0x0Y y low=0x0Z
      // Luego desplazo high cuatro lugares a la izquierda y me queda high=0xY0, luego lo
      // sumo logicamente con low y obtengo 0xYZ.
      byte high,low,aux;
        
      high = num_dec/10;
      aux	 = high*10;
      
      if (aux > num_dec){		// Si redondeo para arriba, le resto 1
      	high = high - 1;
      }
      
      low	= num_dec - (high * 10);
      
      high = high << 4;
      
      return (high|low);
    }    


  /* HEX to BCD - WORD */
  /*********************/    
    uint16_t uint16_t_HEXtoBCD (uint16_t num_dec){
      // Convierte un numero de 1 word menor o igual a 9999 a BCD
      // Lo hace de la siguiente manera. Yo quiero pasar un numero YZ en decimal
      // a BCD. O sea que de YZ decimal pasa a 0xYZ BCD. Notar que el numero en BCD
      // es como si fuera el decimal en hexa. No que es la representacion hexa de 
      // ese decimal. Es decir el numero decimal 64 en hexa es 0x40, pero en BCD es 0x64.
      // El algoritmo de trabajo es el sgte: Tomo en numero de 2 cifras y separao sus
      // cifras en high y low. Si el nro en cuestion es YZ, entonces high=Y=0x0Y y low=0x0Z
      // Luego desplazo high cuatro lugares a la izquierda y me queda high=0xY0, luego lo
      // sumo logicamente con low y obtengo 0xYZ.
      uint16_t high,low,aux, result;
      
      high = num_dec/1000;
      aux	 = high*1000;
      
      if (aux > num_dec){		// Si redondeo para arriba, le resto 1
      	high = high - 1;
      }
      
      low	= num_dec - (high * 1000);
      
      result = high << 12;
      
      num_dec = low;
      
      //------------------------
      
      high = num_dec/100;
      aux	 = high*100;
      
      if (aux > num_dec){		// Si redondeo para arriba, le resto 1
      	high = high - 1;
      }
      
      low	= num_dec - (high * 100);
      
      result |= high << 8; 
      
      num_dec = low;
      
      //------------------------
      
      high = num_dec/10;
      aux	 = high*10;
      
      if (aux > num_dec){		// Si redondeo para arriba, le resto 1
      	high = high - 1;
      }
      
      low	= num_dec - (high * 10);
      
      result |= high << 4; 
      
      return (result|low);
    }    


    /* HEX to BCD - DWORD */
    /*********************/
    //convierte un int de 32bits a 10 digitos
    uint32_t uint32_t_HEXtoBCD( uint32_t num)
     {
    	uint32_t temp,temp1,temp2;
        signed char index ;
        uint8_t datarray[10];
        uint8_t i;


        for ( index=9; num>0; index-- ) {
            temp = num / 10 ;
            datarray[index] = num-10*temp ;
            num = temp ;

            temp1 = ((uint32_t)datarray[index])<<(4*(9-index));
            if(index==9){
   	            temp2 = ((uint32_t)datarray[index]);
            }else{
   	            temp2 = temp2 | temp1;
            }
        }
        //completa con ceros el arreglo
        //for ( ; index>=0; index-- ) datarray[index] = 0 ;

        return(temp2);

}


    /* HEX to BCD - DWORD */
    /*********************/
    //convierte un int de 32bits a 10 digitos
    void uint32_t_HEX_a_BCD( uint32_t num, uint8_t* ptrDATA){
    	uint32_t temp;
        signed char index ;
        uint8_t i;


        for ( index=9; num>0; index-- ) {
            temp = num / 10 ;
            *(ptrDATA+index)= num-10*temp ;
            num = temp ;

        }
        //completa con ceros el arreglo
       //for ( ; index>=0; index-- ) datarray[index] = 0 ;
        for ( ; index>=0; index-- ) *(ptrDATA+index)= 0 ;

}

  /* BCD to ASCII */
  /****************/
    uint16_t BCDtoASCII (byte num_BCD){
      // Convierte 1 byte BCD en 2bytes ASCII
      byte high, low;
      uint16_t ASCII;
      
      high = num_BCD & 0xF0;
      high >>= 4;
      low = num_BCD & 0x0F;
      
      ASCII = ((high + 0x30) << 8) | (low + 0x30);
      
      return(ASCII);
    }


/*********************************************************************************************/
/* CONVERSIONES NUMERICAS -> STRING */
/************************************/

  /* CALCULAR CANTIDAD DE DIGITOS DE NUMERO */
  /******************************************/
    byte calcCantDigitos (uint32_t num, byte base){
      // Rutina que devuelve la cantidad de digitos de un numero.
      // Generalmente usada para agregar "ceros" en la conversion to string
      byte digitos;
      
      digitos = 0;
      while (num > 0){
        num /= base;
        digitos++;
      }
      
      if (digitos == 0){
        digitos++;        // Si es CERO, es un digito, el cero
      }
      
      // EDIT 13/11/12
      if (num == base){
        digitos++;        // Si el numero es la base => agrego un digito xq las bases son de 2 digitos
      }
      
      return(digitos);
    }
    

  /* CONVERTIR A STRING */
  /**********************/
    void convert_to_string(byte* buf, uint32_t i, byte cant_ceros, int base){
      // Convierte un numero a string, con tantos ceros como se le indique, y en
      // la base numerica que se pase como argumento
      byte reverse[MAX_LENGTH+1];	    // plus one for the null
      byte* s;
      byte menor_a_base= i < base;
      
      reverse[MAX_LENGTH]=0;					// null, fin de cadena
      s = reverse+MAX_LENGTH;
      /*
          reverse ->  ------
                     |  0   |
                     |------|
                     |      |
                     |------|
                     |      | <-- reverse+MAX_LENGTH 
                      ------ 
      */
      do {
          *--s = "0123456789ABCDEFghijklmnopqrstuvwxyz"[i % base];
          /*
          Lo de arriba se entiende asi:  stringTab[] = "0123456789ABCDEFghijklmnopqrstuvwxyz";
                                         *s = stringTab[i % base];
                                          --s; 
          Ejemplos:
          
          1-  i = 0 
              base = 16
              resto = i%base = 0x00/16 = 0 -> stringTab[i%base] = stringTab[0] = "0"
                                                                                      
          */
          
          i /= base;
          
      } while (i);
      
      
      // cant_ceros:
      //    0x00: Agregar cero, sólo si es menor a la base
      //    0xFF: No agregar ningun cero
      //    0xAB: Agregar AB ceros
      if (cant_ceros == 0){
        if (menor_a_base)  *--s = '0';// Agrego "0" si es menor a base
      }else if (cant_ceros != 0xFF){
        while (cant_ceros != 0){
          *--s = '0';                 // Agrego "0"
          cant_ceros--;               // Decremento Contador
        }
      }
      
      string_copy(buf, s);
    }

  
  /* CONVERTIR A STRING CON DECIMALES */
  /************************************/
    void convert_to_string_with_decimals (byte* buf, uint32_t i, byte cant_ceros, byte decimales, int base){
	    // Convierte un numero a string, con decimales
      byte reverse[MAX_LENGTH+1];      // plus one for the null
      byte aux_buffer[MAX_LENGTH+1];   // plus one for the null
      byte* s;
      byte* s_aux;
      byte menor_10= i < 10;
      uint16_t cant_digitos;
      uint16_t cant_digitos_old;
      byte TO_F;
      
      reverse[MAX_LENGTH]=0;					    // null, fin de cadena
      s = reverse+MAX_LENGTH;
      
      do {
        *--s = "0123456789ABCDEFghijklmnopqrstuvwxyz"[i % base];
        i /= base;
      } while (i);
      
      // decimales
      if (decimales != 0){
        s_aux = aux_buffer;
        
        cant_digitos = string_length(s);
        
        if (cant_digitos <= decimales){
          *s_aux++ = '0';                 // Agrego cero a la izquierda
          *s_aux++ = '.';                 // Agrego Punto Decimal
          
          // EDIT 13/11/12
          //  Como ya estoy agregando un cero despues de la coma => tengo que sacar un cero
          // si le digo cuantos ceros quiero
          if ((cant_ceros != 0xFF) && (cant_ceros != 0x00)){
            cant_ceros--;
          }
          
          cant_digitos_old = cant_digitos;
          while (cant_digitos < decimales){
            *s_aux++ = '0';
            cant_digitos++;
          }
          cant_digitos = cant_digitos_old;

          *s_aux = 0;                     // Agrego fin de cadena para poder concatenar
          string_concat(s_aux,s);								// Agrego todos los digitos
          
          for(i=0; i<cant_digitos; i++ ){
            s_aux++;                      // Necesito avanzar el puntero, sino copia mal los datos
          }
        
        }else{
          while (cant_digitos > decimales){
            *s_aux++ = *s++;							// Copio digitos a la izquierda
            cant_digitos--;								 
          }
          
          *s_aux++ = '.';									// Agrego punto decimal
          
          while (cant_digitos > 0){
            *s_aux++ = *s++;              // Copio digitos a la derecha
            cant_digitos--;
          }
        }
        
        // Reseteo los puntero "s" y "s_aux", para que "s" apunte al buffer que tiene todos los decimales
        reverse[MAX_LENGTH]=0;
        s = reverse+MAX_LENGTH;
        
        while(s_aux != aux_buffer){
          *--s = *--s_aux;
        }
      }
      
                
      // cant_ceros:
      //    0x00: Agregar cero, sólo si es menor a 10
      //    0xFF: No agregar ningun cero
      //    0xAB: Agregar AB ceros
      if (cant_ceros == 0){
        if (menor_10)  *--s = '0';    // Agrego "0" si es menor a 10
      }else if (cant_ceros != 0xFF){
        while (cant_ceros != 0){
          *--s = '0';                 // Agrego "0"
          cant_ceros--;               // Decremento Contador
        }
      }
      
      string_copy(buf, s);
    }	    

    /* DETERMINACION DE EQUIVALENCIA DE LONGITUD */
      /*********************************************/
    /*
    void GENERIC_LOCATION_determineEqLng (byte gradosLAT){
          // Esta rutina se llama con cada posición de GPS, pero una vez tomado, no se
          // modifica a menos que:
          //  - sea menor al menor valor
          //  - sea mayor al mayor valor
          //
          // EDIT 01/08/13
          // Cambia el valor de equivalencia con cada valor GPS recibido
          //if ((LngEqMts < EqLng_85lat) || (LngEqMts > EqLng_00lat)){
            LngEqMts = GENERIC_LOCATION_TablaEqLng[(BCDtoHEX(gradosLAT))/5];
          //}
    }
    */

  /* CONVERTIR DATE A STRING */
  /***************************/
    void date_to_string(byte* buffer, tDATE date){
      // Convierte un tipo tDATE a string y lo devuelve en el buffer que se pasa
      // como argumento.
      //
      // SALIDA: DD/MM/AAAA HH:MM:SS
      byte aux[5];
      
      convert_to_string(buffer, date.fecha[0], 0, base_BCD);  // Convierto Dia
      string_FAR_concat(buffer, (byte*) _BARRA);
      convert_to_string(aux, date.fecha[1], 0, base_BCD);     // Convierto Mes
      string_FAR_concat(buffer, aux);                         // Agrego Mes
      string_FAR_concat(buffer, (byte*) _BARRA);
      convert_to_string(aux, date.fecha[2], 0, base_BCD);     // Convierto Año
      string_FAR_concat(buffer, (byte*)_20);                         // Agrego 20xx
      string_FAR_concat(buffer, aux);                         // Agrego Año
      
      string_FAR_concat(buffer, (byte*) _SPACE);
      
      convert_to_string(aux, date.hora[0], 0, base_BCD);      // Convierto Hora
      string_FAR_concat(buffer, aux);                         // Agrego Hora
      string_FAR_concat(buffer, (byte*) _2PUNTOS);
      convert_to_string(aux, date.hora[1], 0, base_BCD);      // Convierto Min
      string_FAR_concat(buffer, aux);                         // Agrego Min
      string_FAR_concat(buffer, (byte*) _2PUNTOS);
      convert_to_string(aux, date.hora[2], 0, base_BCD);      // Convierto Seg
      string_FAR_concat(buffer, aux);                         // Agrego Seg
    }


  /* CONVERTIR SHORT-DATE A STRING */
  /*********************************/
    void shortDate_to_string(byte* buffer, tDATE date){
      // Convierte un tipo tDATE a string y lo devuelve en el buffer que se pasa
      // como argumento.
      // Con tipo "short" nos referimos a que solo es DIA/MES y HH:MM
      //
      // SALIDA: DD/MM HH:MM
      byte aux[5];
      
      convert_to_string(buffer, date.fecha[0], 0, base_BCD);  // Convierto Dia
      string_FAR_concat(buffer, (byte*) _BARRA);
      convert_to_string(aux, date.fecha[1], 0, base_BCD);     // Convierto Mes
      string_FAR_concat(buffer, aux);                         // Agrego Mes
      
      string_FAR_concat(buffer, (byte*) _SPACE);
      
      convert_to_string(aux, date.hora[0], 0, base_BCD);      // Convierto Hora
      string_FAR_concat(buffer, aux);                         // Agrego Hora
      string_FAR_concat(buffer, (byte*) _2PUNTOS);
      convert_to_string(aux, date.hora[1], 0, base_BCD);      // Convierto Min
      string_FAR_concat(buffer, aux);                         // Agrego Min
    }

#ifdef VISOR_PRINT_TICKET_FULL
  /* CONVERTIR DDMMAA A STRING */
  /*****************************/
    void DDMMAA_to_string(byte* buffer, byte* fecha){
      // Convierte DD MM AA a string y lo devuelve en el buffer que se pasa
      // como argumento.
      byte aux[5];
      
      convert_to_string(buffer, fecha[0], 0, base_BCD);  // Convierto Dia
      string_FAR_concat(buffer, (byte*) _BARRA);
      convert_to_string(aux, fecha[1], 0, base_BCD);     // Convierto Mes
      string_FAR_concat(buffer, aux);                         // Agrego Mes
      string_FAR_concat(buffer, (byte*) _BARRA);
      convert_to_string(aux, fecha[2], 0, base_BCD);     // Convierto Año
      string_FAR_concat(buffer, (byte*) _20);                         // Agrego 20xx
      string_FAR_concat(buffer, aux);                         // Agrego Año
    }        
#endif

#ifdef VISOR_PRINT_TICKET_FULL
  /* CONVERTIR DDMM A STRING */
  /***************************/
    void DDMM_to_string(byte* buffer, byte* fecha){
      // Convierte DD MM a string y lo devuelve en el buffer que se pasa
      // como argumento.
      byte aux[5];
      
      convert_to_string(buffer, fecha[0], 0, base_BCD);  // Convierto Dia
      string_FAR_concat(buffer, (byte*) _BARRA);
      convert_to_string(aux, fecha[1], 0, base_BCD);     // Convierto Mes
      string_FAR_concat(buffer, aux);                    // Agrego Mes
    }    
#endif


#ifdef VISOR_PRINT_TICKET_FULL
  /* CONVERTIR HHMM A STRING */
  /***************************/
    void HHMM_to_string(byte* buffer, byte* hora){
      // Convierte HH MM a string y lo devuelve en el buffer que se pasa
      // como argumento.
      byte aux[5];
      
      convert_to_string(buffer, hora[0], 0, base_BCD);  // Convierto Hora
      string_FAR_concat(buffer, (byte*) _2PUNTOS);
      convert_to_string(aux, hora[1], 0, base_BCD);     // Convierto Minutos
      string_FAR_concat(buffer, aux);                   // Agrego Minutos
    }
#endif

#ifdef VISOR_PLUS
#if defined(VISOR_PRINT_TICKET_FULL) && defined(_PREMIUM_)
  /* CONVERTIR NUMERO+DECIMALES A PALABRAS */
  /*****************************************/
    void numberDecimals_to_speech (byte* buffer, byte* num, byte* middleText){
      // Rutina que convierte un numero+decimales en su expresion linguistica, como si estuviera
      // escrito con palabras. Se le pasa como argumento el buffer de salida y numero
      // en ASCII
      byte tmpBuffer[10];
      byte* ptr;
      byte* ptr_entero;
      byte* ptr_decimales;
      byte speech[100];
      uint32_t valueEntero;
      uint32_t valueDecimal;
      
      string_copy(tmpBuffer, num);
      
      ptr = tmpBuffer;
      ptr_entero = ptr;                   // Seteo puntero a entero
      
      while (*ptr != 0 && *ptr != '.'){
        ptr++;
      }
      
      ptr_decimales = NULL;               // Asumo que no tiene decimales
      if (*ptr == '.'){
        *ptr = 0x00;                      // Reemplazo punto por fin de cadena para separar entero de decimales
        ptr++;
        ptr_decimales = ptr;              // Seteo puntero a decimales
      }

      // Calculo el valor de la parte ENTERA
      ptr = ptr_entero;
      valueEntero = 0;
      while (*ptr != 0){
        valueEntero += ((*ptr++) - 0x30);
        valueEntero *= 10;
      }
      
      // Calculo el valor de la parte DECIMAL
      ptr = ptr_decimales;
      valueDecimal = 0;
      while (*ptr != 0){
        valueDecimal += ((*ptr++) - 0x30);
        valueDecimal *= 10;
      }
      
      
      // Convierto a SPEECH
      *buffer = 0;
      if (valueEntero != 0){
        number_to_speech(speech, ptr_entero);
        string_concat(buffer, speech);
      }
      
      if (ptr_decimales != NULL){
        if (middleText != NULL && valueEntero != 0){
          string_concat(buffer, middleText);
        }
        
        if (valueDecimal != 0){
          number_to_speech(speech, ptr_decimales);
          string_concat(buffer, speech);
        }else{
          string_concat(buffer, "cero ");
        }
      }
      
    }


  /* CONVERTIR NUMERO A PALABRAS */
  /*******************************/
    void number_to_speech (byte* buffer, byte* num){
      // Solo numeros enteros
      uint16_t N;
      byte decenas;
      byte error;
      
      error = 0;
      N = string_length(num);
      
      *buffer = 0;                  // Agrego fin de cadena a inicio de buffer para agregar desde ahi
      decenas = 0;                  // Reseteo
      
      if (N > 4){
        error = 1;
      
      }else{
        while (N > 0){
          if (N == 4){
            // MILES
            string_FAR_concat(buffer, Tabla_Miles[*num - 0x30]);
            string_FAR_concat(buffer, _SPACE);
            
          }else if (N == 3){
            // CENTENAS
            if ((*num - 0x30 == 1) && (*(num+1)- 0x30 == 0) && (*(num+2)- 0x30 == 0)){
              // 100
              string_FAR_concat(buffer, _cien);
              string_FAR_concat(buffer, _SPACE);
            
            }else{
              // 101-999
              string_FAR_concat(buffer, Tabla_Centenas[*num - 0x30]);
              string_FAR_concat(buffer, _SPACE);
            }
            
          }else if (N == 2){
            // DECENAS
            if (*num - 0x30 >= 2){
              if ((*num - 0x30 == 2) && (*(num+1)- 0x30 == 0)){
                // 20
                string_FAR_concat(buffer, _veinte);
                string_FAR_concat(buffer, _SPACE);
                
              }else{
                // 21-99
                string_FAR_concat(buffer, Tabla_Decenas[*num - 0x30]);
                if (*num != '2'){
                  // no agrego espacio a los "veinti"
                  string_FAR_concat(buffer, _SPACE);
                }
              }
            
            }else if (*num - 0x30 == 1){
              // 10-19
              string_FAR_concat(buffer, Tabla_diez[*(num+1) - 0x30]);  
              string_FAR_concat(buffer, _SPACE);
              N--;                          // Ya hice uso de las unidades
            }
            
            // Guardo decenas para saber si tengo que agregar el Y
            decenas = *num - 0x30;
            
          }else if (N == 1){
            // UNIDADES
            if (decenas > 0 && *num != '0' && decenas != 2){
              string_FAR_concat(buffer, _y);  
              string_FAR_concat(buffer, _SPACE);  
            }
            string_FAR_concat(buffer, Tabla_Unidades[*num - 0x30]);
            
            if (*num != '0'){
              // Si termina es "0" no le agrego el espacio, xq viene del
              // espacio del digito anterior
              string_FAR_concat(buffer, _SPACE);
            }
          }
          
          if (N > 0){
            N--;
          }

          num++;
        }
      }
      
      if (error){
        // Mas de 9999 no puedo convertir
        string_FAR_copy(buffer, _ERROR);
      }
    }    
#endif
#endif  
        


/*********************************************************************************************/
/* CALCULOS DE FECHA Y HORA */
/****************************/

  /* CORRECCION HORARIA */
  /**********************/
    void correccionHoraria (byte* fecha, byte* hora, byte uso, byte signo){
      byte hora_hex;
      byte dia_hex, mes_hex, anio_hex;
      byte day_up, day_down;
      byte mes_up, mes_down;
      byte anio_up, anio_down;
      
      day_up = 0;                             // Asumo que no paso al dia siguiente
      day_down = 0;                           // Asumo que no paso al dia anterior
      mes_up = 0;                             // Asumo que no paso al mes siguiente
      mes_down = 0;											      // Asumo que no paso al mes anterior
      anio_up = 0;                            // Asumo que no paso al año siguiente
      anio_down = 0;										      // Asumo que no paso al año anterior
      
      hora_hex = BCDtoHEX(*hora);
      
      // HORA
      while (uso != 0){
        uso--;                                // Voy decrementando uso
        
        if (signo == negativo){
          // Signo Negativo => Resto
          if(hora_hex > 0){
            hora_hex--;                       // Decremento la hora
          }else{
            hora_hex = 23;                    // Doy vuelta la hora
            day_down = 1;                     // Debo pasar al dia anterior
          }
        
        }else{
          // Signo Positivo => Sumo
          if(hora_hex < 23){
            hora_hex++;                       // Decremento la hora
          }else{
            hora_hex = 0;                     // Doy vuelta la hora
            day_up = 1;                       // Debo pasar al dia siguiente
          }
        }
      }
      
     
      // DIA y RETROCESO DE MES
      dia_hex = BCDtoHEX(*fecha);
      mes_hex = BCDtoHEX(*(fecha+1));
      anio_hex = BCDtoHEX(*(fecha+2));
      if (day_up){
        dia_hex++;                            // Avanzo el Dia
        if (dia_hex > Tabla_FinMeses[mes_hex]){
          // OJO QUE EL AÑO PUEDE SER BICIESTO Y EL ULTIMO DIA DE FEBRERO, EN VEZ
          // DE SER EL 28 ES EL 29
          if ((mes_hex == 2) && (chkBiciesto(anio_hex))){
            // Como es biciesto, febrero tiene 29 dias => No debo hacer nada, xq
            // ya avance el dia; y no debo avanzar el mes
          
          }else{
            // Como no estoy en febrero no me importa si es biciesto o no
            dia_hex = 1;                      // 1er dia del siguiente mes
            mes_up = 1;                       // Debo pasar al siguiente de Mes
          }
        }
        
      }else if(day_down){
        dia_hex--;                            // Retrocedo el Dia
        if (dia_hex == 0){
          if (mes_hex > 1){
            mes_hex--;                        // Paso al mes anterior
          
          }else{
            mes_hex = 12;                     // Paso al ultimo mes del año anterior
            anio_down = 1;                    // Debo pasar al año anterior
          }
          
          dia_hex = Tabla_FinMeses[mes_hex];  // Ultimo dia del mes anterior
          
          // OJO QUE EL AÑO PUEDE SER BICIESTO Y EL ULTIMO DIA DE FEBRERO, EN VEZ
          // DE SER EL 28 ES EL 29
          if ((mes_hex == 2) && (chkBiciesto(anio_hex))){
            dia_hex = 29;                     // El ultimo dia es el 29 de febrero
          }
          
        }
      }
      
      // AVANCE DE MES
      if (mes_up){
        mes_hex++;                            // Paso al siguiente mes
        if (mes_hex > 12){
          mes_hex = 1;                        // Paso al 1er mes del siguiente año
          anio_up = 1;                        // Debo pasar al siguiente año
        }
      }
      
      // AÑO
      if (anio_up){
        anio_hex++;
      }else if (anio_down){
        anio_hex--;
      }
      

      // Ya tengo TODOS los valores corregidos => Los guardo convertidos a BCD
      *hora = HEXtoBCD(hora_hex);             // Convierto y guardo hora corregida
      *fecha = HEXtoBCD(dia_hex);             // Convierto y guardo dia corregido
      *(fecha+1) = HEXtoBCD(mes_hex);         // Convierto y guardo mes corregido
      *(fecha+2) = HEXtoBCD(anio_hex);        // Convierto y guardo año corregido
    }
    


  /* CHEQUEAR SI EL AÑO ES BICIESTO */
  /**********************************/
    byte chkBiciesto (byte anio){
      byte biciesto;
      
      if ((anio % 4) == 0){
        biciesto = 1;
      }else{
        biciesto = 0;
      }
 
      return(biciesto);
    }

  
  /* CONVERTIR SEGUNDOS A HORA-MIN-SEG */
  /*************************************/
    void seconds_to_HMS (byte* HMS_buffer, uint32_t seg){
      // Rutina que convierte los segundos pasados como argumento en "HH:MM:SS"
      //
      // Como con un HMS no puedo representar mas que 99:59:59
      // => el valor de entrada de los segundos esta limitado a 359999
      // En caso de superarse este valor, devuelve 99:99:99
      volatile uint32_t aux, aux1, aux2;
      byte time[4];
      
      aux = seg / 3600;                      // Extraigo HORAS
      aux1 = (seg - (aux*3600)) / 60;        // Extraigo MINUTOS
      aux2 = seg - (aux*3600) - (aux1*60);   // Extraigo SEGUNDOS
      
      if ((aux > 99) || (aux1 > 59) || (aux2 > 59)){
        aux = 99;
        aux1 = 99;
        aux2 = 99;
      }
      convert_to_string(time, aux, 0, base_DECIMAL);
      string_copy(HMS_buffer, time);
      string_FAR_concat(HMS_buffer, (byte*) _2PUNTOS);
      
      convert_to_string(time, aux1, 0, base_DECIMAL);
      string_concat(HMS_buffer, time);
      string_FAR_concat(HMS_buffer, (byte*) _2PUNTOS);
      
      convert_to_string(time, aux2, 0, base_DECIMAL);
      string_concat(HMS_buffer, time);
    }
    

  /* RESTA HORARIA */
  /*****************/
    uint32_t restaHoraria (byte* minuendo, byte* sustraendo){
    // resta hora-min-seg y el resultado lo da en segundos
    // como devuelve tipo WORD, hay error si la diferencia
    // supera las 18hs. El ERROR se identifica con un 
    // resultado = 0xFFFF
    //
    // EDIT 12/06/2012
    // Al pedo devuelve tipo WORD... ahora devuelve uint32_t y no
    // tiene drama con el máximo de hora
      uint32_t dif_hora, dif_min, dif_seg, resultado;
      byte minu,sustra;
      byte igual_hora;
      byte igual_min;
      
      // RESTAR HORA
      minu = BCDtoHEX(minuendo[0]);
      sustra = BCDtoHEX(sustraendo[0]);
      if (minu == sustra){
        igual_hora = 1;           // La hora es IGUAL
      }else{
        igual_hora = 0;           // La hora NO es IGUAL
      }

      if (minu >= sustra){
        // 9:xx:xx
        // 8:xx:xx
        // diferencia = 1 hora
        dif_hora = minu - sustra;
      }else{
        // 23:xx:xx
        // 8:xx:xx
        // diferencia = 9 horas
        dif_hora = (24 - sustra) + minu;
      }
      
      // RESTAR MINUTOS
      minu   = BCDtoHEX(minuendo[1]);
      sustra = BCDtoHEX(sustraendo[1]);
      if (minu == sustra){
        igual_min = 1;           // Los minutos son IGUALES
      }else{
        igual_min = 0;           // Los minutos NO son IGUALES
      }
      
      if (igual_hora){
        // Si la hora es la misma, entonces la diferencia en minutos es el
        // mayor menos el menor
        if (minu >= sustra){
          // 9:50:xx
          // 9:49:xx
          dif_min = minu - sustra;
        }else{
          // 9:49:xx
          // 9:50:xx
          dif_min = sustra - minu;
        }
      }else{
        // Si la hora es distinta, la forma de restar es diferente
        if (minu >= sustra){
          // 9:50:xx
          // 8:49:xx
          // diferencia = 1hora, 1 min
          dif_min = minu - sustra;
        }else{
          // 9:03:xx
          // 8:50:xx
          // diferencia = 13min  (ver q al haber hecho la resta de hora,
          // nos da 1 hora de diferencia. Por eso, si minu<sustra
          // le resto 1 a la diferencia de hora, xq no hay una hora
          // de diferencia, sino q hay minutos de diferencia, pero
          // que justo cambio de una hora a la otra
          dif_min = (60 - sustra) + minu;
          dif_hora--;
        }
      }
      
      // RESTAR SEGUNDOS
      minu   = BCDtoHEX(minuendo[2]);
      sustra = BCDtoHEX(sustraendo[2]);
      if (igual_min){
        // Si los minutos son los mismos, entonces la diferencia en segundos es el
        // mayor menos el menor
        if (minu >= sustra){
          // 9:50:03
          // 9:49:00
          dif_seg = minu - sustra;
        }else{
          // 9:50:00
          // 9:50:03
          dif_seg = sustra - minu;
        }
      }else{
        if (minu >= sustra){
          // 9:50:03
          // 8:49:00
          // diferencia = 1hora, 1 min, 3 seg
          dif_seg = minu - sustra;
        }else{
          // 9:03:01
          // 8:50:02
          // diferencia = 12min, 59 seg (ver q al haber hecho la resta de min,
          // nos da 13min de diferencia. Por eso, si minu<sustra
          // le resto 1 a la diferencia de min, xq no hay 13 min
          // de diferencia, sino q hay 12min y 59 seg de diferencia, pero
          // que justo cambio de un minuto a otro
          dif_seg = (60 - sustra) + minu;
          dif_min--;
        }
      }

      resultado = dif_hora * 3600 + dif_min * 60 + dif_seg;   // resultado en seg
      
      return(resultado);
    }

  
  /* DIFERENCIA HORARIA */
  /**********************/
    byte diferenciaHoraria (byte* minuendo, byte* sustraendo,uint16_t dif_pedida){
    // Hace la resta entre el minuendo y el sustraendo, y la compara con la diferencia
    // pedida (en seg). Si es menor o igual, devuelve "1" sino devuelve "0" en el acum B
      uint32_t dif_real;
      
      dif_real = restaHoraria (minuendo,sustraendo);
      
      if (dif_real <= dif_pedida){
        // diferencia menor a la requerida
        return (1);
      }else{
        // diferencia mayor a la requerida
        return (0);
      }
    }


  /* DIFERENCIA DE DATE */
  /**********************/
    uint32_t restaDate (tDATE date2, tDATE date1){
      // Rutina que resta 2 DATEs (date2 - date1) y devuelve el resultado en dias
      // El modo de proceder es el siguiente:
      //
      //  1- Calculo diferencia en AÑOS
      //  2- Calculo diferencia en MESES y corrijo diferencia en AÑOS de ser necesario
      //  3- Calculo diferencia en DIAS y corrijo diferencia en MES de ser necesario
      //  4- Calculo diferencia total en DIAS
      uint16_t dY, dM,dD;
      byte D1, D2,M1,M2,Y1, Y2,finM1;
      uint16_t longY1, longY2;
      uint32_t difDias;
      byte i;
      uint32_t seconds;
      
      D1 = BCDtoHEX(date1.fecha[0]);
      M1 = BCDtoHEX(date1.fecha[1]);
      Y1 = BCDtoHEX(date1.fecha[2]);
      
      if (Y1 > 50){
        // Si es mayor a 50, es del 1900
        longY1 = 1900 + Y1;
      }else{
        longY1 = 2000 + Y1;
      }
      
      D2 = BCDtoHEX(date2.fecha[0]);
      M2 = BCDtoHEX(date2.fecha[1]);
      Y2 = BCDtoHEX(date2.fecha[2]);
      
      if (Y2 > 50){
        // Si es mayor a 50, es del 1900
        longY2 = 1900 + Y2;
      }else{
        longY2 = 2000 + Y2;
      }
      
      // 1- Diferencia de AÑOS
      dY = longY2 - longY1;
      
      //  2- Calculo diferencia en MESES y corrijo diferencia en AÑOS de ser necesario
      if (M2 >= M1){
        dM = M2 - M1;
      }else{
        dM = (12 - M1) + M2;          // Los mese que falta para que termine el año anterior, mas los meses actuales
        dY--;                         // Como hubo cambio de año y no se superaron los meses, decremento diferencia de AÑOS
      }
      
      //  3- Calculo diferencia en DIAS y corrijo diferencia en MES de ser necesario
      if (D2 >= D1){
        dD = D2 - D1;
      }else{
        finM1 = Tabla_FinMeses[M1];
        if ((M1 == 2) && (Y1%4 == 0)){
          finM1++;                    // Febrero y Año bisiesto => termina un dia despues
        }
        dD = (finM1 - D1) + D2;       // Los dias que falta para que termine el mes anterior, mas los dias actuales
        dM--;                         // Como hubo cambio de mes y no se superaron los dias, decremento diferencia de MESES
      }

      // 4- Calculo diferencia total en DIAS
      // Segun diferencia de AÑOS
      difDias = 0;
      i = Y1;
      while (i < Y1 + dY){
        difDias += 365;
        if ((i % 4 == 0) && (M1<=2)){
          difDias++;
        }
        i++;
      }
      
      // Segun diferencia de MESES
      i = M1;
      while (i < M1 + dM){
        if (i<=12){
          difDias += Tabla_FinMeses[i];
        }else{
          difDias += Tabla_FinMeses[i-12];
        }
        if ((i == 2) &&  (Y1 % 4 == 0)){
          difDias++;                  // Febrero y año bisiesto
        }
        i++;
      }
      
      // Segun diferencia de DIAS
      difDias += dD;

      seconds = restaHoraria(date2.hora, date1.hora);
      
      if ((seconds <= 86400) && 
          (date2.hora[0] <= date1.hora[0]) &&
          (date2.hora[1] <= date1.hora[1]) &&
          (date2.hora[2] <= date1.hora[2])){
        // 86400seg = 24hs => Si pasaron menos de 24hs => a la diferencia de dias 
        // le debo restar 1, siempre y cuando sea mayor a 0
        //
        // EDIT 26/09/12
        //  Ademas se debe cumplir que la nueva hora (hora2) sea menor o igual a la hora1
        if (difDias > 0){
          difDias--;
        }
      }
      
      return(difDias);  
    }


  /* DIFERENCIA DE DIAS EN SEGUNDOS */
  /**********************************/
    uint32_t diferenicaDias_toSeconds(uint32_t difDias, byte* hora2, byte* hora1){
      uint32_t seconds;
      
      seconds = restaHoraria(hora2, hora1);

      seconds += (difDias * 86400);   // Sumo segundos equivalentes a la diferencia de dias
      
      return(seconds);
    }


  /* DETERMINAR DIA DE LA SEMANA */
  /*******************************/
    byte determineDiaSemana (byte* fecha){
      // CALCULO DIA SEMANA
      // El dia de la semana se calcula de la siguiente manera:
			//         N = D + clave[M] + A + entero[A/4] + S
			// donde:
			// - D = Dia
			// - M = Mes
			// - A = Ultimos 2 digitos del año
			// - S = 6 (desde 2000 a 2099)
			//
			// Y el dia de la semana es el resto de dividir N por 7. Pero como segun
			// esta ecuacion, el domingo es 0, y para mi el domingo es 1 => le sumo 1
			// Ademas el resultado de la ecuacion va a 0-6 y el RTC maneja de 1-7
			//
			// EDIT 28/11/12
			//  Correccion en la rutina, no era MES, sino CLAVE DEL MES, por eso daba
			// mal. Ademas, no tenia en cuenta que los datos estan en BCD.
			//  http://www.albaiges.com/cronologia/calculodiasemana.htm
      byte DDLS;
      byte dia, mes, anio;
      
      dia = BCDtoHEX(fecha[0]);
      mes = BCDtoHEX(fecha[1]);
      anio = BCDtoHEX(fecha[2]);
      
      DDLS = dia + Tabla_ClaveMeses[mes] + anio + (anio/4) + 6;
      
      DDLS = (DDLS % 7) + 1;
      
      return(DDLS);
    }


#ifndef VISOR_RELOJ_VIEJO  
  /* COMPRIMIR FECHA y HORA */
  /**************************/    
    uint32_t compressDATE (tDATE date, byte* buffer){
      // El DATE (fecha y hora) se comprime, de la siguiente manera:
      // Se determinan los segundos transcurridos desde el 27/09/2012 00:00:00
      // y ahi esta toda la informacion necesaria
      //
      // En caso de que se pase un BUFFER como argumento, en el mismo se devuelve
      // el resultado de la compresion
      uint32_t seconds;
      uint32_t difDias;
      byte* ptr;
      
      if (date.fecha[0] != 0){
        // Tengo FECHA => puedo comprimir
        difDias = restaDate (date, DATE_REFERENCIA);      // Diferencia de DIAS
      
        seconds = diferenicaDias_toSeconds(difDias, date.hora, DATE_REFERENCIA.hora);
      
      }else{
        // NO tengo fecha => NO puedo comprimir
        seconds = 0xFFFFFFFF;
      }
      
      if (buffer != NULL){
        ptr = (byte*) &seconds;
        *buffer++ = *ptr++;
        *buffer++ = *ptr++;
        *buffer++ = *ptr++;
        *buffer++ = *ptr++;
      }

      return(seconds);
    }
#endif

  /* COMPARAR FECHAS */
  /*******************/
    tCOMPARE_FECHA compareFECHA (tDATE date_1, tDATE date_2){
      // Compara DATE_1 con DATE_2
      // Tomando como referencia a date_1
      tCOMPARE_FECHA result;
      uint32_t diffDias_1;
      uint32_t diffDias_2;
      
      // Como solo me intersa la FECHA => la hora la hago toda cero
      date_1.hora[0] = 0;
      date_1.hora[1] = 0;
      date_1.hora[2] = 0;
      
      date_2.hora[0] = 0;
      date_2.hora[1] = 0;
      date_2.hora[2] = 0;
      
      diffDias_1 = restaDate(date_1, DATE_REFERENCIA);
      diffDias_2 = restaDate(date_2, DATE_REFERENCIA);
      
      if (diffDias_1 == diffDias_2){
        result = FECHA_IGUAL;
      }else if(diffDias_1 < diffDias_2){
        result = FECHA_MAYOR;         // 2 > 1
      }else{
        result = FECHA_MENOR;         // 2 < 1
      }
      
      return(result);
    }


#ifdef VISOR_PRINT_TICKET_FULL
  /* OBTENER FECHA DEL SIGUIENTE DIA */
  /***********************************/
    tDATE getNextDate (tDATE date){
      // Devuelve el dia siguiente al pasado como argumento
      byte dia;
      byte mes;
      byte anio;
      byte finMes;
      
      dia = BCDtoHEX(date.fecha[0]);      // Extraigo dia
      mes = BCDtoHEX(date.fecha[1]);      // Extraigo mes
      anio = BCDtoHEX(date.fecha[2]);     // Extraigo anio
      
      dia++;                              // Incremento dia
      
      
      finMes = Tabla_FinMeses[mes];
      if ((mes == 2) && (anio % 4 == 0)){
          finMes++;                       // Febrero y Año bisiesto => termina un dia despues
        }
      
      if (dia > finMes){
        // Si supere el fin de mes => cambio mes y el dia es el 1ro
        dia = 1;                          // Primero Dia del Mes
        mes++;                            // Avanzo Mes
      }
      
      if (mes > 12){
        // Si me pase de DICIEMBRE comienzo en ENERO y avanzo el año
        mes = 1;                          // Primer Mes del Año
        anio++;                           // Avanzo anio
      }
      
      date.fecha[0] = HEXtoBCD(dia);
      date.fecha[1] = HEXtoBCD(mes);
      date.fecha[2] = HEXtoBCD(anio);
      
      return(date);
    }
#endif


#ifdef VISOR_GPS_Tx_NAV_INIT
  /* CONVERTIR DATE A SEGUNDOS+SEMANA GPS */
  /****************************************/
    void date_to_GPS_weekSeconds (byte* week_ptr, byte* secs_ptr, tDATE date){
      // Info acerca de como esta hecha esta conversion:
      //    http://www.novatel.com/support/knowledge-and-learning/published-papers-and-documents/unit-conversions/
      //
      // DETERMINACION DE SEMANA (desde 06/01/1980)
      //  1- Hago una resta de FECHAS (el resultado es en DIAS)
      //  2- Como la semana tiene 7 dias, divido x7 y tengo la cantidad de semanas
      //
      // DETERMINACION DE SEGUNDOS 
      //  1- Si no fue un numero entero de semanas al dividir x7, multiplico esa diferencia
      //     por la cantidad de segundos que tiene un dia
      //  2- Luego convierto a segundos la hora actual
      //  3- Sumo ambos valores y ya tengo los segundos
      //
      // Finalmente, el resultado es convertido a STRING y se devuelve en los respectivos buffers
      uint32_t diffDias;
      uint32_t week;
      uint32_t diasResto;
      uint32_t secs;
      
      diffDias = restaDate(date, DATE_REFERENCIA_GPS);          // Obtengo diferencia en DIAS desde del GPS
      
      week = diffDias / 7;                                      // Calculo cantidad de semanas completas
      diasResto = diffDias - (week * 7);                        // Dias que me sobran
      
      secs = restaHoraria(date.hora, DATE_REFERENCIA_GPS.hora); // Calculo segundos desde las CERO horas
      secs += (diasResto * 86400);                              // Le sumo la cantidad de segundos, segun dias de resto
      
      // Convierto SEMANA a string
      if (week_ptr != NULL){
        convert_to_string(week_ptr, week, 0xFF, base_DECIMAL);  // Convierto a STRING la cantidad de SEMANAS
      }
      
      // Convierto SEGUNDOS a string
      if (secs_ptr != NULL){
        convert_to_string(secs_ptr, secs, 0xFF, base_DECIMAL);  // Convierto a STRING la cantidad de SEGUNDOS
      }
    }
#endif


/* ******************************* 
 *  ROTAR UN BIT A LA IZQUIERDA   *
 *********************************/

byte  rotarUnBitIzq(byte *a,byte bit){


  /*
 	 
 	 Esta rutina desplaza un bit dos bytes (a,b) y devuelve el bit mas significativo. 
 	 Agregando un cero o un uno al bit menos significativo dependiendo del valor de c.
 	  
            bit de retorno           a             bit de ingreso             
           _ _ _ _ _ _ _ _     _ _ _ _ _ _ _ _     _ _ _ _ _ _ _ _     
          | | | | | | | | |<--| | | | | | | | |<--| | | | | | | | |   
           - - - - - - - -     - - - - - - - -     - - - - - - - -     
            
  */


  byte auxa;
  
  auxa =0x80&*a;  //guardo bit mas significativo
  *a=*a<<1;       //roto a la izquierda el dato de entrada
  *a=*a|bit;      //le agrego el bit de entrada
  bit=auxa>>7;    //preparo el bit de retorno para retornarlo

  return(bit);

}




/* *********************************** 
 *  ROTAR UN BUFFER A LA IZQUIERDA   *
 *************************************/
 
byte  rotarBuffIzq(byte *buff,uint16_t N, byte BIT){

 /*

                          BUFFER
  
	      			      	+---------------+   <- buff_ptr   
		    	 RETURN  <- | | | | | | | | | A	          - 	
		    				      +---------------+  		        |   
		     			        | | | | | | | | | B 	        |   
		      			      + --------------+		          | N   
		    				      | | | | | | | | | C		        | 
		    				      +---------------+		          |
		    				      | | | | | | | | | D <- BIT 	  -
         	    	      + --------------+		

						    
						  RETURN <- A <- B <- C <- D <- BIT
						        
 */





       byte *buff_ptr;
       byte bit;
       byte i;
       
       bit=BIT;
       buff_ptr = buff + (N-1);
       
       for(i=0;i<N;i++){
          bit=rotarUnBitIzq(buff_ptr,bit);
          buff_ptr--;
       }
       
      return(bit);

}


#ifdef VISOR_USES_SKYPATROL
  /* CALCULO DE CHKSUM PROTOCOLO BAX DE UN STRING */
  /************************************************/
   
   /*
   Ejemplo:
   N = 8
   buffer[] ={31 30 41 39 31 42 37 36}
   
   en hex 10 A9 1B 76
   chksum = 10^A9^1B^76 = D4 
   
   */
  byte cal_chk(byte N, byte* Buffer_ptr){
   byte chksum;
   byte aux;
   
   chksum=0;
   if(N%2 == 0){                              //verifico que sea multiplo de dos
     while(N!=0){
       aux = string2byte_to_hex(Buffer_ptr);
       chksum = (chksum)^aux;
       N--;
       N--;
       Buffer_ptr++;
       Buffer_ptr++;
     }
   }
   return (chksum);
  }

  /* CONVIERTE STRING A,B,C,D a hex  */
  /***********************************/
  /* 
   41 -> 0x0A
   42 -> 0x0B
   43 -> 0x0C
   44 -> 0x0D
   45 -> 0x0E
   46 -> 0x0F
   */
   
   byte char_to_hex (byte dato){
        return(dato - 0x37);
   }

  /* CONVIERTE STRING XX a hex  */
  /******************************/
  /*
   EJEMPLO:  "1B" = 31 42 -> 0x1B 
  */

      
      
      byte string2byte_to_hex(byte* buffer_ptr){
         byte aux1,aux2;
         
         //tomo primer byte en ascii y lo convierto a hex
         if(*buffer_ptr < 0x3a){
          aux1 = *buffer_ptr -0x30;             //convierte en decimal 
         }else{
          aux1 = char_to_hex(*buffer_ptr);      //convierte en Hex  
         }
         aux1= aux1<<4;
         
         //tomo segundo byte en ascii y lo convierto a hex
         if(*(buffer_ptr+1) < 0x3a){
          aux2 = *(buffer_ptr+1) -0x30;              //convierte en decimal 
         }else{
          aux2 = char_to_hex(*(buffer_ptr+1));      //convierte en Hex  
         }
         aux2= aux2&0x0F;
         
         //uno los dos byte y devuelvo el resultado
         return(aux1|aux2);
      }

#endif
  
  void convert_bigINDIAN_to_litleINDIAN (uint8_t* buffer, uint8_t n){
	 /*
	  El sistema big-endian adoptado por Motorola entre otros, consiste en representar los bytes
	  en el orden "natural": así el valor hexadecimal 0x4A3B2C1D se codificaría en memoria en la
	  secuencia {4A, 3B, 2C, 1D}.
	  En el sistema little-endian adoptado por Intel, entre otros, el mismo valor se codificaría
	  como {1D, 2C, 3B, 4A}
	  */
	  uint8_t aux,j;

	  j=0;
	  while(j < n-1){
		  aux = *(buffer+j);
		  *(buffer+j) = *(buffer+(n-1)-j);
		  *(buffer+(n-1)-j) = aux;
		  j++;
	  }
  }

uint8_t byteTOascii(uint8_t dataBYTE, uint8_t parte ){
	if(parte==ALTA){
	dataBYTE = dataBYTE & 0xf0;
	dataBYTE = dataBYTE>>4;
	}else{
		dataBYTE = dataBYTE & 0x0f;
	}

	if(dataBYTE == 10){dataBYTE = 'a';
	}else if(dataBYTE == 11){dataBYTE = 'b';
	}else if(dataBYTE == 12){dataBYTE = 'c';
	}else if(dataBYTE == 13){dataBYTE = 'd';
	}else if(dataBYTE == 14){dataBYTE = 'e';
	}else if(dataBYTE == 15){dataBYTE = 'f';
	}else {dataBYTE= dataBYTE + 0x30;
	}
	return(dataBYTE);
}


//carga en un vector de 4 bytes(DATA[4]), la direccon de una variable en asii
void  cargaContenidoPuntero_inDATA (uint16_t* ptrDIR, byte* DATA){

            uint8_t  dataBYTE;
            uint16_t data16;

        	//data16 = *dirREPORTE_PUTptr;
        	data16 = *ptrDIR;
            dataBYTE = data16 & 0x000f;
            DATA[3] = byteTOascii(dataBYTE,0);

            dataBYTE = (data16 & 0x00f0)>>4;
            DATA[2] = byteTOascii(dataBYTE,0);

            dataBYTE = (data16 & 0x0f00)>>8;
            DATA[1] = byteTOascii(dataBYTE,0);

            dataBYTE = (data16 & 0xf000)>>12;
            DATA[0] = byteTOascii(dataBYTE,0);

        }

byte checkRANGE(byte* ini_ptr, byte*  fin_ptr, byte timeOUT ){
    byte error;
    error=0;
	if((ini_ptr > fin_ptr) || timeOUT){
	#ifdef RELOJ_DEBUG
		_Error_Handler(__FILE__, __LINE__);
	#endif
		 error=1;
	 }
	return (error);
 }

  /* TEST RESTA DAYS */
  /*******************/    
    /*void testDifDays (void){
      tDATE date1,date2;
      
      //-------------------------
      date1.fecha[0] = 0x10;
      date1.fecha[1] = 0x01;
      date1.fecha[2] = 0x12;
      
      date2.fecha[0] = 0x25;
      date2.fecha[1] = 0x05;
      date2.fecha[2] = 0x12;
      
      if (restaDate(date2, date1) != 136){
        asm BGND;
      }
      //-------------------------
      
      date1.fecha[0] = 0x10;
      date1.fecha[1] = 0x01;
      date1.fecha[2] = 0x12;
      
      date2.fecha[0] = 0x25;
      date2.fecha[1] = 0x02;
      date2.fecha[2] = 0x12;
      
      if (restaDate(date2, date1) != 46){
        asm BGND;
      }
      //-------------------------
      
      date1.fecha[0] = 0x10;
      date1.fecha[1] = 0x10;
      date1.fecha[2] = 0x12;
      
      date2.fecha[0] = 0x25;
      date2.fecha[1] = 0x02;
      date2.fecha[2] = 0x13;
      
      if (restaDate(date2, date1) != 138){
        asm BGND;
      }
      //-------------------------
      
      date1.fecha[0] = 0x10;
      date1.fecha[1] = 0x10;
      date1.fecha[2] = 0x12;
      
      date2.fecha[0] = 0x10;
      date2.fecha[1] = 0x10;
      date2.fecha[2] = 0x13;
      
      if (restaDate(date2, date1) != 365){
        asm BGND;
      }
      //-------------------------
      
      date1.fecha[0] = 0x10;
      date1.fecha[1] = 0x02;
      date1.fecha[2] = 0x12;
      
      date2.fecha[0] = 0x10;
      date2.fecha[1] = 0x02;
      date2.fecha[2] = 0x13;
      
      if (restaDate(date2, date1) != 366){
        asm BGND;
      }
      //-------------------------
      
      date1.fecha[0] = 0x10;
      date1.fecha[1] = 0x02;
      date1.fecha[2] = 0x12;
      
      date2.fecha[0] = 0x30;
      date2.fecha[1] = 0x03;
      date2.fecha[2] = 0x12;
      
      if (restaDate(date2, date1) != 49){
        asm BGND;
      }
      //-------------------------
      
      date1.fecha[0] = 0x20;
      date1.fecha[1] = 0x02;
      date1.fecha[2] = 0x12;
      
      date2.fecha[0] = 0x05;
      date2.fecha[1] = 0x03;
      date2.fecha[2] = 0x12;
      
      if (restaDate(date2, date1) != 14){
        asm BGND;
      }
    }*/


/* File: <Tarifa Automatica.c> */

/* INCLUDES */
/************/
  
 // #include "Programacion Parametros.h"
  
  #include "Calculos.h"
  #include "Inicio.h"
  
  #include "Reloj.h"
  #include "Tarifa Automatica.h"
  #include "Tarifacion Reloj.h"
  #include "Parametros Reloj.h"
  #include "eeprom_aux.h"
  
  #include "RTC.h"


/*********************************************************************************************/
/* PROTOTIPOS */
/**************/
  // RUTINAS INTERNAS
  static byte determineFeriado (tDATE date);
  static tTARIFA* determineTarifaFeriado (tDATE date);
  static tTARIFA* determineTarifaDiaSemana (tDATE date);
    

/*********************************************************************************************/
/* VARIABLES */
/*************/
  

/*********************************************************************************************/
/* RUTINAS */
/***********/

  /* SELECCION AUTOMATICA DE TARIFA */
  /**********************************/
    byte TARIFA_AUTO_getNroTarifa (void){
      // Funcion que devuelve el numero de tarifa a seleccionar, teniendo en 
      // cuentan la vigencia programada.
      //
      // Lo primero que hay quehacer es determinar si se trata de un feriado
      // 
      // FERIADO:
      //  1- Seleccionar el Numero de tarifa que tiene programado el calendario
      //  2- Determinar DIURNA o NOCTURNA
      //
      // NO FERIADO
      //  1- Seleccionar la/s tarifa/s habilita/s ese dia de la semana
      //  2- Determinar DIURNA o NOCTURNA
      //
      //
      // EDIT 17/08/2012
      //  Debo tener fecha valida para poder levantar una tarifa automaticamente
      // => Si no tengo fecha y hora, levanto tarifa 1, por defecto
      byte feriado;
      tTARIFA* tarifaPTR;
      tDATE date;
      
      date = getDate();                     // Extraigo fecha y hora
      
      if (dateValid(date)){
        feriado = determineFeriado(date);
        
        if (feriado){
          tarifaPTR = determineTarifaFeriado(date);
        }else{
          tarifaPTR = determineTarifaDiaSemana(date);
        }
      
      }else{
        // Como no tengo fecha valida, levanto tarifa 1
        tarifaPTR = (tTARIFA*) (&EEPROM_PROG_relojT1D);
      }
      
      return(tarifaPTR->numero);
    }




/*********************************************************************************************/
/* RUTINAS INTERNAS */
/********************/

  /* DETERMINAR SI SE TRATA DE UN FERIADO */
  /****************************************/
    static byte determineFeriado (tDATE date){
      // Rutina que determina si el dia pasado como argumento es un feriado (dentro de los
      // feriados programados).
      // 0: NO es Feriado
      // 1: SI es Feriado
      byte i;
      byte isFeriado;
      
      isFeriado = 0;                          // Asumo que no es feriado
      for (i=0; i<cant_Feriados; i++){
        if ((CALENDARIO_getDIA(i) == date.fecha[0]) && (CALENDARIO_getMES(i) == date.fecha[1])){
          isFeriado = 1;                      // Se trata de un feriado
        }
      }

      return(isFeriado);
    }


  /* DETERMINAR TARIFA PARA UN FERIADO */
  /*************************************/
    static tTARIFA* determineTarifaFeriado (tDATE date){
      // Esta rutina determina el Nro de Tarifa a levantar para un dia feriado
      tTARIFA* tarifaDiurnaPTR;
      tTARIFA* tarifaNocturnaPTR;
      tTARIFA* resultTarifaPTR;
      byte i;
      
      tarifaDiurnaPTR = NULL;
      tarifaNocturnaPTR = NULL;
      
      // DIURNA
      i = 1;
      while ((i <= prgRELOJ_determineCantTarifasD_MANUAL()) && (tarifaDiurnaPTR == NULL)){
        tarifaDiurnaPTR = prgRELOJ_getTarifa_pointer(i);
        
        if (tarifaDiurnaPTR->vigencia.Bits.cal != 1){
          tarifaDiurnaPTR = NULL;                // Todavia no encontre la tarifa
        }
        i++;
      }
      
      
      // NOCTURNA
      i = 1;
      while ((i <= prgRELOJ_determineCantTarifasN_MANUAL()) && (tarifaNocturnaPTR == NULL)){
        tarifaNocturnaPTR = prgRELOJ_getTarifa_pointer(i + (tarifa1N - 1) );
        
        if (tarifaNocturnaPTR->vigencia.Bits.cal != 1){
          tarifaNocturnaPTR = NULL;                // Todavia no encontre la tarifa
        }
        i++;
      }
      
      // Cuando se selecciona el CALENDARIO => Esta tarifa tienen vigente el calendario
      // para el DIA y la NOCHE => Lo que tengo que hacer es determinar cual de las 2
      // elijo.
      // La tarifa NOCTURNA puede estar programada o no. En cuyo caso la DIURNA esta 
      // siempre vigente
      if (tarifaDiurnaPTR != NULL){
        if (tarifaNocturnaPTR == NULL){
          // No existe la nocturna => Queda vigente la DIURNA. (puede ser que sea NULL
          // lo cual indicaria un error, pero eso lo veo mas adelante; es decir que lo
          // propago)
          resultTarifaPTR = tarifaDiurnaPTR;
        
        }else{
          // Existen tanto la DIURNA como la NOCTURA => Depende de la hora
          resultTarifaPTR = determineTarifaDiurnaNocturna(date, tarifaDiurnaPTR, tarifaNocturnaPTR);
        }
      
      }else{
        resultTarifaPTR = NULL;           // Hay un error xq no encontre tarifa Diurna
      }
      
      return(resultTarifaPTR);
    }


  /* DETERMINAR TARIFA PARA UN DIA DE LA SEMANA */
  /**********************************************/
    static tTARIFA* determineTarifaDiaSemana (tDATE date){
      // Esta rutina determina el Nro de Tarifa a levantar para un dia de la semana
      // La vigencia (segun la programacion) se expresa como:
      //
      //msb                               lsb
      // | D | L | M | M | J | V | S | CAL |
      //
      // Mientras que dentro del RTC, los dias de la semana van del 1-7, siendo 1= DOMINGO
      //
      // => podemos decir que la "mascara" dentro de la vigencia, para el dia de la semana
      // se puede expresar como:
      //
      //          mask = 2^(8 - DDLS);
      //
      // Luego, hay que determinar las tarifas vigentes (DIURNA y NOCTURNA), vigentes ese dia
      byte dayMask;
      byte DDLS;
      tTARIFA* tarifaDiurnaPTR;
      tTARIFA* tarifaNocturnaPTR;
      tTARIFA* resultTarifaPTR;
      byte n_tarifas_habilitadas;
      byte i;
      
      DDLS = determineDiaSemana(date.fecha);  // Calculo dia de la semana (DDLS)

      dayMask = potencia(2, 8 - DDLS);        // Determino mascara en vigencia segun DDLS
      
      
      tarifaDiurnaPTR = NULL;
      tarifaNocturnaPTR = NULL;
      
      // DIURNA
      i = 1;
      n_tarifas_habilitadas = prgRELOJ_determineCantTarifasD_MANUAL();
      while ((i <= n_tarifas_habilitadas) && (tarifaDiurnaPTR == NULL)){
        tarifaDiurnaPTR = prgRELOJ_getTarifa_pointer(i);
        
        if ((tarifaDiurnaPTR->vigencia.Byte & dayMask) != dayMask){
          tarifaDiurnaPTR = NULL;                // Todavia no encontre la tarifa
        }
        i++;
      }
      
      
      // NOCTURNA
      i = 1;
      n_tarifas_habilitadas = prgRELOJ_determineCantTarifasN_MANUAL();
      while ((i <= n_tarifas_habilitadas) && (tarifaNocturnaPTR == NULL)){
        tarifaNocturnaPTR = prgRELOJ_getTarifa_pointer(i + (tarifa1N - 1) );
        
        if ((tarifaNocturnaPTR->vigencia.Byte & dayMask) != dayMask){
          tarifaNocturnaPTR = NULL;                // Todavia no encontre la tarifa
        }
        i++;
      }
      
      // TENGO que determinar cual de las 2 elijo.
      // La tarifa NOCTURNA puede estar programada o no. En cuyo caso la DIURNA esta 
      // siempre vigente.
      if (tarifaDiurnaPTR != NULL){
        if (tarifaNocturnaPTR == NULL){
          // No existe la nocturna => Queda vigente la DIURNA. (puede ser que sea NULL
          // lo cual indicaria un error, pero eso lo veo mas adelante; es decir que lo
          // propago)
          resultTarifaPTR = tarifaDiurnaPTR;
        
        }else{
          // Existen tanto la DIURNA como la NOCTURA => Depende de la hora
          resultTarifaPTR = determineTarifaDiurnaNocturna(date, tarifaDiurnaPTR, tarifaNocturnaPTR);
        }
      
      }else{
        resultTarifaPTR = NULL;           // Hay un error xq no encontre tarifa Diurna
      }
      
      return(resultTarifaPTR);
    }


  /* DETERMINAR TARIFA DIURNA/NOCTURNA */
  /*************************************/
    tTARIFA* determineTarifaDiurnaNocturna (tDATE dateActual, tTARIFA* TD, tTARIFA* TN){
      // Rutina que determina, segun el horario, si corresponde tarifa diurna o nocturna.
      //
      // Para cuando la hora de inicio de la tarifa DIURNA es MENOR al de la NOCTURNA, 
      // tenemos los posibles casos
      //
      // |--------(1)---x---(2)-------------x---(3)--|
      // 0hs           TD                   TN       23.59hs
      //
      //
      // (1) hora < Inicio de Tarifa Diurna ==> Corresponde NOCTURNA
      // (2) hora >= Inicio de Tarifa Diurna y hora < Inicio de Tarifa Nocturna=> Corresponde DIURNA
      // (3) hora >= Inicio de Tarifa Nocturna ==> Corresponde NOCTURNA
      //
      //
      // Para cuando la hora de inicio de la tarifa DIURNA es MAYOR al de la NOCTURNA, 
      // tenemos los posibles casos
      //
      // |--------(4)---x---(5)-------------x---(6)--|
      // 0hs           TN                   TD       23.59hs
      //
      //
      // (4) hora < Inicio de Tarifa Nocturna ==> Corresponde DIURNA
      // (5) hora >= Inicio de Tarifa Nocturna y hora < Inicio de Tarifa Diurna=> Corresponde NOCTURNA
      // (6) hora >= Inicio de Tarifa Diurna ==> Corresponde DIURNA      
      //
      //
      // (0) Si la hora de inicio de ambas tarifas es la misma => Corresponde DIURNA
      
      uint16_t horaMin;
      tTARIFA* resultTarifaPTR;
      
      horaMin = dateActual.hora[0];
      horaMin <<= 8;
      horaMin |= dateActual.hora[1];        // Tengo HH|MM
      
      if (TD->horaInicio == TN->horaInicio){
        // Caso (0)
        //resultTarifaPTR = TD;               // DIURNA
        resultTarifaPTR = TN;               // NOCTURNA
      
      }else if (TD->horaInicio < TN->horaInicio){
        // DIURA MENOR A NOCTURNA
        if ((horaMin >= TD->horaInicio) && (horaMin < TN->horaInicio)){
          // Caso (2)
          resultTarifaPTR = TD;             // DIURNA
        }else{
          // Caso (1) y (3)
          resultTarifaPTR = TN;             // NOCTURNA
        }
      
      }else if (TD->horaInicio > TN->horaInicio){
        // DIURA MAYOR A NOCTURNA
        if ((horaMin >= TN->horaInicio) && (horaMin < TD->horaInicio)){
          // Caso (5)
          resultTarifaPTR = TN;             // NOCTURNA
        }else{
          // Caso (4) y (6)
          resultTarifaPTR = TD;             // DIURNA
        }
      }
      
      return (resultTarifaPTR);
    }
    
    
    
  /* DETERMINAR TARIFA DIURNA/NOCTURNA */
  /*************************************/
    /*static tTARIFA* determineTarifaDiurnaNocturna (tDATE dateActual, tTARIFA* TD, tTARIFA* TN){
      // Rutina que determina, segun el horario, si corresponde tarifa diurna o nocturna.
      //
      // Los posibles casos son:
      //
      // |--------------x********************x-------|
      // 0hs           TD                   TN       0hs
      //
      // 
      // |*****x----------------------x**************|
      // 0hs   TN                    TD              0hs
      //
      //
      // |*****x*************************************|
      // 0hs   TN=TD                                 0hs
      //
      // En las zonas ***** corresponde tarifa diurna
      uint16_t horaMin;
      tTARIFA* resultTarifaPTR;
      
      horaMin = dateActual.hora[0];
      horaMin <<= 8;
      horaMin |= dateActual.hora[1];        // Tengo HH|MM
      
      if ((horaMin >= TD->horaInicio) && (horaMin < TN->horaInicio)){
        // |--------------x********************x-------|
        // 0hs           TD                   TN       0hs  
        resultTarifaPTR = TD;               // Corresponde tarifa DIURNA

      }else if ((horaMin >= TN->horaInicio) && (horaMin < TD->horaInicio)){
        // |*****x----------------------x**************|
        // 0hs   TN                    TD              0hs
        resultTarifaPTR = TN;               // Corresponde tarifa NOCTURNA
      
      }else if (TN->horaInicio == TD->horaInicio){
        // |*****x*************************************|
        // 0hs   TN=TD                                 0hs
        resultTarifaPTR = TD;               // Corresponde tarifa DIURNA
      
      }else if ((TN->horaInicio == 0) && (horaMin >= TD->horaInicio)){
        // |--------------x****************************x
        // TN=0hs       TD                           TN=0hs
        resultTarifaPTR = TD;               // Corresponde tarifa DIURNA
      
      }else if ((TD->horaInicio == 0) && (horaMin >= TN->horaInicio)){
        // x***********************x-------------------x
        // TD=0hs                  TN                TN=0hs
        resultTarifaPTR = TN;               // Corresponde tarifa NOCTURNA
      
      }else{
        resultTarifaPTR = NULL;             // ERROR
      }
      
      return (resultTarifaPTR);
    }*/

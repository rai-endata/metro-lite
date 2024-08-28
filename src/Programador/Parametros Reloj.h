/* File: <Parametros Reloj.h> */

#ifndef __PARAMETROS_RELOJ__
#define	__PARAMETROS_RELOJ__


  /* INCLUDES */
  /************/

    #include "Programacion Parametros.h"
	#include "Parametros Ticket.h"
	#include "Parametros Movil.h"
   	//#include "eeprom.h"
	#include "eeprom_e.h"


  /* VARIABLES */
  /*************/
    // TARIFAS
    #define cantidadTarifasProgramables   8
    #define tarifa1D  1
    #define tarifa2D  2
    #define tarifa3D  3
    #define tarifa4D  4
    #define tarifa1N  5
    #define tarifa2N  6
    #define tarifa3N  7
    #define tarifa4N  8

    //#define cant_Feriados               20
    //#define dim_Feriados                (cant_Feriados + 2) // Como cada feriado son 2 bytes, le sumo 2 feriados mas para DF0A y multiplo 4



    // PARAMETROS DE RELOJ COMUNES
      #define PULSOS_x_KM                 (EEPROM_PROG_relojCOMUN.pulsosKm)
      #define PULSOS_x_KM_UNKNOWN       (PULSOS_x_KM == '?')
      #define PULSOS_x_KM_toGetIt       (1000) // Valor de la CONSTANTE para obtener la CONSTANTE

	//para el programador de endata la carrera de bandera
	//se define como la distancia/tiempo que debe recorrer/tanscurrir desde que se presiona el sensor
    //de asiento para que el taximetro de ocupe por sensor de aiento.

	#define CARRERA_BANDERA             (EEPROM_PROG_relojCOMUN.carreraBandera)

    #define prgRELOJvarios1             (EEPROM_PROG_relojCOMUN.varios1)
        #define T1D_hab                 (EEPROM_PROG_relojCOMUN.varios1.Bits.T1Dhab)
        #define T2D_hab                 (EEPROM_PROG_relojCOMUN.varios1.Bits.T2Dhab)
        #define T3D_hab                 (EEPROM_PROG_relojCOMUN.varios1.Bits.T3Dhab)
        #define T4D_hab                 (EEPROM_PROG_relojCOMUN.varios1.Bits.T4Dhab)

    #define TIPO_SENSOR_ASIENTO       (EEPROM_PROG_relojCOMUN.varios1.MergedBits.tipo_sensor)
        #define SIN_SENSOR_ASIENTO      (TIPO_SENSOR_ASIENTO == 0)
        #define SENSOR_ASIENTO_NORM     (TIPO_SENSOR_ASIENTO == 1)
        #define SENSOR_ASIENTO_INTEL    (TIPO_SENSOR_ASIENTO == 2)

	  #define SENSOR_ASIENTO_segundos   (EEPROM_PROG_relojCOMUN.varios1.Bits.sensorSeg)
      #define TIPO_SERVICIO             (EEPROM_PROG_relojCOMUN.varios1.Bits.tipo_servicio)
        #define REMIS                   0
        #define TAXI                    1
        #define SERVICIO_TAXI           (TIPO_SERVICIO == TAXI)
        #define SERVICIO_REMIS          (TIPO_SERVICIO == REMIS)


    #define prgRELOJvarios2             (EEPROM_PROG_relojCOMUN.varios2)
      #define CORRECCION_HORARIA        (EEPROM_PROG_relojCOMUN.varios2.MergedBits.dif_hora)
      #define signoCorreccionHora       (EEPROM_PROG_relojCOMUN.varios2.Bits.sgn_difHora)
        #define positivo                0
        #define negativo                1
      #define PUNTO_DECIMAL             (EEPROM_PROG_relojCOMUN.varios2.MergedBits.pto_decimal)
	  #define HORA_source               (EEPROM_PROG_relojCOMUN.varios2.Bits.hora_source)
	  //#define HORA_source               horaSourceRTC
      //#define HORA_source               0

        #define horaSourceGPS           0
        #define horaSourceRTC           1
        #define HORA_GPS                (HORA_source == horaSourceGPS)
        #define HORA_RTC                (HORA_source == horaSourceRTC)

        #define prgRELOJvarios3         (EEPROM_PROG_relojCOMUN.varios3)
        #define T1N_hab                  EEPROM_PROG_relojCOMUN.varios3.Bits.T1Nhab
        #define T2N_hab                 (EEPROM_PROG_relojCOMUN.varios3.Bits.T2Nhab)
        #define T3N_hab                 (EEPROM_PROG_relojCOMUN.varios3.Bits.T3Nhab)
        #define T4N_hab                 (EEPROM_PROG_relojCOMUN.varios3.Bits.T4Nhab)

      #define teclaEspera               (EEPROM_PROG_relojCOMUN.varios3.Bits.tecEspera)
      #define NoAcumularEspera          (EEPROM_PROG_relojCOMUN.varios3.Bits.NOacum_espera)
      #define sinEspera                 (EEPROM_PROG_relojCOMUN.varios3.Bits.NOespera)
      #define TARIFACION                (EEPROM_PROG_relojCOMUN.varios3.Bits.tarifacion)
        #define PESOS                   0
        #define FICHAS                  1
        #define TARIFA_PESOS            (TARIFACION == PESOS)
        #define TARIFA_FICHAS           (TARIFACION == FICHAS)

    #define prgRELOJvarios4             (EEPROM_PROG_relojCOMUN.varios4)
      #define cantCHOFERES              (EEPROM_PROG_relojCOMUN.varios4.MergedBits.cant_choferes)
      #define seleccionManualTarifas    (EEPROM_PROG_relojCOMUN.varios4.Bits.tarifasManual)
      #define viajesCD_hab              (EEPROM_PROG_relojCOMUN.varios4.Bits.viajesCD)


    // PARAMETROS DE RELOJ TARIFAS
    #define prgTARIFA_1D                  (EEPROM_PROG_relojT1D.tarifa)
    #define prgTARIFA_2D                  (EEPROM_PROG_relojT2D.tarifa)
    #define prgTARIFA_3D                  (EEPROM_PROG_relojT3D.tarifa)
    #define prgTARIFA_4D                  (EEPROM_PROG_relojT4D.tarifa)
    #define prgTARIFA_1N                  (EEPROM_PROG_relojT1N.tarifa)
    #define prgTARIFA_2N                  (EEPROM_PROG_relojT2N.tarifa)
    #define prgTARIFA_3N                  (EEPROM_PROG_relojT3N.tarifa)
    #define prgTARIFA_4N                  (EEPROM_PROG_relojT4N.tarifa)


    // PARAMETROS DE RELOJ EQ. PESOS
    #define EqPESOS_hab                   (EEPROM_PROG_relojEqPESOS.hab)
	#define EqPESOS_DIURNA                (EEPROM_PROG_relojEqPESOS.diurna)
    #define EqPESOS_NOCTURNA              (EEPROM_PROG_relojEqPESOS.nocturna)
   // #define PUNTO_DECIMAL_EqPesos         3


    // PARAMETROS DE RELOJ CALENDARIO
    #define CALENDARIO_getDIA(x)          (EEPROM_PROG_relojCALEND[x].dia)
    #define CALENDARIO_getMES(x)          (EEPROM_PROG_relojCALEND[x].mes)

/* VARIABLES EN EEPROM */
/***********************/
// LONGITUDES A GRABAR EN EEPROM - debe ser multiplo de 4
	#define EEPROMsize_MSJ                  152
	#define EEPROMsize_DIR                  256
	#define EEPROMsize_PRG_relojCOMUN       (sizeof(tPARAM_RELOJ_COMUNES))
	#define EEPROMsize_PRG_relojTARIFA      (sizeof(tPARAM_RELOJ_TARIFA))

	//#define EEPROMsize_PRG_relojEqPESOS     (sizeof(tPARAM_RELOJ_EQPESOS))
	#define EEPROMsize_PRG_relojEqPESOS		SIZE_PROG_relojEqPESOS
	#define EEPROMsize_PRG_relojCALEND      (sizeof(tPARAM_RELOJ_CALEND) * dim_Feriados)
	#define EEPROMsize_PRG_MOVIL            (sizeof(tPARAM_MOVIL))
	#define EEPROMsize_PRG_RADIO            (sizeof(tPARAM_RADIO))
	#define EEPROMsize_MSJ_PREGRABADO       60
	#define EEPROMsize_HORA_APAGADO         8
	#define EEPROMsize_PRG_TICKET           (sizeof(tPRG_TICKET))
    #define EEPROMsize_PRG_TICKET_RECAUD    (sizeof(tPRG_TICKET_RECAUD))
	#define EEPROMsize_PRG_GEO_FENCE        (sizeof(tPARAM_GEOFENCE))

   
  /* ESTRUCTURA */
  /**************/

#define cant_Feriados               20
#define dim_Feriados                (cant_Feriados + 2) // Como cada feriado son 2 bytes, le sumo 2 feriados mas (2 byte para CRC 2 byte para DF0A)


    // TIPO VIGENCIA
    typedef union{
        byte Byte;
      struct{
        byte cal        :1;             // CALENDARIO
        byte sab        :1;             // Sabado
        byte vie        :1;             // Viernes
        byte jue        :1;             // Jueves
        byte mie        :1;             // Miercoles
        byte mar        :1;             // Martes
        byte lun        :1;             // Lunes        
        byte dom        :1;             // Domingo
      }Bits;
    }tVIGENCIA;
  
   
    // HORARIO DE TARIFA
      enum{
        DIURNA,
        NOCTURNA
      };
   

    // TIPO DE DATOS TARIFA
    typedef struct{
      uint8_t  numero;                       // Numero de Tarifa
      uint8_t  diaNoche;                     // Horario de Tarifa: Diurna o Nocturna
      uint16_t tiempoGracia;                // [REMIS] Tiempo de Gracia -> Tiempo en que no se cuentan fichas por tiempo
      uint16_t bajadaBandera;				// Bajada Bandera -> Valor en Pesos de la Bajada de Bandera
      uint16_t distInicial;                 // Distancia Inicial -> Distancia de validez de la Bajada de Bandera
      uint16_t distFicha;                   // Distancia Ficha -> Distancia a Recorrer para que caiga una Ficha
      uint16_t valorFicha;                  // Valor de la Ficha -> Valor en Pesos de la Ficha
      uint16_t tiempoFicha;                 // Tiempo Ficha -> Tiempo a Esperar para que caiga una Ficha
      uint16_t valorFichaTiempo;            // Valor Ficha Tiempo -> Valor en Pesos de la Ficha que cae por Tiempo
      uint16_t horaInicio;                  // Hora de Inicio -> Hora a partir de la cual tiene vigencia la tarifa
      tVIGENCIA vigencia;                   // Vigencia -> Dias en los que la tarifa esta vigente, asi como tb indica si usa calendario o no
    }tTARIFA;



  typedef union {
    tTARIFA tTARIFA;
	struct {
        byte numero;                      // Numero de Tarifa
        byte diaNoche;                    // Horario de Tarifa: Diurna o Nocturna
        byte tiempoGraciaHI;                // [REMIS] Tiempo de Gracia -> Tiempo en que no se cuentan fichas por tiempo
        byte tiempoGraciaLO;                // [REMIS] Tiempo de Gracia -> Tiempo en que no se cuentan fichas por tiempo
        byte bajadaBanderaHI;				// Bajada Bandera -> Valor en Pesos de la Bajada de Bandera
        byte bajadaBanderaLO;				// Bajada Bandera -> Valor en Pesos de la Bajada de Bandera
        byte distInicialHI;                 // Distancia Inicial -> Distancia de validez de la Bajada de Bandera
        byte distInicialLO;                 // Distancia Inicial -> Distancia de validez de la Bajada de Bandera
        byte distFichaHI;                   // Distancia Ficha -> Distancia a Recorrer para que caiga una Ficha
        byte distFichaLO;                   // Distancia Ficha -> Distancia a Recorrer para que caiga una Ficha
        byte valorFichaHI;                  // Valor de la Ficha -> Valor en Pesos de la Ficha
        byte valorFichaLO;                  // Valor de la Ficha -> Valor en Pesos de la Ficha
        byte tiempoFichaHI;                 // Tiempo Ficha -> Tiempo a Esperar para que caiga una Ficha
        byte tiempoFichaLO;                 // Tiempo Ficha -> Tiempo a Esperar para que caiga una Ficha
        byte valorFichaTiempoHI;            // Valor Ficha Tiempo -> Valor en Pesos de la Ficha que cae por Tiempo
        byte valorFichaTiempoLO;            // Valor Ficha Tiempo -> Valor en Pesos de la Ficha que cae por Tiempo
        byte horaInicioHI;                  // Hora de Inicio -> Hora a partir de la cual tiene vigencia la tarifa
        byte horaInicioLO;                  // Hora de Inicio -> Hora a partir de la cual tiene vigencia la tarifa
        tVIGENCIA vigencia;               // Vigencia -> Dias en los que la tarifa esta vigente, asi como tb indica si usa calendario o no
	}TARIFA;
  }ttTARIFA;



    // PROGRAMACION DE RELOJ VARIOS 1
    typedef union{
      byte Byte;
      struct{
        byte T1Dhab          :1;        // Tarifa 1 Diurna Habilitada
        byte T2Dhab          :1;        // Tarifa 2 Diurna Habilitada
        byte T3Dhab          :1;        // Tarifa 3 Diurna Habilitada
        byte T4Dhab          :1;        // Tarifa 4 Diurna Habilitada
        byte tipo_sensorL    :1;        // Tipo Sensor Asiento Low
        byte tipo_sensorH    :1;        // Tipo Sensor Asiento High
        byte sensorSeg       :1;        // Sensor de asiento por segundos (en vez de mts)
        byte tipo_servicio   :1;        // 0: Taxi  - 1: Remis
      }Bits;
      
      struct{
        byte TD_hab          :4;        // Tarifas Diurnas Habilitadas
        byte tipo_sensor     :2;        // Tipo Sensor Asiento
      }MergedBits;
    }tRLJ_V1;
    
    
    // PROGRAMACION DE RELOJ VARIOS 2
    typedef union{
      byte Byte;
      struct{
        byte dif_hora0       :1;        // Diferencia Horaria LSB
        byte dif_hora1       :1;        // 
        byte dif_hora2       :1;        // Diferencia Horaria MSB
        byte sgn_difHora     :1;        // Signo de la correccion horaria
        byte pto_decimal0    :1;        // Punto Decimal LSB
        byte pto_decimal1    :1;        //
        byte pto_decimal2    :1;        // Punto Decimal MSB
        byte hora_source     :1;        // 0: GPS  - 1: RTC
      }Bits;
      
      struct{
        byte dif_hora        :3;        // Diferencia Horaria
        byte                 :1;
        byte pto_decimal     :3;        // Punto Decimal
      }MergedBits;
    }tRLJ_V2;
    
    
    // PROGRAMACION DE RELOJ VARIOS 3
    typedef union{
      byte Byte;
      struct{
        byte T1Nhab          :1;        // Tarifa 1 Nocturna Habilitada
        byte T2Nhab          :1;        // Tarifa 2 Nocturna Habilitada
        byte T3Nhab          :1;        // Tarifa 3 Nocturna Habilitada
        byte T4Nhab          :1;        // Tarifa 4 Nocturna Habilitada
        byte tecEspera       :1;        // Indica si esta habilitada o no la tecla de espera    
        byte NOacum_espera   :1;        // Indica si acumula espera (convierte a pulsos) o no
        byte NOespera        :1;        // Indica si esta habilitada la espera
        byte tarifacion      :1;        // 0: PESOS  - 1: FICHAS
      }Bits;
      
      struct{
        byte TN_hab          :4;        // Tarifas Nocturna Habilitadas
      }MergedBits;
    }tRLJ_V3;
    

    // PROGRAMACION DE RELOJ VARIOS 4
    typedef union{
      byte Byte;
      struct{
        byte choferes0       :1;        // Cantidad de Choferes LSB
        byte choferes1       :1;        // Cantidad de Choferes MSB
        byte                 :1;
        byte                 :1;
        byte deleteReportes  :1;        // Indica que se deben eliminar los reportes
        byte                 :1;
        byte tarifasManual   :1;        // Indica si esta permitida la seleccion manual de tarifas
        byte viajesCD        :1;        // Indica si estan habilitados o no los viajes con Chofer Desconocido
      }Bits;
      
      struct{
        byte cant_choferes   :2;        // Cantidad de Choferes
      }MergedBits;
    }tRLJ_V4;
    

    // TIPO DE DATOS EQUIVALENCIA EN PESOS
       typedef struct{
         uint16_t bajadaBandera;				// Equivalencia en Pesos de Bajada Bandera -> Valor en Pesos de la Bajada de Bandera
         uint16_t valorFicha;                  // Equivalencia en Pesos de Valor de la Ficha -> Valor en Pesos de la Ficha
         uint16_t valorFichaTiempo;            // Equivalencia en Pesos de Valor Ficha Tiempo -> Valor en Pesos de la Ficha que cae por Tiempo
       //  byte puntoDecimal;				//punto decimal
       }tEQPESOS;


     typedef union{
   	tEQPESOS tEQPESOS;
   	struct{
   		byte bajadaBanderaHI;			    // Equivalencia en Pesos de Bajada Bandera -> Valor en Pesos de la Bajada de Bandera
   		byte bajadaBanderaLO;			    // Equivalencia en Pesos de Bajada Bandera -> Valor en Pesos de la Bajada de Bandera
   		byte valorFichaHI;                  // Equivalencia en Pesos de Valor de la Ficha -> Valor en Pesos de la Ficha
   		byte valorFichaLO;                  // Equivalencia en Pesos de Valor de la Ficha -> Valor en Pesos de la Ficha
   		byte valorFichaTiempoHI;            // Equivalencia en Pesos de Valor Ficha Tiempo -> Valor en Pesos de la Ficha que cae por Tiempo
   		byte valorFichaTiempoLO;            // Equivalencia en Pesos de Valor Ficha Tiempo -> Valor en Pesos de la Ficha que cae por Tiempo
   		byte puntoDecimal;                   // punto decimal
   	}eqPesos;
     }ttEQPESOS;

    

    // TIPO DE DATOS DE PARAMETROS DE RELOJ KATE
    // debe tener un tamaño total multiplo de 4
    
    typedef struct{
      word valorFICHA1;				      // VALOR DE FICHA1: word(VF11 VF12) en hex 
      word valorFICHA2;             // VALOR DE FICHA2: word(VF21 VF22) en hex
      word valorFICHA3;             // VALOR DE FICHA3: word(VF22 VF31) en hex
      byte cambioPENDIENTE1;        // CAMBIO PENDIENTE1: byte(CP1) = (DISTANCIA DE CAMBIO1)/DISTANCIA FICHA  en hex
      byte cambioPENDIENTE2;        // CAMBIO PENDIENTE2: byte(CP2) = (DISTANCIA DE CAMBIO2)/DISTANCIA FICHA en hex
      byte empty[4];                // 2byte finEEPROM
    }tPARAM_RELOJ_KATE;
        

	typedef struct{
       uint8_t kmPRUEBA;        // Constante (pulsos x km)
     }tPARAM_prueba;


    // TIPO DE DATOS DE PARAMETROS DE RELOJ COMUN
    // debe tener un tamaño total multiplo de 4
    //
    //  +----|----|----|----++----|----|----|----+----+----|----|----+
    //  | constan |carr.band|| v1 | v2 | v3 | v4 |crc1|crc2| DF | 0A |
    //  +----|----|----|----++----|----|----|----+----|----+----|----|
    //
    // - constan ---> CONSTANTE
    // - carr.band -> CARRERA DE BANDERA
    // - v1 --------> VARIOS 1
    // - v2 --------> VARIOS 2
    // - v3 --------> VARIOS 3
    // - v4 --------> VARIOS 4

     typedef struct{
      uint16_t pulsosKm;        // Constante (pulsos x km)
      uint16_t carreraBandera;  // Carrera de Bandera en mts
      tRLJ_V1 varios1;      // Varios 1
      tRLJ_V2 varios2;      // Varios 2
      tRLJ_V3 varios3;      // Varios 3
      tRLJ_V4 varios4;      // Varios 4
      uint16_t checksum;     // nro que se DEBE incrementaR secuencialmente EN LA BASE DE DATOS con los cambios de tarifa (para saber que tarifa tiene grabado)
      uint16_t finDATA;     // chksum recibido por aire (igual a suma desde el comando hasta CP2 incluidos estos)
    }tPARAM_RELOJ_COMUNES;

    // TIPO DE DATOS DE PARAMETROS DE RELOJ TARIFA
    // debe tener un tamaño total multiplo de 4
    //
    //  +----|----|----|----++----|----|----|----++----|----|----|----++----|----|----|----++----|----|----|----++----|----|----|----+
    //  | Nº | DN | tGRACIA || bajBand | distIni || disFich | v.Ficha || t.Ficha | vtFicha || horaIni | vg |CHK1||CHK2| DF | 0A | xx |
    //  +----|----|----|----++----|----|----|----++----|----|----|----++----|----|----|----++----|----|----|----++----|----|----|----+
    //
    // - Nº --------> Numero
    // - DN --------> DIURNA / NOCTURNA
    // - tGRACIA ---> TIEMPO DE GRACIA
    // - bajBand ---> BAJADA DE BANDERA
    // - distIni ---> DISTANCIA INICIAL
    // - disFich ---> DISTANCIA FICHA
    // - v.Ficha ---> VALOR FICHA
    // - t.Ficha ---> TIEMPO FICHA
    // - vtFihca ---> VALOR FICHA TIEMPO
    // - horaIni ---> HORA DE INICIO
    // - vg --------> VIGENCIA

    typedef struct{
      tTARIFA tarifa;       // Tarifa
      uint16_t checksum;     //
      uint16_t finDATA;     //
      byte empty[1];        // 2byte DE CHECK Y 2BYTE  finEEPROM y tamaño multiplo de 4
    }tPARAM_RELOJ_TARIFA;
    
    // TIPO DE DATOS DE PARAMETROS DE RELOJ EQ. PESOS
    // debe tener un tamaño total multiplo de 4
    //
    //  +----|----|----|----++----|----|----|----++----|----|----|----++----|----|----|---++----|----+----+----+
    //  | hb | bajBand | v.Ficha  | t.Ficha | bajBand  | v.Ficha | t.Ficha  |CHK1|CHK2| DF|| 0A | xx | xx | xx |
    //  +----|----|----|----++----|----|----|----++----|----|----|----++----|----|----|---++----+----+----|----+
    //
    // Los primeros valores son diurnos, le siguen los nocturnos
    // - hb --------> HABILITACION DE EQUIVALENCIA EN PESOS
    // - bajBand ---> BAJADA DE BANDERA
    // - v.Ficha ---> VALOR FICHA
    // - t.Ficha ---> TIEMPO FICHA
    
    typedef struct{
     	  uint8_t hab;                   // Habilitacion
          tEQPESOS diurna;            	// Equivalencia Diurna
          tEQPESOS nocturna;          	// Equivalencia Nocturna
          uint16_t checksum;     		//
          uint16_t finDATA;     		//
          byte empty[3];        		// tamaño multiplo de 4
        }tPARAM_RELOJ_EQPESOS;

    
    // TIPO DE DATOS DE PARAMETROS DE RELOJ CALENDARIO
    // debe tener un tamaño total multiplo de 4
    //
    //  +----|----|----|----++----|----+----+----+
    //  | DD   MM |CHK1|CHK2|| DF   0A | xx | xx |  son <cant_Feriados> feriados, o sea <cant_Feriados> DD-MM
    //  +----|----|----|----++----|----+----+----+
    typedef struct{
      byte dia;                   // Habilitacion
      byte mes;                   // Equivalencia Diurna
    }tPARAM_RELOJ_CALEND;




    // TIPOS DE RELOJ (segun parametros de movil)
         typedef enum{
           PROTOCOLO,
           BANDERITA_LIBRE_ALTO,
           BANDERITA_OCUPADO_ALTO,
           INTERNO
         }tTIPORELOJ;

         typedef struct {
             	   tPARAM_RELOJ_COMUNES PROG_relojCOMUN;
             	   tPARAM_RELOJ_TARIFA  PROG_relojT1D;
             	   tPARAM_RELOJ_TARIFA  PROG_relojT2D;
             	   tPARAM_RELOJ_TARIFA  PROG_relojT3D;
             	   tPARAM_RELOJ_TARIFA  PROG_relojT4D;
             	   tPARAM_RELOJ_TARIFA  PROG_relojT1N;
             	   tPARAM_RELOJ_TARIFA  PROG_relojT2N;
             	   tPARAM_RELOJ_TARIFA  PROG_relojT3N;
             	   tPARAM_RELOJ_TARIFA  PROG_relojT4N;
             	   tPARAM_RELOJ_EQPESOS PROG_relojEqPESOS;
             	   tPARAM_RELOJ_CALEND  PROG_relojCALEND[dim_Feriados];
                   tPRG_TICKET 			PROG_TICKET;
                   tPRG_TICKET_RECAUD 	PROG_TICKET_RECAUD;
                   tPARAM_MOVIL			PROG_MOVIL;
         }tPROG;


	extern  byte EEPROM_HORA_APAGADO[];
	extern  tPARAM_prueba pepe;
	extern  tPARAM_RELOJ_COMUNES EEPROM_PROG_relojCOMUN;
	extern  tPARAM_RELOJ_TARIFA EEPROM_PROG_relojT1D;
	extern  tPARAM_RELOJ_TARIFA EEPROM_PROG_relojT2D;
	extern  tPARAM_RELOJ_TARIFA EEPROM_PROG_relojT3D;
	extern  tPARAM_RELOJ_TARIFA EEPROM_PROG_relojT4D;
	extern  tPARAM_RELOJ_TARIFA EEPROM_PROG_relojT1N;
	extern  tPARAM_RELOJ_TARIFA EEPROM_PROG_relojT2N;
	extern  tPARAM_RELOJ_TARIFA EEPROM_PROG_relojT3N;
	extern  tPARAM_RELOJ_TARIFA EEPROM_PROG_relojT4N;
	extern  tPARAM_RELOJ_EQPESOS EEPROM_PROG_relojEqPESOS;
	//extern  tPARAM_RELOJ_CALEND EEPROM_PROG_relojCALEND[];
	extern  tPARAM_RELOJ_CALEND EEPROM_PROG_relojCALEND[dim_Feriados];
#ifdef VISOR_PROG_TICKET
//	extern  const tPRG_TICKET EEPROM_PROG_TICKET;
//	extern  const tPRG_TICKET_RECAUD EEPROM_PROG_TICKET_RECAUD;
#endif


	//#define TIPO_RELOJ            ((tTIPORELOJ)(EEPROM_PROG_MOVIL.tipoReloj))
	    //#define TIPO_RELOJ			 INTERNO
	    //#define TIPO_RELOJ			 BANDERITA_LIBRE_ALTO

		//#define RELOJ_INTERNO       (TIPO_RELOJ == INTERNO)
	    //#define RELOJ_EXTERNO       (RELOJ_BANDERITA || RELOJ_PROTOCOLO)
	    //#define RELOJ_BANDERITA     ((TIPO_RELOJ == BANDERITA_LIBRE_ALTO) || (TIPO_RELOJ == BANDERITA_OCUPADO_ALTO))
	    //#define RELOJ_PROTOCOLO     (TIPO_RELOJ == PROTOCOLO)

	extern byte PROG_RELOJ_DATE_PC[6];              // Hora y Fecha recibida desde la PC (DIA-MES-AÑO-HORA-MIN-SEG)

  /* RUTINAS */
  /***********/      
    extern void prgRELOJ_ini (void);
    extern tEEPROM_ERROR prgRELOJ_reprogCOMUNES (word newK);
    extern byte PROG_determineDeleteReportes (void);
    
    extern void PROG_readRELOJkate (byte N);
    extern void PROG_writeRELOJkate (byte N);
    
    extern tEEPROM_ERROR checkProgTarifas(uint32_t progBlock);
    extern tEEPROM_ERROR readTarifa(byte nro, uint32_t addressTarifa);
    extern void PROG_readRELOJcomun (byte N);
    extern void PROG_writeRELOJcomun (byte N);
      extern void PROG_readRELOJ_T1D (byte N);
      extern void PROG_writeRELOJ_T1D (byte N);
      extern void PROG_readRELOJ_T2D (byte N);
      extern void PROG_writeRELOJ_T2D (byte N);
      extern void PROG_readRELOJ_T3D (byte N);
      extern void PROG_writeRELOJ_T3D (byte N);
      extern void PROG_readRELOJ_T4D (byte N);
      extern void PROG_writeRELOJ_T4D (byte N);
      extern void PROG_readRELOJ_T1N (byte N);
      extern void PROG_writeRELOJ_T1N (byte N);
      extern void PROG_readRELOJ_T2N (byte N);
      extern void PROG_writeRELOJ_T2N (byte N);
      extern void PROG_readRELOJ_T3N (byte N);
      extern void PROG_writeRELOJ_T3N (byte N);
      extern void PROG_readRELOJ_T4N (byte N);
      extern void PROG_writeRELOJ_T4N (byte N);

    extern void PROG_readRELOJcalendar (byte N);
    extern void PROG_writeRELOJcalendar (byte N);
    extern void PROG_readRELOJeqPesos (byte N);
    extern void PROG_writeRELOJeqPesos (byte N);


  /* EEPROM */
  /**********/
  // PARAMETROS COMUNES
    extern void PROG_RELOJcomun_to_EEPROM (byte iniGrabacion);
    extern tEEPROM_ERROR PROG_RELOJcomunes_grabarEEPROM (void);
  

  // TARIFAS
   // extern byte* PROG_RELOJtarifa_getEEPROM_ptr(byte nro);
    //extern byte* pruebaPROG_RELOJtarifa_getEEPROM_ptr(byte nro);
    extern byte* getDir_tarifaX_BlockProg(byte nro, byte* dirBlockProg);
  
    extern void PROG_RELOJtarifa_to_EEPROM (byte nro, byte iniGrabacion);
    extern tEEPROM_ERROR PROG_RELOJtarifa_grabarEEPROM (byte nro);
    //extern tEEPROM_ERROR PROG_RELOJtarifa_grabarEEPROM (byte nro, byte* addressTarifa);

      #define PROG_RELOJT1D_grabarEEPROM    PROG_RELOJtarifa_grabarEEPROM(tarifa1D)
      #define PROG_RELOJT2D_grabarEEPROM    PROG_RELOJtarifa_grabarEEPROM(tarifa2D)
      #define PROG_RELOJT3D_grabarEEPROM    PROG_RELOJtarifa_grabarEEPROM(tarifa3D)
      #define PROG_RELOJT4D_grabarEEPROM    PROG_RELOJtarifa_grabarEEPROM(tarifa4D)
      #define PROG_RELOJT1N_grabarEEPROM    PROG_RELOJtarifa_grabarEEPROM(tarifa1N)
      #define PROG_RELOJT2N_grabarEEPROM    PROG_RELOJtarifa_grabarEEPROM(tarifa2N)
      #define PROG_RELOJT3N_grabarEEPROM    PROG_RELOJtarifa_grabarEEPROM(tarifa3N)
      #define PROG_RELOJT4N_grabarEEPROM    PROG_RELOJtarifa_grabarEEPROM(tarifa4N)

    extern tTARIFA        prgRELOJ_getTarifa (byte nro);
    extern tTARIFA* prgRELOJ_getTarifa_pointer (byte nro);
    extern byte prgRELOJ_determineCantTarifasD_MANUAL (void);
    extern byte prgRELOJ_determineCantTarifasD_AUTOMATICA (void);
    extern byte prgRELOJ_determineCantTarifasN_AUTOMATICA (void);
    extern byte prgRELOJ_determineCantTarifasN_MANUAL (void);
    extern byte prgRELOJ_determineTarifaHab (byte nroTarifa);
      
  
#define longitudFECHA_CALENDARIO		2  //1 un byte dia un byte mes
#define ADDRESS_PROG_relojCALEND0   (ADDRESS_PROG_relojCALEND + 0*longitudFECHA_CALENDARIO)
#define ADDRESS_PROG_relojCALEND1   (ADDRESS_PROG_relojCALEND + 1*longitudFECHA_CALENDARIO)
#define ADDRESS_PROG_relojCALEND2   (ADDRESS_PROG_relojCALEND + 2*longitudFECHA_CALENDARIO)
#define ADDRESS_PROG_relojCALEND3   (ADDRESS_PROG_relojCALEND + 3*longitudFECHA_CALENDARIO)
#define ADDRESS_PROG_relojCALEND4   (ADDRESS_PROG_relojCALEND + 4*longitudFECHA_CALENDARIO)
#define ADDRESS_PROG_relojCALEND5   (ADDRESS_PROG_relojCALEND + 5*longitudFECHA_CALENDARIO)
#define ADDRESS_PROG_relojCALEND6   (ADDRESS_PROG_relojCALEND + 6*longitudFECHA_CALENDARIO)
#define ADDRESS_PROG_relojCALEND7   (ADDRESS_PROG_relojCALEND + 7*longitudFECHA_CALENDARIO)
#define ADDRESS_PROG_relojCALEND8   (ADDRESS_PROG_relojCALEND + 8*longitudFECHA_CALENDARIO)
#define ADDRESS_PROG_relojCALEND9   (ADDRESS_PROG_relojCALEND + 9*longitudFECHA_CALENDARIO)
#define ADDRESS_PROG_relojCALEND10  (ADDRESS_PROG_relojCALEND + 10*longitudFECHA_CALENDARIO)
#define ADDRESS_PROG_relojCALEND11  (ADDRESS_PROG_relojCALEND + 11*longitudFECHA_CALENDARIO)
#define ADDRESS_PROG_relojCALEND12  (ADDRESS_PROG_relojCALEND + 12*longitudFECHA_CALENDARIO)
#define ADDRESS_PROG_relojCALEND13  (ADDRESS_PROG_relojCALEND + 13*longitudFECHA_CALENDARIO)
#define ADDRESS_PROG_relojCALEND14  (ADDRESS_PROG_relojCALEND + 14*longitudFECHA_CALENDARIO)
#define ADDRESS_PROG_relojCALEND15  (ADDRESS_PROG_relojCALEND + 15*longitudFECHA_CALENDARIO)
#define ADDRESS_PROG_relojCALEND16  (ADDRESS_PROG_relojCALEND + 16*longitudFECHA_CALENDARIO)
#define ADDRESS_PROG_relojCALEND17  (ADDRESS_PROG_relojCALEND + 17*longitudFECHA_CALENDARIO)
#define ADDRESS_PROG_relojCALEND18  (ADDRESS_PROG_relojCALEND + 18*longitudFECHA_CALENDARIO)
#define ADDRESS_PROG_relojCALEND19  (ADDRESS_PROG_relojCALEND + 19*longitudFECHA_CALENDARIO)
#define ADDRESS_PROG_relojCALEND20  (ADDRESS_PROG_relojCALEND + 20*longitudFECHA_CALENDARIO)
#define ADDRESS_PROG_relojCALEND21  (ADDRESS_PROG_relojCALEND + 21*longitudFECHA_CALENDARIO)

  // EQUIVALENCIA EN PESOS
    extern void PROG_RELOJeqPesos_to_EEPROM (byte iniGrabacion);
    extern tEEPROM_ERROR PROG_RELOJeqPesos_grabarEEPROM (void);
    

  // CALENDARIO
    extern void PROG_RELOJcalend_to_EEPROM (byte iniGrabacion);
    extern tEEPROM_ERROR PROG_RELOJcalend_grabarEEPROM (void);

    extern void read_progRELOJ_eeprom(void);
    extern void read_TICKET_eeprom(uint8_t* buffer_backup);
    extern void read_TICKET_RECAUD_eeprom(uint8_t* buffer_backup);
    extern void read_relojCALEND_eeprom(uint8_t* buffer_backup);
    extern void read_relojEqPESOS_eeprom(uint8_t* buffer_backup);
    extern void read_relojT4N_eeprom(uint8_t* buffer_backup);
    extern void read_relojT3N_eeprom(uint8_t* buffer_backup);
    extern void read_relojT2N_eeprom(uint8_t* buffer_backup);
    extern void read_relojT1N_eeprom(uint8_t* buffer_backup);
    extern void read_relojT4D_eeprom(uint8_t* buffer_backup);
    extern void read_relojT2D_eeprom(uint8_t* buffer_backup);
    extern void read_relojT1D_eeprom(uint8_t* buffer_backup);
    extern void read_relojCOMUN_eeprom(uint8_t* buffer_backup);
    extern tEEPROM_ERROR chkCRC_EnEEPROM(uint32_t addrEEPROM, uint16_t longTOread);
    extern void levantar_progRELOJ (void);
    extern void levantar_progTICKET (void);
    extern void readProgRELOJ (uint32_t blockProg);



  /* AIR UPDATE */
  /**************/
    extern void PROG_saveRELOJ_COMUN_air (byte* data, byte N);
    extern byte PROG_writeRELOJ_COMUN_air (byte N);    
    extern void PROG_saveRELOJ_TARIFA_air (byte* data, byte N);
    extern byte PROG_writeRELOJ_TARIFA_air (byte N);
    extern void PROG_saveRELOJ_EqPESOS_air (byte* data, byte N);
    extern byte PROG_writeRELOJ_EqPESOS_air (byte N);    
    extern void PROG_saveRELOJ_CALEND_air (byte* data, byte N);
    extern byte PROG_writeRELOJ_CALEND_air (byte N);    


 #endif


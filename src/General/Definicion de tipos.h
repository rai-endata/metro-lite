#ifndef _TYPES_DEF_
 #define _TYPES_DEF_

  typedef unsigned char byte;
  typedef unsigned int 	word;
  typedef unsigned long dword;
  typedef unsigned long dlong[2];

//#define NULL                   ((void *) 0)

  /*********************************************************************************************/
  /* ESTRUCTURAS */
  /**************/

	// GEO PUNTO
	typedef struct{
		byte lat[4];                      // Latitud: 3bytes + N/S
		byte lon[4];					  // Latitud: 3bytes + E/W
	}tGeoPOINT;

	// FECHA y HORA
	typedef struct{
		byte fecha[4];                    // Fecha (diaMes,Mes,Año,DiaSemana)
		byte hora[3];					  // Hora  (Hora, minutos,segundos)
	}tDATE;

	// FECHA y HORA
	typedef struct{
		byte fecha[4];                    // Fecha (diaMes,Mes,Año,DiaSemana)
		byte hora[3];					  // Hora  (Hora, minutos,segundos)
		byte auxPAR;					  //para completar longitud par
	}tDATE_flash;

	//GPS
	typedef struct{
		tGeoPOINT Pos;      			  // Posicion: Latitud y Longitud
		tDATE Date;                       // Date: Fecha y Hora
		byte valid;												// 1:Valido - 0:Invalido
		byte vel;                         // Velocidad de GPS en km/h
		word course;                      // Rumbo
	}tGPS;






	#endif

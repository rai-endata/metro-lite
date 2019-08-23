/* File: <Tarifa Automatica.h> */

#ifndef _TARIFA_AUTOMATICA_
  #define _TARIFA_AUTOMATICA_
 
  /* INCLUDES */
  /************/
    //#include "- Visor Config -.h"
    //#include "Programacion Parametros.h"
 // #include "Param Reloj.h"

// #include "Grabaciones en EEPROM.h"
  //#include "Grabaciones en EEPROM.h"
//  #include "Parametros Reloj.h"
 #include "Parametros Reloj.h"

/* RUTINAS */
  /***********/
    extern byte TARIFA_AUTO_getNroTarifa (void);
    
    extern tTARIFA* determineTarifaDiurnaNocturna (tDATE dateActual, tTARIFA* TD, tTARIFA* tN);
#endif

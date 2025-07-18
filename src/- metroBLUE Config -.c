/*
 * - metroBLUE Config -.c
 *
 *  Created on: 13/11/2019
 *      Author: Rai
 */

#include  "- metroBLUE Config -.h"

/* ******** Versionamiento Semantico **************************
    vX.Y.Z

	X: Major, version_major	((uint16_t)0x01)  //se incrementa cuando cambia compatibilidad
	y: Major, version_minor	((uint16_t)0x00)  //se incrementa cuando cambia funcionalidad vuelve a cero cuando se incrementa x
	Z: Major, 								  //se incrementa cuando se arregla algun/os errores, vuelve a cero cuando se incrementa x o y
    */




char versionFIRMWARE[] =  "v2.2.4";
char fechaFIRMWARE[] =  " (17/07/2025)";

// Version v2.2.4 -16/07/2025
//* Otra Corrige comando pactado - error en punto decimal


// Version v2.2.3 -16/07/2025
//* Corrige comando pactado - error en punto decimal

// Version v2.2.2 -10/07/2025
//* Corrige comando pactado


// Version v2.2.0 -07/07/2025
//* agrega comando PACTADO EN OCUPADO (normal 34 sin conexion)


// Version v2.1.6 -12/12/2024
// * resuelve un problema de tx de ocupado sin conexion
// * guardado que no enviaba
// * resuelve problema que en ocupado, despues de un corte corto
//   se perdia los km recorridos


//Version v2.1.5 -12/11/2024

//error al leer programacion de ticket
//y algunos errores en datos de impresion de ticket
//luego de un microcorte en ocupado y cobrando



// guarda cambios de reloj sin conexion en eeprom
// correcion errores varios en corte largo y microcorte
// correccion de velocidad


//Version v2.1.3 - 09/09/24
//error en ticket recaudacion

//Version v2.1.2 - 28/08/24
//  Corrige  error que se da en el arranque despues que la eeprom ha sido borrada totalmente



//version v2.1.1 - 28/04/2024
//En la version anterior cuando se programaba desde el programador
//se guardababa los datos en los cuatro sectores de eeprom, esto demoraba la
//respuesta al programador y este daba error por timeout. Para sortear este inconveniente
//cuando se programa se guarda solo en un sector, y cuando se inicia el equipo se guarda en los restantes sectores.


//version v2.1.0
//Ya no hay programacion por defecto
//crea 4 sectores de programacion para programacion de reloj y Movil y tickets
//todavia no recupera los tckets si hay error en algun sector, si lo hace con reloj y movil


//version v2.0.3
//reordena direcciones de eeprom


//version v2.0.2
//amplia el parche agregado a la recudacion total en la verson v2.0.0
//al viaje total
//modificacion de funciones TURNO_getCantViajesAsiento_turno, TURNO_getCantErroresAsiento_turno()
// y TURNO_calcKmTotales_turno() para tener en cuento los errores de timeout y error de rango de direcciones.
//(el error de recudacion total y viaje total se habia dado en versiones anteriores debido a un timeout (TO_F))

//version v2.0.1
//fusion con rama agregaMiniblue en el que se realizo los sgtes cambios:
//agrega un parche para corregir error que a veces
//se dio al sumar las recaudaciones en el reporte de turno.

//version v2.0.0
//agrega tipo de equipo miniblue
//PB6=0, PB7=1

//version v1.9.0
//  * Actualiza hora y fecha, desde la central con el comando transparente
//  * No mira el crc de programacion del movil (parche temporal, hasta encontrar error en ccr de programaciopn del movil

//version v1.8.6
//   * Actualiza hora y fecha, desde la central con el comando transparente
//  * No mira el crc de programacion del movil (parche temporal, hasta encontrar error en ccr de programaciopn del movil)

//version v1.8.5
//imprime ticket en libre

//verion v1.8.4
//fusion de 1.8.3 con 1.8.2.1

//verion v1.8.3
//fusion de 1.8.2 con 1.8.2.A

//verion v1.8.2.A
//parche reintento infinito de libre, porque la app esta en ocupado
//el reloj en libre, la app queda reintentando el pase a cobrando
//entonces le envio un pase a cobrando trucho para que se pase a cobrando
//y luego se pueda pasar a libre
//manda dato de ficha o pesos con el valor 0x55
//para que la central sepa que es un comando trucho


//v1.8.2.1
//version que agrega kml y kmo en ticket parcial hecho para la app que esta haciendo santi

//v1.8.2
//cambia la distancia del pase por sensor de asiento de cobrando a libre, de 60 a 30 metros.
//en encendido de equipo agrega la version usando 3 bytes (estaba revision en 2 bytes, y estaba fija en 0001)
//Ahora llamo funcion check_pressBLUETOOTH() en el arranque; ademas de hacerlo cuando se produce
//un flanco en el pin de bluethooth
//cuando recibo cambios de reloj con conecccion levanto la bandera appConectada_ACentral
//cuando inicia la app ya no envio la version (Tx_respVersion_Equipo())
//pongo una pausa entre comando y comando de 150 mseg.


//v1.8.1
//arregla el error de desbordamiento en la variable Distanciam
//1.8.0
//Agrega datos del titular al ticket de turno

//v1.7.1
//Si el equipo estaba en cobrando y
//pasaba a libre por movimiento enviaba a la app pase a libre
//y no tenia que hacerlo, tenia que  guardar el comando hasta que recupere la conexion.
//Lo mismo ocurria con el pase a ocupado con sensor de asiento, si estaba sin conexion,
//enviaba el pase a la app, y tenia que guardarlo hasta que se recupere la conexion.


//v1.7.0
// corrige problema de programacion (verdura1 y verdura2 no estaban inicializados)
// Cambiar frecuencia de valor de viaje
// 	cuando cambia de tarifa
// 	cuando cambia minutos
// 	cuando cambia 100 metros

// Ante una consulta de estado de la app, si esta ocupado enviar tambien valor de viaje.
// y en todos los estados enviar la version del metrolite.
// en la funcion pasarCMDS_BUFFER_to_TxBUFFER comenta la sgte: //clear_TxPendiente();


//v1.6.0
// Agrega programacion por defecto

//1.5.4
//Antes de pasar los datos recibidos del programador
//a la eeprom, verifica que el tipo de reloj sea valido
//sino, no guarda datos a la eeprom y se trunca la programacion.
//La primera vez que se programa, si no es parametros de movil el tipo de reloj es invalido
//por lo que no programa y requiere que se programe primero el tipo de reloj.
//En la sgte version le saque esa restriccion y se puede programar todo y en cualquier orden.

//v1.5.3
//no envia valor de viaje cuando no tiene conexion de bluetooth
//no se queda enviando valor de viaje en cobrando
//deja de reintentar comando status
//no reintenta valor de viaje


//v1.5
//agrega cantidad de viajes pendientes no enviados por falta de conexion a los comandos de reloj
//mensaje sincronizando, cuando intenta hacer un pase de reloj y tiene comandos sin conexion guardados
//Cuando recupera conexion envia comandos acumulados de reloj sin conexion

//v1.x.x
//asignado
//asignado doble
//cerrar sesion en libre y volver abrir la app
//cerrar sesion en descanso y volver abrir la app
//salir de la app en descanso y volver abrir la app
//cerrar sesion y pasar a ocupado por sensor de asiento volver a abrir la app
//que pueda tarifar mas de 99999


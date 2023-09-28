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

//micro stm32f091rc
char versionFIRMWARE[] =  "v1.8.3";
char fechaFIRMWARE[] =  " (28/09/2023)";


//verion v1.8.3
//fusion de 1.8.2 con 1.8.2.A


verion v1.8.2.A
parche reintento infinito de libre, porque la app esta en ocupado
el reloj en libre, la app queda reintentando el pase a cobrando
entonces le envio un pase a cobrando trucho para que se pase a cobrando
y luego se pueda pasar a libre
manda dato de ficha o pesos con el valor 0x55
para que la central sepa que es un comando trucho


//v1.8.2
//cambia la distancia del pase por sensor de asiento de cobrando a libre, de 60 a 30 metros.
//en encendido de equipo agrega la version usando 3 bytes (estaba revision en 2 bytes, y estaba fija en 0001)
//Ahora llamo funcion check_pressBLUETOOTH() en el arranque; ademas de hacerlo cuando se produce
//un flanco en el pin de bluethooth
//cuando recibo cambios de reloj con conecccion levanto la bandera appConectada_ACentral
//cuando inicia la app ya no envio la version (Tx_respVersion_Equipo())
//pongo una pausa entre comando y comando de 150 mseg.


//v1.8.1
//arregla el error de desbordamiento en la variable

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


/*
 * gps.c
 *
 *  Created on: 4/10/2017
 *      Author: Rai
 */
#include "gps.h"
#include "Definicion de tipos.h"


/*
y el de Greenwich, se expresa en grados, minutos y segundos de arco y se mide de 0 a 180
La longitud (lambda) es la distancia angular entre el meridiano de un lugar
o hacia el Este o hacia el Oeste desde el meridiano de Greeenwich.
La latitud (w) es la distancia angular entre el paralelo de un lugar y el
Ecuador, se expresa en las mismas unidades que la longitud y se mide de 0 a 90o
hacia el Norte o el Sur. En ocasiones la latitud y longitud se expresan en grados y décimas de grado
en lugar de en grados, minutos y segundos.

Un grado de meridiano equivale siempre a 111 kilómetros, mientras que un grado de paralelo equivale
a 111cos(w), es decir a 111 kilómetros en el Ecuador disminuyendo hasta 0 kilómetros en los polos9.
 *
 * */
tGPS  GPS;


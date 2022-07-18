#ifndef ALGORITMOS_PAG
#define ALGORITMOS_PAG

#include <stdlib.h>
#include <commons/collections/list.h>
#include "./uso_memoria.h"
#include "./crear_estructuras.h"

/*
    Dada una entrada devuelve si bit de uso es == 0
    -En caso de que sea == 1: lo modifica
*/
int criterio_clock(entrada_tabla_N2 *e);

/*
    Dada una entrada devuelve si cumple con:
        -Primer vuelta: bit uso == 0 && bit modificacion == 0
        -Segunda vuelta: bit uso == 0 && bit modificacion == 1
            (cambia el valor de bit de uso en caso negativo)
        -Terver vuelta: bit uso == 0 && bit modificacion == 0
        -Cuarta vuelta: bit uso == 0 && bit modificacion == 1
*/
int criterio_clock_mejorado(entrada_tabla_N2 *e, int vuelta);

/*
    Aplica criterio clock
    NOTA: siempre devuelve una entrada NO nula
*/
entrada_tabla_N2* aplicar_busqueda_clock(int id, int dir_tablaN1);

/*
    Aplica criterio clock mejorado
    NOTA: siempre devuelve una entrada NO nula
*/
entrada_tabla_N2* aplicar_busqueda_clock_mejorado(int id, int dir_tablaN1);

#endif
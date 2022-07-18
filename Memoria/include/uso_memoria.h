#ifndef USO_MEMORIA
#define USO_MEMORIA

#include <stdio.h>
#include <commons/bitarray.h>
#include "./config.h"
#include "./crear_estructuras.h"
#include "./algoritmos_paginas.h"
#include "./Disco.h"

void escribir_memoria(uint32_t dato, uint32_t dir);
uint32_t leer_memoria(uint32_t dir);
void traer_pagina_a_memoria(int id, int dir_tablaN1 ,entrada_tabla_N2 *e);


#endif
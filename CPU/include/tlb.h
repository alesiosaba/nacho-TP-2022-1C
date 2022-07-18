#ifndef TLB
#define TLB

#include <stdlib.h>
#include <commons/collections/list.h>
#include "../include/config.h"
#include "../../shared/include/shared_utils.h"

typedef struct
{
    int num_pag;
    int marco;
}entrada_tlb;

t_list *tlb;

void iniciar_estructuras();
void agregar_entrada_tlb(int num_pag, int dir_marco);
int buscar_pagina_tlb(int pag);
/*
    NOTA: los criterios son LRU y FIFO
    -En el caso de FIFO solo se elimina el primer elemento (el primero que fue agregado)
    y se agrega el nuevo al final (ultimo en llegar)
    -En el caso de LRU se reordena la lista cada vez que hay un TLB HIT
    (se referencia una entrada que esta en la tlb) por lo que ambas funciones
    reemplazan la pagina de la misma forma
*/
void reemplazar_pagina(entrada_tlb *e);
void limpiar_tlb();

#endif
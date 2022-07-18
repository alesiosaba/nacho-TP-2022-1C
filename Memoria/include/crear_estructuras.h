#ifndef CREAR_ESTRUCTURAS
#define CREAR_ESTRUCTURAS

#include <stdlib.h>
#include <math.h>
#include <semaphore.h>
#include <commons/collections/list.h>
#include <commons/bitarray.h>
#include <pthread.h>
#include "./config.h"
#include "../../shared/include/shared_utils.h"
#include "./uso_memoria.h"

typedef struct
{
    int id_proceso;
	int posicion_puntero_clock;
	t_tablaN1 *tablaN1;
    t_list *marcos_reservados; //tipo int
    int esta_suspendido;
    sem_t suspension_completa;
}proceso_en_memoria;

t_list *tablasN1, *tablasN2;
t_list *procesos_en_memoria;

pthread_mutex_t mutex_tablasN1;
pthread_mutex_t mutex_tablasN2;
pthread_mutex_t mutex_procesos_en_memoria;

//MEMORIA PRINCIPAL (espacio usuario)
void *memoria;

void iniciar_estructuras_memoria();

//BITMAP (cuales marcos estan vacios)
t_bitarray *marcos_memoria;

proceso_en_memoria* asignar_proceso(int id, int tamanio_proceso);
proceso_en_memoria* buscar_proceso_por_id(int id);
void eliminar_estructura_proceso(int id);
/* 
    Crea una entrada de tablaN1
    -Define num_tabla
    -No define dir (valor -1)
*/
entrada_tabla_N1* agregar_entrada_tablaN1(t_tablaN1 *tabla);

/*
    Igual a crear entrada tablaN1 pero para tablaN2:
    -Valores de bits inician en 0
    -dir = -1
    -num_pagina = -1
*/
entrada_tabla_N2* agregar_entrada_tablaN2(t_tablaN2 *tabla);
int cantidad_paginas_necesarias(float tamanio_proceso);
int cantidad_tablas_necesarias(int paginas_necesarias);

/*
    Crea tabla de nivel 1 con las tablas de nivel 2 necesarias
    -Guarda las tablas de nivel 2 en la lista general 'tablasN2'
    -Asigna los num_tabla para las tablas de nivel 2 y los num_pagina
    -Asigna la dir de las tablas de nivel 2
    -No asigna las dir de las paginas
    -No guarda la tabla de nivel 1 en la lista general

    NOTA: las listas de tablas se eliminan todas juntas al final del proceso memoria
    ya que, si se eliminan antes, cambiaria las direcciones de todas
    las tablas porque cambiaria el tamanio de la lista general
*/
t_tablaN1* crear_tablaN1(int tamanio_proceso);

/*
    Libera las paginas
*/
void eliminar_paginas_proceso(int id, int dir_tablaN1);

/*
    Asigna memoria al puntero (inicializa memoria principal)
*/
void iniciar_memoria(void *mem, int tamanio_total);

/*
    Crea bitmap a partir del tamanio de la memoria y del tamanio de la pagina
    -Todos los bits comienzan en valor 1 
    NOTA: si el valor de un bit es 1 entonces el marco esta libre
    NOTA2: hay 1 byte perdido en el malloc de la variabl char *bitmap
*/
t_bitarray* crear_bitmap(int tamanio_memoria);

/*
    funcion para imprimir un logger del estado actual de un bitmap
*/
void imprimir_bitmap(t_bitarray *bitmap);

/*
    Otra forma de llamar a bitarray_destroy()
*/
void eliminar_bitmap(t_bitarray *bitmap);

/*
    Si la tabla contiene una pagina con bit presencia == 1 y dir == marco
        retorna entrada de esa pagina
    Si no
        retorna NULL
*/
entrada_tabla_N2* tabla_contiene_marco(t_tablaN2 *t, int num_marco);


/*
    Busca la pagina que ocupa un numero de marco
    NOTA: no se considera el caso en que la funcion sea llamada para un
    marco vacio
*/
entrada_tabla_N2* conseguir_pagina_en_marco(int num_marco);

/*
    Devuelve info de pagina dada una tabla nivel 1 y numero de pagina
*/
entrada_tabla_N2* conseguir_entrada_pagina(int dir_tablaN1, int pag);

/*
    Devuelve lista de paginas que estan en memoria de un proceso
*/
t_list* conseguir_marcos_proceso(int dir_tablaN1);

t_list* conseguir_numeros_marcos_proceso(int id);


void reservar_marcos_proceso(proceso_en_memoria *p);
void liberar_marcos_proceso(int id);
void desmarcar_bitmap(t_list *marcos);
proceso_en_memoria* buscar_proceso_por_id(int id);
void eliminador_proceso(proceso_en_memoria *p);

//funcion auxiliar
void eliminar_lista(void *l);
void crear_listas_tablas();
void eliminar_listas_tablas();
int conseguir_marco_de_dir_fisica(int dir);
#endif
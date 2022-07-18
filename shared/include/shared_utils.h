#ifndef SHARED_UTILS_H
#define SHARED_UTILS_H

#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>
#include "./shared_log.h"
#include "./shared_config.h"
#include <commons/collections/list.h>

typedef struct rango_tiempo_t {
  struct timeval inicio;
  struct timeval fin;
} rango_tiempo_t;

typedef enum {
    NUEVO,
    LISTO,
    EJECUTANDO,
    BLOQUEADO,
    BLOQUEADO_SUSPENDIDO,
    SUSPENDIDO_LISTO,
	TERMINADO
}cod_estado;

typedef struct {
	uint32_t id;
	uint32_t tam_proceso; //(En bytes)
	cod_estado estado;
	t_list* instrucciones;
	uint32_t program_counter; //Indice de la lista
	uint32_t tabla_paginas; //indice a lista general de tablas nivel 1
	uint32_t estimacion_anterior;
	uint32_t ultima_rafaga;
	int prox_rafaga_estimada;
	uint32_t tiempo_a_bloquearse;
	uint32_t tiempo_bloqueado;
	rango_tiempo_t* rafaga;
	rango_tiempo_t* rafaga_bloqueado;
} t_pcb;

typedef struct {
	int num_pag;
	int dir; //MARCO
	int bit_presencia;
	int bit_uso;
	int bit_modificacion;
}entrada_tabla_N2;

typedef struct {
	int num_tabla; //id tabla2
	int dir; //index en lista general de tablasN2
} entrada_tabla_N1;

typedef t_list t_tablaN2; //elementos tipo entrada_tabla_N2
typedef t_list t_tablaN1; //elementos tipo entrada_tabla_N1

void finalizar_programa(t_log*, t_config*);

#endif
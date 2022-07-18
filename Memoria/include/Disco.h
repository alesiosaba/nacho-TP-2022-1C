#ifndef HEADERS_DISCO_H_
#define HEADERS_DISCO_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
//#include <conio.h>
#include <sys/types.h>
#include <semaphore.h>
#include <commons/collections/queue.h>
#include "./config.h"
#include "./server.h"
#include "./crear_estructuras.h"
#include "../../shared/include/shared_log.h"
#include "../../shared/include/shared_utils.h"
#include "../../shared/include/shared_mensajes.h"
#include "../../shared/include/shared_servidor.h"

enum OPERACIONES_DISCO
{
    CREAR_ARCHIVO,
    ESCRIBIR_ARCHIVO,
    LEER_ARCHIVO,
    DISCO_SUSPENDER_PROCESO,
    ELIMINAR_ARCHIVO
};

typedef struct t_pedido_disco
{
    int operacion_disco;
    int argumentos[3];
    sem_t pedido_listo;
}t_pedido_disco;

t_queue *pedidos_disco;
sem_t lista_tiene_pedidos;
pthread_mutex_t mutex_cola_pedidos;


void generar_nuevo_archivo(int);
void eliminar_archivo(int);
char* devolver_path_archivo(int);
void escribir_en_archivo(int PID, int direccion_pagina, int pagina);
void enviar_pagina_a_memoria(int PID, int direccion_pagina, int pagina);
void suspender_paginas(int PID, int dir_tablaN1);
void funciones_disco();

t_pedido_disco* crear_pedido_crear_archivo(int id);
t_pedido_disco* crear_pedido_escribir(int id, int dir_marco, int num_pag);
t_pedido_disco* crear_pedido_lectura(int id, int dir_marco, int num_pag);
t_pedido_disco* crear_pedido_eliminar_archivo(int id);
t_pedido_disco* crear_pedido_suspender_proceso(int id, int dir_marco);
void eliminar_pedido_disco(t_pedido_disco *p);

#endif /* HEADERS_DISCO_H_ */
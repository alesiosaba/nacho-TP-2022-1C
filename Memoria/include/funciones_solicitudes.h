#ifndef FUNCIONES_SOLICITUDES
#define FUNCIONES_SOLICITUDES

#include <stdio.h>
#include <semaphore.h>
#include "./crear_estructuras.h"
#include "./uso_memoria.h"
#include "./Disco.h"
#include "../../shared/include/shared_mensajes.h"

void inicializar_estructuras(int socket_cliente, t_log *logger);
void suspender_proceso(int socket_cliente, t_log *logger);
void desuspender_proceso(int socket_cliente, t_log *logger);
void pedido_lectura(int socket_cliente, t_log *logger);
void pedido_escritura(int socket_cliente, t_log *logger);
void solicitud_tabla_paginas(int socket_cliente, t_log *logger);
void solicitud_marco(int socket_cliente, t_log *logger);
void eliminar_proceso(int socket_cliente, t_log *logger);
#endif
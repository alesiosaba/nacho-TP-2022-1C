#ifndef SERVER
#define SERVER

#include <stdio.h>
#include <stdbool.h>
#include "./config.h"
#include "./procesos.h"
#include "../../shared/include/shared_servidor.h"
#include "../../shared/include/shared_mensajes.h"
#include <commons/collections/list.h>

void escuchar_procesos_nuevos();
void atender_procesos_nuevos(void*);

#endif
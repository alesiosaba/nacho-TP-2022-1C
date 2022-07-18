#ifndef SERVER
#define SERVER

#include <stdio.h>
#include <stdbool.h>
#include "./config.h"
#include "../../shared/include/shared_servidor.h"
#include "../../shared/include/shared_mensajes.h"
#include "./funciones_solicitudes.h"
#include <commons/collections/list.h>

void servidor();
void atender_peticiones(void*);

#endif
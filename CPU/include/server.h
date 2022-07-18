#ifndef SERVER
#define SERVER

#include <stdio.h>
#include <stdbool.h>
#include "./config.h"
#include "../include/ejecucion_procesos.h"
#include "../../shared/include/shared_utils.h"
#include "../../shared/include/shared_servidor.h"
#include "../../shared/include/shared_mensajes.h"
#include <commons/collections/list.h>

void servidor_interrupt();
void servidor_dispatch();
void atender_interrupciones(void*);
void atender_pcb_para_ejecutar(void*);

#endif
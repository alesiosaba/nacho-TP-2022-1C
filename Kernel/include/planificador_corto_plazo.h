#ifndef HEADERS_PLANIFICADOR_CORTO_PLAZO_H_
#define HEADERS_PLANIFICADOR_CORTO_PLAZO_H_

#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>
#include "./config.h"
#include "./procesos.h"
#include "../../shared/include/shared_log.h"
#include "../../shared/include/shared_utils.h"
#include "../../shared/include/shared_servidor.h"
#include "../../shared/include/shared_mensajes.h"
#include <commons/collections/list.h>

void iniciar_planificador_corto_plazo();
void planificar_procesos();
bool algoritmo_es_srt();
void recibir_pcb_luego_de_ejecutar(int);

#endif /* HEADERS_PLANIFICADOR_CORTO_PLAZO_H_ */
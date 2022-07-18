#ifndef HEADERS_PLANIFICADOR_MEDIANO_PLAZO_H_
#define HEADERS_PLANIFICADOR_MEDIANO_PLAZO_H_

#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>
#include "./config.h"
#include "./procesos.h"
#include "../../shared/include/shared_log.h"
#include "../../shared/include/shared_utils.h"
#include <commons/collections/list.h>
#include "../include/shared_mensajes.h"

void iniciar_planificador_mediano_plazo();
void suspender_procesos();

#endif /* HEADERS_PLANIFICADOR_MEDIANO_PLAZO_H_ */
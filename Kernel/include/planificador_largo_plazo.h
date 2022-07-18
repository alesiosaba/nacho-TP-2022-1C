#ifndef HEADERS_PLANIFICADOR_LARGO_PLAZO_H_
#define HEADERS_PLANIFICADOR_LARGO_PLAZO_H_

#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>
#include "./config.h"
#include "./procesos.h"
#include "../../shared/include/shared_log.h"
#include "../../shared/include/shared_utils.h"
#include "../../shared/include/shared_servidor.h"
#include <commons/collections/list.h>

void iniciar_planificador_largo_plazo();
void controlar_grado_de_multiprogramacion();
void mostrar_grado_multiprogramacion_actual();

#endif /* HEADERS_PLANIFICADOR_LARGO_PLAZO_H_ */
#ifndef HEADERS_DISPOSITIVO_IO_H_
#define HEADERS_DISPOSITIVO_IO_H_

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

void iniciar_dispositivo_io();
void atender_procesos();

#endif /* HEADERS_DISPOSITIVO_IO_H_ */
#ifndef EJECUCION_PROCESOS
#define EJECUCION_PROCESOS

#include <stdio.h>
#include <stdbool.h>
#include "./config.h"
#include "../../shared/include/shared_log.h"
#include "../../shared/include/shared_utils.h"
#include "../../shared/include/shared_servidor.h"
#include "../../shared/include/shared_mensajes.h"
#include "./mmu.h"

void realizar_ciclo_de_instruccion(t_pcb*, int); 
t_instruccion* buscar_proxima_instruccion(t_pcb*); 
void interpretar_instruccion_y_ejecutar_pcb(t_instruccion*, t_pcb*, int); 
int hay_interrupcion_para_atender();
void destruir_proceso(t_pcb* pcb);

#endif
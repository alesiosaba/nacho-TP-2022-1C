#ifndef CONFIG
#define CONFIG

#include "../../shared/include/shared_config.h"
#include "../../shared/include/shared_log.h"

void procesar_archivo_config_kernel(t_config*); 

t_log* logger;
char* ip_memoria;
char* puerto_memoria;
char* ip_cpu;
char* puerto_cpu_dispatch;
char* puerto_cpu_interrupt;
char* ip_escucha;
char* puerto_escucha;
char* algoritmo_planificacion;
int estimacion_inicial;
double alfa;
int grado_multiprogramacion;
int tiempo_max_bloqueado; 

#endif
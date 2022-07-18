#ifndef CONFIG
#define CONFIG

#include "../../shared/include/shared_config.h"
#include "../../shared/include/shared_log.h"

void procesar_archivo_config_consola(t_config*); 

t_log* logger_consola;
char* ip_kernel;
char* puerto_kernel;

#endif
#include "../include/config.h"

void procesar_archivo_config_consola(t_config* config_consola) {
    
  ip_kernel = config_get_string_value(config_consola, "IP_KERNEL");
	puerto_kernel = config_get_string_value(config_consola, "PUERTO_KERNEL");
}
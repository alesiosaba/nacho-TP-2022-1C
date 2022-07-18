#include "../include/config.h"

void procesar_archivo_config_kernel(t_config* config_consola) {
    
    ip_memoria = config_get_string_value(config_consola, "IP_MEMORIA");
    puerto_memoria = config_get_string_value(config_consola, "PUERTO_MEMORIA");
    ip_cpu = config_get_string_value(config_consola, "IP_CPU");
    puerto_cpu_dispatch = config_get_string_value(config_consola, "PUERTO_CPU_DISPATCH");
    puerto_cpu_interrupt = config_get_string_value(config_consola, "PUERTO_CPU_INTERRUPT");
    ip_escucha = config_get_string_value(config_consola, "IP_ESCUCHA");
    puerto_escucha = config_get_string_value(config_consola, "PUERTO_ESCUCHA");
    algoritmo_planificacion = config_get_string_value(config_consola, "ALGORITMO_PLANIFICACION");
    estimacion_inicial = config_get_int_value(config_consola, "ESTIMACION_INICIAL");
    alfa = config_get_double_value(config_consola, "ALFA");
    grado_multiprogramacion = config_get_int_value(config_consola, "GRADO_MULTIPROGRAMACION");
    tiempo_max_bloqueado = config_get_int_value(config_consola, "TIEMPO_MAXIMO_BLOQUEADO");
}
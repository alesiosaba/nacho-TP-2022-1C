#include "../include/config.h"

void procesar_archivo_config_memoria(t_config* config_consola) {
    
    ip_escucha = config_get_string_value(config_consola, "IP_ESCUCHA");
    puerto_escucha = config_get_string_value(config_consola, "PUERTO_ESCUCHA");
    tam_memoria = config_get_int_value(config_consola, "TAM_MEMORIA");
    tam_pagina = config_get_int_value(config_consola, "TAM_PAGINA");
    paginas_por_tabla = config_get_int_value(config_consola, "PAGINAS_POR_TABLA");
    retardo_memoria = config_get_int_value(config_consola, "RETARDO_MEMORIA");
    algoritmo_reemplazo = config_get_string_value(config_consola, "ALGORITMO_REEMPLAZO");
    marcos_por_proceso = config_get_int_value(config_consola, "MARCOS_POR_PROCESO");
    retardo_swap = config_get_int_value(config_consola, "RETARDO_SWAP");
    path_swap = config_get_string_value(config_consola, "PATH_SWAP");
}
#include "../include/config.h"

void procesar_archivo_config_cpu(t_config* config_consola) {
    
    ip_escucha = config_get_string_value(config_consola, "IP_ESCUCHA");
    puerto_escucha_dispatch = config_get_string_value(config_consola, "PUERTO_ESCUCHA_DISPATCH");
    puerto_escucha_interrupt = config_get_string_value(config_consola, "PUERTO_ESCUCHA_INTERRUPT");
    ip_memoria = config_get_string_value(config_consola, "IP_MEMORIA");
    puerto_memoria = config_get_string_value(config_consola, "PUERTO_MEMORIA");
    entradas_tlb = config_get_int_value(config_consola, "ENTRADAS_TLB");
    reemplazo_tlb = config_get_string_value(config_consola, "REEMPLAZO_TLB");
    retardo_noop = config_get_int_value(config_consola, "RETARDO_NOOP");
    //AGREGADOS
    tam_pagina = config_get_int_value(config_consola, "TAM_PAGINA");
    paginas_por_tabla = config_get_int_value(config_consola, "PAGINAS_POR_TABLA");
}
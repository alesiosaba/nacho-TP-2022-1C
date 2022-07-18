#include "../include/server.h"

void servidor_interrupt() {

    int socket_servidor = iniciar_servidor(logger_CPU, "CPU", ip_escucha, puerto_escucha_interrupt);
    log_info(logger_CPU, "Servidor CPU Interrupciones iniciado");

    pthread_t hilo_interrupt;

    while(1) 
    {
        int conexion = esperar_cliente(socket_servidor);
        pthread_create(&hilo_interrupt, NULL, (void*) atender_interrupciones, (void*) conexion);
        pthread_detach(hilo_interrupt);
    }

    close(socket_servidor);
}

void atender_interrupciones(void* conexion) {
    int una_conexion = (int) conexion;
    log_info(logger_CPU, "Cliente conectado");
    int op_code = recibir_operacion(una_conexion);

    switch(op_code)
    {
        case INTERRUPCION:
            log_info(logger_CPU, "Petición recibida: INTERRUPCION"); 
            flag_interrupcion = 1;

            break;

        default:
            log_error(logger_CPU, "El OP_CODE recibido es inválido");            
    }

    liberar_conexion(una_conexion);

    log_info(logger_CPU, "El cliente se ha desconectado");
}

void servidor_dispatch() {

    int socket_servidor = iniciar_servidor(logger_CPU, "CPU", ip_escucha, puerto_escucha_dispatch);
    log_info(logger_CPU, "Servidor CPU Dispatch iniciado");

    pthread_t hilo_dispatch;

    while(1) 
    {
        int conexion = esperar_cliente(socket_servidor);
        pthread_create(&hilo_dispatch, NULL, (void*) atender_pcb_para_ejecutar, (void*) conexion);
        pthread_detach(hilo_dispatch);
    }

    close(socket_servidor);
}

void atender_pcb_para_ejecutar(void* conexion) {
    int una_conexion = (int) conexion;
    log_info(logger_CPU, "Cliente conectado");
    int op_code = recibir_operacion(una_conexion);

    switch(op_code)
    {
        case RECIBIR_PCB:

            log_info(logger_CPU, "Petición recibida: RECIBIR_PCB");

            t_pcb* pcb = recibir_pcb(una_conexion, logger_CPU);

            realizar_ciclo_de_instruccion(pcb, una_conexion);
            
            break;

        default:
            log_error(logger_CPU, "El OP_CODE recibido es inválido");
    }

    //liberar_conexion(una_conexion);

    log_info(logger_CPU, "El cliente se ha desconectado");
}
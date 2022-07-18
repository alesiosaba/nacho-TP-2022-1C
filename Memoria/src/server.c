#include "../include/server.h"

void servidor() {
    int socket_servidor = iniciar_servidor(logger, "MEMORIA", ip_escucha, puerto_escucha);
    log_info(logger, "Servidor Memoria iniciado");

    pthread_t hilo_escucha;

    while(1) 
    {
        int conexion = esperar_cliente(socket_servidor);
        pthread_create(&hilo_escucha, NULL, (void*) atender_peticiones, (void*) conexion);
        pthread_detach(hilo_escucha);
    }

    close(socket_servidor);
}

void atender_peticiones(void* conexion) {
    int una_conexion = (int) conexion;
    log_info(logger, "Cliente conectado");
    int op_code = recibir_operacion(una_conexion);
    
    //TIEMPO RETARDO MEMORIA
    usleep(retardo_memoria *1000);

    switch(op_code)
    {
        case ENVIAR_HANDSHAKE:
            log_info(logger, "Peticion recibida: ENVIAR_HANDSHAKE"); 

            break;

        case PEDIDO_LECTURA:
            log_info(logger, "Peticion recibida: PEDIDO_LECTURA");
            pedido_lectura(una_conexion, logger);
            break;

        case PEDIDO_ESCRITURA:
            log_info(logger, "Peticion recibida: PEDIDO_ESCRITURA");
            pedido_escritura(una_conexion, logger);
            break;

        case SOLICITUD_TABLA_PAGINAS: 
            log_info(logger, "Peticion recibida: SOLICITUD_TABLA_PAGINAS");
            solicitud_tabla_paginas(una_conexion, logger);
            break;

        case SOLICITUD_MARCO:
            log_info(logger, "Peticion recibida: SOLICITUD_MARCO");
            solicitud_marco(una_conexion, logger);
            break;

        case INICIALIZAR_ESTRUCTURAS:
            log_info(logger, "Peticion recibida: INICIALIZAR_ESTRUCTURAS");
            inicializar_estructuras(una_conexion, logger);
            break;

        case SUSPENDER_PROCESO:
            log_info(logger, "Peticion recibida: SUSPENDER_PROCESO");
            suspender_proceso(una_conexion, logger);
            break;

        case DESUSPENDER_PROCESO:
            log_info(logger, "Peticion recibida: DESUSPENDER_PROCESO");
            desuspender_proceso(una_conexion, logger);
            break;

        case EXIT:
            log_info(logger, "Peticion recibida: EXIT");
            eliminar_proceso(una_conexion, logger);
            break;

        default: 
            log_error(logger, "El OP_CODE recibido es invalido");
            log_info(logger, "Op code recibido: %d", op_code);
            break;
    }

    close(una_conexion);
    log_info(logger, "El cliente se ha desconectado");
}
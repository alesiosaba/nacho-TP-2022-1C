#include "../include/server.h"

static char* codigos_instrucciones[6] = {
    "NO_OP",
    "I/O",
    "READ",
    "WRITE",
    "COPY",
    "EXIT"
};

void escuchar_procesos_nuevos() {
    int socket_servidor = iniciar_servidor(logger, "KERNEL", ip_escucha, puerto_escucha);
    log_info(logger, "Servidor Kernel iniciado");

    pthread_t atender_consola;

    while(1) 
    {
        int conexion_consola = esperar_cliente(socket_servidor);
        pthread_create(&atender_consola, NULL, (void*) atender_procesos_nuevos, (void*) conexion_consola);
        pthread_detach(atender_consola);
    }

    close(socket_servidor);
}

void atender_procesos_nuevos(void* conexion) {
    
    log_debug(logger, "Cliente conectado");
    int conexion_consola = (int) conexion;
    int op_code = recibir_operacion(conexion_consola);

    switch(op_code)
    {
        case NUEVO_PROCESO:

            log_info(logger, "Petición recibida: NUEVO_PROCESO");

            uint32_t tam_proceso;

            t_list* instrucciones = recibir_lista_instrucciones_y_tam_proceso(conexion_consola, &tam_proceso, logger);

            t_pcb* pcb_nuevo = crear_proceso(conexion_consola, tam_proceso, instrucciones);
            encolar_proceso_en_nuevos(pcb_nuevo);

            list_destroy(instrucciones);

            break;

        default: 
            log_error(logger, "El OP_CODE recibido es inválido");
            break;
    }

    //close(conexion_consola); La conexión se debe cerrar cuando se elimina el proceso,
    //                         dado que se almacena en el PCB.
    //log_info(logger, "El cliente se ha desconectado");
}
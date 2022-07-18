#include "../include/mensajes_kernel.h"

void gestionar_mensajes (t_paquete* paquete_recibido, int socket_cliente, t_log* logger) {
    switch(paquete_recibido->codigo_operacion) {

        case NUEVO_PROCESO:
            log_info(logger,"Llego el mensaje NUEVO_PROCESO");
            break;

        case EXIT:
            log_info(logger, "Llego el mensaje EXIT");
            break;
        
        case IO:
            log_info(logger, "Llego el mensaje de entrada salida");
            break;
        
        case DESALOJAR_PROCESO:
            log_info(logger, "Llego el mensaje de DESALOJAR_PROCESO");
            break;

        case ACTUALIZAR_PCB:
            log_info(logger, "Llego el mensaje ACTUALIZAR_PCB");
            break;

        default:
            log_error(logger, "Hubo problemas con gestionar_mensajes");
            break;
    }
}
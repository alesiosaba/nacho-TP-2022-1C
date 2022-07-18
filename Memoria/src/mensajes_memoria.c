#include "../include/mensajes_memoria.h"

void gestionar_mensaje (t_paquete* paquete_recibido, int socket_cliente, t_log* logger) {

		switch(paquete_recibido->codigo_operacion) {

			case ENVIAR_HANDSHAKE:
				log_info(logger, "Se recibió un handshake");
				break;

			case INICIALIZAR_ESTRUCTURAS:
				log_info(logger, "Se recibio el mensaje INICIALIZAR_ESTRUCTURAS");
				
				
				break;

			case SUSPENDER_PROCESO:
				log_info(logger,"Se recibió el mensaje SUSPENDER_PROCESO");
				break;

			case PEDIDO_LECTURA:
				log_info(logger,"Se recibió el mensaje PEDIDO_LECTURA");
				break;

			case PEDIDO_ESCRITURA:
				log_info(logger,"Se recibió el mensaje PEDIDO_ESCRITURA");
				break;

			case PEDIDO_COPIA:
				log_info(logger,"Se recibió el mensaje PEDIDO_COPIA");
				break;

			case SOLICITUD_TABLA_PAGINAS:
				log_info(logger,"Se recibió el mensaje SOLICITUD_TABLA_PAGINAS");
				break;

			case SOLICITUD_MARCO:
				log_info(logger,"Se recibió el mensaje SOLICITUD_MARCO");
				break;

			case SOLICITUD_DIRECCION_FISICA:
				log_info(logger,"Se recibió el mensaje SOLICITUD_DIRECCION_FISICA");
				break;

			default:
				log_error(logger, "default de gestionarMensajes");
				break;
		}
}
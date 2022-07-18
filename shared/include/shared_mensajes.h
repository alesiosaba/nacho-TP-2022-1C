#ifndef MENSAJES
#define MENSAJES

#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include "./shared_utils.h"
#include <commons/log.h>
#include <commons/collections/list.h>

typedef struct {
    int op;
    int arg[2];
}t_instruccion;

typedef enum {
	MENSAJE,
	PAQUETE,
	//FINALIZAR_PROCESO,
	NUEVO_PROCESO,
	RECIBIR_PCB,
	INTERRUPCION,
	EXIT,
	IO,
	ACTUALIZAR_PCB,
	DESALOJAR_PROCESO,
	INICIALIZAR_ESTRUCTURAS,
	SUSPENDER_PROCESO,
	DESUSPENDER_PROCESO,
	ENVIAR_HANDSHAKE,
	PEDIDO_LECTURA,
	PEDIDO_ESCRITURA,
	PEDIDO_COPIA,
	SOLICITUD_TABLA_PAGINAS,
	SOLICITUD_MARCO,
	SOLICITUD_DIRECCION_FISICA
}op_code;

typedef enum {
    NO_OP,
    I_O,
    READ,
    WRITE,
    COPY,
    EXIT_INST
}cod_instruccion;

typedef struct {
	int size;
	void* stream;
} t_buffer;


typedef struct {
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

//ESTRUCTURAS
void crear_buffer(t_paquete* paquete);
t_paquete* crear_paquete(op_code);

//ENVIAR
int enviar_num(int coneccion, int num, t_log *logger);
int recibir_num(int coneccion, t_log* logger);
void enviar_valor_leido(uint32_t dato, int socket_cliente, t_log* logger);
uint32_t recibir_valor_leido(int socket_cliente, t_log* logger);
void* serializar_paquete(t_paquete* paquete, int bytes);
void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio);
void enviar_paquete(t_paquete* paquete, int socket_cliente, t_log *logger);
void eliminar_paquete(t_paquete* paquete);
void enviar_mensaje(char* mensaje, int socket_cliente, t_log *logger);

void enviar_lista_instrucciones_y_tam_proceso(op_code cod_op, t_list* instrucciones, u_int32_t tam_proceso, int socket_cliente, t_log* logger);
void* serializar_lista_instrucciones_y_tam_proceso(op_code cod_op, t_list* instrucciones, u_int32_t tam_proceso, size_t* size, t_log* logger);

void enviar_pcb(op_code, t_pcb*, int, t_log *);
void* serializar_pcb(op_code, t_pcb*, size_t*, t_log* logger);
void enviar_pcb_con_tiempo_bloqueo(op_code cod_op, t_pcb* pcb, uint32_t tiempo_bloqueo, int socket_cliente, t_log* logger);
void* serializar_pcb_con_tiempo_bloqueo(op_code cod_op, t_pcb* pcb, uint32_t tiempo_bloqueo, size_t* size, t_log* logger);
void enviar_interrupcion(int socket_cliente, t_log* logger);

//RECIBIR
int recibir_operacion(int);
void* recibir_buffer(int* size, int socket_cliente, t_log *logger);
void recibir_mensaje(int socket_cliente, t_log *logger);
t_list* recibir_paquete(int socket_cliente, t_log *logger);
t_list* recibir_lista_instrucciones_y_tam_proceso(int socket_cliente, uint32_t *tam_proceso, t_log* logger);
t_list* deserializar_lista_instrucciones_y_tam_proceso(void* stream, uint32_t *tam_proceso);
t_pcb* recibir_pcb(int, t_log*);
t_pcb* deserializar_pcb(void*);
t_pcb* recibir_pcb_con_tiempo_bloqueo(int socket_cliente, t_log* logger);
t_pcb* deserializar_pcb_con_tiempo_bloqueo(void* stream);


void* serializar_tabla_N2(op_code op, t_tablaN2 *t, size_t *size);
void enviar_tabla_N2(int socket_cliente, t_tablaN2 *t, t_log *logger);
t_list* deserializar_tabla_N2(void *stream, int tamanio_lista);
t_tablaN2* recibir_tabla_N2(int socket_cliente, t_log *logger);
void enviar_pedido_escritura(uint32_t dir, uint32_t dato, int conexion, t_log *logger);
void* serializar_pedido_escritura(uint32_t dir, uint32_t dato, size_t *size, t_log* logger);
t_list* recibir_pedido_escritura(int conexion, t_log *logger);
t_list *deserializar_pedido_escritura(void *buffer);
void enviar_pedido_lectura(uint32_t dir, int conexion, t_log *logger);
void* serializar_pedido_lectura(uint32_t dir, size_t *size, t_log* logger);
uint32_t recibir_pedido_lectura(int conexion, t_log *logger);
uint32_t deserializar_pedido_lectura(void *buffer);
void* serializar_pedido_liberar_memoria(op_code cod_op, uint32_t id, uint32_t dir_tabla, size_t *tamanio_buffer, t_log *logger);
void enviar_pedido_liberar_memoria(op_code cod_op, int conexion, uint32_t id, uint32_t dir_tabla, t_log *logger);
t_list* deserializar_pedido_liberar_memoria(void *stream);
t_list* recibir_pedido_liberar_memoria(int conexion, t_log *logger);
void* serializar_pedido_desuspender_proceso(int id, size_t *tamanio_buffer);
void enviar_pedido_desuspender_proceso(int conexion, int id, t_log *logger);
int deserializar_pedido_desuspender_proceso(void *buffer);
int recibir_pedido_desuspender_proceso(int conexion, t_log *logger);

#endif
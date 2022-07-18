#include "../include/shared_mensajes.h"

//NOTA:
//Los enteros que se envian estan en formato 'int', por ahi hay que cambiarlos a 'uint32_t'

int enviar_num(int conexion, int num, t_log *logger)
{
    //send() ENVIA POR conexión EL num Y RETORNA LA CANTIDAD DE BYTES ENVIADOS
	int ret = send(conexion, &num, sizeof(uint32_t), 0);
	if(ret < sizeof(uint32_t))
		log_error(logger, "Error al enviar numero");

	return ret;
}

int recibir_num(int conexion, t_log* logger)
{
    //recv() RETORNA -1 SI EN CASO DE ERROR
	int num;
	if(recv(conexion, &num, sizeof(int), MSG_WAITALL) > 0)
		return num;
	else
	{
        log_error(logger, "Error al recibir numero");
		return -1;
	}
}

void enviar_valor_leido(uint32_t dato, int socket_cliente, t_log* logger)
{
	size_t size;

	op_code cod_op = PEDIDO_LECTURA;

 	size = sizeof(op_code) +
	 		sizeof(size_t) +
	 		sizeof(uint32_t);

	size_t size_payload = size - sizeof(op_code) - sizeof(size_t);
	log_info(logger, "Size payload: %zu", size_payload);

	void* stream = malloc(size);
	int desplazamiento = 0;

    memcpy(stream, &cod_op, sizeof(op_code));
	desplazamiento += sizeof(op_code);

	memcpy(stream + desplazamiento, &size_payload, sizeof(size_t));
	desplazamiento += sizeof(size_t);

	memcpy(stream + desplazamiento, &dato, sizeof(size_t));
	desplazamiento += sizeof(size_t);
	
	if(send(socket_cliente, stream, size, 0) != size)
		log_error(logger, "Los datos no se enviaron correctamente");

	free(stream);
}

uint32_t recibir_valor_leido(int socket_cliente, t_log* logger)
{
 	size_t size;
    if (recv(socket_cliente, &size, sizeof(size_t), 0) != sizeof(size_t)) {
        log_error(logger, "Los datos no se recibieron correctamente"); 
    }

    void* stream = malloc(size);
	log_debug(logger, "Size payload: %zu", size);

    if (recv(socket_cliente, stream, size, 0) != size) {
		log_error(logger, "Los datos no se recibieron correctamente");
    }

	uint32_t dato = 0;

	memcpy(&dato, stream, sizeof(uint32_t));
	free(stream);

	return dato;
}

/*-------------------------------*/
void* serializar_paquete(t_paquete* paquete, int bytes)
{
	//Inicia puntero vacio
	void * total_paquete = malloc(bytes);
	int desplazamiento = 0;

	//Copia codigo de operacion (la primer parte del mensaje que va a llegar)
	memcpy(total_paquete + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	desplazamiento+= sizeof(int);
	
	//Copia tamanio del paquete
	memcpy(total_paquete + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento+= sizeof(int);
	
	//Copia contenido del paquete
	memcpy(total_paquete + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento+= paquete->buffer->size;

	return total_paquete;
}

void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio)
{
	//'Agranda' el paquete para agregar un valor
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio + sizeof(int));

	//Copia el tamanio del valor
	memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio, sizeof(int));
	
	//Copia el valor
	memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(int), valor, tamanio);

	//Actualiza el tamanio del paquete (se agrego dato y tamanio del dato)
	paquete->buffer->size += tamanio + sizeof(int);
}

void enviar_paquete(t_paquete* paquete, int socket_cliente, t_log *logger)
{
	//Total mensaje = buffer + tamanio buffer + codigo operacion
	int bytes = paquete->buffer->size + 2*sizeof(int);
	
	//Transforma de t_paquete a *void
	void* a_enviar = serializar_paquete(paquete, bytes);

	//Envia datos
	if(send(socket_cliente, a_enviar, bytes, 0) <= 0)
		log_error(logger, "Datos no se enviaron correctamente");

	//Liberar datos
	free(a_enviar);
}

void eliminar_paquete(t_paquete* paquete)
{
	//Primer eliminar *void del paquete
	free(paquete->buffer->stream);

	//Despues el puntero que guardaba la estructura t_buffer
	free(paquete->buffer);

	//Despues el puntero que guardaba a t_paquete
	free(paquete);
}

void enviar_mensaje(char* mensaje, int socket_cliente, t_log *logger)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));

	//Definir codigo de operacion
	paquete->codigo_operacion = MENSAJE;
	
	//Iniciar estructura buffer del paquete
	paquete->buffer = malloc(sizeof(t_buffer));
	
	//Tamanio = largo mensaje + caracter '\0'
	paquete->buffer->size = strlen(mensaje) + 1;
	
	//Reservar espacio para mensaje
	paquete->buffer->stream = malloc(paquete->buffer->size);
	
	//Copiar mensaje en buffer
	memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size);

	//Mensaje total = mensaje + tamanio + codigo operacion
	int bytes = paquete->buffer->size + 2*sizeof(int);

	//De paquete a *void
	void* a_enviar = serializar_paquete(paquete, bytes);
	send(socket_cliente, a_enviar, bytes, 0);
	/*
	int check;
	if( (check = send(socket_cliente, a_enviar, bytes, 0)) <= 0)
		log_error(logger, "Mensaje no se envio correctamente");
	*/
	free(a_enviar);
	eliminar_paquete(paquete);
}


void crear_buffer(t_paquete* paquete)
{
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = 0;
	paquete->buffer->stream = NULL;
}

t_paquete* crear_paquete(op_code cod_op)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = cod_op;
	crear_buffer(paquete);
	return paquete;
}

/*-----------------------------------*/

int recibir_operacion(int socket_cliente)
{
	int cod_op;
	if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) != 0)
		return cod_op;
	else
	{
		close(socket_cliente);
		return -1;
	}
}

void* recibir_buffer(int* size, int socket_cliente, t_log *logger)
{
	/*void* buffer;

	//Recibir tamanio de buffer
	if(recv(socket_cliente, size, sizeof(int), MSG_WAITALL) <= 0)
		log_error(logger, "Error al recibir tamanio buffer");
	
	//Asignar espacio en base a tamanio
	buffer = malloc(*size);
	
	//Recibir buffer
	if(recv(socket_cliente, buffer, *size, MSG_WAITALL) <= 0)
		log_error(logger, "Error al recibir buffer");

	return buffer;*/

	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}

void recibir_mensaje(int socket_cliente, t_log *logger)
{
	int size;
	char* buffer = recibir_buffer(&size, socket_cliente, logger);
	log_info(logger, "Mensaje recibido: \"%s\"", buffer);
	free(buffer);
}

t_list* recibir_paquete(int socket_cliente, t_log *logger)
{
	int size;
	int desplazamiento = 0;
	void * buffer;
	t_list* valores = list_create();
	int tamanio;

	//Recibe tamanio del paquete y el paquete mismo
	buffer = recibir_buffer(&size, socket_cliente, logger);
	
	//Mientras: cantidad datos < tamanio paquete
	while(desplazamiento < size)
	{
		//Copia tamanio de dato
		memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
		
		//Actualiza: se recibio un entero
		desplazamiento+=sizeof(int);
		
		//Asigna espacio segun tamanio recibido
		void* valor = malloc(tamanio); 
		
		//Copia valor recibido
		memcpy(valor, buffer+desplazamiento, tamanio);
		
		//Actualiza: recibio un dato
		desplazamiento+=tamanio;

		//Enlistar dato
		list_add(valores, valor);
		//free(valor);
	}

	free(buffer);
	return valores;
	return NULL;
}

void enviar_pcb(op_code cod_op, t_pcb* pcb, int socket_cliente, t_log* logger) {

	size_t size;
	log_debug(logger, "ID antes de serializar: %u", pcb->id);
	log_debug(logger, "TAM antes de serializar: %u", pcb->tam_proceso);
	log_debug(logger, "Program_counter antes de serializar: %u", pcb->program_counter);
	log_debug(logger, "Estimacion_anterior antes de serializar: %u", pcb->estimacion_anterior);
	log_debug(logger, "Ultima_rafaga antes de serializar: %u", pcb->ultima_rafaga);
	void* stream = serializar_pcb(cod_op, pcb, &size, logger);
	if(send(socket_cliente, stream, size, 0) != size)
		log_error(logger, "Los datos no se enviaron correctamente");

	log_info(logger, "Tamaño stream: %d", *(int*) stream);
	free(stream);
}

void* serializar_pcb(op_code cod_op, t_pcb* pcb, size_t* size, t_log* logger) {

	int length_lista = list_size(pcb->instrucciones);
	size_t size_instrucciones = sizeof(t_instruccion) * length_lista;
	log_debug(logger, "Size instrucciones: %zu", size_instrucciones);

 	*size = sizeof(op_code) +
	 		sizeof(size_t) +
			size_instrucciones + //tamanio lista de instrucciones
			sizeof(t_instruccion) * length_lista + //tamanio de cada instruccion
	 		sizeof(uint32_t) * 6;
			//sizeof(rango_tiempo_t);

	size_t size_payload = *size - sizeof(op_code) - sizeof(size_t);
	log_debug(logger, "Size payload: %zu", size_payload);

	void* stream = malloc(*size);
	int desplazamiento = 0;
	int desplazamiento_lista = 0;
	int indice = 0;

    memcpy(stream, &cod_op, sizeof(op_code));
	desplazamiento += sizeof(op_code);
	memcpy(stream + desplazamiento, &size_payload, sizeof(size_t));
	desplazamiento += sizeof(size_t);
	memcpy(stream + desplazamiento, &pcb->tam_proceso, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(stream + desplazamiento, &pcb->id, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(stream + desplazamiento, &pcb->program_counter, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(stream + desplazamiento, &pcb->estimacion_anterior, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(stream + desplazamiento, &pcb->ultima_rafaga, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(stream + desplazamiento, &pcb->tabla_paginas, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	
	memcpy(stream + desplazamiento, &size_instrucciones, sizeof(size_t));
	desplazamiento += sizeof(size_t);
	
	while(desplazamiento_lista < size_instrucciones)
	{
		t_instruccion* instruccion = (t_instruccion*) list_get(pcb->instrucciones, indice);
		memcpy(stream + desplazamiento + desplazamiento_lista, instruccion, sizeof(t_instruccion));
		desplazamiento_lista += sizeof(t_instruccion);
		indice ++;
	}

    return stream;
}

t_pcb* recibir_pcb(int socket_cliente, t_log* logger) {

 	size_t size;
    if (recv(socket_cliente, &size, sizeof(size_t), 0) != sizeof(size_t)) {
        log_error(logger, "Los datos no se recibieron correctamente"); 
    }

    void* stream = malloc(size);
	log_debug(logger, "Size payload: %zu", size);

    if (recv(socket_cliente, stream, size, 0) != size) {
		log_error(logger, "Los datos no se recibieron correctamente");
    }

    t_pcb* pcb = deserializar_pcb(stream);

	log_debug(logger, "El ID del PCB recibido es: %d", pcb->id);
	log_debug(logger, "El tamaño del PCB recibido es: %d", pcb->tam_proceso);
	log_debug(logger, "Program_counter antes de serializar: %u", pcb->program_counter);
	log_debug(logger, "Estimacion_anterior antes de serializar: %u", pcb->estimacion_anterior);
	log_debug(logger, "Ultima_rafaga antes de serializar: %u", pcb->ultima_rafaga);

    free(stream); 
	return pcb;
}

t_pcb* deserializar_pcb(void* stream) {
	
	t_pcb* pcb = malloc(sizeof(t_pcb));
    pcb->rafaga = malloc(sizeof(rango_tiempo_t));
    pcb->rafaga_bloqueado = malloc(sizeof(rango_tiempo_t));

	size_t size_instrucciones;
	int desplazamiento = 0;
	int desplazamiento_lista = 0;

	memcpy(&pcb->tam_proceso, stream, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(&pcb->id, stream + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(&pcb->program_counter, stream + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(&pcb->estimacion_anterior, stream + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(&pcb->ultima_rafaga, stream + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(&pcb->tabla_paginas, stream + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(&size_instrucciones, stream + desplazamiento, sizeof(size_t));
	desplazamiento += sizeof(size_t);
	
	pcb->instrucciones = list_create();

	while(desplazamiento_lista < size_instrucciones)
	{
		t_instruccion* instruccion = malloc(sizeof(t_instruccion));
		memcpy(instruccion, stream + desplazamiento + desplazamiento_lista, sizeof(t_instruccion));
		list_add(pcb->instrucciones, instruccion);
		desplazamiento_lista += sizeof(t_instruccion);
	}

	return pcb;
}

void enviar_pcb_con_tiempo_bloqueo(op_code cod_op, t_pcb* pcb, uint32_t tiempo_bloqueo, int socket_cliente, t_log* logger) {

	size_t size;
	log_debug(logger, "ID antes de serializar: %u", pcb->id);
	log_debug(logger, "TAM antes de serializar: %u", pcb->tam_proceso);
	log_debug(logger, "Program_counter antes de serializar: %u", pcb->program_counter);
	log_debug(logger, "Estimacion_anterior antes de serializar: %u", pcb->estimacion_anterior);
	log_debug(logger, "Ultima_rafaga antes de serializar: %u", pcb->ultima_rafaga);
	void* stream = serializar_pcb_con_tiempo_bloqueo(cod_op, pcb, tiempo_bloqueo, &size, logger);
	if(send(socket_cliente, stream, size, 0) != size)
		log_error(logger, "Los datos no se enviaron correctamente");

	free(stream);

}

void* serializar_pcb_con_tiempo_bloqueo(op_code cod_op, t_pcb* pcb, uint32_t tiempo_bloqueo, size_t* size, t_log* logger) {

	int length_lista = list_size(pcb->instrucciones);
	size_t size_instrucciones = sizeof(t_instruccion) * length_lista;
	log_debug(logger, "Size instrucciones: %zu", size_instrucciones);

 	*size = sizeof(op_code) +
	 		sizeof(size_t) +
			size_instrucciones + //tamanio lista de instrucciones
			sizeof(t_instruccion) * length_lista + //tamanio de cada instruccion
	 		sizeof(uint32_t) * 6;

	size_t size_payload = *size - sizeof(op_code) - sizeof(size_t);
	log_debug(logger, "Size payload: %zu", size_payload);

	void* stream = malloc(*size);
	int desplazamiento = 0;
	int desplazamiento_lista = 0;
	int indice = 0;

    memcpy(stream, &cod_op, sizeof(op_code));
	desplazamiento += sizeof(op_code);
	memcpy(stream + desplazamiento, &size_payload, sizeof(size_t));
	desplazamiento += sizeof(size_t);
	memcpy(stream + desplazamiento, &pcb->tam_proceso, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(stream + desplazamiento, &pcb->id, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(stream + desplazamiento, &pcb->program_counter, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(stream + desplazamiento, &pcb->estimacion_anterior, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(stream + desplazamiento, &pcb->ultima_rafaga, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(stream + desplazamiento, &tiempo_bloqueo, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	
	memcpy(stream + desplazamiento, &size_instrucciones, sizeof(size_t));
	desplazamiento += sizeof(size_t);
	
	while(desplazamiento_lista < size_instrucciones)
	{
		t_instruccion* instruccion = (t_instruccion*) list_get(pcb->instrucciones, indice);
		memcpy(stream + desplazamiento + desplazamiento_lista, instruccion, sizeof(t_instruccion));
		desplazamiento_lista += sizeof(t_instruccion);
		indice ++;
	}

    return stream;
}

t_pcb* recibir_pcb_con_tiempo_bloqueo(int socket_cliente, t_log* logger) {

 	size_t size;
    if (recv(socket_cliente, &size, sizeof(size_t), 0) != sizeof(size_t)) {
        log_error(logger, "Los datos no se recibieron correctamente"); 
    }

    void* stream = malloc(size);
	log_debug(logger, "Size payload: %zu", size);

    if (recv(socket_cliente, stream, size, 0) != size) {
		log_error(logger, "Los datos no se recibieron correctamente");
    }

    t_pcb* pcb = deserializar_pcb_con_tiempo_bloqueo(stream);

	log_debug(logger, "El ID del PCB recibido es: %d", pcb->id);
	log_debug(logger, "El tamaño del PCB recibido es: %d", pcb->tam_proceso);
	log_debug(logger, "Program_counter antes de serializar: %u", pcb->program_counter);
	log_debug(logger, "Estimacion_anterior antes de serializar: %u", pcb->estimacion_anterior);
	log_debug(logger, "Ultima_rafaga antes de serializar: %u", pcb->ultima_rafaga);
	log_debug(logger, "Tiempo bloqueo: %u", pcb->tiempo_a_bloquearse);

    free(stream); 
	return pcb; 
}

t_pcb* deserializar_pcb_con_tiempo_bloqueo(void* stream) {
	t_pcb* pcb = malloc(sizeof(t_pcb));
    pcb->rafaga = malloc(sizeof(rango_tiempo_t));
    pcb->rafaga_bloqueado = malloc(sizeof(rango_tiempo_t));

	size_t size_instrucciones;
	int desplazamiento = 0;
	int desplazamiento_lista = 0;

	memcpy(&pcb->tam_proceso, stream, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(&pcb->id, stream + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(&pcb->program_counter, stream + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(&pcb->estimacion_anterior, stream + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(&pcb->ultima_rafaga, stream + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(&pcb->tiempo_a_bloquearse, stream + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(&size_instrucciones, stream + desplazamiento, sizeof(size_t));
	desplazamiento += sizeof(size_t);
	
	pcb->instrucciones = list_create();

	while(desplazamiento_lista < size_instrucciones)
	{
		t_instruccion* instruccion = malloc(sizeof(t_instruccion));
		memcpy(instruccion, stream + desplazamiento + desplazamiento_lista, sizeof(t_instruccion));
		list_add(pcb->instrucciones, instruccion);
		desplazamiento_lista += sizeof(t_instruccion);
	}

	return pcb;
}

//---------------------------------------------------------------//

void* serializar_tabla_N2(op_code op, t_tablaN2 *t, size_t *size)
{
	//TAMANIO = OP_CODE + DATO_TAMANIO + (LARGO LISTA * TAMANIO ENTRADA)
	int tamanio_lista = list_size(t) * sizeof(entrada_tabla_N2);
	*size = sizeof(op_code) + sizeof(int) + tamanio_lista;
	void *buffer = malloc(*size);

	int desplazamiento = 0;
	memcpy(buffer + desplazamiento, &op, sizeof(op_code));
	desplazamiento += sizeof(op_code);
	memcpy(buffer + desplazamiento, size, sizeof(size_t));
	desplazamiento += sizeof(size_t);
	for(int i = 0; i < list_size(t); i++)
	{
		entrada_tabla_N2 *e = list_get(t, i);
		memcpy(buffer + desplazamiento, e, sizeof(entrada_tabla_N2));
		desplazamiento += sizeof(entrada_tabla_N2);
	}
	return buffer;
}

void enviar_tabla_N2(int socket_cliente, t_tablaN2 *t, t_log *logger)
{
	size_t size;
	log_info(logger, "Respondiendo solicitud de tabla nivel 2");
	void *buffer = serializar_tabla_N2(SOLICITUD_TABLA_PAGINAS, t, &size);

	if(send(socket_cliente, buffer, size, 0) != size)
		log_error(logger, "Los datos no se enviaron correctamente");

	free(buffer);
}

t_list* deserializar_tabla_N2(void *stream, int tamanio_lista)
{
	int desplazamiento = 0;
	//memcpy(&tamanio_lista, stream, sizeof(int));
	//desplazamiento += sizeof(int);

	t_list *ret = list_create();
	while(desplazamiento < tamanio_lista)
	{
		entrada_tabla_N2 *e = malloc(sizeof(entrada_tabla_N2));
		memcpy(e, stream + desplazamiento, sizeof(entrada_tabla_N2));
		desplazamiento += sizeof(entrada_tabla_N2);
		list_add(ret, e);
	}
	return ret;
}

t_tablaN2* recibir_tabla_N2(int socket_cliente, t_log *logger)
{
	size_t size;
    if (recv(socket_cliente, &size, sizeof(size_t), 0) != sizeof(size_t)) {
        log_error(logger, "Error al recibir tamanio tabla nivel 2"); 
    }

	size -= sizeof(size_t);
	size -= sizeof(op_code);
    void* stream = malloc(size);
	log_info(logger, "Tamanio tabla (en formato paquete): %zu", size);

    if (recv(socket_cliente, stream, size, 0) != size) {
		log_error(logger, "Error al recibir tabla nivel 2");
    }

    t_tablaN2 *ret = deserializar_tabla_N2(stream, size);
    free(stream); 
	return ret;
}

void* serializar_pedido_escritura(uint32_t dir, uint32_t dato, size_t *size, t_log* logger)
{
	op_code cod_op = PEDIDO_ESCRITURA;

 	*size = sizeof(op_code) +
	 		sizeof(size_t) +
	 		sizeof(uint32_t) * 2;

	size_t size_payload = *size - sizeof(op_code) - sizeof(size_t);
	log_info(logger, "Size payload: %zu", size_payload);

	void* stream = malloc(*size);
	int desplazamiento = 0;

    memcpy(stream, &cod_op, sizeof(op_code));
	desplazamiento += sizeof(op_code);

	memcpy(stream + desplazamiento, &size_payload, sizeof(size_t));
	desplazamiento += sizeof(size_t);

	memcpy(stream + desplazamiento, &dir, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(stream + desplazamiento, &dato, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	
	return stream;
}

void enviar_pedido_escritura(uint32_t dir, uint32_t dato, int socket_cliente, t_log *logger)
{
	size_t size;
	log_info(logger, "Dir física: %d", dir);
	log_info(logger, "Dato: %d", dato);

	void* stream = serializar_pedido_escritura(dir, dato, &size, logger);
	if(send(socket_cliente, stream, size, 0) != size)
		log_error(logger, "Los datos no se enviaron correctamente");

	free(stream);
}

t_list *deserializar_pedido_escritura(void *stream)
{
	t_list *parametros = list_create();
	int desplazamiento = 0;
	uint32_t dir = 0;
	uint32_t dato = 0;

	memcpy(&dir, stream, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	list_add(parametros, (void*) dir);

	memcpy(&dato, stream + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	list_add(parametros, (void*) dato);

	free(stream);

	return parametros;
}

t_list* recibir_pedido_escritura(int socket_cliente, t_log *logger)
{
 	size_t size;
    if (recv(socket_cliente, &size, sizeof(size_t), 0) != sizeof(size_t)) {
        log_error(logger, "Los datos no se recibieron correctamente"); 
    }

    void* stream = malloc(size);
	log_debug(logger, "Size payload: %zu", size);

    if (recv(socket_cliente, stream, size, 0) != size) {
		log_error(logger, "Los datos no se recibieron correctamente");
    }

	t_list *parametros = deserializar_pedido_escritura(stream);
	
	return parametros;
}

void enviar_pedido_lectura(uint32_t dir, int socket_cliente, t_log *logger)
{
	size_t size;
	log_info(logger, "Dir física: %d", dir);

	void* stream = serializar_pedido_lectura(dir, &size, logger);
	if(send(socket_cliente, stream, size, 0) != size)
		log_error(logger, "Los datos no se enviaron correctamente");

	free(stream);
}

void* serializar_pedido_lectura(uint32_t dir, size_t *size, t_log* logger)
{
	op_code cod_op = PEDIDO_LECTURA;

 	*size = sizeof(op_code) +
	 		sizeof(size_t) +
	 		sizeof(uint32_t);

	size_t size_payload = *size - sizeof(op_code) - sizeof(size_t);
	log_info(logger, "Size payload: %zu", size_payload);

	void* stream = malloc(*size);
	int desplazamiento = 0;

    memcpy(stream, &cod_op, sizeof(op_code));
	desplazamiento += sizeof(op_code);

	memcpy(stream + desplazamiento, &size_payload, sizeof(size_t));
	desplazamiento += sizeof(size_t);

	memcpy(stream + desplazamiento, &dir, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	return stream;
}

uint32_t recibir_pedido_lectura(int socket_cliente, t_log *logger)
{
 	size_t size;
    if (recv(socket_cliente, &size, sizeof(size_t), 0) != sizeof(size_t)) {
        log_error(logger, "Los datos no se recibieron correctamente"); 
    }

    void* stream = malloc(size);
	log_debug(logger, "Size payload: %zu", size);

    if (recv(socket_cliente, stream, size, 0) != size) {
		log_error(logger, "Los datos no se recibieron correctamente");
    }

	uint32_t dir = deserializar_pedido_lectura(stream);
	
	return dir;
}

uint32_t deserializar_pedido_lectura(void *stream)
{
	int desplazamiento = 0;
	uint32_t dir = 0;

	memcpy(&dir, stream, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	free(stream);

	return dir;
}

void* serializar_pedido_liberar_memoria(op_code cod_op, uint32_t id, uint32_t dir_tabla, size_t *tamanio_buffer, t_log *logger)
{
 	*tamanio_buffer = sizeof(op_code) +
	 		sizeof(size_t) +
	 		sizeof(uint32_t) * 2;

	size_t size_payload = *tamanio_buffer - sizeof(op_code) - sizeof(size_t);
	log_info(logger, "Size payload: %zu", size_payload);

	void* stream = malloc(*tamanio_buffer);
	int desplazamiento = 0;

    memcpy(stream, &cod_op, sizeof(op_code));
	desplazamiento += sizeof(op_code);

	memcpy(stream + desplazamiento, &size_payload, sizeof(size_t));
	desplazamiento += sizeof(size_t);

	memcpy(stream + desplazamiento, &id, sizeof(int));
	desplazamiento += sizeof(int);

	memcpy(stream + desplazamiento, &dir_tabla, sizeof(int));
	desplazamiento += sizeof(int);

	return stream;
}

void enviar_pedido_liberar_memoria(op_code cod_op, int conexion, uint32_t id, uint32_t dir_tabla, t_log *logger)
{
	size_t tamanio_buffer = 0;
	void *buffer = serializar_pedido_liberar_memoria(cod_op, id, dir_tabla, &tamanio_buffer, logger);
	if(send(conexion, buffer, tamanio_buffer, 0) != tamanio_buffer)
		log_error(logger, "Error al enviar pedido liberar memoria");

	free(buffer);
}

t_list* deserializar_pedido_liberar_memoria(void *stream)
{
	t_list *parametros = list_create();
	int desplazamiento = 0;
	int id = 0;
	int dir_tabla = 0;

	memcpy(&id, stream, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	list_add(parametros, (void*) id);

	memcpy(&dir_tabla, stream + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	list_add(parametros, (void*) dir_tabla);

	free(stream);
	return parametros;
}

t_list* recibir_pedido_liberar_memoria(int conexion, t_log *logger)
{
	size_t tamanio_payload;
	if(recv(conexion, &tamanio_payload, sizeof(size_t), 0) < sizeof(size_t))
		log_error(logger, "Error al recibir pedido liberar memoria");

	void *buffer = malloc(tamanio_payload);
	if(recv(conexion, buffer, tamanio_payload, 0) < tamanio_payload)
		log_error(logger, "Error al recibir pedido liberar memoria");
	
	t_list *ret = deserializar_pedido_liberar_memoria(buffer);
	return ret;
}

void* serializar_pedido_desuspender_proceso(int id, size_t *tamanio_buffer)
{
	op_code code_op = DESUSPENDER_PROCESO;

	*tamanio_buffer = 
		sizeof(op_code) +
		sizeof(uint32_t);

	int desplazamiento = 0;
	void *buffer = malloc(*tamanio_buffer);
	memcpy(buffer + desplazamiento, &code_op, sizeof(op_code));
	desplazamiento += sizeof(op_code);

	memcpy(buffer + desplazamiento, &id, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	return buffer;
}

void enviar_pedido_desuspender_proceso(int conexion, int id, t_log *logger)
{
	size_t tamanio_buffer = 0;
	void *buffer = serializar_pedido_desuspender_proceso(id, &tamanio_buffer);
	if(send(conexion, buffer, tamanio_buffer, 0) < 0)
		log_error(logger, "Error al enviar pedido desuspender proceso");

	free(buffer);
}

int deserializar_pedido_desuspender_proceso(void *buffer)
{
	int id;
	memcpy(&id, buffer, sizeof(int));
	free(buffer);
	return id;
}

int recibir_pedido_desuspender_proceso(int conexion, t_log *logger)
{
	void *buffer = malloc(sizeof(uint32_t));
	if(recv(conexion, buffer,sizeof(uint32_t), 0) < 0)
		log_error(logger, "Error al recibir pedido desuspender proceso");

	int id = deserializar_pedido_desuspender_proceso(buffer);
	return id;
}

//------------------------------------------------------------------//

void enviar_lista_instrucciones_y_tam_proceso(op_code cod_op, t_list* instrucciones, u_int32_t tam_proceso, int socket_cliente, t_log* logger) {

	size_t size;
	t_instruccion* instruccion = (t_instruccion*) list_get(instrucciones, 0);
	log_debug(logger, "Instruccion a serializar: %d", instruccion->op);
	log_debug(logger, "Tam proceso: %d", tam_proceso);
	void* stream = serializar_lista_instrucciones_y_tam_proceso(cod_op, instrucciones, tam_proceso, &size, logger);
	if(send(socket_cliente, stream, size, 0) != size)
		log_error(logger, "Los datos no se enviaron correctamente");

	free(stream);
}

void* serializar_lista_instrucciones_y_tam_proceso(op_code cod_op, t_list* instrucciones, uint32_t tam_proceso, size_t* size, t_log* logger) {

	int length_lista = list_size(instrucciones);
	size_t size_instrucciones = sizeof(t_instruccion) * length_lista;
	log_debug(logger, "Size instrucciones: %zu", size_instrucciones);

 	*size = sizeof(op_code) +
	 		sizeof(size_t) +
	 		sizeof(uint32_t) +
			size_instrucciones + //tamanio lista de instrucciones
			sizeof(t_instruccion) * length_lista //tamanio de cada instruccion
	;		

	size_t size_payload = *size - sizeof(op_code) - sizeof(size_t);
	log_debug(logger, "Size payload: %zu", size_payload);

	void* stream = malloc(*size);
	int desplazamiento = 0;
	int desplazamiento_lista = 0;
	int indice = 0;

    memcpy(stream, &cod_op, sizeof(op_code));
	desplazamiento += sizeof(op_code);
	memcpy(stream + desplazamiento, &size_payload, sizeof(size_t));
	desplazamiento += sizeof(size_t);
	memcpy(stream + desplazamiento, &tam_proceso, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(stream + desplazamiento, &size_instrucciones, sizeof(size_t));
	desplazamiento += sizeof(size_t);
	
	log_debug(logger, "Tam lista instrucciones %zu", size_instrucciones);

	while(desplazamiento_lista < size_instrucciones)
	{
		t_instruccion* instruccion = (t_instruccion*) list_get(instrucciones, indice);
		memcpy(stream + desplazamiento + desplazamiento_lista, instruccion, sizeof(t_instruccion));
		desplazamiento_lista += sizeof(t_instruccion);
		indice ++;
	}

    return stream;
}

t_list* recibir_lista_instrucciones_y_tam_proceso(int socket_cliente, u_int32_t *tam_proceso, t_log* logger) {

 	size_t size;
    if (recv(socket_cliente, &size, sizeof(size_t), 0) != sizeof(size_t)) {
        log_error(logger, "Los datos no se recibieron correctamente"); 
    }

    void* stream = malloc(size);
	log_debug(logger, "Size payload: %zu", size);

    if (recv(socket_cliente, stream, size, 0) != size) {
		log_error(logger, "Los datos no se recibieron correctamente");
    }

    t_list* lista_instrucciones = deserializar_lista_instrucciones_y_tam_proceso(stream, tam_proceso);
    free(stream); 
	return lista_instrucciones;
}

t_list* deserializar_lista_instrucciones_y_tam_proceso(void* stream, uint32_t *tam_proceso) {
	
	t_list* lista_instrucciones = list_create();
	size_t size_instrucciones;
	int desplazamiento = 0;
	int desplazamiento_lista = 0;

	memcpy(tam_proceso, stream, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(&size_instrucciones, stream + desplazamiento, sizeof(size_t));
	desplazamiento += sizeof(size_t);

	while(desplazamiento_lista < size_instrucciones)
	{
		t_instruccion* instruccion = malloc(sizeof(t_instruccion));
		memcpy(instruccion, stream + desplazamiento + desplazamiento_lista, sizeof(t_instruccion));
		list_add(lista_instrucciones, instruccion);
		desplazamiento_lista += sizeof(t_instruccion);
	}

	return lista_instrucciones;
}

void enviar_interrupcion(int socket_cliente, t_log* logger) {

	op_code cod_op = INTERRUPCION;
	size_t size = sizeof(op_code); 
	void* stream = malloc(size);

    memcpy(stream, &cod_op, sizeof(op_code));

	if(send(socket_cliente, stream, size, 0) != size)
		log_error(logger, "Los datos no se enviaron correctamente");

	free(stream);
}
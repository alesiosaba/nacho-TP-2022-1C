#include "../include/procesos.h"

static t_list *cola_nuevos;
static t_list *cola_listos;
static t_list *cola_ejecucion;
static t_list *cola_bloqueados;
static t_list *cola_suspendidos_bloqueados;
static t_list *cola_suspendidos_listos;
static t_list *cola_terminados;

pthread_mutex_t proceso_mutex;
pthread_mutex_t procesos_nuevos_mutex;
pthread_mutex_t procesos_listos_mutex;
pthread_mutex_t procesos_ejecutando_mutex;
pthread_mutex_t procesos_bloqueados_mutex;
pthread_mutex_t procesos_suspendidos_bloqueados_mutex;
pthread_mutex_t procesos_suspendidos_listos_mutex;
pthread_mutex_t procesos_terminados_mutex;
pthread_mutex_t procesos_rafaga_mutex;
pthread_mutex_t procesos_rafaga_bloqueado_mutex;

void iniciar_estructuras_de_estados_de_procesos() {
    cola_nuevos = list_create();
    cola_listos = list_create();
    cola_ejecucion = list_create();
    cola_bloqueados = list_create();
    cola_suspendidos_bloqueados = list_create();
    cola_suspendidos_listos = list_create();
    cola_terminados = list_create();

    pthread_mutex_init(&proceso_mutex, NULL);
    pthread_mutex_init(&procesos_nuevos_mutex, NULL);
    pthread_mutex_init(&procesos_listos_mutex, NULL);
    pthread_mutex_init(&procesos_ejecutando_mutex, NULL);
    pthread_mutex_init(&procesos_bloqueados_mutex, NULL);
    pthread_mutex_init(&procesos_suspendidos_bloqueados_mutex, NULL);
    pthread_mutex_init(&procesos_suspendidos_listos_mutex, NULL);
    pthread_mutex_init(&procesos_terminados_mutex, NULL);
    pthread_mutex_init(&procesos_rafaga_mutex, NULL);
    pthread_mutex_init(&procesos_rafaga_bloqueado_mutex, NULL);

    sem_init(&sem_multiprogramacion, 0, grado_multiprogramacion);
    sem_init(&sem_proceso_nuevo, 0, 0);
    sem_init(&sem_proceso_listo, 0, 0);
    sem_init(&sem_proceso_suspendido_listo, 0, 0);
    sem_init(&sem_proceso_a_suspender,0,0);
    sem_init(&sem_proceso_bloqueado,0,0);   
}

t_pcb* crear_proceso(uint32_t id, uint32_t tam, t_list* lista_instrucciones) {
    
    pthread_mutex_lock(&proceso_mutex);

    t_pcb* pcb_nuevo = malloc(sizeof(t_pcb));

    pcb_nuevo->id = id;
    pcb_nuevo->tam_proceso = tam;
    pcb_nuevo->instrucciones = list_create();
    pcb_nuevo->program_counter = 0;
    pcb_nuevo->estimacion_anterior = estimacion_inicial;
    pcb_nuevo->ultima_rafaga = 0;
    pcb_nuevo->prox_rafaga_estimada = estimacion_inicial;
    pcb_nuevo->rafaga = malloc(sizeof(rango_tiempo_t));
    pcb_nuevo->tiempo_a_bloquearse = 0;
    pcb_nuevo->rafaga_bloqueado = malloc(sizeof(rango_tiempo_t));

    t_list_iterator *iterador_proceso = list_iterator_create(lista_instrucciones);
    while(list_iterator_has_next(iterador_proceso))
    {
        char *i = list_iterator_next(iterador_proceso);
        list_add(pcb_nuevo->instrucciones, i);
    }
    list_iterator_destroy(iterador_proceso);

    //ENVIAR MENSAJE A MEMORIA
    int conexion_memoria = crear_conexion(logger, "Memoria", ip_memoria, puerto_memoria);

    t_paquete *info = crear_paquete(INICIALIZAR_ESTRUCTURAS);
    agregar_a_paquete(info, &id, sizeof(uint32_t));
    agregar_a_paquete(info, &tam, sizeof(uint32_t));
    
    enviar_paquete(info, conexion_memoria, logger);
    eliminar_paquete(info);

    pcb_nuevo->tabla_paginas = recibir_num(conexion_memoria, logger);
    log_info(logger, "PCB ID %d Direccion de tabla recibida: %d", pcb_nuevo->id, pcb_nuevo->tabla_paginas);
    close(conexion_memoria);
    //FIN ENVIAR MENSAJE

    pthread_mutex_unlock(&proceso_mutex);

    return pcb_nuevo;
}

void modificar_identificador_tabla_de_paginas_del_proceso(t_pcb* pcb, uint32_t id_tabla) {
    
    pthread_mutex_lock(&proceso_mutex);

    pcb->tabla_paginas = id_tabla;

    pthread_mutex_unlock(&proceso_mutex);
}

void copiar_inicio_rafaga_del_proceso(t_pcb* pcb1, t_pcb* pcb2) {
    
    pthread_mutex_lock(&proceso_mutex);

    pcb1->rafaga->inicio = pcb2->rafaga->inicio;
    pcb1->tabla_paginas = pcb2->tabla_paginas;

    pthread_mutex_unlock(&proceso_mutex);
}

void copiar_proxima_rafaga_estimada_del_proceso(t_pcb* pcb1, t_pcb* pcb2) {
    
    pthread_mutex_lock(&proceso_mutex);

    pcb1->prox_rafaga_estimada = pcb2->prox_rafaga_estimada;

    pthread_mutex_unlock(&proceso_mutex);
}

void modificar_estado_proceso(t_pcb* pcb, int estado) {

    pthread_mutex_lock(&proceso_mutex);

    pcb->estado = estado;

    pthread_mutex_unlock(&proceso_mutex);
}

void inicializar_tiempo_bloqueado(t_pcb* pcb) {
    pthread_mutex_lock(&proceso_mutex);

    pcb->tiempo_bloqueado = 0;

    pthread_mutex_unlock(&proceso_mutex);
}

void destruir_proceso(t_pcb* pcb) {
    //TODO: semaforo mutex
    free(pcb->rafaga);
    free(pcb->rafaga_bloqueado);
    list_destroy_and_destroy_elements(pcb->instrucciones, free);
    free(pcb);
}

/* --------------- Funciones Procesos Nuevos --------------- */

void encolar_proceso_en_nuevos(t_pcb* proceso) {

    pthread_mutex_lock(&procesos_nuevos_mutex);

    list_add(cola_nuevos, proceso);

    pthread_mutex_unlock(&procesos_nuevos_mutex);

    sem_post(&sem_proceso_nuevo);
}

t_pcb* desencolar_proceso_nuevo() {

    pthread_mutex_lock(&procesos_nuevos_mutex);

    t_pcb* proceso = list_remove(cola_nuevos, 0);

    pthread_mutex_unlock(&procesos_nuevos_mutex);

  return proceso;
}

/* --------------- Funciones Procesos Listos --------------- */

void encolar_proceso_en_listos(t_pcb* proceso) {

    pthread_mutex_lock(&procesos_listos_mutex);

    list_add(cola_listos, proceso);
    modificar_estado_proceso(proceso, LISTO);

    pthread_mutex_unlock(&procesos_listos_mutex);

    sem_post(&sem_proceso_listo);

    if(!strcmp(algoritmo_planificacion, "SRT")) {
    
        if(hay_proceso_en_ejecucion()) {
            int conexion_interrupt = crear_conexion(logger, "CPU", ip_cpu, puerto_cpu_interrupt);
            enviar_interrupcion(conexion_interrupt, logger);
            log_info(logger, "Se envio interrupcion");
            close(conexion_interrupt);
        }
    }
}

t_pcb* desencolar_proceso_listo() {

    pthread_mutex_lock(&procesos_listos_mutex);

    t_pcb* proceso = list_remove(cola_listos, 0);

    pthread_mutex_unlock(&procesos_listos_mutex);

  return proceso;
}

void ordenar_cola_listos() {

	//list_iterate(cola_listos, (void*) iterator);

    pthread_mutex_lock(&procesos_listos_mutex);

    log_info(logger, "Ordenando cola de listos");
    list_sort(cola_listos, (void*)mayor_prioridad);
    list_iterate(cola_listos, (void*) imprimir_proxima_rafaga);
    //log_info(logger, "Actualizadas todas las estimaciones anteriores");

    pthread_mutex_unlock(&procesos_listos_mutex);
}

/* --------------- Funciones Procesos en Ejecución --------------- */

void encolar_proceso_en_ejecucion(t_pcb* proceso) {
 
    pthread_mutex_lock(&procesos_ejecutando_mutex);

    list_add(cola_ejecucion, proceso);
    proceso_iniciar_rafaga(proceso);

    pthread_mutex_unlock(&procesos_ejecutando_mutex);
}

t_pcb* desencolar_proceso_en_ejecucion() {

    pthread_mutex_lock(&procesos_ejecutando_mutex);

    t_pcb* proceso = list_remove(cola_ejecucion, 0);
    //proceso_finalizar_rafaga(proceso); Al ordenar la lista más tarde, este pcb no existe más

    pthread_mutex_unlock(&procesos_ejecutando_mutex);

  return proceso;
}

int hay_proceso_en_ejecucion() {

    pthread_mutex_lock(&procesos_ejecutando_mutex);

    int resultado = (int) list_is_empty(cola_ejecucion);
    if(!resultado)
        log_info(logger, "Hay proceso en ejecucion");
    else
        log_info(logger, "NO hay proceso en ejecucion");
 
    pthread_mutex_unlock(&procesos_ejecutando_mutex);

    return !resultado;
}

/* --------------- Funciones Procesos Bloqueados --------------- */

void encolar_proceso_en_bloqueados(t_pcb* proceso) {

    pthread_mutex_lock(&procesos_bloqueados_mutex);

    list_add(cola_bloqueados, proceso);
    modificar_estado_proceso(proceso, BLOQUEADO);
    iniciar_rafaga_bloqueado(proceso);

    pthread_mutex_unlock(&procesos_bloqueados_mutex);

    //------------BORRAR DESPUES :(------//
    /*t_pcb* pcb = desencolar_proceso_bloqueado();
    log_info(logger, "PCB ID %d debe ejecutar I/O %u ms", pcb->id, pcb->tiempo_a_bloquearse);

    usleep(pcb->tiempo_a_bloquearse * 1000);
    encolar_proceso_en_listos(pcb);

    log_info(logger, "PCB ID %d ejecuto I/O %u ms", pcb->id, pcb->tiempo_a_bloquearse);
    //--------------------------------------//
*/

    pthread_t planificador_mediano_plazo;
    pthread_create(&planificador_mediano_plazo, NULL, (void*) evaluar_suspender_proceso, proceso);
    pthread_detach(planificador_mediano_plazo);

    sem_post(&sem_proceso_bloqueado);
}

void evaluar_suspender_proceso(t_pcb* pcb) {
    
    usleep(tiempo_max_bloqueado * 1000);

    if(proceso_esta_bloqueado(pcb))
    {
        log_info(logger, "PCB ID %d ha sido suspendido", pcb->id);
        modificar_estado_proceso(pcb, BLOQUEADO_SUSPENDIDO);
        sem_post(&sem_multiprogramacion);
        int conexion_memoria = crear_conexion(logger, "Memoria", ip_memoria, puerto_memoria);
        enviar_pedido_liberar_memoria(SUSPENDER_PROCESO, conexion_memoria, pcb->id, pcb->tabla_paginas, logger);
        close(conexion_memoria);
    }
}

int proceso_esta_bloqueado(t_pcb* pcb) {

    pthread_mutex_lock(&procesos_bloqueados_mutex);

    int resultado = pcb->estado == BLOQUEADO;

    pthread_mutex_unlock(&procesos_bloqueados_mutex);

    return resultado;
}

t_pcb* desencolar_proceso_bloqueado() {

    pthread_mutex_lock(&procesos_bloqueados_mutex);

    t_pcb* proceso = list_remove(cola_bloqueados, 0);

    pthread_mutex_unlock(&procesos_bloqueados_mutex);

  return proceso;
}

void iniciar_rafaga_bloqueado(t_pcb *pcb) {
    gettimeofday(&pcb->rafaga_bloqueado->inicio, NULL);
}

void actualizar_rafaga_bloqueado(t_pcb* pcb) {
    pthread_mutex_lock(&procesos_rafaga_bloqueado_mutex);

    gettimeofday(&pcb->rafaga_bloqueado->fin, NULL);
    pcb->tiempo_bloqueado = timedifference_msec(pcb->rafaga_bloqueado->inicio, pcb->rafaga_bloqueado->fin);

    pthread_mutex_unlock(&procesos_rafaga_bloqueado_mutex);
}

/* --------------- Funciones Procesos Suspendidos Bloqueados --------------- */

void encolar_proceso_en_suspendidos_bloqueados(t_pcb* proceso) {

    pthread_mutex_lock(&procesos_suspendidos_bloqueados_mutex);

    list_add(cola_suspendidos_bloqueados, proceso);

    pthread_mutex_unlock(&procesos_suspendidos_bloqueados_mutex);

}

t_pcb* desencolar_proceso_suspendido_bloqueado() {

    pthread_mutex_lock(&procesos_suspendidos_bloqueados_mutex);

    t_pcb* proceso = list_remove(cola_suspendidos_bloqueados, 0);

    pthread_mutex_unlock(&procesos_suspendidos_bloqueados_mutex);

    return proceso;
}

/* --------------- Funciones Procesos Suspendidos Listos --------------- */

void encolar_proceso_en_suspendidos_listos(t_pcb* proceso) {

    pthread_mutex_lock(&procesos_suspendidos_listos_mutex);

    list_add(cola_suspendidos_listos, proceso);

    pthread_mutex_unlock(&procesos_suspendidos_listos_mutex);
    sem_post(&sem_proceso_nuevo);
}

t_pcb* desencolar_proceso_suspendido_listo() {

    pthread_mutex_lock(&procesos_suspendidos_listos_mutex);

    t_pcb* proceso = list_remove(cola_suspendidos_listos, 0);
 
    pthread_mutex_unlock(&procesos_suspendidos_listos_mutex);

    return proceso;
}

int hay_proceso_suspendido_listo() {

    pthread_mutex_lock(&procesos_suspendidos_listos_mutex);

    int resultado = (int) list_is_empty(cola_suspendidos_listos);
 
    pthread_mutex_unlock(&procesos_suspendidos_listos_mutex);

    return !resultado;
}

/* --------------- Funciones Procesos Terminados --------------- */

void encolar_proceso_en_terminados(t_pcb* proceso) {

    pthread_mutex_lock(&procesos_terminados_mutex);

    list_add(cola_terminados, proceso);

    pthread_mutex_unlock(&procesos_terminados_mutex);

}

t_pcb* desencolar_proceso_terminado() {

    pthread_mutex_lock(&procesos_terminados_mutex);

    t_pcb* proceso = list_remove(cola_terminados, 0);

    pthread_mutex_unlock(&procesos_terminados_mutex);

    return proceso;
}

/* --------------- Funciones Generales --------------- */

int cantidad_procesos_en_sistema() {

    int cantidad_listos, cantidad_ejecutando, cantidad_bloqueados, cantidad_total;

    pthread_mutex_lock(&procesos_listos_mutex);
    pthread_mutex_lock(&procesos_ejecutando_mutex);
    pthread_mutex_lock(&procesos_bloqueados_mutex);

    cantidad_listos = list_size(cola_listos);
    cantidad_ejecutando = list_size(cola_ejecucion);
    cantidad_bloqueados = list_size(cola_bloqueados);

    pthread_mutex_unlock(&procesos_listos_mutex);
    pthread_mutex_unlock(&procesos_ejecutando_mutex);
    pthread_mutex_unlock(&procesos_bloqueados_mutex);

    cantidad_total = cantidad_listos + cantidad_ejecutando + cantidad_bloqueados;

    log_debug(logger, "Cantidad de procesos en sistema: %d", cantidad_total);

    return cantidad_total;
}

void proceso_iniciar_rafaga(t_pcb *pcb) {
    gettimeofday(&pcb->rafaga->inicio, NULL);
    //log_debug(logger, "Inicio ráfaga: %d", pcb->rafaga->inicio);
}

void proceso_finalizar_rafaga(t_pcb* pcb) {
    pthread_mutex_lock(&procesos_rafaga_mutex);

    gettimeofday(&pcb->rafaga->fin, NULL);
    //log_debug(logger, "Fin ráfaga: %d", pcb->rafaga->fin);
    pcb->ultima_rafaga = timedifference_msec(pcb->rafaga->inicio, pcb->rafaga->fin);
    log_info(logger, "PCB ID %d - Última ráfaga: %d ms", pcb->id, pcb->ultima_rafaga);

    pthread_mutex_unlock(&procesos_rafaga_mutex);
}

void actualizar_proxima_rafaga(t_pcb* pcb)
{
    pthread_mutex_lock(&proceso_mutex);
	pcb->prox_rafaga_estimada -= pcb->ultima_rafaga;
    pthread_mutex_unlock(&proceso_mutex);
    log_info(logger, "PCB ID %d - Próxima ráfaga estimada: %d ms", pcb->id, pcb->prox_rafaga_estimada);
}

void imprimir_proxima_rafaga(t_pcb* pcb)
{
    log_info(logger, "PCB ID %d - Próxima ráfaga estimada: %d ms", pcb->id, pcb->prox_rafaga_estimada);
}

void estimar_proxima_rafaga(t_pcb* pcb) {

    pthread_mutex_lock(&proceso_mutex);

    // Fórmula SJF	pr = ur * alpha + (1 - alpha) * t estimado ur
    pcb->prox_rafaga_estimada = pcb->ultima_rafaga * alfa + (1 - alfa) * pcb->estimacion_anterior;
    pcb->estimacion_anterior = pcb->prox_rafaga_estimada;

    pthread_mutex_unlock(&proceso_mutex);
    log_info(logger, "PCB ID %d - Próxima ráfaga estimada: %d ms", pcb->id, pcb->prox_rafaga_estimada);
}

int mayor_prioridad(t_pcb *pcb1, t_pcb *pcb2) {
    return pcb1->prox_rafaga_estimada <= pcb2->prox_rafaga_estimada;
}

int puede_suspenderse(t_pcb* pcb) {

    //int tiempo_bloqueado;
    //tiempo_bloqueado = timedifference_msec(pcb->tiempo_bloqueado->inicio, pcb->tiempo_bloqueado->fin);
    //return tiempo_bloqueado > tiempo_max_bloqueado;
    return 1;
}

float timedifference_msec(struct timeval t0, struct timeval t1) {
    return (t1.tv_sec - t0.tv_sec) * 1000.0f + (t1.tv_usec - t0.tv_usec) / 1000.0f;
}
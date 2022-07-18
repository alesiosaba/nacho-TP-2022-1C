#include "../include/planificador_corto_plazo.h"

static pthread_t planificador_corto_plazo;

void iniciar_planificador_corto_plazo() {

    pthread_create(&planificador_corto_plazo, NULL, (void*) planificar_procesos, NULL);
    pthread_detach(planificador_corto_plazo);
}

void planificar_procesos() {

    log_debug(logger, "Planificador de corto plazo iniciado");

    while (1) {

        sem_wait(&sem_proceso_listo);
        log_debug(logger, "Plani CP notificado proceso listo");

        int conexion_dispatch = crear_conexion(logger, "CPU", ip_cpu, puerto_cpu_dispatch);

        if(algoritmo_es_srt()) {
            log_debug(logger, "Algoritmo Plani: SRT");

            ordenar_cola_listos();   
        }

        t_pcb* pcb = desencolar_proceso_listo();
        encolar_proceso_en_ejecucion(pcb);
        enviar_pcb(RECIBIR_PCB, pcb, conexion_dispatch, logger);
        log_info(logger, "PCB ID %d ha sido enviado a CPU", pcb->id);
        recibir_pcb_luego_de_ejecutar(conexion_dispatch);
        close(conexion_dispatch);

    }
}

bool algoritmo_es_srt() {
    return 0 == strcmp(algoritmo_planificacion, "SRT");
}

void recibir_pcb_luego_de_ejecutar(int conexion) {

    int op_code = recibir_operacion(conexion);
    t_pcb *pcb_en_ejecucion, *pcb_actualizado;

    switch(op_code) {
        case EXIT:
            log_info(logger, "Petición recibida: EXIT");
            pcb_en_ejecucion = desencolar_proceso_en_ejecucion();
            proceso_finalizar_rafaga(pcb_en_ejecucion);
            destruir_proceso(pcb_en_ejecucion); //Porque tenemos que empezar a usar el pcb actulizado
            pcb_actualizado = recibir_pcb(conexion, logger);
            log_info(logger, "PCB ID %d ha finalizado", pcb_actualizado->id);
            encolar_proceso_en_terminados(pcb_actualizado);
            sem_post(&sem_multiprogramacion);
            enviar_mensaje("El proceso ha finalizado su ejecucion", pcb_actualizado->id, logger);
            break;

        case IO:
            log_info(logger, "Petición recibida: IO");
            pcb_en_ejecucion = desencolar_proceso_en_ejecucion();
            pcb_actualizado = recibir_pcb_con_tiempo_bloqueo(conexion, logger);
            copiar_inicio_rafaga_del_proceso(pcb_actualizado, pcb_en_ejecucion);
            proceso_finalizar_rafaga(pcb_actualizado);
            estimar_proxima_rafaga(pcb_actualizado);
            inicializar_tiempo_bloqueado(pcb_actualizado);
            destruir_proceso(pcb_en_ejecucion); //Porque tenemos que empezar a usar el pcb actulizado
            encolar_proceso_en_bloqueados(pcb_actualizado);
            break;

        case ACTUALIZAR_PCB:
            log_info(logger, "Petición recibida: ACTUALIZAR_PCB"); 
            pcb_en_ejecucion = desencolar_proceso_en_ejecucion();
            pcb_actualizado = recibir_pcb(conexion, logger);
            copiar_inicio_rafaga_del_proceso(pcb_actualizado, pcb_en_ejecucion);
            copiar_proxima_rafaga_estimada_del_proceso(pcb_actualizado, pcb_en_ejecucion);
            proceso_finalizar_rafaga(pcb_actualizado);
            destruir_proceso(pcb_en_ejecucion); //Porque tenemos que empezar a usar el pcb actulizado
            actualizar_proxima_rafaga(pcb_actualizado);
            encolar_proceso_en_listos(pcb_actualizado);

            break;
        
        default:
            log_error(logger, "El OP_CODE recibido es inválido");
            break;
    }
}
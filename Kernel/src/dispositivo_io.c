#include "../include/dispositivo_io.h"

static pthread_t dispositivo_io;

void iniciar_dispositivo_io() {

    pthread_create(&dispositivo_io, NULL, (void*) atender_procesos, NULL);
    pthread_detach(dispositivo_io);
}

void atender_procesos() {

    while (1)
    {
        sem_wait(&sem_proceso_bloqueado);

        t_pcb* pcb = desencolar_proceso_bloqueado();
        log_info(logger, "PCB ID %d debe ejecutar I/O %u ms", pcb->id, pcb->tiempo_a_bloquearse);

        usleep(pcb->tiempo_a_bloquearse * 1000);

        log_info(logger, "PCB ID %d terminó de ejecutar I/O", pcb->id);

        if (pcb->estado == BLOQUEADO)
            encolar_proceso_en_listos(pcb);
        else
            encolar_proceso_en_suspendidos_listos(pcb);
    }
}
#include "../include/planificador_mediano_plazo.h"

static pthread_t planificador_mediano_plazo;

void iniciar_planificador_mediano_plazo() {

    pthread_create(&planificador_mediano_plazo, NULL, (void*) suspender_procesos, NULL);
    pthread_detach(planificador_mediano_plazo);
}

void suspender_procesos() {

}
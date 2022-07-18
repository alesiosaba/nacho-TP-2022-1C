#include "../include/shared_log.h"

t_log* iniciar_logger(char* log_file_path, char* nombre_modulo) {
    return log_create(log_file_path, nombre_modulo, true, LOG_LEVEL_INFO);
}

void destruir_logger(t_log* logger) {
    log_destroy(logger);
}

/*void shared_log_trace(t_log* logger) {
    pthread_mutex_lock(&log_mutex);
    log_trace(logger, "Hola");
    pthread_mutex_unlock(&log_mutex);
}

void shared_log_debug(t_log* logger, char* mensaje) {
    pthread_mutex_lock(&log_mutex);
    char* mensaje_string = string_from_format("%s", mensaje);
    log_debug(logger, mensaje_string);
    free(mensaje_string);
    pthread_mutex_unlock(&log_mutex);
}

void shared_log_info(t_log* logger, char *mensaje) {
    pthread_mutex_lock(&log_mutex);
    char* mensaje_string = string_from_format("%s", mensaje);
    log_info(logger, mensaje_string);
    free(mensaje_string);
    pthread_mutex_unlock(&log_mutex);
}

void shared_log_warning(t_log* logger, char* mensaje) {
    char* nuevo = string_new();
    strcpy(nuevo, mensaje);
    pthread_mutex_lock(&log_mutex);
    log_debug(logger, nuevo);
    pthread_mutex_unlock(&log_mutex);
}

void shared_log_error(t_log* logger, char *mensaje) {
    pthread_mutex_lock(&log_mutex);
    log_error(logger, mensaje);
    pthread_mutex_unlock(&log_mutex);
}*/

/*
Ejemplo sin warning:

void shared_log_trace(t_log* logger) {
    pthread_mutex_lock(&log_mutex);
    log_trace(logger, "Hola");
    pthread_mutex_unlock(&log_mutex);
}
*/
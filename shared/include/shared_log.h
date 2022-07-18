#ifndef SHARED_LOG
#define SHARED_LOG

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include <commons/string.h>

t_log* iniciar_logger(char*, char*);
void destruir_logger(t_log*);

/*void shared_log_trace(t_log*);
void shared_log_debug(t_log*, char*);
void shared_log_info(t_log*, char*);
void shared_log_warning(t_log*, char*); 
void shared_log_error(t_log*, char*);
void error_log(char *ubicacion, char *mensaje);*/

#endif
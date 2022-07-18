#include "../include/shared_utils.h"

void finalizar_programa(t_log* logger, t_config* config) {
    destruir_logger(logger);
    destruir_config(config);
}
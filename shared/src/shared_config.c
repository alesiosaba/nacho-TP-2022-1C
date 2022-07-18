#include "../include/shared_config.h"

t_config* iniciar_config(char* CONFIG_FILE_PATH) {

  t_config* config_consola = config_create(CONFIG_FILE_PATH);

  return config_consola;
}

void destruir_config(t_config* config) {
  config_destroy(config);
}
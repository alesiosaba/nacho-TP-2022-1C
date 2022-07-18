#include "../include/main.h"

#define CONFIG_FILE_PATH "./cfg/Memoria.config"
#define LOG_FILE_PATH "./cfg/Memoria.log"
#define NOMBRE_MODULO "MEMORIA"

int main(int argc, char** argv) {

	t_config* config_memoria;
	logger = iniciar_logger(LOG_FILE_PATH, NOMBRE_MODULO);

    if(argc < 2) {
        log_error(logger, "Cantidad de argumentos insuficientes");
        return EXIT_FAILURE;
    }

	config_memoria = iniciar_config(argv[1]);

	procesar_archivo_config_memoria(config_memoria);

	iniciar_estructuras_memoria();

	pthread_t hilo_disco;
	pthread_create(&hilo_disco, NULL, (void*)funciones_disco, NULL);

	pthread_t hilo_servidor;
    pthread_create(&hilo_servidor, NULL, (void*) servidor, NULL);
	
	pthread_join(hilo_servidor, NULL);

	eliminar_listas_tablas();

	finalizar_programa(logger, config_memoria);
	
	return EXIT_SUCCESS;
}

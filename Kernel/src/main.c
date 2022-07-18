#include "../include/main.h"

#define CONFIG_FILE_PATH "./cfg/Kernel.config"
#define LOG_FILE_PATH "./cfg/Kernel.log"
#define NOMBRE_MODULO "KERNEL"

int main(int argc, char** argv) {

    t_config* config_kernel;
	logger = iniciar_logger(LOG_FILE_PATH, NOMBRE_MODULO);

    if(argc < 2) {
        log_error(logger, "Cantidad de argumentos insuficientes");
        return EXIT_FAILURE;
    }

	config_kernel = iniciar_config(argv[1]);
	procesar_archivo_config_kernel(config_kernel);

    iniciar_estructuras_de_estados_de_procesos();
    iniciar_planificador_largo_plazo();
    iniciar_planificador_corto_plazo();
    iniciar_dispositivo_io();

    pthread_t hilo_escucha;
    pthread_create(&hilo_escucha, NULL, (void*) escuchar_procesos_nuevos, NULL);

    pthread_join(hilo_escucha, NULL);

    finalizar_programa(logger, config_kernel);
}

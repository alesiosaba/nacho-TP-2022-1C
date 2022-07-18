#include "../include/main.h"

#define CONFIG_FILE_PATH "./cfg/CPU.config"
#define LOG_FILE_PATH "./cfg/CPU.log"
#define NOMBRE_MODULO "CPU"

int main(int argc, char** argv) {

	t_config* config_cpu;
	logger_CPU = iniciar_logger(LOG_FILE_PATH, NOMBRE_MODULO);
	flag_interrupcion = 0;

    if(argc < 2) {
        log_error(logger_CPU, "Cantidad de argumentos insuficientes");
        return EXIT_FAILURE;
    }

	config_cpu = iniciar_config(argv[1]);
	procesar_archivo_config_cpu(config_cpu);

	iniciar_estructuras(); //de momento, solo TLB

	pthread_t hilo_servidor_interrupt;
    pthread_create(&hilo_servidor_interrupt, NULL, (void*) servidor_interrupt, NULL);

	pthread_t hilo_servidor_dispatch;
    pthread_create(&hilo_servidor_dispatch, NULL, (void*) servidor_dispatch, NULL);

	pthread_join(hilo_servidor_interrupt, NULL);
	pthread_join(hilo_servidor_dispatch, NULL);
	finalizar_programa(logger_CPU, config_cpu);

	return 0;
}

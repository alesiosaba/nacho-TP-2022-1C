#include "../include/main.h"

#define CONFIG_FILE_PATH "./cfg/Consola.config"
#define LOG_FILE_PATH "./cfg/Consola.log"
#define NOMBRE_MODULO "Consola"

int main(int argc, char** argv) {

    t_config* config_consola;
    logger_consola = iniciar_logger(LOG_FILE_PATH, NOMBRE_MODULO);

    if(argc < 3) {
        log_error(logger_consola, "Cantidad de argumentos insuficientes");
        return EXIT_FAILURE;
    }
    
    // Path del archivo: argv[1] && Tamaño del proceso: argv[2]
    log_info(logger_consola, "Módulo Consola iniciado");

    config_consola = iniciar_config(CONFIG_FILE_PATH);
    procesar_archivo_config_consola(config_consola);

    int conexion_kernel = crear_conexion(logger_consola, "KERNEL", ip_kernel, puerto_kernel);

    t_list* lista_instrucciones = leer_archivo(argv[1]);

    //t_instruccion* instruccion = (t_instruccion*) list_get(lista_instrucciones, 0);

    u_int32_t tam_proceso = atoi(argv[2]);
    
    enviar_lista_instrucciones_y_tam_proceso(NUEVO_PROCESO, lista_instrucciones, tam_proceso, conexion_kernel, logger_consola);

    int op_code = recibir_operacion(conexion_kernel);
    recibir_mensaje(conexion_kernel, logger_consola);

    close(conexion_kernel);
    //TODO list_destroy_and_destroy_elements(lista_instrucciones, free);
    list_destroy(lista_instrucciones);
    finalizar_programa(logger_consola, config_consola);
}

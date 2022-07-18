#include "../include/ejecucion_procesos.h"

void realizar_ciclo_de_instruccion(t_pcb* pcb, int conexion_kernel) { 

    t_instruccion* instruccion = buscar_proxima_instruccion(pcb);
    interpretar_instruccion_y_ejecutar_pcb(instruccion, pcb, conexion_kernel);
}

t_instruccion* buscar_proxima_instruccion(t_pcb* pcb) { 
    log_info(logger_CPU, "Etapa FETCH iniciada");
    
    t_instruccion* instruccion = list_get(pcb->instrucciones, pcb->program_counter);

    pcb->program_counter += 1;

    return instruccion;
}

void interpretar_instruccion_y_ejecutar_pcb(t_instruccion* instruccion, t_pcb* pcb, int conexion_kernel) {
    log_info(logger_CPU, "Etapa DECODE iniciada");

    switch(instruccion->op) {
        case NO_OP:

            log_info(logger_CPU, "Instruccion NO_OP");
            log_info(logger_CPU, "Etapa EXECUTE iniciada");

            usleep(retardo_noop * 1000);

            if(hay_interrupcion_para_atender()) {
                enviar_pcb(ACTUALIZAR_PCB, pcb, conexion_kernel, logger_CPU);
                destruir_proceso(pcb);
                limpiar_tlb();
                liberar_conexion(conexion_kernel);
            }
            else {
                realizar_ciclo_de_instruccion(pcb, conexion_kernel);
            }

            break;

        case I_O:

            log_info(logger_CPU, "Instruccion I_O");
            log_info(logger_CPU, "Etapa EXECUTE iniciada");

            int segundos_bloqueado = instruccion->arg[0];
            log_info(logger_CPU, "Segundos a bloquear: %u", segundos_bloqueado);
            enviar_pcb_con_tiempo_bloqueo(IO, pcb, segundos_bloqueado, conexion_kernel, logger_CPU);

            destruir_proceso(pcb);
            limpiar_tlb();
            liberar_conexion(conexion_kernel);
         
            break;

        case READ:
         
            log_info(logger_CPU, "Instruccion READ");
            log_info(logger_CPU, "Etapa EXECUTE iniciada");

            int dir_logica = instruccion->arg[0];
            //Pasar a mmu:
            int valor_leido = pedido_lectura(dir_logica, pcb, logger_CPU);
            log_info(logger_CPU, "Valor leido: %d", valor_leido);
            
            if(hay_interrupcion_para_atender()) {
                enviar_pcb(ACTUALIZAR_PCB, pcb, conexion_kernel, logger_CPU);
                destruir_proceso(pcb);
                limpiar_tlb();
                liberar_conexion(conexion_kernel);
            }
            else {
                realizar_ciclo_de_instruccion(pcb, conexion_kernel);
            }

            break;

        case WRITE:
         
            log_info(logger_CPU, "Instruccion WRITE");
            log_info(logger_CPU, "Etapa EXECUTE iniciada");

            int dir_logica2 = instruccion->arg[0];
            int valor2 = instruccion->arg[1];
            int conexion_memoria0 = crear_conexion(logger_CPU, "Memoria", ip_memoria, puerto_memoria);
            //Pasar a mmu:
            pedido_escritura(valor2, dir_logica2, pcb, conexion_memoria0, logger_CPU); //Sacar logger pq es global

            int op_code = recibir_operacion(conexion_memoria0);
            recibir_mensaje(conexion_memoria0, logger_CPU);
            close(conexion_memoria0);
            
            if(hay_interrupcion_para_atender()) {
                enviar_pcb(ACTUALIZAR_PCB, pcb, conexion_kernel, logger_CPU);
                destruir_proceso(pcb);
                limpiar_tlb();
                liberar_conexion(conexion_kernel);
            }
            else {
                realizar_ciclo_de_instruccion(pcb, conexion_kernel);
            }

            break;

        case COPY:
         
            log_info(logger_CPU, "Instruccion COPY");
            log_info(logger_CPU, "Etapa FETCH OPERANDS iniciada");

            int dir_logica_destino = instruccion->arg[0];
            int dir_logica_origen = instruccion->arg[1];
            //Pasar a mmu:
            int dato = pedido_lectura(dir_logica_origen, pcb, logger_CPU);
            
            log_info(logger_CPU, "Etapa EXECUTE iniciada");
            int conexion_memoria = crear_conexion(logger_CPU, "Memoria", ip_memoria, puerto_memoria);
            pedido_escritura(dato, dir_logica_destino, pcb, conexion_memoria,logger_CPU); //Sacar logger pq es global

            int op_code2 = recibir_operacion(conexion_memoria);
            recibir_mensaje(conexion_memoria, logger_CPU);
            close(conexion_memoria);

            if(hay_interrupcion_para_atender()) {
                enviar_pcb(ACTUALIZAR_PCB, pcb, conexion_kernel, logger_CPU);
                destruir_proceso(pcb);
                limpiar_tlb();
                liberar_conexion(conexion_kernel);
            }
            else 
                realizar_ciclo_de_instruccion(pcb, conexion_kernel);

            break;

        case EXIT:
        
            log_info(logger_CPU, "Instruccion EXIT");
            log_info(logger_CPU, "Etapa EXECUTE iniciada");
            int conexion_memoria2 = crear_conexion(logger_CPU, "Memoria", ip_memoria, puerto_memoria);
            enviar_pedido_liberar_memoria(EXIT, conexion_memoria2, pcb->id, pcb->tabla_paginas, logger_CPU);

            int op_code3 = recibir_operacion(conexion_memoria2);
            recibir_mensaje(conexion_memoria2, logger_CPU);
            close(conexion_memoria2);

            enviar_pcb(EXIT, pcb, conexion_kernel, logger_CPU);
            

            destruir_proceso(pcb);
            limpiar_tlb();
            liberar_conexion(conexion_kernel);

            break;
        
        default:
            log_error(logger_CPU, "Instruccion inválida");
    }
}

int hay_interrupcion_para_atender() {

   if(flag_interrupcion) {
       flag_interrupcion = 0;
       return 1;
   }
   else
       return 0;
}

void destruir_proceso(t_pcb* pcb) {
    free(pcb->rafaga);
    free(pcb->rafaga_bloqueado);
    //void eliminar_instruccion(t_instruccion *i)
    //{
        //free(i->arg[0]);
        //free(i->arg[1]);
        //free(i);
    //}
    //list_destroy_and_destroy_elements(pcb->instrucciones, (void*)eliminar_instruccion);
    list_destroy_and_destroy_elements(pcb->instrucciones, free);
    free(pcb);
}

#include "../include/Disco.h"

void funciones_disco()
{
    pedidos_disco = queue_create();
    sem_init(&lista_tiene_pedidos, 0, 0);
    while(1)
    {
        //semaforo 'hay pedidos?'
        sem_wait(&lista_tiene_pedidos);
        log_info(logger, "Pedido a disco");
        usleep(retardo_swap *1000);
        
        pthread_mutex_lock(&mutex_cola_pedidos);
        t_pedido_disco *p = queue_pop(pedidos_disco);
        pthread_mutex_unlock(&mutex_cola_pedidos);
        
        int id, direccion, pag;
        switch(p->operacion_disco)
        {
            case CREAR_ARCHIVO:
                id = p->argumentos[0];
                generar_nuevo_archivo(id);
            break;

            case ESCRIBIR_ARCHIVO:
                id = p->argumentos[0];
                direccion = p->argumentos[1];
                pag = p->argumentos[2];

                escribir_en_archivo(id, direccion, pag);
            break;

            case LEER_ARCHIVO:
                id = p->argumentos[0];
                direccion = p->argumentos[1];
                pag = p->argumentos[2];

                enviar_pagina_a_memoria(id, direccion, pag);
            break;

            case ELIMINAR_ARCHIVO:
                id = p->argumentos[0];
                eliminar_archivo(id);
            break;

            case DISCO_SUSPENDER_PROCESO:
                id = p->argumentos[0];
                direccion = p->argumentos[1];
                suspender_paginas(id, direccion);
            break;

            default:
                log_error(logger, "Operacion de disco invalida");
            break;
        }
        sem_post(&(p->pedido_listo));
    }
}

void generar_nuevo_archivo(int PID){
    
    struct stat buffer;
    //int exist = stat(filename,&buffer);
    
    if(stat(path_swap,&buffer)!=0){
        log_error(logger,"El directorio %s no existe",path_swap);

        system("mkdir /home/utnso/swap");

        //mkdir (char *filename);
    }    


    char * aux  = string_new();
    char * idAux = string_itoa(PID);
    
    string_append(&aux, path_swap);
    string_append(&aux, "/");
    string_append(&aux, idAux);
    string_append(&aux, ".swap.txt");

    FILE * new_file;

    new_file = fopen(aux,"w+");

    log_info(logger, "se genero el archivo: %s\n", aux);

    free(idAux);
    free(aux);
    fclose(new_file);
}


//RECORDAR LIBERAR AUX
char* devolver_path_archivo(int PID){

    char * aux = string_new();
    char * idAux = string_itoa(PID);


    string_append(&aux, path_swap);
    string_append(&aux, "/");
    string_append(&aux, idAux);
    string_append(&aux, ".swap.txt");

    free(idAux);
    return aux;  //TODO ver como liberar aux y poder devolverlo
}

void suspender_paginas(int PID, int dir_tablaN1)
{
    t_list *marcos = conseguir_marcos_proceso(dir_tablaN1);
    /*
        NOTA: este valor se usa para que en la primer vuelta
        disco no haga un usleep dos veces (una por el pedido suspension
        y otra por el primer pedido de escribir pagina)
    */
    int contador = 0;
    for(int i = 0; i < list_size(marcos); i++)
    {
        entrada_tabla_N2 *e = list_get(marcos, i);
        if(e->bit_modificacion == 1)
        {
            usleep(contador * 1000);
            contador = retardo_swap;
            escribir_en_archivo(PID, e->dir, e->num_pag);
            log_info(logger, "se guardaron los cambios de la pagina %d correctamente del proceso %d",e->num_pag,PID);
        }
            
        
        e->bit_modificacion = 0;
        e->bit_presencia = 0;
    }
    list_destroy(marcos);
}

void escribir_en_archivo(int PID, int direccion_pagina, int pagina){

    char * path = devolver_path_archivo(PID);

    FILE * file;
    file = fopen(path, "r+");
    
    fseek(file,tam_pagina*pagina,SEEK_SET);

    fwrite(memoria + direccion_pagina,tam_pagina,1,file);    
    
    fclose(file);
    free(path);
} //TODO: testear cuando podamos cargar cosas en memoria :D

void enviar_pagina_a_memoria(int PID, int direccion_pagina, int pagina){

    char * path = devolver_path_archivo(PID);

    FILE * file;

    file = fopen(path, "r");

    fseek(file,tam_pagina*pagina,SEEK_SET);

    fread(memoria + direccion_pagina,tam_pagina,1,file);

    log_info(logger,"pagina %d enviada a memoria correctamente",pagina);

    fclose(file);
    free(path);
}

void eliminar_archivo(int PID){

    char* aux = devolver_path_archivo(PID);

    if(remove(aux)==0){
        log_info(logger, "el archivo %s fue eliminado correctamente", aux);
    }
    else{
        log_error(logger, "el archivo no se pudo eliminar");
    }

    free(aux);
}

/*----------------------------------------------------------*/
t_pedido_disco* crear_pedido_crear_archivo(int id)
{
    t_pedido_disco *p = malloc(sizeof(t_pedido_disco));
    p->operacion_disco = CREAR_ARCHIVO;
    p->argumentos[0] = id;
    sem_init(&(p->pedido_listo), 0,0);

    pthread_mutex_lock(&mutex_cola_pedidos);
    queue_push(pedidos_disco, p);
    pthread_mutex_unlock(&mutex_cola_pedidos);

    sem_post(&lista_tiene_pedidos);
    return p;
}

t_pedido_disco* crear_pedido_escribir(int id, int dir_marco, int num_pag)
{
    t_pedido_disco *p = malloc(sizeof(t_pedido_disco));
    p->operacion_disco = ESCRIBIR_ARCHIVO;
    p->argumentos[0] = id;
    p->argumentos[1] = dir_marco;
    p->argumentos[2] = num_pag;
    sem_init(&(p->pedido_listo), 0, 0);

    pthread_mutex_lock(&mutex_cola_pedidos);
    queue_push(pedidos_disco, p);
    pthread_mutex_unlock(&mutex_cola_pedidos);

    sem_post(&lista_tiene_pedidos);
    return p;
}

t_pedido_disco* crear_pedido_lectura(int id, int dir_marco, int num_pag)
{
    t_pedido_disco *p = malloc(sizeof(t_pedido_disco));
    p->operacion_disco = LEER_ARCHIVO;
    p->argumentos[0] = id;
    p->argumentos[1] = dir_marco;
    p->argumentos[2] = num_pag;
    sem_init(&(p->pedido_listo), 0, 0);
    
    pthread_mutex_lock(&mutex_cola_pedidos);    
    queue_push(pedidos_disco, p);
    pthread_mutex_unlock(&mutex_cola_pedidos);
    
    sem_post(&lista_tiene_pedidos);
    return p;
}

t_pedido_disco* crear_pedido_eliminar_archivo(int id)
{   
    t_pedido_disco *p = malloc(sizeof(t_pedido_disco));
    p->operacion_disco = ELIMINAR_ARCHIVO;
    p->argumentos[0] = id;
    sem_init(&(p->pedido_listo), 0, 0);

    pthread_mutex_lock(&mutex_cola_pedidos);
    queue_push(pedidos_disco, p);
    pthread_mutex_unlock(&mutex_cola_pedidos);
    
    sem_post(&lista_tiene_pedidos);
    return p;
}

t_pedido_disco* crear_pedido_suspender_proceso(int id, int dir_tabla)
{   
    t_pedido_disco *p = malloc(sizeof(t_pedido_disco));
    p->operacion_disco = DISCO_SUSPENDER_PROCESO;
    p->argumentos[0] = id;
    p->argumentos[1] = dir_tabla;    
    sem_init(&(p->pedido_listo), 0, 0);

    pthread_mutex_lock(&mutex_cola_pedidos);
    queue_push(pedidos_disco, p);
    pthread_mutex_unlock(&mutex_cola_pedidos);

    sem_post(&lista_tiene_pedidos);
    return p;
}

void eliminar_pedido_disco(t_pedido_disco *p)
{
    //free(p->argumentos[0]);
    //free(p->argumentos[1]);
    //free(p->argumentos[2]);
    free(p);
}
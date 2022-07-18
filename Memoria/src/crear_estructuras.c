#include "../include/crear_estructuras.h"

/*----------------------MEMORIA PROCESO------------------------------------*/
void iniciar_estructuras_memoria()
{
    log_info(logger, "TAM Memoria: %d", tam_memoria);
    memoria = malloc(tam_memoria);
    crear_listas_tablas();
	marcos_memoria = crear_bitmap(tam_memoria);

    pthread_mutex_init(&mutex_procesos_en_memoria, NULL);
    pthread_mutex_init(&mutex_tablasN1, NULL);
    pthread_mutex_init(&mutex_tablasN2, NULL);
    pthread_mutex_init(&mutex_cola_pedidos, NULL);
}


proceso_en_memoria* asignar_proceso(int id, int tamanio_proceso)
{
    proceso_en_memoria *ret = malloc(sizeof(proceso_en_memoria));
    ret->id_proceso = id;
    ret->posicion_puntero_clock = 0;
    ret->tablaN1 = crear_tablaN1(tamanio_proceso);
    ret->marcos_reservados = list_create();
    sem_init(&(ret->suspension_completa), 0, 0);
    ret->esta_suspendido = 0;
    return ret;
}
/*
proceso_en_memoria* buscar_proceso_por_id(int id)
{
    proceso_en_memoria *p = list_get(procesos_en_memoria, 0);

    //NOTA: REVISAR LOGICA LOOP for PRIMER VUELTA
    for(int i = 1; p->id_proceso != id; i++)
    {
        p = list_get(procesos_en_memoria, i);
    }

    return p;
}*/

void eliminador_proceso(proceso_en_memoria *p)
{
    sem_destroy(&(p->suspension_completa));
    free(p);
}

void eliminar_estructura_proceso(int id)
{
    bool id_equals(proceso_en_memoria *p){
        return p->id_proceso == id;
    }
    pthread_mutex_lock(&mutex_procesos_en_memoria);
    list_remove_and_destroy_by_condition(procesos_en_memoria, (void*)id_equals, (void*)eliminador_proceso);
    pthread_mutex_unlock(&mutex_procesos_en_memoria);
}

entrada_tabla_N1* agregar_entrada_tablaN1(t_tablaN1 *tabla)
{
    entrada_tabla_N1* e = malloc(sizeof(entrada_tabla_N1));
    e->num_tabla = list_add(tabla, e);
    e->dir = -1;
    return e;
}

entrada_tabla_N2* agregar_entrada_tablaN2(t_tablaN2 *tabla)
{
    entrada_tabla_N2* e = malloc(sizeof(entrada_tabla_N2));
    e->num_pag = -1;
    e->dir = -1;
    e->bit_modificacion = 0;
    e->bit_presencia = 0;
    e->bit_uso = 0;
    list_add(tabla, e);
    return e;
}

int cantidad_paginas_necesarias(float tamanio_proceso)
{
    float division = tamanio_proceso / tam_pagina;
    return ceil(division);
}

int cantidad_tablas_necesarias(int paginas_necesarias)
{
    return ceil(paginas_necesarias / paginas_por_tabla);
}

t_tablaN1* crear_tablaN1(int tamanio_proceso)
{
    t_tablaN1 *t = list_create();
    int paginas_necesarias = cantidad_paginas_necesarias(tamanio_proceso);
    log_info(logger, "Paginas necesarias %d segun tamanio %d", paginas_necesarias, tamanio_proceso);

    for(int paginas_reservadas = 0; paginas_reservadas < paginas_necesarias; paginas_reservadas++)
    {
        if(paginas_reservadas % paginas_por_tabla == 0)
        {
            //NECESITO TABLA NUEVA
            //creo entrada
            entrada_tabla_N1 *e = agregar_entrada_tablaN1(t);
            //creo tabla nivel 2
            t_tablaN2 *t2 = list_create();
            //dir = index en lista general
            pthread_mutex_lock(&mutex_tablasN2);
            e->dir = list_add(tablasN2, t2);
            pthread_mutex_unlock(&mutex_tablasN2);
        }
        //conseguir ultima entrada (ultima tabla 2)
        entrada_tabla_N1 *aux = list_get(t, list_size(t) -1);
        //buscar tabla en la dir que dice la entrada de tabla 1
        t_tablaN2 *aux2 = list_get(tablasN2, aux->dir);
        entrada_tabla_N2 *aux3 = agregar_entrada_tablaN2(aux2);
        
        //DIR = NUMERO PAGINA * TAMANIO PAGINA
        aux3->num_pag = paginas_reservadas;
        aux3->dir = aux3->num_pag * tam_pagina;
        aux3->bit_presencia = 0;
    }
    return t;
}
/* VERSION ANTERIOR
void eliminar_paginas_proceso(int dir_tablaN1)
{
    t_tablaN1 *t = list_get(tablasN1, dir_tablaN1);
    t_list_iterator *iteradorN1 = list_iterator_create(t);
    while(list_iterator_has_next(iteradorN1))
    {
        entrada_tabla_N1 *e1 = list_iterator_next(iteradorN1);
        t_list_iterator *iteradorN2 = list_iterator_create(list_get(tablasN2, e1->dir));
        while(list_iterator_has_next(iteradorN2))
        {
            entrada_tabla_N2 *e2 = list_iterator_next(iteradorN2);
            if(e2->bit_presencia == 1)
                liberar_marco_proceso(e2->dir);
            //NOTA: LIBERAR PAGINA
        }
        list_iterator_destroy(iteradorN2);
        list_replace_and_destroy_element(tablasN2, e1->dir, NULL, eliminar_lista);
    }
    list_iterator_destroy(iteradorN1);
    list_replace_and_destroy_element(tablasN1, dir_tablaN1, NULL, eliminar_lista);
}*/


//TODO: tiene memory leaks
//Nota al pie: ahora hacemos un list_clean para los marcos_reservados, no un list_destroy, puede ser eso
void eliminar_paginas_proceso(int id, int dir_tablaN1)                      //TODO: chequear con el team que hacer con esta lista que quedaria x cantidad 
{                                                                       // de procesos con sus paginas en NULL
    t_tablaN1 *t = list_get(tablasN1, dir_tablaN1);
    t_list_iterator *iteradorN1 = list_iterator_create(t);
    while(list_iterator_has_next(iteradorN1))
    {
        entrada_tabla_N1 *e1 = list_iterator_next(iteradorN1);
        t_tablaN2 *t2 = list_get(tablasN2, e1->dir);

        list_clean_and_destroy_elements(t2, free);

        //list_replace_and_destroy_element(tablasN2, e1->dir, crear_tabla_vacia(), eliminar_lista);
    }
    list_clean_and_destroy_elements(t, free);
    list_iterator_destroy(iteradorN1);
    //list_replace_and_destroy_element(tablasN1, dir_tablaN1, crear_tabla_vacia(), eliminar_lista);

    liberar_marcos_proceso(id);
}

void reservar_marcos_proceso(proceso_en_memoria *p)
{
    //p->marcos_reservados = list_create();
    int cantidad_marcos_reservados = 0;
    for(int i = 0; cantidad_marcos_reservados < marcos_por_proceso; i++)
    {
        if(bitarray_test_bit(marcos_memoria, i))
        {
            int marco = i;

            list_add(p->marcos_reservados, (void*) marco);

            bitarray_clean_bit(marcos_memoria, i);
            cantidad_marcos_reservados++;
        }
    }
}

void liberar_marcos_proceso(int id)
{    
    proceso_en_memoria *aux = buscar_proceso_por_id(id);

    desmarcar_bitmap(aux->marcos_reservados);

    list_clean(aux->marcos_reservados);
    //free(aux);
}

proceso_en_memoria* buscar_proceso_por_id(int id)
{
    
    bool id_equals(proceso_en_memoria *p){
        return p->id_proceso == id;
    }
    
    proceso_en_memoria *aux = (proceso_en_memoria*)list_find(procesos_en_memoria, (void*)id_equals);
    return aux;
}

/*---------------------MEMORIA GENERAL---------------------------------*/

void desmarcar_bitmap(t_list *marcos)
{
    t_list_iterator *iterador = list_iterator_create(marcos);
    while(list_iterator_has_next(iterador))
    {
        int marco = (int) list_iterator_next(iterador);
        bitarray_set_bit(marcos_memoria, marco);
    }
    list_iterator_destroy(iterador);
}

void iniciar_memoria(void *mem, int tamanio_total)
{
    mem = malloc(tamanio_total);
}

t_bitarray* crear_bitmap(int tamanio_memoria)
{
    int cantidad_bits = ceil(tamanio_memoria / tam_pagina);
    int bytes_necesarios = ceil(cantidad_bits / 8);

    char *bitmap = malloc(bytes_necesarios);
    t_bitarray *ret = bitarray_create(bitmap, bytes_necesarios);

    for(int i = 0; i < cantidad_bits; i++)
    {
        bitarray_set_bit(ret, i);
    }                                                           //CHEQUEAR (para vos joaco) CON QUE VALOR (0,1) SE RELACIONA A VACIO/OCUPADO

    return ret;
}

void imprimir_bitmap(t_bitarray *bitmap)
{
    size_t cantidadDeBits =  bitarray_get_max_bit(bitmap);

    // int bytes_necesarios = ceil(cantidadDeBits / 8); 

    char* cadenaDeBitmap = malloc(cantidadDeBits);

    strcpy(cadenaDeBitmap, "\0");           //La usamos para limpiar la basura del char*

    for(int aux=0;aux<cantidadDeBits;aux++){
        if(bitarray_test_bit(bitmap, aux)==1){
            //cadenaDeBitmap[aux] = "1";
            string_append(&cadenaDeBitmap, "1");

        }
        else
            //cadenaDeBitmap[aux] = "0";
            string_append(&cadenaDeBitmap, "0");
    }

    log_info(logger,"el estado del bitmap es: %s",cadenaDeBitmap);

    free(cadenaDeBitmap);
}


void eliminar_bitmap(t_bitarray *bitmap)
{
    free(bitmap->bitarray);
    free(bitmap);
}

void eliminador(void *info)
{
    if(info != NULL)
        eliminar_lista(info);
}

void eliminar_lista(void *l)
{
    list_destroy_and_destroy_elements(l, free);
}

void crear_listas_tablas()
{
    tablasN1 = list_create();
    tablasN2 = list_create();
    procesos_en_memoria = list_create();
}

void eliminar_listas_tablas()
{
    list_destroy_and_destroy_elements(tablasN1, eliminador);
    list_destroy_and_destroy_elements(tablasN2, eliminador);
}

int conseguir_marco_de_dir_fisica(int dir)
{
    return (int)floor(dir / tam_pagina);
}

entrada_tabla_N2* tabla_contiene_marco(t_tablaN2 *t, int num_marco)
{
    entrada_tabla_N2 *ret;
    for(int i = 0; i < list_size(t); i++)
    {
        ret = list_get(t, i);
        if(ret->bit_presencia == 1 && ret->dir == num_marco * tam_pagina)
        {
            return ret;
        }
    }
    return NULL;
}

entrada_tabla_N2* conseguir_pagina_en_marco(int num_marco)
{
    t_list_iterator *iterador = list_iterator_create(tablasN2);
    entrada_tabla_N2 *ret = NULL;
    //int size_tablasN2 = list_size(tablasN2);

    //t_tablaN2 *t = list_iterator_next(iterador);
    //t_tablaN2 *t = (t_tablaN2*) list_get(tablasN2, indice);
    t_tablaN2 *t;
    //while(indice < size_tablasN2)
    while(list_iterator_has_next(iterador))
    {
        //NOTA: se reemplazan la tablas por NULL cuando se elimina el proceso
        //por eso list_iterator_next() podria dar valores nulos
        t = list_iterator_next(iterador);
        
        ret = tabla_contiene_marco(t, num_marco);
        if(ret != NULL)
        {
            list_iterator_destroy(iterador);
            return ret;
        }
    }
    list_iterator_destroy(iterador);
    return ret;  
}

entrada_tabla_N2* conseguir_entrada_pagina(int dir_tablaN1, int pag)
{
    //conseguir tabla nivel 1
    t_tablaN1 *t = list_get(tablasN1, dir_tablaN1);

    //numero entrada = division numero pagina por paginas tablas redondeado para arriba
    int num_entrada_n1 = ceil(pag / paginas_por_tabla);
    entrada_tabla_N1 *e1 = list_get(t, num_entrada_n1);

    //conseguir tabla nivel 2
    t_tablaN2 *t2 = list_get(tablasN2, e1->dir);

    //desplazamiento en tabla = resto de division anterior
    int num_entrada_n2 = pag % paginas_por_tabla;
    entrada_tabla_N2 *e2 = list_get(t2, num_entrada_n2);
    return e2;
}

t_list* conseguir_numeros_marcos_proceso(int id)
{
    proceso_en_memoria *ret = buscar_proceso_por_id(id);
    //proceso_en_memoria *ret = list_get(procesos_en_memoria, 0);
    //for(int i = 1; ret->id_proceso != id; i++)
    //{
        //ret = list_get(procesos_en_memoria, i);
    //}
    return ret->marcos_reservados;
}

t_list* conseguir_marcos_proceso(int dir_tablaN1)
{
    t_tablaN1 *t = list_get(tablasN1, dir_tablaN1);
    t_list *marcos = list_create();
    t_list_iterator *iterador = list_iterator_create(t);
    while(list_iterator_has_next(iterador))
    {
        //ENTRADA TIENE DIR DE TABLA NIVEL 2
        entrada_tabla_N1 *e1 = list_iterator_next(iterador);
        //CONSIGUE TABLA
        t_list_iterator *iterador2 = list_iterator_create(list_get(tablasN2, e1->dir));
        while(list_iterator_has_next(iterador2))
        {
            //CONSIGUE ENTRADA DE TABLA N2
            entrada_tabla_N2 *e2 = list_iterator_next(iterador2);
            if(e2->bit_presencia == 1)
            {
                list_add(marcos, e2);
            }
        }
        list_iterator_destroy(iterador2);
    }
    list_iterator_destroy(iterador);
    return marcos;
}
#include "../include/algoritmos_paginas.h"

int criterio_clock(entrada_tabla_N2 *e)
{
    if(e == NULL)
        return 0;
    if(e->bit_uso == 1)
    {
        //CAMBIAR BIT DE USO Y SIGUE BUSCANDO
        e->bit_uso = 0;
        return 0;
    }
    //SINO TERMINA DE BUSCAR
    else
        return 1;
}

int criterio_clock_mejorado(entrada_tabla_N2 *e, int vuelta)
{
    switch(vuelta)
    {
        case 0:
        //PRIMER VUELTA SOLO VERIFICA POR MEJOR CASO
        return (e->bit_uso == 0 && e->bit_modificacion == 0);
        break;
        
        case 1:
        //SEGUNDA VUELTA VERIFICA POR SEGUNDO CASO ACEPTABLE
        //MODIFICA BIT USO
        if(e->bit_uso == 0 && e->bit_modificacion == 1)
            return 1;
        else
        {
            e->bit_uso = 0;
            return 0;
        }
        break;

        case 2:
        //TERCER VUELTA VUELVE A VERIFICAR MEJOR CASO
        return (e->bit_uso == 0 && e->bit_modificacion == 0);
        break;

        case 3:
        //CUARTA VUELTA VUELVE A VERIFICAR POR SEGUNDO CASO ACEPTABLE
        return (e->bit_uso == 0 && e->bit_modificacion == 1);
        break;

        default:
        return 0;
        break;
    }
}

entrada_tabla_N2* aplicar_busqueda_clock(int id, int dir_tablaN1)
{
    //TODO: Organizar por orden de marco
    proceso_en_memoria *p = buscar_proceso_por_id(id);
    t_list *marcos_proceso = conseguir_marcos_proceso(dir_tablaN1);

    //NOTA: revisar logica en primer vuelta de for()
    //se toma valor en siguiente linea para no empezar el for con valor nulo en ret
    //entrada_tabla_N2 *ret = list_get(marcos_proceso, 0);
    entrada_tabla_N2 *ret = NULL;
    while(criterio_clock(ret) == 0)
    {
        //DIO UNA VUELTA
        ret = list_get(marcos_proceso, p->posicion_puntero_clock);
        p->posicion_puntero_clock++;
        if(p->posicion_puntero_clock == list_size(marcos_proceso))
            p->posicion_puntero_clock = 0;
    }

    list_destroy(marcos_proceso);
    log_info(logger, "Pagina a reemplazar %d en marco %d", ret->num_pag, ret->dir);
    //NOTA: podria eliminar los elementos en la lista principal (chequear despues)
    return ret;
}

entrada_tabla_N2* aplicar_busqueda_clock_mejorado(int id, int dir_tablaN1)
{
    proceso_en_memoria *p = buscar_proceso_por_id(id);
    t_list *marcos_proceso = conseguir_marcos_proceso(dir_tablaN1);

    entrada_tabla_N2 *ret = list_get(marcos_proceso, p->posicion_puntero_clock);
    int vuelta = 0;
    int pos_inicial = p->posicion_puntero_clock;
    /*for(int i = 0; i < marcos_por_proceso; i++)
    {
        if(p->posicion_puntero_clock == list_size(marcos_proceso))
        {
            p->posicion_puntero_clock = 0;
        }
        if(criterio_clock_mejorado(ret, vuelta))
            break;
    }
    return ret;*/

    while(criterio_clock_mejorado(ret, vuelta))
    {
        ret = list_get(marcos_proceso, p->posicion_puntero_clock);
        p->posicion_puntero_clock++;
        if(p->posicion_puntero_clock == list_size(marcos_proceso))
        {
            p->posicion_puntero_clock = 0;
        }
        if(p->posicion_puntero_clock == pos_inicial)
        {
            vuelta++;
        }
    }

    //list_destroy_and_destroy_elements(marcos_proceso, free);
    list_destroy(marcos_proceso);
    log_info(logger, "Pagina a reemplazar %d en marco %d", ret->num_pag, ret->dir);
    //NOTA: podria eliminar los elementos en la lista principal (chequear despues)
    return ret;
}
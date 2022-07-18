#include "../include/leer_codigo.h"

static char* codigos_instrucciones[6] = {
    "NO_OP",
    "I/O",
    "READ",
    "WRITE",
    "COPY",
    "EXIT"
};

t_list* leer_archivo(char *path)
{
    FILE *codigo = fopen(path, "r");
    char *linea = (char*)malloc(LARGO_LINEA_MAX);

    t_list* lista_instrucciones = list_create();

    while(!feof(codigo))
    {
        fgets(linea, LARGO_LINEA_MAX, codigo);
        if(strcmp(linea, "") != 0)
        {
            agregar_instruccion_a_lista(linea,lista_instrucciones);
        }
    }

    free(linea);
    fclose(codigo);
    return lista_instrucciones;
}

void agregar_instruccion_a_lista(char* linea, t_list* lista_instrucciones) {
    t_instruccion *instruccion = malloc(sizeof(t_instruccion));

    char **palabras_linea = string_split(linea, " ");

    int op;
    for(op = 0; strcmp(palabras_linea[0], codigos_instrucciones[op]) != 0 && op < 6; op++);

    instruccion->op = op;
    switch(instruccion->op)
    {
        case NO_OP:
            instruccion->arg[0] = -1;
            instruccion->arg[1] = -1;
            for(int i = 0; i< atoi(palabras_linea[1]); i++) {
                list_add(lista_instrucciones, instruccion);
            }
            free(palabras_linea[0]);
            free(palabras_linea[1]);
            break;

        case I_O:
            instruccion->arg[0] = atoi(palabras_linea[1]);
            instruccion->arg[1] = -1;
            free(palabras_linea[0]);
            free(palabras_linea[1]);
            list_add(lista_instrucciones, instruccion);
            
            break;

        case READ:
            instruccion->arg[0] = atoi(palabras_linea[1]);
            instruccion->arg[1] = -1;
            free(palabras_linea[0]);
            free(palabras_linea[1]);
            list_add(lista_instrucciones, instruccion);
            break;

        case WRITE:
            instruccion->arg[0] = atoi(palabras_linea[1]);
            instruccion->arg[1] = atoi(palabras_linea[2]);
            free(palabras_linea[0]);
            free(palabras_linea[1]);
            free(palabras_linea[2]);
            list_add(lista_instrucciones, instruccion);
            break;

        case COPY:
            instruccion->arg[0] = atoi(palabras_linea[1]);
            instruccion->arg[1] = atoi(palabras_linea[2]);
            free(palabras_linea[0]);
            free(palabras_linea[1]);
            free(palabras_linea[2]);
            list_add(lista_instrucciones, instruccion);
            break;

        case EXIT_INST:
            instruccion->arg[0] = -1;
            instruccion->arg[1] = -1;
            free(palabras_linea[0]);
            list_add(lista_instrucciones, instruccion);
            break;

        default:
            //ERROR: OPERACION NO VALIDA
            break;
    }

    free(palabras_linea);
    //free(instruccion);
}
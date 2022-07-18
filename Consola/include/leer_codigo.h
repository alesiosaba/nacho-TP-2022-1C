#ifndef LEER_CODIGO
#define LEER_CODIGO

#include <stdlib.h>
#include <commons/string.h>
#include "../../shared/include/shared_mensajes.h"

#define LARGO_LINEA_MAX 20

char* leer_instruccion(FILE *f);
t_list* leer_archivo(char *path);
void agregar_instruccion_a_paquete(char* linea, t_paquete* paquete);
void agregar_instruccion_a_lista(char* linea, t_list* lista_instrucciones);

#endif
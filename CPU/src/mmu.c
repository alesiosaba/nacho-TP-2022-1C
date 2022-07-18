#include "../include/mmu.h"

int traducir_dir_logica(float dir, t_pcb *proceso, t_log *logger)
{
	log_info(logger, "Dir lógica: %d", (int) dir);
	int numero_pagina = floor(dir / tam_pagina);
	//TODO Probar dir con int vs float
	int desplazamiento = dir - numero_pagina * tam_pagina;

	//BUSCAR EN TLB
	int result_tlb = buscar_pagina_tlb(numero_pagina);
	//SI NO ES PAGE FAULT, RETORNAR RESULTADO
	if(result_tlb != -1)
	{
		log_info(logger,"TLB Hit");
		return result_tlb + desplazamiento;
	}
	log_info(logger, "TLB Miss");

	int dir_tablaN1 = proceso->tabla_paginas;
	log_info(logger,"Dir tabla de proceso: %d", proceso->tabla_paginas);
	int dir_entradaN1 = floor(numero_pagina / paginas_por_tabla);

	//(SOLICITUD TABLA)
	//ENVIAR DIR TABLA N1 Y NUM ENTRADA TABLA NIVEL 1
	t_paquete *primer_acceso = crear_paquete(SOLICITUD_TABLA_PAGINAS);
	agregar_a_paquete(primer_acceso, &dir_tablaN1, sizeof(int));
	agregar_a_paquete(primer_acceso, &dir_entradaN1, sizeof(int));

	int conexion_memoria = crear_conexion(logger, "Memoria", ip_memoria, puerto_memoria);
	enviar_paquete(primer_acceso, conexion_memoria, logger);

	eliminar_paquete(primer_acceso);

	//RECIBIR TABLA N2
	recibir_operacion(conexion_memoria);
	t_tablaN2 *tablaN2 = recibir_tabla_N2(conexion_memoria, logger);
	close(conexion_memoria);

	for(int i = 0; i < list_size(tablaN2); i++)
	{
		entrada_tabla_N2 *e = list_get(tablaN2, i);
		/*log_info(logger,"Entrada num: %d Dir pag: %d Bit presencia: %d",
		e->num_pag,
		e->dir,
		e->bit_presencia
		);*/
	}

	int dir_entradaN2 = numero_pagina % paginas_por_tabla;
	entrada_tabla_N2 *e2 = list_get(tablaN2, dir_entradaN2);
	log_info(logger_CPU,"Entrada buscada: %d", e2->num_pag);
	
	//(SOLICITUD MARCO)
	//ENVIAR DIR PAGINA
	t_paquete *segundo_acceso = crear_paquete(SOLICITUD_MARCO);
	agregar_a_paquete(segundo_acceso, &(proceso->id), sizeof(int));
	agregar_a_paquete(segundo_acceso, &(proceso->tabla_paginas), sizeof(int));
	agregar_a_paquete(segundo_acceso, &(e2->num_pag), sizeof(int));
	
	conexion_memoria = crear_conexion(logger_CPU, "Memoria", ip_memoria, puerto_memoria);
	enviar_paquete(segundo_acceso, conexion_memoria, logger);
	//SI BIT PRESENCIA == 0 → TRAER PAGINA
	//RECIBIR DIR MARCO
	int dir_marco = recibir_num(conexion_memoria, logger);
	close(conexion_memoria);

	log_info(logger_CPU, "Marco recibido %d", dir_marco);

	//NO HUBO TLB HIT, ENTONCES LA AGREGA
	if(result_tlb == -1)
		agregar_entrada_tlb(numero_pagina, dir_marco);

	//int desplazamiento = dir - numero_pagina * tam_pagina;
	log_info(logger_CPU, "Direccion fisica: %d", dir_marco + desplazamiento);

	eliminar_paquete(segundo_acceso);
	list_destroy_and_destroy_elements(tablaN2, free);
	
	return dir_marco + desplazamiento;
}

int exceso_de_dato_en_pagina(int dir_logica)
{
	int desplazamiento = dir_logica % tam_pagina;
	//RETORNA EXCESO O CERO
	return max(desplazamiento + sizeof(int) - tam_pagina, 0);
}

int resto_pagina(int dir_logica)
{
	return tam_pagina - (dir_logica % tam_pagina);
}

int dir_resto_dato(int bytes_por_procesar)
{
	int desplazamiento = sizeof(int) - bytes_por_procesar;
	return desplazamiento;
}

void pedido_escritura(int valor, int dir_logica, t_pcb *proceso, int conexion_memoria, t_log *logger)
{
	int resto_pag = resto_pagina(dir_logica);
	int bytes_por_procesar = sizeof(int);
	while(bytes_por_procesar > 0)
	{
		int dir_fisica = traducir_dir_logica(dir_logica, proceso, logger);
		//ENVIAR DIR CON PEDIDO Y TAMAÑO bytes_por_procesar
		enviar_pedido_escritura(dir_fisica,
			valor, 
			conexion_memoria, 
			logger
		);
		bytes_por_procesar = 0;
	}
}

int pedido_lectura(int dir_logica, t_pcb *proceso, t_log *logger)
{
	int resto_pag = resto_pagina(dir_logica);
	int bytes_por_procesar = sizeof(int);

	//int *dato = malloc(sizeof(int));
	uint32_t dato;
	//int desplazamiento = 0;
	while(bytes_por_procesar > 0) {
		int dir_fisica = traducir_dir_logica(dir_logica, proceso, logger);
		int conexion_memoria = crear_conexion(logger_CPU, "Memoria", ip_memoria, puerto_memoria);
		enviar_pedido_lectura(dir_fisica, conexion_memoria, logger);

		//RECIBIR VALOR
		recibir_operacion(conexion_memoria);

		dato = recibir_valor_leido(conexion_memoria, logger);

		bytes_por_procesar = 0;

	}
	return dato;
}

int max(int a,int b) {
    if(a > b)
        return a;
    return b;
}
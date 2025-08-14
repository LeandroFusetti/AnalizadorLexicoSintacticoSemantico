/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include <stdio.h>
#include <stdlib.h>
#include "general.h"
#include <ctype.h>
#include <string.h>
extern YYLTYPE yylloc;

void pausa(void){
    printf("Presione ENTER para continuar...\n");
    getchar();
}

void inicializarUbicacion(void){
    yylloc.first_line=yylloc.last_line = INICIO_CONTEO_LINEA;
    yylloc.first_column= yylloc.last_column = INICIO_CONTEO_COLUMNA;
}

/* void newLine(){
   yylloc.first_line=yylloc.last_line++;
    yylloc.last_column=INICIO_CONTEO_COLUMNA;
} */
void updateColumn(int n){
    yylloc.last_column += n;
}

extern tNodo* listaVar;
extern tNodo* listaSentencias;
extern tNodo* listaVarComp;
extern tNodo* listaRow;
extern parameter* listaParam;
extern tNodo* listaFunc;
extern tNodo* listaIF;
extern tNodo* listaNoReconocidas;
extern tNodo* listaEstructurasNoReconocidas;
FILE *log_file;

void limpiarArchivo(const char *nombreArchivo){
    // Abrir el archivo en modo escritura ("w") para truncarlo
    FILE *archivo = fopen("parser_log.txt", "w");
    
    // Verificar si el archivo se abrió correctamente
    if(archivo == NULL){
        perror("Error al abrir el archivo");
        return;
    }
    
    // Cerrar el archivo
    fclose(archivo);
}

tNodo* insertarAlFinal(tNodo* lista,tInfo info){
    tNodo* p = (tNodo*)malloc(sizeof(tNodo));
    p->info = info;

    if(lista == NULL){
        p->sgte = lista;
        lista = p;
    }
    else{
        tNodo* q = lista;

        while(q->sgte != NULL){//strcmp(valor.id, q->sgte->info.id) > 0
            q = q->sgte;
        }

        p->sgte = q->sgte;
        q->sgte = p;
    }

    return lista;
}

void varManager(char* type,int row){
    tInfo info;

    while(listaVarComp){
        info.id=listaVarComp->info.id;
        info.row=row;
        info.type=type;
        listaVar=insertarAlFinal(listaVar,info);
        listaVarComp=listaVarComp->sgte;
    }
}

tNodo* insertarOrdenadoIds(tNodo* lista, tInfo valor){
    tNodo* p = (tNodo*)malloc(sizeof(tNodo));
    p->info = valor;

    if(lista == NULL || valor.row < lista->info.row){
        p->sgte = lista;
        lista = p;
    }
    else{
        tNodo* q = lista;
        while(q->sgte != NULL && valor.row >= q->sgte->info.row){
            q = q->sgte;
        }
        p->sgte = q->sgte;
        q->sgte = p;
    }

    return lista;
}

tNodo* insertaInicio(tNodo* lista,tInfo info){
    tNodo* p = (tNodo*)malloc(sizeof(tNodo));
    p->info = info;
    p->sgte = lista;
    return p;
}

void printVarList(tNodo* lista){
    printf("* Listado de variables declaradas (tipo de dato y numero de linea):\n");

    while(lista != NULL){
        printf("%s: %s, linea %d\n", lista->info.id, lista->info.type, lista->info.row);
        lista = lista->sgte;
    }
}

void statementManager(char* statement){ // este sirve pra el if/ if else
    if(listaRow!=NULL){
        tInfo info;
        info.sentencia = strdup(statement);
        info.row = listaRow->info.row;
        info.column = listaRow->info.column;
        listaSentencias = insertarAlFinal(listaSentencias,info);
        listaRow=listaRow->sgte;
    }
}

void printStatementList(tNodo* lista){
    printf("\n* Listado de sentencias indicando tipo, numero de linea y de columna:\n");

    while(listaIF){
        lista = insertarOrdenadoIds(lista, listaIF->info);
        listaIF = listaIF->sgte;
    }

    while(listaSentencias){
        lista = insertarOrdenadoIds(lista, listaSentencias->info);
        listaSentencias = listaSentencias->sgte;
    }
    
    while(lista != NULL){
        printf("%s: linea %d, columna %d\n", lista->info.sentencia, lista->info.row, lista->info.column);
        lista = lista->sgte;
    }
}

void otherStatementManager(char* statement,int row,int column){
    tInfo info;
    info.sentencia = strdup(statement);
    info.row = row ;
    info.column = column;
    listaSentencias = insertarAlFinal(listaSentencias, info);
}

void posManager(int row,int column){
    tInfo aux;
    aux.row = row;
    aux.column = column;
    listaRow = insertarAlFinal(listaRow,aux);
}

parameter* insertarAlFinalParam(parameter* lista, struct param info){
    parameter* p = (parameter*)malloc(sizeof(parameter));
    p->info = info;

    if(lista == NULL){
        p->sgte = lista;
        lista = p;
    }
    else{
        parameter* q = lista;

        while (q->sgte != NULL){//strcmp(valor.id, q->sgte->info.id) > 0
            q = q->sgte;
        }

        p->sgte = q->sgte;
        q->sgte = p;
    }

    return lista;
}

void parameterManager(char* id,char* type){
    struct param info;
    info.id = id;
    info.tipo = type;
    listaParam = insertarAlFinalParam(listaParam, info);
}

// Función para formatear los parámetros como "tipo1 id1, tipo2 id2"
char* formatearParametros(parameter* cabeza){
    parameter* temp = cabeza;
    int size = 0;
    
    // Calcular el tamaño requerido para el string
    while(temp != NULL){
        size += strlen(temp->info.tipo) + strlen(temp->info.id) + 3; // 1 espacio y 2 comas o '\0'
        temp = temp->sgte;
    }

    // Reservar memoria para el string resultante
    char* resultado = (char*)malloc(size * sizeof(char));

    if(resultado == NULL){
        printf("Error al reservar memoria.\n");
        exit(1);
    }

    resultado[0] = '\0'; // Inicializar el string vacío

    // Concatenar los elementos de la lista en el formato requerido
    
    while(cabeza != NULL){
        strcat(resultado, cabeza->info.tipo);
        if(strcmp(cabeza->info.id,"") != 0){

            strcat(resultado," ");
        }
        strcat(resultado, cabeza->info.id);

        if(cabeza->sgte != NULL){
            strcat(resultado, ", ");
        }

        cabeza = cabeza->sgte;
    }

    listaParam = NULL;

    return resultado; // Devolver el string formateado
}

void liberarLista(parameter* cabeza){
    while(cabeza != NULL){
        cabeza = cabeza->sgte;
        free(cabeza);
    }
}

void functionManager(char*id,char* tipo,short proto,int row){
    char *cadenaFinal;
    tInfo aux;
    aux.id = id;
    aux.type = tipo;
    aux.listaParametros = listaParam;
    aux.proto = proto;
    aux.row = row;
    char* parametros = formatearParametros(listaParam);

    if(aux.proto == 0){    
        int ret = asprintf(&cadenaFinal, "%s: definicion, input: %s, retorna: %s, linea %d\n", aux.id, parametros, aux.type, aux.row);
    }
    else{
        int ret = asprintf(&cadenaFinal, "%s: declaracion, input: %s, retorna: %s, linea %d\n", aux.id, parametros, aux.type, aux.row); 
    }

    aux.cadenaFinal = cadenaFinal;
    listaFunc = insertarAlFinal(listaFunc, aux);
}

void printFunction(tNodo* lista){
    printf("\n* Listado de funciones declaradas o definidas:\n");
    char* resultado;

    while(lista != NULL){
        printf("%s", lista->info.cadenaFinal);
        lista = lista->sgte;
    }
}

void ifManager(int linea,int columna,int casoAparte){
    tInfo aux;
    aux.row = linea;
    aux.column = columna-2;

    if(casoAparte == 0){
       aux.sentencia = "if";
    }
    else{
        aux.sentencia = "if/else";
    }

    listaIF = insertarAlFinal(listaIF, aux);
}

void printIF(tNodo* lista){

    while(lista != NULL){
        printf("%s", lista->info.cadenaFinal);
        lista = lista->sgte;
    }
}

void noReconocidasManager(char* output, int fila, int columna){
    tInfo aux;
    aux.cadenaFinal = strdup(output);
    aux.row = fila;
    aux.column = columna;
    listaNoReconocidas = insertarAlFinal(listaNoReconocidas, aux);
}

void printNoReconocidas(tNodo* lista){
    printf("\n* Listado de cadenas no reconocidas:\n");

    while(lista != NULL){
        printf("%s: linea %d, columna %d\n", lista->info.cadenaFinal, lista->info.row, lista->info.column);
        lista = lista->sgte;
    }
}

void printEstructurasNoReconocidas(tNodo* lista){
    printf("\n* Listado de estructuras sintácticas no reconocidas\n");

    while(lista != NULL){
        printf("\"%s\": linea %d\n", lista->info.sentencia, lista->info.row);
        lista = lista->sgte;
    }
}

char* trim(char* str) {
    char* inicio = str; // Puntero al inicio de la cadena

    // Saltar los espacios al inicio
    while (*inicio && isspace((unsigned char)*inicio)) {
        inicio++;
    }

    // Si la cadena está vacía después de eliminar espacios al inicio
    if (*inicio == '\0') {
        return inicio; // Retorna cadena vacía
    }

    // Puntero al final de la cadena
    char* fin = inicio + strlen(inicio) - 1;

    // Saltar los espacios en blanco al final
    while (fin > inicio && isspace((unsigned char)*fin)) {
        fin--;
    }

    // Colocar el terminador nulo para cortar la cadena
    *(fin + 1) = '\0';

    // Mover la cadena ajustada al principio
    memmove(str, inicio, (fin - inicio + 2) * sizeof(char)); // +2 para incluir el terminador nulo
    return str; // Retorna la cadena recortada
}

char* obtenerLineaDelError(const char* nombreArchivo, int numeroLinea) {
    FILE* archivo = fopen(nombreArchivo, "r");  // Abrir el archivo en modo lectura
    if (archivo == NULL) {
        printf("Error al abrir el archivo %s.\n", nombreArchivo);
        return NULL;
    }

    char* linea = NULL;
    size_t longitud = 0;  // Inicialización requerida para getline
    int contador = 0;

    // Leer el archivo línea por línea
    while (getline(&linea, &longitud, archivo) != -1) {
        contador++;
        if (contador == numeroLinea) {
            fclose(archivo);  // Cerrar el archivo
            return trim(linea);     // Devolver la línea encontrada
        }
    }

    // Si no se encuentra la línea solicitada
    fclose(archivo);
    if (linea) {
        free(linea);  // Liberar la memoria si se asignó con getline
    }
    printf("No se encontró la línea solicitada.\n");
    return NULL;
}

char* string_trim(char* str){
    char* inicio = str;
    char* final = str + strlen(str) - 1;


    while(isspace((unsigned char)inicio)){
        inicio++;
    }


    while(final > inicio && isspace((unsigned char)final)){
        final--;
    }

    *(final + 1) = '\0';

    return inicio;
}

void manejarError(char* texto, int linea) {
    char* textoAux = strdup(texto);
    tInfo aux;
    aux.sentencia = textoAux;
    aux.row = linea-1;
    listaEstructurasNoReconocidas = insertarAlFinal(listaEstructurasNoReconocidas, aux);
}
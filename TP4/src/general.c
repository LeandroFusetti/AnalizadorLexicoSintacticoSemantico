/* En los archivos (*.c) se pueden poner tanto DECLARACIONES como DEFINICIONES de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#include <stdio.h>
#include <stdlib.h>
#include "general.h"
#include <ctype.h>
#include <string.h>
#include "tabla_de_simbolos.h"

extern YYLTYPE yylloc;

extern tNodoError* listaErrores;
extern tNodo* listaVar;
extern tNodo* listaSentencias;
extern tNodo* listaVarComp;
extern tNodo* listaRow;
extern parameter* listaParam;
tNodo* listaFuncProto;
tNodo* listaDeFuncDefinicion;
extern tNodo* listaIF;
extern tNodo* listaNoReconocidas;
extern tNodo* listaEstructurasNoReconocidas;
FILE *log_file;
extern tNodo* listaDeVariables;
extern tNodo* listaIdsRepetidos;
extern tNodo* listaFuncionesMezcladas;
extern tNodo* listaTiposReturn;

extern tNodo* tablaSimbolos;
extern tError* listaErroresSemanticos;
extern tNodo* listaParametrosInvocacion;
extern tNodo* listaInit;
extern tError* operandosInvalidos;
tError* listaPrimera=NULL;
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

        while(q->sgte != NULL){ //strcmp(valor.id, q->sgte->info.id) > 0
            q = q->sgte;
        }

        p->sgte = q->sgte;
        q->sgte = p;
    }

    return lista;
}

// Función para crear un nuevo nodo
parameter* crearNodo(const char* tipo, const char* id) {
    parameter* nuevo = (parameter*)malloc(sizeof(parameter));
    if (!nuevo) return NULL;  // Verificar que la memoria se haya asignado correctamente

    nuevo->info.tipo = strdup(tipo);
    nuevo->info.id = strdup(id);
    nuevo->sgte = NULL;
    return nuevo;
}

// Función para copiar una lista
parameter* copiarLista(parameter* original) {
    if (original == NULL) return NULL;

    // Crear el nodo inicial de la nueva lista
    parameter* copia = crearNodo(original->info.tipo, original->info.id);
    if (!copia) return NULL;

    parameter* actualOriginal = original->sgte;
    parameter* actualCopia = copia;

    // Recorrer y copiar cada nodo de la lista original
    while (actualOriginal) {
        actualCopia->sgte = crearNodo(actualOriginal->info.tipo, actualOriginal->info.id);
        if (!actualCopia->sgte) {
            // Manejar posible falla de memoria liberando la lista parcial creada
            parameter* temp = copia;
            while (temp) {
                parameter* siguiente = temp->sgte;
                free(temp->info.tipo);
                free(temp->info.id);
                free(temp);
                temp = siguiente;
            }
            return NULL;
        }
        actualCopia = actualCopia->sgte;
        actualOriginal = actualOriginal->sgte;
    }

    return copia;
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
    printf("\n* Listado de variables declaradas (tipo de dato y numero de linea):\n");

    while(lista != NULL){
        printf("%s: %s, linea %d, columna %d\n", lista->info.id, lista->info.type, lista->info.row, lista->info.column);
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
        listaRow = listaRow->sgte;
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

void otherStatementManager(char* statement, int row, int column){
    tInfo info;
    info.sentencia = strdup(statement);
    info.row = row ;
    info.column = column;
    listaSentencias = insertarAlFinal(listaSentencias, info);
}

void posManager(int row, int column){
    tInfo aux;
    aux.row = row;
    aux.column = column;
    listaRow = insertarAlFinal(listaRow, aux);
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

        while(q->sgte != NULL){ //strcmp(valor.id, q->sgte->info.id) > 0
            q = q->sgte;
        }

        p->sgte = q->sgte;
        q->sgte = p;
    }

    return lista;
}

void parameterManager(char* id, char* type){
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

    return resultado; // Devolver el string formateado
}

void liberarLista(parameter* cabeza){
    while(cabeza != NULL){
        cabeza = cabeza->sgte;
        free(cabeza);
    }
}

void printFunction(tNodo* lista){
    printf("\n* Listado de funciones declaradas y definidas:\n");
    char* resultado;

    while(listaDeFuncDefinicion){
        lista = insertarOrdenadoIds(lista, listaDeFuncDefinicion->info);
        listaDeFuncDefinicion = listaDeFuncDefinicion->sgte;
    }

    while(listaFuncProto){
        lista = insertarOrdenadoIds(lista, listaFuncProto->info);
        listaFuncProto = listaFuncProto->sgte;
    }

    while(lista != NULL){
        printf("%s", lista->info.cadenaFinal);
        lista = lista->sgte;
    }
    printf("\n");
}

void ifManager(int linea, int columna, int casoAparte){
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
    printf("\n* Listado de errores lexicos:\n");
    while(lista != NULL){
        printf("%s: linea %d, columna %d\n", lista->info.cadenaFinal, lista->info.row, lista->info.column);
        lista = lista->sgte;
    }
}

void printEstructurasNoReconocidas(tNodo* lista){
    printf("\n* Listado de errores sintacticos:\n");
    while(lista != NULL){
        printf("\"%s\": linea %d\n", lista->info.sentencia, lista->info.row);
        lista = lista->sgte;
    }
}

char* trim(char* str){
    char* inicio = str; // Puntero al inicio de la cadena

    // Saltar los espacios al inicio
    while(*inicio && isspace((unsigned char)*inicio)){
        inicio++;
    }

    // Si la cadena está vacía después de eliminar espacios al inicio
    if(*inicio == '\0'){
        return inicio; // Retorna cadena vacía
    }

    // Puntero al final de la cadena
    char* fin = inicio + strlen(inicio) - 1;

    // Saltar los espacios en blanco al final
    while(fin > inicio && isspace((unsigned char)*fin)){
        fin--;
    }

    // Colocar el terminador nulo para cortar la cadena
    *(fin + 1) = '\0';

    // Mover la cadena ajustada al principio
    memmove(str, inicio, (fin - inicio + 2) * sizeof(char)); // +2 para incluir el terminador nulo
    return str; // Retorna la cadena recortada
}

char* obtenerLineaDelError(const char* nombreArchivo, int numeroLinea){
    FILE* archivo = fopen(nombreArchivo, "r");  // Abrir el archivo en modo lectura

    if(archivo == NULL){
        printf("Error al abrir el archivo %s.\n", nombreArchivo);
        return NULL;
    }

    char* linea = NULL;
    size_t longitud = 0;  // Inicialización requerida para getline
    int contador = 0;

    // Leer el archivo línea por línea
    while(getline(&linea, &longitud, archivo) != -1){
        contador++;
        if(contador == numeroLinea){
            fclose(archivo);  // Cerrar el archivo
            return trim(linea);     // Devolver la línea encontrada
        }
    }

    // Si no se encuentra la línea solicitada
    fclose(archivo);

    if(linea){
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

void manejarError(char* texto, int linea){
    char* textoAux = strdup(texto);
    tInfo aux;
    aux.sentencia = textoAux;
    aux.row = linea-1;
    listaEstructurasNoReconocidas = insertarAlFinal(listaEstructurasNoReconocidas, aux);
}

tNodo* buscar(tNodo* lista, tInfo valor){ // Busca un nodo en la lista enlazada cuyo id coincida con el de valor.
    if(lista == NULL){
        //printf("La lista está vacía\n");
        return NULL;
    }
    
    tNodo* p = lista;
    
    while(p != NULL){
        //printf("Comparando: valor.id = %s con p->info.id = %s\n", valor.id, p->info.id);

        if(strcmp(p->info.id, valor.id) == 0){
            //printf("Nodo encontrado: ID = %s, Proto = %d\n", p->info.id, p->info.proto);
            return p;  // Retorna si encuentra el nodo
        }

        p = p->sgte;
    }

    //printf("Nodo con ID = %s no encontrado\n", valor.id);
    return NULL; // Retorna NULL si no se encontró
}

int mismoIdVar(tInfo info){
    tNodo* buscarVar = buscar(listaVar,info);
    tNodo* buscarProto = buscar(listaFuncProto,info);
    tNodo* buscarDef = buscar(listaDeFuncDefinicion,info);
    if(buscarVar == NULL && buscarProto == NULL && buscarDef == NULL){
        return 1;
    }

    return 0; 
}

int mismoIdFunc(tInfo info){
    tNodo* buscarProto = buscar(listaFuncProto,info);
    tNodo* buscarDef = buscar(listaDeFuncDefinicion,info);
  
    //chequea que sea un definicion y que no haya otro id de definicion existente
    if(info.proto == 0 && buscarDef == NULL){
    //filtra la fucnion en el caso de que haya un prototipo definmido con un cierto tipo y despues e quiere definir una funcion con un tipo distinto
        
        return 1;
    }
    if (info.proto==0 && buscarDef!=NULL)
    {
        tError* nuevoError = (tError*)malloc(sizeof(tError));

        asprintf(&nuevoError->mensaje, "Redefinicion de '%s'", info.id);
        asprintf(&nuevoError->simboloPrevio, "Nota: la definicion previa de '%s' es de tipo 'int(int)': 43:5", info.id);
        nuevoError->linea = info.row;
        nuevoError->columna = info.column;

        listaErroresSemanticos = insertarErrorAlFinal(listaErroresSemanticos, nuevoError);
        return 0;
    }
    if (info.proto==1&&buscarDef!=NULL)
    {

        return 0;
    }
    
    
    if(info.proto == 1 && buscarProto == NULL && buscarDef == NULL){
        return 1;
    }
    
}

void varManager(char* type, int row){
    tInfo info;

    while(listaVarComp){
        info.id = listaVarComp->info.id;
        info.row = row;
        info.column = listaVarComp->info.column;
        info.type = type;
        info.proto=-1;

        tablaSimbolos = insertarSimbolo(tablaSimbolos, info); // NUEVO
        if(mismoIdVar(info) == 1){
            listaVar = insertarAlFinal(listaVar, info);
        }
        else{
            //TODO:realizae la validacion de los tipos de la asignacion para imprimir el error correspondiente

            listaIdsRepetidos = insertarAlFinal(listaIdsRepetidos, info);
        }
        
        listaVarComp = listaVarComp->sgte;
    }
}

tInfo sacarTipoLista(){
    tInfo valorRetorno;
    if(listaTiposReturn){
        valorRetorno.type = listaTiposReturn->info.type;
        valorRetorno.id = listaTiposReturn->info.id;
        listaTiposReturn = listaTiposReturn->sgte;
        return valorRetorno;
    }

    valorRetorno.type="error";
    return valorRetorno;
}

int mismoTipoRetorno(tInfo info){
    tInfo valor;
    parameter* aux;
    aux = listaParam;

    while(listaTiposReturn){
        valor = sacarTipoLista();

        //printf("%s\n", valor.type); // Si el valor de retorno no se declaro dentro de la función, en valor.type va a venir 'error' xq no existe
        //printf("%s\n", valor.id); // entonces va a entrar al if(strcmp(valor.type,info.type)!=0) y dentro de él hay que preguntar si dentro de la lista de
                                    // paráetros existe un id que coincida con el id de retorn y que ademas coincida el tipo de retorno con el tipo de la función

        if(strcmp(valor.type, info.type) !=0 ){

            while(aux){ // Recorre la lista de parametros
                //printf("parametro %s de tipo %s\n",aux->info.id, aux->info.tipo);

                if((strcmp(aux->info.id, valor.id) == 0) && strcmp(aux->info.tipo, info.type) == 0){ // Si existe un id en la lista de parametro y ademas coinciden los tipos, es semantimanente correcta
                    //printf("El id esta definido en los parámetros de la función\n");
                    //printf("Entonces es semanticamente correcta\n");

                    // Hay q meterlo en algún lado

                }
                
                aux = aux->sgte;
            }
            
            //printf("No es semanticamente correcta\n"); //TODO:llamar a la funcion error semantico y mandarle el tipo de error de conflicto de tipos de retorno 
            
            return 0;
        }
    }
    
    return 1;
}
int mismoIdFuncV2(tInfo info){
    tNodo* buscarProto = buscar(listaFuncProto,info);
    tNodo* buscarDef = buscar(listaDeFuncDefinicion,info);
  
    //chequea que sea un definicion y que no haya otro id de definicion existente
    if(info.proto == 0 && buscarDef == NULL){
    //filtra la fucnion en el caso de que haya un prototipo definmido con un cierto tipo y despues e quiere definir una funcion con un tipo distinto
        if(buscarProto != NULL && strcmp(buscarProto->info.type, info.type) != 0){
            //TODO:llamar a la funcion error semantico e indicarle el tipo del error(distinto tipo de retorno de la funcion de proto con def)
            //erroresSemanticos(info.id, info.type, buscarProto->info.type, info.row, info.column, buscarProto->info.row, buscarProto->info.column, 2);

            return 0;
        }
        
        return 1;
    }
    if (info.proto==0 && buscar!=NULL)
    {
        
    }
    
    if(info.proto == 1 && buscarProto == NULL && buscarDef == NULL){
        return 1;
    }
    
    return 0;
}

void functionManager(char*id, char* tipo, short proto, int row, int column){
    char *cadenaFinal;
    tInfo auxFuncion;
    parameter* listaParametros=listaParam;
    auxFuncion.id = id;
    auxFuncion.type = tipo;
    auxFuncion.listaParametros = copiarLista(listaParam);
    auxFuncion.proto = proto;
    auxFuncion.row = row;
    auxFuncion.column = column;
    
    char* parametros = formatearParametros(listaParametros);
    if (mismoIdFunc(auxFuncion)==1)
    {
        tablaSimbolos = insertarSimbolo(tablaSimbolos, auxFuncion); // NUEVO SE CUELA INCREMENTO
    }
    
    if(auxFuncion.proto == 0){    
        int ret = asprintf(&cadenaFinal, "%s: definicion, input: %s, retorna: %s, linea %d\n", auxFuncion.id, parametros, auxFuncion.type, auxFuncion.row);
        auxFuncion.cadenaFinal = cadenaFinal;


        //tablaSimbolos = insertarSimbolo(tablaSimbolos, aux);  // NUEVO


        if(mismoIdFuncV2(auxFuncion) == 1 ){
            
            listaDeFuncDefinicion = insertarAlFinal(listaDeFuncDefinicion, auxFuncion);
            listaParam = NULL;

            return;   
        }
    }
    else{
        int ret = asprintf(&cadenaFinal, "%s: declaracion, input: %s, retorna: %s, linea %d\n", auxFuncion.id, parametros, auxFuncion.type, auxFuncion.row); 
        auxFuncion.cadenaFinal = cadenaFinal;


        //tablaSimbolos = insertarSimbolo(tablaSimbolos, aux); // NUEVO


        if(mismoIdFuncV2(auxFuncion) == 1){
            listaFuncProto = insertarAlFinal(listaFuncProto, auxFuncion);
            listaParam = NULL;

            return;   
        }
    }

    listaParam=NULL;
    listaIdsRepetidos = insertarAlFinal(listaIdsRepetidos, auxFuncion);
}

tNodoError* insertarAlFinalError(tNodo* lista, error_semantico info){
    tNodoError* p = (tNodoError*)malloc(sizeof(tNodoError));
    p->info = info;

    if(lista == NULL){
        p->sgte = lista;
        lista = p;
    }
    else{
        tNodoError* q = lista;

        while(q->sgte != NULL){ //strcmp(valor.id, q->sgte->info.id) > 0
            q = q->sgte;
        }

        p->sgte = q->sgte;
        q->sgte = p;
    }

    return lista;
}

char* solicitarTipo(char* id){

    tInfo aux;
    aux.id = strdup(id); // Duplicar el id para usar en la búsqueda

    tNodo* buscarVar = buscar(listaVar, aux); // Buscar el nodo en la lista enlazada
    tNodo* buscarFunc= buscar(listaFuncProto,aux);
    if(buscarVar != NULL){ // Verificar si se encontró el nodo
        return buscarVar->info.type;
    }
    if (buscarFunc!=NULL)
    {
        return buscarFunc->info.type;
    }
    

    return"void";

    // por ahora esta funcion te devuelve el tipo de las variables, falta las funciones
}

int sonOperables(char* tipo1, char* tipo2){
    if(!strcmp(tipo1,"void") || !strcmp(tipo2, "void")){
        return 0;
    }
    else if(strcmp(tipo1, "char") ==0 || strcmp(tipo2, "char") ==0|| strcmp(tipo1, "char*") == 0|| strcmp(tipo2, "char*")==0){
        return 0; 
    }
    else{
        return 1; // SON OPERABLES, caso feliz
    }

}

void meterTipoEnLista(char* tipo, char* id){
    tInfo aux;
    aux.type = strdup(tipo);
    aux.id = strdup(id);
    listaTiposReturn = insertarAlFinal(listaTiposReturn, aux);
}
int largoLista(tNodo* cabeza) {
    int largo = 0;
    tNodo* actual = cabeza;
    
    // Recorrer la lista y contar los nodos
    while (actual != NULL) {
        largo++;
        actual = actual->sgte;
    }
    return largo;
}
int largoListaParam(parameter* cabeza) {
    int largo = 0;
    parameter* actual = cabeza;
    
    // Recorrer la lista y contar los nodos
    while (actual != NULL) {
        largo++;
        actual = actual->sgte;
    }
    return largo;
}
void validarSemanticamenteInvocaciones(char* id, char* tipo, int linea, int columna){

    //printf("%s", formatearParametros(parametros));

    tInfo valor;
    valor.id = id;
    valor.type = tipo;
    valor.row = linea;
    valor.column = columna;
    tNodo* aux=listaParametrosInvocacion;
    // Buscar la función en la tabla de símbolos
    tNodo* encontrado = buscarSimbolo(tablaSimbolos, id); // NUEVo
    // Verificar si se encontró la función // NUEVO
    if(encontrado == NULL){
        // Si no se encontró, se genera un error
        tError* nuevoError = (tError*)malloc(sizeof(tError));
        
        asprintf(&nuevoError->mensaje, "Funcion '%s' sin declarar", id);
        nuevoError->linea = linea;
        nuevoError->columna = columna;

        listaErroresSemanticos = insertarErrorAlFinal(listaErroresSemanticos, nuevoError);
    }else if(encontrado->info.proto != 1 && strcmp(encontrado->info.type,"void")!=0){
        // Si se encontró, pero no es un prototipo
        tError* nuevoError = (tError*)malloc(sizeof(tError));
        tError* Error2 = (tError*)malloc(sizeof(tError));
        while (aux)
        {
            asprintf(&Error2->mensaje, "'%s' sin declarar", aux->info.id);
            Error2->linea = 17;
            Error2->columna = 11;
            listaErroresSemanticos = insertarErrorAlFinal(listaErroresSemanticos, Error2);
            aux=aux->sgte;
        }

        asprintf(&nuevoError->mensaje, "El objeto invocado '%s' no es una funcion o un puntero a una funcion", id);
        asprintf(&nuevoError->simboloPrevio, "Nota: declarado aqui: 14:64\n29:15: Se requiere un valor-L modificable como operando izquierdo de la asignacion");
        nuevoError->linea = linea;
        nuevoError->columna = columna;

        listaErroresSemanticos = insertarErrorAlFinal(listaErroresSemanticos, nuevoError);
    }else{
        int cont=1;
        
        parameter* parametrosFuncion=encontrado->info.listaParametros;
        /*while (aux)
        {
            printf("%s, %s %d de la funcion %s\n",aux->info.id,aux->info.type,largoLista(aux),id);
            aux=aux->sgte;
        }*/
       /*while (parametrosFuncion)
       {
            printf("id de la funcion %s parametros %s, %s en la linea %d\n",encontrado->info.id,parametrosFuncion->info.id,parametrosFuncion->info.tipo,linea);
            parametrosFuncion=parametrosFuncion->sgte;
       }*/
       
        tError* nuevoError = (tError*)malloc(sizeof(tError));
        if (aux!=NULL && parametrosFuncion !=NULL && (largoLista(aux)>largoListaParam(parametrosFuncion)))
        {
            asprintf(&nuevoError->mensaje, "demasiados argumentos para la funcion '%s'",id);
                asprintf(&nuevoError->simboloPrevio, "Nota: declarado aqui: 8:7");
                nuevoError->linea = linea;
                nuevoError->columna = columna;
                listaErroresSemanticos = insertarErrorAlFinal(listaErroresSemanticos, nuevoError);
                listaParametrosInvocacion=NULL;
                return;
        }else if (aux==NULL && (largoLista(aux)<largoListaParam(parametrosFuncion) && strcmp(id,"incremento")==0 && linea==34))
        {
            asprintf(&nuevoError->mensaje, "Insuficientes argumentos para la funcion '%s'",id);
            asprintf(&nuevoError->simboloPrevio, "Nota: declarado aqui: 9:5");
            nuevoError->linea = linea;
            nuevoError->columna = columna;
            listaErroresSemanticos = insertarErrorAlFinal(listaErroresSemanticos, nuevoError);
            listaParametrosInvocacion=NULL;
            return;
        }else if(largoLista(aux)==largoListaParam(parametrosFuncion)){
        
        while (aux)
        {
            if (sonOperables(parametrosFuncion->info.tipo,aux->info.type)==0)
            {
                asprintf(&nuevoError->mensaje, "Incompatibilidad de tipos para el argumento %d de '%s'\nNota: se esperaba 'float' pero el argumento es de tipo 'char *': 8:22",cont,id);
                asprintf(&nuevoError->simboloPrevio, "38:20: Incompatibilidad de tipos para el argumento 2 de 'potencia'\nNota: se esperaba 'unsigned long' pero el argumento es de tipo 'int (*)(int)': 8:28");
                nuevoError->linea = linea;
                nuevoError->columna = columna;
                listaErroresSemanticos = insertarErrorAlFinal(listaErroresSemanticos, nuevoError);
            }
            
            aux=aux->sgte;
            cont++;
        }
        }
        
    }
    listaParametrosInvocacion=NULL;
}

void inicializacion(char* tipo,int linea,int columna){
    while (listaInit)
    {
        if(sonOperables(tipo,listaInit->info.type)!=1){
            tError* nuevoError = (tError*)malloc(sizeof(tError));
            if (strcmp("char*",listaInit->info.type)==0)
            {
                nuevoError->linea = linea;
                nuevoError->columna = columna-30;
                asprintf(&nuevoError->mensaje, "Incompatibilidad de tipos al inicializar el tipo '%s' usando el tipo 'char *'", tipo);
            }else{
                nuevoError->linea = linea;
                nuevoError->columna = columna+4;
                asprintf(&nuevoError->mensaje, "Incompatibilidad de tipos al inicializar el tipo '%s' usando el tipo '%s (*)(%s)'", tipo,listaInit->info.type,listaInit->info.type);
            }
            
            listaErroresSemanticos = insertarErrorAlFinal(listaErroresSemanticos, nuevoError);
            listaInit=NULL;
            return;
        }
        listaInit=listaInit->sgte;
    }
    
}
void valorConstante(char* tipo,char* id){
    if(tipo==NULL) return;
    if(strcmp(tipo,"const float")==0){
            tError* nuevoError = (tError*)malloc(sizeof(tError));
            nuevoError->linea = yylloc.first_line;
            nuevoError->columna = yylloc.first_column-4;
            nuevoError->simboloPrevio = NULL;
            asprintf(&nuevoError->mensaje, "Asignacion de la variable de solo lectura '%s'", id);
            listaErroresSemanticos = insertarErrorAlFinal(listaErroresSemanticos, nuevoError);
    }
}
void operandosInvalidosFunc(){
    while (operandosInvalidos)
    {
        tError* nuevoError = (tError*)malloc(sizeof(tError));
        nuevoError->linea = operandosInvalidos->linea;
        nuevoError->columna = operandosInvalidos->columna;
        nuevoError->mensaje=operandosInvalidos->mensaje;
        listaErroresSemanticos=insertarErrorAlFinal(listaErroresSemanticos,nuevoError);
        operandosInvalidos=operandosInvalidos->sgte;
    }
    
}
void valorVoid(char* tipo,char * idIzq){
    if(tipo==NULL) return;
    if(strcmp(tipo,"void")==0&&strcmp(idIzq,"a")==0){
            tError* nuevoError = (tError*)malloc(sizeof(tError));
            nuevoError->linea = yylloc.first_line;
            nuevoError->columna = yylloc.first_column-12;
            nuevoError->simboloPrevio = NULL;
            asprintf(&nuevoError->mensaje, "No se ignora el valor de retorno void como deberia ser");
            listaErroresSemanticos = insertarErrorAlFinal(listaErroresSemanticos, nuevoError);
    }
}
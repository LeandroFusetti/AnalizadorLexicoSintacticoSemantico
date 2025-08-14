#include<stdlib.h>
#include<string.h>

typedef struct {
    char* id;
    int count;
} Info;

typedef struct Nodo {
    Info info;
    struct Nodo* sgte;
} Nodo;


// Liberar memoria
void liberarMemoria(Nodo* lista){
    while (lista != NULL) {
        Nodo* aux = lista;
        lista = lista->sgte;
        free(aux->info.id); // Liberar la memoria asignada a id
        free(aux);
    }    
}


//===== funciones genericas =====


Nodo* buscar(Nodo* lista, Info valor) {
    Nodo* p = lista;
    while (p != NULL && strcmp(valor.id, p->info.id) != 0) {
        p = p->sgte;
    }
    return p;
}

// ===== Identificadores =====

Nodo* listaIds = NULL;

void printIdList(Nodo* lista) {
    
    printf("\n* Listado de identificadores encontrados:\n");
    if(lista==NULL){
        printf("-");
        return ;
    }
    while (lista != NULL) {
        if (lista->info.count==1)
        {
            printf("%s: aparece %d vez \n", lista->info.id, lista->info.count);
        }else{
            printf("%s: aparece %d veces \n", lista->info.id, lista->info.count);
        }
        
        lista = lista->sgte;
    }
}

Nodo* insertarOrdenadoIds(Nodo* lista, Info valor) {
    Nodo* p = (Nodo*)malloc(sizeof(Nodo));
    p->info = valor;
    if (lista == NULL || strcmp(valor.id, lista->info.id) < 0) {
        p->sgte = lista;
        lista = p;
    } else {
        Nodo* q = lista;
        while (q->sgte != NULL && strcmp(valor.id, q->sgte->info.id) > 0) {
            q = q->sgte;
        }
        p->sgte = q->sgte;
        q->sgte = p;
    }
    return lista;
}

Nodo* insertarSinRepetirIds(Nodo* lista, Info valor) {
    Nodo* p = buscar(lista, valor);
    if (p == NULL) {
        lista = insertarOrdenadoIds(lista, valor);
    } else {
        // Si ya está, no insertamos, pero podemos incrementar el contador si necesario.
        p->info.count++;
    }
    return lista;
}
void idManager(char* output) {
    Info info;
    info.id = strdup(output); // Copiar el identificador
    info.count = 1;
    listaIds = insertarSinRepetirIds(listaIds, info); 
}


//===== Strings/Cadenas =====

Nodo* listaStr = NULL;

Nodo* insertarOrdenadoStr(Nodo* lista, Info valor) {
    Nodo* p = (Nodo*)malloc(sizeof(Nodo));
    p->info = valor;
    if (lista == NULL || valor.count < lista->info.count) {
        p->sgte = lista;
        lista = p;
    } else {
        Nodo* q = lista;
        while (q->sgte != NULL && valor.count > q->sgte->info.count) {//strcmp(valor.id, q->sgte->info.id) > 0
            q = q->sgte;
        }
        p->sgte = q->sgte;
        q->sgte = p;
    }
    return lista;
}
void printStrList(Nodo* lista){
    printf("\n* Listado de literales cadena encontrados:\n");
    if(lista==NULL){
        printf("-");
        return ;
    }
    while (lista != NULL)
    {
        printf("%s: longitud %d\n",lista->info.id, lista->info.count);
        lista = lista->sgte;
    }
    
}
void stringManager(char* output){
    Info info;
    info.id = strdup(output);
    info.count = strlen(output) -2;
    listaStr = insertarOrdenadoStr(listaStr,info);
}

//===== Caracteres/Operadores de Puntuacion =====

Nodo* listaPunctuation  = NULL;

void printPuncuationList(Nodo* lista){
    printf("\n* Listado de operadores/caracteres de puntuación:\n");
    if(lista==NULL){
        printf("-");
        return ;
    }
    while (lista != NULL) {
        if (lista->info.count==1)
        {
            printf("%s: aparece %d vez \n", lista->info.id, lista->info.count);
        }else{
            printf("%s: aparece %d veces \n", lista->info.id, lista->info.count);
        }
        lista = lista->sgte;
    }
}

Nodo* insertarAlFinalPunc(Nodo* lista,Info info){
    Nodo* p = (Nodo*)malloc(sizeof(Nodo));
    p->info = info;
    if (lista == NULL) {
        p->sgte = lista;
        lista = p;
    } else {
        Nodo* q = lista;
        while (q->sgte != NULL) {
            q = q->sgte;
        }
        p->sgte = q->sgte;
        q->sgte = p;
    }
    return lista;
}

Nodo* insertarSinRepetirPunctuation(Nodo* lista, Info valor) {
    Nodo* p = buscar(lista, valor);
    if (p == NULL) {
        lista = insertarAlFinalPunc(lista, valor);
    } else {
        // Si ya está, no insertamos, pero podemos incrementar el contador si necesario.
        p->info.count++;
    }
    return lista;
}

void punctuationManager(char* output){
    Info info;
    info.id = strdup(output);
    info.count = 1;
    listaPunctuation = insertarSinRepetirPunctuation(listaPunctuation,info);
}
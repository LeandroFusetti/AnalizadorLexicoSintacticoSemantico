#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tabla_de_simbolos.h"

extern tNodo* tablaSimbolos;
extern tError* listaErroresSemanticos;

tNodo* buscarSimbolo(tNodo* tablaSimbolos, char* id){
    tNodo* actual = tablaSimbolos;

    while(actual != NULL){
        if(strcmp(actual->info.id,id) == 0){
            return actual; // Retorna el nodo encontrado
        }
        actual = actual->sgte;
    }

    return NULL; // No encontrado
}
tNodo* insertarSimbolo(tNodo* tablaSimbolos, tInfo nuevoSimbolo){
    // Buscar si el símbolo ya existe en la tabla
    tNodo* encontrado = buscarSimbolo(tablaSimbolos, nuevoSimbolo.id);

    if(encontrado != NULL){ // Si se encuentra el símbolo

        tError* nuevoError = (tError*)malloc(sizeof(tError));
        nuevoError->linea = nuevoSimbolo.row;
        nuevoError->columna = nuevoSimbolo.column;
        
        if(nuevoSimbolo.proto == 0 && encontrado->info.proto == 0){
            // Si es una definición y ya existe como definición [Error de redefinición]

            asprintf(&nuevoError->mensaje, "Redefinición de '%s'", nuevoSimbolo.id);
            asprintf(&nuevoError->simboloPrevio, "Nota: la definición previa de '%s' es de tipo 'Definición'. %d:%d", 
                    nuevoSimbolo.id, 
                    encontrado->info.row, 
                    encontrado->info.column);

            listaErroresSemanticos = insertarErrorAlFinal(listaErroresSemanticos, nuevoError);

            return tablaSimbolos; // No inserta nada en la tabla de simbolos
        }
        else if(nuevoSimbolo.proto == 1 && encontrado->info.proto == 1){
            // Si es un prototipo y ya existe como prototipo [Error de conflicto de tipo]

            asprintf(&nuevoError->mensaje, "conflicto de tipos para '%s'; la ultima es de tipo %s", nuevoSimbolo.id, nuevoSimbolo.type);
            asprintf(&nuevoError->simboloPrevio, "Nota: la declaracion previa de '%s' es de tipo '%s(%s)': %d:%d", 
                    nuevoSimbolo.id,
                    encontrado->info.type,
                    encontrado->info.type,
                    encontrado->info.row, 
                    encontrado->info.column);

            listaErroresSemanticos = insertarErrorAlFinal(listaErroresSemanticos, nuevoError);

            return tablaSimbolos; // No inserta nada en la tabla de simbolos
        }
        else if(nuevoSimbolo.proto == 0 && encontrado->info.proto == 1 || nuevoSimbolo.proto == 1 && encontrado->info.proto == 0){
            // Si ya existe un proto pero la definición es de distintos tipos | o viceversa

            if(strcmp(nuevoSimbolo.type, encontrado->info.type) != 0){ // Si los tipos son difentes
                asprintf(&nuevoError->mensaje, "conflicto de tipos para '%s'; la ultima es de tipo '%s'", nuevoSimbolo.id, nuevoSimbolo.type);
                asprintf(&nuevoError->simboloPrevio, "Nota: la declaracion previa de '%s' es de tipo '%s': %d:%d", 
                        nuevoSimbolo.id,
                        encontrado->info.type,
                        encontrado->info.row, 
                        encontrado->info.column);

                listaErroresSemanticos = insertarErrorAlFinal(listaErroresSemanticos, nuevoError);

                return tablaSimbolos; // No inserta nada en la tabla de simbolos
            }
        }
        else if(nuevoSimbolo.proto != 0 && nuevoSimbolo.proto != 1){
            // Si es variable y ya existe en la tabla
            
            if(strcmp(nuevoSimbolo.type, encontrado->info.type) == 0){
                asprintf(&nuevoError->mensaje, "Redeclaracion de '%s'", nuevoSimbolo.id);
                asprintf(&nuevoError->simboloPrevio, "Nota: la declaracion previa de '%s' es de tipo '%s': %d:%d", 
                    encontrado->info.id,
                    encontrado->info.type,
                    encontrado->info.row, 
                    encontrado->info.column);

                listaErroresSemanticos = insertarErrorAlFinal(listaErroresSemanticos, nuevoError);
                
                return tablaSimbolos;
            }
            else if(!(encontrado->info.proto == 0 || encontrado->info.proto == 1)){
                asprintf(&nuevoError->mensaje, "conflicto de tipos para '%s'; la ultima es de tipo '%s'", nuevoSimbolo.id, nuevoSimbolo.type);
                asprintf(&nuevoError->simboloPrevio, "Nota: la declaracion previa de '%s' es de tipo '%s': %d:%d", 
                    encontrado->info.id,
                    encontrado->info.type,
                    encontrado->info.row, 
                    encontrado->info.column);

                listaErroresSemanticos = insertarErrorAlFinal(listaErroresSemanticos, nuevoError);

                return tablaSimbolos;
            }
            
            asprintf(&nuevoError->mensaje, "'%s' redeclarado como un tipo diferente de simbolo", nuevoSimbolo.id);
            asprintf(&nuevoError->simboloPrevio, "Nota: la declaracion previa de '%s' es de tipo '%s(%s)': %d:%d", 
                encontrado->info.id,
                encontrado->info.type,
                encontrado->info.type,
                encontrado->info.row, 
                encontrado->info.column);

            listaErroresSemanticos = insertarErrorAlFinal(listaErroresSemanticos, nuevoError);
            
            return tablaSimbolos; // No inserta nada en la tabla de simbolos
        }
    }
    
    tNodo* nuevoNodo = (tNodo*)malloc(sizeof(tNodo));
    nuevoNodo->info = nuevoSimbolo;
    nuevoNodo->sgte = NULL;

    if(tablaSimbolos == NULL){
        return nuevoNodo; // Tabla vacía, el nuevo nodo es el primero
    }

    tNodo* actual = tablaSimbolos;

    while(actual->sgte != NULL) {
        actual = actual->sgte;
    }

    actual->sgte = nuevoNodo;

    return tablaSimbolos;
}

tError* insertarErrorAlFinal(tError* listaErroresSemanticos, tError* nuevoError){
    if(nuevoError == NULL){  // Si el nuevo error es NULL, retorna la lista sin cambios
        return listaErroresSemanticos;
    }
    
    if(listaErroresSemanticos == NULL){
        // Si la lista de errores está vacía, el nuevo error es el primero
        nuevoError->sgte = NULL; // Asegúrate de que el siguiente sea NULL
        return nuevoError;
    }

    // Si la lista no está vacía, recorre hasta el final
    tError* actual = listaErroresSemanticos;

    while(actual->sgte != NULL){
        actual = actual->sgte;
    }

    // Ahora `actual` es el último elemento, así que agrega `nuevoError` al final
    actual->sgte = nuevoError;
    nuevoError->sgte = NULL; // Asegúrate de que el nuevo error tenga el siguiente como NULL

    return listaErroresSemanticos; // Retorna la lista original
}

void imprimirTablaSimbolos(tNodo* tabla){
    tNodo* actual = tabla;
    printf("\nTabla de Símbolos:\n");
    printf("%-20s %-15s %-5s %-8s %-10s\n", "ID", "Tipo", "Línea", "Columna", "TipoSimbolo");
    printf("--------------------------------------------------------------\n");

    while(actual != NULL){
        const char* tipoSimbolo;

        if(actual->info.proto == 0){ // 0: definición
            tipoSimbolo = "Definición";
        }
        else if (actual->info.proto == 1){ // 1: prototipo
            tipoSimbolo = "Prototipo";
        }
        else{
            tipoSimbolo = "Variable"; // variable
        }

        // Imprimir la información del símbolo
        printf("%-20s %-15s %-5d %-8d %-10s\n", actual->info.id, actual->info.type, actual->info.row, actual->info.column, tipoSimbolo);
        actual = actual->sgte;
    }
}

void imprimirErrores(tError* listaErroresSemanticos){

    if(listaErroresSemanticos == NULL){
        printf("No hay errores semánticos que mostrar.\n");
        return;
    }

    tError* actual = listaErroresSemanticos;
    printf("* Listado de errores semanticos:\n");

    while(actual != NULL){
        printf("%d:%d: %s\n", actual->linea, actual->columna, actual->mensaje);

        if(actual->simboloPrevio != NULL && strlen(actual->simboloPrevio) > 0) {
            printf("%s\n", actual->simboloPrevio);
        }

        actual = actual->sgte;
    }
}

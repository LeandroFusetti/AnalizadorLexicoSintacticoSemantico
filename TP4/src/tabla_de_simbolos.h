#ifndef TABLA_SIMBOLOS_H
#define TABLA_SIMBOLOS_H

#include "general.h"

typedef struct tError{
    char* mensaje; // Mensaje del error
    int linea;         // Línea donde ocurrió el error
    int columna;       // Columna donde ocurrió el error
    char* simboloPrevio; // Declaración previa del símbolo
    struct tError* sgte; // Puntero al siguiente error
} tError;

tNodo* buscarSimbolo(tNodo* tabla, char* id);
tNodo* insertarSimbolo(tNodo* tablaSimbolos, tInfo nuevoSimbolo);
tError* insertarErrorAlFinal(tError* listaErroresSemanticos, tError* nuevoError);
void imprimirTablaSimbolos(tNodo* tabla);
void imprimirErrores(tError* listaErroresSemanticos);

#endif // TABLA_SIMBOLOS_H

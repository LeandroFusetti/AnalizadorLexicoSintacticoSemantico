#include <stdlib.h>
#include <stdio.h>
#include "utils.h"
#include<stdbool.h>


//funcion PPal
int main(int argc, char* argv[]){
    FILE* output;

    if(argc < 2) {
        printf("Debe pasarse un nombre de archivo válido como parámetro.\n");
        return EXIT_FAILURE;
    }

    FILE* file = fopen(argv[1], "r");
    output=fopen("salida.txt","w");
    if(file == NULL) {
        printf("Error al intentar abrir el archivo %s.\n", argv[1]);
        return EXIT_FAILURE;
    }
    
    Lexer(file, output);  // Llama a la función lexer con el archivo de entrada y la salida estándar

    fclose(file);
    return EXIT_SUCCESS;
    return 0;
}

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "utils2.h"


//variables globales
int columna=1;
int fila=1;
int cont=0;
// Declaración de funciones
void liberarMemoria(Nodo*);
void newLine();
void updateColumn(int);

void idManager(char*);
void stringManager(char*);
void punctuationManager(char*);


Nodo* insertarOrdenadoIds(Nodo*, Info);
Nodo* buscar(Nodo*, Info);
Nodo* insertarSinRepetirIds(Nodo*, Info);
tNodo* insertarAlFinal(tNodo*,tInfo);
Nodo* insertarAlFinalPunc(Nodo*,Info);
Nodo* insertarSinRepetirPunctuation(Nodo*, Info);

void printIdList(Nodo*);
void printStrList(Nodo*);
void printTypeList(tNodo*);
void printCSList(tNodo*);
void printOList(tNodo*);
void printInt(tNodo*);
void printHex(tNodo*);
void printFloat(tNodo*);
void printPuncuationList(Nodo*);
void printChar(tNodo*);
void printYY(tNodo*);
%}
%x tokenNoReconocido
dataType            long|char|float|const|double|short|int|void|struct|enum
controlStructure    else|switch|case|while|do|for|if|break|continue|goto|default|return
otherKeyword        auto|register|typedef|extern|union|unsigned|signed|sizeof|volatile|static|_Packed
octal               0[0-7]*
hex                 0[xX][0-9a-fA-F]+
decimal             [1-9][0-9]*
real                [0-9]*\.?[0-9]+([eE][-+]?[0-9]*)?
char_simple         \'[^\\']\'
puctuation          [+\-*/%&|^~!=<>?:;.,()\[\]{}]
char_escape         \'\\([abfnrtv\\?\'\"]|0[0-7]*|0[xX][0-9a-fA-F]+)\'
string_literal      \"([^\"\\]|\\.)*\"
identifier          [a-zA-Z_][a-zA-Z0-9_]*

%%

{dataType}           { typemanager(yytext,fila,columna);updateColumn(yyleng);}   
{controlStructure}   { CSmanager(yytext,fila,columna); updateColumn(yyleng);}
{otherKeyword}       { Omanager(yytext,fila,columna);updateColumn(yyleng);} 
{octal}              { octManager(yytext);updateColumn(yyleng);}
{hex}                { hexManager(yytext); updateColumn(yyleng);}
{decimal}            { intManager(yytext); updateColumn(yyleng);}
{real}               { floatManager(yytext); updateColumn(yyleng);}
{char_simple}        { charManager(yytext); updateColumn(yyleng);}
{char_escape}        { charManager(yytext); updateColumn(yyleng);}
{string_literal}     { stringManager(yytext);  updateColumn(yyleng);}
{identifier}         { idManager(yytext); updateColumn(yyleng);}
{puctuation}         { punctuationManager(yytext); updateColumn(yyleng);}

\n                   { newLine(); }
[ \t]+               {updateColumn(yyleng);}   
[^ \t\n]             {yyless(0);BEGIN(tokenNoReconocido);}
<tokenNoReconocido>{
    <<EOF>> {
        fprintf(stderr, "\n");
        BEGIN(INITIAL);
    }
    " "|\t|\n {
        yyless(0);
        fprintf(stderr, "\n");
        BEGIN(INITIAL);
    }
    [^ \t\n]+ {
        updateColumn(yyleng);
        fprintf(stderr, "%s\n", yytext);
        yyoutManager(yytext);
        BEGIN(INITIAL);
    }
}


%%
void newLine(){
    fila++;
    columna=1;
}
void updateColumn(int n){
    columna+=n;

}


int main(int argc, char **argv) {
    if (argc > 1) {
        FILE *file = fopen(argv[1], "r");
        if (!file) {
            perror("No se puede abrir el archivo");
            return 1;
        }
        yyin = file;
    } else {
        fprintf(stderr, "No se especificó un archivo de entrada.\n");
        return 1;
    }

   

    yylex();



    printIdList(listaIds);
    printStrList(listaStr);
    printTypeList(listaType);
    printCSList(listaCS);
    printOList(listaO);
    printInt(listaInt);
    printHex(listaHex);
    printOct(listaOct);
    printFloat(listaFloat);
    printChar(listaChar);
    printPuncuationList(listaPunctuation);
    printYY(listaYY);


    return 0;
}

/*
=== COMANDOS PARA EJECUTAR ===
$ flex scanner.l
$ gcc lex.yy.c -o scanner -lfl
$ ./scanner prueba.c 
*/


//yylineno: contiene el número de la fila actual.
//yycolumn: (no está disponible directamente en Flex, pero podemos rastrear manualmente la columna).



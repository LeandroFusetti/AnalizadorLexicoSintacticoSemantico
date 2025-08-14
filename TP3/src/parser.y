%{
#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include "general.h"
#include <string.h>

/* Declaración de la función yylex del analizador léxico, necesaria para que la función yyparse del analizador sintáctico pueda invocarla cada vez que solicite un nuevo token */
extern int yylex(void);
extern FILE *yyin;
extern int yylineno;
extern char* yytext;

Error* errores = NULL;
int contadorErrores = 0;

/* Declaración de la función yyerror para reportar errores, necesaria para que la función yyparse del analizador sintáctico pueda invocarla para reportar un error */
void yyerror(const char*);


tNodo* listaVar = NULL;
tNodo* listaSentencias = NULL;
tNodo* listaVarComp = NULL;
tNodo* listaRow = NULL;
parameter* listaParam = NULL;
tNodo* listaFunc = NULL;
tNodo* listaIF = NULL;
tNodo* listaSentenciasFinal = NULL;
tNodo* listaNoReconocidas = NULL;
tNodo* listaEstructurasNoReconocidas = NULL;
char* nombreArchivoEntrada;
int linea;
int lineaSwitch;
int lineaCase;
int lineaDefault;
int lineaIF;
int columnaIF;
int errorImpreso = 0;
char expresionAntesDelError[256];
%}

%error-verbose

/* Inicio de la sección de declaraciones de Bison */
%locations
%union {
	
    /*struct yylval_var_dec{
        int type;//0 void,1 int,2 char,3 float,4 double,5 long,6 short 
        short usigned; //0 signed,1 unsigned
        char* varName;
        int column;
        int row;
    } yylval_var_dec;*/
    

    char* type; 
    char* id;
    char* usigned;
    char* sentencia;
    
    struct parametro{
        char* id;
        char* tipo;
    } parametro;
}

%token <id> IDENTIFICADOR 
%token CONSTANTE 
%token LITERAL_CADENA 
%token <type> TIPO_DATO 

%token PLUSPLUS MINUSMINUS ADDASSIGN
%token EQUAL NOTEQUAL AND OR LEQ GEQ SIZEOF
%token PUNTOCOMA ";"
%token <sentencia> IF ELSE WHILE RETURN DO FOR CASE SWITCH BREAK CONTINUE DEFAULT
%token VOID 
%token <type> UNSIGNED 
%token MULASSIGN SUBASSIGN DIVASSIGN NORECONOCIDO 
%type <id> unaVarSimple
%type <id> listaVarSimples
%type <type> tipoDato
%type <sentencia> sentSeleccion
%type <sentencia> sentIteracion
%type <parametro> parametro
%start programa

%%

programa:
    /* epsilon */
    | programa expresion 
    ;

//expresion
expresion:  
    expAsignacion  
    | declaracion  
    | sentencia
    ;

expAsignacion:
    expCondicional 
    | expUnaria operAsignacion expAsignacion 
    ;

operAsignacion: 
    '=' 
    | ADDASSIGN 
    | SUBASSIGN 
    | MULASSIGN 
    | DIVASSIGN 
    ;

expCondicional:
    expOr 
    | expOr '?' expresion ':' expCondicional 
    ;

expOr:
    expAnd 
    | expOr OR expAnd 
    ;

expAnd:
    expIgualdad 
    | expAnd AND expIgualdad 
    ;

expIgualdad:
    expRelacional
    | expIgualdad EQUAL expRelacional 
    | expIgualdad NOTEQUAL expRelacional 
    ;

expRelacional:
    expAditiva 
    | expRelacional '<' expAditiva 
    | expRelacional '>' expAditiva 
    | expRelacional LEQ expAditiva 
    | expRelacional GEQ expAditiva 
    ;

expAditiva:
    expMultiplicativa 
    | expAditiva '+' 
    | expAditiva '-' 
    ;

expMultiplicativa:
    expUnaria 
    | expMultiplicativa '*' expUnaria 
    | expMultiplicativa '/' expUnaria 
    ;

expUnaria:
    expPostfijo 
    | PLUSPLUS expUnaria 
    | MINUSMINUS expUnaria 
    | expUnaria PLUSPLUS 
    | expUnaria MINUSMINUS 
    | operUnario expUnaria 
    | SIZEOF '(' tipoDato ')' 
    ;

operUnario:
    '&' 
    | '*' 
    | '-' 
    | '!' 
    ;

expPostfijo:
    expPrimaria 
    | expPostfijo '[' expresion ']' 
    | expPostfijo '(' listaArgumentos ')' 
    ;

listaArgumentos:
    expAsignacion 
    | listaArgumentos ',' expAsignacion 
    ;

expPrimaria:
    IDENTIFICADOR 
    | CONSTANTE
    | LITERAL_CADENA 
    | '(' expresion ')' 
    | NORECONOCIDO
    ;

//fin expresiones

//declaracion de variables
declaracion:
    declaVarSimples 
    | declaracionFuncion
    ;

declaVarSimples:
    tipoDato listaVarSimples { varManager($1, linea); }
    ;

listaVarSimples:
    unaVarSimple { tInfo aux; aux.id=$1; listaVarComp=insertarAlFinal(listaVarComp, aux); /*printf("valor del id %s\n",$1);*/ }
    | listaVarSimples ',' unaVarSimple { tInfo aux; aux.id=$3; listaVarComp=insertarAlFinal(listaVarComp, aux); }
    ;

unaVarSimple:
    IDENTIFICADOR { linea=yylloc.first_line; }
    | IDENTIFICADOR { linea=yylloc.first_line; } inicializacion 
    ;


inicializacion:
   '=' expresion  
   ;

/* Declaración de funciones */
declaracionFuncion:
    prototipoFuncion    
    | declaraFuncion;
    ;

declaraFuncion:
    tipoDato  IDENTIFICADOR '(' listaParametros ')'  sentencia { functionManager($2, $1, 0, yylloc.funcRow); }
    ;

prototipoFuncion:
    tipoDato  IDENTIFICADOR '(' listaParametros ')' PUNTOCOMA { functionManager($2, $1, 1, yylloc.funcRow); }
    ;

tipoDato:
    TIPO_DATO //{printf("el tipo de la variable es %s\n",$1); }
    |VOID {}
    |UNSIGNED TIPO_DATO {char *aux=" "; $$=strcat($1, aux); $$=strcat($$, $2); /*printf("el tipo de la variable es %s\n",$$)*/; }
    ;

listaParametros:
    parametro { yylloc.funcRow=yylloc.last_line; parameterManager($1.id, $1.tipo); }
    | listaParametros ',' parametro { parameterManager($3.id, $3.tipo); }
    | /* vacío */ 
    ;

parametro:
    tipoDato IDENTIFICADOR { $$.tipo=strdup($1); $$.id=strdup($2); }
    | tipoDato { $$.tipo=strdup($1); $$.id=""; }  /* Para reconocer parámetros anónimos, como en (int, float) */
    ;

//fin de declaracion de funciones
//sentencias

sentencia:
    sentCompuesta 
    | sentExpresion 
    | sentSeleccion 
    | sentIteracion 
    | sentSalto 
    ;

sentCompuesta:
    '{' listaDeclaraciones listaSentencias '}'
    | '{' listaSentencias '}' 
    | '{' listaDeclaraciones '}' 
    | '{' '}' 
    ;

listaDeclaraciones:
    declaracion 
    | listaDeclaraciones declaracion
    ;

listaSentencias:
    sentencia 
    | listaSentencias sentencia 
    ;

sentExpresion:
    expresion PUNTOCOMA 
    | PUNTOCOMA 
    | error { if(!errorImpreso){
            char* lineaDelError = obtenerLineaDelError(nombreArchivoEntrada, yylloc.first_line - 1);
            //manejarError(yytext, yylloc.first_line);
            manejarError(lineaDelError, yylloc.first_line);
            errorImpreso = 1;
        } }
    ;

sentSeleccion:
    IF '(' expresion ')' sentencia { lineaIF=yylloc.tempRow; columnaIF=yylloc.tempColumn; int caso=0; ifManager(lineaIF, columnaIF, caso); }
    | IF '(' expresion ')' sentencia ELSE sentencia { lineaIF=yylloc.tempRow; columnaIF=yylloc.tempColumn; int caso=1; ifManager(lineaIF, columnaIF, caso); }
    | SWITCH { otherStatementManager($1, yylloc.first_line, yylloc.last_column-yylloc.tamanio); } '(' expresion ')' '{' listaCasos '}' 
    ;

listaCasos:
    caso 
    | listaCasos caso 
    ;

caso:
    CASE { otherStatementManager($1, yylloc.first_line, yylloc.last_column-yylloc.tamanio); } expresion ':' listaSentencias 
    | DEFAULT ':' { otherStatementManager($1, yylloc.first_line, yylloc.last_column-yylloc.tamanio-1); } listaSentencias 
    ;

sentIteracion:
    WHILE { posManager(yylloc.last_line, yylloc.last_column-yylloc.tamanio); statementManager($1); } '(' expresion ')' sentencia 
    | DO { posManager(yylloc.last_line, yylloc.last_column-yylloc.tamanio); } sentencia WHILE '(' expresion ')' PUNTOCOMA { char *aux="/"; $$=strcat($1,aux); $$=strcat($$,$4); statementManager($$); }
    | FOR { posManager(yylloc.last_line, yylloc.last_column-yylloc.tamanio); statementManager($1); } '(' expresion  expresion  PUNTOCOMA expresion ')' sentencia 
    ;

sentSalto:
    RETURN { otherStatementManager($1, yylloc.first_line, yylloc.last_column-yylloc.tamanio); } expresion PUNTOCOMA 
    | RETURN { otherStatementManager($1, yylloc.first_line, yylloc.last_column-yylloc.tamanio); } PUNTOCOMA 
    | BREAK { otherStatementManager($1, yylloc.first_line, yylloc.last_column-yylloc.tamanio); } PUNTOCOMA 
    | CONTINUE { otherStatementManager($1, yylloc.first_line, yylloc.last_column-yylloc.tamanio); }PUNTOCOMA 
    ;

//fin sentencias
%%

/* Fin de las reglas gramaticales */

int main(int argc, char **argv){
    inicializarUbicacion();
    nombreArchivoEntrada=argv[1];
    //limpiarArchivo("parser_log.txt");

    if(argc > 1){
        FILE *file = fopen(argv[1], "r");

        if(!file){
            perror("No se puede abrir el archivo");
            return 1;
        }

        yyin = file;
    }
    else{
        fprintf(stderr, "No se especificó un archivo de entrada.\n");
        return 1;
    }
    
    #if YYDEBUG
    yydebug = 1;
    #endif

    yyparse();
    printVarList(listaVar);
    printFunction(listaFunc);
    printStatementList(listaSentenciasFinal);
    printEstructurasNoReconocidas(listaEstructurasNoReconocidas);
    printNoReconocidas(listaNoReconocidas);

    //pausa();
    return 0;
}

void yyerror(const char* literalCadena) {
    //fprintf(stderr, "* Listado de estructuras sintácticas no reconocidas\n");
    //fprintf(stderr, "\"%s\": linea %d\n", literalCadena, yylloc.first_line);
}

/*void manejarError(char* texto, int linea) {
    errores = realloc(errores, sizeof(Error) * (contadorErrores + 1));
    errores[contadorErrores].texto = strdup(texto);
    errores[contadorErrores].linea = linea;
    contadorErrores++;
}*/
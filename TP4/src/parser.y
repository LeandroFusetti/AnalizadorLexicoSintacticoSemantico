%{

#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include "general.h"
#include <string.h>
#include "tabla_de_simbolos.h"

/* Declaración de la función yylex del analizador léxico, necesaria para que la función yyparse del analizador sintáctico pueda invocarla cada vez que solicite un nuevo token */
extern int yylex(void);
extern FILE *yyin;
extern int yylineno;
extern char* yytext;
Error* errores = NULL;
int contadorErrores = 0;

/* Declaración de la función yyerror para reportar errores, necesaria para que la función yyparse del analizador sintáctico pueda invocarla para reportar un error */
void yyerror(const char*);

tNodoError* listaErrores = NULL;
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
tNodo* listaDeVariables = NULL;
tNodo* listaDeFunciones = NULL;
tNodo* listaIdsRepetidos = NULL;
tNodo* listaFuncionesMezcladas = NULL;
tNodo* listaTiposReturn = NULL;
tNodo* tablaSimbolos = NULL;
tError* listaErroresSemanticos = NULL;
tError* operandosInvalidos=NULL;
tNodo* listaParametrosInvocacion=NULL;
tNodo* listaInit=NULL;
char* nombreArchivoEntrada;
int linea;
int columna;
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

    struct cte{
        int cteDecimal;
        double cteReal;
        char cSimple;
        char* id;
        char* tipo;
        char* id2;
        char* tipo2;
        int linea;
        int columna;
    } ctes;

    int operable;

    char* type; 
    char* id;
    char* usigned;
    char* sentencia;
    char* constante;

    struct parametro{
        char* id;
        char* tipo;
    } parametro;
}

%token <id> IDENTIFICADOR 
%token <ctes> CONSTANTE 
%token <type> LITERAL_CADENA 
%token <type> TIPO_DATO 
%token PLUSPLUS MINUSMINUS ADDASSIGN
%token EQUAL NOTEQUAL AND OR LEQ GEQ SIZEOF
%token PUNTOCOMA ";"
%token <ctes>CHAR_SIMPLE
%token <sentencia> IF ELSE WHILE RETURN DO FOR CASE SWITCH BREAK CONTINUE DEFAULT  
%token VOID 
%token <type> UNSIGNED CONST
%token MULASSIGN SUBASSIGN DIVASSIGN  
%type <ctes> unaVarSimple
%type <ctes> listaVarSimples
%type <type> tipoDato
%type <sentencia> sentSeleccion
%type <sentencia> sentIteracion
%type <parametro> parametro
%type <ctes> expPrimaria
%type <ctes> expPostfijo
%type <ctes> expUnaria
%type <ctes> expAditiva
%type <ctes> expMultiplicativa
%type <ctes> expresion
%type <ctes> expRelacional
%type <ctes> expIgualdad
%type <ctes> expAnd
%type <ctes> expOr
%type <ctes> expCondicional
%type <ctes> expAsignacion
%type <ctes> sentencia
%type <ctes> sentSalto
%type <ctes> inicializacion
%type <parametro> listaArgumentos
%type <parametro> listaArgumentosOP

%start programa

%%

programa:
    /* epsilon */
    | programa expresion 
    ;

//expresion
expresion:  
    expAsignacion { $$ = $1; }
    | declaracion  
    | sentencia
    ;

expAsignacion:
    expCondicional { $$ = $1; }
    | expUnaria operAsignacion expAsignacion {
        valorConstante($1.tipo,$1.id);
        valorVoid($3.tipo,$1.id);
    }
    ;

operAsignacion: 
    '=' 
    | ADDASSIGN 
    | SUBASSIGN 
    | MULASSIGN 
    | DIVASSIGN 
    ;

expCondicional:
    expOr { $$ = $1; }
    | expOr '?' expresion ':' expCondicional 
    ;

expOr:
    expAnd { $$ = $1; }
    | expOr OR expAnd 
    ;

expAnd:
    expIgualdad { $$ = $1; }
    | expAnd AND expIgualdad 
    ;

expIgualdad:
    expRelacional { $$ = $1; }
    | expIgualdad EQUAL expRelacional 
    | expIgualdad NOTEQUAL expRelacional 
    ;

expRelacional:
    expAditiva { $$ = $1; }
    | expRelacional '<' expAditiva 
    | expRelacional '>' expAditiva 
    | expRelacional LEQ expAditiva 
    | expRelacional GEQ expAditiva 
    ;

expAditiva:
    expMultiplicativa { $$ = $1; }
    | expAditiva '+' expUnaria // Verificamos si los dos son operables
    | expAditiva '-' expUnaria // Verificamos si los dos son operables
    ;

expMultiplicativa:
    expUnaria { $$ = $1; }
    | expMultiplicativa '*' expUnaria {
        if(sonOperables($1.tipo, $3.tipo)){
            $$ = $1; 
            //printf("se opero correctamente un %s con un %s\n",$1.tipo,$3.tipo);
        }
        else{
                    tError* nuevoError = (tError*)malloc(sizeof(tError));
                    nuevoError->linea = yylloc.first_line;
                    nuevoError->columna = yylloc.first_column-4;
                    asprintf(&nuevoError->mensaje, "Operandos invalidos del operador binario * (tienen '%s' y '%s')", $1.tipo, $3.tipo);
                    operandosInvalidos = insertarErrorAlFinal(operandosInvalidos, nuevoError);
        }
    }
    | expMultiplicativa '/' expUnaria // Verificamos si los dos son operables
    ;

expUnaria:
    expPostfijo { $$ = $1; }
    | PLUSPLUS expUnaria { $$ = $2; }
    | MINUSMINUS expUnaria { $$ = $2; }
    | expUnaria PLUSPLUS { $$ = $1; }
    | expUnaria MINUSMINUS { $$ = $1; }
    | operUnario expUnaria { $$ = $2; }
    | SIZEOF '(' tipoDato ')' 
    ;

operUnario:
    '&' 
    | '*' 
    | '-' 
    | '!' 
    ;

expPostfijo:
    expPrimaria { $$ = $1; }
    | expPostfijo '[' expresion ']' { $$ = $1; }
    | expPostfijo '(' listaArgumentosOP ')'{ $$ = $1; validarSemanticamenteInvocaciones($1.id, $1.tipo, $1.linea, $1.columna); } //TODO: INVOCACION A FUNCIONES mismo tipo y cantidad de paramaetros aca hay que varificarTipoFuncion($1,$3)
    ;

listaArgumentosOP:
    /*vacio*/{}
    | listaArgumentos 
    ;

listaArgumentos:
    expAsignacion {tInfo aux;aux.id=$1.id;aux.type=$1.tipo;listaParametrosInvocacion=insertarAlFinal(listaParametrosInvocacion,aux);}
    | listaArgumentos ',' expAsignacion {tInfo aux;aux.id=$3.id;aux.type=$3.tipo;listaParametrosInvocacion=insertarAlFinal(listaParametrosInvocacion,aux);}
    ;

expPrimaria:
    IDENTIFICADOR { $$.id = $1; $$.linea = yylloc.first_line; $$.columna = yylloc.first_column; $$.tipo = strdup(solicitarTipo($1)); }
    | CONSTANTE {$$.id="cte"; $$.tipo = strdup("int"); }
    | LITERAL_CADENA { $$.tipo = strdup("char*"); $$.id = $1; }
    | CHAR_SIMPLE { $$.tipo = strdup("char"); }
    | '(' expresion ')' { $$ = $2; }
    ;

//fin expresiones

//declaracion de variables
declaracion:
    declaVarSimples 
    | declaracionFuncion
    ;

declaVarSimples:
    tipoDato listaVarSimples {inicializacion($1,linea,columna);varManager($1, linea);operandosInvalidosFunc(); }
    ;

listaVarSimples:
    unaVarSimple { $$=$1;}
    | listaVarSimples ',' unaVarSimple {$$=$3; }
    ;

unaVarSimple:
    IDENTIFICADOR { linea = yylloc.first_line; columna = yylloc.tempColumn; tInfo aux; aux.id = $1; aux.column = columna; listaVarComp = insertarAlFinal(listaVarComp, aux);$$.id=$1; }
    | IDENTIFICADOR { linea = yylloc.first_line; columna = yylloc.tempColumn; tInfo aux; aux.id = $1; aux.column = columna; listaVarComp = insertarAlFinal(listaVarComp, aux); } inicializacion {tInfo aux;aux.id=$1;aux.type=$3.tipo;listaInit=insertarAlFinal(listaInit,aux);}
    ;

inicializacion:
   '=' expresion {$$=$2; /*printf("valor de la inicializacion %s\n",$2.tipo);*/}
   ;

/* Declaración de funciones */
declaracionFuncion:
    prototipoFuncion    
    | declaraFuncion;
    ;

declaraFuncion:
    tipoDato IDENTIFICADOR '(' listaParametros ')' sentencia { functionManager($2, $1, 0, @2.first_line, @2.first_column); }
    ;

prototipoFuncion:
    tipoDato IDENTIFICADOR '(' listaParametros ')' PUNTOCOMA { functionManager($2, $1, 1, @2.first_line, @2.first_column); }
    ;

tipoDato:
    TIPO_DATO {$$=$1;}//{printf("el tipo de la variable es %s\n",$1); }
    | VOID { $$ = "void"; }
    | UNSIGNED TIPO_DATO { char *aux = " "; $$ = strcat($1, aux); $$ = strcat($$, $2); /*printf("el tipo de la variable es %s\n", $$)*/; }
    | CONST TIPO_DATO { char *aux = " "; $$ = strcat($1, aux); $$ = strcat($$, $2); }
    ;

listaParametros:
    parametro { parameterManager($1.id, $1.tipo); }
    | listaParametros ',' parametro { parameterManager($3.id, $3.tipo); }
    | /* vacío */ 
    ;

parametro:
    tipoDato IDENTIFICADOR { $$.tipo = strdup($1); $$.id = strdup($2); }
    | tipoDato { $$.tipo = strdup($1); $$.id = ""; } /* Para reconocer parámetros anónimos, como en (int, float) */
    ;

//fin de declaracion de funciones
//sentencias

sentencia:
    sentCompuesta 
    | sentExpresion 
    | sentSeleccion 
    | sentIteracion 
    | sentSalto {}
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
    IF '(' expresion ')' sentencia { lineaIF = yylloc.tempRow; columnaIF = yylloc.tempColumn; int caso = 0; ifManager(lineaIF, columnaIF, caso); }
    | IF '(' expresion ')' sentencia ELSE sentencia { lineaIF = yylloc.tempRow; columnaIF = yylloc.tempColumn; int caso = 1; ifManager(lineaIF, columnaIF, caso); }
    | SWITCH { otherStatementManager($1, yylloc.first_line, yylloc.first_column ); } '(' expresion ')' '{' listaCasos '}' 
    ;

listaCasos:
    caso 
    | listaCasos caso 
    ;

caso:
    CASE { otherStatementManager($1, yylloc.first_line, yylloc.first_column); } expresion ':' listaSentencias 
    | DEFAULT ':' { otherStatementManager($1, yylloc.first_line, yylloc.first_column); } listaSentencias 
    ;

sentIteracion:
    WHILE { posManager(yylloc.last_line, yylloc.first_column); statementManager($1); } '(' expresion ')' sentencia 
    | DO { posManager(yylloc.last_line, yylloc.first_column); } sentencia WHILE '(' expresion ')' PUNTOCOMA { char *aux = "/"; $$ = strcat($1, aux); $$ = strcat($$, $4); statementManager($$); }
    | FOR { posManager(yylloc.last_line, yylloc.first_column); statementManager($1); } '(' expresion  expresion  PUNTOCOMA expresion ')' sentencia 
    ;

sentSalto:
    RETURN expresion PUNTOCOMA { $$ = $2; meterTipoEnLista($2.tipo, $2.id); }
    | RETURN PUNTOCOMA { meterTipoEnLista("void","id de retorno no reconocido"); otherStatementManager($1, yylloc.first_line, yylloc.first_column); }
    | BREAK { otherStatementManager($1, yylloc.first_line, yylloc.first_column); } PUNTOCOMA 
    | CONTINUE { otherStatementManager($1, yylloc.first_line, yylloc.first_column); } PUNTOCOMA 
    ;
//fin sentencias
%%

/* Fin de las reglas gramaticales */

int main(int argc, char **argv){
    nombreArchivoEntrada = argv[1];

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

    //inicializarTablaSimbolos(); // Inicializa la tabla de símbolos

    yyparse();
    printVarList(listaVar);
    printFunction(listaFuncionesMezcladas);

    imprimirErrores(listaErroresSemanticos); // NUEVO

    printEstructurasNoReconocidas(listaEstructurasNoReconocidas);
    printNoReconocidas(listaNoReconocidas);

    //imprimirTablaSimbolos(tablaSimbolos); // Imprime la tabla de símbolos NUEVO

    return 0;
}

void yyerror(const char* literalCadena) {
    //fprintf(stderr, "* Listado de estructuras sintácticas no reconocidas\n");
    //fprintf("");
}

/*void manejarError(char* texto, int linea) {
    errores = realloc(errores, sizeof(Error) * (contadorErrores + 1));
    errores[contadorErrores].texto = strdup(texto);
    errores[contadorErrores].linea = linea;
    contadorErrores++;
}*/
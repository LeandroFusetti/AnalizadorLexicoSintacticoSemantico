#ifndef GENERAL_H
#define GENERAL_H

/* En los archivos de cabecera (header files) (*.h) poner DECLARACIONES (evitar DEFINICIONES) de C, así como directivas de preprocesador */
/* Recordar solamente indicar archivos *.h en las directivas de preprocesador #include, nunca archivos *.c */

#define YYLTYPE YYLTYPE

typedef struct YYLTYPE{
	int first_line;
	int first_column;
	int last_line;
	int last_column;
	int eof;
	int tempRow;
	int varRow;
	int tamanio;
	int tempColumn;
} YYLTYPE;

typedef struct tInfo{
    char* id;
    char* type;
    int column;
    int row;
    char* sentencia;
    struct parameter* listaParametros;
    short proto;
    char* cadenaFinal;
} tInfo;

typedef struct tNodo{
	tInfo info;
	struct tNodo* sgte;
} tNodo;

struct param{
  char* tipo;
  char* id;
};

typedef struct parameter{
	struct param info;
	struct parameter* sgte;

} parameter;

typedef struct error_semantico{
    char* identificador;
    char* tipo_1; // Usamos este tipo_1 para marcar el tipo de nuestro identificador y el tipo_2 en caso de que necesitemos almacenar un tipo antiguo, de otro identificador, etc.
    char* tipo_2;
    int linea_1;
    int columna_1;
    int linea_2;
	char* mensaje;
    int columna_2; //lo mismo ocurre con linea y columna, en caso de necesitar mostrar 2 lineas y columnas distintas usamos linea 1 y 2.
    int tipo_de_error; //dependiendo del tipo de error que tenga se va a hacer un printf distinto a la hora de mostrar esto
} error_semantico;

typedef struct {
	struct error_semantico info;
	struct tNodoError* sgte;
} tNodoError;

typedef struct {
    char* texto;
    int linea;
} Error;

int sonOperables(char*, char*);
char* solicitarTipo(char*);
void reinicializarUbicacion(void);
void log_message(const char*);
void limpiarArchivo(const char*);
void varManager(char*, int); //nombre, tipo, fila
void printVarList(tNodo*);
void statementManager(char*); // nombre, fila, columna
void otherStatementManager(char*, int, int); //return, brake, switch, case...
void printStatementList(tNodo*);
tNodo* insertarAlFinal(tNodo*, tInfo);
void posManager(int, int);
void parameterManager(char*, char*);
void functionManager(char*, char*, short, int, int);
void ifManager(int, int, int);
void printIF(tNodo*);
void noReconocidasManager(char*, int, int);
void printNoReconocidas(tNodo*);
void printEstructurasNoReconocidas(tNodo*);
char* obtenerLineaDelError(const char*, int);
void manejarError(char*, int);
void meterTipoEnLista(char*,char*);
tInfo sacarTipoLista();
tNodoError* insertarAlFinalError(tNodo*, error_semantico);
void validarSemanticamenteInvocaciones(char*,char*,int,int);
void inicializacion(char*,int,int);
void valorConstante(char*,char *);
void valorVoid(char*,char*);
#endif
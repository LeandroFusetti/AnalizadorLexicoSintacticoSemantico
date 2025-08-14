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
	int funcRow;
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

typedef struct {
    char* texto;
    int linea;
} Error;

#define INICIO_CONTEO_LINEA 1
#define INICIO_CONTEO_COLUMNA 1

void pausa(void);
void inicializarUbicacion(void);
void reinicializarUbicacion(void);
void log_message(const char *);
void limpiarArchivo(const char *);
void newLine();
void updateColumn(int);
void varManager(char*, int); //nombre, tipo, fila
void printVarList(tNodo*);
void statementManager(char*); // nombre, fila, columna
void otherStatementManager(char*, int, int); //return, brake, switch, case...
void printStatementList(tNodo*);
tNodo* insertarAlFinal(tNodo*, tInfo);
void posManager(int, int);
void parameterManager(char*, char*);
void functionManager(char*, char*, short, int);
void ifManager(int, int, int);
void printIF(tNodo*);
void noReconocidasManager(char*, int, int);
void printNoReconocidas(tNodo*);
void printEstructurasNoReconocidas(tNodo*);
char* obtenerLineaDelError(const char*, int);
void manejarError(char*, int);

#endif
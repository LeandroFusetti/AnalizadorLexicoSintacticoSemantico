#include<stdlib.h>
#include<math.h>
#include<stdio.h>
#include<string.h>
//declaracion struct
typedef struct tInfo
{
    char* field;
    int column;
    int row;
}tInfo;

typedef struct tNodo
{
    tInfo info;
    struct tNodo* sgte;
}tNodo;

//declaracion de listas globables
tNodo* listaType = NULL;
tNodo* listaCS=NULL;
tNodo* listaO=NULL;
tNodo* listaInt=NULL;
tNodo* listaHex=NULL;
tNodo* listaOct=NULL;
tNodo* listaFloat= NULL;
tNodo* listaChar=NULL;
tNodo* listaYY=NULL;

//funciones genericas 
tNodo* insertarAlFinal(tNodo* lista,tInfo info){
    tNodo* p = (tNodo*)malloc(sizeof(tNodo));
    p->info = info;
    if (lista == NULL) {
        p->sgte = lista;
        lista = p;
    } else {
        tNodo* q = lista;
        while (q->sgte != NULL) {//strcmp(valor.id, q->sgte->info.id) > 0
            q = q->sgte;
        }
        p->sgte = q->sgte;
        q->sgte = p;
    }
    return lista;
}

int stringToInt(const char *hexStr,int num) {
    // strtol convierte la cadena hexStr en un número entero en base 16
    return strtol(hexStr, NULL, num);
}


//-----------Tipo de datos-----------

void typemanager(char* output,int row,int column){
    tInfo info;
    info.field=strdup(output);
    info.column=column;
    info.row=row;

    listaType = insertarAlFinal(listaType, info);

}

void printTypeList(tNodo* lista){
    printf("\n* Listado de palabras reservadas (tipos de dato):\n");
    if(lista==NULL){
        printf("-");
        return ;
    }
    while (lista!=NULL)
    {
        printf("%s: linea %d, columna %d \n",lista->info.field,lista->info.row,lista->info.column);
        lista=lista->sgte;
    }
    
}




//------------------Estructuras de control-------------------

void CSmanager(char* output,int row,int column){
    tInfo info;
    info.field=strdup(output);
    info.column=column;
    info.row=row;

    listaCS = insertarAlFinal(listaCS, info);
   
}

void printCSList(tNodo* lista){
    printf("\n* Listado de palabras reservadas (estructuras de control):\n");
    if(lista==NULL){
        printf("-");
        return ;
    }
    while (lista!=NULL)
    {
        printf("%s: linea %d, columna %d \n",lista->info.field,lista->info.row,lista->info.column);
        lista=lista->sgte;
    }
    
}

//-----------Otras-----------

void Omanager(char* output,int row,int column){
    tInfo info;
    info.field=strdup(output);
    info.column=column;
    info.row=row;

    listaO = insertarAlFinal(listaO, info);
   
}

void printOList(tNodo* lista){
    printf("\n* Listado de palabras reservadas (otros):\n");
    if(lista==NULL){
        printf("-");
        return ;
    }
    while (lista!=NULL)
    {
        printf("%s: linea %d, columna %d \n",lista->info.field,lista->info.row,lista->info.column);
        lista=lista->sgte;
    }
    
}

//------------Constantes enteras decimales--------------------




void intManager(char* output){
    tInfo info;
    info.field=strdup(output);

    
    listaInt=insertarAlFinal(listaInt,info);
}

void printInt(tNodo* lista){
    int acum;
    printf("\n* Listado de constantes enteras decimales: \n");
    if(lista==NULL){
        printf("-");
        return ;
    }
    while (lista!=NULL)
    {
        printf("%s: valor %d \n",lista->info.field,stringToInt(lista->info.field,10));
        acum+=stringToInt(lista->info.field,10);
        lista=lista->sgte;
    }
    if (acum==0)
    {
        return;
    }
    
    printf("Total acumulado de sumar todas las constantes decimales: %d \n",acum);
}

//-----------Constantes enteras hexadecimales------------


void hexManager(char* output){
    tInfo info;
    info.field=strdup(output);

    listaHex=insertarAlFinal(listaHex,info);
}

void printHex(tNodo* lista){
    printf("\n* Listado de constantes enteras hexadecimales:\n");
    if(lista==NULL){
        printf("-");
        return ;
    }
    while (lista!=NULL)
    {
        printf("%s: valor entero decimal %d \n",lista->info.field,stringToInt(lista->info.field,16));
        lista=lista->sgte;
    }
}

//-----------------constantes octales---------------------


void octManager(char* output){
    tInfo info;
    info.field=strdup(output);

    
    listaOct=insertarAlFinal(listaOct,info);
}

void printOct(tNodo* lista){
    printf("\n* Listado de constantes enteras octales:\n");
    if(lista==NULL){
        printf("-");
        return ;
    }
    while (lista!=NULL)
    {
        printf("%s: valor entero decimal %d \n",lista->info.field,stringToInt(lista->info.field,8));
        lista=lista->sgte;
    }
}

//-------------------constantes reales---------------

void printIntegerAndMantissa(const char *floatStr) {
    // Convierte la cadena a un número float
    float num = strtof(floatStr, NULL);

    // Separa la parte entera y la mantisa
    int integerPart = (int)num;
    float mantissa = fabs(num - integerPart);
    float integerFPart=integerPart;
    // Imprime la parte entera y la mantisa
    printf("%s: parte entera %.6f, mantisa %f\n",floatStr,integerFPart, mantissa);
   
}

void floatManager(char* output){
    tInfo info;
    info.field=strdup(output);

    listaFloat=insertarAlFinal(listaFloat,info);
}

void printFloat(tNodo* lista){
    printf("\n* Listado de constantes reales:\n");
    if(lista==NULL){
        printf("-");
        return ;
    }
    while (lista!=NULL)
    {
        printIntegerAndMantissa(lista->info.field);
        lista=lista->sgte;
    }
}

//-----------------listado constante caracter enumeradas ------------------

void charManager(char* output){
    tInfo info;
    info.field=strdup(output);

    listaChar=insertarAlFinal(listaChar,info);
}

void printChar(tNodo* lista){
    int i=1;
    printf("* Listado de constantes caracter enumerados:\n ");
    if(lista==NULL){
        printf("-");
        return ;
    }
    while (lista!=NULL)
    {
        printf("%d) %s \n",i,lista->info.field);
        i++;
        lista=lista->sgte;
    }

}

//listado de palabras no reconocidas

void yyoutManager(char* output){
    tInfo info;
    info.field=strdup(output);
    if (output==" ")
    {
        return;
    }
    
    listaYY=insertarAlFinal(listaYY,info);
}

void printYY(tNodo* lista){
    printf("* Listado de cadenas no reconocidas:\n ");
    if(lista==NULL){
        printf("-");
        return ;
    }
    while (lista!=NULL)
    {
        printf("%s",lista->info.field);
        lista=lista->sgte;
    }

}
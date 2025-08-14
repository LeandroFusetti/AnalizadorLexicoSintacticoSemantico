#include <stdlib.h>
#include <stdio.h>
#include<stdbool.h>

#define CANT_ESTADOS 7
#define CANT_SIMBOLOS 5
#define ESTADO_INICIAL Q0
#define CENTINELA ','
typedef enum {
    Q0,  // Estado inicial
    Q1,  // Estado FINAL
    Q2,  // Estado FINAL
    Q3,  // Estado FINAL
    Q4,  // Estado Intermedio
    Q5,  // Estado FINAL
    QX   // Estado error
} t_estado;
typedef enum{
    CERO,
    UNOSIETE,
    OCHONUEVE,
    afAF,
    xX,
    error
} t_caracter;

int FinalstateArray[4]={Q1,Q2,Q3,Q5};
//Q1=decimal
//Q2=octal
//Q3=octal
//Q5=hexa
int tabla_transiciones[CANT_ESTADOS][CANT_SIMBOLOS] = {
//   0[1-7][8,9][a-f,A-F][Xx]
    {Q2, Q1, Q1, QX, QX},  //Q0
    {Q1, Q1, Q1, QX, QX},  //Q1
    {Q3, Q3, QX, QX, Q4},  //Q2
    {Q3, Q3, QX, QX, QX},  //Q3
    {Q5, Q5, Q5, Q5, QX},  //Q4
    {Q5, Q5, Q5, Q5, QX},  //Q5
    {QX, QX, QX, QX, QX}   //Qx
};

int char_to_enum(char c){
   
    if(c =='a'||c=='b'||c=='c'||c=='d'||c=='e'||c=='f'||c=='A'||c=='B'||c=='C'||c=='D'||c=='E'||c=='F'){
        return afAF;
    }
    else if (c=='0')
    {
        return CERO;
    }    
    else if (c=='1'||c=='2'||c=='3'||c=='4'||c=='5'||c=='6'||c=='7')
    {
        return UNOSIETE ;
    }
    else if (c=='8'||c=='9'){
        return OCHONUEVE;
    }
    
    else if (c=='x'||c=='X')
    {
        return xX;
    }
    else{
        return error;
    }
}
bool checkArray(int state){
    for (short i = 0; i < 4; i++)
    {
        if (state==FinalstateArray[i])
        {
          return true;
        }
        
    }
    return false;
}

void printResult(int estado, FILE*output){
     fputs("    ", output);  
            
            if(checkArray(estado)){
                switch (estado){
                    case 1:
                        fputs("DECIMAL\n",output);
                        break;
                    case 2:
                    case 3:
                        fputs("OCTAL\n",output);
                        break;
                    case 5:
                        fputs("HEXADECIMAL\n",output);
                        break;
                }
               
            }
            else{
                 fputs("NO RECONOCIDA\n", output);  
            }
}

void Lexer(FILE* input,FILE* output){
    int estado = ESTADO_INICIAL;
    char c;
    while((c = fgetc(input)) != EOF){
        if(c != CENTINELA){
            fputc(c, output); 
            estado = tabla_transiciones[estado][char_to_enum(c)]; 
        }
        else{
            printResult(estado,output);
            estado = ESTADO_INICIAL;  
        }
    }
   printResult(estado,output);
}

#ifndef QUICKSORTEXT_H
#define QUICKSORTEXT_H

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#define TAMAREA 20
#define INT_MIN -1
#define INT_MAX TAMAREA+1

typedef struct{
    long chave;
    double nota;
    char estado[3];
    char cidade[51];
    char curso[31];
}Registro;

typedef Registro TipoArea[TAMAREA];

void QuicksortExterno (FILE **ArqLi , FILE **ArqEi , FILE **ArqLEs, int Esq, int Dir);
void LeSup(FILE **ArqLEs, Registro *UltLido , int *Ls , bool *OndeLer);
void LeInf(FILE **ArqLi , Registro *UltLido , int *Li , bool *OndeLer);
void InserirArea (TipoArea *Area, Registro *UltLido , int *NRArea);

#endif
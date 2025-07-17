#ifndef QUICKSORTEXT_H
#define QUICKSORTEXT_H

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#define TAMAREA 20
#define INT_MIN -1
#define INT_MAX TAMAREA+1

void QuicksortExterno (FILE **ArqLi , FILE **ArqEi , FILE **ArqLEs, int Esq, int Dir);
void LeSup(FILE **ArqLEs, Registro *UltLido , int *Ls , bool *OndeLer);
void LeInf(FILE **ArqLi , Registro *UltLido , int *Li , bool *OndeLer);
void InserirArea (TipoArea *Area, Registro *UltLido , int *NRArea);
void EscreveMax(FILE **ArqLEs, Registro R, int *Es);
void EscreveMin(FILE **ArqEi , Registro R, int *Ei);
void RetiraMax(TipoArea *Area, Registro *R, int *NRArea);
void RetiraMin(TipoArea *Area, Registro *R, int *NRArea);
void Particao(FILE **ArqLi , FILE **ArqEi , FILE **ArqLEs, TipoArea Area, int Esq, int Dir , int * i , int * j );

#endif
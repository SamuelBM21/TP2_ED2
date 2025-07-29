#ifndef AREA_H
#define AREA_H

#include <float.h>
#include <stdio.h>

#define TAMAREA 20

typedef struct{
    long chave;
    double nota;
    char estado[3];
    char cidade[51];
    char curso[31];
}Registro;

typedef struct {
    Registro itens[TAMAREA];
    int n; // Número de elementos ocupados
} TipoArea;

void FAVazia(TipoArea *Area);
void InsereItem(Registro reg, TipoArea *Area);
void RetiraUltimo(TipoArea *Area, Registro *reg);
void RetiraPrimeiro(TipoArea *Area, Registro *reg);
int ObterNumCelOcupadas(TipoArea *Area);

#endif
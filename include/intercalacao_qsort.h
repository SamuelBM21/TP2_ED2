#ifndef INTERCALACAO_BALANCEADA_H
#define INTERCALACAO_BALANCEADA_H

#include "area.h"
#include <stdlib.h>
#include <math.h>
#define TAM_MEM 20
#define TAM_NOME 32
#define TOTAL_FITAS 40
#define FITAS 20

// Adiciona esta estrutura para controlar o estado de cada fita
typedef struct {
    FILE *arquivo;
    int blocosRestantes;
    int elementosRestantes;
    short ativo;
} FitaEstado;

// Executa a intercalação balanceada nas fitas
int numFitasPreenchidas(int elementos[], int inicio, int *ultimaFita);
int menorRegistroAtivo(Registro registros[], short ativos[], int numFitas);
void quickSort(Registro *v, int esquerda, int direita);
void gerarBlocosOrdenados(const char *inputFile, int totalRegs, int numBlocos[], int nElem[]);
void intercalacaoBalanceada(const char *inputFile, int totalBlocos);

#endif // INTERCALACAO_BALANCEADA_H

#ifndef INTERCALACAO_BALANCEADA_H
#define INTERCALACAO_BALANCEADA_H

#include "area.h"
#include <stdlib.h>
#include <math.h>                           //Para o uso de 'pow()'

#define TAM_MEM 20                          //Espaço na memória
#define TAM_NOME 32                         //Tamanho do nome dos arquivos
#define TOTAL_FITAS 40                      //Número total de fitas
#define FITAS 20                            //Metade das fitas

//Estrutura para controlar o estado de cada fita
typedef struct {
    FILE *arquivo;
    int blocosRestantes;
    int elementosRestantes;
    short ativo;
    
} FitaEstado;

//Funções
int numFitasPreenchidas(int elementos[], int inicio, int *ultimaFita);
int menorRegistroAtivo(Registro registros[], short ativos[], int numFitas);
void quickSort(Registro *v, int esquerda, int direita);
void gerarBlocosOrdenadosQS(const char *inputFile, int totalRegs, int numBlocos[], int nElem[]);
void intercalacaoBalanceadaQS(const char *inputFile, char *outFile, int totalBlocos);

void intercalacaoBalanceadaSS(const char *inputFile, char *outFile, int totalRegs);
void gerarBlocosOrdenadosSS(const char *inputFile, int totalRegs, int numBlocos[], int nElem[]);
void construirHeapMin(HeapElem heap[], int n);
void heapify(HeapElem heap[], int n, int i);
void gerarResumoFitas(int numFitas);

#endif // INTERCALACAO_BALANCEADA_H

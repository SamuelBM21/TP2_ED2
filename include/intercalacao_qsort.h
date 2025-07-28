#ifndef INTERCALACAO_BALANCEADA_H
#define INTERCALACAO_BALANCEADA_H

#include "area.h"
#include <stdlib.h>
#define MEMORIA 20
#define FITAS 40
#define FITAS_ENTRADA 20
#define FITAS_SAIDA 20

// Função de comparação para ordenação
int compara(const void *a, const void *b);

// Cria blocos ordenados de entrada
int criaBlocos(char *arquivoEntrada);

// Lê um registro de uma fita (arquivo)
int leRegistro(FILE *fp, Registro *r);

// Executa a intercalação balanceada nas fitas
void intercalacaoBalanceada(int totalBlocos);

#endif // INTERCALACAO_BALANCEADA_H

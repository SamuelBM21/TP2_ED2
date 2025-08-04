#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "../include/area.h"
#include "../include/intercalacao_qsort.h"  
#include "../include/conversor.h"  

int numFitasPreenchidas(int elementos[], int inicio, int *ultimaFita) {
    int count = 0;
    *ultimaFita = -1;

    for (int i = 0; i < FITAS; i++) {
        if (elementos[inicio + i] > 0) {
            count++;
            *ultimaFita = inicio + i;
        }
    }
    return count;
}

// Função para encontrar o índice do menor registro ativo
int menorRegistroAtivo(Registro registros[], short ativos[], int numFitas) {
    int menor = -1;
    for (int i = 0; i < numFitas; i++) {
        if (ativos[i]) {
            if (menor == -1 || registros[i].nota < registros[menor].nota) {
                menor = i;
            }
        }
    }
    return menor;
}

void quickSort(Registro *v, int esquerda, int direita) {
    // Implementação do QuickSort permanece a mesma
    if (esquerda >= direita) return;
    Registro pivo = v[(esquerda + direita) / 2];
    int i = esquerda, j = direita;
    while (i <= j) {
        while (v[i].nota < pivo.nota) i++;
        while (v[j].nota > pivo.nota) j--;
        if (i <= j) {
            Registro tmp = v[i];
            v[i] = v[j];
            v[j] = tmp;
            i++;
            j--;
        }
    }

    if (esquerda < j) quickSort(v, esquerda, j);
    if (i < direita) quickSort(v, i, direita);
}

void gerarBlocosOrdenados(const char *inputFile, int totalRegs, int numBlocos[], int nElem[]) {
    FILE *entrada = fopen(inputFile, "rb");
    if (!entrada) {
        fprintf(stderr, "Erro ao abrir '%s': %s\n", inputFile, strerror(errno));
        exit(EXIT_FAILURE);
    }

    Registro memoria[TAM_MEM];
    int lidos = 0, fita = 0;

    // Inicialização dos arrays
    for(int i = 0; i < TOTAL_FITAS; i++) {
        numBlocos[i] = 0;
        nElem[i] = 0;
    }

    while (lidos < totalRegs) {
        int cnt = 0;
        while (cnt < TAM_MEM && fread(&memoria[cnt], sizeof(Registro), 1, entrada) == 1) {
            cnt++;
            lidos++;
        }

        quickSort(memoria, 0, cnt - 1);

        char nomeFita[TAM_NOME];
        sprintf(nomeFita, "fitas/fita%02d.bin", fita);
        FILE *f = fopen(nomeFita, "ab");
        if (!f) {
            fprintf(stderr, "Erro ao criar '%s': %s\n", nomeFita, strerror(errno));
            exit(EXIT_FAILURE);
        }

        size_t escritos = fwrite(memoria, sizeof(Registro), cnt, f);
        if (escritos != (size_t)cnt) {
            fprintf(stderr, "Erro na escrita em '%s'\n", nomeFita);
            exit(EXIT_FAILURE);
        }
        fclose(f);
        
        numBlocos[fita]++;
        nElem[fita] += cnt;
        fita = (fita + 1) % FITAS;  // Rotação entre fitas
    }
    fclose(entrada);
}

void intercalacaoBalanceada(const char *inputFile, int totalRegs) {
    system("mkdir -p fitas");
    
    // Inicialização de variáveis
    int numBlocos[TOTAL_FITAS] = {0};
    int nElem[TOTAL_FITAS] = {0};
    int inicioEntrada = 0;
    int inicioSaida = FITAS;
    int ultimaFitaComDados = -1;
    int passagem = 0;
    int numFitasComDados;

    // Gerar blocos iniciais
    gerarBlocosOrdenados(inputFile, totalRegs, numBlocos, nElem);

    // Inicializar estados das fitas
    FitaEstado estados[TOTAL_FITAS];
    memset(estados, 0, sizeof(estados));

    do {
        printf("\n=== Passagem %d ===\n", passagem++);
        
        // Abrir fitas de entrada
        for (int i = 0; i < FITAS; i++) {
            char nome[50];
            sprintf(nome, "fitas/fita%02d.bin", inicioEntrada + i);
            estados[inicioEntrada + i].arquivo = fopen(nome, "rb");
            estados[inicioEntrada + i].blocosRestantes = numBlocos[inicioEntrada + i];
            estados[inicioEntrada + i].elementosRestantes = nElem[inicioEntrada + i];
            estados[inicioEntrada + i].ativo = (numBlocos[inicioEntrada + i] > 0);
        }

        // Abrir fitas de saída (limpar arquivos existentes)
        for (int i = 0; i < FITAS; i++) {
            char nome[50];
            sprintf(nome, "fitas/fita%02d.bin", inicioSaida + i);
            estados[inicioSaida + i].arquivo = fopen(nome, "wb");
            numBlocos[inicioSaida + i] = 0;
            nElem[inicioSaida + i] = 0;
        }

        // Contadores para nova passagem
        int blocosProduzidos = 0;
        int elementosProduzidos = 0;
        int blocoAtual = 0;

        while (1) {
            Registro memoria[FITAS];
            short fitasAtivas[FITAS] = {0};
            int registrosRestantesBloco[FITAS] = {0};  // registros que ainda faltam do bloco atual
            int fitasComDados = 0;

            // Carregar primeiro registro do primeiro bloco de cada fita
            for (int i = 0; i < FITAS; i++) {
                int idx = inicioEntrada + i;

                if (estados[idx].ativo && estados[idx].blocosRestantes > 0) {
                    // Estimar tamanho médio do bloco da fita
                    int tamanhoBloco = numBlocos[idx] > 0 ? nElem[idx] / numBlocos[idx] : 0;
                    registrosRestantesBloco[i] = tamanhoBloco;
                    printf("Registros Restantes %d: %d\n", i , registrosRestantesBloco[i]);
                    if (tamanhoBloco > 0 &&
                        fread(&memoria[i], sizeof(Registro), 1, estados[idx].arquivo) == 1) {
                        fitasAtivas[i] = 1;
                        fitasComDados++;
                        registrosRestantesBloco[i]--;  // já lemos um
                    } else {
                        estados[idx].ativo = 0;
                        fitasAtivas[i] = 0;
                        registrosRestantesBloco[i] = 0;
                    }
                } else {
                    fitasAtivas[i] = 0;
                    registrosRestantesBloco[i] = 0;
                }
            }

            if (fitasComDados == 0) break;  // acabou todos os blocos

            // Fita de saída onde esse bloco será armazenado
            int fitaSaidaBloco = inicioSaida + (blocoAtual % FITAS);
            blocoAtual++;

            int elementosBloco = 0;

            // Começa a intercalação de um conjunto de blocos (um de cada fita)
            while (fitasComDados > 0) {
                int idxMenor = menorRegistroAtivo(memoria, fitasAtivas, FITAS);
                if (idxMenor == -1) break;

                fwrite(&memoria[idxMenor], sizeof(Registro), 1, estados[fitaSaidaBloco].arquivo);
                elementosBloco++;
                elementosProduzidos++;

                int fitaIdx = inicioEntrada + idxMenor;

                if (registrosRestantesBloco[idxMenor] == 0 ||
                    fread(&memoria[idxMenor], sizeof(Registro), 1, estados[fitaIdx].arquivo) != 1) {
                    
                    // Acabou o bloco ou não há mais o que ler da fita
                    fitasAtivas[idxMenor] = 0;
                    fitasComDados--;
                    estados[fitaIdx].blocosRestantes--;
                    registrosRestantesBloco[idxMenor] = 0;
                    memset(&memoria[idxMenor], 0, sizeof(Registro));
                } else {
                    registrosRestantesBloco[idxMenor]--;
                }
            }

            // Finaliza e contabiliza o bloco criado na fita de saída
            numBlocos[fitaSaidaBloco]++;
            nElem[fitaSaidaBloco] += elementosBloco;
            blocosProduzidos++;
        }

        // Fechar todas as fitas
        for (int i = 0; i < TOTAL_FITAS; i++) {
            if (estados[i].arquivo) {
                fclose(estados[i].arquivo);
                estados[i].arquivo = NULL;
            }
        }

        // Atualizar contadores para próxima passagem
        for (int i = 0; i < FITAS; i++) {
            numBlocos[inicioEntrada + i] = 0;
            nElem[inicioEntrada + i] = 0;
        }

        // Preparar próxima iteração
        inicioEntrada = inicioSaida;
        inicioSaida = (inicioSaida + FITAS) % TOTAL_FITAS;
        
        // Contar fitas com dados
        numFitasComDados = 0;
        for (int i = 0; i < FITAS; i++) {
            if (numBlocos[inicioEntrada + i] > 0) {
                numFitasComDados++;
                ultimaFitaComDados = inicioEntrada + i;
            }
        }

        printf("Fitas com dados: %d\n", numFitasComDados);
    } while (numFitasComDados > 1);

    // Gerar arquivo final
    if (numFitasComDados == 1) {
        char nomeFinal[TAM_NOME];
        sprintf(nomeFinal, "fitas/fita%02d.bin", ultimaFitaComDados);
        printf("Arquivo ordenado: %s\n", nomeFinal);
        bintxt(nomeFinal, "resultado.txt");
    }
}
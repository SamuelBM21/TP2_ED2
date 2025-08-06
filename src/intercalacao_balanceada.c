#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "../include/area.h"
#include "../include/intercalacao_balanceada.h"  
#include "../include/conversor.h"  

/*
Nome: numFitasPreenchidas  
Função: Conta quantas fitas possuem elementos válidos a partir de um índice e retorna o índice da última fita com dados.  
Entrada: Vetor de elementos, índice inicial e ponteiro para armazenar o índice da última fita com dados.  
Saída: Número de fitas preenchidas a partir do índice dado.  
*/

int numFitasPreenchidas(int elementos[], int inicio, int *ultimaFita) {
    int count = 0;
    *ultimaFita = -1;

    for (int i = 0; i < FITAS; i++) {
        if (elementos[inicio + i] > 0) {            //Se existem elementos
            count++;                                //Incrementa o contador
            *ultimaFita = inicio + i;               //Atualiza a última fita válida
        }
    }
    return count;
}

/*
Nome: menorRegistroAtivo  
Função: Retorna o índice do menor registro entre os ativos no vetor.  
Entrada: Vetor de registros, vetor de flags de atividade e número de fitas.  
Saída: Índice do menor registro entre os ativos, ou -1 se nenhum ativo.  
*/

int menorRegistroAtivo(Registro registros[], short ativos[], int numFitas) {
    int menor = -1;
    for (int i = 0; i < numFitas; i++) {
        if (ativos[i]) {                                                                //Se a fita na posição está ativa
            if (menor == -1 || registros[i].nota < registros[menor].nota) {             //Se for o primeiro ou menor que o atual menor
                menor = i;                                                              //Atualiza o menor valor encontrado
            }
        }
    }
    return menor;
}

/*
Nome: quickSort  
Função: Ordena um vetor de registros em ordem crescente de nota usando o algoritmo QuickSort.  
Entrada: Vetor de registros, índices inicial e final da parte a ser ordenada.  
Saída: --  
*/

void quickSort(Registro *v, int esquerda, int direita) {
    if (esquerda >= direita) return;                                //Se a esquerda for maior que a direita
    Registro pivo = v[(esquerda + direita) / 2];                    //Define qual é o pivô 
    int i = esquerda, j = direita;
    while (i <= j) { 
        while (v[i].nota < pivo.nota) i++;
        while (v[j].nota > pivo.nota) j--;
        if (i <= j) {
            Registro tmp = v[i];
            v[i] = v[j];
            v[j] = tmp;                                             //Troca os elementos
            i++;
            j--;
        }
    }

    if (esquerda < j) quickSort(v, esquerda, j);                    //Chama o quickSort para a partição da esquerda
    if (i < direita) quickSort(v, i, direita);                      //Chama o quickSort para a partição da direita
}

/*
Nome: gerarBlocosOrdenados  
Função: Lê registros do arquivo de entrada, ordena blocos de tamanho fixo e os distribui ciclicamente entre as fitas de entrada.  
Entrada: Nome do arquivo de entrada, número total de registros, vetores de blocos e elementos por fita.  
Saída: -- (gera os arquivos binários das fitas com os blocos ordenados).  
*/

void gerarBlocosOrdenadosQS(const char *inputFile, int totalRegs, int numBlocos[], int nElem[]) {
    FILE *entrada = fopen(inputFile, "rb");                                             //Abre o arquivo
    if (!entrada) {                                                                     //Se não conseguiu abrir
        fprintf(stderr, "Erro ao abrir '%s': %s\n", inputFile, strerror(errno));
        exit(EXIT_FAILURE);
    }

    Registro memoria[TAM_MEM];                                                          //Cria a memória, tamanho 20
    int lidos = 0, fita = 0;

    for(int i = 0; i < TOTAL_FITAS; i++) {                                              //Zera todas as posições dos vetores de Blocos e Elementos (por garantia)
        numBlocos[i] = 0;
        nElem[i] = 0;
    }

    //Enquanto o número de valores lidos for menor que o total desejado
    while (lidos < totalRegs) {
        int cnt = 0;                                                                            //Contador

        while (cnt < TAM_MEM && fread(&memoria[cnt], sizeof(Registro), 1, entrada) == 1) {      //Enquanto o número de registros lidos é menor que o tamanho da memória
            cnt++;
            lidos++;
        }

        quickSort(memoria, 0, cnt - 1);                                                         //Ordena o bloco lido utilizando o quicksort

        char nomeFita[TAM_NOME];
        sprintf(nomeFita, "fitas/fita%02d.bin", fita);                                          //Cria o nome da fita para escrever o bloco

        FILE *f = fopen(nomeFita, "ab");                                                        //Abre a fita
        if (!f) {                                                                               //Se não conseguiu abrir
            fprintf(stderr, "Erro ao criar '%s': %s\n", nomeFita, strerror(errno));
            exit(EXIT_FAILURE);
        }

        fwrite(memoria, sizeof(Registro), cnt, f);                                              //Escreve bloco ordenado na fita
        fclose(f);                                                                              //Fecha a fita
        
        numBlocos[fita]++;                                                                      
        nElem[fita] += cnt;
        fita = (fita + 1) % FITAS;                                                              //Rotaciona para próxima fita
    }
    fclose(entrada);                                                                            //Fecha o arquivo de entrada
}

/*
Nome: intercalacaoBalanceada  
Função: Realiza a intercalação balanceada dos blocos ordenados das fitas até gerar um arquivo totalmente ordenado.  
Entrada: Nome do arquivo de entrada, número total de registros e flag para saber se os registros devem ser imprimidos.  
Saída: Gera o arquivo de saída ordenado 'resultado.txt'.  
*/

void intercalacaoBalanceadaQS(const char *inputFile, int totalRegs, char *flag) {
    system("rm -rf fitas");                                                             // Remove diretório antigo
    system("mkdir -p fitas");                                                           // Cria novo diretório de fitas
    
    int numBlocos[TOTAL_FITAS] = {0};
    int nElem[TOTAL_FITAS] = {0};
    int inicioEntrada = 0;
    int inicioSaida = FITAS;
    int ultimaFitaComDados = -1;
    int passagem = 0;
    int numFitasComDados;

    gerarBlocosOrdenadosQS(inputFile, totalRegs, numBlocos, nElem);                       //Gera os blocos ordenados nas fitas

    FitaEstado estados[TOTAL_FITAS];
    memset(estados, 0, sizeof(estados));                                                //Zera estrutura de controle das fitas

    do {
        printf("\n=== Passagem %d ===\n", passagem++);
        
        //Abre as fitas de entrada para leitura
        for (int i = 0; i < FITAS; i++) {
            char nome[50];
            sprintf(nome, "fitas/fita%02d.bin", inicioEntrada + i);
            estados[inicioEntrada + i].arquivo = fopen(nome, "rb");                     //Abre o arquivo na posição correspondente da struct
            estados[inicioEntrada + i].blocosRestantes = numBlocos[inicioEntrada + i];  
            estados[inicioEntrada + i].elementosRestantes = nElem[inicioEntrada + i];
            estados[inicioEntrada + i].ativo = (numBlocos[inicioEntrada + i] > 0);
        }

        //Abre fitas de saída (modo escrita) e zera contadores
        for (int i = 0; i < FITAS; i++) {
            char nome[50];
            sprintf(nome, "fitas/fita%02d.bin", inicioSaida + i);
            estados[inicioSaida + i].arquivo = fopen(nome, "wb");
            numBlocos[inicioSaida + i] = 0;
            nElem[inicioSaida + i] = 0;
        }

        int elementosProduzidos = 0;
        int blocoAtual = 0;
        int tamanhoBloco = 0;

        while (1) {
            Registro memoria[FITAS];
            short fitasAtivas[FITAS] = {0};
            int registrosRestantesBloco[FITAS] = {0};
            int fitasComDados = 0;

            //Carrega o primeiro registro de cada fita com bloco disponível
            for (int i = 0; i < FITAS; i++) {
                int idx = inicioEntrada + i;
                if (estados[idx].ativo && estados[idx].blocosRestantes > 0) {           //Se o estado é ativo e existem blocos restantes
                    
                    //Se o número de elementos da fita é divisível pela quantidade esperada na passagem e não está no último bloco
                    if((nElem[idx]%((int)pow(20,passagem))) == 0 || estados[idx].blocosRestantes != 1){  
                        tamanhoBloco = (int)pow(20,passagem);
                    }
                    else{
                        tamanhoBloco = nElem[idx]%(int)pow(20,passagem);                //Se é o último, pega o resto dos itens
                    }
                    
                    registrosRestantesBloco[i] = tamanhoBloco;                          //Atualiza (inicia) a quantidade no de registros restantes por bloco

                    //Se o tamanho do bloco é maior que 0 e conseguiu ler um registro para a memória
                    if (tamanhoBloco > 0 && fread(&memoria[i], sizeof(Registro), 1, estados[idx].arquivo) == 1) {
                        fitasAtivas[i] = 1;                                             //"Ativa" a fita no vetor 
                        fitasComDados++;                                                //Aumenta o número de fitas com dados
                        registrosRestantesBloco[i]--;                                   //Por ter lido, diminui o número de registros restantes 
                    } else {
                        estados[idx].ativo = 0;                                         //Desativa a fita no vetor de estados
                        fitasAtivas[i] = 0;                                             //"Desativa" a fita no vetor 
                        registrosRestantesBloco[i] = 0;                                 //Acabaram os registros restantes
                    }
                }
            }

            if (fitasComDados == 0) break; // Se nenhuma fita tem dados, fim da intercalação

            int fitaSaidaBloco = inicioSaida + (blocoAtual % FITAS);                    //Calcula qual a fita de saída do em que o bloco será escrito
            blocoAtual++;

            int elementosBloco = 0;

            //Enquanto ainda houver registros nos blocos ativos
            while (fitasComDados > 0) {
                int idxMenor = menorRegistroAtivo(memoria, fitasAtivas, FITAS);         //Encontra o índice do registro com a menor nota
                if (idxMenor == -1) break;                                              //Se não encontrou, sai do loop

                fwrite(&memoria[idxMenor], sizeof(Registro), 1, estados[fitaSaidaBloco].arquivo);       //Escreve o menor na fita de saída escolhida
                elementosBloco++;                                                                       //Aumenta o número de elementos do bloco de saída
                elementosProduzidos++;                                                                  //Aumenta o número de elementos produzidos

                int fitaIdx = inicioEntrada + idxMenor;                                                 //Encontra a fita da qual o menor registro foi retirado
                
                //Se não existem mais registros restantes no bloco ou não conseguiu ler
                if (registrosRestantesBloco[idxMenor] == 0 || fread(&memoria[idxMenor], sizeof(Registro), 1, estados[fitaIdx].arquivo) != 1) {
                    fitasAtivas[idxMenor] = 0;                                          //Desativa a fita no vetor 
                    fitasComDados--;                                                    //Diminui o número de fitas com dados
                    estados[fitaIdx].blocosRestantes--;                                 //Diminui o número de blocos restantes na fita de entrada
                    registrosRestantesBloco[idxMenor] = 0;                              //Zera o número de registros restantes no bloco
                    memset(&memoria[idxMenor], 0, sizeof(Registro));                    //Zera a posição do index do menor na memória 
                } else {
                    registrosRestantesBloco[idxMenor]--;                                //Diminui o número de registros restantes no bloco
                }
            }

            numBlocos[fitaSaidaBloco]++;                                                //Incrementa o número de blocos na fita de saída
            nElem[fitaSaidaBloco] += elementosBloco;                                    //Incrementa o número de elementos na fita de saída
        }

        //Fecha todas as fitas dentro da struct de estados
        for (int i = 0; i < TOTAL_FITAS; i++) {
            if (estados[i].arquivo) {
                fclose(estados[i].arquivo);
                estados[i].arquivo = NULL;                                              
            }
        }

        // Zera contadores antigos
        for (int i = 0; i < FITAS; i++) {
            numBlocos[inicioEntrada + i] = 0;
            nElem[inicioEntrada + i] = 0;
        }

        // Alterna entrada/saída para próxima passagem
        inicioEntrada = inicioSaida;
        inicioSaida = (inicioSaida + FITAS) % TOTAL_FITAS;
        
        numFitasComDados = 0;                                                           //Reseta o número de fitas com dados

        for (int i = 0; i < FITAS; i++) {
            if (numBlocos[inicioEntrada + i] > 0) {                                     //Se o número de blocos da nova entrada for maior que 0
                numFitasComDados++;                                                     //Aumenta o número de fitas com dados
                ultimaFitaComDados = inicioEntrada + i;                                 //Atualiza o index da última fita com dados
            }
        }

        printf("Fitas com dados: %d\n", numFitasComDados);                              //Imprime quantas fitas têm dados
    } while (numFitasComDados > 1);

    // Quando o processo termina e só sobra uma única fita, converte para .txt
    if (numFitasComDados == 1) {
        char nomeFinal[TAM_NOME];                                                       
        sprintf(nomeFinal, "fitas/fita%02d.bin", ultimaFitaComDados);                   //Dá valor à variável com nome da fita que tem os dados ordenados
        printf("Arquivo ordenado: %s\n", nomeFinal);
        bintxt(nomeFinal, "resultado.txt");                                             //Realiza a conversão
    }
}







void heapify(HeapElem heap[], int n, int i) {
    int menor = i;
    int esq = 2 * i + 1;
    int dir = 2 * i + 2;

    // Considera congelados como infinitamente maiores
    if (esq < n && 
        (!heap[esq].congelado && 
        (heap[menor].congelado || heap[esq].reg.chave < heap[menor].reg.chave)))
        menor = esq;

    if (dir < n && 
        (!heap[dir].congelado &&
        (heap[menor].congelado || heap[dir].reg.chave < heap[menor].reg.chave)))
        menor = dir;

    if (menor != i) {
        HeapElem temp = heap[i];
        heap[i] = heap[menor];
        heap[menor] = temp;
        heapify(heap, n, menor);
    }
}

void construirHeapMin(HeapElem heap[], int n) {
    for (int i = n / 2 - 1; i >= 0; i--) {
        heapify(heap, n, i);
    }
}

void gerarBlocosOrdenadosSS(const char *inputFile, int totalRegs, int numBlocos[], int nElem[]) {
    system("rm -rf fitas");
    system("mkdir -p fitas");

    FILE *entrada = fopen(inputFile, "rb");
    if (!entrada) {
        fprintf(stderr, "Erro ao abrir '%s': %s\n", inputFile, strerror(errno));
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < TOTAL_FITAS; i++) {
        numBlocos[i] = 0;
        nElem[i] = 0;
    }

    HeapElem heap[TAM_MEM];
    int lidos = 0, fita = 0;

    // Carrega o heap inicialmente
    int tamanhoHeap = 0;
    while (tamanhoHeap < TAM_MEM && fread(&heap[tamanhoHeap].reg, sizeof(Registro), 1, entrada) == 1) {
        heap[tamanhoHeap].congelado = 0;
        lidos++;
        tamanhoHeap++;
    }

    construirHeapMin(heap, tamanhoHeap);

    Registro ultimoSaido = {.chave = -1};
    int blocoTam = 0;

    char nomeFita[TAM_NOME];
    sprintf(nomeFita, "fitas/fita%02d.bin", fita);
    FILE *fitaAtual = fopen(nomeFita, "ab");
    if (!fitaAtual) {
        fprintf(stderr, "Erro ao criar '%s': %s\n", nomeFita, strerror(errno));
        exit(EXIT_FAILURE);
    }

    while (tamanhoHeap > 0) {
        HeapElem menor = heap[0];

        heap[0].reg.fimDeBloco = 0;  // Marca como NÃO sendo fim de bloco (padrão)
        fwrite(&menor.reg, sizeof(Registro), 1, fitaAtual);
        ultimoSaido = menor.reg;
        blocoTam++;

        Registro novo;
        if (lidos < totalRegs && fread(&novo, sizeof(Registro), 1, entrada) == 1) {
            lidos++;
            heap[0].reg = novo;
            heap[0].congelado = (novo.chave < ultimoSaido.chave);
        } else {
            // Sem novos registros, remove do heap
            heap[0] = heap[tamanhoHeap - 1];
            tamanhoHeap--;
        }

        construirHeapMin(heap, tamanhoHeap);

        // Verifica se todos estão congelados
        int todosCongelados = 1;
        for (int i = 0; i < tamanhoHeap; i++) {
            if (!heap[i].congelado) {
                todosCongelados = 0;
                break;
            }
        }

        if (todosCongelados) {
            // Marca o último registro gravado como fim de bloco
            fseek(fitaAtual, -sizeof(Registro), SEEK_CUR); // Volta para o último registro
            ultimoSaido.fimDeBloco = 1;
            fwrite(&ultimoSaido, sizeof(Registro), 1, fitaAtual);
            fclose(fitaAtual);
            numBlocos[fita]++;
            nElem[fita] += blocoTam;
            fita = (fita + 1) % FITAS;

            // Descongela todos os registros restantes
            for (int i = 0; i < tamanhoHeap; i++) {
                heap[i].congelado = 0;
            }

            construirHeapMin(heap, tamanhoHeap);

            // Recomeça novo bloco
            blocoTam = 0;
            sprintf(nomeFita, "fitas/fita%02d.bin", fita);
            fitaAtual = fopen(nomeFita, "ab");
            if (!fitaAtual) {
                fprintf(stderr, "Erro ao criar '%s': %s\n", nomeFita, strerror(errno));
                exit(EXIT_FAILURE);
            }
        }
    }

    // Finaliza último bloco
    if (blocoTam > 0) {
        fseek(fitaAtual, -sizeof(Registro), SEEK_CUR);
        ultimoSaido.fimDeBloco = 1;
        fwrite(&ultimoSaido, sizeof(Registro), 1, fitaAtual);

        fclose(fitaAtual);
        numBlocos[fita]++;
        nElem[fita] += blocoTam;
    }

    fclose(entrada);
}

void gerarResumoFitas(int numFitas) {
    char nomeFita[100];
    char nomeSaida[100];
    FILE *fita, *saida;
    Registro reg;

    for (int i = 0; i < numFitas; i++) {
        sprintf(nomeFita, "fitas/fita%02d.bin", i);
        sprintf(nomeSaida, "fita%02d.txt", i);

        fita = fopen(nomeFita, "rb");
        saida = fopen(nomeSaida, "w");

        if (!fita || !saida) {
            printf("Erro ao abrir arquivos da fita %d\n", i);
            continue;
        }

        fprintf(saida, "FITA %d\n", i);

        while (fread(&reg, sizeof(Registro), 1, fita) == 1) {
            fprintf(saida, "%ld, ", reg.chave);
            if (reg.fimDeBloco)
                fprintf(saida, "\n\n\n"); // quebra de linha ao fim do bloco
        }

        fclose(fita);
        fclose(saida);
    }

    printf("Arquivos .txt gerados com sucesso.\n");
}
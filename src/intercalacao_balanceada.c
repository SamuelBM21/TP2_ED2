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

int menorRegistroAtivo(Registro registros[], short ativos[], int numFitas, long *compCount) {
    int menor = -1;
    for (int i = 0; i < numFitas; i++) {
        if (ativos[i]) {                                                                //Se a fita na posição está ativa
            (*compCount)++;
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

void quickSort(Registro *v, int esquerda, int direita, long *compCount) {
    if (esquerda >= direita) return;                                //Se a esquerda for maior que a direita
    Registro pivo = v[(esquerda + direita) / 2];                    //Define qual é o pivô 
    int i = esquerda, j = direita;
    while (i <= j) { 
        while (++(*compCount) && v[i].nota < pivo.nota) i++;
        while (++(*compCount) && v[j].nota > pivo.nota) j--;
        if (i <= j) {
            Registro tmp = v[i];
            v[i] = v[j];
            v[j] = tmp;                                             //Troca os elementos
            i++;
            j--;
        }
    }

    if (esquerda < j) quickSort(v, esquerda, j, compCount);                    //Chama o quickSort para a partição da esquerda
    if (i < direita) quickSort(v, i, direita, compCount);                      //Chama o quickSort para a partição da direita
}

/*
Nome: gerarBlocosOrdenados  
Função: Lê registros do arquivo de entrada, ordena blocos de tamanho fixo e os distribui ciclicamente entre as fitas de entrada.  
Entrada: Nome do arquivo de entrada, número total de registros, vetores de blocos e elementos por fita.  
Saída: -- (gera os arquivos binários das fitas com os blocos ordenados).  
*/

void gerarBlocosOrdenadosQS(const char *inputFile, int totalRegs, int numBlocos[], int nElem[], long *compCount, long *readCount, long *writeCount) {
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

        while (cnt < TAM_MEM && fread(&memoria[cnt], sizeof(Registro), 1, entrada) == 1 && lidos<totalRegs) {      //Enquanto o número de registros lidos é menor que o tamanho da memória
            cnt++;
            lidos++;
            (*readCount)++;
        }

        quickSort(memoria, 0, cnt - 1, compCount);                                                         //Ordena o bloco lido utilizando o quicksort

        char nomeFita[TAM_NOME];
        sprintf(nomeFita, "fitas/fita%02d.bin", fita);                                          //Cria o nome da fita para escrever o bloco

        FILE *f = fopen(nomeFita, "ab");                                                        //Abre a fita
        if (!f) {                                                                               //Se não conseguiu abrir
            fprintf(stderr, "Erro ao criar '%s': %s\n", nomeFita, strerror(errno));
            exit(EXIT_FAILURE);
        }

        fwrite(memoria, sizeof(Registro), cnt, f);                                              //Escreve bloco ordenado na fita
        fclose(f);
        (*writeCount)++;                                                                              //Fecha a fita
        
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

void intercalacaoBalanceadaQS(const char *inputFile, char *outFile, int totalRegs, long *compCount, long *readCount, long *writeCount) {
    system("rm -rf fitas");                                                             // Remove diretório antigo
    system("mkdir -p fitas");                                                           // Cria novo diretório de fitas
    
    int numBlocos[TOTAL_FITAS] = {0};
    int nElem[TOTAL_FITAS] = {0};
    int inicioEntrada = 0;
    int inicioSaida = FITAS;
    int ultimaFitaComDados = -1;
    int passagem = 0;
    int numFitasComDados;

    gerarBlocosOrdenadosQS(inputFile, totalRegs, numBlocos, nElem, compCount, readCount, writeCount);                       //Gera os blocos ordenados nas fitas

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
                        (*readCount)++;
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
                int idxMenor = menorRegistroAtivo(memoria, fitasAtivas, FITAS, compCount);         //Encontra o índice do registro com a menor nota
                if (idxMenor == -1) break;                                              //Se não encontrou, sai do loop
                
                (*writeCount)++;
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
                    (*readCount)++;                                                     //A leitura só é bem sucedida se cair no else
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
        strcpy(outFile,nomeFinal);
        bintxt(nomeFinal, "resultado_ibqs.txt",totalRegs);                                             //Realiza a conversão
        printf("Arquivo com os registros ordenados: 'resultado_ibqs.txt' \n");
    }
}

/*
Nome: heapify  
Função: Reorganiza uma estrutura de heap mínimo a partir de um índice específico, mantendo a propriedade de heap.  
Entrada: Vetor de elementos do heap, tamanho total do heap e índice a partir do qual a reorganização será feita.  
Saída: Heap reorganizado com o menor elemento na raiz.  
*/

void heapify(HeapElem heap[], int n, int i, long *compCount) {
    int menor = i;
    int esq = 2 * i + 1;
    int dir = 2 * i + 2;

    // Se a esquerda é menor que o tamanho, o reg da esquerda não está congelado e o menor não está congelado ou é maior que o da esquerda
    if (esq < n && (!heap[esq].congelado && (heap[menor].congelado || heap[esq].reg.nota < heap[menor].reg.nota))) {
        menor = esq;
        (*compCount)++;
    }
    // Se a direita é menor que o tamanho, o reg da direita não está congelado e o menor não está congelado ou é maior que o da direita
    if (dir < n && (!heap[dir].congelado && (heap[menor].congelado || heap[dir].reg.nota < heap[menor].reg.nota))) {
        menor = dir;
        (*compCount)++;
    }
    //Se trocou o menor, realiza a troca e refaz o heap
    if (menor != i) {
        HeapElem temp = heap[i];
        heap[i] = heap[menor];
        heap[menor] = temp;
        heapify(heap, n, menor, compCount);
    }
}

/*
Nome: construirHeapMin  
Função: Constrói um heap mínimo a partir de um vetor de elementos, aplicando heapify de forma bottom-up.  
Entrada: Vetor de elementos do heap e seu tamanho.  
Saída: Vetor reorganizado em forma de heap mínimo.  
*/
void construirHeapMin(HeapElem heap[], int n, long *compCount) {
    for (int i = n / 2 - 1; i >= 0; i--) {
        heapify(heap, n, i, compCount);
    }
}

/*
Nome: gerarBlocosOrdenadosSS  
Função: Gera os blocos ordenados utilizando a estratégia de substituição natural com heap.  
Entrada: Nome do arquivo de entrada, número total de registros, vetores para armazenar o número de blocos e elementos por fita.  
Saída: Blocos ordenados são distribuídos entre as fitas, marcando fim de bloco quando necessário.  
*/

void gerarBlocosOrdenadosSS(const char *inputFile, int totalRegs, int numBlocos[], int nElem[], long *compCount, long *readCount, long *writeCount) {
    //Remove a pasta antiga de fitas e a cria novamente
    system("rm -rf fitas");
    system("mkdir -p fitas");
    
    //Abre o arquivo da entrada
    FILE *entrada = fopen(inputFile, "rb");
    if (!entrada) {
        fprintf(stderr, "Erro ao abrir '%s': %s\n", inputFile, strerror(errno));
        exit(EXIT_FAILURE);
    }

    //Inicia com zero os vetores de número de elementos e número de blocos
    for (int i = 0; i < TOTAL_FITAS; i++) {
        numBlocos[i] = 0;
        nElem[i] = 0;
    }

    HeapElem heap[TAM_MEM];
    int lidos = 0, fita = 0;
    int tamanhoHeap = 0;

    //Enquanto a memoria(heap) não foi preenchida, foi possível ler um registro e o número de lidos não foi atingido
    while (tamanhoHeap < TAM_MEM && fread(&heap[tamanhoHeap].reg, sizeof(Registro), 1, entrada) == 1 && lidos < totalRegs) {
        (*readCount)++;
        heap[tamanhoHeap].congelado = 0;
        lidos++;
        tamanhoHeap++;
    }

    construirHeapMin(heap, tamanhoHeap, compCount);                                //Constrói o heap mínimo

    Registro ultimoSaido;
    int blocoTam = 0;

    char nomeFita[TAM_NOME];
    FILE *fitaAtual = NULL;

    //Enquanto o heap tem registros
    while (tamanhoHeap > 0) {
        if (fitaAtual == NULL) {
            sprintf(nomeFita, "fitas/fita%02d.bin", fita);
            fitaAtual = fopen(nomeFita, "r+b");                                                 //Abre a fita para leitura e escrita
            if (fitaAtual == NULL) {                                                            //Se falhou, o arquivo não existe. Cria ele com "w+b".
                fitaAtual = fopen(nomeFita, "w+b");
                if (!fitaAtual) {
                    fprintf(stderr, "Erro ao criar '%s': %s\n", nomeFita, strerror(errno));
                    exit(EXIT_FAILURE);
                }
            }
            fseek(fitaAtual, 0, SEEK_END);                                                      //Garante que a escrita começará no final do arquivo, preservando blocos anteriores.
            blocoTam = 0;
        }

        HeapElem menor = heap[0];
        Registro temp = menor.reg;
        temp.fimDeBloco = 0;
        
        fwrite(&temp, sizeof(Registro), 1, fitaAtual);                                         //Escreve o menor registro na fita atual
        (*writeCount)++;
        blocoTam++;                                                                             //Aumenta o tamanho do bloco
        ultimoSaido = temp;                                                                     //O último que saiu se torna esse menor

        Registro novo;
        //Se ainda não foram lidos todos os pedidos e ainda é possível ler 
        (*readCount)++;
        if (lidos < totalRegs && fread(&novo, sizeof(Registro), 1, entrada) == 1) {
            lidos++;
            heap[0].reg = novo;                                                                 //Insere o novo na posição antiga do menor
            heap[0].congelado = (novo.nota < ultimoSaido.nota);                                 //Se a nota do novo for menor que a do último, congela ele
            (*compCount)++;
        } else {                                                                                //Se não conseguiu ler, diminui o tamanho do heap e coloca o último valor no início 
            heap[0] = heap[tamanhoHeap - 1];
            tamanhoHeap--;
        }

        //Se ainda tiverem registros no heap
        if (tamanhoHeap > 0) {
            heapify(heap, tamanhoHeap, 0, compCount);                                                      //Refaz o heap
        }

        int todosCongelados = 1;
        if (tamanhoHeap > 0) {                                                                  //Se o heap não está vazio
            //Percorre o heap verificando se algum não está congelado
            for (int i = 0; i < tamanhoHeap; i++) {
                if (!heap[i].congelado) {                                               
                    todosCongelados = 0;
                    break;
                }
            }
        }
        
        if (todosCongelados) {                                                                  //Se todos os registros foram marcados
            if (blocoTam > 0) {                                                                 //Garante que algo foi escrito antes de marcar
                fseek(fitaAtual, -sizeof(Registro), SEEK_CUR);                                  //Encontra o último registro e o marca como fim do bloco
                ultimoSaido.fimDeBloco = 1;
                fwrite(&ultimoSaido, sizeof(Registro), 1, fitaAtual);
                (*writeCount)++;
                fflush(fitaAtual);
            }
            
            //Incrementa o número de elementos e blocos de tal fita
            numBlocos[fita]++;
            nElem[fita] += blocoTam;

            fclose(fitaAtual);                                                                  //Fecha a fita
            fitaAtual = NULL;

            fita = (fita + 1) % FITAS;

            for (int i = 0; i < tamanhoHeap; i++) {
                heap[i].congelado = 0;                                                          //Desmarca todos os registros
            }

            if (tamanhoHeap > 0) {
                construirHeapMin(heap, tamanhoHeap, compCount);                                            //Constrói o heap novamente se o tamanho for maior que 0
            }
        }
    }

    //Se o tamanho do bloco é maior que zero e ainda tem a fita atual.
    if (blocoTam > 0 && fitaAtual) {
        fseek(fitaAtual, -sizeof(Registro), SEEK_CUR);                                          //Posiciona o ponteiro no final -1 registro
        ultimoSaido.fimDeBloco = 1;                                                             //Define tal registro como último do bloco
        fwrite(&ultimoSaido, sizeof(Registro), 1, fitaAtual);                                   //Escreve
        (*writeCount)++;
        numBlocos[fita]++;                                                                      //Incrementa o número de blocos e elementos
        nElem[fita] += blocoTam;
        fclose(fitaAtual);                                                                      //Fecha a fita
    }

    fclose(entrada);
}

/*
Nome: intercalacaoBalanceadaSS  
Função: Realiza a intercalação balanceada dos blocos ordenados das fitas até gerar um arquivo totalmente ordenado.  
Entrada: Nome do arquivo de entrada, número total de registros e flag para saber se os registros devem ser imprimidos.  
Saída: Gera o arquivo de saída ordenado.  
*/ 

void intercalacaoBalanceadaSS(const char *inputFile, char *outFile, int totalRegs, long *compCount, long *readCount, long *writeCount) {
    system("rm -rf fitas");
    system("mkdir -p fitas");

    int numBlocos[TOTAL_FITAS] = {0};
    int nElem[TOTAL_FITAS] = {0};

    int inicioEntrada = 0;
    int inicioSaida = FITAS;
    int ultimaFitaComDados = -1;
    int passagem = 0;
    int numFitasComDados;

    gerarBlocosOrdenadosSS(inputFile, totalRegs, numBlocos, nElem, compCount, readCount, writeCount);                                 //Gera os blocos ordenados por seleção por substituição

    FitaEstado estados[TOTAL_FITAS];                                                                //Cria a struct para guardar os estados das fitas
    memset(estados, 0, sizeof(estados));                                                            //Zera essa struct

    do {
        printf("\n=== Passagem %d ===\n", passagem++);

        //Inicia os valores da struct para as fitas de entrada, como o ponteiro pro arquivo, quantidade de elementos e blocos e se está ativo
        for (int i = 0; i < FITAS; i++) {
            char nome[50];
            sprintf(nome, "fitas/fita%02d.bin", inicioEntrada + i);
            estados[inicioEntrada + i].arquivo = fopen(nome, "rb");
            estados[inicioEntrada + i].blocosRestantes = numBlocos[inicioEntrada + i];
            estados[inicioEntrada + i].elementosRestantes = nElem[inicioEntrada + i];
            estados[inicioEntrada + i].ativo = (estados[inicioEntrada + i].arquivo != NULL && numBlocos[inicioEntrada + i] > 0);
        }

        //Inicia os valores da struct para as fitas de saída (abre o arquivo e zera os valores por garantia)
        for (int i = 0; i < FITAS; i++) {
            char nome[50];
            sprintf(nome, "fitas/fita%02d.bin", inicioSaida + i);
            estados[inicioSaida + i].arquivo = fopen(nome, "wb");
            numBlocos[inicioSaida + i] = 0;
            nElem[inicioSaida + i] = 0;
        }

        int blocoAtual = 0;
        while (1) {
            Registro memoria[FITAS];
            short fitasAtivas[FITAS] = {0};
            int fitasComDados = 0;

            //Percorre todas as fitas de entrada pegando o primeiro registro e colocando na memória
            for (int i = 0; i < FITAS; i++) {
                int idx = inicioEntrada + i;
                if (estados[idx].ativo && estados[idx].blocosRestantes > 0) {
                    (*readCount)++;
                    if (fread(&memoria[i], sizeof(Registro), 1, estados[idx].arquivo) == 1) {
                        fitasAtivas[i] = 1;
                        fitasComDados++;
                    } else {
                        estados[idx].ativo = 0;
                        fitasAtivas[i] = 0;
                    }
                }
            }

            //Se nenhuma fita tem dados, algo deu errado, sai do loop
            if (fitasComDados == 0) break;

            //Atualiza qual será a fita de saída
            int fitaSaidaBloco = inicioSaida + (blocoAtual % FITAS);
            blocoAtual++;
            
            int elementosBloco = 0;
            Registro ultimoEscrito;

            //Enquanto tiverem fitas com dados
            while (fitasComDados > 0) {
                int idxMenor = menorRegistroAtivo(memoria, fitasAtivas, FITAS, compCount);                                         //Encontra o menor registro na memória
                if (idxMenor == -1) break;

                Registro tempParaEscrever = memoria[idxMenor];                                                          //Cria uma variável temporária para escrita            
                ultimoEscrito = tempParaEscrever;
                tempParaEscrever.fimDeBloco = 0; // Limpa a flag antes de escrever

                fwrite(&tempParaEscrever, sizeof(Registro), 1, estados[fitaSaidaBloco].arquivo);                        //Escreve essa menor no arquivo
                (*writeCount)++;
                elementosBloco++;

                int idxEntrada = inicioEntrada + idxMenor;
                if (memoria[idxMenor].fimDeBloco == 1) {                                                                //Usa a flag original para controle do fim do bloco
                    //Se chegou ao fim do bloco, desativa a fita, retira um bloco do vetor da mesma e diminui o número de fitas com dados
                    fitasAtivas[idxMenor] = 0;
                    fitasComDados--;
                    estados[idxEntrada].blocosRestantes--;
                } else {                                                                                           //Se não chegou ao fim, tenta ler mais um registros
                    (*readCount)++;
                    if (fread(&memoria[idxMenor], sizeof(Registro), 1, estados[idxEntrada].arquivo) != 1) {
                        fitasAtivas[idxMenor] = 0;
                        fitasComDados--;
                        estados[idxEntrada].blocosRestantes--; 
                    }
                }
            }

            if (elementosBloco > 0) {
                // Marca o novo fim de bloco na fita de saída
                fseek(estados[fitaSaidaBloco].arquivo, -sizeof(Registro), SEEK_CUR);
                ultimoEscrito.fimDeBloco = 1;
                fwrite(&ultimoEscrito, sizeof(Registro), 1, estados[fitaSaidaBloco].arquivo);
                (*writeCount)++;
            }
            
            numBlocos[fitaSaidaBloco]++;
            nElem[fitaSaidaBloco] += elementosBloco;
        }

        //Fecha todas as fitas 
        for (int i = 0; i < TOTAL_FITAS; i++) {
            if (estados[i].arquivo) {
                fclose(estados[i].arquivo);
                estados[i].arquivo = NULL;
            }
        }

        //Zera todos os valores dos vetores
        for (int i = 0; i < FITAS; i++) {
            numBlocos[inicioEntrada + i] = 0;
            nElem[inicioEntrada + i] = 0;
        }
        
        //Realiza a troca entre a saída e a entrada
        inicioEntrada = inicioSaida;
        inicioSaida = (inicioSaida + FITAS) % TOTAL_FITAS;

        //Percorre as fitas encontrando quantas tem dados
        numFitasComDados = 0;
        for (int i = 0; i < FITAS; i++) {
            if (numBlocos[inicioEntrada + i] > 0) {
                numFitasComDados++;
                ultimaFitaComDados = inicioEntrada + i;
            }
        }

        printf("Fitas com dados: %d\n", numFitasComDados);
    } while (numFitasComDados > 1);                                                                     //Enquanto ainda tiver mais de uma fita com dados

    //Se ocorreu tudo corretamente, imprime o nome do arquivo ordenado e o converte para .txt
    if (numFitasComDados == 1) {
        char nomeFinal[TAM_NOME];
        sprintf(nomeFinal, "fitas/fita%02d.bin", ultimaFitaComDados);
        printf("Arquivo ordenado: %s\n", nomeFinal);
        strcpy(outFile,nomeFinal);
        bintxt(nomeFinal, "resultado_ibss.txt", totalRegs);
        printf("Arquivo com os registros ordenados: 'resultado_ibss.txt' \n");
    }
}

#include <stdio.h>
#include "../include/quickSortExt.h"
#include "../include/intercalacao_qsort.h"
#include "../include/conversor.h"
#include "../include/area.h"

int main(int argc, char* argv[]) {

    char in[20],out[20],conv[20];
    if (argc != 3 && argc != 4){                    
        printf("Não foi passado o número mínimo de parâmetros. \n");
        return 0;
    }

    int metodo = atoi(argv[1]);    
    int quantidade = atoi(argv[2]);  

    if(quantidade >  471705 ||quantidade < 0){
        printf("ERRO: A quantidade de Registros considerados deve ser positiva e menor que 471705.\n");
        return 0;
    }
    
    int situacao = atoi(argv[3]);
    char flag[10] = "";
    if (argc == 5){
        strcpy(flag,argv[4]);
        if(strcmp(flag,"[-P]") != 0){
            printf("Esta flag '%s' não existe. \n", flag);
            return 0;
        }

    }

    switch(situacao){
        case 1: //Ordenado Ascendentemente
            strcpy(in,"ordenadoasc.txt");
            strcpy(out,"saida.bin");
            break;
        case 2:
            strcpy(in,"ordenadodesc.txt");
            strcpy(out,"saida.bin");
            break;
        case 3:
            strcpy(in,"PROVAO.TXT");
            strcpy(out,"saida.bin");
            break;
    }

    txtbin(in,out,quantidade);

    FILE *li = fopen("saida.bin", "rb"); 
    FILE *ei = fopen("saida.bin", "r+b");
    FILE *les = fopen("saida.bin", "r+b");


    if (!li || !ei || !les) {
        perror("Erro ao abrir arquivos");
        return 1;
    }

    switch(metodo){
        case 1: 
            printf("Chamando intercalação balanceada com quantidade = %d\n", quantidade);
            intercalacaoBalanceada(quantidade);
            break;
        case 2: break;
        case 3:
            printf("Chamando quicksort com quantidade = %d\n", quantidade);
            QuicksortExterno(&li, &ei, &les, 0, quantidade);
            break;
    }
    strcpy(conv,"resultado.txt");
    bintxt(out,conv);
    return 0;
}
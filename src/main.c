#include <stdio.h>
#include "../include/quickSortExt.h"
#include "../include/intercalacao_balanceada.h"
#include "../include/conversor.h"
#include "../include/area.h"

int main(int argc, char* argv[]) {

    char in[20],out[20];
    if (argc != 4 && argc != 5){                                        //Se o número de parâmetros não estiver correto             
        printf("Não foi passado o número mínimo de parâmetros. \n");
        return 0;
    }

    int metodo = atoi(argv[1]);                                         //Converte argv na posição 1 para ter o número do método
    int quantidade = atoi(argv[2]);                                     //Converte argv na posição 2 para ter a quantidade

    if(quantidade >  471705 ||quantidade < 0){                          //Se a quantidade não está nos limites esperados
        printf("ERRO: A quantidade de Registros considerados deve ser positiva e menor que 471705.\n");
        return 0;
    }
    
    int situacao = atoi(argv[3]);                                       //Converte argv na posição 3 para ter a situação do arquivo
    char flag[10] = "";
    if (argc == 5){                                                     //Se tem 5 argumentos
        strcpy(flag,argv[4]);
        if(strcmp(flag,"[-P]") != 0){                                   //Se a flag colocada não for a esperada
            printf("Esta flag '%s' não existe. \n", flag);
            return 0;
        }

    }

    switch(situacao){
        case 1:                                                         //Ordenado Ascendentemente
            //Dá nome aos arquivos de saída e entrada
            strcpy(in,"ordenadoasc.txt");                   
            strcpy(out,"saida.bin");
            break;
        case 2:                                                         //Ordenado Descendentemente
            //Dá nome aos arquivos de saída e entrada
            strcpy(in,"ordenadodesc.txt");
            strcpy(out,"saida.bin");
            break;
        case 3:                                                         //Aleatório
            //Dá nome aos arquivos de saída e entrada
            strcpy(in,"PROVAO.TXT");
            strcpy(out,"saida.bin");
            break;
    }

    switch(metodo){
    case 1:                                                         //Intercalação Balanceada - QuickSort
            txtbin(in,out,quantidade);
            printf("Chamando intercalação balanceada com quantidade = %d\n", quantidade);
            intercalacaoBalanceadaQS("saida.bin",quantidade,flag);
            break;
        case 2:  
            txtbin(in,out,quantidade);                                                   //Intercalação Balanceada - Seleção por substituição
            printf("Chamando intercalação balanceada com quantidade = %d\n", quantidade);
            int numBlocos[TOTAL_FITAS] = {0};
            int nElem[TOTAL_FITAS] = {0};
            gerarBlocosOrdenadosSS("saida.bin",quantidade, numBlocos, nElem);
            gerarResumoFitas(20);
            break;
        case 3:                                                     //QuickSort Externo
            txtbin(in,out,quantidade);

            //Abre os arquivos (o mesmo) para leitura e escrita superior e inferior
            FILE *li = fopen("saida.bin", "rb"); 
            FILE *ei = fopen("saida.bin", "r+b");
            FILE *les = fopen("saida.bin", "r+b");
        
        
            if (!li || !ei || !les) {                               //Se não conseguiu abrir
                perror("Erro ao abrir arquivos");
                return 1;
            }

            printf("Chamando quicksort com quantidade = %d\n", quantidade);
            QuicksortExterno(&li, &ei, &les, 1, quantidade);        //Chama o quickSort
            break;
    }
    return 0;
}
#include <stdio.h>
#include "../include/quickSortExt.h"
#include "../include/area.h"

int main(int argc, char* argv[]) {

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

    // Simples chamada fictícia só para compilar
    FILE *li = fopen("PROVAO.TXT", "r"); 
    FILE *ei = fopen("PROVAO.TXT", "r+");
    FILE *les = fopen("PROVAO.TXT", "r+");


    if (!li || !ei || !les) {
        perror("Erro ao abrir arquivos");
        return 1;
    }

    switch(metodo){
        case 1: break;
        case 2: break;
        case 3:
            QuicksortExterno(&li, &ei, &les, 0, quantidade);
            break;
    }
    printf("Quicksort externo chamado!\n");
    return 0;
}
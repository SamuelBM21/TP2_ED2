#include <time.h>
#include <stdio.h>
#include "../include/quickSortExt.h"
#include "../include/intercalacao_balanceada.h"
#include "../include/conversor.h"
#include "../include/area.h"

int main(int argc, char* argv[]) {

    time_t inicio, fim, inicio_metodo, fim_metodo;
    inicio = clock();

    char filename[20], inName[20],outname[20];
    Registro reg;

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
            //Dá nome aos arquivos de saída e entrada da conversão
            strcpy(filename,"ORDENADOASC.txt");  
            strcpy(inName,"ORDENADOASC.bin");                 
            break;
        case 2:                                                         //Ordenado Descendentemente
            //Dá nome aos arquivos de saída e entrada da conversão
            strcpy(filename,"ORDENADODESC.txt");
            strcpy(inName,"ORDENADODESC.bin");
            break;
        case 3:                                                         //Aleatório
            //Dá nome aos arquivos de saída e entrada da conversão
            strcpy(filename,"PROVAO.TXT");
            strcpy(inName,"PROVAO.bin");
            break;
        default:
            printf("\nERRO!! Situação inválida, use um número de 1 a 3.\n\n");
            return 1;
            break;
    }

    if(txtbin(filename,inName, 471705))
        return 1;
    

    if(strcmp(flag, "") != 0){
        FILE *org = fopen(inName, "rb");
        printf("\nRegistros Originais: \n");
        for(int i = 0; i<quantidade; i++){                                           //Imprime como os registros estavam originalmente
            fread(&reg,sizeof(Registro),1,org);
            printf(
                "%08ld %05.1f %2s %-50s %-30s\n",  // Formato fixo
                reg.chave, 
                reg.nota, 
                reg.estado, 
                reg.cidade, 
                reg.curso
            );
        }
        fclose(org);
    }

    inicio_metodo = clock();
    long compCount = 0;  //Contador de comparações para os metodos

    switch(metodo){
    case 1:                                                         //Intercalação Balanceada - QuickSort
            printf("\n================= Intercalação balanceada com quantidade : %d =================\n", quantidade);
            intercalacaoBalanceadaQS(inName,outname,quantidade,&compCount);
            break;
        case 2:                                                     //Intercalação Balanceada - Seleção por substituição
            printf("\n================= Intercalação balanceada com quantidade : %d =================\n", quantidade);
            intercalacaoBalanceadaSS(inName,outname,quantidade,&compCount);
            break;
        case 3:                                                     //QuickSort Externo
            printf("\n================= QuickSort Externo com quantidade : %d =================\n", quantidade);
            //Abre os arquivos (o mesmo) para leitura e escrita superior e inferior
            FILE *li = fopen(inName, "rb"); 
            FILE *ei = fopen(inName, "r+b");
            FILE *les = fopen(inName, "r+b");
            strcpy(outname,inName);
        
        
            if (!li || !ei || !les) {                               //Se não conseguiu abrir
                perror("Erro ao abrir arquivos");
                if(li) fclose(li);
                if(ei) fclose(ei);
                if(les) fclose(les);
                return 1;
            }

            QuicksortExterno(&li, &ei, &les, 1, quantidade, &compCount);        //Chama o quickSort
            fclose(li);
            fclose(ei);
            fclose(les);

            printf("Arquivo ordenado: %s\n",inName);
            printf("Arquivo com os registros ordenados: 'resultado_qse.txt' \n");
            bintxt(inName, "resultado_qse.txt",quantidade);
            break;
        default:
            printf("\nERRO!! Método inválido, use um número de 1 a 3.\n\n");
            return 1;
            break;
    }

    fim_metodo = clock();

    if(strcmp(flag, "") != 0){                                                                      //Se tiver sido passada a flag                                                               
        FILE *ord = fopen(outname, "rb");
        printf("\nRegistros Ordenados: \n");
        for(int i = 0; i<quantidade; i++){                                                          //Imprime o resultado ordenado
            fread(&reg,sizeof(Registro),1,ord);
            printf(
                "%08ld %05.1f %2s %-50s %-30s\n",  // Formato fixo
                reg.chave, 
                reg.nota, 
                reg.estado, 
                reg.cidade, 
                reg.curso
            );
        }
        fclose(ord);
    }

    fim = clock();

    printf("\nNúmero de Comparações do Método: %ld",compCount);
    printf("\nTempo de Execução do Método: %lf\n",((double)(fim_metodo - inicio_metodo)) / CLOCKS_PER_SEC);
    printf("Tempo de Execução Total: %lf\n\n",((double)(fim - inicio)) / CLOCKS_PER_SEC);
    
    return 0;
}
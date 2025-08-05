#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/area.h"

/*
Nome: txtbin
Função: Converte um arquivo texto para um arquivo binário.
Entrada: Nome do arquivo texto, nome do arquivo binário e a quantidade de linhas a converter.  
Saída: 0 se tudo ocorreu certo e 1 caso contrário.
*/

int txtbin(char *in, char *out, int qtd) {
    FILE *txtFile, *binFile;
    int i = 0;
    char buffer[256];

    txtFile = fopen(in, "r");                           //Abre o arquivo de entrada
    if (!txtFile) {                                     //Se não conseguiu abrir
        perror("Erro ao abrir arquivo de texto");
        return 1;
    }

    binFile = fopen(out, "wb");                         //Abre o arquivo de saída
    if (!binFile) {                                     //Se não conseguiu abrir
        perror("Erro ao criar arquivo binário");
        fclose(txtFile);
        return 1;
    }

    //Enquanto existem linhas para ler e a quantidade lida é menor que a desejada
    while (fgets(buffer, sizeof(buffer), txtFile) != NULL && i < qtd) {         
        size_t len = strlen(buffer);

        //Substitui o último caractere caso ele não seja desejado
        if (len > 0 && buffer[len - 1] == '\n') buffer[--len] = '\0';       
        if (len > 0 && buffer[len - 1] == '\r') buffer[--len] = '\0';
        if (len < 98) continue;

        Registro reg = {0};
        char temp[20];

        //Lê a chave, converte e escreve no registro
        memcpy(temp, buffer, 8); 
        temp[8] = '\0';
        reg.chave = atol(temp);

        //Lê a nota, converte e escreve no registro
        memcpy(temp, buffer + 9, 5);
        temp[5] = '\0';
        reg.nota = atof(temp);

        //Lê o estado e escreve no registro
        memcpy(reg.estado, buffer + 15, 2); 
        reg.estado[2] = '\0';

        //Lê o cidade e escreve no registro
        memcpy(reg.cidade, buffer + 18, 50);
        reg.cidade[50] = '\0';

        //Lê o curso e escreve no registro
        memcpy(reg.curso, buffer + 69, 30);
        reg.curso[30] = '\0';

        //Escreve o registro completo no arquivo
        fwrite(&reg, sizeof(Registro), 1, binFile);
        i++;
    }

    //Fecha os arquivos
    fclose(txtFile);
    fclose(binFile);
    printf("Conversão concluída com sucesso!\n");
    return 0;
}

/*
Nome: bintxt
Função: Converte um arquivo binário para um arquivo texto.
Entrada: Nome do arquivo binário, nome do arquivo texto.  
Saída: 0 se tudo ocorreu certo e 1 caso contrário.
*/
int bintxt(char *in, char *out) {
    FILE *binFile, *txtFile;
    Registro reg;

    binFile = fopen(in, "rb");                                      //Abre o arquivo de entrada
    if (binFile == NULL) {                                          //Se não conseguiu abrir  
        perror("Erro ao abrir arquivo binário");
        return 1;
    }

    txtFile = fopen(out, "w");                                      //Abre o arquivo de saída
    if (txtFile == NULL) {                                          //Se não conseguiu abrir   
        perror("Erro ao criar arquivo de texto");
        fclose(binFile);
        return 1;
    }

    //Lê cada registro do binário e escreve no .txt
    while (fread(&reg, sizeof(Registro), 1, binFile) == 1) {
        fprintf(txtFile, 
            "%08ld %05.1f %2s %-50s %-30s\n",  // Formato fixo
            reg.chave, 
            reg.nota, 
            reg.estado, 
            reg.cidade, 
            reg.curso
        );
    }

    //Fecha os arquivos
    fclose(binFile);
    fclose(txtFile);

    printf("Arquivo texto restaurado com sucesso: '%s'\n", out);
    return 0;
}
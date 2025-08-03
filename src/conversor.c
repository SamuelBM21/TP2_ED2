#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/area.h"

int txtbin(char *in, char *out, int qtd) {
    FILE *txtFile, *binFile;
    int i = 0;
    char buffer[256];

    txtFile = fopen(in, "r");
    if (!txtFile) {
        perror("Erro ao abrir arquivo de texto");
        return 1;
    }

    binFile = fopen(out, "wb");
    if (!binFile) {
        perror("Erro ao criar arquivo binário");
        fclose(txtFile);
        return 1;
    }

    while (fgets(buffer, sizeof(buffer), txtFile) != NULL && i < qtd) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') buffer[--len] = '\0';
        if (len > 0 && buffer[len - 1] == '\r') buffer[--len] = '\0';
        if (len < 98) continue;

        Registro reg = {0};
        char temp[20];

        memcpy(temp, buffer, 8); temp[8] = '\0';
        reg.chave = atol(temp);

        memcpy(temp, buffer + 9, 5); temp[5] = '\0';
        reg.nota = atof(temp);

        memcpy(reg.estado, buffer + 15, 2); reg.estado[2] = '\0';

        memcpy(reg.cidade, buffer + 18, 50); reg.cidade[50] = '\0';

        memcpy(reg.curso, buffer + 69, 30); reg.curso[30] = '\0'; // Corrigido aqui

        fwrite(&reg, sizeof(Registro), 1, binFile);
        i++;
    }

    fclose(txtFile);
    fclose(binFile);
    printf("Conversão concluída com sucesso!\n");
    return 0;
}


int bintxt(char *in, char *out) {
    FILE *binFile, *txtFile;
    Registro reg;

    // Abrir arquivos
    binFile = fopen(in, "rb");
    if (binFile == NULL) {
        perror("Erro ao abrir arquivo binário");
        return 1;
    }

    txtFile = fopen(out, "w");
    if (txtFile == NULL) {
        perror("Erro ao criar arquivo de texto");
        fclose(binFile);
        return 1;
    }

    // Lê cada registro do binário e escreve no .txt
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

    fclose(binFile);
    fclose(txtFile);

    printf("Arquivo texto restaurado com sucesso: 'resultado.txt'\n");
    return 0;
}
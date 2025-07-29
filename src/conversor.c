#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/area.h"

int txtbin(char in[20], char out[20], int qtd) {
    FILE *txtFile, *binFile;
    int i = 0;
    char buffer[256]; // Buffer aumentado para segurança

    // Abrir arquivos
    txtFile = fopen(in, "r");
    if (txtFile == NULL) {
        perror("Erro ao abrir arquivo de texto");
        return 1;
    }

    binFile = fopen(out, "wb");
    if (binFile == NULL) {
        perror("Erro ao criar arquivo binário");
        fclose(txtFile);
        return 1;
    }

    while (fgets(buffer, sizeof(buffer), txtFile) != NULL && i < qtd) {
        // Remove '\n' e '\r' se existirem
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
            len--;
        }
        if (len > 0 && buffer[len - 1] == '\r') {
            buffer[len - 1] = '\0';
            len--;
        }

        // Ignora linhas vazias
        if (len == 0) {
            continue;
        }

        // Verifica se a linha tem pelo menos 98 caracteres (ajustável conforme necessidade)
        if (len < 98) {
            fprintf(stderr, "Linha muito curta: %zu caracteres\n", len);
            continue;
        }

        // Processa os campos
        Registro reg;
        char temp[20];

        // Chave (8 caracteres)
        memcpy(temp, buffer, 8);
        temp[8] = '\0';
        reg.chave = atol(temp);

        // Nota (5 caracteres após um espaço)
        memcpy(temp, buffer + 9, 5);
        temp[5] = '\0';
        reg.nota = atof(temp);

        // Estado (2 caracteres após dois espaços)
        memcpy(reg.estado, buffer + 15, 2);
        reg.estado[2] = '\0';

        // Cidade (50 caracteres)
        memcpy(reg.cidade, buffer + 18, 50);
        reg.cidade[50] = '\0';

        // Curso (30 caracteres)
        memcpy(reg.curso, buffer + 68, 30);
        reg.curso[30] = '\0';

        // Escreve no arquivo binário
        fwrite(&reg, sizeof(Registro), 1, binFile);
        i++;
    }

    fclose(txtFile);
    fclose(binFile);

    printf("Conversão concluída com sucesso!\n");
    return 0;

}

int bintxt(char in[20], char out[20]) {
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
            "%08ld %04.1f %2s %-50s %-30s\n",  // Formato fixo
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
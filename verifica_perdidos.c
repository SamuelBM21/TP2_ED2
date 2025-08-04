#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_REGS 500000

long chavesOrig[MAX_REGS];
long chavesRes[MAX_REGS];
int totalOrig = 0, totalRes = 0;

// Função para extrair a chave (8 primeiros caracteres) de uma linha
long extraiChave(char *linha) {
    char chaveStr[9];
    strncpy(chaveStr, linha, 8);
    chaveStr[8] = '\0';
    return atol(chaveStr);
}

// Função de busca binária
int busca(long chave, long *vet, int tamanho) {
    int ini = 0, fim = tamanho - 1, meio;
    while (ini <= fim) {
        meio = (ini + fim) / 2;
        if (vet[meio] == chave)
            return 1;
        else if (vet[meio] < chave)
            ini = meio + 1;
        else
            fim = meio - 1;
    }
    return 0;
}

// Função para comparar dois longs (para qsort)
int comparaLongs(const void *a, const void *b) {
    long la = *(long *)a;
    long lb = *(long *)b;
    return (la > lb) - (la < lb);
}

int main() {
    FILE *arqOrig = fopen("PROVAO.TXT", "r");
    FILE *arqRes = fopen("resultado.txt", "r");
    int qtd = 1000;
    int i = 0;

    if (!arqOrig || !arqRes) {
        perror("Erro ao abrir arquivos");
        return 1;
    }

    char linha[256];

    // Lê chaves do original
    while (fgets(linha, sizeof(linha), arqOrig) != NULL && i<qtd) {
        chavesOrig[totalOrig++] = extraiChave(linha);
        i++;
    }

    i=0;
    // Lê chaves do resultado
    while (fgets(linha, sizeof(linha), arqRes) != NULL && i<qtd) {
        chavesRes[totalRes++] = extraiChave(linha);
        i++;
    }

    fclose(arqOrig);
    fclose(arqRes);

    // Ordena as chaves do resultado para fazer busca binária
    qsort(chavesRes, totalRes, sizeof(long), comparaLongs);

    printf("Verificando chaves faltantes...\n");

    int faltando = 0;
    for (int i = 0; i < totalOrig; i++) {
        if (!busca(chavesOrig[i], chavesRes, totalRes)) {
            printf("Chave faltando: %08ld\n", chavesOrig[i]);
            faltando++;
        }
    }

    if (faltando == 0) {
        printf("✅ Nenhuma chave perdida no resultado.\n");
    } else {
        printf("❌ Total de chaves perdidas: %d\n", faltando);
    }

    return 0;
}

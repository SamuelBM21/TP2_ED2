#include "../include/intercalacao_qsort.h"

int compara(const void *a, const void *b) {
    Registro *ra = (Registro *)a;
    Registro *rb = (Registro *)b;
    return (ra->chave - rb->chave);
}

// Cria blocos ordenados com até 20 registros e distribui nas 20 primeiras fitas
int criaBlocos(char *arquivoEntrada) {
    FILE *entrada = fopen(arquivoEntrada, "r");
    if (!entrada) {
        perror("Erro ao abrir PROVAO.TXT");
        exit(1);
    }

    Registro buffer[MEMORIA];
    char nomeFita[20];
    FILE *fitas[FITAS_ENTRADA];
    for (int i = 0; i < FITAS_ENTRADA; i++) {
        sprintf(nomeFita, "fita%d.txt", i);
        fitas[i] = fopen(nomeFita, "w");
    }

    int totalBlocos = 0;
    char linha[256];
    while (!feof(entrada)) {
        int count = 0;
        while (count < MEMORIA && fgets(linha, sizeof(linha), entrada)) {
            sscanf(linha, "%ld %lf %2s %50s %30[^\n]",
                   &buffer[count].chave,
                   &buffer[count].nota,
                   buffer[count].estado,
                   buffer[count].cidade,
                   buffer[count].curso);
            count++;
        }
        if (count == 0) break;

        qsort(buffer, count, sizeof(Registro), compara);
        FILE *destino = fitas[totalBlocos % FITAS_ENTRADA];
        for (int i = 0; i < count; i++) {
            fprintf(destino, "%08ld %5.1lf %2s %-50s %-30s\n",
                    buffer[i].chave,
                    buffer[i].nota,
                    buffer[i].estado,
                    buffer[i].cidade,
                    buffer[i].curso);
        }
        totalBlocos++;
    }

    for (int i = 0; i < FITAS_ENTRADA; i++) fclose(fitas[i]);
    fclose(entrada);
    return totalBlocos;
}

// Lê próximo registro de uma fita, retorna 1 se conseguiu ler
int leRegistro(FILE *fp, Registro *r) {
    char linha[256];
    if (fgets(linha, sizeof(linha), fp) == NULL) return 0;
    sscanf(linha, "%ld %lf %2s %50s %30[^\n]",
           &r->chave, &r->nota, r->estado, r->cidade, r->curso);
    return 1;
}

// Realiza a intercalação múltipla até sobrar uma fita com resultado
void intercalacaoBalanceada(int totalBlocos) {
    int rodada = 0;
    int blocosAtivos = totalBlocos;

    while (blocosAtivos > 1) {
        char nome[20];
        FILE *entrada[FITAS_ENTRADA];
        FILE *saida[FITAS_SAIDA];

        // Abre fitas de entrada
        for (int i = 0; i < FITAS_ENTRADA; i++) {
            sprintf(nome, "fita%d.txt", i);
            entrada[i] = fopen(nome, "r");
        }

        // Abre fitas de saída
        for (int i = 0; i < FITAS_SAIDA; i++) {
            sprintf(nome, "fita_out%d.txt", i);
            saida[i] = fopen(nome, "w");
        }

        int registrosRestantes[FITAS_ENTRADA];
        Registro buffer[FITAS_ENTRADA];
        int ativos = 0;

        // Inicializa os buffers
        for (int i = 0; i < FITAS_ENTRADA; i++) {
            registrosRestantes[i] = 0; // Assume que a fita está inativa
        
            // SÓ TENTA LER SE O ARQUIVO FOI ABERTO COM SUCESSO!
            if (entrada[i] != NULL) {
                if (leRegistro(entrada[i], &buffer[i])) {
                    registrosRestantes[i] = 1;
                    ativos++;
                }
            }
        }

        // Faz a intercalação
        int fitaAtual = 0;
        while (ativos > 0) {
            int menor = -1;
            for (int i = 0; i < FITAS_ENTRADA; i++) {
                if (registrosRestantes[i]) {
                    if (menor == -1 || buffer[i].chave < buffer[menor].chave) {
                        menor = i;
                    }
                }
            }

            if (menor == -1) break;

            FILE *dest = saida[fitaAtual % FITAS_SAIDA];
            fprintf(dest, "%08ld %5.1lf %2s %-50s %-30s\n",
                    buffer[menor].chave,
                    buffer[menor].nota,
                    buffer[menor].estado,
                    buffer[menor].cidade,
                    buffer[menor].curso);

            if (leRegistro(entrada[menor], &buffer[menor])) {
                registrosRestantes[menor] = 1;
            } else {
                registrosRestantes[menor] = 0;
                ativos--;
            }

            fitaAtual++;
        }

        // Fecha arquivos
        for (int i = 0; i < FITAS_ENTRADA; i++) fclose(entrada[i]);
        for (int i = 0; i < FITAS_SAIDA; i++) fclose(saida[i]);

        // Troca papéis: fitas de saída viram entrada
        for (int i = 0; i < FITAS_ENTRADA; i++) {
            char oldname[20], newname[20];
            sprintf(oldname, "fita_out%d.txt", i);
            sprintf(newname, "fita%d.txt", i);
            rename(oldname, newname);
        }

        blocosAtivos = (blocosAtivos + FITAS_ENTRADA - 1) / FITAS_ENTRADA;
        rodada++;
    }

    // Resultado final na fita0.txt
    rename("fita0.txt", "PROVAO_ORDENADO.TXT");
}


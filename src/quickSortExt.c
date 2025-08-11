#include "quickSortExt.h"

/*
Nome: QuicksortExterno
Função: Executa o algoritmo de QuickSort para ordenação externa de registros em arquivo.
Entrada: Ponteiros para arquivos de leitura/escrita, índices Esq e Dir, e contadores de comparações/leitura/escrita.
Saída:--
*/

void QuicksortExterno(FILE **ArqLi , FILE **ArqEi , FILE **ArqLEs, int Esq, int Dir, long *compCount, long *readCount, long *writeCount) {
    int i, j;
    TipoArea Area;
    if (Esq >= Dir) return; // condição de parada
    FAVazia(&Area);
    Particao(ArqLi , ArqEi , ArqLEs, Area, Esq, Dir, &i , &j, compCount, readCount, writeCount);

    // Ordena a menor partição primeiro (otimização de recursão)
    if (i - Esq < Dir - j) {
        QuicksortExterno(ArqLi , ArqEi , ArqLEs, Esq, i, compCount, readCount, writeCount);
        QuicksortExterno(ArqLi , ArqEi , ArqLEs, j, Dir, compCount, readCount, writeCount);
    } else {
        QuicksortExterno(ArqLi , ArqEi , ArqLEs, j, Dir, compCount, readCount, writeCount);
        QuicksortExterno(ArqLi , ArqEi , ArqLEs, Esq, i, compCount, readCount, writeCount);
    }
}

/*
Nome: LeSup
Função: Lê o próximo registro a partir da posição no arquivo superior.
Entrada: Ponteiro para arquivo, registro destino, posição e flag de controle.
Saída:--
*/
void LeSup(FILE **ArqLEs, Registro *UltLido , int *Ls , bool *OndeLer) {
    fflush(*ArqLEs);
    fseek(*ArqLEs, (*Ls - 1) * sizeof(Registro), SEEK_SET);
    fread(UltLido, sizeof(Registro), 1, *ArqLEs);
    (*Ls)--;
    *OndeLer = false;
}

/*
Nome: LeInf
Função: Lê o próximo registro a partir da posição no arquivo inferior.
Entrada: Ponteiro para arquivo, registro destino, posição e flag de controle.
Saída: --
*/
void LeInf(FILE **ArqLi , Registro *UltLido , int *Li , bool *OndeLer) {
    fflush(*ArqLi);
    fread(UltLido, sizeof(Registro), 1, *ArqLi);
    (*Li)++;
    *OndeLer = true;
}

/*
Nome: InserirArea
Função: Insere um registro na área de memória temporária.
Entrada: Área, registro e número de registros na área.
Saída: --
*/
void InserirArea(TipoArea *Area, Registro *UltLido , int *NRArea) {
    InsereItem(*UltLido, Area);
    *NRArea = ObterNumCelOcupadas(Area);
}

/*
Nome: EscreveMax
Função: Escreve um registro na posição final do arquivo.
Entrada: Ponteiro para arquivo, registro e posição.
Saída: --
*/
void EscreveMax(FILE **ArqLEs, Registro R, int *Es) {
    fseek(*ArqLEs, (*Es - 1) * sizeof(Registro), SEEK_SET);
    fwrite(&R, sizeof(Registro), 1, *ArqLEs);
    fflush(*ArqLEs);
    (*Es)--;
}

/*
Nome: EscreveMin
Função: Escreve um registro na próxima posição inicial do arquivo.
Entrada: Ponteiro para arquivo, registro e posição.
Saída: --
*/
void EscreveMin(FILE **ArqEi , Registro R, int *Ei) {
    fwrite(&R, sizeof(Registro), 1, *ArqEi);
    fflush(*ArqEi);
    (*Ei)++;
}

/*
Nome: RetiraMax
Função: Remove o maior registro da área.
Entrada: Área, registro destino e número de registros.
Saída:--
*/
void RetiraMax(TipoArea *Area, Registro *R, int *NRArea) {
    RetiraUltimo(Area, R);
    *NRArea = ObterNumCelOcupadas(Area);
}

/*
Nome: RetiraMin
Função: Remove o menor registro da área.
Entrada: Área, registro destino e número de registros.
Saída: --
*/
void RetiraMin(TipoArea *Area, Registro *R, int *NRArea) {
    RetiraPrimeiro(Area, R);
    *NRArea = ObterNumCelOcupadas(Area);
}

/*
Nome: Particao
Função: Realiza a partição dos registros em três áreas: menor, maior e área temporária.
Entrada: Ponteiros para arquivos, área, índices Esq e Dir, ponteiros de retorno i e j, e contadores.
Saída:--
*/
void Particao(FILE **ArqLi , FILE **ArqEi , FILE **ArqLEs, TipoArea Area, int Esq, int Dir , int *i , int *j, long *compCount, long *readCount, long *writeCount) {
    int Ls = Dir, Es = Dir, Li = Esq, Ei = Esq, NRArea = 0;
    double Linf = DBL_MIN, Lsup = DBL_MAX;
    bool OndeLer = true;
    Registro UltLido, R;

    // Posiciona ponteiros de leitura nos arquivos
    fseek(*ArqLi , (Li - 1) * sizeof(Registro), SEEK_SET);
    fseek(*ArqEi , (Ei - 1) * sizeof(Registro), SEEK_SET);
    *i = Esq - 1;
    *j = Dir + 1;

    // Loop principal de leitura e distribuição
    while (Ls >= Li) {
        (*compCount)++;
        // Preenche a área até seu limite
        if (NRArea < TAMAREA - 1) {
            if (OndeLer)
                LeSup(ArqLEs, &UltLido, &Ls, &OndeLer);
            else
                LeInf(ArqLi, &UltLido, &Li, &OndeLer);
            InserirArea(&Area, &UltLido, &NRArea);
            (*readCount)++;
            continue;
        }

        // Decide de onde ler
        if (Ls == Es)
            LeSup(ArqLEs, &UltLido, &Ls, &OndeLer);
        else if (Li == Ei)
            LeInf(ArqLi, &UltLido, &Li, &OndeLer);
        else if (OndeLer)
            LeSup(ArqLEs, &UltLido, &Ls, &OndeLer);
        else
            LeInf(ArqLi, &UltLido, &Li, &OndeLer);
        (*readCount)++;

        // Comparações para decidir onde colocar o registro
        (*compCount)++;
        if (UltLido.nota > Lsup) {
            *j = Es;
            EscreveMax(ArqLEs, UltLido, &Es);
            (*writeCount)++;
            continue;
        }
        (*compCount)++;
        if (UltLido.nota < Linf) {
            *i = Ei;
            EscreveMin(ArqEi, UltLido, &Ei);
            (*writeCount)++;
            continue;
        }

        // Insere na área e escreve o menor ou maior conforme o lado menor
        InserirArea(&Area, &UltLido, &NRArea);
        if (Ei - Esq < Dir - Es) {
            RetiraMin(&Area, &R, &NRArea);
            EscreveMin(ArqEi, R, &Ei);
            (*writeCount)++;
            Linf = R.nota;
        } else {
            RetiraMax(&Area, &R, &NRArea);
            EscreveMax(ArqLEs, R, &Es);
            (*writeCount)++;
            Lsup = R.nota;
        }
    }

    // Esvazia a área restante
    while (Ei <= Es) {
        RetiraMin(&Area, &R, &NRArea);
        EscreveMin(ArqEi, R, &Ei);
        (*writeCount)++;
    }
}

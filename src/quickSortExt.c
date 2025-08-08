#include "quickSortExt.h"

void QuicksortExterno(FILE **ArqLi , FILE **ArqEi , FILE **ArqLEs, int Esq, int Dir, long *compCount) {
    int i, j;
    TipoArea Area;
    if (Esq >= Dir) return; // condição de parada mais clara
    FAVazia(&Area);
    Particao(ArqLi , ArqEi , ArqLEs, Area, Esq, Dir, &i , &j, compCount);
    if (i - Esq < Dir - j) {
        QuicksortExterno(ArqLi , ArqEi , ArqLEs, Esq, i, compCount);
        QuicksortExterno(ArqLi , ArqEi , ArqLEs, j, Dir, compCount);
    } else {
        QuicksortExterno(ArqLi , ArqEi , ArqLEs, j, Dir, compCount);
        QuicksortExterno(ArqLi , ArqEi , ArqLEs, Esq, i, compCount);
    }
}

void LeSup(FILE **ArqLEs, Registro *UltLido , int *Ls , bool *OndeLer) {
    fflush(*ArqLEs);
    fseek(*ArqLEs, (*Ls - 1) * sizeof(Registro), SEEK_SET);
    fread(UltLido, sizeof(Registro), 1, *ArqLEs);
    (*Ls)--;
    *OndeLer = false;
}

void LeInf(FILE **ArqLi , Registro *UltLido , int *Li , bool *OndeLer) {
    fflush(*ArqLi);
    fread(UltLido, sizeof(Registro), 1, *ArqLi);
    (*Li)++;
    *OndeLer = true;
}

void InserirArea(TipoArea *Area, Registro *UltLido , int *NRArea) {
    InsereItem(*UltLido, Area);
    *NRArea = ObterNumCelOcupadas(Area);
}

void EscreveMax(FILE **ArqLEs, Registro R, int *Es) {
    fseek(*ArqLEs, (*Es - 1) * sizeof(Registro), SEEK_SET);
    fwrite(&R, sizeof(Registro), 1, *ArqLEs);
    fflush(*ArqLEs);
    (*Es)--;
}

void EscreveMin(FILE **ArqEi , Registro R, int *Ei) {
    fwrite(&R, sizeof(Registro), 1, *ArqEi);
    fflush(*ArqEi);
    (*Ei)++;
}

void RetiraMax(TipoArea *Area, Registro *R, int *NRArea) {
    RetiraUltimo(Area, R);
    *NRArea = ObterNumCelOcupadas(Area);
}

void RetiraMin(TipoArea *Area, Registro *R, int *NRArea) {
    RetiraPrimeiro(Area, R);
    *NRArea = ObterNumCelOcupadas(Area);
}

void Particao(FILE **ArqLi , FILE **ArqEi , FILE **ArqLEs, TipoArea Area, int Esq, int Dir , int *i , int *j, long *compCount) {
    int Ls = Dir, Es = Dir, Li = Esq, Ei = Esq, NRArea = 0;
    double Linf = DBL_MIN, Lsup = DBL_MAX;
    bool OndeLer = true;
    Registro UltLido, R;

    fseek(*ArqLi , (Li - 1) * sizeof(Registro), SEEK_SET);
    fseek(*ArqEi , (Ei - 1) * sizeof(Registro), SEEK_SET);
    *i = Esq - 1;
    *j = Dir + 1;

    while (Ls >= Li) {
        (*compCount)++;
        if (NRArea < TAMAREA - 1) {
            if (OndeLer)
                LeSup(ArqLEs, &UltLido, &Ls, &OndeLer);
            else
                LeInf(ArqLi, &UltLido, &Li, &OndeLer);
            InserirArea(&Area, &UltLido, &NRArea);
            continue;
        }

        if (Ls == Es)
            LeSup(ArqLEs, &UltLido, &Ls, &OndeLer);
        else if (Li == Ei)
            LeInf(ArqLi, &UltLido, &Li, &OndeLer);
        else if (OndeLer)
            LeSup(ArqLEs, &UltLido, &Ls, &OndeLer);
        else
            LeInf(ArqLi, &UltLido, &Li, &OndeLer);

        (*compCount)++;
        if (UltLido.nota > Lsup) {
            *j = Es;
            EscreveMax(ArqLEs, UltLido, &Es);
            continue;
        }
        (*compCount)++;
        if (UltLido.nota < Linf) {
            *i = Ei;
            EscreveMin(ArqEi, UltLido, &Ei);
            continue;
        }

        InserirArea(&Area, &UltLido, &NRArea);
        if (Ei - Esq < Dir - Es) {
            RetiraMin(&Area, &R, &NRArea);
            EscreveMin(ArqEi, R, &Ei);
            Linf = R.nota;
        } else {
            RetiraMax(&Area, &R, &NRArea);
            EscreveMax(ArqLEs, R, &Es);
            Lsup = R.nota;
        }
    }

    while (Ei <= Es) {
        RetiraMin(&Area, &R, &NRArea);
        EscreveMin(ArqEi, R, &Ei);
    }
}

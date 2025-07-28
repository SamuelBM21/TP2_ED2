#include "../include/quickSortExt.h"
#include "../include/area.h"

void QuicksortExterno (FILE **ArqLi , FILE **ArqEi , FILE **ArqLEs, int Esq, int Dir){
    int i , j; 
    TipoArea Area; /*Area de armazenamento interna */ 
    if (Dir - Esq < 1) return; 
    FAVazia(&Area); 
    Particao(ArqLi , ArqEi , ArqLEs, Area, Esq, Dir, &i , &j ); 
    if ( i - Esq < Dir - j ) { /*ordene primeiro o subarquivo menor */ 
        QuicksortExterno (ArqLi , ArqEi , ArqLEs, Esq, i ); 
        QuicksortExterno (ArqLi , ArqEi , ArqLEs, j , Dir );
    } else{
        QuicksortExterno (ArqLi , ArqEi , ArqLEs, j , Dir );
        QuicksortExterno (ArqLi , ArqEi , ArqLEs, Esq, i );
    } 
}

void LeInf(FILE **Arq, Registro *reg, int *Li, bool *OndeLer) {
    printf("LeInf: tentando acessar linha %d\n", *Li);
    char linha[200];
    fseek(*Arq, 0, SEEK_SET); // ← volta ao início do arquivo
    for (int i = 0; i < *Li; i++) {
        if (fgets(linha, sizeof(linha), *Arq) == NULL) return;  // até alcançar a linha desejada
    }

    sscanf(linha, "%8ld %5lf %2s %50[^\n] %30[^\n]",
           &reg->chave, &reg->nota, reg->estado, reg->cidade, reg->curso);

    (*Li)++;
    *OndeLer = true;
}

void LeSup(FILE **Arq, Registro *reg, int *Ls, bool *OndeLer) {
    printf("LeSup: tentando acessar linha %d\n", *Ls);
    char linha[200];
    fseek(*Arq, 0, SEEK_SET); // ← volta ao início do arquivo
    for (int i = 0; i < *Ls; i++) {
        if (fgets(linha, sizeof(linha), *Arq) == NULL) return;  // até alcançar a linha desejada
    }

    sscanf(linha, "%8ld %5lf %2s %50[^\n] %30[^\n]",
           &reg->chave, &reg->nota, reg->estado, reg->cidade, reg->curso);

    (*Ls)--;
    *OndeLer = false;
}


void InserirArea (TipoArea *Area, Registro *UltLido , int *NRArea) { /*Insere UltLido de forma ordenada na Area*/
    InsereItem(*UltLido , Area);
    *NRArea = ObterNumCelOcupadas(Area);
}

void EscreveMax(FILE **ArqLEs, Registro R, int *Es) {
    char linha[200];
    for (int i = 0; i < *Es; i++) {
        if (fgets(linha, sizeof(linha), *ArqLEs) == NULL) return;  // até alcançar a linha desejada
    }
    fprintf(*ArqLEs, "%08ld %5.1lf %2s %-50s %-30s\n",
        R.chave,
        R.nota,
        R.estado,
        R.cidade,
        R.curso
    );
    (*Es)--;
}

void EscreveMin(FILE **ArqEi , Registro R, int *Ei) {
    fprintf(*ArqEi, "%08ld %5.1lf %2s %-50s %-30s\n",
        R.chave,
        R.nota,
        R.estado,
        R.cidade,
        R.curso
    );
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

void Particao(FILE **ArqLi , FILE **ArqEi , FILE **ArqLEs, TipoArea Area, int Esq, int Dir , int * i , int * j ){
    int Ls = Dir , Es = Dir , Li = Esq, Ei = Esq, NRArea = 0;
    double Linf = -DBL_MAX, Lsup = DBL_MAX;
    bool OndeLer = true;
    Registro UltLido , R;
    //fseek (*ArqLi , ( Li - 1)* sizeof(Registro ) , SEEK_SET );
    //fseek (*ArqEi , (Ei - 1)* sizeof(Registro ) , SEEK_SET );
    *i = Esq - 1;
    *j = Dir + 1;
    printf("Particao: Esq = %d, Dir = %d\n", Esq, Dir);
    while (Ls >= Li) {
        if (NRArea < (TAMAREA - 1)) {
            if (OndeLer) 
                LeSup(ArqLEs, &UltLido, &Ls, &OndeLer);
            else 
                LeInf(ArqLi, &UltLido, &Li , &OndeLer);
            InserirArea(&Area, &UltLido, &NRArea );
            continue;
        }

        if (Ls == Es) 
            LeSup(ArqLEs, &UltLido, &Ls, &OndeLer);
        else if ( Li == Ei) 
            LeInf(ArqLi, &UltLido, &Li , &OndeLer);
        else if (OndeLer)
            LeSup(ArqLEs, &UltLido, &Ls, &OndeLer);
        else    
            LeInf(ArqLi, &UltLido, &Li , &OndeLer);

        if (UltLido.nota > Lsup) {
            * j = Es;
            EscreveMax(ArqLEs, UltLido, &Es);
            continue ;
        }
        if (UltLido.nota < Linf) {
            * i = Ei ;
            EscreveMin(ArqEi , UltLido, &Ei );
            continue ;
        }
        InserirArea(&Area, &UltLido, &NRArea );
        if (Ei - Esq < Dir - Es) {
            RetiraMin(&Area, &R, &NRArea );
            EscreveMin(ArqEi , R, &Ei );
            Linf = R.nota;
        } 
        else { 
            RetiraMax(&Area, &R, &NRArea );
            EscreveMax(ArqLEs, R, &Es);
            Lsup = R.nota;
        } 
    }
    while (Ei <= Es) { 
        RetiraMin(&Area, &R, &NRArea );
        EscreveMin(ArqEi , R, &Ei );
    }
    printf("Particao terminou: *i = %d, *j = %d\n", *i, *j);
}
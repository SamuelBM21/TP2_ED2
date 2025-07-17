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

void LeSup(FILE **ArqLEs, Registro *UltLido , int *Ls , bool *OndeLer) { 
    fseek(*ArqLEs, (*Ls - 1) * sizeof(Registro) ,SEEK_SET );
    fread(UltLido , sizeof(Registro ) , 1 , *ArqLEs);
    (*Ls)--;
    *OndeLer = false;
}

void LeInf(FILE **ArqLi , Registro *UltLido , int *Li , bool *OndeLer) {
    fread(UltLido , sizeof(Registro ) , 1 , *ArqLi );
    (*Li)++;
    *OndeLer = true;
}

void InserirArea (TipoArea *Area, Registro *UltLido , int *NRArea) { /*Insere UltLido de forma ordenada na Area*/
    InsereItem(*UltLido , Area);
    *NRArea = ObterNumCelOcupadas(Area);
}

void EscreveMax(FILE **ArqLEs, Registro R, int *Es) {
    fseek(*ArqLEs, (*Es - 1) * sizeof(Registro) ,SEEK_SET );
    fwrite(&R, sizeof(Registro ) , 1 , *ArqLEs);
    (*Es)--;
}

void EscreveMin(FILE **ArqEi , Registro R, int *Ei) {
    fwrite(&R, sizeof(Registro ) , 1 , *ArqEi );
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
    int Ls = Dir , Es = Dir , Li = Esq, Ei = Esq, NRArea = 0 , Linf = INT_MIN , Lsup = INT_MAX;
    bool OndeLer = true;
    Registro UltLido , R;
    fseek (*ArqLi , ( Li - 1)* sizeof(Registro ) , SEEK_SET );
    fseek (*ArqEi , (Ei - 1)* sizeof(Registro ) , SEEK_SET );
    * i = Esq - 1;
    *j = Dir + 1;
    while (Ls >= Li) {
        if (NRArea < TAMAREA - 1) {
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

        if (UltLido.chave > Lsup) {
            * j = Es;
            EscreveMax(ArqLEs, UltLido, &Es);
            continue ;
        }
        if (UltLido.chave < Linf) {
            * i = Ei ;
            EscreveMin(ArqEi , UltLido, &Ei );
            continue ;
        }
        InserirArea(&Area, &UltLido, &NRArea );
        if (Ei - Esq < Dir - Es) {
            RetiraMin(&Area, &R, &NRArea );
            EscreveMin(ArqEi , R, &Ei );
            Linf = R.chave;
        } 
        else { 
            RetiraMax(&Area, &R, &NRArea );
            EscreveMax(ArqLEs, R, &Es);
            Lsup = R.chave;
        } 
    }
    while (Ei <= Es) { 
        RetiraMin(&Area, &R, &NRArea );
        EscreveMin(ArqEi , R, &Ei );
    }
}
#include "area.h"

void FAVazia(TipoArea *Area) {
    Area->n = 0;
}

int ObterNumCelOcupadas(TipoArea *Area) {
    return Area->n;
}

void troca(Registro *a, Registro *b) {
    Registro temp = *a;
    *a = *b;
    *b = temp;
}

void sobeHeapMin(Registro v[], int i) {
    while (i > 0 && v[i].nota < v[(i - 1) / 2].nota) {
        troca(&v[i], &v[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

void desceHeapMin(Registro v[], int n, int i) {
    int menor = i;
    int esq = 2 * i + 1;
    int dir = 2 * i + 2;

    if (esq < n && v[esq].nota < v[menor].nota) menor = esq;
    if (dir < n && v[dir].nota < v[menor].nota) menor = dir;

    if (menor != i) {
        troca(&v[i], &v[menor]);
        desceHeapMin(v, n, menor);
    }
}

void InsereItem(Registro reg, TipoArea *Area) {
    if (Area->n >= TAMAREA) return;
    Area->itens[Area->n] = reg;
    sobeHeapMin(Area->itens, Area->n);
    Area->n++;
}

void RetiraPrimeiro(TipoArea *Area, Registro *reg) {
    if (Area->n == 0) return;
    *reg = Area->itens[0];
    Area->itens[0] = Area->itens[Area->n - 1];
    Area->n--;
    desceHeapMin(Area->itens, Area->n, 0);
}

void RetiraUltimo(TipoArea *Area, Registro *reg) {
    if (Area->n == 0) return;

    // Procurar o índice do maior elemento
    int idxMaior = 0;
    for (int i = 1; i < Area->n; i++) {
        if (Area->itens[i].nota > Area->itens[idxMaior].nota) {
            idxMaior = i;
        }
    }

    *reg = Area->itens[idxMaior];

    // Substituir o maior pelo último e reduzir o tamanho
    Area->itens[idxMaior] = Area->itens[Area->n - 1];
    Area->n--;

    // Reorganizar a heap mínima (apenas para manter estrutura)
    for (int i = (Area->n / 2) - 1; i >= 0; i--) {
        desceHeapMin(Area->itens, Area->n, i);
    }
}

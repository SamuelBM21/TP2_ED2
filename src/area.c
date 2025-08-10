#include "area.h"

void FAVazia(TipoArea *Area) {
    Area->n = 0;
}

int ObterNumCelOcupadas(TipoArea *Area) {
    return Area->n;
}

void InsereItem(Registro reg, TipoArea *Area) {
    if (Area->n >= TAMAREA) return;

    int i = Area->n - 1;

    // Desloca elementos maiores para a direita
    while (i >= 0 && Area->itens[i].nota > reg.nota) {
        Area->itens[i + 1] = Area->itens[i];
        i--;
    }
    // Insere o novo registro na posição correta
    Area->itens[i + 1] = reg;
    Area->n++;
}

void RetiraPrimeiro(TipoArea *Area, Registro *reg) {
    if (Area->n == 0) return;

    // O primeiro elemento é o menor (lista ordenada crescente)
    *reg = Area->itens[0];

    // Desloca todos os elementos para a esquerda
    for (int i = 1; i < Area->n; i++) {
        Area->itens[i - 1] = Area->itens[i];
    }
    Area->n--;
}

void RetiraUltimo(TipoArea *Area, Registro *reg) {
    if (Area->n == 0) return;

    // O último elemento é o maior
    *reg = Area->itens[Area->n - 1];
    Area->n--;
}
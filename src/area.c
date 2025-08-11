#include "area.h"

/*
Nome: FAVazia  
Função: Inicializa a área de memória para que comece vazia.  
Entrada: Ponteiro para uma estrutura TipoArea.  
Saída:-- 
*/
void FAVazia(TipoArea *Area) {
    Area->n = 0;  // Define que não há elementos ocupando a área
}

/*
Nome: ObterNumCelOcupadas  
Função: Retorna o número de células ocupadas na área.  
Entrada: Ponteiro para uma estrutura TipoArea.  
Saída: Número de registros armazenados atualmente.  
*/
int ObterNumCelOcupadas(TipoArea *Area) {
    return Area->n;
}

/*
Nome: InsereItem  
Função: Insere um novo registro na área em ordem crescente de nota, deslocando registros maiores para a direita.  
Entrada: Registro a ser inserido e ponteiro para a estrutura TipoArea.  
Saída:--  
*/
void InsereItem(Registro reg, TipoArea *Area) {
    if (Area->n >= TAMAREA) return;  // Se a área está cheia, não insere

    int i = Area->n - 1;

    // Desloca elementos maiores para a direita para manter a ordenação
    while (i >= 0 && Area->itens[i].nota > reg.nota) {
        Area->itens[i + 1] = Area->itens[i];
        i--;
    }
    // Insere o novo registro na posição correta
    Area->itens[i + 1] = reg;
    Area->n++;
}

/*
Nome: RetiraUltimo  
Função: Remove o último elemento da área e o retorna.  
Entrada: Ponteiro para estrutura TipoArea e ponteiro para Registro (onde será armazenado o valor removido).  
Saída:-- 
*/
void RetiraUltimo(TipoArea *Area, Registro *reg) {
    *reg = Area->itens[Area->n - 1];  // Copia o último registro para a variável de saída
    Area->n--;  // Diminui a contagem de elementos ocupados
}

/*
Nome: RetiraPrimeiro  
Função: Remove o primeiro elemento da área e o retorna, deslocando todos os outros registros uma posição para a esquerda.  
Entrada: Ponteiro para estrutura TipoArea e ponteiro para Registro (onde será armazenado o valor removido).  
Saída:--
*/
void RetiraPrimeiro(TipoArea *Area, Registro *reg) {
    *reg = Area->itens[0];  // Copia o primeiro registro
    // Desloca todos os elementos uma posição para a esquerda
    for (int i = 0; i < Area->n - 1; i++) {
        Area->itens[i] = Area->itens[i + 1];
    }
    Area->n--;
}
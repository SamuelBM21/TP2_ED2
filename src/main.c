#include <stdio.h>
#include "../include/quickSortExt.h"
#include "../include/area.h"

int main() {
    // Simples chamada fictícia só para compilar
    FILE *li = NULL, *ei = NULL, *les = NULL;
    QuicksortExterno(&li, &ei, &les, 0, 0);
    printf("Quicksort externo chamado!\n");
    return 0;
}
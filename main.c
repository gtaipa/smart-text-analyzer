#include <stdio.h>
#include "Matrix_create.h"



// função auxiliar só para imprimir as linhas
void listarMatriz(MatrizTexto m) {
    printf("\nMatriz contém %d linhas:\n", m.linhas_usadas);
    for (int i = 0; i < m.linhas_usadas; i++) {
        printf("[%d] %s\n", i, m.linhas[i]);
    }
}

int main() {
    // Criar matriz com capacidade para 3 linhas
    MatrizTexto texto = criarMatrizTexto(3);

    // Adicionar frases
    adicionarLinha(&texto, "the cat sleeps");
    adicionarLinha(&texto, "the dog runs");
    adicionarLinha(&texto, "a cat eats");

    // Mostrar conteúdo
    listarMatriz(texto);

    // Libertar memória
    libertarMatriz(texto);

    printf("\n✅ Teste concluído com sucesso!\n");
    return 0;
}


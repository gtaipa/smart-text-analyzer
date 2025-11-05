#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// =============================================================
//  Estrutura: MatrizTexto
//  Representa um conjunto de strings (texto) armazenadas dinamicamente
// =============================================================
typedef struct {
    char **linhas;        // array de strings (cada linha é uma frase)
    int linhas_usadas;    // número de linhas atualmente usadas
    int capacidade;       // tamanho máximo da matriz
} MatrizTexto;

// =============================================================
//  Funções do módulo (antes estavam em Matrix_create.c)
// =============================================================

MatrizTexto criarMatrizTexto(int capacidade_inicial) {
    MatrizTexto m;
    m.linhas = malloc(capacidade_inicial * sizeof(char*));
    if (m.linhas == NULL) {
        printf("Erro: sem memória.\n");
        exit(1);
    }
    m.linhas_usadas = 0;
    m.capacidade = capacidade_inicial;
    return m;
}

void libertarMatriz(MatrizTexto m) {
    if (m.linhas != NULL) {
        for (int i = 0; i < m.linhas_usadas; i++) {
            if (m.linhas[i] != NULL)
                free(m.linhas[i]);
        }
        free(m.linhas);
    }
}

int adicionarLinha(MatrizTexto *m, const char *frase) {
    // Verificar se ainda há espaço
    if (m->linhas_usadas == m->capacidade) {
        printf("Aviso: matriz cheia. (não pode adicionar mais frases)\n");
        return 0;
    }

    // Reservar memória para a nova string
    m->linhas[m->linhas_usadas] = malloc(strlen(frase) + 1); // +1 para o '\0'
    if (m->linhas[m->linhas_usadas] == NULL) {
        printf("Erro: sem memória para guardar a frase.\n");
        return 0;
    }

    // Copiar a frase
    strcpy(m->linhas[m->linhas_usadas], frase);

    // Atualizar contador
    m->linhas_usadas++;

    return 1;
}

// =============================================================
//  Função auxiliar apenas para listar o conteúdo
// =============================================================

void listarMatriz(MatrizTexto m) {
    printf("\nMatriz contém %d linhas:\n", m.linhas_usadas);
    for (int i = 0; i < m.linhas_usadas; i++) {
        printf("[%d] %s\n", i, m.linhas[i]);
    }
}

// =============================================================
//  Função principal (main)
// =============================================================

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

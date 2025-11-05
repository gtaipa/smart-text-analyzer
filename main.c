#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// =============================================================
//  Estrutura: MatrizTexto
// =============================================================
typedef struct {
    char **linhas;        // array de strings (cada linha é uma frase)
    int linhas_usadas;    // número de linhas atualmente usadas
    int capacidade;       // tamanho máximo da matriz
} MatrizTexto;

// =============================================================
//  Estrutura: MatrizTokens
// =============================================================
typedef struct {
    char **tokens;        // array de strings (cada token é uma palavra/símbolo)
    int usados;           // número de tokens usados
    int capacidade;       // capacidade máxima
} MatrizTokens;

// =============================================================
//  Funções auxiliares de texto
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

// Redimensionar matriz de texto
int redimensionarMatriz(MatrizTexto *m, int nova_capacidade) {
    char **novas_linhas = realloc(m->linhas, nova_capacidade * sizeof(char*));
    if (novas_linhas == NULL) {
        printf("Erro: sem memória ao redimensionar.\n");
        return 0;
    }
    m->linhas = novas_linhas;
    m->capacidade = nova_capacidade;
    printf("🔄 Matriz redimensionada para capacidade %d.\n", nova_capacidade);
    return 1;
}

int adicionarLinha(MatrizTexto *m, const char *frase) {
    if (m->linhas_usadas == m->capacidade) {
        // Dobra a capacidade automaticamente
        redimensionarMatriz(m, m->capacidade * 2);
    }

    m->linhas[m->linhas_usadas] = malloc(strlen(frase) + 1);
    if (m->linhas[m->linhas_usadas] == NULL) {
        printf("Erro: sem memória para guardar a frase.\n");
        return 0;
    }

    strcpy(m->linhas[m->linhas_usadas], frase);
    m->linhas_usadas++;
    return 1;
}

void listarMatriz(MatrizTexto m) {
    printf("\n📜 Matriz contém %d linhas:\n", m.linhas_usadas);
    for (int i = 0; i < m.linhas_usadas; i++) {
        printf("[%d] %s\n", i, m.linhas[i]);
    }
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

// =============================================================
//  Funções auxiliares de tokens
// =============================================================
MatrizTokens criarMatrizTokens(int capacidade_inicial) {
    MatrizTokens t;
    t.tokens = malloc(capacidade_inicial * sizeof(char*));
    if (t.tokens == NULL) {
        printf("Erro: sem memória para tokens.\n");
        exit(1);
    }
    t.usados = 0;
    t.capacidade = capacidade_inicial;
    return t;
}

int adicionarToken(MatrizTokens *t, const char *token) {
    if (t->usados == t->capacidade) {
        int nova_cap = t->capacidade * 2;
        char **novos = realloc(t->tokens, nova_cap * sizeof(char*));
        if (novos == NULL) {
            printf("Erro: sem memória ao redimensionar tokens.\n");
            return 0;
        }
        t->tokens = novos;
        t->capacidade = nova_cap;
        printf("🔄 Tokens redimensionados para capacidade %d.\n", nova_cap);
    }

    t->tokens[t->usados] = malloc(strlen(token) + 1);
    strcpy(t->tokens[t->usados], token);
    t->usados++;
    return 1;
}

void listarTokens(MatrizTokens t) {
    printf("\n🔤 Tokens armazenados (%d):\n", t.usados);
    for (int i = 0; i < t.usados; i++) {
        printf("[%d] %s\n", i, t.tokens[i]);
    }
}

void libertarTokens(MatrizTokens t) {
    for (int i = 0; i < t.usados; i++) {
        free(t.tokens[i]);
    }
    free(t.tokens);
}

// =============================================================
//  Programa principal (testes automáticos)
// =============================================================
int main() {

    printf("=== Início dos testes automáticos ===\n");

    // Teste 1: criar matriz de texto
    printf("\n[1] Criar matriz de texto com capacidade para 3 linhas...\n");
    MatrizTexto texto = criarMatrizTexto(3);
    adicionarLinha(&texto, "the cat sleeps");
    adicionarLinha(&texto, "the dog runs");
    adicionarLinha(&texto, "a cat eats");

    // Teste 2: redimensionar automaticamente
    printf("\n[2] Adicionar mais frases para forçar redimensionamento...\n");
    adicionarLinha(&texto, "extra line that triggers resize");

    listarMatriz(texto);

    // Teste 3: criar matriz de tokens
    printf("\n[3] Criar e listar tokens...\n");
    MatrizTokens tokens = criarMatrizTokens(4);
    adicionarToken(&tokens, "the");
    adicionarToken(&tokens, "cat");
    adicionarToken(&tokens, "dog");
    adicionarToken(&tokens, "runs");
    adicionarToken(&tokens, "eats");

    listarTokens(tokens);

    // Libertar memória
    printf("\n[4] Libertar memória...\n");
    libertarMatriz(texto);
    libertarTokens(tokens);

    printf("\n✅ Teste R1.1 concluído com sucesso!\n");
    printf("=== Fim dos testes ===\n");

    return 0;
}

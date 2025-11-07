// Created by pedro on 07/11/2025.

#include "manipulacao_matrizes.h"

// =============================================================
//  Funcoes auxiliares de texto
// =============================================================
MatrizTexto criarMatrizTexto(int capacidade_inicial) {
    MatrizTexto m;
    m.linhas = malloc(capacidade_inicial * sizeof(char*));
    if (m.linhas == NULL) {
        printf("Erro: sem memoria.\n");
        exit(1);
    }
    m.linhas_usadas = 0;
    m.capacidade = capacidade_inicial;
    return m;
}

int redimensionarMatriz(MatrizTexto *m, int nova_capacidade) {
    char **novas_linhas = realloc(m->linhas, nova_capacidade * sizeof(char*));
    if (novas_linhas == NULL) {
        printf("Erro: sem memoria ao redimensionar.\n");
        return 0;
    }
    m->linhas = novas_linhas;
    m->capacidade = nova_capacidade;
    printf("Matriz redimensionada para capacidade %d.\n", nova_capacidade);
    return 1;
}

int adicionarLinha(MatrizTexto *m, const char *frase) {
    if (m->linhas_usadas == m->capacidade) {
        redimensionarMatriz(m, m->capacidade * 2);
    }

    m->linhas[m->linhas_usadas] = malloc(strlen(frase) + 1);
    if (m->linhas[m->linhas_usadas] == NULL) {
        printf("Erro: sem memoria para guardar a frase.\n");
        return 0;
    }

    strcpy(m->linhas[m->linhas_usadas], frase);
    m->linhas_usadas++;
    return 1;
}

void listarMatriz(MatrizTexto m) {
    printf("\nMatriz contem %d linhas:\n", m.linhas_usadas);
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

void pesquisarTexto(MatrizTexto m, const char *palavra) {
    int encontrada = 0;
    for (int i = 0; i < m.linhas_usadas; i++) {
        if (strstr(m.linhas[i], palavra) != NULL) {
            printf("Linha %d: %s\n", i, m.linhas[i]);
            encontrada++;
        }
    }
    if (encontrada == 0)
        printf("Nenhuma linha contem \"%s\".\n", palavra);
    else
        printf("%d linha(s) encontradas.\n", encontrada);
}

int removerLinha(MatrizTexto *m, int indice) {
    if (indice < 0 || indice >= m->linhas_usadas) {
        printf("Indice fora dos limites!\n");
        return 0;
    }
    free(m->linhas[indice]);
    for (int i = indice; i < m->linhas_usadas - 1; i++) {
        m->linhas[i] = m->linhas[i + 1];
    }
    m->linhas_usadas--;
    printf("Linha %d removida.\n", indice);
    return 1;
}

int removerLinhaPorConteudo(MatrizTexto *m, const char *frase) {
    for (int i = 0; i < m->linhas_usadas; i++) {
        if (strcmp(m->linhas[i], frase) == 0) {
            return removerLinha(m, i);
        }
    }
    printf("Nenhuma linha igual a \"%s\" foi encontrada.\n", frase);
    return 0;
}

// =============================================================
//  Funcoes auxiliares de tokens
// =============================================================
MatrizTokens criarMatrizTokens(int capacidade_inicial) {
    MatrizTokens t;
    t.tokens = malloc(capacidade_inicial * sizeof(char*));
    if (t.tokens == NULL) {
        printf("Erro: sem memoria para tokens.\n");
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
            printf("Erro: sem memoria ao redimensionar tokens.\n");
            return 0;
        }
        t->tokens = novos;
        t->capacidade = nova_cap;
        printf("Tokens redimensionados para capacidade %d.\n", nova_cap);
    }

    t->tokens[t->usados] = malloc(strlen(token) + 1);
    strcpy(t->tokens[t->usados], token);
    t->usados++;
    return 1;
}

void listarTokens(MatrizTokens t) {
    printf("\nTokens armazenados (%d):\n", t.usados);
    for (int i = 0; i < t.usados; i++) {
        printf("[%d] %s\n", i, t.tokens[i]);
    }
}

int removerToken(MatrizTokens *t, int indice) {
    if (indice < 0 || indice >= t->usados) {
        printf("Indice de token fora dos limites!\n");
        return 0;
    }

    free(t->tokens[indice]);

    for (int i = indice; i < t->usados - 1; i++) {
        t->tokens[i] = t->tokens[i + 1];
    }

    t->usados--;
    printf("Token %d removido com sucesso.\n", indice);
    return 1;
}

int removerTokenPorConteudo(MatrizTokens *t, const char *token) {
    for (int i = 0; i < t->usados; i++) {
        if (strcmp(t->tokens[i], token) == 0) {
            return removerToken(t, i);
        }
    }
    printf("Token \"%s\" nao encontrado.\n", token);
    return 0;
}

void libertarTokens(MatrizTokens t) {
    for (int i = 0; i < t.usados; i++) {
        free(t.tokens[i]);
    }
    free(t.tokens);
}

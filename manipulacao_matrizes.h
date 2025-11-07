#ifndef MANIPULACAO_MATRIZES_H
#define MANIPULACAO_MATRIZES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// =============================================================
//  Estruturas
// =============================================================
typedef struct {
    char **linhas;
    int linhas_usadas;
    int capacidade;
} MatrizTexto;

typedef struct {
    char **tokens;
    int usados;
    int capacidade;
} MatrizTokens;

// =============================================================
//  Manipulação de MatrizTexto
// =============================================================
MatrizTexto criarMatrizTexto(int capacidade_inicial);
int redimensionarMatriz(MatrizTexto *m, int nova_capacidade);
int adicionarLinha(MatrizTexto *m, const char *frase);
void listarMatriz(MatrizTexto m);
void libertarMatriz(MatrizTexto m);
void pesquisarTexto(MatrizTexto m, const char *palavra);
int removerLinha(MatrizTexto *m, int indice);
int removerLinhaPorConteudo(MatrizTexto *m, const char *frase);

// =============================================================
//  Manipulação de MatrizTokens
// =============================================================
MatrizTokens criarMatrizTokens(int capacidade_inicial);
int adicionarToken(MatrizTokens *t, const char *token);
void listarTokens(MatrizTokens t);
int removerToken(MatrizTokens *t, int indice);
int removerTokenPorConteudo(MatrizTokens *t, const char *token);
void libertarTokens(MatrizTokens t);

#endif

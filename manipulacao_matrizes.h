#ifndef MATRIZ_UTILS_H
#define MATRIZ_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
//  Funcoes R1.1 e R1.2 - MatrizTexto
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
//  Funcoes R1.1 e R1.2 - MatrizTokens
// =============================================================

MatrizTokens criarMatrizTokens(int capacidade_inicial);
int adicionarToken(MatrizTokens *t, const char *token);
void listarTokens(MatrizTokens t);
int removerToken(MatrizTokens *t, int indice);
int removerTokenPorConteudo(MatrizTokens *t, const char *token);
void libertarTokens(MatrizTokens t);

#endif //MATRIZ_UTILS_H
#ifndef MANIPULACAO_MATRIZES_H
#define MANIPULACAO_MATRIZES_H

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
//  Funcoes R1.1 e R1.2
// =============================================================

MatrizTexto criarMatrizTexto(int capacidade_inicial);
int redimensionarMatriz(MatrizTexto *m, int nova_capacidade);
int adicionarLinha(MatrizTexto *m, const char *frase);
void listarMatriz(MatrizTexto m);
void libertarMatriz(MatrizTexto m);
void pesquisarTexto(MatrizTexto m, const char *palavra);
int removerLinha(MatrizTexto *m, int indice);
int removerLinhaPorConteudo(MatrizTexto *m, const char *frase);

MatrizTokens criarMatrizTokens(int capacidade_inicial);
int adicionarToken(MatrizTokens *t, const char *token);
void listarTokens(MatrizTokens t);
int removerToken(MatrizTokens *t, int indice);
int removerTokenPorConteudo(MatrizTokens *t, const char *token);
void libertarTokens(MatrizTokens t);

// =============================================================
//  Funcoes R1.3 (BPE)
// =============================================================

int inicializarTokensUnicos(MatrizTexto m_texto, MatrizTokens *m_tokens);

int encontrarParMaisFrequente(MatrizTexto m_texto,
                              MatrizTokens m_tokens,
                              int *freq_max_out,
                              char **t1_out,
                              char **t2_out);

int aplicarFusaoNoCorpus(MatrizTexto *m_texto,
                         const char *token1,
                         const char *token2,
                         const char *token_fundido);

int calcularAlfabetoTokens(MatrizTexto *texto_corpus,
                           MatrizTokens *alfabeto_tokens,
                           int num_tokens_desejado);

// =============================================================
//  Funcoes R1.4 (ADICIONA ESTA LINHA)
// =============================================================
int* tokenizarFrase(const char *frase, MatrizTokens tokens, int *n_ids);

// =============================================================
//  Funcoes R1.5
// =============================================================
int* calcularFrequenciaTokens(MatrizTexto m_texto,
                              MatrizTokens m_tokens,
                              int *num_frequencias_out);

int* calcularTfParaFrase(int *ids_frase, int num_ids_frase, int tamanho_vocabulario);

#endif
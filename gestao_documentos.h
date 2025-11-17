#ifndef GESTAO_DOCUMENTOS_H
#define GESTAO_DOCUMENTOS_H

#include "manipulacao_matrizes.h"
#include "vetorizacao.h"

// =============================================================
//  R2.2: Estrutura MATRIX_INT (Matriz Dinâmica de Inteiros)
// =============================================================
typedef struct {
    int **linhas;       // Array de arrays de ints
    int *tamanho_linhas;// Array que guarda o tamanho de cada linha
    int linhas_usadas;
    int capacidade;
} MATRIX_INT;

// =============================================================
//  R2.1: Estruturas da Lista Ligada (LL_TK_TF)
// =============================================================
typedef struct tk_tf_node {
    int *ids;           // Vetor de IDs da frase (R1.4)
    int tam_ids;        // Tamanho do vetor de IDs

    int *tf;            // Vetor TF da frase (R1.5)
    int tam_vocab;      // Tamanho do vetor TF (vocabulario)

    struct tk_tf_node *proximo;
    struct tk_tf_node *anterior;
} TK_TF_NODE;

typedef struct {
    TK_TF_NODE *cabeca;
    TK_TF_NODE *cauda;
    int quantidade_nos;
} LL_TK_TF;

// =============================================================
//  R2.2: Estrutura DOC (Agregador Principal)
// =============================================================
typedef struct {
    MatrizTexto texto;          // As frases originais (Strings)
    MATRIX_INT ids_tokenizados; // As frases em IDs (Ints)
    LL_TK_TF lista_processada;  // As frases em Nós (Lista Ligada com TF)

    MatrizTokens tokens;        // O vocabulário associado a este documento
} DOC;

// --- Funções de Gestão da MATRIX_INT ---
MATRIX_INT criarMatrixInt(int capacidade_inicial);
int adicionarLinhaInt(MATRIX_INT *m, int *vetor_ids, int num_ids);
void libertarMatrixInt(MATRIX_INT m);
void listarMatrixInt(MATRIX_INT m);

// --- Funções de Gestão da Lista Ligada (R2.1) ---
LL_TK_TF criarListaLigada();
int adicionarNoLista(LL_TK_TF *lista, int *ids, int n_ids, int *tf, int n_vocab);
void libertarListaLigada(LL_TK_TF *lista);
void listarListaLigada(LL_TK_TF lista);

// --- Funções do Documento (R2.2) ---
DOC* criarDocumento();
void libertarDocumento(DOC *doc);

/**
 * @brief Adiciona uma frase ao documento, preenchendo automaticamente:
 * 1. MatrizTexto (Texto original)
 * 2. MATRIX_INT (IDs tokenizados)
 * 3. LL_TK_TF (Lista com IDs e TF)
 */
void adicionarFraseAoDocumento(DOC *doc, const char *frase);

#endif // GESTAO_DOCUMENTOS_H
//
// Created by guilh on 16/11/2025.
//
#ifndef GESTAO_DOCUMENTOS_H
#define GESTAO_DOCUMENTOS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "manipulacao_matrizes.h"
#include "vetorizacao.h"

// =============================================================
//  R2.2: A "Caixa" (t_doc)
// =============================================================
typedef struct t_doc {
    char *titulo;
    char *texto_original;

    int *ids_tokens;        // Resultado R1.4
    int num_ids;
    int *vetor_tf;          // Resultado R1.5
    int vocab_size;

    struct t_doc *proximo;  // "Fio" para a próxima caixa
} t_doc;


// =============================================================
//  R2.1: A "Prateleira" (t_lista_docs)
// =============================================================
typedef struct {
    t_doc *inicio;              // Ponteiro para a primeira caixa
    int num_documentos;
    MatrizTokens *vocabulario;  // O Alfabeto (essencial para os cálculos)
} t_lista_docs;


// =============================================================
//  Funções R2.1 e R2.2
// =============================================================

// Cria a Prateleira
t_lista_docs criarListaDocumentos(MatrizTokens *vocab);

// Fabrica uma Caixa, faz os cálculos, e arruma na Prateleira
int adicionarDocumentoLista(t_lista_docs *lista, const char *titulo, const char *texto_original);

// Mostra o que está na Prateleira
void listarDocumentos(t_lista_docs lista);

// Destrói a Prateleira e as Caixas
void libertarListaDocumentos(t_lista_docs *lista);

#endif //GESTAO_DOCUMENTOS_H
#ifndef GESTAO_DOCUMENTOS_H
#define GESTAO_DOCUMENTOS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "manipulacao_matrizes.h"
#include "vetorizacao.h"

// =============================================================
//  Estruturas de Dados (Mantidas do teu código)
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

typedef struct {
    t_doc *inicio;              // Ponteiro para a primeira caixa
    int num_documentos;
    MatrizTokens *vocabulario;  // O Alfabeto
} t_lista_docs;


// =============================================================
//  Funções R2.1 e R2.2
// =============================================================

// Cria a Prateleira
t_lista_docs criarListaDocumentos(MatrizTokens *vocab);

// Adiciona no fim (já tens esta)
int adicionarDocumentoLista(t_lista_docs *lista, const char *titulo, const char *texto_original);

// Mostra o que está na Prateleira
void listarDocumentos(t_lista_docs lista);

// Destrói a Prateleira
void libertarListaDocumentos(t_lista_docs *lista);

// --- NOVAS FUNÇÕES (Para completar o R2.1) ---

// Obter um documento numa posição especifica (apenas leitura)
t_doc* obterDocumentoPorIndice(t_lista_docs lista, int indice);

// Inserir um documento numa posição especifica (0 = inicio)
int inserirDocumentoNoIndice(t_lista_docs *lista, const char *titulo, const char *texto, int indice);

// Remover um documento numa posição especifica
int removerDocumentoPorIndice(t_lista_docs *lista, int indice);

// Pesquisar documentos que contêm um ID de token especifico
void pesquisarDocumentosPorTokenID(t_lista_docs lista, int token_id);

#endif //GESTAO_DOCUMENTOS_H
#ifndef GESTAO_DOCUMENTOS_H
#define GESTAO_DOCUMENTOS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "manipulacao_matrizes.h"
#include "vetorizacao.h"

/**
 * @brief Estrutura que representa um Documento (Nó da Lista Ligada).
 * Contém a informação textual, a versão tokenizada e vetorial.
 * [R2.1] Nome alterado de t_doc para TK_TF_NODE conforme requisito.
 */
typedef struct TK_TF_NODE {
    char *titulo;           /**< Nome do ficheiro ou título do documento */
    char *texto_original;   /**< O conteúdo integral do texto */

    int *ids_tokens;        /**< Array de IDs após a tokenização (R1.4) */
    int num_ids;            /**< Tamanho do array ids_tokens */

    int *vetor_tf;          /**< Vetor de frequências (Histograma) (R1.5) */
    int vocab_size;         /**< Tamanho do vocabulário no momento da criação */

    struct TK_TF_NODE *proximo;  /**< Ponteiro para o próximo documento na lista */
} TK_TF_NODE;

/**
 * @brief Estrutura que representa a Lista Ligada de Documentos.
 * Contém o ponteiro para o início da lista e uma referência partilhada para o vocabulário.
 * [R2.1] Nome alterado de t_lista_docs para LL_TK_TF conforme requisito.
 */
typedef struct {
    TK_TF_NODE *inicio;         /**< Ponteiro para o primeiro nó (Head) */
    int num_documentos;         /**< Contador de documentos na lista */
    MatrizTokens *vocabulario;  /**< Ponteiro para o alfabeto usado nestes documentos */
} LL_TK_TF;


// =============================================================
//  Funções de Gestão de Documentos
// =============================================================

/**
 * @brief Inicializa uma lista de documentos vazia.
 * @param vocab Ponteiro para o vocabulário que será usado na tokenização dos documentos.
 * @return LL_TK_TF Lista inicializada.
 */
LL_TK_TF criarListaDocumentos(MatrizTokens *vocab);

/**
 * @brief Adiciona um documento ao final da lista.
 * Esta função chama internamente as rotinas de tokenização e vetorização.
 * @param lista Ponteiro para a lista.
 * @param titulo Título do documento.
 * @param texto_original Conteúdo do documento.
 * @return int 1 se sucesso, 0 se erro.
 * @note **Complexidade:** O(N) para percorrer a lista até ao fim + custo da tokenização.
 */
int adicionarDocumentoLista(LL_TK_TF *lista, const char *titulo, const char *texto_original);

/**
 * @brief Percorre a lista e imprime os dados básicos de cada documento.
 * @param lista A lista a imprimir.
 */
void listarDocumentos(LL_TK_TF lista);

/**
 * @brief Liberta toda a memória da lista (incluindo todos os nós e arrays internos).
 * @param lista Ponteiro para a lista a destruir.
 */
void libertarListaDocumentos(LL_TK_TF *lista);

// --- NOVAS FUNÇÕES (R2.1) ---

/**
 * @brief Retorna um ponteiro para o documento num determinado índice.
 * @param lista A lista de documentos.
 * @param indice A posição desejada (base 0).
 * @return TK_TF_NODE* Ponteiro para o nó ou NULL se o índice for inválido.
 * @note **Complexidade:** O(K), onde K é o índice solicitado (acesso sequencial).
 */
TK_TF_NODE* obterDocumentoPorIndice(LL_TK_TF lista, int indice);

/**
 * @brief Insere um novo documento numa posição específica da lista.
 * @param lista Ponteiro para a lista.
 * @param titulo Título do novo documento.
 * @param texto Conteúdo do novo documento.
 * @param indice Posição onde inserir (0 = início, N = fim).
 * @return int 1 se sucesso, 0 se erro ou índice inválido.
 * @note **Complexidade:** O(K) para chegar à posição de inserção.
 */
int inserirDocumentoNoIndice(LL_TK_TF *lista, const char *titulo, const char *texto, int indice);

/**
 * @brief Remove e liberta a memória de um documento numa posição específica.
 * @param lista Ponteiro para a lista.
 * @param indice Índice do documento a remover.
 * @return int 1 se sucesso, 0 se erro.
 */
int removerDocumentoPorIndice(LL_TK_TF *lista, int indice);

/**
 * @brief Pesquisa em toda a lista quais os documentos que contêm um determinado Token ID.
 * Utiliza o vetor TF para verificar se a frequência é > 0.
 * @param lista A lista de documentos.
 * @param token_id O ID do token a pesquisar.
 * @note **Complexidade:** O(N), onde N é o número de documentos na lista.
 */
void pesquisarDocumentosPorTokenID(LL_TK_TF lista, int token_id);

#endif //GESTAO_DOCUMENTOS_H
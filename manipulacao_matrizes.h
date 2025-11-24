#ifndef MATRIZ_UTILS_H
#define MATRIZ_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Estrutura para armazenar um conjunto de strings (linhas de texto).
 * Utiliza um array dinâmico de strings (char**).
 */
typedef struct {
    char **linhas;      /**< Array de ponteiros para strings */
    int linhas_usadas;  /**< Número atual de strings armazenadas */
    int capacidade;     /**< Capacidade total alocada antes de precisar de resize */
} MatrizTexto;

/**
 * @brief Estrutura para armazenar o vocabulário (tokens).
 * Funciona de forma idêntica à MatrizTexto, mas semanticamente separa tokens de frases.
 */
typedef struct {
    char **tokens;      /**< Array de ponteiros para os tokens */
    int usados;         /**< Número de tokens registados */
    int capacidade;     /**< Capacidade total alocada */
} MatrizTokens;

// =============================================================
//  Funcoes R1.1 e R1.2 - MatrizTexto
// =============================================================

/**
 * @brief Cria uma nova matriz de texto com uma capacidade inicial.
 * @param capacidade_inicial Tamanho inicial do array dinâmico.
 * @return MatrizTexto Estrutura inicializada.
 */
MatrizTexto criarMatrizTexto(int capacidade_inicial);

/**
 * @brief Redimensiona a matriz de texto quando esta fica cheia.
 * Utiliza `realloc` para aumentar a capacidade do array de ponteiros.
 * @param m Ponteiro para a matriz a redimensionar.
 * @param nova_capacidade O novo tamanho total desejado.
 * @return int 1 em caso de sucesso, 0 se falhar a alocação de memória.
 * @note **Complexidade:** O(N) onde N é o número de linhas, devido à cópia de ponteiros pelo realloc.
 */
int redimensionarMatriz(MatrizTexto *m, int nova_capacidade);

/**
 * @brief Adiciona uma nova frase à matriz.
 * Se a matriz estiver cheia, invoca automaticamente o redimensionamento.
 * @param m Ponteiro para a matriz.
 * @param frase A string a adicionar (será feita uma cópia dinâmica com strdup/malloc).
 * @return int 1 se sucesso, 0 se erro.
 * @note **Complexidade:** O(1) amortizado (devido ao redimensionamento ocasional).
 */
int adicionarLinha(MatrizTexto *m, const char *frase);

/**
 * @brief Imprime todas as linhas armazenadas na consola.
 * @param m A matriz a listar.
 */
void listarMatriz(MatrizTexto m);

/**
 * @brief Liberta toda a memória associada à matriz.
 * Percorre cada linha libertando a string e, no fim, liberta o array de ponteiros.
 * @param m A matriz a destruir.
 */
void libertarMatriz(MatrizTexto m);

/**
 * @brief Pesquisa por uma substring dentro de todas as linhas da matriz.
 * Imprime as linhas onde a palavra ocorre.
 * @param m A matriz de texto.
 * @param palavra O termo a pesquisar (substring).
 * @note **Complexidade:** O(L * S), onde L é o nº de linhas e S o tamanho médio das strings (uso de strstr).
 */
void pesquisarTexto(MatrizTexto m, const char *palavra);

/**
 * @brief Remove uma linha num índice específico.
 * Desloca todas as linhas subsequentes para a esquerda para tapar o buraco.
 * @param m Ponteiro para a matriz.
 * @param indice O índice da linha a remover.
 * @return int 1 se sucesso, 0 se índice inválido.
 * @note **Complexidade:** O(N) no pior caso, pois necessita de deslocar elementos.
 */
int removerLinha(MatrizTexto *m, int indice);

/**
 * @brief Remove a primeira linha encontrada que seja idêntica ao conteúdo fornecido.
 * @param m Ponteiro para a matriz.
 * @param frase A frase exata a remover.
 * @return int 1 se removeu, 0 se não encontrou.
 */
int removerLinhaPorConteudo(MatrizTexto *m, const char *frase);

// =============================================================
//  Funcoes R1.1 e R1.2 - MatrizTokens
// =============================================================

/**
 * @brief Cria uma matriz de tokens (Vocabulário).
 * @param capacidade_inicial Capacidade inicial.
 * @return MatrizTokens Estrutura inicializada.
 */
MatrizTokens criarMatrizTokens(int capacidade_inicial);

/**
 * @brief Adiciona um token ao vocabulário.
 * @param t Ponteiro para a matriz de tokens.
 * @param token String do token a adicionar.
 * @return int 1 sucesso, 0 erro.
 */
int adicionarToken(MatrizTokens *t, const char *token);

/**
 * @brief Lista todos os tokens e os seus índices (IDs).
 * @param t Matriz de tokens.
 */
void listarTokens(MatrizTokens t);

/**
 * @brief Remove um token pelo seu índice (ID).
 * @warning Ao remover um token, os IDs dos tokens seguintes mudam!
 * @param t Ponteiro para a matriz.
 * @param indice Índice a remover.
 * @return int 1 sucesso, 0 erro.
 */
int removerToken(MatrizTokens *t, int indice);

/**
 * @brief Remove um token pelo seu conteúdo textual.
 * @param t Ponteiro para a matriz.
 * @param token String do token.
 * @return int 1 sucesso, 0 erro.
 */
int removerTokenPorConteudo(MatrizTokens *t, const char *token);

/**
 * @brief Liberta a memória da matriz de tokens.
 * @param t Matriz a libertar.
 */
void libertarTokens(MatrizTokens t);

#endif //MATRIZ_UTILS_H
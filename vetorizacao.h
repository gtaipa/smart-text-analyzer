#ifndef VETORIZACAO_H
#define VETORIZACAO_H

#include "manipulacao_matrizes.h"
#include "bpe.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//  Funcoes R1.4 (Tokenizacao)
/**
 * @brief Converte uma string numa sequência de IDs de tokens usando "Greedy Longest-Match".
 * Percorre a frase e, em cada posição, tenta encontrar o maior token possível do vocabulário
 * que coincida com o texto atual.
 * @param frase A string de entrada.
 * @param tokens O vocabulário disponível.
 * @param n_ids Ponteiro de saída para armazenar o número de tokens encontrados.
 * @return int* Array dinâmico contendo os IDs dos tokens (deve ser libertado com free).
 * @note **Complexidade:** O(L * V), onde L é o tamanho da frase e V o tamanho do vocabulário,
 * pois testa todos os tokens em cada posição.
 */
int* tokenizarFrase(const char *frase, MatrizTokens tokens, int *n_ids);

//  Funcoes R1.5 (Vetorizacao TF)

/**
 * @brief Calcula as frequências absolutas de cada token num corpus inteiro.
 * @param m_texto O conjunto de textos.
 * @param m_tokens O vocabulário.
 * @param num_frequencias_out Ponteiro para retornar o tamanho do array.
 * @return int* Array com as frequências globais.
 */
int* calcularFrequenciaTokens(MatrizTexto m_texto,
                              MatrizTokens m_tokens,
                              int *num_frequencias_out);

/**
 * @brief Cria o vetor TF (Term Frequency) para uma única frase.
 * Transforma o array de IDs num histograma onde o índice é o ID do token e o valor é a frequência.
 * @param ids_frase Array de IDs retornado pela tokenização.
 * @param num_ids_frase Número de elementos no array de IDs.
 * @param tamanho_vocabulario Tamanho total do vocabulário (dimensão do vetor resultante).
 * @return int* Array de inteiros (histograma) de tamanho `tamanho_vocabulario`.
 * @note **Complexidade:** O(N), onde N é o número de tokens na frase.
 */
int* calcularTfParaFrase(int *ids_frase, int num_ids_frase, int tamanho_vocabulario);

#endif //VETORIZACAO_H
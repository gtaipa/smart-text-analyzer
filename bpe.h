#ifndef BPE_H
#define BPE_H

#include "manipulacao_matrizes.h"

// =============================================================
//  Funções R1.3 (BPE - Byte Pair Encoding)
// =============================================================

/**
 * @brief Inicializa o alfabeto com todos os caracteres únicos presentes no texto.
 * * Esta função percorre todo o corpus de texto e identifica os caracteres individuais
 * (bytes) únicos. Cada caractere único é adicionado como um token inicial ao vocabulário.
 * É o primeiro passo do algoritmo BPE.
 * * @param m_texto A matriz contendo o corpus de texto original.
 * @param m_tokens Ponteiro para a matriz de tokens onde os caracteres serão guardados.
 * @return int Retorna 1 em caso de sucesso, 0 se houver erro de alocação.
 * * @note **Complexidade:** O(N), onde N é o número total de caracteres no texto, pois
 * faz uma leitura linear completa do corpus.
 */
int inicializarTokensUnicos(MatrizTexto m_texto, MatrizTokens *m_tokens);

/**
 * @brief Encontra o par de tokens consecutivos mais frequente no texto.
 * * Analisa o texto para contar a frequência de todos os pares de tokens adjacentes.
 * Na implementação atual (simplificada), foca-se em pares de caracteres/tokens
 * para decidir qual será a próxima fusão.
 * * @param m_texto A matriz contendo o texto atual (pode já ter sofrido fusões).
 * @param m_tokens A lista atual de tokens disponíveis.
 * @param freq_max Ponteiro de saída para guardar a frequência do par vencedor.
 * @return char* Retorna uma nova string contendo o par vencedor concatenado (ex: "ar"),
 * ou NULL se não houver pares elegíveis.
 * * @note **Complexidade:** Alta (aproximadamente O(T^2 * L)), onde T é o número de tokens
 * e L o tamanho do texto, devido aos loops aninhados de procura.
 */
char *encontrarParMaisFrequente(MatrizTexto m_texto, MatrizTokens m_tokens, int *freq_max);

/**
 * @brief Aplica a fusão de um par de tokens em todo o corpus de texto.
 * * Percorre todas as linhas do texto e substitui todas as ocorrências da sequência
 * `token1` seguido de `token2` pelo novo token fundido. As linhas de texto são
 * realocadas para acomodar as alterações.
 * * @param m_texto Ponteiro para a matriz de texto a ser modificada.
 * @param token1 O primeiro token do par (ex: "a").
 * @param token2 O segundo token do par (ex: "r").
 * @return int Retorna o número total de fusões realizadas, ou -1 em caso de erro.
 * * @note **Complexidade:** O(L), onde L é o comprimento total do texto, pois envolve
 * scanning linear e reconstrução das strings.
 */
int aplicarFusaoNoCorpus(MatrizTexto *m_texto, const char *token1, const char *token2);

/**
 * @brief Algoritmo Principal BPE (Byte Pair Encoding).
 * * Executa o processo iterativo de construção do vocabulário:
 * 1. Inicializa o vocabulário com caracteres únicos.
 * 2. Enquanto o número de tokens for menor que o desejado:
 * - [cite_start]Encontra o par mais frequente[cite: 153].
 * - Adiciona esse par ao vocabulário.
 * - [cite_start]Atualiza o corpus fundindo esse par[cite: 154].
 * * @param texto_corpus Ponteiro para o texto de entrada (será modificado durante o processo).
 * @param alfabeto_tokens Ponteiro para a matriz de tokens (resultado final).
 * [cite_start]@param num_tokens_desejado Critério de paragem (tamanho final do vocabulário)[cite: 155].
 * @return int Retorna 1 se o processo concluir com sucesso, 0 caso contrário.
 * * @note **Complexidade Global:** Depende do número de fusões (K). Aproximadamente O(K * Custo_Procura),
 * o que torna o algoritmo intensivo computacionalmente para textos grandes.
 */

#endif //BPE_H
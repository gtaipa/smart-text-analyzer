#ifndef BPE_H
#define BPE_H

#include "manipulacao_matrizes.h"

// =============================================================
//  Funções R1.3 (BPE - Byte Pair Encoding)
// =============================================================

/**
 * @brief Algoritmo Principal BPE (Byte Pair Encoding).
 * * Executa o processo completo:
 * 1. Inicializa o vocabulário com letras individuais.
 * 2. Converte o texto para Listas Ligadas.
 * 3. Num loop: encontra o par mais frequente e funde-o.
 * * @param texto_corpus Ponteiro para o texto de entrada (será modificado).
 * @param alfabeto_tokens Ponteiro para a matriz de tokens (resultado final).
 * @param num_tokens_desejado Quantos tokens queres descobrir no total.
 * @return int Retorna 1 se sucesso, 0 se erro.
 */
int calcularAlfabetoTokens(MatrizTexto *texto_corpus, MatrizTokens *alfabeto_tokens, int num_tokens_desejado);

// Funções auxiliares que podem ser usadas isoladamente (opcionais)
int inicializarTokensUnicos(MatrizTexto m_texto, MatrizTokens *m_tokens);

// Mantida apenas para compatibilidade (caso algum teste antigo a chame)
int aplicarFusaoNoCorpus(MatrizTexto *m_texto, const char *token1, const char *token2);

#endif
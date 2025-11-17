#ifndef BPE_H
#define BPE_H

#include "manipulacao_matrizes.h" // Precisa das structs

// =============================================================
//  Funções R1.3 (BPE)
// =============================================================

int inicializarTokensUnicos(MatrizTexto m_texto, MatrizTokens *m_tokens);

char *encontrarParMaisFrequente(MatrizTexto m_texto, MatrizTokens m_tokens, int *freq_max);

int aplicarFusaoNoCorpus(MatrizTexto *m_texto, const char *token1, const char *token2);

int calcularAlfabetoTokens(MatrizTexto *texto_corpus,
                           MatrizTokens *alfabeto_tokens,
                           int num_tokens_desejado);

#endif //BPE_H
#ifndef VETORIZACAO_H
#define VETORIZACAO_H

#include "manipulacao_matrizes.h"
#include "bpe.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// =============================================================
//  Funcoes R1.4 (Tokenizacao)
// =============================================================
int* tokenizarFrase(const char *frase, MatrizTokens tokens, int *n_ids);

// =============================================================
//  Funcoes R1.5 (Vetorizacao)
// =============================================================
int* calcularFrequenciaTokens(MatrizTexto m_texto,
                              MatrizTokens m_tokens,
                              int *num_frequencias_out);

int* calcularTfParaFrase(int *ids_frase, int num_ids_frase, int tamanho_vocabulario);


#endif //VETORIZACAO_H
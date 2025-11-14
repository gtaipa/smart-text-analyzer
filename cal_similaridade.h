#ifndef SIMILARIDADE_H
#define SIMILARIDADE_H
#include <math.h>
#include "manipulacao_matrizes.h"
#include "bpe.h"
#include "vetorizacao.h"
// =============================================================
//  Funcoes R1.6 (Similaridade)
// =============================================================

/**
 * @brief Calcula o Produto Interno entre dois vetores TF.
 */
double calcularProdutoInterno(int *vetorA, int *vetorB, int tamanhoVocabulario);

/**
 * @brief Calcula a Similaridade de Cosseno entre dois vetores TF.
 */
double calcularSimilaridadeCosseno(int *vetorA, int *vetorB, int tamanhoVocabulario);

#endif //SIMILARIDADE_H
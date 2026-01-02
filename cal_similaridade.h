#ifndef SIMILARIDADE_H
#define SIMILARIDADE_H

#include <math.h>
#include "manipulacao_matrizes.h"
#include "bpe.h"
#include "vetorizacao.h"

//  Funcoes R1.6 (Similaridade)


/**
 * @brief Calcula o Produto Interno (Dot Product) entre dois vetores TF.
 * Soma do produto dos elementos correspondentes: sum(A[i] * B[i]).
 * @param vetorA Primeiro vetor.
 * @param vetorB Segundo vetor.
 * @param tamanhoVocabulario Dimensão dos vetores.
 * @return double Resultado do produto interno.
 * @note **Complexidade:** O(N), onde N é o tamanho do vocabulário.
 */
double calcularProdutoInterno(int *vetorA, int *vetorB, int tamanhoVocabulario);

/**
 * @brief Calcula a Similaridade de Cosseno entre dois vetores TF.
 * Fórmula: (A . B) / (||A|| * ||B||).
 * Mede o cosseno do ângulo entre dois vetores num espaço multidimensional.
 * @param vetorA Primeiro vetor.
 * @param vetorB Segundo vetor.
 * @param tamanhoVocabulario Dimensão dos vetores.
 * @return double Valor entre 0.0 (ortogonais/diferentes) e 1.0 (idênticos em direção).
 * @note **Complexidade:** O(N), percorre os vetores linearmente para calcular normas e produto.
 */
double calcularSimilaridadeCosseno(int *vetorA, int *vetorB, int tamanhoVocabulario);

#endif //SIMILARIDADE_H
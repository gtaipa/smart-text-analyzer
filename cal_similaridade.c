#include "cal_similaridade.h"


/**
 * @brief Funcao auxiliar (privada) para calcular a magnitude (norma L2).
 * Nota: 'static' torna-a privada a este ficheiro .c
 */
static double calcularMagnitude(int *vetor, int tamanhoVocabulario) {
    double soma_quadrados = 0.0;

    // 1. Fazer a SOMA dos quadrados
    for(int i=0; i<tamanhoVocabulario; i++) {
        soma_quadrados += (double)vetor[i] * (double)vetor[i];
    }

    // 2. Calcular a raiz quadrada UMA VEZ, no fim.
    return sqrt(soma_quadrados);
}

// =============================================================
//  Funcoes R1.6 (Publicas)
// =============================================================

double calcularProdutoInterno(int *vetorA, int *vetorB, int tamanhoVocabulario) {
    double soma = 0.0;
    for (int i = 0; i < tamanhoVocabulario; i++) {
        soma += (double)vetorA[i] * (double)vetorB[i];
    }
    return soma;
}


double calcularSimilaridadeCosseno(int *vetorA, int *vetorB, int tamanhoVocabulario) {
    
    double produto_interno = calcularProdutoInterno(vetorA, vetorB, tamanhoVocabulario);
    
    // Chama a funcao auxiliar 'static'
    double magnitudeA = calcularMagnitude(vetorA, tamanhoVocabulario);
    double magnitudeB = calcularMagnitude(vetorB, tamanhoVocabulario);
    
    // Evitar divisao por zero se um dos vetores for nulo
    if (magnitudeA == 0.0 || magnitudeB == 0.0) {
        return 0.0;
    }
    
    return produto_interno / (magnitudeA * magnitudeB);
}
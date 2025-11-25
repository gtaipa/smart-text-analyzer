#include "cal_similaridade.h"


/**
 * @brief Funcao auxiliar (privada) para calcular a magnitude (norma L2).
 * Nota: 'static' torna-a privada a este ficheiro .c
 */
static double calcularMagnitude(int *vetor, int tamanhoVocabulario) {
    double soma_quadrados = 0.0;
    // Fazer a SOMA dos quadrados
    for(int i=0; i<tamanhoVocabulario; i++) {
        soma_quadrados += (double)vetor[i] * (double)vetor[i];
    }
    //Calcular a raiz quadrada de acordo com a formula.
    return sqrt(soma_quadrados);
}

// =============================================================
//  Funcoes R1.6 (Publicas)
// =============================================================

double calcularProdutoInterno(int *vetorA, int *vetorB, int tamanhoVocabulario) {//soma o produto dos conteudos presentes nas varias posicoes dos vetores
    double soma = 0.0;
    for (int i = 0; i < tamanhoVocabulario; i++) {
        soma += (double)vetorA[i] * (double)vetorB[i];
    }
    return soma;
}


double calcularSimilaridadeCosseno(int *vetorA, int *vetorB, int tamanhoVocabulario) {//usamos as funcoes anteriores para simplificar o processo
    double produto_interno = calcularProdutoInterno(vetorA, vetorB, tamanhoVocabulario);
    double magnitudeA = calcularMagnitude(vetorA, tamanhoVocabulario);
    double magnitudeB = calcularMagnitude(vetorB, tamanhoVocabulario);
    // Evitar divisao por zero se um dos vetores for nulo
    if (magnitudeA == 0.0 || magnitudeB == 0.0) {
        return 0.0;
    }
    return produto_interno / (magnitudeA * magnitudeB);//retornar o resultado que so faltava fazer a divisao
}
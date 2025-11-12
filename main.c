#include "manipulacao_matrizes.h"
#include <stdio.h>

void testar_R1_1_e_R1_2() {
    printf("=== Testes R1.1 e R1.2: Manipulacao de Matrizes ===\n");

    printf("\n[1] Criar matriz de texto com capacidade para 3 linhas...\n");
    MatrizTexto texto = criarMatrizTexto(3);
    adicionarLinha(&texto, "the cat sleeps");
    adicionarLinha(&texto, "the dog runs");
    adicionarLinha(&texto, "a cat eats");

    printf("\n[2] Adicionar mais frases para forcar redimensionamento...\n");
    adicionarLinha(&texto, "extra line that triggers resize");
    listarMatriz(texto);

    printf("\n[3] Criar e listar tokens...\n");
    MatrizTokens tokens = criarMatrizTokens(4);
    adicionarToken(&tokens, "the");
    adicionarToken(&tokens, "cat");
    adicionarToken(&tokens, "dog");
    adicionarToken(&tokens, "runs");
    adicionarToken(&tokens, "eats");
    listarTokens(tokens);

    printf("\n[4] Teste de pesquisa...\n");
    pesquisarTexto(texto, "cat");
    pesquisarTexto(texto, "dog");
    pesquisarTexto(texto, "banana");

    printf("\n[5] Remover linha 1 (\"the dog runs\")...\n");
    removerLinha(&texto, 1);
    listarMatriz(texto);

    printf("\n[9] Libertar memoria...\n");
    libertarMatriz(texto);
    libertarTokens(tokens);

    printf("\n=== Testes R1.1 e R1.2 Concluidos ===\n\n");
}

void testar_R1_3() {
    printf("=== Teste R1.3: Calculo de Alfabeto (BPE) ===\n");

    MatrizTexto corpus_bpe = criarMatrizTexto(2);
    adicionarLinha(&corpus_bpe, "b a r   p a r");
    adicionarLinha(&corpus_bpe, "b a r");

    printf("Corpus inicial para BPE:\n");
    listarMatriz(corpus_bpe);

    MatrizTokens tokens_bpe = criarMatrizTokens(10);

    int num_tokens_desejado = 7;

    calcularAlfabetoTokens(&corpus_bpe, &tokens_bpe, num_tokens_desejado);

    printf("\n--- RESULTADO FINAL BPE (R1.3) ---\n");
    printf("Corpus final (modificado pelas fusoes):\n");
    listarMatriz(corpus_bpe);

    printf("\nAlfabeto final (%d tokens):\n", tokens_bpe.usados);
    listarTokens(tokens_bpe);

    libertarMatriz(corpus_bpe);
    libertarTokens(tokens_bpe);

    printf("\n=== Teste R1.3 Concluido ===\n");
}

void testar_R1_4() {
    printf("\n=== Teste R1.4: Tokenizacao (Greedy Longest-Match) ===\n");

    // 1. Criar o alfabeto de tokens de teste
    printf("[1] A criar alfabeto de teste (7 tokens)...\n");
    MatrizTokens tokens_teste = criarMatrizTokens(7);

    // A ordem e crucial para que os IDs correspondam ao exemplo
    adicionarToken(&tokens_teste, "b");   // ID 0
    adicionarToken(&tokens_teste, "a");   // ID 1
    adicionarToken(&tokens_teste, "r");   // ID 2
    adicionarToken(&tokens_teste, " ");  // ID 3
    adicionarToken(&tokens_teste, "p");   // ID 4
    adicionarToken(&tokens_teste, "ar");  // ID 5
    adicionarToken(&tokens_teste, "bar"); // ID 6

    listarTokens(tokens_teste);

    // 2. Definir a frase de teste
    const char *frase_teste = "bar par";
    printf("\n[2] A tokenizar a frase: \"%s\"\n", frase_teste);

    // 3. Chamar a funcao a ser testada
    int num_ids_resultante = 0;
    int *ids = tokenizarFrase(frase_teste, tokens_teste, &num_ids_resultante);

    // 4. Imprimir o resultado para validacao
    printf("[3] Resultado da Tokenizacao:\n");
    if (ids != NULL) {
        printf("    Vetor de IDs: [");
        for (int i = 0; i < num_ids_resultante; i++) {
            printf("%d", ids[i]);
            if (i < num_ids_resultante - 1) {
                printf(", ");
            }
        }
        printf("]\n");
        printf("    Total de IDs: %d\n", num_ids_resultante);
    } else {
        printf("    Erro: Funcao retornou NULL.\n");
    }

    // 5. Validacao contra o resultado esperado
    printf("[4] Validacao:\n");
    printf("    Resultado Esperado: [6, 3, 4, 5] (Total: 4 IDs)\n");

    // Verificacao automatica simples
    if (num_ids_resultante == 4 && ids[0] == 6 && ids[1] == 3 && ids[2] == 4 && ids[3] == 5) {
        printf("    Estado: SUCESSO!\n");
    } else {
        printf("    Estado: FALHOU!\n");
    }

    // 6. Libertar a memoria alocada para o teste
    printf("[5] A libertar memoria do teste...\n");
    free(ids); // Libertar o array de IDs retornado pela tokenizarFrase
    libertarTokens(tokens_teste); // Libertar a matriz de tokens de teste

    printf("=== Teste R1.4 Concluido ===\n");
}
void testar_R1_5() {
    printf("\n=== Teste R1.5: Vetorizacao TF (Term Frequency) ===\n");

    // --- Setup (igual ao teste R1.4) ---
    MatrizTokens tokens_teste = criarMatrizTokens(7);
    adicionarToken(&tokens_teste, "b");   // ID 0
    adicionarToken(&tokens_teste, "a");   // ID 1
    adicionarToken(&tokens_teste, "r");   // ID 2
    adicionarToken(&tokens_teste, " ");  // ID 3
    adicionarToken(&tokens_teste, "p");   // ID 4
    adicionarToken(&tokens_teste, "ar");  // ID 5
    adicionarToken(&tokens_teste, "bar"); // ID 6

    const char *frase_teste = "bar par";
    int num_ids_frase = 0;
    int *ids_da_frase = tokenizarFrase(frase_teste, tokens_teste, &num_ids_frase);

    if (ids_da_frase == NULL) {
        printf("Erro: Falhou o setup (R1.4). A cancelar teste R1.5.\n");
        libertarTokens(tokens_teste);
        return;
    }
    printf("[1] Setup (R1.4) concluido. IDs da frase: [6, 3, 4, 5]\n");
    // --- Fim do Setup ---


    // [2] Chamar a funcao a ser testada (R1.5)
    printf("[2] A chamar calcularTfParaFrase...\n");
    int tamanho_vocabulario = tokens_teste.usados; // = 7
    int *vetor_tf = calcularTfParaFrase(ids_da_frase, num_ids_frase, tamanho_vocabulario);

    // [3] Imprimir e validar o resultado
    printf("[3] Resultado (Vetor TF no formato 'ID:Frequencia'):\n");
    if (vetor_tf != NULL) {
        // Imprimir no formato "sparse" (so os que contam)
        // para ser igual ao exemplo do PDF
        printf("    Resultado Esperado: (6:1, 3:1, 4:1, 5:1)\n");
        printf("    Resultado Obtido:   ");
        int count_validos = 0;
        for (int i = 0; i < tamanho_vocabulario; i++) {
            if (vetor_tf[i] > 0) {
                printf("(%d:%d)", i, vetor_tf[i]);
                count_validos++;
                if (count_validos < num_ids_frase) {
                     printf(", ");
                }
            }
        }
        printf("\n");

        // Validacao
        if (vetor_tf[6] == 1 && vetor_tf[3] == 1 && vetor_tf[4] == 1 && vetor_tf[5] == 1) {
            printf("    Estado: SUCESSO!\n");
        } else {
            printf("    Estado: FALHOU!\n");
        }

    } else {
        printf("    Erro: Funcao retornou NULL.\n");
    }

    // [4] Libertar memoria
    printf("[4] A libertar memoria do teste...\n");
    free(ids_da_frase);
    free(vetor_tf);
    libertarTokens(tokens_teste);

    printf("=== Teste R1.5 Concluido ===\n");
}


int main() {
    printf("=== INICIO DOS TESTES DO PROJETO ===\n");

    //testar_R1_1_e_R1_2();
    //testar_R1_3();
    testar_R1_4();
    testar_R1_5();

    printf("\n=== FIM DE TODOS OS TESTES ===\n");
    return 0;
}
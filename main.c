#include <stdio.h>
#include <stdlib.h>
#include "manipulacao_matrizes.h"
#include "bpe.h"
#include "vetorizacao.h"
#include "cal_similaridade.h"
#include "gestao_documentos.h"
#include "files.h"


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
void testar_R1_6() {
    printf("\n=== Teste R1.6: Calculo de Similaridade ===\n");
    MatrizTokens tokens = criarMatrizTokens(8);

    // --- ISTO É O QUE FALTAVA ---
    // Precisamos de adicionar os "blocos de construção"
    // A ordem aqui define os IDs (0, 1, 2, ...)
    printf("[1] A popular o vocabulario de teste...\n");
    adicionarToken(&tokens, "the");    // ID 0
    adicionarToken(&tokens, "a");      // ID 1
    adicionarToken(&tokens, " ");      // ID 2 (CRUCIAL! O espaço é um token)
    adicionarToken(&tokens, "cat");    // ID 3
    adicionarToken(&tokens, "dog");    // ID 4
    adicionarToken(&tokens, "sleeps"); // ID 5
    adicionarToken(&tokens, "runs");   // ID 6
    adicionarToken(&tokens, "eats");   // ID 7
    // --- FIM DA PARTE QUE FALTA ---
    int vocab_size = tokens.usados;

    // 2. Definir as frases do exemplo
    const char* s1_str = "the cat sleeps";
    const char* s2_str = "the dog runs";
    const char* s3_str = "a cat eats";
    const char* sin_str = "the cat eats";

    // 3. Tokenizar e Vetorizar (TF) cada frase
    int n_ids;
    int* ids_s1 = tokenizarFrase(s1_str, tokens, &n_ids);
    int* tf_s1 = calcularTfParaFrase(ids_s1, n_ids, vocab_size); // Vetor A

    int* ids_s2 = tokenizarFrase(s2_str, tokens, &n_ids);
    int* tf_s2 = calcularTfParaFrase(ids_s2, n_ids, vocab_size);

    int* ids_s3 = tokenizarFrase(s3_str, tokens, &n_ids);
    int* tf_s3 = calcularTfParaFrase(ids_s3, n_ids, vocab_size);

    int* ids_sin = tokenizarFrase(sin_str, tokens, &n_ids);
    int* tf_sin = calcularTfParaFrase(ids_sin, n_ids, vocab_size); // Vetor B

    // 4. Calcular e Imprimir Similaridades (comparar Sin com S1, S2, S3)
    printf("Comparando Sin (\"%s\") com:\n", sin_str);

    double sim_s1 = calcularSimilaridadeCosseno(tf_sin, tf_s1, vocab_size);
    printf("  S1 (\"%s\"): %.2f (Esperado: ~0.86)\n", s1_str, sim_s1);

    double sim_s2 = calcularSimilaridadeCosseno(tf_sin, tf_s2, vocab_size);
    printf("  S2 (\"%s\"): %.2f (Esperado: ~0.71)\n", s2_str, sim_s2);

    double sim_s3 = calcularSimilaridadeCosseno(tf_sin, tf_s3, vocab_size);
    printf("  S3 (\"%s\"): %.2f (Esperado: ~0.86)\n", s3_str, sim_s3);

    // 5. Libertar memoria
    free(ids_s1); free(tf_s1);
    free(ids_s2); free(tf_s2);
    free(ids_s3); free(tf_s3);
    free(ids_sin); free(tf_sin);
    libertarTokens(tokens);

    printf("=== Teste R1.6 Concluido ===\n");
}

void testar_R2_1_Avancado() {
    printf("\n=== Teste R2.1: Lista Ligada Avancada ===\n");

    // 1. Setup do Vocabulario
    MatrizTokens vocab = criarMatrizTokens(5);
    adicionarToken(&vocab, "banana"); // ID 0
    adicionarToken(&vocab, "apple");  // ID 1

    t_lista_docs lista = criarListaDocumentos(&vocab);

    // 2. Insercoes
    printf("[1] A inserir documentos...\n");
    adicionarDocumentoLista(&lista, "Doc1", "banana");
    adicionarDocumentoLista(&lista, "Doc3", "apple");

    // Teste Inserir no Meio (Indice 1)
    inserirDocumentoNoIndice(&lista, "Doc2", "banana apple", 1);
    listarDocumentos(lista);
    // Esperado: Doc1, Doc2, Doc3

    // 3. Obter Documento
    t_doc *d = obterDocumentoPorIndice(lista, 1);
    if(d) printf("[2] Doc no indice 1: %s (Esperado: Doc2)\n", d->titulo);

    // 4. Pesquisa Inversa (Quais docs têm 'banana' ID 0?)
    printf("[3] A pesquisar quem tem 'banana' (ID 0)...\n");
    pesquisarDocumentosPorTokenID(lista, 0);
    // Esperado: Doc1 e Doc2

    // 5. Remocao
    printf("[4] A remover indice 1 (Doc2)...\n");
    removerDocumentoPorIndice(&lista, 1);
    listarDocumentos(lista);
    // Esperado: Doc1, Doc3

    libertarListaDocumentos(&lista);
    libertarTokens(vocab);
}
void testar_R2_3() {
    printf("\n=== Teste R2.3: I/O com Ficheiros de Texto ===\n");

    // 1. Criar dados em memoria
    MatrizTokens vocab = criarMatrizTokens(5);
    adicionarToken(&vocab, "ola");
    adicionarToken(&vocab, "mundo");

    t_lista_docs lista_orig = criarListaDocumentos(&vocab);
    adicionarDocumentoLista(&lista_orig, "DocA", "ola mundo");
    adicionarDocumentoLista(&lista_orig, "DocB", "mundo mundo");

    // 2. Guardar em ficheiro
    const char *ficheiro = "dados_projeto.txt";
    guardarColecaoTexto(ficheiro, lista_orig);

    // 3. Limpar memoria da lista original (simular fecho do programa)
    // Nota: Numa implementacao real farias uma funcao para limpar a lista completa
    // Aqui vamos apenas libertar a estrutura wrapper para o teste

    // 4. Carregar do ficheiro para uma NOVA lista
    printf("\nA carregar dados do ficheiro...\n");
    t_lista_docs lista_nova = carregarColecaoTexto(ficheiro);

    // 5. Validar
    listarDocumentos(lista_nova);

    // Verificar se o vocabulario veio correto
    printf("Vocabulario carregado (%d tokens):\n", lista_nova.vocabulario->usados);
    listarTokens(*lista_nova.vocabulario);

    // Pequena validacao de vetorizacao
    t_doc *d = lista_nova.inicio;
    if(d && d->vetor_tf) {
        printf("Validacao DocA TF: 'ola'(ID 0)=%d (Esperado: 1)\n", d->vetor_tf[0]);
    }

    // Limpeza final do teste
    libertarTokens(vocab); // Original
    // A lista_nova tem o seu proprio vocabulario alocado, precisa de limpeza propria
    if (lista_nova.vocabulario) {
        libertarTokens(*lista_nova.vocabulario);
        free(lista_nova.vocabulario);
    }
    libertarListaDocumentos(&lista_nova);

    printf("=== Teste R2.3 Concluido ===\n");
}


int main() {
    printf("=== INICIO DOS TESTES DO PROJETO ===\n");
    testar_R1_1_e_R1_2();
    testar_R1_3();
    testar_R1_4();
    testar_R1_5();
    testar_R1_6();
    testar_R2_1_Avancado();
    testar_R2_3();

    printf("\n=== FIM DE TODOS OS TESTES ===\n");
    return 0;
}
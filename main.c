#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "manipulacao_matrizes.h"
#include "bpe.h"
#include "vetorizacao.h"
#include "cal_similaridade.h"
#include "gestao_documentos.h"
#include "files.h"


//  TESTES UNITARIOS

// Testa se as estruturas básicas de texto funcionam corretamente
void testar_estruturas_basicas() {
    printf("\n=== [Diagnostico] Teste Estruturas Basicas ===\n");// Cabeçalho visual para identificar o teste
    MatrizTexto texto = criarMatrizTexto(2);// Cria uma matriz de texto com capacidade inicial para 2 linhas
    adicionarLinha(&texto, "teste 1");// Adiciona uma linha de texto à matriz
    if (texto.linhas_usadas == 1) printf("[OK] Adicionar linha funciona.\n");// Verifica se a linha foi realmente adicionada
    libertarMatriz(texto);// Liberta a memória usada pela matriz de texto
}
// Testa se a tokenização escolhe sempre o maior token possível (greedy)
void testar_tokenizacao_greedy() {
    printf("\n=== [Diagnostico] Teste Tokenizacao Greedy ===\n");
    MatrizTokens t = criarMatrizTokens(5); // Cria um vocabulário pequeno para o teste
    // Adiciona tokens de diferentes tamanhos
    adicionarToken(&t, "uni");
    adicionarToken(&t, "versidade");
    adicionarToken(&t, "universidade");


    // Tokeniza a palavra "universidade"
    int n;
    int *ids = tokenizarFrase("universidade", t, &n);


    // Espera-se que escolha apenas o token "universidade"
    // (ID 2), em vez de dividir em partes menores
    if (n == 1 && ids[0] == 2) {
        printf("[OK] O sistema escolheu o maior token possivel.\n");
    } else {
        printf("[FALHA] O sistema nao foi ganancioso (Greedy).\n");
    }
    // Liberta memória usada no teste
    free(ids);
    libertarTokens(t);
}


//  MOTOR DE TESTE GENERICO



void executar_demonstracao_completa(char *nome_teste, char **frases_treino, int n_treino, char *frase_pesquisa, int n_tokens_alvo) {
    printf("\n\n");
    printf("### CENARIO: %s\n", nome_teste);


    // 1. Criar o Corpus
    MatrizTexto corpus = criarMatrizTexto(10);
    // Adiciona todas as frases de treino ao corpus
    for(int i=0; i<n_treino; i++) {
        adicionarLinha(&corpus, frases_treino[i]);
    }

    // 2. Aprender Vocabulario (BPE)
    printf(">> 1. A aprender padroes no texto (BPE)...\n");
    MatrizTokens vocab = criarMatrizTokens(n_tokens_alvo);// Cria o vocabulário com o número alvo de tokens

    // Executa o BPE
    calcularAlfabetoTokens(&corpus, &vocab, n_tokens_alvo);

    // MOSTRAR TUDO (Como pediste, sem '...')
    printf("   Vocabulario Final (%d tokens):\n   ", vocab.usados);
    for(int i=0; i < vocab.usados; i++) {
        printf("[%s] ", vocab.tokens[i]);
        // Quebra de linha a cada 10 tokens para nao ficar ilegivel
        if ((i + 1) % 10 == 0) printf("\n   ");
    }
    printf("\n");

    // 3. Criar Base de Dados
    printf(">> 2. A indexar documentos e calcular frequencias (TF)...\n");
    LL_TK_TF bd = criarListaDocumentos(&vocab);// Cria a lista ligada de documentos
    // Adiciona cada frase como um documento
    for(int i=0; i<n_treino; i++) {
        char titulo[20];
        sprintf(titulo, "Doc_%d", i+1);
        adicionarDocumentoLista(&bd, titulo, frases_treino[i]);
    }

    // 4. Testar a Pesquisa
    printf(">> 3. A pesquisar por: '%s'\n", frase_pesquisa);
    int n;
    int *ids_q = tokenizarFrase(frase_pesquisa, *bd.vocabulario, &n);

    // Debug: ver se a frase de pesquisa foi bem tokenizada
    printf("   (Tokens identificados na pesquisa: %d)\n", n);

    int *tf_q = calcularTfParaFrase(ids_q, n, bd.vocabulario->usados);

    TK_TF_NODE *atual = bd.inicio;
    int encontrou_algo = 0;
    while(atual) {
        double s = calcularSimilaridadeCosseno(tf_q, atual->vetor_tf, bd.vocabulario->usados);

        // Removemos o filtro! Mostra TUDO, mesmo que seja 0.000
        printf("   -> Similaridade com '%s': %.3f\n", atual->texto_original, s);

        if (s > 0.001) encontrou_algo = 1;
        atual = atual->proximo;
    }

    if(!encontrou_algo) printf("   [AVISO] Nenhuma similaridade > 0 encontrada.\n");

    // Limpezas
    free(ids_q); free(tf_q);
    libertarMatriz(corpus);
    libertarTokens(vocab);
    libertarListaDocumentos(&bd);
}


//  MAIN


int main() {
    // Testes básicos para garantir que as estruturas
    // fundamentais (matrizes, listas, etc.) funcionam corretamente
    testar_estruturas_basicas();
    // Testa se a tokenização é realmente "greedy",
    // ou seja, se escolhe sempre o maior token possível
    testar_tokenizacao_greedy();

    // 1. O Exemplo do PDF (Ingles)
    char *frases_pdf[] = {
        "the cat sleeps",
        "the dog runs",
        "a cat eats"
    };
    // Pedimos 50 tokens (vai ter de inventar merges mesmo com freq=1)
    executar_demonstracao_completa("PDF Exemplo (Ingles)", frases_pdf, 3, "the cat eats", 20);

    // 2. Teste Robustez (Portugues)
    char *frases_pt[] = {
        "eu gosto de programar em C",
        "o algoritmia e dificil mas util",
        "programar computadores e fixe"
    };
    executar_demonstracao_completa("Teste Robustez (Portugues)", frases_pt, 3, "gosto de programar", 50);

    printf("\n=== TODOS OS TESTES CONCLUIDOS ===\n");// Mensagem final
    return 0;
}
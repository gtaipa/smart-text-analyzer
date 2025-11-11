#include "manipulacao_matrizes.h"











// =============================================================
//  Programa principal (testes automaticos)
// =============================================================


int main() {
    printf("=== Inicio dos testes automaticos ===\n");

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

    printf("\n[6] Remover linha por conteudo (\"a cat eats\")...\n");
    removerLinhaPorConteudo(&texto, "a cat eats");
    listarMatriz(texto);

    printf("\n[7] Remover token por indice (2)...\n");
    removerToken(&tokens, 2);
    listarTokens(tokens);

    printf("\n[8] Remover token por conteudo (\"eats\")...\n");
    removerTokenPorConteudo(&tokens, "eats");
    listarTokens(tokens);

    printf("\n[9] Libertar memoria...\n");
    libertarMatriz(texto);
    libertarTokens(tokens);

    printf("\nTeste R1.2 concluido com sucesso!\n");
    printf("=== Fim dos testes ===\n");

    return 0;
}

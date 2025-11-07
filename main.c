#include "manipulacao_matrizes.h"
int calcularAlfabetoTokens(MatrizTexto *texto_corpus,
                           MatrizTokens *alfabeto_tokens,
                           int num_tokens_desejado){

                           }


/**
 * @brief Percorre o corpus e o alfabeto atual para encontrar o par de tokens
 * adjacentes mais frequente.
 * * Nota: A implementação real pode ser complexa. Esta função idealmente retorna o novo
 * token que deve ser criado (a fusão dos dois tokens mais frequentes).
 * * @param m_texto A MatrizTexto contendo as frases do corpus.
 * @param m_tokens A MatrizTokens com o alfabeto de tokens atual.
 * @param token1_out Ponteiro para uma string onde o primeiro token do par mais frequente será armazenado.
 * @param token2_out Ponteiro para uma string onde o segundo token do par mais frequente será armazenado.
 * @return int Retorna a frequência máxima encontrada, ou 0 se nenhum par for encontrado.
 */
int encontrarParMaisFrequente(MatrizTexto m_texto,
                              MatrizTokens m_tokens,
                              char **token1_out,
                              char **token2_out);


/**
* @brief Atualiza o corpus substituindo todas as ocorrências do par de tokens
* especificado pelo seu novo token fundido.
* * Esta função deve reescrever as linhas da MatrizTexto de forma eficiente.
* * @param m_texto Ponteiro para a MatrizTexto a ser atualizada.
* @param par_a_fundir O novo token (string) resultante da fusão, ex: "ar" de "a" + "r".
* @param token1 O primeiro token da fusão, ex: "a".
* @param token2 O segundo token da fusão, ex: "r".
* @return int Retorna o número de fusões efetuadas, ou -1 em caso de erro.
*/
int aplicarFusaoNoCorpus(MatrizTexto *m_texto,
                         const char *token1,
                         const char *token2);
int inicializarTokensUnicos(MatrizTexto m_texto, MatrizTokens *m_tokens) {
    char vistos[256] = {0};
    char token[2];

    for (int i = 0; i < m_texto.linhas_usadas; i++) {
        for (int j = 0; m_texto.linhas[i][j] != '\0'; j++) {
            char c = m_texto.linhas[i][j];

            if (vistos[(unsigned char)c] == 0) {
                vistos[(unsigned char)c] = 1;
                token[0] = c;
                token[1] = '\0';
                adicionarToken(m_tokens, token);
            }
        }
    }

    printf("Total de tokens únicos: %d\n", m_tokens->usados);
    return 1;
}
/**
 * @brief Encontra o par de tokens (dois caracteres) mais frequente no texto.
 * @param m_texto Matriz com as linhas do texto.
 * @param m_tokens Matriz com os tokens únicos.
 * @param par_max_freq Ponteiro onde será guardada a frequência do par mais comum.
 * @return Ponteiro para uma string com o par mais frequente, ou NULL se não existir.
 */
char* encontrarParMaisFrequente(MatrizTexto m_texto, MatrizTokens m_tokens, int *par_max_freq) {
    int max_frequencia = 0;
    char *melhor_par = NULL;
    char token_temp[3];

    // Se houver menos de 2 tokens, não há pares possíveis
    if (m_tokens.usados < 2) {
        *par_max_freq = 0;
        return NULL;
    }

    for (int i = 0; i < m_tokens.usados; i++) {
        char *token1 = m_tokens.tokens[i];
        if (strlen(token1) > 1) continue;

        for (int j = 0; j < m_tokens.usados; j++) {
            char *token2 = m_tokens.tokens[j];
            if (strlen(token2) > 1) continue;

            // Cria o par T1T2 (ex: "ab")
            token_temp[0] = token1[0];
            token_temp[1] = token2[0];
            token_temp[2] = '\0';

            int freq_atual = 0;

            // Conta quantas vezes o par aparece no texto
            for (int k = 0; k < m_texto.linhas_usadas; k++) {
                char *linha = m_texto.linhas[k];
                char *ptr = linha;

                while ((ptr = strstr(ptr, token_temp)) != NULL) {
                    freq_atual++;
                    ptr += 2;
                }
            }

            // Atualiza o par mais frequente se necessário
            if (freq_atual > max_frequencia) {
                max_frequencia = freq_atual;

                if (melhor_par != NULL) free(melhor_par);
                melhor_par = strdup(token_temp);

                if (melhor_par == NULL) {
                    printf("Erro: falha de memória.\n");
                    *par_max_freq = 0;
                    return NULL;
                }
            }
        }
    }

    *par_max_freq = max_frequencia;

    if (max_frequencia > 0)
        printf("Par mais frequente: '%s' (%d ocorrencias)\n", melhor_par, max_frequencia);
    else
        printf("Nenhum par encontrado.\n");

    return melhor_par;
}













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

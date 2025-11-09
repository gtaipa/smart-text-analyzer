#include "manipulacao_matrizes.h"
int adicionarToken(MatrizTokens *m, const char *novo_token) {
    if (m->usados >= MAX_TOKENS)
        return 0;
    m->tokens[m->usados] = strdup(novo_token);
    if (m->tokens[m->usados] == NULL)
        return 0;
    m->usados++;
    return 1;
}

// ----------------------------------------------------
// R1.3 - Inicializar tokens únicos (alfabeto base)
// ----------------------------------------------------
int inicializarTokensUnicos(MatrizTexto m_texto, MatrizTokens *m_tokens) {
    unsigned char vistos[256] = {0};
    char token[2];

    for (int i = 0; i < m_texto.linhas_usadas; i++) {
        char *linha = m_texto.linhas[i];
        for (int j = 0; linha[j] != '\0'; j++) {
            unsigned char c = (unsigned char)linha[j];
            if (!vistos[c]) {
                vistos[c] = 1;
                token[0] = linha[j];
                token[1] = '\0';
                if (!adicionarToken(m_tokens, token))
                    return 0;
            }
        }
    }
    return 1;
}

// ----------------------------------------------------
// R1.4 - Encontrar o par mais frequente no corpus
// ----------------------------------------------------
char *encontrarParMaisFrequente(MatrizTexto m_texto, MatrizTokens m_tokens, int *freq_max) {
    int max_frequencia = 0;
    char *melhor_par = NULL;
    char temp[3];

    if (m_tokens.usados < 2) {
        *freq_max = 0;
        return NULL;
    }

    for (int i = 0; i < m_tokens.usados; i++) {
        char *t1 = m_tokens.tokens[i];
        if (strlen(t1) > 1) continue;

        for (int j = 0; j < m_tokens.usados; j++) {
            char *t2 = m_tokens.tokens[j];
            if (strlen(t2) > 1) continue;

            temp[0] = t1[0];
            temp[1] = t2[0];
            temp[2] = '\0';

            int freq_atual = 0;

            for (int k = 0; k < m_texto.linhas_usadas; k++) {
                char *linha = m_texto.linhas[k];
                char *ptr = linha;
                while ((ptr = strstr(ptr, temp)) != NULL) {
                    freq_atual++;
                    ptr += 2;
                }
            }

            if (freq_atual > max_frequencia) {
                max_frequencia = freq_atual;
                free(melhor_par);
                melhor_par = strdup(temp);
                if (!melhor_par) return NULL;
            }
        }
    }

    *freq_max = max_frequencia;
    return melhor_par;
}

// ----------------------------------------------------
// R1.5 - Aplicar fusão no corpus
// ----------------------------------------------------
int aplicarFusaoNoCorpus(MatrizTexto *m_texto, const char *token1, const char *token2) {
    int fusoes = 0;
    char par[3] = {token1[0], token2[0], '\0'};

    for (int i = 0; i < m_texto->linhas_usadas; i++) {
        char *orig = m_texto->linhas[i];
        int tam_orig = strlen(orig);

        // Contar ocorrências do par
        int count = 0;
        char *ptr = orig;
        while ((ptr = strstr(ptr, par)) != NULL) {
            count++;
            ptr += 2;
        }

        if (count == 0) continue;

        // Alocar nova linha
        char *nova = malloc(tam_orig + 1);
        if (!nova) return -1;

        char *p_orig = orig;
        char *p_nova = nova;
        char *next;

        while (*p_orig) {
            next = strstr(p_orig, par);
            if (!next) {
                strcpy(p_nova, p_orig);
                break;
            }
            int antes = next - p_orig;
            strncpy(p_nova, p_orig, antes);
            p_nova += antes;
            strcpy(p_nova, par);
            p_nova += 2;
            p_orig = next + 2;
            fusoes++;
        }

        free(m_texto->linhas[i]);
        m_texto->linhas[i] = nova;
    }
    return fusoes;
}

// ----------------------------------------------------
// R1.6 - Calcular o alfabeto final (loop principal BPE)
// ----------------------------------------------------
int calcularAlfabetoTokens(MatrizTexto *texto_corpus,
                           MatrizTokens *alfabeto_tokens,
                           int num_tokens_desejado) {

    if (!inicializarTokensUnicos(*texto_corpus, alfabeto_tokens))
        return 0;

    while (alfabeto_tokens->usados < num_tokens_desejado) {
        int freq;
        char *par = encontrarParMaisFrequente(*texto_corpus, *alfabeto_tokens, &freq);

        if (!par || freq <= 1) {
            free(par);
            break;
        }

        if (!adicionarToken(alfabeto_tokens, par)) {
            free(par);
            return 0;
        }

        if (aplicarFusaoNoCorpus(texto_corpus, &par[0], &par[1]) == -1) {
            free(par);
            return 0;
        }

        free(par);
    }

    return 1;
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

// Created by pedro on 07/11/2025.

#include "manipulacao_matrizes.h"

// =============================================================
//  Funcoes auxiliares de texto
// =============================================================
MatrizTexto criarMatrizTexto(int capacidade_inicial) {
    MatrizTexto m;
    m.linhas = malloc(capacidade_inicial * sizeof(char*));
    if (m.linhas == NULL) {
        printf("Erro: sem memoria.\n");
        exit(1);
    }
    m.linhas_usadas = 0;
    m.capacidade = capacidade_inicial;
    return m;
}

int redimensionarMatriz(MatrizTexto *m, int nova_capacidade) {
    char **novas_linhas = realloc(m->linhas, nova_capacidade * sizeof(char*));
    if (novas_linhas == NULL) {
        printf("Erro: sem memoria ao redimensionar.\n");
        return 0;
    }
    m->linhas = novas_linhas;
    m->capacidade = nova_capacidade;
    printf("Matriz redimensionada para capacidade %d.\n", nova_capacidade);
    return 1;
}

int adicionarLinha(MatrizTexto *m, const char *frase) {
    if (m->linhas_usadas == m->capacidade) {
        redimensionarMatriz(m, m->capacidade * 2);
    }

    m->linhas[m->linhas_usadas] = malloc(strlen(frase) + 1);
    if (m->linhas[m->linhas_usadas] == NULL) {
        printf("Erro: sem memoria para guardar a frase.\n");
        return 0;
    }

    strcpy(m->linhas[m->linhas_usadas], frase);
    m->linhas_usadas++;
    return 1;
}

void listarMatriz(MatrizTexto m) {
    printf("\nMatriz contem %d linhas:\n", m.linhas_usadas);
    for (int i = 0; i < m.linhas_usadas; i++) {
        printf("[%d] %s\n", i, m.linhas[i]);
    }
}

void libertarMatriz(MatrizTexto m) {
    if (m.linhas != NULL) {
        for (int i = 0; i < m.linhas_usadas; i++) {
            if (m.linhas[i] != NULL)
                free(m.linhas[i]);
        }
        free(m.linhas);
    }
}

void pesquisarTexto(MatrizTexto m, const char *palavra) {
    int encontrada = 0;
    for (int i = 0; i < m.linhas_usadas; i++) {
        if (strstr(m.linhas[i], palavra) != NULL) {
            printf("Linha %d: %s\n", i, m.linhas[i]);
            encontrada++;
        }
    }
    if (encontrada == 0)
        printf("Nenhuma linha contem \"%s\".\n", palavra);
    else
        printf("%d linha(s) encontradas.\n", encontrada);
}

int removerLinha(MatrizTexto *m, int indice) {
    if (indice < 0 || indice >= m->linhas_usadas) {
        printf("Indice fora dos limites!\n");
        return 0;
    }
    free(m->linhas[indice]);
    for (int i = indice; i < m->linhas_usadas - 1; i++) {
        m->linhas[i] = m->linhas[i + 1];
    }
    m->linhas_usadas--;
    printf("Linha %d removida.\n", indice);
    return 1;
}

int removerLinhaPorConteudo(MatrizTexto *m, const char *frase) {
    for (int i = 0; i < m->linhas_usadas; i++) {
        if (strcmp(m->linhas[i], frase) == 0) {
            return removerLinha(m, i);
        }
    }
    printf("Nenhuma linha igual a \"%s\" foi encontrada.\n", frase);
    return 0;
}

// =============================================================
//  Funcoes auxiliares de tokens
// =============================================================
MatrizTokens criarMatrizTokens(int capacidade_inicial) {
    MatrizTokens t;
    t.tokens = malloc(capacidade_inicial * sizeof(char*));
    if (t.tokens == NULL) {
        printf("Erro: sem memoria para tokens.\n");
        exit(1);
    }
    t.usados = 0;
    t.capacidade = capacidade_inicial;
    return t;
}

int adicionarToken(MatrizTokens *t, const char *token) {
    if (t->usados == t->capacidade) {
        int nova_cap = t->capacidade * 2;
        char **novos = realloc(t->tokens, nova_cap * sizeof(char*));
        if (novos == NULL) {
            printf("Erro: sem memoria ao redimensionar tokens.\n");
            return 0;
        }
        t->tokens = novos;
        t->capacidade = nova_cap;
        printf("Tokens redimensionados para capacidade %d.\n", nova_cap);
    }

    t->tokens[t->usados] = malloc(strlen(token) + 1);
    strcpy(t->tokens[t->usados], token);
    t->usados++;
    return 1;
}

void listarTokens(MatrizTokens t) {
    printf("\nTokens armazenados (%d):\n", t.usados);
    for (int i = 0; i < t.usados; i++) {
        printf("[%d] %s\n", i, t.tokens[i]);
    }
}

int removerToken(MatrizTokens *t, int indice) {
    if (indice < 0 || indice >= t->usados) {
        printf("Indice de token fora dos limites!\n");
        return 0;
    }

    free(t->tokens[indice]);

    for (int i = indice; i < t->usados - 1; i++) {
        t->tokens[i] = t->tokens[i + 1];
    }

    t->usados--;
    printf("Token %d removido com sucesso.\n", indice);
    return 1;
}

int removerTokenPorConteudo(MatrizTokens *t, const char *token) {
    for (int i = 0; i < t->usados; i++) {
        if (strcmp(t->tokens[i], token) == 0) {
            return removerToken(t, i);
        }
    }
    printf("Token \"%s\" nao encontrado.\n", token);
    return 0;
}

void libertarTokens(MatrizTokens t) {
    for (int i = 0; i < t.usados; i++) {
        free(t.tokens[i]);
    }
    free(t.tokens);
}

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
//  Requisito 1.5: Cálculo da Frequência de Tokens
// =============================================================

/**
 * @brief Calcula a frequência de cada token presente na MatrizTokens, contando
 * as suas ocorrências no MatrizTexto (corpus modificado).
 * @param m_texto MatrizTexto contendo o corpus (já com as fusoes aplicadas).
 * @param m_tokens MatrizTokens contendo o alfabeto final.
 * @param num_frequencias_out Ponteiro para guardar o numero total de frequencias (igual a m_tokens.usados).
 * @return int* Retorna um array de inteiros alocado dinamicamente com as frequencias, ou NULL em caso de erro.
 */
int* calcularFrequenciaTokens(MatrizTexto m_texto,
                              MatrizTokens m_tokens,
                              int *num_frequencias_out) {

    // 1. Alocar o array de frequências
    int *frequencias = (int*) calloc(m_tokens.usados, sizeof(int));
    if (frequencias == NULL) {
        printf("Erro: Falha na alocacao de memoria para o array de frequencias.\n");
        *num_frequencias_out = 0;
        return NULL;
    }

    printf("\n⚙  A calcular a frequencia de %d tokens...\n", m_tokens.usados);

    // 2. Iterar por cada token no alfabeto
    for (int t_idx = 0; t_idx < m_tokens.usados; t_idx++) {
        const char *token_atual = m_tokens.tokens[t_idx];
        int tamanho_token = strlen(token_atual);
        int freq_atual = 0;

        // 3. Iterar por cada linha do corpus
        for (int l_idx = 0; l_idx < m_texto.linhas_usadas; l_idx++) {
            char *linha = m_texto.linhas[l_idx];
            char *ptr = linha;

            // 4. Usar strstr para encontrar todas as ocorrências do token na linha
            // Nota: Esta abordagem é simples mas sensível a sobreposições,
            // no BPE real é mais seguro contar IDs, mas funciona para strings.
            while ((ptr = strstr(ptr, token_atual)) != NULL) {
                freq_atual++;
                // Avança o ponteiro pelo tamanho do token para evitar contagem sobreposta
                // Ex: em "banana", o token "ana" so deve ser contado uma vez.
                ptr += tamanho_token;
            }
        }

        // 5. Guardar a frequência no array alocado
        frequencias[t_idx] = freq_atual;
    }

    *num_frequencias_out = m_tokens.usados;
    printf("Calculo de frequencias concluido. Total de %d frequencias.\n", *num_frequencias_out);

    return frequencias;
}
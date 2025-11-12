// Created by pedro on 07/11/2025.

#include "manipulacao_matrizes.h"
#include <string.h> // Adicionado para strdup, strstr, etc.
#include <stdlib.h> // Adicionado para malloc, free, realloc
#include <stdio.h>  // Adicionado para printf, sprintf

// =============================================================
//  Funcoes auxiliares de texto (R1.1, R1.2)
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
//  Funcoes auxiliares de tokens (R1.1, R1.2)
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
        // printf("Tokens redimensionados para capacidade %d.\n", nova_cap); // Comentado para BPE
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


// =============================================================
//  Requisito 1.3 - BPE
// =============================================================

int inicializarTokensUnicos(MatrizTexto m_texto, MatrizTokens *m_tokens) {
    unsigned char vistos[256] = {0};
    char token[2] = {'\0', '\0'};

    for (int i = 0; i < m_texto.linhas_usadas; i++) {
        char *linha = m_texto.linhas[i];
        for (int j = 0; linha[j] != '\0'; j++) {
            unsigned char c = (unsigned char)linha[j];
            if (!vistos[c]) {
                vistos[c] = 1;
                token[0] = linha[j];
                if (!adicionarToken(m_tokens, token))
                    return 0;
            }
        }
    }
    return 1;
}

int encontrarParMaisFrequente(MatrizTexto m_texto,
                              MatrizTokens m_tokens,
                              int *freq_max_out,
                              char **t1_out,
                              char **t2_out) {

    int max_frequencia = 0;
    char *melhor_t1 = NULL;
    char *melhor_t2 = NULL;

    for (int i = 0; i < m_tokens.usados; i++) {
        char *t1 = m_tokens.tokens[i];

        for (int j = 0; j < m_tokens.usados; j++) {
            char *t2 = m_tokens.tokens[j];

            int len_procura = strlen(t1) + strlen(t2) + 2;
            char *par_procura = malloc(len_procura);
            if (!par_procura) return 0;
            sprintf(par_procura, "%s %s", t1, t2);

            int freq_atual = 0;

            for (int k = 0; k < m_texto.linhas_usadas; k++) {
                char *linha = m_texto.linhas[k];
                char *ptr = linha;
                while ((ptr = strstr(ptr, par_procura)) != NULL) {
                    freq_atual++;
                    ptr += 1;
                }
            }

            if (freq_atual > max_frequencia) {
                max_frequencia = freq_atual;
                free(melhor_t1);
                free(melhor_t2);
                melhor_t1 = strdup(t1);
                melhor_t2 = strdup(t2);
                if (!melhor_t1 || !melhor_t2) return 0;
            }

            free(par_procura);
        }
    }

    if (max_frequencia > 0) {
        *freq_max_out = max_frequencia;
        *t1_out = melhor_t1;
        *t2_out = melhor_t2;
        return 1; // Encontrou
    } else {
        free(melhor_t1);
        free(melhor_t2);
        *freq_max_out = 0;
        return 0; // Nao encontrou
    }
}

// Funcao auxiliar para "find and replace" numa string
char *str_replace(char *orig, const char *procura, const char *substituto) {
    char *result;
    char *ins;
    char *tmp;
    int len_procura = strlen(procura);
    int len_substituto = strlen(substituto);
    int count;

    if (!orig || !procura) return NULL;
    if (len_procura == 0) return NULL;

    for (count = 0; (tmp = strstr(orig, procura)); count++) {
        orig = tmp + len_procura;
    }

    if (count == 0) {
        // strdup nao e C standard, mas e comum. Se nao der, usamos malloc+strcpy
        char *copia = malloc(strlen(orig) + 1);
        strcpy(copia, orig);
        return copia;
    }

    orig -= (count * len_procura);

    result = (char *)malloc(strlen(orig) + (len_substituto - len_procura) * count + 1);
    if (result == NULL) return NULL;

    tmp = result;
    while (count--) {
        ins = strstr(orig, procura);
        int len_front = ins - orig;
        memcpy(tmp, orig, len_front);
        tmp += len_front;
        memcpy(tmp, substituto, len_substituto);
        tmp += len_substituto;
        orig += len_front + len_procura;
    }
    strcpy(tmp, orig);
    return result;
}


int aplicarFusaoNoCorpus(MatrizTexto *m_texto,
                         const char *token1,
                         const char *token2,
                         const char *token_fundido) {
    int fusoes_total = 0;

    int len_procura = strlen(token1) + strlen(token2) + 2;
    char *par_procura = malloc(len_procura);
    if (!par_procura) return -1;
    sprintf(par_procura, "%s %s", token1, token2);

    for (int i = 0; i < m_texto->linhas_usadas; i++) {
        char *linha_original = m_texto->linhas[i];

        if (strstr(linha_original, par_procura) != NULL) {

            char *nova_linha = str_replace(linha_original, par_procura, token_fundido);

            if (nova_linha != NULL) {
                free(m_texto->linhas[i]);
                m_texto->linhas[i] = nova_linha;
                fusoes_total++;
            }
        }
    }

    free(par_procura);
    return fusoes_total;
}


int calcularAlfabetoTokens(MatrizTexto *texto_corpus,
                           MatrizTokens *alfabeto_tokens,
                           int num_tokens_desejado) {

    printf("--- Iniciando calculo BPE ---\n");
    if (!inicializarTokensUnicos(*texto_corpus, alfabeto_tokens))
        return 0;

    printf("Tokens iniciais (caracteres unicos) criados: %d\n", alfabeto_tokens->usados);
    // listarTokens(*alfabeto_tokens);

    while (alfabeto_tokens->usados < num_tokens_desejado) {
        int freq;
        char *t1 = NULL;
        char *t2 = NULL;

        if (!encontrarParMaisFrequente(*texto_corpus, *alfabeto_tokens, &freq, &t1, &t2)) {
            printf("Nao ha mais pares para fundir.\n");
            break;
        }

        if (freq == 0) {
             printf("Frequencia 0, a parar.\n");
             free(t1); free(t2);
             break;
        }

        int len_novo = strlen(t1) + strlen(t2) + 1;
        char *novo_token = malloc(len_novo);
        if (!novo_token) { free(t1); free(t2); return 0; }
        sprintf(novo_token, "%s%s", t1, t2);

        printf("\nIteracao: Fundir '%s' e '%s' -> '%s' (Freq: %d)\n", t1, t2, novo_token, freq);

        if (!adicionarToken(alfabeto_tokens, novo_token)) {
            free(t1); free(t2); free(novo_token);
            return 0;
        }

        aplicarFusaoNoCorpus(texto_corpus, t1, t2, novo_token);

        free(t1);
        free(t2);
        free(novo_token);
    }

    printf("--- Fim do calculo BPE ---\n");
    return 1;
}

// =============================================================
//  Requisito 1.4: Tokenizacao das frases
// =============================================================
int* tokenizarFrase(const char *frase, MatrizTokens tokens, int *n_ids) {
    int capacidade = 16;
    int usados = 0;
    int *ids = malloc(capacidade * sizeof(int));
    if (ids == NULL) {
        printf("Erro: sem memoria para tokenizacao.\n");
        exit(1);
    }

    int len = strlen(frase);
    int i = 0;

    while (i < len) {
        int melhor_id = -1;
        int melhor_tamanho = 0;

        // procura o token mais longo que encaixa na posicao atual
        for (int t = 0; t < tokens.usados; t++) {
            int tam = strlen(tokens.tokens[t]);
            if (tam > melhor_tamanho && strncmp(frase + i, tokens.tokens[t], tam) == 0) {
                melhor_id = t;
                melhor_tamanho = tam;
            }
        }

        if (melhor_id != -1) {
            // guarda o id do token encontrado
            if (usados == capacidade) {
                capacidade *= 2;
                ids = realloc(ids, capacidade * sizeof(int));
            }
            ids[usados++] = melhor_id;
            i += melhor_tamanho;
        } else {
            // se nao encontrar token, avanca 1 caractere e guarda -1
            if (usados == capacidade) {
                capacidade *= 2;
                ids = realloc(ids, capacidade * sizeof(int));
            }
            ids[usados++] = -1; // token desconhecido
            i++;
        }
    }

    *n_ids = usados;
    return ids;
}

// =============================================================
//  Requisito 1.5
// =============================================================
int* calcularFrequenciaTokens(MatrizTexto m_texto,
                              MatrizTokens m_tokens,
                              int *num_frequencias_out) {

    int *frequencias = (int*) calloc(m_tokens.usados, sizeof(int));
    if (frequencias == NULL) {
        printf("Erro: Falha na alocacao de memoria para o array de frequencias.\n");
        *num_frequencias_out = 0;
        return NULL;
    }

    printf("\n⚙  A calcular a frequencia de %d tokens...\n", m_tokens.usados);

    for (int t_idx = 0; t_idx < m_tokens.usados; t_idx++) {
        const char *token_atual = m_tokens.tokens[t_idx];
        int tamanho_token = strlen(token_atual);
        int freq_atual = 0;

        for (int l_idx = 0; l_idx < m_texto.linhas_usadas; l_idx++) {
            char *linha = m_texto.linhas[l_idx];
            char *ptr = linha;

            while ((ptr = strstr(ptr, token_atual)) != NULL) {
                freq_atual++;
                ptr += tamanho_token;
            }
        }
        frequencias[t_idx] = freq_atual;
    }

    *num_frequencias_out = m_tokens.usados;
    printf("Calculo de frequencias concluido. Total de %d frequencias.\n", *num_frequencias_out);

    return frequencias;
}
/**
 * @brief Calcula o vetor de frequencia (TF) para UMA frase.
 *
 * Recebe o array de IDs de uma frase (resultado de R1.4) e
 * conta quantas vezes cada ID aparece.
 *
 * @param ids_frase Array de inteiros com os IDs dos tokens da frase.
 * @param num_ids_frase O numero de elementos no array ids_frase.
 * @param tamanho_vocabulario O numero total de tokens no alfabeto (ex: m_tokens.usados).
 * @return int* Retorna um novo array (vetor TF) alocado dinamicamente
 * com tamanho 'tamanho_vocabulario'.
 * A posica 'i' deste vetor contem a contagem do token ID 'i'.
 */
int* calcularTfParaFrase(int *ids_frase, int num_ids_frase, int tamanho_vocabulario) {

    // 1. Alocar e inicializar o vetor de frequencias com zeros.
    // Usamos calloc porque ja inicializa a memoria a 0.
    int *tf_vetor = (int*) calloc(tamanho_vocabulario, sizeof(int));
    if (tf_vetor == NULL) {
        printf("Erro: sem memoria para vetor TF.\n");
        return NULL;
    }

    // 2. Iterar pelos IDs da frase e contar
    for (int i = 0; i < num_ids_frase; i++) {
        int token_id = ids_frase[i];

        // Ignorar tokens desconhecidos (ID -1)
        if (token_id >= 0 && token_id < tamanho_vocabulario) {
            // Incrementar o contador para este ID
            tf_vetor[token_id]++;
        }
    }

    return tf_vetor;
}


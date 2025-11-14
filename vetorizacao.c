#include "vetorizacao.h"

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
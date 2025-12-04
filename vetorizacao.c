#include "vetorizacao.h"
//  Requisito 1.4: Tokenizacao das frases

// O objetivo desta funcao e transformar uma frase (texto) numa lista de numeros (IDs).
int* tokenizarFrase(const char *frase, MatrizTokens tokens, int *n_ids) {
    // Preparar a Memoria Começo, com espaço para 16 IDs. Se precisar de mais, aumento depois.
    int capacidade = 16;
    int usados = 0;
    int *ids = malloc(capacidade * sizeof(int));

    if (ids == NULL) {
        printf("Erro: O computador nao tem memoria suficiente.\n");
        exit(1);
    }
    int len = strlen(frase);
    int i = 0;
    while (i < len) {
        int melhor_id = -1;       // Qual o ID do token que encontrei?
        int melhor_tamanho = 0;   // Qual o tamanho desse token?

        // --- O SEGREDO DO "GREEDY MATCH" ---
        // Em cada posicao da frase, testo TODOS os tokens do vocabulario.
        // O meu objetivo é encontrar o MAIOR token que encaixe aqui.
        for (int t = 0; t < tokens.usados; t++) {
            int tam_token = strlen(tokens.tokens[t]);

            // Verifico duas coisas:
            // 1. Se este token é maior do que o melhor que ja encontrei ate agora.
            // 2. Se o token encaixa exatamente na frase na posicao 'i'.
            // (strncmp compara apenas 'tam_token' caracteres)
            if (tam_token > melhor_tamanho && strncmp(frase + i, tokens.tokens[t], tam_token) == 0) {
                melhor_id = t;
                melhor_tamanho = tam_token;
            }
        }

        // Se encontrei algum token valido:
        if (melhor_id != -1) {
            // Verifico se o meu cesto de IDs esta cheio
            if (usados == capacidade) {
                capacidade *= 2; // Duplico o tamanho
                // O realloc tenta aumentar o espaco mantendo os dados que la estao
                ids = realloc(ids, capacidade * sizeof(int));
                // Verificacao de seguranca
                if (ids == NULL) {
                    printf("Erro ao redimensionar memoria.\n");
                    exit(1);
                }
            }
            // Guardo o ID na lista
            ids[usados] = melhor_id;
            usados++;
            // AVANÇAR O CURSOR:
            i += melhor_tamanho;
        }
        else {
            // Se nao encontrei nenhum token conhecido
            // Tambem tenho de guardar espaco se necessario
            if (usados == capacidade) {
                capacidade *= 2;
                ids = realloc(ids, capacidade * sizeof(int));
            }

            ids[usados] = -1; // Marco com -1 para dizer "Desconhecido"
            usados++;
            i++; // Avanço apenas 1 letra para tentar de novo na proxima
        }
    }
    // Devolvo o tamanho final do array atraves do ponteiro
    *n_ids = usados;
    return ids;
}

//  Requisito 1.5 - Estatisticas e Vetores

int* calcularFrequenciaTokens(MatrizTexto m_texto,MatrizTokens m_tokens,int *num_frequencias_out) {
    // 1. Alocar o array de contadores.
    // O calloc mete logo zeros em tudo. Se usasse malloc, vinha com lixo na memoria.
    int *frequencias = (int*) calloc(m_tokens.usados, sizeof(int));

    if (frequencias == NULL) {
        printf("Erro: Sem memoria para contar frequencias.\n");
        *num_frequencias_out = 0;
        return NULL;
    }

    printf("\nA calcular a frequencia de %d tokens...\n", m_tokens.usados);
    // Vou percorrer cada token do vocabulario
    for (int t_idx = 0; t_idx < m_tokens.usados; t_idx++) {
        const char *token_atual = m_tokens.tokens[t_idx];
        int tamanho_token = strlen(token_atual);
        int freq_atual = 0;
        // Para cada token, vou procurar em TODAS as linhas do texto
        for (int l_idx = 0; l_idx < m_texto.linhas_usadas; l_idx++) {
            char *linha = m_texto.linhas[l_idx];
            char *ptr = linha;
            // O strstr procura a palavra na linha.
            while ((ptr = strstr(ptr, token_atual)) != NULL) {
                freq_atual++; // Conto mais um
                ptr += tamanho_token; // Salto a palavra para procurar a seguinte na mesma linha
            }
        }
        // Guardo o total na posicao correspondente ao ID do token
        frequencias[t_idx] = freq_atual;
    }

    *num_frequencias_out = m_tokens.usados;
    printf("Calculo de frequencias concluido.\n");

    return frequencias;
}

// Esta funcao cria o vetor TF (Term Frequency) para UMA FRASE especifica.
// Transforma a lista de IDs [0, 5, 0] num histograma [2, 0, 0, 0, 0, 1]
// (Significa: ID 0 aparece 2 vezes, ID 5 aparece 1 vez)
int* calcularTfParaFrase(int *ids_frase, int num_ids_frase, int tamanho_vocabulario) {
    // 1. Alocar e inicializar o vetor de frequencias com zeros.
    // Mais uma vez, 'calloc' é o nosso melhor amigo aqui para limpar a memoria.
    int *tf_vetor = (int*) calloc(tamanho_vocabulario, sizeof(int));
    if (tf_vetor == NULL) {
        printf("Erro: sem memoria para vetor TF.\n");
        return NULL;
    }
    // 2. Iterar pelos IDs que a tokenizacao nos deu
    for (int i = 0; i < num_ids_frase; i++) {
        int token_id = ids_frase[i];
        // Só conto se for um ID valido.
        // Ignoro o -1 (desconhecido) e IDs que estejam fora do limite (segurança)
        if (token_id >= 0 && token_id < tamanho_vocabulario) {
            // Uso o proprio ID como indice do array.
            // Se o ID é 5, vou à gaveta 5 e somo +1.
            tf_vetor[token_id]++;
        }
    }
    return tf_vetor; // Devolvo o histograma
}
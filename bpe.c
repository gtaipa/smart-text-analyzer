#include "bpe.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// =========================================================
// ESTRUTURAS DE DADOS INTERNAS (Privadas ao BPE)
// =========================================================

// Um nó representa um token na frase (ex: "c", "a", "t")
typedef struct TokenNode {
    char *token;
    struct TokenNode *next;
    struct TokenNode *prev;
} TokenNode;

// Representa uma frase como uma lista ligada de tokens
typedef struct {
    TokenNode *head;
    TokenNode *tail;
} FraseLista;

// Hash Table para contar frequências (R1.3 Otimizado)
typedef struct ParFreq {
    char par[128]; // Guarda "th", "er", etc.
    int frequencia;
    struct ParFreq *proximo;
} ParFreq;

#define HASH_SIZE 2003 // Primo maior para menos colisões

// =========================================================
// FUNÇÕES AUXILIARES (Gestão de Listas e Hash)
// =========================================================

// Função Hash DJB2 (Eficiente para strings)
static unsigned int hash_func(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    return hash % HASH_SIZE;
}

// Converte uma string bruta ("gato") numa lista: [g]<->[a]<->[t]<->[o]
static FraseLista converterStringParaLista(char *linha) {
    FraseLista lista = {NULL, NULL};
    if (!linha) return lista;

    for (int i = 0; linha[i] != '\0'; i++) {
        TokenNode *novo = (TokenNode*)malloc(sizeof(TokenNode));
        // Aloca 2 chars: 1 para a letra, 1 para o terminador \0
        novo->token = (char*)malloc(2 * sizeof(char));
        novo->token[0] = linha[i];
        novo->token[1] = '\0';
        
        novo->next = NULL;
        novo->prev = lista.tail;

        if (lista.tail) {
            lista.tail->next = novo;
        } else {
            lista.head = novo;
        }
        lista.tail = novo;
    }
    return lista;
}

// Liberta a memória de uma lista ligada
static void libertarLista(FraseLista *lista) {
    TokenNode *atual = lista->head;
    while (atual) {
        TokenNode *prox = atual->next;
        free(atual->token);
        free(atual);
        atual = prox;
    }
    lista->head = NULL;
    lista->tail = NULL;
}

// =========================================================
// IMPLEMENTAÇÃO LÓGICA BPE
// =========================================================

// Passo 1: Inicializa o alfabeto com caracteres únicos
int inicializarTokensUnicos(MatrizTexto m_texto, MatrizTokens *m_tokens) {
    unsigned char vistos[256] = {0};
    char token_str[2]; 

    for (int i = 0; i < m_texto.linhas_usadas; i++) {
        char *linha = m_texto.linhas[i];
        for (int j = 0; linha[j] != '\0'; j++) {
            unsigned char c = (unsigned char)linha[j];
            if (vistos[c] == 0) {
                vistos[c] = 1;
                token_str[0] = linha[j];
                token_str[1] = '\0';
                if (!adicionarToken(m_tokens, token_str)) return 0;
            }
        }
    }
    return 1;
}

// Passo 2: Encontrar o par mais frequente (Hash Table)
// Recebe o array de listas ligadas (o corpus em memória dinâmica)
static char *encontrarMelhorPar(FraseLista *corpus, int qtd_linhas, int *freq_out) {
    ParFreq *hash_table[HASH_SIZE] = {0};
    int max_freq = 0;
    char *melhor_par = NULL;

    // Iterar sobre todas as frases (listas ligadas)
    for (int i = 0; i < qtd_linhas; i++) {
        TokenNode *atual = corpus[i].head;
        
        // Iterar sobre os tokens da frase: atual + proximo
        while (atual != NULL && atual->next != NULL) {
            char buffer[128];
            // Construir o par: token_atual + token_seguinte
            snprintf(buffer, sizeof(buffer), "%s%s", atual->token, atual->next->token);

            unsigned int idx = hash_func(buffer);
            
            // Lógica Hash Table (Procura ou Insere)
            ParFreq *no = hash_table[idx];
            ParFreq *ant = NULL;
            int encontrado = 0;

            while (no != NULL) {
                if (strcmp(no->par, buffer) == 0) {
                    no->frequencia++;
                    if (no->frequencia > max_freq) {
                        max_freq = no->frequencia;
                        if (melhor_par) free(melhor_par);
                        melhor_par = strdup(no->par); // Guarda cópia do vencedor
                    }
                    encontrado = 1;
                    break;
                }
                ant = no;
                no = no->proximo;
            }

            if (!encontrado) {
                ParFreq *novo = malloc(sizeof(ParFreq));
                strcpy(novo->par, buffer);
                novo->frequencia = 1;
                novo->proximo = NULL;
                
                if (ant) ant->proximo = novo;
                else hash_table[idx] = novo;

                // Se for o primeiro par de sempre
                if (max_freq == 0) {
                    max_freq = 1;
                    if (melhor_par) free(melhor_par);
                    melhor_par = strdup(buffer);
                }
            }
            atual = atual->next;
        }
    }

    // Limpar memória da Hash Table (CRÍTICO)
    for (int i = 0; i < HASH_SIZE; i++) {
        ParFreq *n = hash_table[i];
        while (n) {
            ParFreq *temp = n;
            n = n->proximo;
            free(temp);
        }
    }

    *freq_out = max_freq;
    return melhor_par;
}

// Passo 3: Fundir o par vencedor nas listas ligadas
static void fundirParNoCorpus(FraseLista *corpus, int qtd_linhas, const char *par_alvo) {
    for (int i = 0; i < qtd_linhas; i++) {
        TokenNode *atual = corpus[i].head;

        while (atual != NULL && atual->next != NULL) {
            char buffer[128];
            snprintf(buffer, sizeof(buffer), "%s%s", atual->token, atual->next->token);

            // Verifica se este par é o vencedor
            if (strcmp(buffer, par_alvo) == 0) {
                // FUNDIR!
                
                // 1. Atualizar texto do nó atual
                char *novo_texto = strdup(buffer);
                free(atual->token);
                atual->token = novo_texto;

                // 2. Remover o nó seguinte
                TokenNode *para_apagar = atual->next;
                atual->next = para_apagar->next;
                
                if (para_apagar->next) {
                    para_apagar->next->prev = atual;
                } else {
                    corpus[i].tail = atual; // Atual agora é o fim
                }

                free(para_apagar->token);
                free(para_apagar);

                // Não avançamos 'atual' imediatamente para evitar double merge
                // Mas avançamos uma vez para não ficar preso se a lógica exigir
            }
            atual = atual->next;
        }
    }
}

// =========================================================
// FUNÇÃO PRINCIPAL (A que o main chama)
// =========================================================

int calcularAlfabetoTokens(MatrizTexto *texto_corpus, MatrizTokens *alfabeto_tokens, int num_tokens_desejado) {
    
    // 1. Inicializar tokens básicos (a, b, c...)
    if (!inicializarTokensUnicos(*texto_corpus, alfabeto_tokens)) {
        return 0;
    }

    // 2. Converter Texto Bruto para Listas Ligadas (para manipulação fácil) 
    // Isto cria a estrutura S mencionada no PDF
    FraseLista *listas_corpus = malloc(sizeof(FraseLista) * texto_corpus->linhas_usadas);
    for (int i = 0; i < texto_corpus->linhas_usadas; i++) {
        listas_corpus[i] = converterStringParaLista(texto_corpus->linhas[i]);
    }

    printf("Iniciando BPE Loop (Alvo: %d tokens)...\n", num_tokens_desejado);

    // 3. Loop BPE
    while (alfabeto_tokens->usados < num_tokens_desejado) {
        int freq;
        char *melhor_par = encontrarMelhorPar(listas_corpus, texto_corpus->linhas_usadas, &freq);

        if (melhor_par == NULL || freq < 1) {
            if (melhor_par) free(melhor_par);
            printf("Nenhum par frequente encontrado. A parar.\n");
            break; // Não há mais nada para fundir
        }

        // Adicionar ao vocabulário
        adicionarToken(alfabeto_tokens, melhor_par);
        // printf("Fusao: '%s' (freq: %d)\n", melhor_par, freq); // Debug opcional

        // Aplicar a fusão nas listas ligadas
        fundirParNoCorpus(listas_corpus, texto_corpus->linhas_usadas, melhor_par);

        free(melhor_par);
    }

    // 4. Limpeza da memória temporária (Listas Ligadas)
    for (int i = 0; i < texto_corpus->linhas_usadas; i++) {
        libertarLista(&listas_corpus[i]);
    }
    free(listas_corpus);

    return 1;
}
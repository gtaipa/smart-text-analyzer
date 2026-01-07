
// BPE.C - Implementação do algoritmo Byte Pair Encoding (BPE)
// Este ficheiro implementa o algoritmo BPE que cria um vocabulário de tokens
// a partir de um texto, fundindo pares de caracteres/tokens mais frequentes.

#include "bpe.h"        // Inclui as declarações das funções públicas do BPE
#include <stdio.h>       // Para printf, fprintf (entrada/saída)
#include <stdlib.h>      // Para malloc, free, exit (gestão de memória)
#include <string.h>      // Para strcmp, strcpy, strdup (manipulação de strings)



// ESTRUTURAS DE DADOS INTERNAS (Privadas ao BPE)


// TokenNode: Representa um único token numa lista ligada
// Exemplo: Se temos "gato", criamos 4 nós: [g] <-> [a] <-> [t] <-> [o]
typedef struct TokenNode {
    char *token;                    // Ponteiro para a string do token (ex: "g", "a", "th")
    struct TokenNode *next;         // Ponteiro para o próximo nó na lista
    struct TokenNode *prev;         // Ponteiro para o nó anterior (lista duplamente ligada)
} TokenNode;

// FraseLista: Representa uma frase completa como uma lista ligada de tokens
// Permite manipular facilmente os tokens (fundir, remover, etc.)
typedef struct {
    TokenNode *head;                // Ponteiro para o primeiro token da frase
    TokenNode *tail;                // Ponteiro para o último token da frase
} FraseLista;

// ParFreq: Usado na hash table para contar frequências de pares de tokens
// Exemplo: Se "th" aparece 10 vezes, guardamos {"th", 10}
typedef struct ParFreq {
    char par[128];                  // O par de tokens concatenado (ex: "th", "er")
    int frequencia;                 // Quantas vezes este par aparece no texto
    struct ParFreq *proximo;        // Ponteiro para o próximo na lista (resolução de colisões)
} ParFreq;

#define HASH_SIZE 2003               // Tamanho da hash table (número primo para menos colisões)


// FUNÇÕES AUXILIARES (Gestão de Listas e Hash)

// hash_func: Calcula um índice na hash table para uma string
// Usa o algoritmo DJB2 que é eficiente para strings
static unsigned int hash_func(const char *str) {
    unsigned long hash = 5381;      // Valor inicial do hash (número mágico do DJB2), 
    int c;                          // Variável para guardar cada caractere
    
    // Percorre cada caractere da string até ao fim ('\0')
    while ((c!= '\0')) {    
        // Fórmula do DJB2: hash = hash * 33 + caractere
        // (hash << 5) é equivalente a hash * 32, depois soma hash (fica *33)
        hash = hash * 33 + c;
    }
    
    // Retorna o resto da divisão pelo tamanho da tabela (garante índice válido)
    return hash % HASH_SIZE;
}

// converterStringParaLista: Converte uma string normal numa lista ligada de tokens
// Exemplo: "gato" -> [g]<->[a]<->[t]<->[o]
static FraseLista converterStringParaLista(char *linha) {
    FraseLista lista = {NULL, NULL};  // Inicializa lista vazia (head e tail são NULL)
    
    // Verifica se a linha existe (proteção contra NULL)
    if (linha == NULL ) return lista;         // Se não existe, retorna lista vazia
    
    // Percorre cada caractere da string
    for (int i = 0; linha[i] != '\0'; i++) {
        // Aloca memória para um novo nó
        TokenNode *novo = (TokenNode*)malloc(sizeof(TokenNode));
        
        // Aloca memória para o token (2 chars: 1 para a letra + 1 para '\0')
        novo->token = (char*)malloc(2 * sizeof(char));
        novo->token[0] = linha[i];    // Copia o caractere atual
        novo->token[1] = '\0';        // Adiciona terminador de string
        
        // Configura os ponteiros do nó
        novo->next = NULL;            // Ainda não sabemos qual é o próximo
        novo->prev = lista.tail;      // O anterior é o que era o último (ou NULL se for o primeiro)
        
        // Liga o novo nó à lista
        if (lista.tail) {             // Se já existe um último nó
            lista.tail->next = novo;  // O último nó agora aponta para o novo
        } else {                      // Se é o primeiro nó
            lista.head = novo;        // O novo nó é o primeiro
        }
        lista.tail = novo;            // O novo nó é agora o último
    }
    
    return lista;                     // Retorna a lista completa
}

// libertarLista: Liberta toda a memória de uma lista ligada
// IMPORTANTE: Deve ser chamada para evitar memory leaks
static void libertarLista(FraseLista *lista) {
    TokenNode *atual = lista->head;   // Começa no primeiro nó
    
    // Percorre todos os nós da lista
    while (atual != NULL) {                   // Enquanto houver nós
        TokenNode *prox = atual->next; // Guarda o próximo ANTES de libertar o atual
        free(atual->token);           // Liberta a string do token
        free(atual);                  // Liberta o nó
        atual = prox;                 // Avança para o próximo
    }
    
    // Limpa os ponteiros da lista
    lista->head = NULL;
    lista->tail = NULL;
}


// IMPLEMENTAÇÃO LÓGICA BPE

// inicializarTokensUnicos: PASSO 1 do BPE
// Identifica todos os caracteres únicos no texto e adiciona ao vocabulário
// Exemplo: "gato gato" -> vocabulário inicial: ['g', 'a', 't', 'o', ' ']
/**
 * @brief Inicializa o alfabeto com todos os caracteres únicos presentes no texto.
 * * Esta função percorre todo o corpus de texto e identifica os caracteres individuais
 * (bytes) únicos. Cada caractere único é adicionado como um token inicial ao vocabulário.
 * É o primeiro passo do algoritmo BPE.
 * * @param m_texto A matriz contendo o corpus de texto original.
 * @param m_tokens Ponteiro para a matriz de tokens onde os caracteres serão guardados.
 * @return int Retorna 1 em caso de sucesso, 0 se houver erro de alocação.
 * * @note **Complexidade:** O(N), onde N é o número total de caracteres no texto, pois
 * faz uma leitura linear completa do corpus.
 */
int inicializarTokensUnicos(MatrizTexto m_texto, MatrizTokens *m_tokens) {
    // Array de 256 posições (um para cada possível byte/char)
    // vistos[i] = 1 significa que o caractere com código i já foi encontrado
    unsigned char vistos[256] = {0};
    
    // String temporária para criar tokens de um caractere
    char token_str[2];
    
    // PASSO 1: Percorrer todas as linhas do texto
    for (int i = 0; i < m_texto.linhas_usadas; i++) {
        char *linha = m_texto.linhas[i];  // Pega a linha atual
        
        // PASSO 2: Percorrer cada caractere da linha
        for (int j = 0; linha[j] != '\0'; j++) {
            // Converte o caractere para unsigned char (0-255)
            unsigned char c = (unsigned char)linha[j];
            
            // PASSO 3: Se este caractere ainda não foi visto
            if (vistos[c] == 0) {
                vistos[c] = 1;              // Marca como visto
                token_str[0] = linha[j];     // Copia o caractere
                token_str[1] = '\0';        // Adiciona terminador
                
                // PASSO 4: Adiciona ao vocabulário
                // Se falhar (retorna 0), retorna erro
                if (!adicionarToken(m_tokens, token_str)) return 0;
            }
        }
    }
    
    return 1;  // Sucesso: todos os caracteres únicos foram adicionados
}

// encontrarMelhorPar: PASSO 2 do BPE
// Encontra qual par de tokens consecutivos aparece mais vezes no texto
// Exemplo: Se "th" aparece 50 vezes e "er" aparece 30, retorna "th"
// Recebe o array de listas ligadas (o corpus em memória dinâmica)


// @brief Encontra o par de tokens que mais se repete no corpus
// @param corpus Array de listas ligadas (o corpus em memória dinâmica)
// @param qtd_linhas Número de linhas no corpus
// @param freq_out Ponteiro para armazenar a frequência do par mais frequente
// @return char* String do par mais frequente
// @note **Complexidade:** O(L * S), onde L é o nº de linhas e S o tamanho médio das strings (uso de strstr).

static char *encontrarMelhorPar(FraseLista *corpus, int qtd_linhas, int *freq_out) {
    // Cria a hash table (array de ponteiros, inicializados a NULL)
    ParFreq *hash_table[HASH_SIZE] = {0};// Inicializa todos os ponteiros a NULL
    
    int max_freq = 0;                // Maior frequência encontrada até agora
    char *melhor_par = NULL;          // String do par mais frequente (será alocada)
    
    // PASSO 1: Percorrer todas as frases (linhas do corpus)
    for (int i = 0; i < qtd_linhas; i++) {
        TokenNode *atual = corpus[i].head;  // Começa no primeiro token da frase
        
        // PASSO 2: Percorrer os tokens da frase (atual + próximo)
        while (atual != NULL && atual->next != NULL) {
            char buffer[128];         // Buffer para segurar o par de tokens , o numero de bytes e suficiente em maior parte dos casos
            
            // PASSO 3: Constrói o par concatenando token_atual + token_seguinte
            // Exemplo: se atual="t" e next="h", buffer fica "th"
            snprintf(buffer, sizeof(buffer), "%s%s", atual->token, atual->next->token);
            
            // PASSO 4: Calcula o índice na hash table para este par
            unsigned int idx = hash_func(buffer);
            
            // PASSO 5: Procura na hash table se este par já existe
            ParFreq *no = hash_table[idx];  // Pega na lista de pares neste índice
            ParFreq *ant = NULL;            // Guarda o nó anterior (para inserção)
            int encontrado = 0;             // Flag: encontramos o par?
            
            // Percorre a lista de pares neste índice da hash table
            while (no != NULL) {
                // Se encontramos o par (mesma string)
                if (strcmp(no->par, buffer) == 0) {
                    no->frequencia++;       // Aumenta a contagem
                    
                    // Se este par agora tem a maior frequência
                    if (no->frequencia > max_freq) {
                        max_freq = no->frequencia;  // Atualiza máximo
                        // Liberta o antigo melhor_par se existir
                        if (melhor_par) free(melhor_par);
                        // Cria cópia do par vencedor
                        melhor_par = strdup(no->par);
                    }
                    encontrado = 1;         // Marca como encontrado
                    break;                  // Sai do loop
                }
                ant = no;                   // Guarda nó anterior
                no = no->proximo;           // Avança para próximo
            }
            
            // PASSO 6: Se o par não foi encontrado, criar novo
            if (!encontrado) {
                ParFreq *novo = malloc(sizeof(ParFreq));  // Aloca novo nó
                strcpy(novo->par, buffer);  // Copia o par
                novo->frequencia = 1;       // Primeira ocorrência
                novo->proximo = NULL;        // Ainda não tem próximo
                
                // Insere na lista (no início ou no fim)
                if (ant) ant->proximo = novo;  // Se havia nó anterior, liga
                else hash_table[idx] = novo;    // Se não, é o primeiro neste índice
                
                // Se é o primeiro par de sempre, já é o melhor
                if (max_freq == 0) {
                    max_freq = 1;
                    if (melhor_par) free(melhor_par);
                    melhor_par = strdup(buffer);
                }
            }
            
            atual = atual->next;      // Avança para o próximo token
        }
    }
    
    // PASSO 7: Limpar memória da hash table (IMPORTANTE para evitar leaks)
    for (int i = 0; i < HASH_SIZE; i++) {
        ParFreq *n = hash_table[i];  // Pega a lista deste índice
        while (n) {                   // Enquanto houver nós
            ParFreq *temp = n;        // Guarda o nó atual
            n = n->proximo;           // Avança
            free(temp);                // Liberta o nó
        }
    }
    
    // PASSO 8: Retorna os resultados
    *freq_out = max_freq;            // Escreve a frequência no parâmetro de saída
    return melhor_par;                // Retorna o par mais frequente (ou NULL se nenhum)
}

// fundirParNoCorpus: PASSO 3 do BPE
// Substitui todas as ocorrências do par vencedor pelo token fundido
// Exemplo: Se par_alvo="th", substitui "t" seguido de "h" por "th" em todo o texto



// @brief Substitui todas as ocorrências do par vencedor pelo token fundido
// @param corpus Array de listas ligadas (o corpus em memória dinâmica)
// @param qtd_linhas Número de linhas no corpus
// @param par_alvo Par de tokens a fundir
// @note **Complexidade:** O(L * S), onde L é o nº de linhas e S o tamanho médio das strings (uso de strstr).
static void fundirParNoCorpus(FraseLista *corpus, int qtd_linhas, const char *par_alvo) {
    // PASSO 1: Percorrer todas as frases
    for (int i = 0; i < qtd_linhas; i++) {
        TokenNode *atual = corpus[i].head;  // Começa no primeiro token
        
        // PASSO 2: Percorrer os tokens da frase
        while (atual != NULL && atual->next != NULL) {
            char buffer[128];
            
            // PASSO 3: Constrói o par atual+próximo
            snprintf(buffer, sizeof(buffer), "%s%s", atual->token, atual->next->token);
            
            // PASSO 4: Verifica se este par é o que queremos fundir
            if (strcmp(buffer, par_alvo) == 0) {
                // ENCONTRAMOS O PAR! Vamos fundir:
                
                // SUBPASSO 4.1: Atualiza o token do nó atual
                // Cria nova string com o par fundido
                char *novo_texto = strdup(buffer);  // strdup aloca e copia
                free(atual->token);                 // Liberta o token antigo
                atual->token = novo_texto;           // Atualiza para o novo token
                
                // SUBPASSO 4.2: Remove o nó seguinte (já foi fundido no atual)
                TokenNode *para_apagar = atual->next;  // Guarda o nó a remover
                atual->next = para_apagar->next;       // Liga atual ao próximo do removido
                
                // Ajusta o ponteiro prev do nó seguinte (se existir)
                if (para_apagar->next) {
                    para_apagar->next->prev = atual;   // O próximo aponta para atual
                } else {
                    corpus[i].tail = atual;            // Se não há próximo, atual é o fim
                }
                
                // Liberta a memória do nó removido
                free(para_apagar->token);
                free(para_apagar);
                
                // NOTA: Não avançamos 'atual' imediatamente para evitar fundir
                // o mesmo par duas vezes seguidas (ex: "thth" -> fundir só uma vez)
            }
            
            atual = atual->next;     // Avança para o próximo token
        }
    }
}


// FUNÇÃO PRINCIPAL (A que o main chama)

/**
 * @brief Calcula o alfabeto de tokens usando BPE até atingir o número desejado.
 * @param texto_corpus Ponteiro para a matriz de texto contendo o corpus.
 * @param alfabeto_tokens Ponteiro para a matriz de tokens onde será construído o vocabulário.
 * @param num_tokens_desejado Número total de tokens desejado no vocabulário final.
 * @return int 1 em caso de sucesso, 0 se houver erro.
 */
int calcularAlfabetoTokens(MatrizTexto *texto_corpus, MatrizTokens *alfabeto_tokens, int num_tokens_desejado) {

    // PASSO 1: Inicializar tokens básicos (a, b, c...)
    // Adiciona todos os caracteres únicos ao vocabulário
    if (!inicializarTokensUnicos(*texto_corpus, alfabeto_tokens)) {
        return 0;  // Se falhar, retorna erro
    }

    // PASSO 2: Converter Texto Bruto para Listas Ligadas
    // Isto permite manipular os tokens facilmente (fundir, remover, etc.)
    // Aloca memória para um array de listas (uma por cada linha do corpus)
    FraseLista *listas_corpus = malloc(sizeof(FraseLista) * texto_corpus->linhas_usadas);
    
    // Converte cada linha do texto numa lista ligada de tokens
    for (int i = 0; i < texto_corpus->linhas_usadas; i++) {
        listas_corpus[i] = converterStringParaLista(texto_corpus->linhas[i]);
    }

    printf("Iniciando BPE Loop (Alvo: %d tokens)...\n", num_tokens_desejado);

    // PASSO 3: Loop BPE - Repete até ter tokens suficientes
    while (alfabeto_tokens->usados < num_tokens_desejado) {
        int freq;  // Variável para guardar a frequência do melhor par
        
        // SUBPASSO 3.1: Encontrar o par mais frequente
        char *melhor_par = encontrarMelhorPar(listas_corpus, texto_corpus->linhas_usadas, &freq);

        // Se não encontrou nenhum par ou frequência é 0, parar
        if (melhor_par == NULL || freq < 1) {
            if (melhor_par) free(melhor_par);  // Liberta memória se foi alocada
            printf("Nenhum par frequente encontrado. A parar.\n");
            break;  // Sai do loop
        }

        // SUBPASSO 3.2: Adicionar o par fundido ao vocabulário
        // Exemplo: Se melhor_par="th", adiciona "th" como novo token
        adicionarToken(alfabeto_tokens, melhor_par);

        // SUBPASSO 3.3: Aplicar a fusão nas listas ligadas
        // Substitui todas as ocorrências do par pelo token fundido
        fundirParNoCorpus(listas_corpus, texto_corpus->linhas_usadas, melhor_par);

        free(melhor_par);  // Liberta a memória do melhor_par (já foi usado)
    }

    // PASSO 4: Limpeza da memória temporária (Listas Ligadas)
    // IMPORTANTE: Libertar toda a memória alocada para evitar leaks
    for (int i = 0; i < texto_corpus->linhas_usadas; i++) {
        libertarLista(&listas_corpus[i]);  // Liberta cada lista
    }
    free(listas_corpus);  // Liberta o array de listas

    return 1;  // Sucesso: vocabulário criado
}

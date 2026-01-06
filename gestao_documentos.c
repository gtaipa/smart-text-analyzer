// ============================================================================
// GESTAO_DOCUMENTOS.C - Gestão de Documentos e Lista Ligada
// ============================================================================
// Este ficheiro implementa uma lista ligada de documentos, onde cada documento
// contém: título, texto original, IDs de tokens, e vetor TF (Term Frequency).

#include "gestao_documentos.h"  // Declarações das estruturas e funções públicas
#include "files.h"               // Para funções de ficheiros (se necessário)
#include <string.h>              // Para memcpy, strlen (manipulação de strings)


// ============================================================================
// Funções Auxiliares (Privadas a este ficheiro)
// ============================================================================

// duplicarString: Cria uma cópia independente de uma string
// IMPORTANTE: A string retornada deve ser libertada com free()
// Porquê? Para evitar problemas quando a string original é libertada
char *duplicarString(const char *s) {
    // Verifica se a string é válida (proteção contra NULL)
    if (s == NULL) return NULL;
    
    // Calcula o tamanho necessário: strlen(s) + 1 (para o '\0')
    size_t tamanho = strlen(s) + 1;
    
    // Aloca memória para a cópia
    char *nova_copia = (char*)malloc(tamanho);
    
    // Verifica se a alocação foi bem-sucedida
    if (nova_copia == NULL) {
        return NULL;  // Se falhar, retorna NULL
    }
    
    // Copia os bytes da string original para a nova
    // memcpy é mais eficiente que strcpy para este caso
    memcpy(nova_copia, s, tamanho);
    
    return nova_copia;  // Retorna a nova string (deve ser libertada depois)
}


// priv_criarDocumentoNode: Cria um nó completo de documento
// Esta função faz TODO o processamento: tokenização(é o processo de conversão de texto em tokens) e vetorização(é o processo de conversão de tokens em vetores)
// É privada (static) porque só é usada internamente neste ficheiro
static TK_TF_NODE* priv_criarDocumentoNode(const char *titulo, const char *texto, MatrizTokens *vocab) {
    // Verificação de segurança: vocabulário deve existir e ter tokens
    if (vocab == NULL || vocab->usados == 0) return NULL;

    // PASSO 1: Alocar memória para a estrutura do documento
    TK_TF_NODE *novo_doc = (TK_TF_NODE*) malloc(sizeof(TK_TF_NODE)); //TK_TF_NODE significa que a alocação é para um nó de documento TK é a abreviatura de Term Frequency - Frequency Count Vector
    
    // Se falhar a alocação, retorna NULL
    if (novo_doc == NULL) return NULL;

    // PASSO 2: Guardar o tamanho do vocabulário atual
    // Isto é importante porque o vocabulário pode crescer depois
    int vocab_size = vocab->usados;

    // PASSO 3: Duplicar título e texto (criar cópias independentes)
    // Porquê duplicar? Para garantir que mesmo que as strings originais
    // sejam libertadas, o documento mantém os seus dados
    novo_doc->titulo = duplicarString(titulo);
    novo_doc->texto_original = duplicarString(texto);
    novo_doc->vocab_size = vocab_size;  // Guarda o tamanho do vocabulário
    novo_doc->proximo = NULL;           // Ainda não sabemos qual é o próximo documento

    // PASSO 4: TOKENIZAÇÃO
    // Converte o texto numa sequência de IDs de tokens
    // Exemplo: "o gato" -> [5, 12, 8] (IDs dos tokens no vocabulário)
    int n_ids = 0;  // Variável para receber o número de IDs encontrados
    int *ids_array = tokenizarFrase(texto, *vocab, &n_ids);// *ids_array é o array de IDs dos tokens , e atribuimos o valor de n_ids para o número de IDs encontrados
    
    // Se a tokenização falhar, é necessário libertar tudo que já foi alocado
    // ANTES de sair da função (evitar memory leaks)
    if (ids_array == NULL) {
        free(novo_doc->titulo);        // Liberta o título
        free(novo_doc->texto_original); // Liberta o texto
        free(novo_doc);                 // Liberta a estrutura
        return NULL;                    // Retorna erro
    }
    
    // Guarda o array de IDs e a quantidade no documento
    novo_doc->ids_tokens = ids_array;
    novo_doc->num_ids = n_ids;

    // PASSO 5: VETORIZAÇÃO (TF - Term Frequency)
    // Calcula o vetor de frequências do documento
    // Cada posição indica quantas vezes um token aparece no texto
    // Exemplo: Se vocab tem ["o", "gato", "corre"], e o texto é "o gato o"
    // O vetor TF será: [2, 1, 0] (token "o" aparece 2x, "gato" 1x, "corre" 0x)
    int *tf_vetor = calcularTfParaFrase(ids_array, n_ids, vocab_size);

    // Se o cálculo do TF falhar, liberta toda a memória associada ao documento
    if (tf_vetor == NULL) {
        free(novo_doc->titulo);        // Liberta o título
        free(novo_doc->texto_original); // Liberta o texto
        free(novo_doc->ids_tokens);     // Liberta o array de IDs
        free(novo_doc);                 // Liberta a estrutura
        return NULL;                    // Retorna erro
    }
    
    // Guarda o vetor TF no documento
    novo_doc->vetor_tf = tf_vetor;

    return novo_doc;  // Retorna o documento completo e processado
}

// priv_libertarDocNode: Liberta toda a memória de UM documento
// É uma função privada (static) porque só é usada neste ficheiro
static void priv_libertarDocNode(TK_TF_NODE *doc) {
    // Se o ponteiro for NULL, não há nada para libertar
    if (doc == NULL) return;

    // Liberta cada campo que foi alocado dinamicamente:
    if (doc->titulo) free(doc->titulo);              // Liberta o título
    if (doc->texto_original) free(doc->texto_original); // Liberta o texto original
    if (doc->ids_tokens) free(doc->ids_tokens);       // Liberta o array de IDs dos tokens
    if (doc->vetor_tf) free(doc->vetor_tf);           // Liberta o vetor de frequências

    free(doc);  // Por fim, liberta a própria estrutura do documento
}


// ============================================================================
// Funções da Lista Ligada (R2.1 e R2.2)
// ============================================================================

// criarListaDocumentos: Cria e inicializa uma lista de documentos vazia
// Apenas associa o vocabulário que será usado pelos documentos
/**
 * @brief Inicializa uma lista de documentos vazia.
 * @param vocab Ponteiro para o vocabulário que será usado na tokenização dos documentos.
 * @return LL_TK_TF Lista inicializada.
 */
LL_TK_TF criarListaDocumentos(MatrizTokens *vocab) {
    LL_TK_TF lista;                    // Cria uma nova estrutura de lista
    
    lista.inicio = NULL;               // No início não existem documentos (lista vazia)
    lista.num_documentos = 0;          // Zero documentos
    lista.vocabulario = vocab;         // Guarda o ponteiro para o vocabulário
    
    return lista;                      // Retorna a lista inicializada
}

// adicionarDocumentoLista: Adiciona um novo documento à lista
// O documento é inserido no fim (índice = num_documentos atual)
/**
 * @brief Adiciona um documento ao final da lista.
 * Esta função chama internamente as rotinas de tokenização e vetorização.
 * @param lista Ponteiro para a lista.
 * @param titulo Título do documento.
 * @param texto_original Conteúdo do documento.
 * @return int 1 se sucesso, 0 se erro.
 * @note **Complexidade:** O(N) para percorrer a lista até ao fim + custo da tokenização.
 */
int adicionarDocumentoLista(LL_TK_TF *lista, const char *titulo, const char *texto_original) {
    // Reutiliza a função de inserção genérica, garantindo que o documento
    // fica na posição correta (no fim = índice = num_documentos)
    return inserirDocumentoNoIndice(lista, titulo, texto_original, lista->num_documentos);
}

// listarDocumentos: Mostra no ecrã todos os documentos da lista
// Mostra o índice, título e número de tokens de cada documento
/**
 * @brief Percorre a lista e imprime os dados básicos de cada documento.
 * @param lista A lista a imprimir.
 */
void listarDocumentos(LL_TK_TF lista) {
    printf("\n--- Lista de Documentos (%d) ---\n", lista.num_documentos);
    TK_TF_NODE *atual = lista.inicio;  // Começa no primeiro documento
    
    // Caso especial: lista vazia
    if (atual == NULL) {
        printf("Lista vazia.\n");
        return;  // Sai da função se não há documentos
    }
    
    int i = 0;  // Contador para o índice do documento
    
    // Percorre toda a lista ligada
    while (atual != NULL) {
        // Mostra informação essencial de cada documento:
        // [índice] Título: ... (Tem X Tokens)
        printf("[%d] Titulo: %s (Tem %d Tokens)\n", i, atual->titulo, atual->num_ids);
        
        atual = atual->proximo;  // Avança para o próximo documento
        i++;                     // Incrementa o índice
    }
}

// libertarListaDocumentos: Liberta toda a lista de documentos
// Percorre a lista ligada e liberta cada nó individualmente
/**
 * @brief Liberta toda a memória da lista (incluindo todos os nós e arrays internos).
 * @param lista Ponteiro para a lista a destruir.
 */
void libertarListaDocumentos(LL_TK_TF *lista) {
    TK_TF_NODE *atual = lista->inicio;  // Começa no primeiro documento
    
    // Percorre a lista inteira
    while (atual != NULL) {
        TK_TF_NODE *prox = atual->proximo;  // Guarda o próximo nó ANTES de libertar o atual
        // (Importante! Se libertássemos primeiro, perderíamos a referência ao próximo)
        
        priv_libertarDocNode(atual);  // Liberta toda a memória do documento atual
        atual = prox;                 // Avança para o próximo documento
    }
    
    // Depois de libertar tudo, a lista fica vazia
    lista->inicio = NULL;
    lista->num_documentos = 0;
}


// ============================================================================
// Funções de Manipulação Avançada
// ============================================================================

// obterDocumentoPorIndice: Devolve o documento que está numa determinada posição
// Se o índice for inválido, devolve NULL
/**
 * @brief Retorna um ponteiro para o documento num determinado índice.
 * @param lista A lista de documentos.
 * @param indice A posição desejada (base 0).
 * @return TK_TF_NODE* Ponteiro para o nó ou NULL se o índice for inválido.
 * @note **Complexidade:** O(K), onde K é o índice solicitado (acesso sequencial).
 */
TK_TF_NODE* obterDocumentoPorIndice(LL_TK_TF lista, int indice) {
    // Verifica se o índice está dentro dos limites da lista
    // Índice válido: 0 <= indice < num_documentos
    if (indice < 0 || indice >= lista.num_documentos) {
        return NULL;  // Índice inválido
    }
    
    TK_TF_NODE *atual = lista.inicio;  // Começa no início da lista
    int contador = 0;                  // Contador para acompanhar a posição
    
    // Percorre a lista ligada até chegar à posição desejada
    while (atual != NULL) {
        // Quando o contador coincide com o índice, encontramos o documento pretendido
        if (contador == indice) {
            return atual;  // Retorna o ponteiro para o documento
        }
        
        atual = atual->proximo;  // Avança para o próximo nó
        contador++;               // Incrementa o contador
    }
    
    // Se por algum motivo não encontrar (situação improvável),
    // devolve NULL por segurança
    return NULL;
}

// inserirDocumentoNoIndice: Insere um novo documento numa posição específica
// O índice indica onde o documento deve ficar (0 = início, N = fim)
/**
 * @brief Insere um novo documento numa posição específica da lista.
 * @param lista Ponteiro para a lista.
 * @param titulo Título do novo documento.
 * @param texto Conteúdo do novo documento.
 * @param indice Posição onde inserir (0 = início, N = fim).
 * @return int 1 se sucesso, 0 se erro ou índice inválido.
 * @note **Complexidade:** O(K) para chegar à posição de inserção.
 */
int inserirDocumentoNoIndice(LL_TK_TF *lista, const char *titulo, const char *texto, int indice) {
    // Verifica se o índice é válido para inserção
    // Índice válido: 0 <= indice <= num_documentos
    // (pode ser igual ao número de documentos para inserir no fim)
    if (indice < 0 || indice > lista->num_documentos) {
        printf("Erro: Indice invalido para insercao.\n");
        return 0;  // Retorna erro
    }
    
    // Cria um novo nó de documento (com tokenização e TF incluídos)
    // Esta função faz TODO o processamento necessário
    TK_TF_NODE *novo_doc = priv_criarDocumentoNode(titulo, texto, lista->vocabulario);
    
    // Se a criação falhar, aborta a inserção
    if (novo_doc == NULL) return 0;

    // CASO ESPECIAL: Inserir no início da lista (índice 0)
    if (indice == 0) {
        novo_doc->proximo = lista->inicio;  // O novo nó passa a apontar para o antigo primeiro
        lista->inicio = novo_doc;           // O novo nó torna-se o novo início da lista
    }
    else {
        // CASO GERAL: Inserir no meio ou no fim
        // Percorre a lista até ao nó anterior à posição de inserção
        TK_TF_NODE *anterior = lista->inicio;
        for (int i = 0; i < indice - 1; i++) {
            anterior = anterior->proximo;  // Avança até chegar ao nó anterior
        }
        
        // Ajusta os ponteiros para inserir o novo nó:
        // 1. O novo nó aponta para o que estava na posição de inserção
        novo_doc->proximo = anterior->proximo;
        // 2. O nó anterior aponta para o novo nó
        anterior->proximo = novo_doc;
    }
    
    // Atualiza o número total de documentos
    lista->num_documentos++;
    return 1;  // Sucesso
}

// removerDocumentoPorIndice: Remove o documento que está numa determinada posição
/**
 * @brief Remove e liberta a memória de um documento numa posição específica.
 * @param lista Ponteiro para a lista.
 * @param indice Índice do documento a remover.
 * @return int 1 se sucesso, 0 se erro.
 */
int removerDocumentoPorIndice(LL_TK_TF *lista, int indice) {
    // Verifica se a remoção é possível
    // Condições: índice válido E lista não vazia
    if (indice < 0 || indice >= lista->num_documentos || lista->inicio == NULL) {
        printf("Erro: Impossivel remover (Indice invalido ou lista vazia).\n");
        return 0;  // Retorna erro
    }
    
    TK_TF_NODE *para_remover = NULL;  // Ponteiro para o nó a remover
    
    // CASO ESPECIAL: Remover o primeiro documento (índice 0)
    if (indice == 0) {
        para_remover = lista->inicio;           // Guarda o nó a remover
        lista->inicio = lista->inicio->proximo; // Atualiza o início da lista
        // (O segundo documento passa a ser o primeiro)
    }
    else {
        // CASO GERAL: Remover do meio ou do fim
        // Percorre até ao nó anterior ao que vai ser removido
        TK_TF_NODE *anterior = lista->inicio;
        for (int i = 0; i < indice - 1; i++) {
            anterior = anterior->proximo;  // Avança até chegar ao nó anterior
        }
        
        // Ajusta os ponteiros para "saltar" o nó removido:
        para_remover = anterior->proximo;        // Guarda o nó a remover
        anterior->proximo = para_remover->proximo; // Liga anterior ao próximo do removido
    }
    
    // Mensagem informativa (útil para debug)
    printf("A remover documento: %s\n", para_remover->titulo);
    
    // Liberta toda a memória associada ao documento
    priv_libertarDocNode(para_remover);
    
    // Atualiza o número total de documentos
    lista->num_documentos--;
    return 1;  // Sucesso
}

// pesquisarDocumentosPorTokenID: Procura em todos os documentos
// quais contêm um determinado token (identificado pelo seu ID)
// Mostra também quantas vezes o token aparece em cada documento
/**
 * @brief Pesquisa em toda a lista quais os documentos que contêm um determinado Token ID.
 * Utiliza o vetor TF para verificar se a frequência é > 0.
 * @param lista A lista de documentos.
 * @param token_id O ID do token a pesquisar.
 * @note **Complexidade:** O(N), onde N é o número de documentos na lista.
 */
void pesquisarDocumentosPorTokenID(LL_TK_TF lista, int token_id) {
    printf("\n--- Pesquisa por Token ID [%d] ---\n", token_id);
    
    // Verifica se o vocabulário existe e se o token_id está dentro dos limites válidos
    // token_id válido: 0 <= token_id < vocab->usados
    if (lista.vocabulario == NULL || token_id < 0 || token_id >= lista.vocabulario->usados) {
        printf("Token ID invalido.\n");
        return;  // Sai da função se o ID é inválido
    }
    
    // Mostra ao utilizador qual é a palavra associada ao ID
    printf("A procurar documentos com a palavra: \"%s\"\n", lista.vocabulario->tokens[token_id]);

    int encontrados = 0;              // Contador de documentos encontrados
    TK_TF_NODE *atual = lista.inicio; // Começa no primeiro documento da lista
    int doc_idx = 0;                  // Índice do documento (posição na lista)

    // Percorre todos os documentos da lista ligada
    while (atual != NULL) {
        // Confirma que o documento tem vetor TF e que o token_id é válido para este vetor
        if (atual->vetor_tf != NULL && atual->vocab_size > token_id) {
            // Pega a frequência deste token no documento
            // O vetor_tf[token_id] indica quantas vezes o token aparece
            int frequencia = atual->vetor_tf[token_id];

            // Se a frequência for maior que zero, então o documento contém esta palavra
            if (frequencia > 0) {
                printf(" -> Encontrado no Doc [%d] \"%s\" (Aparece %d vezes)\n",
                       doc_idx, atual->titulo, frequencia);
                encontrados++;  // Incrementa o contador
            }
        }
        
        // Avança para o próximo documento
        atual = atual->proximo;
        doc_idx++;  // Incrementa o índice
    }
    
    // Se nenhum documento contiver a palavra, informa o utilizador
    if (encontrados == 0) {
        printf("Nenhum documento contem esta palavra.\n");
    }
}

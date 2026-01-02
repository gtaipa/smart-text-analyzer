#include "gestao_documentos.h"
#include "files.h"
#include <string.h>


//  Funções Auxiliares (Privadas a este ficheiro)


// Esta funcao serve para criar copias seguras das strings (Titulos e Textos).
char *duplicarString(const char *s) {
    if (s == NULL) return NULL;
    size_t tamanho = strlen(s) + 1;
    char *nova_copia = (char*)malloc(tamanho);
    if (nova_copia == NULL) {
        return NULL;
    }
    memcpy(nova_copia, s, tamanho);
    return nova_copia;
}

// Esta funcao cria um "Nó" (TK_TF_NODE) com toda a informacao de um documento.
static TK_TF_NODE* priv_criarDocumentoNode(const char *titulo, const char *texto, MatrizTokens *vocab) {
    // Verificacao de seguranca
    if (vocab == NULL || vocab->usados == 0) return NULL;

    // 1. Aloco memoria para a estrutura do documento
    TK_TF_NODE *novo_doc = (TK_TF_NODE*) malloc(sizeof(TK_TF_NODE));
    if (novo_doc == NULL) return NULL;

    // 2. Guardo o tamanho do vocabulario atual
    int vocab_size = vocab->usados;

    // 3. Duplico titulo e texto
    novo_doc->titulo = duplicarString(titulo);
    novo_doc->texto_original = duplicarString(texto);
    novo_doc->vocab_size = vocab_size;
    novo_doc->proximo = NULL;

    // 4. Tokenizacao
    // Converte o texto do documento numa sequência de IDs de tokens.
    // A função devolve:
    // - um array de inteiros
    // - o número total de IDs através de n_ids
    int n_ids = 0;
    int *ids_array = tokenizarFrase(texto, *vocab, &n_ids);
    // se a tokenização falhar é necessario libertar tudo o que já foi alocado antes de sair da função
    if (ids_array == NULL) {
        free(novo_doc->titulo);
        free(novo_doc->texto_original);
        free(novo_doc);
        return NULL;
    }
    // Guarda o array de IDs e a sua quantidade no documento
    novo_doc->ids_tokens = ids_array;
    novo_doc->num_ids = n_ids;

    // 5. Vetorizacao (TF)
    // Calcula o vetor de frequencias do documento
    //cada posição indica quantas vezes um token aparece no texto
    int *tf_vetor = calcularTfParaFrase(ids_array, n_ids, vocab_size);

    // se o calculo do TF falhar liberta toda a memoria associada ao documento
    if (tf_vetor == NULL) {
        free(novo_doc->titulo);
        free(novo_doc->texto_original);
        free(novo_doc->ids_tokens);
        free(novo_doc);
        return NULL;
    }
    //Guarda o vetor TF no documento
    novo_doc->vetor_tf = tf_vetor;

    return novo_doc;
}

// Funcao auxiliar para limpar a memoria de UM UNICO documento
// Liberta toda a memória associada a UM documento.
// É uma função privada (static) porque só é usada neste ficheiro.
static void priv_libertarDocNode(TK_TF_NODE *doc) {
    if (doc == NULL) return;// se o ponteiro for Null, não há nada para libertar

    if (doc->titulo) free(doc->titulo);// liberta o titulo
    if (doc->texto_original) free(doc->texto_original);//liberta o texto original
    if (doc->ids_tokens) free(doc->ids_tokens);//liberta o array de IDs dos tokens
    if (doc->vetor_tf) free(doc->vetor_tf);// Liberta o vetor de frequencias

    free(doc);// por fim, liberta o proprio no de documento
}


//  Funções da Lista Ligada (R2.1 e R2.2)


// Cria e inicializa uma lista de documentos vazia.
// Apenas associa o vocabulário que será usado pelos documentos.
LL_TK_TF criarListaDocumentos(MatrizTokens *vocab) {
    LL_TK_TF lista;
    lista.inicio = NULL; // No início não existem documentos
    lista.num_documentos = 0;
    lista.vocabulario = vocab;
    return lista;
}
// Adiciona um novo documento à lista.
// O documento é inserido no fim (índice = num_documentos atual).
int adicionarDocumentoLista(LL_TK_TF *lista, const char *titulo, const char *texto_original) {// Reutiliza a função de inserção genérica,garantindo que o documento fica na posição correta
    return inserirDocumentoNoIndice(lista, titulo, texto_original, lista->num_documentos);
}
// Mostra no ecrã todos os documentos da lista,com o respetivo índice, título e número de tokens.

void listarDocumentos(LL_TK_TF lista) {
    printf("\n--- Lista de Documentos (%d) ---\n", lista.num_documentos);
    TK_TF_NODE *atual = lista.inicio;// Começa no primeiro documento

    // Caso especial: lista vazia
    if (atual == NULL) {
        printf("Lista vazia.\n");
        return;
    }
    int i = 0;
    while (atual != NULL) {// Percorre toda a lista ligada
        printf("[%d] Titulo: %s (Tem %d Tokens)\n", i, atual->titulo, atual->num_ids);// Mostra informação essencial de cada documento
        atual = atual->proximo;// Avança para o próximo documento
        i++;
    }
}
// Liberta toda a lista de documentos.
// Percorre a lista ligada e liberta cada nó individualmente.
void libertarListaDocumentos(LL_TK_TF *lista) {
    TK_TF_NODE *atual = lista->inicio;  // Começa no primeiro documento
    while (atual != NULL) {// Percorre a lista inteira
        TK_TF_NODE *prox = atual->proximo;// Guarda o próximo nó antes de libertar o atual
        priv_libertarDocNode(atual);// Liberta toda a memória do documento atual
        atual = prox;// Avança para o próximo documento
    }
    // Depois de libertar tudo, a lista fica vazia
    lista->inicio = NULL;
    lista->num_documentos = 0;
}


//  Funcoes de Manipulacao Avancada

// Devolve o documento que está numa determinada posição da lista.
// Se o índice for inválido, devolve NULL.
TK_TF_NODE* obterDocumentoPorIndice(LL_TK_TF lista, int indice) {
    if (indice < 0 || indice >= lista.num_documentos) {// Verifica se o índice está dentro dos limites da lista
        return NULL;
    }
    TK_TF_NODE *atual = lista.inicio;// Começa no início da lista
    int contador = 0;
    while (atual != NULL) {// Percorre a lista ligada até chegar à posição desejada
        if (contador == indice) {// Quando o contador coincide com o índice encontramos o documento pretendido
            return atual;
        }
        atual = atual->proximo;// Avança para o próximo nó
        contador++;
    }
    // Se por algum motivo não encontrar (situação improvável),
    // devolve NULL por segurança
    return NULL;
}
// Insere um novo documento numa posição específica da lista.
// O índice indica onde o documento deve ficar.
int inserirDocumentoNoIndice(LL_TK_TF *lista, const char *titulo, const char *texto, int indice) {
    // Verifica se o índice é válido para inserção
    // (pode ser igual ao número de documentos para inserir no fim)
    if (indice < 0 || indice > lista->num_documentos) {
        printf("Erro: Indice invalido para insercao.\n");
        return 0;
    }
    // Cria um novo nó de documento (com tokenização e TF incluídos)
    TK_TF_NODE *novo_doc = priv_criarDocumentoNode(titulo, texto, lista->vocabulario);
    if (novo_doc == NULL) return 0;// Se a criação falhar, aborta a inserção

    if (indice == 0) {// Caso especial: inserir no início da lista
        novo_doc->proximo = lista->inicio;// O novo nó passa a apontar para o antigo primeiro
        lista->inicio = novo_doc;// O novo nó torna-se o novo início da lista
    }
    else {
        // Percorre a lista até ao nó anterior à posição de inserção
        TK_TF_NODE *anterior = lista->inicio;
        for (int i = 0; i < indice - 1; i++) {
            anterior = anterior->proximo;
        }
        // Ajusta os ponteiros para inserir o novo nó no meio/fim
        novo_doc->proximo = anterior->proximo;
        anterior->proximo = novo_doc;
    }
    // Atualiza o número total de documentos
    lista->num_documentos++;
    return 1;
}
// Remove o documento que está numa determinada posição da lista.
int removerDocumentoPorIndice(LL_TK_TF *lista, int indice) {
    // Verifica se a remoção é possível
    if (indice < 0 || indice >= lista->num_documentos || lista->inicio == NULL) {
        printf("Erro: Impossivel remover (Indice invalido ou lista vazia).\n");
        return 0;
    }
    TK_TF_NODE *para_remover = NULL;
    // Caso especial: remover o primeiro documento

    if (indice == 0) {
        para_remover = lista->inicio;// Guarda o nó a remover
        lista->inicio = lista->inicio->proximo;// Atualiza o início da lista
    }
    else {
        TK_TF_NODE *anterior = lista->inicio;// Percorre até ao nó anterior ao que vai ser removido
        for (int i = 0; i < indice - 1; i++) {
            anterior = anterior->proximo;
        }
        para_remover = anterior->proximo;// Ajusta os ponteiros para "saltar" o nó removido
        anterior->proximo = para_remover->proximo;
    }
    // Mensagem informativa (útil para debug)
    printf("A remover documento: %s\n", para_remover->titulo);
    priv_libertarDocNode(para_remover);// Liberta toda a memória associada ao documento
    lista->num_documentos--;// Atualiza o número total de documentos
    return 1;
}
// Procura em todos os documentos aqueles que contêm
// um determinado token (identificado pelo seu ID).
// Mostra também quantas vezes o token aparece em cada documento.
void pesquisarDocumentosPorTokenID(LL_TK_TF lista, int token_id) {
    printf("\n--- Pesquisa por Token ID [%d] ---\n", token_id);
    // Verifica se o vocabulário existe
    // e se o token_id está dentro dos limites válidos
    if (lista.vocabulario == NULL || token_id < 0 || token_id >= lista.vocabulario->usados) {
        printf("Token ID invalido.\n");
        return;
    }
    // Mostra ao utilizador qual é a palavra associada ao ID
    printf("A procurar documentos com a palavra: \"%s\"\n", lista.vocabulario->tokens[token_id]);

    int encontrados = 0;
    TK_TF_NODE *atual = lista.inicio;// Começa no primeiro documento da lista
    int doc_idx = 0;// Índice do documento (posição na lista)


    // Percorre todos os documentos da lista ligada
    while (atual != NULL) {
        // Confirma que o documento tem vetor TF
        // e que o token_id é válido para este vetor
        if (atual->vetor_tf != NULL && atual->vocab_size > token_id) {
            // Se a frequência for maior que zero,
            // então o documento contém esta palavra
            int frequencia = atual->vetor_tf[token_id];

            if (frequencia > 0) {
                printf(" -> Encontrado no Doc [%d] \"%s\" (Aparece %d vezes)\n",
                       doc_idx, atual->titulo, frequencia);
                encontrados++;
            }
        }
        // Avança para o próximo documento
        atual = atual->proximo;
        doc_idx++;
    }
    // Se nenhum documento contiver a palavra,
    // informa o utilizador
    if (encontrados == 0) {
        printf("Nenhum documento contem esta palavra.\n");
    }
}
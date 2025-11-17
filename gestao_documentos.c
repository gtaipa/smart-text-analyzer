#include "gestao_documentos.h"

// =============================================================
//  Funcoes Privadas (Auxiliares)
// =============================================================

// (A tua funcao static original)
static t_doc* priv_criarDocumentoNode(const char *titulo, const char *texto, MatrizTokens *vocab) {
    if (vocab == NULL || vocab->usados == 0) return NULL;

    t_doc *novo_doc = (t_doc*) malloc(sizeof(t_doc));
    if (novo_doc == NULL) return NULL;

    int vocab_size = vocab->usados;

    novo_doc->titulo = strdup(titulo);
    novo_doc->texto_original = strdup(texto);
    novo_doc->vocab_size = vocab_size;
    novo_doc->proximo = NULL;

    int n_ids = 0;
    int *ids_array = tokenizarFrase(texto, *vocab, &n_ids);
    if (ids_array == NULL) {
        free(novo_doc->titulo); free(novo_doc->texto_original); free(novo_doc);
        return NULL;
    }
    novo_doc->ids_tokens = ids_array;
    novo_doc->num_ids = n_ids;

    int *tf_vetor = calcularTfParaFrase(ids_array, n_ids, vocab_size);
    if (tf_vetor == NULL) {
        free(novo_doc->titulo); free(novo_doc->texto_original);
        free(novo_doc->ids_tokens); free(novo_doc);
        return NULL;
    }
    novo_doc->vetor_tf = tf_vetor;

    return novo_doc;
}

// Nova auxiliar para limpar a memória de um único nó
static void priv_libertarDocNode(t_doc *doc) {
    if (doc == NULL) return;
    if (doc->titulo) free(doc->titulo);
    if (doc->texto_original) free(doc->texto_original);
    if (doc->ids_tokens) free(doc->ids_tokens);
    if (doc->vetor_tf) free(doc->vetor_tf);
    free(doc);
}

// =============================================================
//  Funções R2.1 e R2.2 (Existentes)
// =============================================================

t_lista_docs criarListaDocumentos(MatrizTokens *vocab) {
    t_lista_docs lista;
    lista.inicio = NULL;
    lista.num_documentos = 0;
    lista.vocabulario = vocab;
    return lista;
}

int adicionarDocumentoLista(t_lista_docs *lista, const char *titulo, const char *texto_original) {
    // Reutilizamos a inserção por índice para simplificar
    // Inserir no indice == num_documentos coloca no fim
    return inserirDocumentoNoIndice(lista, titulo, texto_original, lista->num_documentos);
}

void listarDocumentos(t_lista_docs lista) {
    printf("\n--- Lista de Documentos (%d) ---\n", lista.num_documentos);
    t_doc *atual = lista.inicio;
    if (atual == NULL) {
        printf("Lista vazia.\n");
        return;
    }
    int i = 0;
    while (atual != NULL) {
        printf("[%d] Titulo: %s (Tokens: %d)\n", i, atual->titulo, atual->num_ids);
        atual = atual->proximo;
        i++;
    }
}

void libertarListaDocumentos(t_lista_docs *lista) {
    t_doc *atual = lista->inicio;
    while (atual != NULL) {
        t_doc *prox = atual->proximo;
        priv_libertarDocNode(atual); // Usa a auxiliar
        atual = prox;
    }
    lista->inicio = NULL;
    lista->num_documentos = 0;
}

// =============================================================
//  NOVAS FUNÇÕES (Completando o R2.1)
// =============================================================

t_doc* obterDocumentoPorIndice(t_lista_docs lista, int indice) {
    if (indice < 0 || indice >= lista.num_documentos) {
        return NULL;
    }

    t_doc *atual = lista.inicio;
    int contador = 0;
    while (atual != NULL) {
        if (contador == indice) {
            return atual;
        }
        atual = atual->proximo;
        contador++;
    }
    return NULL;
}

int inserirDocumentoNoIndice(t_lista_docs *lista, const char *titulo, const char *texto, int indice) {
    if (indice < 0 || indice > lista->num_documentos) {
        printf("Erro: Indice %d invalido para insercao.\n", indice);
        return 0;
    }

    // 1. Criar o nó completo
    t_doc *novo_doc = priv_criarDocumentoNode(titulo, texto, lista->vocabulario);
    if (novo_doc == NULL) return 0;

    // 2. Caso especial: Inserir no início (Head)
    if (indice == 0) {
        novo_doc->proximo = lista->inicio;
        lista->inicio = novo_doc;
    }
    else {
        // 3. Caso normal: Encontrar o anterior (indice - 1)
        t_doc *anterior = lista->inicio;
        for (int i = 0; i < indice - 1; i++) {
            anterior = anterior->proximo;
        }
        // Ligar os fios
        novo_doc->proximo = anterior->proximo;
        anterior->proximo = novo_doc;
    }

    lista->num_documentos++;
    return 1;
}

int removerDocumentoPorIndice(t_lista_docs *lista, int indice) {
    if (indice < 0 || indice >= lista->num_documentos || lista->inicio == NULL) {
        printf("Erro: Indice %d invalido para remocao.\n", indice);
        return 0;
    }

    t_doc *para_remover = NULL;

    // 1. Caso especial: Remover o primeiro
    if (indice == 0) {
        para_remover = lista->inicio;
        lista->inicio = lista->inicio->proximo;
    }
    else {
        // 2. Caso normal: Encontrar o anterior
        t_doc *anterior = lista->inicio;
        for (int i = 0; i < indice - 1; i++) {
            anterior = anterior->proximo;
        }
        para_remover = anterior->proximo;
        anterior->proximo = para_remover->proximo;
    }

    // 3. Libertar a memória do nó removido
    printf("A remover documento: %s\n", para_remover->titulo);
    priv_libertarDocNode(para_remover);

    lista->num_documentos--;
    return 1;
}

void pesquisarDocumentosPorTokenID(t_lista_docs lista, int token_id) {
    printf("\n--- Pesquisa por Token ID [%d] ---\n", token_id);

    // Verificar se o token_id é válido no vocabulário atual
    if (lista.vocabulario == NULL || token_id < 0 || token_id >= lista.vocabulario->usados) {
        printf("Token ID invalido ou fora dos limites.\n");
        return;
    }

    // Imprimir qual é a palavra correspondente ao ID
    printf("Token: \"%s\"\n", lista.vocabulario->tokens[token_id]);

    int encontrados = 0;
    t_doc *atual = lista.inicio;
    int doc_idx = 0;

    while (atual != NULL) {
        // Verificar se o documento tem este token (Vetor TF > 0)
        // Como o vetor TF está alinhado com os IDs, basta verificar a posição vetor_tf[token_id]
        if (atual->vetor_tf != NULL && atual->vocab_size > token_id) {
            int frequencia = atual->vetor_tf[token_id];

            if (frequencia > 0) {
                printf(" -> Encontrado no Doc [%d] \"%s\" (Freq: %d)\n",
                       doc_idx, atual->titulo, frequencia);
                encontrados++;
            }
        }

        atual = atual->proximo;
        doc_idx++;
    }

    if (encontrados == 0) {
        printf("Nenhum documento contem este token.\n");
    }
}
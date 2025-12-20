#include "gestao_documentos.h"
#include "files.h"
#include <string.h>

// =============================================================
//  Funções Auxiliares (Privadas a este ficheiro)
// =============================================================

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
    int n_ids = 0;
    int *ids_array = tokenizarFrase(texto, *vocab, &n_ids);

    if (ids_array == NULL) {
        free(novo_doc->titulo);
        free(novo_doc->texto_original);
        free(novo_doc);
        return NULL;
    }
    novo_doc->ids_tokens = ids_array;
    novo_doc->num_ids = n_ids;

    // 5. Vetorizacao (TF)
    int *tf_vetor = calcularTfParaFrase(ids_array, n_ids, vocab_size);

    if (tf_vetor == NULL) {
        free(novo_doc->titulo);
        free(novo_doc->texto_original);
        free(novo_doc->ids_tokens);
        free(novo_doc);
        return NULL;
    }
    novo_doc->vetor_tf = tf_vetor;

    return novo_doc;
}

// Funcao auxiliar para limpar a memoria de UM UNICO documento
static void priv_libertarDocNode(TK_TF_NODE *doc) {
    if (doc == NULL) return;

    if (doc->titulo) free(doc->titulo);
    if (doc->texto_original) free(doc->texto_original);
    if (doc->ids_tokens) free(doc->ids_tokens);
    if (doc->vetor_tf) free(doc->vetor_tf);

    free(doc);
}

// =============================================================
//  Funções da Lista Ligada (R2.1 e R2.2)
// =============================================================

LL_TK_TF criarListaDocumentos(MatrizTokens *vocab) {
    LL_TK_TF lista;
    lista.inicio = NULL;
    lista.num_documentos = 0;
    lista.vocabulario = vocab;
    return lista;
}

int adicionarDocumentoLista(LL_TK_TF *lista, const char *titulo, const char *texto_original) {
    return inserirDocumentoNoIndice(lista, titulo, texto_original, lista->num_documentos);
}

void listarDocumentos(LL_TK_TF lista) {
    printf("\n--- Lista de Documentos (%d) ---\n", lista.num_documentos);
    TK_TF_NODE *atual = lista.inicio;

    if (atual == NULL) {
        printf("Lista vazia.\n");
        return;
    }
    int i = 0;
    while (atual != NULL) {
        printf("[%d] Titulo: %s (Tem %d Tokens)\n", i, atual->titulo, atual->num_ids);
        atual = atual->proximo;
        i++;
    }
}

void libertarListaDocumentos(LL_TK_TF *lista) {
    TK_TF_NODE *atual = lista->inicio;
    while (atual != NULL) {
        TK_TF_NODE *prox = atual->proximo;
        priv_libertarDocNode(atual);
        atual = prox;
    }
    lista->inicio = NULL;
    lista->num_documentos = 0;
}

// =============================================================
//  Funcoes de Manipulacao Avancada
// =============================================================

TK_TF_NODE* obterDocumentoPorIndice(LL_TK_TF lista, int indice) {
    if (indice < 0 || indice >= lista.num_documentos) {
        return NULL;
    }
    TK_TF_NODE *atual = lista.inicio;
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

int inserirDocumentoNoIndice(LL_TK_TF *lista, const char *titulo, const char *texto, int indice) {
    if (indice < 0 || indice > lista->num_documentos) {
        printf("Erro: Indice invalido para insercao.\n");
        return 0;
    }

    TK_TF_NODE *novo_doc = priv_criarDocumentoNode(titulo, texto, lista->vocabulario);
    if (novo_doc == NULL) return 0;

    if (indice == 0) {
        novo_doc->proximo = lista->inicio;
        lista->inicio = novo_doc;
    }
    else {
        TK_TF_NODE *anterior = lista->inicio;
        for (int i = 0; i < indice - 1; i++) {
            anterior = anterior->proximo;
        }
        novo_doc->proximo = anterior->proximo;
        anterior->proximo = novo_doc;
    }

    lista->num_documentos++;
    return 1;
}

int removerDocumentoPorIndice(LL_TK_TF *lista, int indice) {
    if (indice < 0 || indice >= lista->num_documentos || lista->inicio == NULL) {
        printf("Erro: Impossivel remover (Indice invalido ou lista vazia).\n");
        return 0;
    }
    TK_TF_NODE *para_remover = NULL;

    if (indice == 0) {
        para_remover = lista->inicio;
        lista->inicio = lista->inicio->proximo;
    }
    else {
        TK_TF_NODE *anterior = lista->inicio;
        for (int i = 0; i < indice - 1; i++) {
            anterior = anterior->proximo;
        }
        para_remover = anterior->proximo;
        anterior->proximo = para_remover->proximo;
    }

    printf("A remover documento: %s\n", para_remover->titulo);
    priv_libertarDocNode(para_remover);
    lista->num_documentos--;
    return 1;
}

void pesquisarDocumentosPorTokenID(LL_TK_TF lista, int token_id) {
    printf("\n--- Pesquisa por Token ID [%d] ---\n", token_id);

    if (lista.vocabulario == NULL || token_id < 0 || token_id >= lista.vocabulario->usados) {
        printf("Token ID invalido.\n");
        return;
    }

    printf("A procurar documentos com a palavra: \"%s\"\n", lista.vocabulario->tokens[token_id]);

    int encontrados = 0;
    TK_TF_NODE *atual = lista.inicio;
    int doc_idx = 0;

    while (atual != NULL) {
        if (atual->vetor_tf != NULL && atual->vocab_size > token_id) {
            int frequencia = atual->vetor_tf[token_id];

            if (frequencia > 0) {
                printf(" -> Encontrado no Doc [%d] \"%s\" (Aparece %d vezes)\n",
                       doc_idx, atual->titulo, frequencia);
                encontrados++;
            }
        }
        atual = atual->proximo;
        doc_idx++;
    }

    if (encontrados == 0) {
        printf("Nenhum documento contem esta palavra.\n");
    }
}
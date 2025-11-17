//
// Created by guilh on 16/11/2025.
//
#include "gestao_documentos.h"

// (Funcao privada para criar a "Caixa" e fazer os calculos)
static t_doc* priv_criarDocumentoNode(const char *titulo, const char *texto, MatrizTokens *vocab) {
    if (vocab == NULL || vocab->usados == 0) {
        return NULL; // Precisa do alfabeto
    }

    t_doc *novo_doc = (t_doc*) malloc(sizeof(t_doc));
    if (novo_doc == NULL) return NULL;

    int vocab_size = vocab->usados;

    novo_doc->titulo = strdup(titulo);
    novo_doc->texto_original = strdup(texto);
    novo_doc->vocab_size = vocab_size;
    novo_doc->proximo = NULL;

    // Chamar R1.4 (Tokenizar)
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

    // Chamar R1.5 (Vetorizar)
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


// =============================================================
//  Funções R2.1 e R2.2 (Públicas)
// =============================================================

t_lista_docs criarListaDocumentos(MatrizTokens *vocab) {
    t_lista_docs lista;
    lista.inicio = NULL;
    lista.num_documentos = 0;
    lista.vocabulario = vocab; // Guarda a referência ao alfabeto
    return lista;
}

int adicionarDocumentoLista(t_lista_docs *lista, const char *titulo, const char *texto_original) {

    // 1. Criar a "Caixa" (já com R1.4 e R1.5 feitos)
    t_doc *novo_doc = priv_criarDocumentoNode(titulo, texto_original, lista->vocabulario);
    if (novo_doc == NULL) {
        return 0; // Falhou
    }

    // 2. Arrumar na "Prateleira" (no fim da corrente)
    if (lista->inicio == NULL) {
        lista->inicio = novo_doc; // É a primeira
    } else {
        // Vai até ao fim da corrente
        t_doc *atual = lista->inicio;
        while (atual->proximo != NULL) {
            atual = atual->proximo;
        }
        atual->proximo = novo_doc; // Liga o último ao novo
    }

    lista->num_documentos++;
    return 1;
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
        atual = atual->proximo; // Passa para a próxima caixa
        i++;
    }
}

void libertarListaDocumentos(t_lista_docs *lista) {
    t_doc *atual = lista->inicio;
    t_doc *proximo_temp;

    while (atual != NULL) {
        proximo_temp = atual->proximo; // Guarda o "fio" para a próxima

        // Liberta tudo dentro da "Caixa"
        free(atual->titulo);
        free(atual->texto_original);
        free(atual->ids_tokens);
        free(atual->vetor_tf);

        // Liberta a "Caixa"
        free(atual);

        atual = proximo_temp; // Passa para a próxima
    }

    lista->inicio = NULL;
    lista->num_documentos = 0;
}
#include "gestao_documentos.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// =============================================================
//  Implementação MATRIX_INT (R2.2)
// =============================================================
MATRIX_INT criarMatrixInt(int capacidade_inicial) {
    MATRIX_INT m;
    m.linhas = malloc(capacidade_inicial * sizeof(int*));
    m.tamanho_linhas = malloc(capacidade_inicial * sizeof(int));

    if (m.linhas == NULL || m.tamanho_linhas == NULL) {
        printf("Erro: Falha ao alocar memoria para MATRIX_INT.\n");
        exit(1);
    }
    m.linhas_usadas = 0;
    m.capacidade = capacidade_inicial;
    return m;
}

int adicionarLinhaInt(MATRIX_INT *m, int *vetor_ids, int num_ids) {
    // 1. Redimensionar se necessário
    if (m->linhas_usadas == m->capacidade) {
        int nova_cap = m->capacidade * 2;
        int **novas_linhas = realloc(m->linhas, nova_cap * sizeof(int*));
        int *novos_tamanhos = realloc(m->tamanho_linhas, nova_cap * sizeof(int));

        if (!novas_linhas || !novos_tamanhos) return 0;

        m->linhas = novas_linhas;
        m->tamanho_linhas = novos_tamanhos;
        m->capacidade = nova_cap;
    }

    // 2. Alocar e copiar a linha (Deep Copy)
    m->linhas[m->linhas_usadas] = malloc(num_ids * sizeof(int));
    if (!m->linhas[m->linhas_usadas]) return 0;

    memcpy(m->linhas[m->linhas_usadas], vetor_ids, num_ids * sizeof(int));
    m->tamanho_linhas[m->linhas_usadas] = num_ids;
    m->linhas_usadas++;
    return 1;
}

void libertarMatrixInt(MATRIX_INT m) {
    for (int i = 0; i < m.linhas_usadas; i++) {
        free(m.linhas[i]);
    }
    free(m.linhas);
    free(m.tamanho_linhas);
}

void listarMatrixInt(MATRIX_INT m) {
    printf("--- MATRIX_INT (%d linhas) ---\n", m.linhas_usadas);
    for (int i = 0; i < m.linhas_usadas; i++) {
        printf("[%d]: ", i);
        for (int j = 0; j < m.tamanho_linhas[i]; j++) {
            printf("%d ", m.linhas[i][j]);
        }
        printf("\n");
    }
}

// =============================================================
//  Implementação Lista Ligada (R2.1)
// =============================================================
LL_TK_TF criarListaLigada() {
    LL_TK_TF lista;
    lista.cabeca = NULL;
    lista.cauda = NULL;
    lista.quantidade_nos = 0;
    return lista;
}
int adicionarNoLista(LL_TK_TF *lista, int *ids, int n_ids, int *tf, int n_vocab) {
    TK_TF_NODE *novo = malloc(sizeof(TK_TF_NODE));
    if (!novo) return 0;

    // Copiar IDs
    novo->ids = malloc(n_ids * sizeof(int));
    memcpy(novo->ids, ids, n_ids * sizeof(int));
    novo->tam_ids = n_ids;

    // Copiar TF
    novo->tf = malloc(n_vocab * sizeof(int));
    memcpy(novo->tf, tf, n_vocab * sizeof(int));
    novo->tam_vocab = n_vocab;

    // Inserir no fim da lista
    novo->proximo = NULL;
    novo->anterior = lista->cauda;

    if (lista->cauda != NULL) {
        lista->cauda->proximo = novo;
    }
    lista->cauda = novo;

    if (lista->cabeca == NULL) {
        lista->cabeca = novo;
    }

    lista->quantidade_nos++;
    return 1;
}

void libertarListaLigada(LL_TK_TF *lista) {
    TK_TF_NODE *atual = lista->cabeca;
    while (atual != NULL) {
        TK_TF_NODE *prox = atual->proximo;
        free(atual->ids); // Libertar vetor IDs
        free(atual->tf);  // Libertar vetor TF
        free(atual);      // Libertar o nó
        atual = prox;
    }
    lista->cabeca = NULL;
    lista->cauda = NULL;
    lista->quantidade_nos = 0;
}

void listarListaLigada(LL_TK_TF lista) {
    TK_TF_NODE *atual = lista.cabeca;
    int i = 0;
    printf("--- Lista Ligada (%d nos) ---\n", lista.quantidade_nos);
    while (atual != NULL) {
        printf("No %d: %d IDs, Vetor TF (tam=%d)\n", i++, atual->tam_ids, atual->tam_vocab);
        atual = atual->proximo;
    }
}

// =============================================================
//  Implementação DOC (R2.2 - Agregador)
// =============================================================
DOC* criarDocumento() {
    DOC *d = malloc(sizeof(DOC));
    if (!d) return NULL;

    d->texto = criarMatrizTexto(10);
    d->ids_tokenizados = criarMatrixInt(10);
    d->lista_processada = criarListaLigada();
    d->tokens = criarMatrizTokens(10); // Vocabulário próprio do doc

    return d;
}

void libertarDocumento(DOC *doc) {
    if (!doc) return;
    libertarMatriz(doc->texto);
    libertarMatrixInt(doc->ids_tokenizados);
    libertarListaLigada(&doc->lista_processada);
    libertarTokens(doc->tokens);
    free(doc);
}

void adicionarFraseAoDocumento(DOC *doc, const char *frase) {
    if (!doc) return;

    // 1. Guardar Texto Original
    adicionarLinha(&doc->texto, frase);

    // Se não houver tokens definidos, não podemos calcular o resto.
    if (doc->tokens.usados == 0) {
        printf("Aviso: Vocabulario vazio. A guardar apenas texto.\n");
        return;
    }

    // 2. Calcular IDs (Tokenização - R1.4)
    int n_ids = 0;
    int *ids = tokenizarFrase(frase, doc->tokens, &n_ids);

    // Guardar na MATRIX_INT
    adicionarLinhaInt(&doc->ids_tokenizados, ids, n_ids);

    // 3. Calcular TF (Vetorização - R1.5)
    int *tf = calcularTfParaFrase(ids, n_ids, doc->tokens.usados);

    // 4. Guardar na Lista Ligada (R2.1)
    adicionarNoLista(&doc->lista_processada, ids, n_ids, tf, doc->tokens.usados);

    // Limpar memória temporária
    free(ids);
    free(tf);
}
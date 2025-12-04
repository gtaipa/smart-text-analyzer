#include "gestao_documentos.h"
#include <string.h>

// =============================================================
//  Funções Auxiliares (Privadas a este ficheiro)
// =============================================================

// Esta funcao serve para criar copias seguras das strings (Titulos e Textos).
// Se eu nao fizesse isto, e o texto original mudasse no 'main', o documento ficava estragado.
char *duplicarString(const char *s) {
    if (s == NULL) return NULL;

    // Vou medir o tamanho e somar +1 para o caracter final '\0'
    size_t tamanho = strlen(s) + 1;

    // Peço memoria ao computador (malloc) do tamanho exato que preciso
    char *nova_copia = (char*)malloc(tamanho);

    // Se nao houver memoria disponivel, retorno NULL
    if (nova_copia == NULL) {
        return NULL;
    }

    // Copio byte a byte da origem para o destino. É mais rapido e seguro.
    memcpy(nova_copia, s, tamanho);

    return nova_copia; // Devolvo o endereço da nova copia
}

// Esta funcao cria um "Nó" (uma caixa da lista ligada) com toda a informacao de um documento.
// Ela ja faz tudo: copia as strings, tokeniza e calcula as frequencias (TF).
static t_doc* priv_criarDocumentoNode(const char *titulo, const char *texto, MatrizTokens *vocab) {
    // Verificacao de seguranca: Preciso de um vocabulario valido para trabalhar
    if (vocab == NULL || vocab->usados == 0) return NULL;

    // 1. Aloco memoria para a estrutura do documento
    t_doc *novo_doc = (t_doc*) malloc(sizeof(t_doc));
    if (novo_doc == NULL) return NULL;

    // 2. Guardo o tamanho do vocabulario atual (para saber o tamanho do vetor TF)
    int vocab_size = vocab->usados;

    // 3. Uso a minha funcao para duplicar o titulo e o texto para dentro do Nó
    novo_doc->titulo = duplicarString(titulo);
    novo_doc->texto_original = duplicarString(texto);
    novo_doc->vocab_size = vocab_size;
    novo_doc->proximo = NULL; // Como acabou de nascer, ainda nao aponta para ninguem

    // 4. Tokenizacao: Transformo o texto em numeros (IDs)
    int n_ids = 0;
    // Chamo a funcao do Requisito R1.4 que esta noutro ficheiro
    int *ids_array = tokenizarFrase(texto, *vocab, &n_ids);

    // Se a tokenizacao falhar, tenho de limpar tudo o que ja aloquei
    if (ids_array == NULL) {
        free(novo_doc->titulo);
        free(novo_doc->texto_original);
        free(novo_doc);
        return NULL;
    }
    novo_doc->ids_tokens = ids_array;
    novo_doc->num_ids = n_ids;

    // 5. Vetorizacao: Conto as frequencias dos tokens (TF)
    // Chamo a funcao do Requisito R1.5
    int *tf_vetor = calcularTfParaFrase(ids_array, n_ids, vocab_size);

    // Se falhar, limpo tudo (titulo, texto, ids e a propria estrutura)
    if (tf_vetor == NULL) {
        free(novo_doc->titulo);
        free(novo_doc->texto_original);
        free(novo_doc->ids_tokens);
        free(novo_doc);
        return NULL;
    }
    novo_doc->vetor_tf = tf_vetor;

    return novo_doc; // Devolvo o documento pronto a ser usado
}

// Funcao auxiliar para limpar a memoria de UM UNICO documento
static void priv_libertarDocNode(t_doc *doc) {
    if (doc == NULL) return;

    // Tenho de libertar cada pedaço de memoria que aloquei com malloc
    if (doc->titulo) free(doc->titulo);
    if (doc->texto_original) free(doc->texto_original);
    if (doc->ids_tokens) free(doc->ids_tokens);
    if (doc->vetor_tf) free(doc->vetor_tf);

    free(doc); // No fim, liberto a "caixa" do documento
}

// =============================================================
//  Funções da Lista Ligada (R2.1 e R2.2)
// =============================================================

t_lista_docs criarListaDocumentos(MatrizTokens *vocab) {
    t_lista_docs lista;
    lista.inicio = NULL; // A lista começa vazia
    lista.num_documentos = 0;
    lista.vocabulario = vocab; // Guardo a referencia para o vocabulario que vou usar
    return lista;
}

// Esta funcao serve apenas de atalho.
// Como inserir no FIM é a operação mais comum, reutilizo a funcao 'inserirDocumentoNoIndice'.
int adicionarDocumentoLista(t_lista_docs *lista, const char *titulo, const char *texto_original) {
    // Se a lista tem 5 documentos, inserir no indice 5 coloca-o no fim (posicao 6).
    return inserirDocumentoNoIndice(lista, titulo, texto_original, lista->num_documentos);
}

void listarDocumentos(t_lista_docs lista) {
    printf("\n--- Lista de Documentos (%d) ---\n", lista.num_documentos);
    t_doc *atual = lista.inicio;
    // Se o inicio for NULL, a lista esta vazia
    if (atual == NULL) {
        printf("Lista vazia.\n");
        return;
    }
    int i = 0;
    // Percorro a lista ate chegar ao fim (NULL)
    while (atual != NULL) {
        printf("[%d] Titulo: %s (Tem %d Tokens)\n", i, atual->titulo, atual->num_ids);
        atual = atual->proximo; // Salto para o proximo documento
        i++;
    }
}

// Esta funcao destroi a lista completa e liberta toda a memoria usada.
void libertarListaDocumentos(t_lista_docs *lista) {
    t_doc *atual = lista->inicio;
    while (atual != NULL) {
        t_doc *prox = atual->proximo; // Guardo o endereço do proximo antes de apagar o atual
        priv_libertarDocNode(atual);  // Apago o atual da memoria
        atual = prox; // Avanço para o seguinte
    }
    // Reset à estrutura da lista
    lista->inicio = NULL;
    lista->num_documentos = 0;
}

// =============================================================
//  Funcoes de Manipulacao Avancada (Inserir, Remover, Pesquisar)
// =============================================================

t_doc* obterDocumentoPorIndice(t_lista_docs lista, int indice) {
    // Verifico se o indice faz sentido (nao pode ser negativo nem maior que o tamanho)
    if (indice < 0 || indice >= lista.num_documentos) {
        return NULL;
    }
    t_doc *atual = lista.inicio;
    int contador = 0;
    // Vou andando na lista ate o contador ser igual ao indice que quero
    while (atual != NULL) {
        if (contador == indice) {
            return atual; // Encontrei!
        }
        atual = atual->proximo;
        contador++;
    }
    return NULL;
}

int inserirDocumentoNoIndice(t_lista_docs *lista, const char *titulo, const char *texto, int indice) {
    // Validacao do indice
    if (indice < 0 || indice > lista->num_documentos) {
        printf("Erro: Indice invalido para insercao.\n");
        return 0;
    }
    // Crio o novo documento na memoria (usando a funcao auxiliar que fiz acima)
    t_doc *novo_doc = priv_criarDocumentoNode(titulo, texto, lista->vocabulario);
    if (novo_doc == NULL) return 0; // Se falhou a criacao (falta de memoria, etc)
    // Caso Especial: Inserir no inicio da lista (cabeça)
    // Se vou ser o primeiro, tenho de apontar para quem era o primeiro antes.
    if (indice == 0) {
        novo_doc->proximo = lista->inicio;
        lista->inicio = novo_doc; // A lista agora começa no novo
    }
    else {
        // Caso Normal: Inserir no meio ou no fim.
        // Tenho de encontrar o documento ANTERIOR à posição onde quero ficar.
        t_doc *anterior = lista->inicio;
        for (int i = 0; i < indice - 1; i++) {
            anterior = anterior->proximo;
        }
        novo_doc->proximo = anterior->proximo; // O meu proximo sera o proximo do anterior
        anterior->proximo = novo_doc; // O proximo do anterior passo a ser eu
    }

    lista->num_documentos++; // Aumento o contador total
    return 1;
}
int removerDocumentoPorIndice(t_lista_docs *lista, int indice) {
    // Validacoes basicas
    if (indice < 0 || indice >= lista.num_documentos || lista->inicio == NULL) {
        printf("Erro: Impossivel remover (Indice invalido ou lista vazia).\n");
        return 0;
    }
    t_doc *para_remover = NULL;
    //Caso Especial: Remover o primeiro da lista
    if (indice == 0) {
        para_remover = lista->inicio;
        lista->inicio = lista->inicio->proximo; // O inicio passa a ser o segundo
    }
    else {
        // Caso Normal: Remover do meio ou fim.
        // Encontrar o anterior para "saltar" por cima do que vamos apagar.
        t_doc *anterior = lista->inicio;
        for (int i = 0; i < indice - 1; i++) {
            anterior = anterior->proximo;
        }
        para_remover = anterior->proximo; // Este é o que vai ser apagado
        // O anterior passa a apontar para o seguinte do removido (saltamos o removido)
        anterior->proximo = para_remover->proximo;
    }
    //Limpar a memoria do documento removido
    printf("A remover documento: %s\n", para_remover->titulo);
    priv_libertarDocNode(para_remover);
    lista->num_documentos--;
    return 1;
}

void pesquisarDocumentosPorTokenID(t_lista_docs lista, int token_id) {
    printf("\n--- Pesquisa por Token ID [%d] ---\n", token_id);

    // Validacao: O token tem de existir no vocabulario
    if (lista.vocabulario == NULL || token_id < 0 || token_id >= lista.vocabulario->usados) {
        printf("Token ID invalido.\n");
        return;
    }

    printf("A procurar documentos com a palavra: \"%s\"\n", lista.vocabulario->tokens[token_id]);

    int encontrados = 0;
    t_doc *atual = lista.inicio;
    int doc_idx = 0;

    // Percorro todos os documentos da lista
    while (atual != NULL) {
        // Para saber se o documento tem a palavra, vou ver ao vetor TF.
        // Se vetor_tf[id] > 0, significa que a palavra aparece lá.
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
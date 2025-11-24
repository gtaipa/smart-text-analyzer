/**
 * @file files.c
 * @brief Implementação das funções de persistência de dados (Texto e Binário).
 * @details Este módulo gere a leitura e escrita da coleção de documentos e vocabulário
 * em dois formatos: texto simples (human-readable) e binário (eficiente).
 */

#include "files.h"
#include <string.h>

// =============================================================
//  Funções Auxiliares (Privadas)
// =============================================================

/**
 * @brief Remove o caractere de nova linha ('\n') do final de uma string.
 * Útil após leitura com fgets.
 */
static void removerQuebraLinha(char *str) {
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';
    }
}

/**
 * @brief Escreve uma string em formato binário: [Tamanho int][Caracteres...]
 * @param f Ponteiro para o ficheiro.
 * @param str A string a escrever.
 * @return 1 sucesso, 0 erro.
 */
static int escreverString(FILE *f, const char *str) {
    int len = (str != NULL) ? strlen(str) : 0;

    // 1. Escreve o tamanho
    if (fwrite(&len, sizeof(int), 1, f) != 1) return 0;

    // 2. Escreve os dados (se existirem)
    if (len > 0) {
        if (fwrite(str, sizeof(char), len, f) != (size_t)len) return 0;
    }
    return 1;
}

/**
 * @brief Lê uma string em formato binário e aloca memória dinâmica.
 * @param f Ponteiro para o ficheiro.
 * @return char* String alocada (o utilizador deve fazer free), ou NULL em erro.
 */
static char* lerString(FILE *f) {
    int len = 0;
    // 1. Lê o tamanho
    if (fread(&len, sizeof(int), 1, f) != 1) return NULL;

    // Aloca memória (+1 para o terminador nulo)
    char *str = malloc(len + 1);
    if (str == NULL) return NULL;

    // 2. Lê os caracteres
    if (len > 0) {
        if (fread(str, sizeof(char), len, f) != (size_t)len) {
            free(str);
            return NULL;
        }
    }
    str[len] = '\0'; // Fecha a string
    return str;
}

// =============================================================
//  Funções R2.3 - Ficheiros de Texto
// =============================================================

int guardarColecaoTexto(const char *nome_ficheiro, t_lista_docs lista) {
    FILE *f = fopen(nome_ficheiro, "w");
    if (f == NULL) {
        printf("Erro: Nao foi possivel criar o ficheiro '%s'.\n", nome_ficheiro);
        return 0;
    }

    // 1. Guardar Vocabulário
    if (lista.vocabulario != NULL) {
        fprintf(f, "%d\n", lista.vocabulario->usados);
        for (int i = 0; i < lista.vocabulario->usados; i++) {
            fprintf(f, "%s\n", lista.vocabulario->tokens[i]);
        }
    } else {
        fprintf(f, "0\n");
    }

    // 2. Guardar Documentos
    fprintf(f, "%d\n", lista.num_documentos);

    t_doc *atual = lista.inicio;
    while (atual != NULL) {
        // Guardar Titulo e Texto
        fprintf(f, "%s\n", atual->titulo);
        fprintf(f, "%s\n", atual->texto_original);

        // Guardar IDs
        fprintf(f, "IDS: ");
        for(int i=0; i<atual->num_ids; i++){
            fprintf(f, "%d ", atual->ids_tokens[i]);
        }
        fprintf(f, "\n");

        // Guardar TF
        fprintf(f, "TF: ");
        for(int i=0; i<atual->vocab_size; i++){
            if(atual->vetor_tf[i] > 0){
                fprintf(f, "%d:%d ", i, atual->vetor_tf[i]);
            }
        }
        fprintf(f, "\n");

        atual = atual->proximo;
    }

    fclose(f);
    printf("Colecao guardada com sucesso (texto) em '%s'.\n", nome_ficheiro);
    return 1;
}

t_lista_docs carregarColecaoTexto(const char *nome_ficheiro) {
    t_lista_docs lista;
    lista.inicio = NULL;
    lista.num_documentos = 0;

    // Alocar vocabulário temporário
    MatrizTokens *vocab = malloc(sizeof(MatrizTokens));
    *vocab = criarMatrizTokens(10);
    lista.vocabulario = vocab;

    FILE *f = fopen(nome_ficheiro, "r");
    if (f == NULL) {
        printf("Erro: Nao foi possivel abrir '%s'. Retornando lista vazia.\n", nome_ficheiro);
        return lista;
    }

    char buffer[1024];

    // 1. Ler Vocabulario
    int num_tokens = 0;
    if (fgets(buffer, sizeof(buffer), f) != NULL) num_tokens = atoi(buffer);

    for (int i = 0; i < num_tokens; i++) {
        if (fgets(buffer, sizeof(buffer), f) != NULL) {
            removerQuebraLinha(buffer);
            adicionarToken(lista.vocabulario, buffer);
        }
    }

    // 2. Ler Documentos
    int num_docs = 0;
    if (fgets(buffer, sizeof(buffer), f) != NULL) num_docs = atoi(buffer);

    for (int i = 0; i < num_docs; i++) {
        char titulo[200];
        char texto[1024];

        if (fgets(titulo, sizeof(titulo), f) != NULL) removerQuebraLinha(titulo);
        if (fgets(texto, sizeof(texto), f) != NULL) removerQuebraLinha(texto);

        // Ignorar linhas de IDS e TF (são recalculadas ao adicionar)
        fgets(buffer, sizeof(buffer), f);
        fgets(buffer, sizeof(buffer), f);

        adicionarDocumentoLista(&lista, titulo, texto);
    }

    fclose(f);
    printf("Colecao carregada (texto): %d tokens e %d documentos.\n",
           lista.vocabulario->usados, lista.num_documentos);
    return lista;
}

// =============================================================
//  Funções R2.4 - Ficheiros Binários
// =============================================================

int guardarColecaoBinaria(const char *nome_ficheiro, t_lista_docs lista) {
    FILE *f = fopen(nome_ficheiro, "wb");
    int sucesso = 1;

    if (f == NULL) {
        printf("Erro R2.4: Nao foi possivel criar o ficheiro binario '%s'.\n", nome_ficheiro);
        return 0;
    }

    // 1. Guardar Vocabulário
    int num_tokens = lista.vocabulario ? lista.vocabulario->usados : 0;
    if (fwrite(&num_tokens, sizeof(int), 1, f) != 1) { sucesso = 0; goto fim; }

    if (lista.vocabulario) {
        for (int i = 0; i < num_tokens; i++) {
            if (!escreverString(f, lista.vocabulario->tokens[i])) { sucesso = 0; goto fim; }
        }
    }

    // 2. Guardar Documentos
    int num_docs = lista.num_documentos;
    if (fwrite(&num_docs, sizeof(int), 1, f) != 1) { sucesso = 0; goto fim; }

    t_doc *atual = lista.inicio;
    while (atual != NULL) {
        // 2.1. Strings
        if (!escreverString(f, atual->titulo)) { sucesso = 0; goto fim; }
        if (!escreverString(f, atual->texto_original)) { sucesso = 0; goto fim; }

        // 2.2. IDs dos Tokens
        int num_ids = atual->num_ids;
        if (fwrite(&num_ids, sizeof(int), 1, f) != 1) { sucesso = 0; goto fim; }
        if (atual->ids_tokens && num_ids > 0) {
            if (fwrite(atual->ids_tokens, sizeof(int), (size_t)num_ids, f) != (size_t)num_ids) {
                sucesso = 0; goto fim;
            }
        }

        // 2.3. Vetor TF
        int vocab_size = atual->vocab_size;
        if (fwrite(&vocab_size, sizeof(int), 1, f) != 1) { sucesso = 0; goto fim; }
        if (atual->vetor_tf && vocab_size > 0) {
            if (fwrite(atual->vetor_tf, sizeof(int), (size_t)vocab_size, f) != (size_t)vocab_size) {
                sucesso = 0; goto fim;
            }
        }

        atual = atual->proximo;
    }

fim:
    fclose(f);
    if (sucesso) printf("Sucesso R2.4: Colecao guardada em binario '%s'.\n", nome_ficheiro);
    else printf("Erro R2.4: Falha na escrita binaria.\n");
    return sucesso;
}

t_lista_docs carregarColecaoBinaria(const char *nome_ficheiro) {
    t_lista_docs lista;
    lista.inicio = NULL;
    lista.num_documentos = 0;
    lista.vocabulario = malloc(sizeof(MatrizTokens));
    *lista.vocabulario = criarMatrizTokens(10);

    FILE *f = fopen(nome_ficheiro, "rb");
    if (f == NULL) {
        printf("Erro R2.4: Nao foi possivel abrir '%s'.\n", nome_ficheiro);
        return lista;
    }

    // 1. Carregar Vocabulário
    int num_tokens = 0;
    if (fread(&num_tokens, sizeof(int), 1, f) != 1) goto erro;

    for (int i = 0; i < num_tokens; i++) {
        char *token = lerString(f);
        if (!token) goto erro;
        adicionarToken(lista.vocabulario, token);
        free(token);
    }

    // 2. Carregar Documentos
    int num_docs = 0;
    if (fread(&num_docs, sizeof(int), 1, f) != 1) goto erro;

    for (int i = 0; i < num_docs; i++) {
        char *titulo = lerString(f);
        char *texto = lerString(f);

        if (!titulo || !texto) { free(titulo); free(texto); goto erro; }

        // Ler e descartar IDs/TF do ficheiro (recalculamos para garantir integridade)
        // Precisamos de ler para avançar o cursor do ficheiro corretamente
        int num_ids, vocab_size_doc;

        // Pular IDs
        fread(&num_ids, sizeof(int), 1, f);
        if (num_ids > 0) fseek(f, num_ids * sizeof(int), SEEK_CUR);

        // Pular TF
        fread(&vocab_size_doc, sizeof(int), 1, f);
        if (vocab_size_doc > 0) fseek(f, vocab_size_doc * sizeof(int), SEEK_CUR);

        // Adiciona à lista (isto recalcula IDs e TF frescos usando o vocab carregado)
        adicionarDocumentoLista(&lista, titulo, texto);

        free(titulo);
        free(texto);
    }

    fclose(f);
    printf("Sucesso R2.4: Colecao binaria carregada (%d docs).\n", lista.num_documentos);
    return lista;

erro:
    printf("Erro R2.4: Falha na leitura do ficheiro binario.\n");
    if (f) fclose(f);
    return lista;
}
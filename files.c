#include "files.h"
#include <string.h>

// =============================================================
//  Funções Auxiliares
// =============================================================
static void removerQuebraLinha(char *str) {
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';
    }
}

static int escreverStringBinario(FILE *f, const char *str) {
    int tamanho = 0;
    if (str != NULL) {
        tamanho = strlen(str);
    }
    int coisas_escritas = fwrite(&tamanho, sizeof(int), 1, f);
    if (coisas_escritas != 1) {
        return 0;
    }
    if (tamanho > 0) {
        coisas_escritas = fwrite(str, sizeof(char), tamanho, f);
        if (coisas_escritas != tamanho) {
            return 0;
        }
    }
    return 1;
}

static char* lerStringBinario(FILE *f) {
    int tamanho = 0;
    int coisas_lidas = fread(&tamanho, sizeof(int), 1, f);
    if (coisas_lidas != 1) {
        return NULL;
    }
    char *palavra = malloc(tamanho + 1);
    if (palavra == NULL) {
        return NULL;
    }
    if (tamanho > 0) {
        int letras_lidas = fread(palavra, sizeof(char), tamanho, f);
        if (letras_lidas != tamanho) {
            free(palavra);
            return NULL;
        }
    }
    palavra[tamanho] = '\0';
    return palavra;
}

// =============================================================
//  Funções R2.3 - Ficheiros de Texto
// =============================================================

int guardarColecaoTexto(const char *nome_ficheiro, LL_TK_TF lista) {

    FILE *ficheiro = fopen(nome_ficheiro, "w");
    if (ficheiro == NULL) {
        printf("Erro: Nao foi possivel criar o ficheiro %s.\n", nome_ficheiro);
        return 0;
    }
    // Guardar o Vocabulario
    int total_tokens = 0;
    if (lista.vocabulario != NULL) {
        total_tokens = lista.vocabulario->usados;
    }
    fprintf(ficheiro, "%d\n", total_tokens);
    for (int i = 0; i < total_tokens; i++) {
        fprintf(ficheiro, "%s\n", lista.vocabulario->tokens[i]);
    }
    // Guardar os Documentos
    fprintf(ficheiro, "%d\n", lista.num_documentos);

    TK_TF_NODE *atual = lista.inicio;
    while (atual != NULL) {
        fprintf(ficheiro, "%s\n", atual->titulo);
        fprintf(ficheiro, "%s\n", atual->texto_original);
        // IDs
        fprintf(ficheiro, "IDS: ");
        for(int i = 0; i < atual->num_ids; i++){
            fprintf(ficheiro, "%d ", atual->ids_tokens[i]);
        }
        fprintf(ficheiro, "\n");
        // TF
        fprintf(ficheiro, "TF: ");
        for(int i = 0; i < atual->vocab_size; i++){
            if(atual->vetor_tf[i] > 0){
                fprintf(ficheiro, "%d:%d ", i, atual->vetor_tf[i]);
            }
        }
        fprintf(ficheiro, "\n");
        atual = atual->proximo;
    }
    fclose(ficheiro);
    return 1;
}

LL_TK_TF carregarColecaoTexto(const char *nome_ficheiro) {
    LL_TK_TF lista;
    lista.inicio = NULL;
    lista.num_documentos = 0;

    MatrizTokens *vocab = malloc(sizeof(MatrizTokens));
    *vocab = criarMatrizTokens(10);
    lista.vocabulario = vocab;

    FILE *ficheiro = fopen(nome_ficheiro, "r");
    if (ficheiro == NULL) {
        return lista;
    }
    char buffer[2048];
    // Ler Vocabulario
    int num_tokens = 0;
    if (fgets(buffer, sizeof(buffer), ficheiro) != NULL) {
        num_tokens = atoi(buffer);
    }
    for (int i = 0; i < num_tokens; i++) {
        if (fgets(buffer, sizeof(buffer), ficheiro) != NULL) {
            removerQuebraLinha(buffer);
            adicionarToken(lista.vocabulario, buffer);
        }
    }
    // Ler Documentos
    int num_docs = 0;
    if (fgets(buffer, sizeof(buffer), ficheiro) != NULL) {
        num_docs = atoi(buffer);
    }
    for (int i = 0; i < num_docs; i++) {
        char titulo[200];
        char texto[2048];
        if (fgets(titulo, sizeof(titulo), ficheiro) != NULL) {
            removerQuebraLinha(titulo);
        }
        if (fgets(texto, sizeof(texto), ficheiro) != NULL) {
            removerQuebraLinha(texto);
        }
        // Consumir linhas IDS e TF
        fgets(buffer, sizeof(buffer), ficheiro);
        fgets(buffer, sizeof(buffer), ficheiro);

        adicionarDocumentoLista(&lista, titulo, texto);
    }
    fclose(ficheiro);
    return lista;
}

//  Funções R2.4 - Ficheiros Binários

int guardarColecaoBinaria(const char *nome_ficheiro, LL_TK_TF lista) {
    FILE *ficheiro = fopen(nome_ficheiro, "wb");
    if (ficheiro == NULL) {
        return 0;
    }
    // Guardar Vocabulario
    int num_tokens = 0;
    if (lista.vocabulario != NULL) {
        num_tokens = lista.vocabulario->usados;
    }
    fwrite(&num_tokens, sizeof(int), 1, ficheiro);
    if (num_tokens > 0) {
        for (int i = 0; i < num_tokens; i++) {
            if (!escreverStringBinario(ficheiro, lista.vocabulario->tokens[i])) {
                fclose(ficheiro);
                return 0;
            }
        }
    }
    // Guardar Documentos
    fwrite(&lista.num_documentos, sizeof(int), 1, ficheiro);
    TK_TF_NODE *atual = lista.inicio;
    while (atual != NULL) {
        escreverStringBinario(ficheiro, atual->titulo);
        escreverStringBinario(ficheiro, atual->texto_original);

        int n_ids = atual->num_ids;
        fwrite(&n_ids, sizeof(int), 1, ficheiro);
        if (n_ids > 0) {
            fwrite(atual->ids_tokens, sizeof(int), n_ids, ficheiro);
        }

        int n_tf = atual->vocab_size;
        fwrite(&n_tf, sizeof(int), 1, ficheiro);
        if (n_tf > 0) {
            fwrite(atual->vetor_tf, sizeof(int), n_tf, ficheiro);
        }
        atual = atual->proximo;
    }
    fclose(ficheiro);
    return 1;
}

LL_TK_TF carregarColecaoBinaria(const char *nome_ficheiro) {
    LL_TK_TF lista;
    lista.inicio = NULL;
    lista.num_documentos = 0;

    lista.vocabulario = malloc(sizeof(MatrizTokens));
    *lista.vocabulario = criarMatrizTokens(10);

    FILE *ficheiro = fopen(nome_ficheiro, "rb");
    if (ficheiro == NULL) {
        return lista;
    }
    // Ler Vocabulario
    int num_tokens = 0;
    fread(&num_tokens, sizeof(int), 1, ficheiro);
    for (int i = 0; i < num_tokens; i++) {
        char *token = lerStringBinario(ficheiro);
        if (token != NULL) {
            adicionarToken(lista.vocabulario, token);
            free(token);
        }
    }
    // Ler Documentos
    int num_docs = 0;
    fread(&num_docs, sizeof(int), 1, ficheiro);
    for (int i = 0; i < num_docs; i++) {
        char *titulo = lerStringBinario(ficheiro);
        char *texto = lerStringBinario(ficheiro);
        if (titulo && texto) {
            adicionarDocumentoLista(&lista, titulo, texto);
        }
        if(titulo) free(titulo);
        if(texto) free(texto);

        // SALTO MORTAL (SKIP)
        int n_ids, n_tf;
        fread(&n_ids, sizeof(int), 1, ficheiro);
        if(n_ids > 0) {
            fseek(ficheiro, n_ids * sizeof(int), SEEK_CUR);
        }
        fread(&n_tf, sizeof(int), 1, ficheiro);
        if(n_tf > 0) {
            fseek(ficheiro, n_tf * sizeof(int), SEEK_CUR);
        }
    }
    fclose(ficheiro);
    return lista;
}
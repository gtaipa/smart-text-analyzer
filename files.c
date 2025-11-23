#include "files.h"

// Remove o \n do final da string lida pelo fgets
static void removerQuebraLinha(char *str) {
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';
    }
}

int guardarColecaoTexto(const char *nome_ficheiro, t_lista_docs lista) {
    FILE *f = fopen(nome_ficheiro, "w");
    if (f == NULL) {
        printf("Erro: Nao foi possivel criar o ficheiro '%s'.\n", nome_ficheiro);
        return 0;
    }

    if (lista.vocabulario != NULL) {
        fprintf(f, "%d\n", lista.vocabulario->usados);
        for (int i = 0; i < lista.vocabulario->usados; i++) {
            fprintf(f, "%s\n", lista.vocabulario->tokens[i]);
        }
    } else {
        fprintf(f, "0\n");
    }

    fprintf(f, "%d\n", lista.num_documentos);
    
    t_doc *atual = lista.inicio;
    while (atual != NULL) {
        // Guardar Titulo e Texto
        fprintf(f, "%s\n", atual->titulo);
        fprintf(f, "%s\n", atual->texto_original);

        // Guardar IDs (para cumprir requisito de output de vetorizacao)
        fprintf(f, "IDS: ");
        for(int i=0; i<atual->num_ids; i++){
            fprintf(f, "%d ", atual->ids_tokens[i]);
        }
        fprintf(f, "\n");

        // Guardar TF (para visualizacao no ficheiro)
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
    printf("Colecao guardada com sucesso em '%s'.\n", nome_ficheiro);
    return 1;
}

t_lista_docs carregarColecaoTexto(const char *nome_ficheiro) {
    // Inicializar lista vazia
    t_lista_docs lista;
    lista.inicio = NULL;
    lista.num_documentos = 0;
    
    // Precisamos de alocar o vocabulario primeiro
    // Criamos com capacidade 1 temporaria, logo cresce
    MatrizTokens *vocab = malloc(sizeof(MatrizTokens)); 
    *vocab = criarMatrizTokens(10); 
    lista.vocabulario = vocab;

    FILE *f = fopen(nome_ficheiro, "r");
    if (f == NULL) {
        printf("Erro: Nao foi possivel abrir '%s'. Retornando lista vazia.\n", nome_ficheiro);
        return lista;
    }

    char buffer[1024]; // Buffer para leitura de linhas

    // 1. Ler Vocabulario
    int num_tokens = 0;
    if (fgets(buffer, sizeof(buffer), f) != NULL) {
        num_tokens = atoi(buffer);
    }

    for (int i = 0; i < num_tokens; i++) {
        if (fgets(buffer, sizeof(buffer), f) != NULL) {
            removerQuebraLinha(buffer);
            adicionarToken(lista.vocabulario, buffer);
        }
    }

    // 2. Ler Documentos
    int num_docs = 0;
    if (fgets(buffer, sizeof(buffer), f) != NULL) {
        num_docs = atoi(buffer);
    }

    for (int i = 0; i < num_docs; i++) {
        char titulo[200];
        char texto[1024];

        // Ler Titulo
        if (fgets(titulo, sizeof(titulo), f) != NULL) removerQuebraLinha(titulo);
        
        // Ler Texto
        if (fgets(texto, sizeof(texto), f) != NULL) removerQuebraLinha(texto);

        // Ignorar as linhas de "IDS:" e "TF:" pois vamos recalcular
        // para garantir integridade dos ponteiros
        fgets(buffer, sizeof(buffer), f); // Linha IDS
        fgets(buffer, sizeof(buffer), f); // Linha TF

        // Adicionar à lista (isto recalcula automaticamente IDs e TF)
        adicionarDocumentoLista(&lista, titulo, texto);
    }

    fclose(f);
    printf("Colecao carregada: %d tokens e %d documentos.\n", 
           lista.vocabulario->usados, lista.num_documentos);
    return lista;
}
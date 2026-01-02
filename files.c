#include "files.h"
#include <string.h>


//  Funções Auxiliares

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


//  Funções R2.3 - Ficheiros de Texto


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
// Esta função carrega um ficheiro de texto que contém:
//  - um vocabulário de tokens
//  - uma coleção de documentos
// e devolve tudo organizado numa estrutura LL_TK_TF


LL_TK_TF carregarColecaoTexto(const char *nome_ficheiro) {
    LL_TK_TF lista;// main struct
    lista.inicio = NULL;
    lista.num_documentos = 0;// nenhum foi lido ainda(struct vazia)

    MatrizTokens *vocab = malloc(sizeof(MatrizTokens));// vocabulário dinamico porque o nº de tokens só é conhecidonotempo de execução
    *vocab = criarMatrizTokens(10);// inicializa a função
    lista.vocabulario = vocab;// armazena na main struct

    FILE *ficheiro = fopen(nome_ficheiro, "r");// abre em modo leitor
    if (ficheiro == NULL) {
        return lista;
    }
    char buffer[2048];// usado para ler linhas de texto
    // Ler Vocabulario
    int num_tokens = 0;// o primeiro valor do ficheiro indica quantos tokens existem
    if (fgets(buffer, sizeof(buffer), ficheiro) != NULL) {
        num_tokens = atoi(buffer);// le a primeira linha e converte para inteiro
    }
    for (int i = 0; i < num_tokens; i++) {// le cada  token do vocabulário um por linha
        if (fgets(buffer, sizeof(buffer), ficheiro) != NULL) {// le a linha que corresponde ao token
            removerQuebraLinha(buffer);// remove"\n" no final da string para o token ficar limpo
            adicionarToken(lista.vocabulario, buffer);// adiciona o token ao vacabulário
        }
    }
    // Ler numero total de documentos
    int num_docs = 0;
    if (fgets(buffer, sizeof(buffer), ficheiro) != NULL) {// le a linha que indica quantos documentos existem e converte a string lida para inteiro
        num_docs = atoi(buffer);
    }
    for (int i = 0; i < num_docs; i++) {// oercorre a totalidade de documentos indicados no ficheiro
        char titulo[200];// variaveis temporarios
        char texto[2048];
        if (fgets(titulo, sizeof(titulo), ficheiro) != NULL) {// le o titulo do documento
            removerQuebraLinha(titulo);// remove o "\n" do fim da linha
        }
        if (fgets(texto, sizeof(texto), ficheiro) != NULL) {// le o principal texto do documento
            removerQuebraLinha(texto);// remove o "\n" do fim da linha
        }
        // Consumir linhas IDS e TF
        fgets(buffer, sizeof(buffer), ficheiro);
        fgets(buffer, sizeof(buffer), ficheiro);

        adicionarDocumentoLista(&lista, titulo, texto);// adiciona o documento(titulo+texto) á lista ligada de documentos
    }
    fclose(ficheiro);// fecha o ficheiro depois de toda a leitura estar concluida
    return lista;// devolve a struct completa em vocabulário e documentos
}

//  Funções R2.4 - Ficheiros Binários

int guardarColecaoBinaria(const char *nome_ficheiro, LL_TK_TF lista) {
    FILE *ficheiro = fopen(nome_ficheiro, "wb");//abre o ficheiro em modo escrita binária
    if (ficheiro == NULL) {// se não for possivel abrir o ficheiro, aborta
        return 0;
    }
    // Guardar Vocabulario
    int num_tokens = 0;//determina quantos tokens existem ao vocabulario
    if (lista.vocabulario != NULL) {
        num_tokens = lista.vocabulario->usados;
    }
    fwrite(&num_tokens, sizeof(int), 1, ficheiro);//guarda primeiro o numero de tokens para que ao ler o ficheiro saibamos quantos vem a seguir
    if (num_tokens > 0) {// se existirem tokens guarda-os um a um
        for (int i = 0; i < num_tokens; i++) {//cada token é uma string de tamanho variavel, por isso usa-se uma função auxiliar para escrever o tamanho e o conteudo corretamente
            if (!escreverStringBinario(ficheiro, lista.vocabulario->tokens[i])) {// em caso de erro, fecha o ficheiro e sinaliza falha
                fclose(ficheiro);
                return 0;
            }
        }
    }
    // Guardar Documentos
    fwrite(&lista.num_documentos, sizeof(int), 1, ficheiro);//guarda o numero total de documentos
    TK_TF_NODE *atual = lista.inicio;// percorre a lista ligada de documentos
    while (atual != NULL) {
        escreverStringBinario(ficheiro, atual->titulo);// guardar o tituloinal do documento
        escreverStringBinario(ficheiro, atual->texto_original);// guardar o texto original do documento

        int n_ids = atual->num_ids;// numero de IDs associados ao documento
        fwrite(&n_ids, sizeof(int), 1, ficheiro);//guarda quantos IDs existem
        if (n_ids > 0) {
            fwrite(atual->ids_tokens, sizeof(int), n_ids, ficheiro);// se existirem IDs, guarda o vetor completo
        }
//Guarda o vetor de TF
        int n_tf = atual->vocab_size;//Tamanha do vetor de frequencias (TF)
        fwrite(&n_tf, sizeof(int), 1, ficheiro);//guarda o tamanho do vetor TF
        if (n_tf > 0) {
            fwrite(atual->vetor_tf, sizeof(int), n_tf, ficheiro);//Se existir Vetor TF, guarda todos os valores
        }
        atual = atual->proximo;// avança para o proximo documento da lista
    }
    fclose(ficheiro);
    return 1;
}
// Carrega uma coleção previamente guardada num ficheiro binário.
// Reconstrói o vocabulário e a lista de documentos.
// Devolve uma estrutura LL_TK_TF pronta a ser usada.
LL_TK_TF carregarColecaoBinaria(const char *nome_ficheiro) {
    LL_TK_TF lista;// Struct principal
    lista.inicio = NULL;//inicialmente não existem documentos na lista
    lista.num_documentos = 0;

    lista.vocabulario = malloc(sizeof(MatrizTokens));//aloca memoria para o vocabulario porque o seu tamanho só é conhecido depois de ler o ficheiro
    *lista.vocabulario = criarMatrizTokens(10);//Inicializa o vocabulario com uma capacidade inicial(pode crescer conforme os tokens são lidos)

    FILE *ficheiro = fopen(nome_ficheiro, "rb");// abre em modo leitura binária
    if (ficheiro == NULL) {// se o ficheiro não poder ser aberto devolve a struct vazia para evitar erros
        return lista;
    }
    // Ler Vocabulario
    int num_tokens = 0;
    fread(&num_tokens, sizeof(int), 1, ficheiro);// le o numero de tokens guardados
    for (int i = 0; i < num_tokens; i++) {// le cada token do vocabulario
        char *token = lerStringBinario(ficheiro);// cada token foi guardada como string de tamanho variavel por isso é lido atraves de uma função auxiliar
        if (token != NULL) {//adiciona o token ao vocabulario reconstruido
            adicionarToken(lista.vocabulario, token);//adiciona o token ao vocabulario reconstruido
            free(token);// liberta a tring temporaria pois o vocabulario já fez a sua propria copia
        }
    }
    // Ler Documentos
    int num_docs = 0;// le o numero total de documentos gurdados no ficheiro
    fread(&num_docs, sizeof(int), 1, ficheiro);
    for (int i = 0; i < num_docs; i++) { //percorre todos os documentos
        char *titulo = lerStringBinario(ficheiro);//le o titulo do documento (STRING DINAMICA)
        char *texto = lerStringBinario(ficheiro);//le o texto original
        if (titulo && texto) {// se ambos foram lidos corretamente, adiciona o documento a lista ligada
            adicionarDocumentoLista(&lista, titulo, texto);
        }
        if(titulo) free(titulo);//liberta a memoria temporaria das strings lidas pois a lista ja fez a sua propria copia
        if(texto) free(texto);

        // SALTO MORTAL (SKIP)
        int n_ids, n_tf;
        fread(&n_ids, sizeof(int), 1, ficheiro);//le quantidade de IDs
        if(n_ids > 0) {
            fseek(ficheiro, n_ids * sizeof(int), SEEK_CUR);// salta diretamente os IDs no ficheiro
        }
        fread(&n_tf, sizeof(int), 1, ficheiro);// le o tamanho do vetor TF
        if(n_tf > 0) {//salta o vetor TF
            fseek(ficheiro, n_tf * sizeof(int), SEEK_CUR);
        }
    }
    fclose(ficheiro);// fecha o ficheiro depois de terminar a leitura
    return lista;// devolve a coleção reconstruida
}
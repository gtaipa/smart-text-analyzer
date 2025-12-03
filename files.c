#include "files.h"
#include <string.h>

// =============================================================
//  Funções Auxiliares
// =============================================================
static void removerQuebraLinha(char *str) {
    //medir o tamanho da frase
    size_t len = strlen(str);

    //trocar um ocasional \n por \0 para terminar as strings
    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';
    }
}

// Esta funcao ajuda a escrever palavras em ficheiros binarios(gravar o tamanho da palavra para o pc saber onde acaba)
static int escreverStringBinario(FILE *f, const char *str) {
    // Descobrir o tamanho da palavra.
    // Começo com tamanho 0, uso o strlen para medir.
    int tamanho = 0;
    if (str != NULL) {
        tamanho = strlen(str);
    }
    // Gravar (o tamanho) no ficheiro.
    // Peço ao fwrite para escrever 1 inteiro. Ele devolve quantos conseguiu escrever.
    int coisas_escritas = fwrite(&tamanho, sizeof(int), 1, f);
    if (coisas_escritas != 1) {
        return 0; // Devolve 0 para avisar que falhou
    }
    // Gravar as letras da palavra
    if (tamanho > 0) {
        // Agora peço para escrever tamanho caracteres.
        coisas_escritas = fwrite(str, sizeof(char), tamanho, f);
        //verificaçao de erros
        if (coisas_escritas != tamanho) {
            return 0;
        }
    }

    return 1;
}

// Esta funcao le uma palavra de um ficheiro binario.
static char* lerStringBinario(FILE *f) {
    int tamanho = 0;
    // O fread devolve a quantidade de coisas que conseguiu ler com sucesso.
    int coisas_lidas = fread(&tamanho, sizeof(int), 1, f);
    // Se nao leu exatamente 1 numero, entao o ficheiro acabou ou houve erro.
    if (coisas_lidas != 1) {
        return NULL;
    }
    // Reservar memoria para a palavra.
    char *palavra = malloc(tamanho + 1);
    // Verificacao de seguranca
    if (palavra == NULL) {
        return NULL;
    }
    if (tamanho > 0) {
        // Tentar ler as letras todas de uma vez para dentro da variavel 'palavra'
        int letras_lidas = fread(palavra, sizeof(char), tamanho, f);
        // Verificaçao
        if (letras_lidas != tamanho) {
            free(palavra); //Se falhou, limpo a memoria que aloquei para nao deixar lixo.
            return NULL;
        }
    }

    //colocar o terminador de string
    palavra[tamanho] = '\0';
    return palavra;
}
// =============================================================
//  Funções R2.3 - Ficheiros de Texto
// =============================================================

int guardarColecaoTexto(const char *nome_ficheiro, t_lista_docs lista) {

    //Criar um ficheiro
    // "w" significa Write Se o ficheiro ja existir, ele apaga tudo e cria de novo
    FILE *ficheiro = fopen(nome_ficheiro, "w");
    // Verificacao obrigatoria
    if (ficheiro == NULL) {
        printf("Erro: Nao foi possivel criar o ficheiro %s.\n", nome_ficheiro);
        return 0;
    }
    // Guardar o Vocabulario
    // preciso saber quantos tokens existem
    int total_tokens = 0;
    if (lista.vocabulario != NULL) {
        total_tokens = lista.vocabulario->usados;
    }
    fprintf(ficheiro, "%d\n", total_tokens);
    // Agora faço um ciclo para escrever as palavras, uma por linha.
    for (int i = 0; i < total_tokens; i++) {
        fprintf(ficheiro, "%s\n", lista.vocabulario->tokens[i]);
    }
    //Guardar os Documentos
    // Primeiro,escrevo quantos documentos vou guardar
    fprintf(ficheiro, "%d\n", lista.num_documentos);
    // Vou usar um ponteiro auxiliar 'atual' para percorrer a Lista Ligada.
    t_doc *atual = lista.inicio;
    // Enquanto houver documentos
    while (atual != NULL) {
        // Escrevo o Titulo e o Texto Original
        fprintf(ficheiro, "%s\n", atual->titulo);
        fprintf(ficheiro, "%s\n", atual->texto_original);
        //Escrevo os IDs dos tokens
        fprintf(ficheiro, "IDS: ");
        for(int i = 0; i < atual->num_ids; i++){
            fprintf(ficheiro, "%d ", atual->ids_tokens[i]);
        }
        fprintf(ficheiro, "\n");
        // Escrever a Frequencia (tf)
        fprintf(ficheiro, "TF: ");
        for(int i = 0; i < atual->vocab_size; i++){
            if(atual->vetor_tf[i] > 0){
                //"ID:Quantidade"(ex:"0:2" -> o token 0 aparece 2 vezes)
                fprintf(ficheiro, "%d:%d ", i, atual->vetor_tf[i]);
            }
        }
        fprintf(ficheiro, "\n");
        // avançar o documento para o proximo
        atual = atual->proximo;
    }
    // Fechar o ficheiro para gravar tudo o que ficou em memoria.
    fclose(ficheiro);

    return 1;
}
t_lista_docs carregarColecaoTexto(const char *nome_ficheiro) {
    //Preparar a Lista Vazia
    t_lista_docs lista;
    lista.inicio = NULL;
    lista.num_documentos = 0;
    //RETIRAR ANTES DE APRESENTAR
    // Reservo memoria DINAMICA para o vocabulario.
    // Se nao usasse 'malloc', a variavel 'vocab' seria destruida quando esta funcao
    // terminasse, e a lista ficaria a apontar para memoria invalida (lixo).
    // Com malloc, o vocabulario sobrevive na memoria até usarmos 'free'.
    MatrizTokens *vocab = malloc(sizeof(MatrizTokens));
    *vocab = criarMatrizTokens(10);
    lista.vocabulario = vocab;
    // ABRIR O FICHEIRO
    FILE *ficheiro = fopen(nome_ficheiro, "r");
    if (ficheiro == NULL) {
        // Se falhar, devolvo a lista vazia.
        return lista;
    }
    // Crio um buffer para trazer as linhas de texto.
    char buffer[2048];
    //-----------------------------------
    // Ler o Vocabulario
    int num_tokens = 0;
    //REMOVER ANTES DE APRESENTAR
    // Lemos a primeira linha do ficheiro, que indica a quantidade total de tokens.
    // Como o 'fgets' lê tudo como texto (string), usamos a função 'atoi' (ASCII to Integer)
    // para converter esse texto num número inteiro (int) que possamos usar no código.
    if (fgets(buffer, sizeof(buffer), ficheiro) != NULL) {
        num_tokens = atoi(buffer);
    }
    // Agora sei que tenho de ler X linhas, uma para cada palavra.
    for (int i = 0; i < num_tokens; i++) {
        if (fgets(buffer, sizeof(buffer), ficheiro) != NULL) {
            removerQuebraLinha(buffer);
            adicionarToken(lista.vocabulario, buffer); // Guardo na minha lista de tokens
        }
    }
    //------------------
    //Ler os Documentos
    int num_docs = 0;
    // Leio quantos documentos existem no ficheiro
    if (fgets(buffer, sizeof(buffer), ficheiro) != NULL) {
        num_docs = atoi(buffer);
    }
    for (int i = 0; i < num_docs; i++) {
        // Preparo variaveis temporarias
        char titulo[200];
        char texto[2048];
        // Leio o Titulo
        if (fgets(titulo, sizeof(titulo), ficheiro) != NULL) {
            removerQuebraLinha(titulo);
        }
        // Leio o Texto
        if (fgets(texto, sizeof(texto), ficheiro) != NULL) {
            removerQuebraLinha(texto);
        }
        //RETIRAR ANTES DE APRESENTAR!!!!! COMENTARIOS PARA COMPREENSAO APENAS
        // --- LIMPEZA DE DADOS REDUNDANTES ---
        // O ficheiro de texto contém as linhas "IDS: ..." e "TF: ..." para leitura humana.
        // No entanto, como a função 'adicionarDocumentoLista' recalcula automaticamente
        // os IDs e o vetor TF a partir do texto, não precisamos de usar os dados do ficheiro.
        // Fazemos estes dois fgets apenas para "ler e deitar fora" (consumir) essas linhas,
        // garantindo que o cursor avança para o título do próximo documento.
        fgets(buffer, sizeof(buffer), ficheiro); // Leio a linha dos IDS e deito fora (ignoro)
        fgets(buffer, sizeof(buffer), ficheiro); // Leio a linha do TF e deito fora (ignoro)
        // Adiciono o documento à lista.
        adicionarDocumentoLista(&lista, titulo, texto);
    }
    fclose(ficheiro);
    return lista;
}
//  Funções R2.4 - Ficheiros Binários

int guardarColecaoBinaria(const char *nome_ficheiro, t_lista_docs lista) {
    // 1. Abrir o ficheiro em modo BINARIO ("wb" = Write Binary).
    // Isto é crucial: no modo binário, o computador não altera os dados (não muda \n para \r\n).
    FILE *ficheiro = fopen(nome_ficheiro, "wb");
    if (ficheiro == NULL) {
        return 0;
    }
    // Guardar o Vocabulario
    // Descobrir quantos tokens temos
    int num_tokens = 0;
    if (lista.vocabulario != NULL) {
        num_tokens = lista.vocabulario->usados;
    }
    // Gravar o numero de tokens.
    fwrite(&num_tokens, sizeof(int), 1, ficheiro);
    // Gravar as palavras uma a uma.
    if (num_tokens > 0) {
        for (int i = 0; i < num_tokens; i++) {
            // Uso a minha funcao auxiliar que grava o tamanho da palavra + as letras
            if (!escreverStringBinario(ficheiro, lista.vocabulario->tokens[i])) {
                fclose(ficheiro);
                return 0;
            }
        }
    }
    // Guardar os Documentos
    // Gravar quantos documentos existem
    fwrite(&lista.num_documentos, sizeof(int), 1, ficheiro);
    t_doc *atual = lista.inicio;
    while (atual != NULL) {
        // Gravar Titulo e Texto (são strings, uso a funcao auxiliar)
        escreverStringBinario(ficheiro, atual->titulo);
        escreverStringBinario(ficheiro, atual->texto_original);
        //Tirar antes de Apresentar
        // --- O TRUQUE DE MESTRE DO BINARIO (Arrays de IDs) ---
        // Aqui está a grande vantagem do binário: Como os IDs já estão num array de inteiros (int*),
        // eles estão todos seguidos na memória RAM.
        // Em vez de fazer um ciclo e gravar numero a numero (lento), pego no bloco de memoria todo
        // e copio diretamente para o disco. É instantâneo!
        int n_ids = atual->num_ids;
        fwrite(&n_ids, sizeof(int), 1, ficheiro); // Gravo o tamanho do array
        if (n_ids > 0) {
            // Gravo o array inteiro de uma vez so (Block Write)
            fwrite(atual->ids_tokens, sizeof(int), n_ids, ficheiro);
        }
        // --- Repetir o truque para o Vetor TF ---
        // O vetor de frequencias (TF) tambem é um array de inteiros. Gravo tudo de uma vez.
        int n_tf = atual->vocab_size;
        fwrite(&n_tf, sizeof(int), 1, ficheiro); // Gravo o tamanho
        if (n_tf > 0) {
            fwrite(atual->vetor_tf, sizeof(int), n_tf, ficheiro);
        }
        atual = atual->proximo; // Avançar para o proximo documento
    }
    fclose(ficheiro);
    return 1;
}
t_lista_docs carregarColecaoBinaria(const char *nome_ficheiro) {
    //Preparar a Lista na Memoria
    t_lista_docs lista;
    lista.inicio = NULL;
    lista.num_documentos = 0;
    // Criar o Vocabulario.
    // Uso malloc para ele ficar na memoria permanente.
    lista.vocabulario = malloc(sizeof(MatrizTokens));
    *lista.vocabulario = criarMatrizTokens(10);
    //Abrir o ficheiro para Leitura Binaria ("rb" -> Read Binary)
    FILE *ficheiro = fopen(nome_ficheiro, "rb");
    if (ficheiro == NULL) {
        return lista; // Se falhar, devolvo a lista vazia
    }
    // Ler o Vocabulario
    // Primeiro leio quantos tokens existem
    int num_tokens = 0;
    fread(&num_tokens, sizeof(int), 1, ficheiro);
    for (int i = 0; i < num_tokens; i++) {
        // Uso a minha funcao auxiliar que sabe ler strings binarias
        char *token = lerStringBinario(ficheiro);
        if (token != NULL) {
            // ATENCAO: A funcao 'adicionarToken' faz a sua propria copia (strdup).
            adicionarToken(lista.vocabulario, token);
            // Como a lista ja tem a copia dela, nao preciso mais desta string temporaria.
            // Tenho de fazer free para nao encher a memoria de lixo.
            free(token);
        }
    }
    // Ler os Documentos
    int num_docs = 0;
    fread(&num_docs, sizeof(int), 1, ficheiro); // Leio o total de documentos
    for (int i = 0; i < num_docs; i++) {
        // Leio o Titulo e o Texto do disco para variaveis temporarias
        char *titulo = lerStringBinario(ficheiro);
        char *texto = lerStringBinario(ficheiro);
        if (titulo && texto) {
            //Ao adicionar o documento à lista,o programa vai recalcular os IDs e o TF automaticamente
            adicionarDocumentoLista(&lista, titulo, texto);
        }
        // Limpeza de memoria
        if(titulo) free(titulo);
        if(texto) free(texto);
        //ELIMINAR ANTES DE APRESENTAR
        // --- O SALTO MORTAL (SKIP) ---
        // O ficheiro tem os IDs e o TF guardados, mas eu acabei de recalcular tudo
        // na funcao 'adicionarDocumentoLista'. Entao, aqueles dados sao inuteis agora.
        // Preciso de "saltar" por cima deles para o cursor ficar pronto para o proximo documento.
        int n_ids, n_tf;
        // Leio quantos IDs estao gravados
        fread(&n_ids, sizeof(int), 1, ficheiro);
        // Se houver IDs, salto por cima deles.
        // FSEEK: Manda o cursor andar para a frente.
        // Contas: Se tenho 10 IDs, e cada ID ocupa 4 bytes (sizeof int), salto 40 bytes.
        // SEEK_CUR: Salta a partir de onde estas agora.
        if(n_ids > 0) {
            fseek(ficheiro, n_ids * sizeof(int), SEEK_CUR);
        }
        // Faço o mesmo para o vetor TF
        fread(&n_tf, sizeof(int), 1, ficheiro);
        if(n_tf > 0) {
            fseek(ficheiro, n_tf * sizeof(int), SEEK_CUR);
        }
    }
    fclose(ficheiro);
    return lista;
}
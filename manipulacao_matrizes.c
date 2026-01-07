#include "manipulacao_matrizes.h"

//  Funcoes auxiliares de texto (R1.1, R1.2)

/**
 * @brief Cria uma nova matriz de texto com uma capacidade inicial.
 * @param capacidade_inicial Tamanho inicial do array dinâmico.
 * @return MatrizTexto Estrutura inicializada.
 */
MatrizTexto criarMatrizTexto(int capacidade_inicial) {
    MatrizTexto m;//criamos uma variavel m do tipo Matriz texto,que contem(linhas,capacidade,linhas_usadas)
    m.linhas = malloc(capacidade_inicial * sizeof(char*));//estamos a alocar espaço para os endereços das frases
    if (m.linhas == NULL) {//verificaçao caso o malloc falhe, e retorne NULL
        printf("Erro: sem memoria.\n");
        exit(1);//termina impedindo escrever em memoria que nao existe
    }
    m.linhas_usadas = 0;//zero linhas para ja
    m.capacidade = capacidade_inicial;//inicia a capacidade consuante a capacidade inicial introduzida
    return m;//retornamos a estrutura ja criada
}

/**
 * @brief Redimensiona a matriz de texto quando esta fica cheia.
 * Utiliza `realloc` para aumentar a capacidade do array de ponteiros.
 * @param m Ponteiro para a matriz a redimensionar.
 * @param nova_capacidade O novo tamanho total desejado.
 * @return int 1 em caso de sucesso, 0 se falhar a alocação de memória.
 * @note **Complexidade:** O(N) onde N é o número de linhas, devido à cópia de ponteiros pelo realloc.
 */
int redimensionarMatriz(MatrizTexto *m, int nova_capacidade) {//usamos *m porque queremos alterar a funcao no main, e nao uma copia de m
    char **novas_linhas = realloc(m->linhas, nova_capacidade * sizeof(char*));//verifica se ha espaço,se houver estica o bloco mantendo o endereço, se nao procura um lugar na ram onde caiba
    if (novas_linhas == NULL) {//pode nao haver espaço, criamos novas linhas para caso nao dê nao perdemos as linhas todas
        printf("Erro: sem memoria ao redimensionar.\n");
        return 0;
    }
    m->linhas = novas_linhas;// atualizamos os registros, para mais linhas e para uma nova capacidade
    m->capacidade = nova_capacidade;
    return 1;
}

/**
 * @brief Adiciona uma nova frase à matriz.
 * Se a matriz estiver cheia, invoca automaticamente o redimensionamento.
 * @param m Ponteiro para a matriz.
 * @param frase A string a adicionar (será feita uma cópia dinâmica com strdup/malloc).
 * @return int 1 se sucesso, 0 se erro.
 * @note **Complexidade:** O(1) amortizado (devido ao redimensionamento ocasional).
 */
int adicionarLinha(MatrizTexto *m, const char *frase) {

    if (m->linhas_usadas == m->capacidade) {
        int correu_bem = redimensionarMatriz(m, m->capacidade * 2);
        if (correu_bem == 0) {
            return 0;
        }
    }
    // Calcula quanto espaço a frase precisa
    // O +1 é para o '\0' que marca o fim da string
    int tamanho_necessario = strlen(frase) + 1;

    // Criamos uma copia da frase na memoria
    // Usamos uma variavel temporaria 'nova_copia' para ser mais facil de ler
    char *nova_copia = malloc(tamanho_necessario);
    if (nova_copia == NULL) {
        printf("Erro: sem memoria para guardar a frase.\n");
        return 0;
    }
    // Copiar o texto para o espaço que acabamos de criar
    strcpy(nova_copia, frase);

    // Guardar a copia na nossa matriz
    int indice_livre = m->linhas_usadas;
    m->linhas[indice_livre] = nova_copia;
    // 6. Atualizar o contador
    m->linhas_usadas++;
    return 1;
}

/**
 * @brief Imprime todas as linhas armazenadas na consola.
 * @param m A matriz a listar.
 */
void listarMatriz(MatrizTexto m) {//funcao simples para listar a matriz ao utilizador
    printf("\nMatriz contem %d linhas:\n", m.linhas_usadas);
    for (int i = 0; i < m.linhas_usadas; i++) {
        printf("[%d] %s\n", i, m.linhas[i]);
    }
}

/**
 * @brief Liberta toda a memória associada à matriz.
 * Percorre cada linha libertando a string e, no fim, liberta o array de ponteiros.
 * @param m A matriz a destruir.
 */
void libertarMatriz(MatrizTexto m) {
    if (m.linhas != NULL) {// limpa o conteudo da matriz ou seja as frases
        for (int i = 0; i < m.linhas_usadas; i++) {
            if (m.linhas[i] != NULL)
                free(m.linhas[i]);
        }
        free(m.linhas);//podemos destruir o esqueleto que sobrou
    }
}

/**
 * @brief Pesquisa por uma substring dentro de todas as linhas da matriz.
 * Imprime as linhas onde a palavra ocorre.
 * @param m A matriz de texto.
 * @param palavra O termo a pesquisar (substring).
 * @note **Complexidade:** O(L * S), onde L é o nº de linhas e S o tamanho médio das strings (uso de strstr).
 */
void pesquisarTexto(MatrizTexto m, const char *palavra) {
    int encontrada = 0;
    for (int i = 0; i < m.linhas_usadas; i++) {//percorremos as linhas e pesquisamos por o que queremos utilizando a comparaçao de strings
        if (strstr(m.linhas[i], palavra) != NULL) {
            printf("Linha %d: %s\n", i, m.linhas[i]);//identificamos a linha e a respetiva frase
            encontrada++;
        }
    }
    if (encontrada == 0)
        printf("Nenhuma linha contem \"%s\".\n", palavra);
    else
        printf("%d linha(s) encontradas.\n", encontrada);
}

/**
 * @brief Remove uma linha num índice específico.
 * Desloca todas as linhas subsequentes para a esquerda para tapar o buraco.
 * @param m Ponteiro para a matriz.
 * @param indice O índice da linha a remover.
 * @return int 1 se sucesso, 0 se índice inválido.
 * @note **Complexidade:** O(N) no pior caso, pois necessita de deslocar elementos.
 */
int removerLinha(MatrizTexto *m, int indice) {//usamos *m porque queremos alterar a funcao no main, e nao uma copia de m
    if (indice < 0 || indice >= m->linhas_usadas) {//verificacao simples
        printf("Indice fora dos limites!\n");
        return 0; 
    }
    free(m->linhas[indice]);// limpamos a linha antes de a eliminar
    for (int i = indice; i < m->linhas_usadas - 1; i++) {// fazemos ate i+1 pois dentro do ciclo vamos ate i+1 o que iria tentar ler memoria fora do array
        m->linhas[i] = m->linhas[i + 1];// utilizamos o ciclo para deslocar as linhas
    }
    m->linhas_usadas--;
    printf("Linha %d removida.\n", indice);
    return 1;
}

/**
 * @brief Remove a primeira linha encontrada que seja idêntica ao conteúdo fornecido.
 * @param m Ponteiro para a matriz.
 * @param frase A frase exata a remover.
 * @return int 1 se removeu, 0 se não encontrou.
 */
int removerLinhaPorConteudo(MatrizTexto *m, const char *frase) {
    for (int i = 0; i < m->linhas_usadas; i++) {
        if (strcmp(m->linhas[i], frase) == 0) {
            return removerLinha(m, i);//utilizamos a funcao anteriormente feita para remover a linha
        }
    }
    printf("Nenhuma linha igual a \"%s\" foi encontrada.\n", frase);
    return 0;
}

//  Funcoes auxiliares de tokens (R1.1, R1.2)

/**
 * @brief Cria uma matriz de tokens (Vocabulário).
 * @param capacidade_inicial Capacidade inicial.
 * @return MatrizTokens Estrutura inicializada.
 */
MatrizTokens criarMatrizTokens(int capacidade_inicial) {
    MatrizTokens t;//variavel do tipo matriz tokens que contem(tokens, tokens usados, capacidade)
    t.tokens = malloc(capacidade_inicial * sizeof(char*));//alocamos espaço para os endereços onde vao ficar os tokens
    if (t.tokens == NULL) {
        printf("Erro: sem memoria para tokens.\n");
        exit(1);
    }
    t.usados = 0;//inicializamos a matriz toda a zero
    t.capacidade = capacidade_inicial;//inicializamos a capacidade consuante a capacidade pretendida
    return t;
}

/**
 * @brief Adiciona um token ao vocabulário.
 * @param t Ponteiro para a matriz de tokens.
 * @param token String do token a adicionar.
 * @return int 1 sucesso, 0 erro.
 */
int adicionarToken(MatrizTokens *t, const char *token) {
    if (t->usados == t->capacidade) {
        int nova_cap = t->capacidade * 2;
        char **novos = realloc(t->tokens, nova_cap * sizeof(char*));
        if (novos == NULL) {
            printf("Erro: sem memoria.\n");
            return 0;
        }
        t->tokens = novos;
        t->capacidade = nova_cap;
    }
    // cópia numa variável local simples
    int tamanho = strlen(token) + 1;
    char *temp = malloc(tamanho);
    if (temp == NULL) return 0; // Verificação de segurança
    strcpy(temp, token);
    //Guardar na estrutura
    int i = t->usados;
    t->tokens[i] = temp;
    t->usados++;

    return 1;
}

/**
 * @brief Lista todos os tokens e os seus índices (IDs).
 * @param t Matriz de tokens.
 */
void listarTokens(MatrizTokens t) {
    printf("\nTokens armazenados (%d):\n", t.usados);
    for (int i = 0; i < t.usados; i++) {
        printf("[%d] %s\n", i, t.tokens[i]);
    }
}

/**
 * @brief Remove um token pelo seu índice (ID).
 * @warning Ao remover um token, os IDs dos tokens seguintes mudam!
 * @param t Ponteiro para a matriz.
 * @param indice Índice a remover.
 * @return int 1 sucesso, 0 erro.
 */
int removerToken(MatrizTokens *t, int indice) {
    if (indice < 0 || indice >= t->usados) {
        printf("Indice de token fora dos limites!\n");
        return 0;
    }

    free(t->tokens[indice]);// limpamos antes de eliminar
    for (int i = indice; i < t->usados - 1; i++) {//ciclo para sobrepor as os tokens para apagar
        t->tokens[i] = t->tokens[i + 1];
    }
    t->usados--;
    printf("Token %d removido com sucesso.\n", indice);
    return 1;
}

/**
 * @brief Remove um token pelo seu conteúdo textual.
 * @param t Ponteiro para a matriz.
 * @param token String do token.
 * @return int 1 sucesso, 0 erro.
 */
int removerTokenPorConteudo(MatrizTokens *t, const char *token) {
    for (int i = 0; i < t->usados; i++) {
        if (strcmp(t->tokens[i], token) == 0) {
            return removerToken(t, i);
        }
    }
    printf("Token \"%s\" nao encontrado.\n", token);
    return 0;
}

/**
 * @brief Liberta a memória da matriz de tokens.
 * @param t Matriz a libertar.
 */
void libertarTokens(MatrizTokens t) {
    for (int i = 0; i < t.usados; i++) {
        free(t.tokens[i]);
    }
    free(t.tokens);
}
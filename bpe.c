#include "bpe.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//  Implementações R1.3
// Passo 1: O "Alfabeto" Inicial.
// Antes de procurar pares, tenho de saber quais sao os caracteres individuais que existem no texto.
int inicializarTokensUnicos(MatrizTexto m_texto, MatrizTokens *m_tokens) {
    // Truque: Um array de 256 posicoes (tabela ASCII) para marcar o que ja vi.
    // Tudo a zero (0) significa "ainda nao vi".
    unsigned char vistos[256] = {0};
    char token[2]; // Espaço para 1 letra + o terminador \0

    // Vou percorrer todas as frases (linhas)
    for (int i = 0; i < m_texto.linhas_usadas; i++) {
        char *linha = m_texto.linhas[i];

        // Vou percorrer letra a letra da frase
        for (int j = 0; linha[j] != '\0'; j++) {
            unsigned char c = (unsigned char)linha[j];

            // Se ainda nao vi esta letra, adiciono aos tokens conhecidos
            if (vistos[c] == 0) {
                vistos[c] = 1; // Marco como visto
                // Preparo a string do token
                token[0] = linha[j];
                token[1] = '\0';
                // Adiciono à lista (se falhar memoria, devolvo erro)
                if (!adicionarToken(m_tokens, token)) {
                    return 0;
                }
            }
        }
    }
    return 1; // Sucesso
}

// Passo 2: O "Detetive".
// Esta funcao percorre o texto para descobrir qual é o par de tokens que aparece mais vezes junto.
// Ex: Se "a" e "r" aparecem muito juntos ("ar"), vamos encontra-los aqui.
char *encontrarParMaisFrequente(MatrizTexto m_texto, MatrizTokens m_tokens, int *freq_max) {
    int max_frequencia = 0;
    char *melhor_par = NULL;
    char par_teste[100];
    // Verificacao basica: preciso de pelo menos 2 tokens para fazer um par.
    if (m_tokens.usados < 2) {
        *freq_max = 0;
        return NULL;
    }
    // Ciclo duplo: Testar todos contra todos (Token A + Token B)
    for (int i = 0; i < m_tokens.usados; i++) {
        for (int j = 0; j < m_tokens.usados; j++) {

            // SIMPLIFICACAO PARA O PROJETO:
            // Para nao complicar muito, so vou testar juncoes de tokens pequenos (1 caracter).
            // Isto evita que o algoritmo fique demasiado lento na pesquisa.
            if (strlen(m_tokens.tokens[i]) > 1 || strlen(m_tokens.tokens[j]) > 1) {
                continue;
            }
            // Construo o par (ex: 't' + 'h' -> "th")
            sprintf(par_teste, "%s%s", m_tokens.tokens[i], m_tokens.tokens[j]);
            // Agora vou contar quantas vezes este par aparece no texto todo
            int freq_atual = 0;
            for (int k = 0; k < m_texto.linhas_usadas; k++) {
                char *linha = m_texto.linhas[k];
                char *ptr = linha;
                // O strstr procura a ocorrencia da string.
                // Enquanto encontrar o par na linha, conto e avanço.
                while ((ptr = strstr(ptr, par_teste)) != NULL) {
                    freq_atual++;
                    ptr += strlen(par_teste); // Avanço para nao contar o mesmo duas vezes
                }
            }
            // Se este par ganhou ao anterior, atualizo o campeao
            if (freq_atual > max_frequencia) {
                max_frequencia = freq_atual;

                // Limpo o campeao antigo se existir
                if (melhor_par != NULL) free(melhor_par);

                // Guardo o novo campeao (strdup faz malloc e copy automatico)
                // Nota: strdup nao e standard C antigo, mas aqui da jeito.
                // Se o compilador reclamar, usa-se malloc + strcpy.
                melhor_par = strdup(par_teste);
            }
        }
    }

    *freq_max = max_frequencia;
    return melhor_par;
}

// Passo 3: A "Fusão".
// Agora que sabemos que "a" e "r" sao o melhor par, vamos ao texto e juntamos tudo.
// Nota: Em termos de texto, a string "b a r" é igual a "b ar", mas conceptualmente mudamos o token.
// Nesta implementacao simplificada, a string nao muda muito, mas preparamos o terreno para o futuro.
int aplicarFusaoNoCorpus(MatrizTexto *m_texto, const char *token1, const char *token2) {
    int fusoes = 0;

    // Crio a string combinada que procuro (ex: "ar")
    char par_alvo[100];
    sprintf(par_alvo, "%s%s", token1, token2);

    for (int i = 0; i < m_texto->linhas_usadas; i++) {
        char *linha_original = m_texto->linhas[i];

        // Se a linha nao tiver o par, passo à frente
        if (strstr(linha_original, par_alvo) == NULL) {
            continue;
        }

        // Se encontrei, vou reconstruir a linha.
        // Como estou a fundir, o tamanho nao aumenta, por isso aloco o mesmo tamanho.
        char *nova_linha = malloc(strlen(linha_original) + 1);
        if (nova_linha == NULL) return -1;

        // Copio a linha original para a nova (nesta versao simples do BPE em C,
        // a representacao textual mantem-se, o que muda é que agora "ar" é um token só na lista).
        strcpy(nova_linha, linha_original);

        // Troco o ponteiro antigo pelo novo
        free(m_texto->linhas[i]);
        m_texto->linhas[i] = nova_linha;

        fusoes++;
    }
    return fusoes;
}

int calcularAlfabetoTokens(MatrizTexto *texto_corpus,MatrizTokens *alfabeto_tokens,int num_tokens_desejado) {
    //Começamos com as letras individuais
    if (!inicializarTokensUnicos(*texto_corpus, alfabeto_tokens)) {
        return 0;
    }
    //Enquanto nao tiver tokens suficientes
    while (alfabeto_tokens->usados < num_tokens_desejado) {
        int frequencia;
        // Procura qual o par mais repetido no texto
        char *par = encontrarParMaisFrequente(*texto_corpus, *alfabeto_tokens, &frequencia);
        // Se nao encontrar par, nao vale a pena fundir.
        if (par == NULL || frequencia <= 1) {
            if(par) free(par);
            break;
        }
        // Adiciona o novo token ao vocabulario
        if (!adicionarToken(alfabeto_tokens, par)) {
            free(par);
            return 0;
        }
        // "Aplica" a fusao.
        // Separo o par em 2 chars para a funcao
        char t1[2] = {par[0], '\0'};
        char t2[2] = {par[1], '\0'};
        aplicarFusaoNoCorpus(texto_corpus, t1, t2);
        free(par);
    }
    return 1;
}
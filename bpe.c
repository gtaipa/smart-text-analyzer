#include "bpe.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// =============================================================
//  Função Auxiliar (Privada)
//  Serve para impedir que o algoritmo escolha repetidamente o mesmo par.
// =============================================================
int tokenJaExiste(MatrizTokens t, const char *str) {
    for (int i = 0; i < t.usados; i++) {
        if (strcmp(t.tokens[i], str) == 0) {
            return 1; // Já existe
        }
    }
    return 0; // Não existe
}

// =============================================================
//  Implementações R1.3
// =============================================================

// Passo 1: O "Alfabeto" Inicial.
int inicializarTokensUnicos(MatrizTexto m_texto, MatrizTokens *m_tokens) {
    unsigned char vistos[256] = {0};
    char token[2];

    for (int i = 0; i < m_texto.linhas_usadas; i++) {
        char *linha = m_texto.linhas[i];
        for (int j = 0; linha[j] != '\0'; j++) {
            unsigned char c = (unsigned char)linha[j];
            if (vistos[c] == 0) {
                vistos[c] = 1;
                token[0] = linha[j];
                token[1] = '\0';
                if (!adicionarToken(m_tokens, token)) {
                    return 0;
                }
            }
        }
    }
    return 1;
}

// Passo 2: O "Detetive".
char *encontrarParMaisFrequente(MatrizTexto m_texto, MatrizTokens m_tokens, int *freq_max) {
    int max_frequencia = 0;
    char *melhor_par = NULL;
    char par_teste[100];

    if (m_tokens.usados < 2) {
        *freq_max = 0;
        return NULL;
    }

    // Ciclo duplo: Testar todos contra todos
    for (int i = 0; i < m_tokens.usados; i++) {
        for (int j = 0; j < m_tokens.usados; j++) {

            // Tenta juntar o token[i] com o token[j]
            sprintf(par_teste, "%s%s", m_tokens.tokens[i], m_tokens.tokens[j]);

            // --- CORREÇÃO IMPORTANTE ---
            // Se este par ("ar") já existe no vocabulário, ignoramos!
            // Isto impede o loop infinito de adicionar sempre o mesmo.
            if (tokenJaExiste(m_tokens, par_teste)) {
                continue;
            }

            int freq_atual = 0;
            for (int k = 0; k < m_texto.linhas_usadas; k++) {
                char *linha = m_texto.linhas[k];
                char *ptr = linha;
                while ((ptr = strstr(ptr, par_teste)) != NULL) {
                    freq_atual++;
                    // Avança o tamanho do par para não contar sobreposições erradas
                    ptr += strlen(par_teste);
                }
            }

            if (freq_atual > max_frequencia) {
                max_frequencia = freq_atual;
                if (melhor_par != NULL) free(melhor_par);
                melhor_par = strdup(par_teste);
            }
        }
    }

    *freq_max = max_frequencia;
    return melhor_par;
}

// Passo 3: A "Fusão".
int aplicarFusaoNoCorpus(MatrizTexto *m_texto, const char *token1, const char *token2) {
    int fusoes = 0;
    char par_alvo[100];
    sprintf(par_alvo, "%s%s", token1, token2);

    for (int i = 0; i < m_texto->linhas_usadas; i++) {
        if (strstr(m_texto->linhas[i], par_alvo) != NULL) {
            fusoes++;
        }
    }
    return fusoes;
}

int calcularAlfabetoTokens(MatrizTexto *texto_corpus, MatrizTokens *alfabeto_tokens, int num_tokens_desejado) {
    if (!inicializarTokensUnicos(*texto_corpus, alfabeto_tokens)) {
        return 0;
    }

    while (alfabeto_tokens->usados < num_tokens_desejado) {
        int frequencia;
        char *par = encontrarParMaisFrequente(*texto_corpus, *alfabeto_tokens, &frequencia);

        // CORREÇÃO: Aceitar frequencia == 0 para apanhar pares únicos como "bar"
        if (par == NULL || frequencia == 0) {
            if(par) free(par);
            break;
        }

        if (!adicionarToken(alfabeto_tokens, par)) {
            free(par);
            return 0;
        }

        aplicarFusaoNoCorpus(texto_corpus, par, "");

        free(par);
    }
    return 1;
}
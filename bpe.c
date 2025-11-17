#include "bpe.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// =============================================================
//  Implementações R1.3 (BPE)
// =============================================================

int inicializarTokensUnicos(MatrizTexto m_texto, MatrizTokens *m_tokens) {
    unsigned char vistos[256] = {0};
    char token[2];

    for (int i = 0; i < m_texto.linhas_usadas; i++) {
        char *linha = m_texto.linhas[i];
        for (int j = 0; linha[j] != '\0'; j++) {
            unsigned char c = (unsigned char)linha[j];
            if (!vistos[c]) {
                vistos[c] = 1;
                token[0] = linha[j];
                token[1] = '\0';
                if (!adicionarToken(m_tokens, token))
                    return 0;
            }
        }
    }
    return 1;
}

char *encontrarParMaisFrequente(MatrizTexto m_texto, MatrizTokens m_tokens, int *freq_max) {
    int max_frequencia = 0;
    char *melhor_par = NULL;
    char temp[3];

    if (m_tokens.usados < 2) {
        *freq_max = 0;
        return NULL;
    }

    for (int i = 0; i < m_tokens.usados; i++) {
        char *t1 = m_tokens.tokens[i];
        if (strlen(t1) > 1) continue; // Simplificação: só pares de 1 char

        for (int j = 0; j < m_tokens.usados; j++) {
            char *t2 = m_tokens.tokens[j];
            if (strlen(t2) > 1) continue; // Simplificação

            temp[0] = t1[0];
            temp[1] = t2[0];
            temp[2] = '\0';

            int freq_atual = 0;
            for (int k = 0; k < m_texto.linhas_usadas; k++) {
                char *linha = m_texto.linhas[k];
                char *ptr = linha;
                while ((ptr = strstr(ptr, temp)) != NULL) {
                    freq_atual++;
                    ptr += 2; // Avança 2 para não sobrepor
                }
            }

            if (freq_atual > max_frequencia) {
                max_frequencia = freq_atual;
                free(melhor_par); // Liberta o melhor anterior
                melhor_par = strdup(temp);
                if (!melhor_par) return NULL; // Falha de memória
            }
        }
    }

    *freq_max = max_frequencia;
    return melhor_par;
}

int aplicarFusaoNoCorpus(MatrizTexto *m_texto, const char *token1, const char *token2) {
    int fusoes = 0;
    char par[3] = {token1[0], token2[0], '\0'};

    for (int i = 0; i < m_texto->linhas_usadas; i++) {
        char *orig = m_texto->linhas[i];
        int tam_orig = strlen(orig);
        int count = 0;
        char *ptr = orig;

        // 1. Contar ocorrências
        while ((ptr = strstr(ptr, par)) != NULL) {
            count++;
            ptr += 2;
        }
        if (count == 0) continue;

        // 2. Alocar nova linha (nesta simplificação, o tamanho não muda)
        char *nova = malloc(tam_orig + 1);
        if (!nova) return -1;

        // 3. Reconstruir a string
        char *p_orig = orig;
        char *p_nova = nova;
        char *next;

        while (*p_orig) {
            next = strstr(p_orig, par);
            if (!next) { // Não há mais pares
                strcpy(p_nova, p_orig); // Copia o resto
                break;
            }
            // Copia a parte ANTES do par
            int antes = next - p_orig;
            strncpy(p_nova, p_orig, antes);
            p_nova += antes;
            
            // "Funde" (copia o próprio par, mas como uma unidade)
            strcpy(p_nova, par);
            p_nova += 2;
            
            p_orig = next + 2; // Avança o ponteiro original
            fusoes++;
        }
        
        free(m_texto->linhas[i]); // Liberta a linha antiga
        m_texto->linhas[i] = nova; // Aponta para a nova
    }
    return fusoes;
}

int calcularAlfabetoTokens(MatrizTexto *texto_corpus,
                           MatrizTokens *alfabeto_tokens,
                           int num_tokens_desejado) {

    if (!inicializarTokensUnicos(*texto_corpus, alfabeto_tokens))
        return 0;

    while (alfabeto_tokens->usados < num_tokens_desejado) {
        int freq;
        char *par = encontrarParMaisFrequente(*texto_corpus, *alfabeto_tokens, &freq);

        // Condição de paragem: não há mais pares ou não são frequentes
        if (!par || freq <= 1) {
            free(par); // Liberta o par encontrado (se houver)
            break;
        }

        // Adiciona o novo token (ex: "an") ao alfabeto
        if (!adicionarToken(alfabeto_tokens, par)) {
            free(par);
            return 0;
        }

        // Reescreve o corpus (aqui é onde a simplificação acontece)
        // Substitui 'a' e 'n' por "an"
        if (aplicarFusaoNoCorpus(texto_corpus, &par[0], &par[1]) == -1) {
            free(par);
            return 0;
        }

        free(par); // Liberta a string do par
    }

    return 1;
}
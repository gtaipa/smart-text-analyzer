#include "vetorizacao.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//  Requisito 1.4: Tokenizacao das frases

// O objetivo desta funcao e transformar uma frase (texto) numa lista de numeros (IDs).
int *tokenizarFrase(const char *frase, MatrizTokens tokens, int *n_ids)
{
    // aloca lista inicial para ids
    int capacidade = 16;
    int usados = 0;
    int *ids = malloc(capacidade * sizeof(int));
    if (ids == NULL)
    {
        printf("Erro: O computador nao tem memoria suficiente.\n");
        exit(1);
    }

    int tamanho_frase = (int)strlen(frase);
    int posicao = 0;

    while (posicao < tamanho_frase)
    {
        int melhor_id = -1;
        int melhor_tamanho = 0;

        // percorre todos os tokens do vocabulario
        for (int t = 0; t < tokens.usados; t++)
        {
            const char *token_atual = tokens.tokens[t];
            int tamanho_token = (int)strlen(token_atual);

            if (tamanho_token <= 0)
            {
                continue;
            }

            // verifica se o token cabe na posicao atual
            if (posicao + tamanho_token > tamanho_frase)
            {
                continue;
            }

            // compara caractere a caractere usando a posicao (evita aritmetica de ponteiros)
            if (tamanho_token > melhor_tamanho)
            {
                if (strncmp(&frase[posicao], token_atual, tamanho_token) == 0)
                {
                    melhor_id = t;
                    melhor_tamanho = tamanho_token;
                }
            }
        }

        // se encontrou um token valido guarda e avanca o cursor
        if (melhor_id != -1)
        {
            if (usados == capacidade)
            {
                int nova_capacidade = capacidade * 2;
                int *tmp = realloc(ids, nova_capacidade * sizeof(int));
                if (tmp == NULL)
                {
                    printf("Erro ao redimensionar memoria.\n");
                    free(ids);
                    exit(1);
                }
                ids = tmp;
                capacidade = nova_capacidade;
            }

            ids[usados] = melhor_id;
            usados = usados + 1;
            posicao = posicao + melhor_tamanho;

        }
        else
        {
            // token desconhecido -> marca com -1 e avanca 1 caractere
            if (usados == capacidade)
            {
                int nova_capacidade = capacidade * 2;
                int *tmp = realloc(ids, nova_capacidade * sizeof(int));
                if (tmp == NULL)
                {
                    printf("Erro ao redimensionar memoria.\n");
                    free(ids);
                    exit(1);
                }
                ids = tmp;
                capacidade = nova_capacidade;
            }

            ids[usados] = -1;
            usados = usados + 1;
            posicao = posicao + 1;
        }
    }

    *n_ids = usados;
    return ids;
}

//  Requisito 1.5 - Estatisticas e Vetores


int *calcularFrequenciaTokens(MatrizTexto m_texto, MatrizTokens m_tokens, int *num_frequencias_out)
{
    int quantidade_tokens = m_tokens.usados;

    // aloca array de contadores e inicializa com zeros
    int *frequencias = (int *)calloc(quantidade_tokens, sizeof(int));
    if (frequencias == NULL)
    {
        printf("Erro: Sem memoria para contar frequencias.\n");
        *num_frequencias_out = 0;
        return NULL;
    }

    printf("\nA calcular a frequencia de %d tokens...\n", quantidade_tokens);

    // para cada token do vocabulario
    for (int t_idx = 0; t_idx < quantidade_tokens; t_idx++)
    {
        const char *token_atual = m_tokens.tokens[t_idx];
        int tamanho_token = (int)strlen(token_atual);
        int freq_atual = 0;

        // procura o token em cada linha do texto
        for (int l_idx = 0; l_idx < m_texto.linhas_usadas; l_idx++)
        {
            char *linha = m_texto.linhas[l_idx];
            int tamanho_linha = (int)strlen(linha);

            if (tamanho_token == 0)
            {
                continue;
            }

            // percorre a linha por indices, evitando aritmetica de ponteiros
            int pos = 0;
            while (pos + tamanho_token <= tamanho_linha)
            {
                if (strncmp(&linha[pos], token_atual, tamanho_token) == 0)
                {
                    freq_atual = freq_atual + 1;
                    pos = pos + tamanho_token;
                }
                else
                {
                    pos = pos + 1;
                }
            }
        }

        frequencias[t_idx] = freq_atual;
    }

    *num_frequencias_out = quantidade_tokens;
    printf("Calculo de frequencias concluido.\n");

    return frequencias;
}

// Esta funcao cria o vetor TF (Term Frequency) para UMA FRASE especifica.
// Transforma a lista de IDs [0, 5, 0] num histograma [2, 0, 0, 0, 0, 1]
// (Significa: ID 0 aparece 2 vezes, ID 5 aparece 1 vez)
int *calcularTfParaFrase(int *ids_frase, int num_ids_frase, int tamanho_vocabulario)
{
    // aloca e inicializa o vetor de frequencias com zeros
    int *tf_vetor = (int *)calloc(tamanho_vocabulario, sizeof(int));
    if (tf_vetor == NULL)
    {
        printf("Erro: sem memoria para vetor TF.\n");
        return NULL;
    }

    // itera pelos IDs que a tokenizacao nos deu
    for (int i = 0; i < num_ids_frase; i++)
    {
        int token_id = ids_frase[i];
        if (token_id >= 0 && token_id < tamanho_vocabulario)
        {
            tf_vetor[token_id] = tf_vetor[token_id] + 1;
        }
    }

    return tf_vetor;
}
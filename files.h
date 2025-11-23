//
// Created by pedro on 23/11/2025.
//

#ifndef FILES_H
#define FILES_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gestao_documentos.h"
#include "manipulacao_matrizes.h"

// =============================================================
//  Funcoes R2.3 - Ficheiros de Texto
// =============================================================

/**
 * @brief Guarda todo o estado (Vocabulario + Documentos + Dados TF) num ficheiro de texto.
 * @param nome_ficheiro Caminho para o ficheiro.
 * @param lista A lista de documentos (que contem o vocabulario).
 * @return 1 se sucesso, 0 se erro.
 */
int guardarColecaoTexto(const char *nome_ficheiro, t_lista_docs lista);

/**
 * @brief Carrega uma colecao a partir de um ficheiro de texto.
 * Reconstrói o Vocabulario e recalcula IDs/TF baseados no texto lido.
 * @param nome_ficheiro Caminho para o ficheiro.
 * @return Uma nova t_lista_docs preenchida.
 */
t_lista_docs carregarColecaoTexto(const char *nome_ficheiro);

int guardarColecaoBinaria(const char *nome_ficheiro, t_lista_docs lista);
t_lista_docs carregarColecaoBinaria(const char *nome_ficheiro);

// ficheiro: files.h

// ... (Incluções de cabeçalhos e declarações R2.3)

// =============================================================
//  Funções R2.4 - Ficheiros Binários
// =============================================================

/**
 * @brief Guarda todo o estado (Vocabulario + Documentos + Dados TF/IDs)
 * num ficheiro binário.
 * @param nome_ficheiro Caminho para o ficheiro binário (ex: "colecao.bin").
 * @param lista A lista de documentos.
 * @return 1 se sucesso, 0 se erro.
 */
int guardarColecaoBinaria(const char *nome_ficheiro, t_lista_docs lista);

/**
 * @brief Carrega uma colecao a partir de um ficheiro binário.
 * Recupera Vocabulario, Documentos, IDs e Vetores TF.
 * @param nome_ficheiro Caminho para o ficheiro binário.
 * @return Uma nova t_lista_docs preenchida.
 */
t_lista_docs carregarColecaoBinaria(const char *nome_ficheiro);


#endif //FILES_H

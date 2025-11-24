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
 * @brief Guarda a coleção completa (Vocabulário e Documentos) num ficheiro de texto legível.
 * Formato: Lista de tokens, seguida pela lista de documentos com os seus metadados.
 * @param nome_ficheiro Caminho/Nome do ficheiro de destino.
 * @param lista A estrutura contendo os dados a guardar.
 * @return int 1 em caso de sucesso, 0 se houver erro de I/O.
 */
int guardarColecaoTexto(const char *nome_ficheiro, t_lista_docs lista);

/**
 * @brief Carrega uma coleção a partir de um ficheiro de texto.
 * Lê o vocabulário e os textos, e recria as estruturas dinâmicas em memória (incluindo recalcular IDs e TF).
 * @param nome_ficheiro Caminho do ficheiro a ler.
 * @return t_lista_docs Estrutura carregada (deve ser libertada posteriormente).
 */
t_lista_docs carregarColecaoTexto(const char *nome_ficheiro);


// =============================================================
//  Funções R2.4 - Ficheiros Binários
// =============================================================

/**
 * @brief Guarda a coleção completa num ficheiro binário.
 * Mais eficiente que texto: guarda a representação exata da memória (tamanhos seguidos de bytes).
 * Inclui: Vocabulário, Textos, IDs de Tokens e Vetores TF.
 * @param nome_ficheiro Caminho do ficheiro binário (.bin).
 * @param lista A estrutura de dados a persistir.
 * @return int 1 em caso de sucesso, 0 se erro.
 */
int guardarColecaoBinaria(const char *nome_ficheiro, t_lista_docs lista);

/**
 * @brief Lê uma coleção completa de um ficheiro binário.
 * @param nome_ficheiro Caminho do ficheiro binário a ler.
 * @return t_lista_docs Estrutura carregada em memória.
 */
t_lista_docs carregarColecaoBinaria(const char *nome_ficheiro);

#endif //FILES_H
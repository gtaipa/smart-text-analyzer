#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "manipulacao_matrizes.h"
#include "bpe.h"
#include "vetorizacao.h"
#include "cal_similaridade.h"
#include "gestao_documentos.h"
#include "files.h"

//TESTES UNITARIOS
void testar_R1_1_e_R1_2() {
    printf("\n[R1.1 & R1.2] Teste Unitario: Matrizes Dinamicas\n");

    // 1. Criar e Adicionar
    MatrizTexto texto = criarMatrizTexto(2);
    adicionarLinha(&texto, "linha 1");
    adicionarLinha(&texto, "linha 2");
    // Forçar redimensionamento (Resize)
    adicionarLinha(&texto, "linha 3 (trigger resize)");

    if (texto.linhas_usadas == 3) {
        printf("[SUCESSO] Adicionar linhas e Redimensionar.\n");
    } else {
        printf("[ERRO] Falha no redimensionamento.\n");
    }

    // 2. Pesquisar
    printf(">> Teste Pesquisa:\n");
    pesquisarTexto(texto, "linha");

    // 3. Remover
    removerLinha(&texto, 1); // Remove "linha 2" (indice 1)

    // Verificar se a lista encolheu e se a "linha 3" passou para o lugar da "linha 2"
    if (texto.linhas_usadas == 2 && strcmp(texto.linhas[1], "linha 3 (trigger resize)") == 0) {
        printf("[SUCESSO] Remocao de linha e deslocamento de memoria.\n");
    } else {
        printf("[ERRO] Falha na remocao de linha.\n");
    }

    libertarMatriz(texto);
}

void testar_R1_3_BPE_Manual() {
    printf("\n [R1.3] Teste Unitario: BPE (Manual) \n");
    MatrizTexto corpus = criarMatrizTexto(2);

    // SEM ESPAÇOS: "bar par" para permitir fusão
    adicionarLinha(&corpus, "bar par");

    MatrizTokens tokens = criarMatrizTokens(10);

    // PEDIR 8 TOKENS: Para dar tempo de formar "b"+"ar" -> "bar"
    calcularAlfabetoTokens(&corpus, &tokens, 8);

    // --- MOSTRAR SEMPRE OS TOKENS ---
    printf("Estado final do Vocabulario (%d tokens):\n", tokens.usados);
    printf(">> ");
    for(int i=0; i<tokens.usados; i++) {
        printf("[%s] ", tokens.tokens[i]);
    }
    printf("\n");
    // ---------------------------------------------------------

    if (tokens.usados == 8) {
        printf("[SUCESSO] Numero de tokens gerados correto (8).\n");
    } else {
        printf("[ERRO] Numero de tokens incorreto (Gerados: %d).\n", tokens.usados);
    }

    // Verificar se o token composto "bar" foi criado
    int encontrou_bar = 0;
    for(int i=0; i<tokens.usados; i++) {
        if(strcmp(tokens.tokens[i], "bar") == 0) encontrou_bar = 1;
    }

    if (encontrou_bar) {
        printf("[SUCESSO] Token 'bar' encontrado no vocabulario.\n");
    } else {
        printf("[ERRO] Token 'bar' nao foi criado.\n");
    }

    libertarMatriz(corpus);
    libertarTokens(tokens);
}

void testar_R1_4_Tokenizacao() {
    printf("\n[R1.4] Teste Unitario: Tokenizacao (Greedy) \n");

    // Setup manual do vocabulario para controlar os IDs
    MatrizTokens t = criarMatrizTokens(5);
    adicionarToken(&t, "a");    // ID 0
    adicionarToken(&t, "b");    // ID 1
    adicionarToken(&t, "ab");   // ID 2 (Mais longo, deve ter preferencia)

    int n_ids;
    int *ids = tokenizarFrase("aba", t, &n_ids);

    // Esperado: "ab" (ID 2) + "a" (ID 0) -> IDs [2, 0]
    // Se fosse nao-greedy podia ser "a", "b", "a" [0, 1, 0]

    if (n_ids == 2 && ids[0] == 2 && ids[1] == 0) {
        printf("[SUCESSO] Greedy Match funcionou (escolheu o maior token primeiro).\n");
    } else {
        printf("[ERRO] Tokenizacao falhou ou nao foi Greedy.\n");
    }

    free(ids);
    libertarTokens(t);
}

void testar_R1_5_e_R1_6() {
    printf("\n[R1.5 & R1.6] Teste Unitario: TF e Similaridade \n");
    // Vetores simples para teste matematico
    // A = [1, 1, 0]
    // B = [1, 0, 1]
    int vetA[] = {1, 1, 0};
    int vetB[] = {1, 0, 1};
    int vocab_size = 3;

    double sim = calcularSimilaridadeCosseno(vetA, vetB, vocab_size);
    // Cosseno: (1*1 + 0 + 0) / (sqrt(2) * sqrt(2)) = 1 / 2 = 0.5

    printf("Similaridade calculada: %.2f (Esperado: 0.50)\n", sim);

    if (sim > 0.49 && sim < 0.51) {
        printf("[SUCESSO] Calculo de Cosseno correto.\n");
    } else {
        printf("[ERRO] Calculo de similaridade incorreto.\n");
    }
}

void testar_R2_1_ListaLigada() {
    printf("\n[R2.1] Teste Unitario: Lista Ligada (Inserir/Remover)\n");

    MatrizTokens v = criarMatrizTokens(1);
    adicionarToken(&v, "dummy");

    LL_TK_TF lista = criarListaDocumentos(&v);

    // Inserir 3 documentos
    adicionarDocumentoLista(&lista, "Doc1", "dummy");
    adicionarDocumentoLista(&lista, "Doc2", "dummy");
    adicionarDocumentoLista(&lista, "Doc3", "dummy");

    if (lista.num_documentos == 3) {
        printf("[SUCESSO] 3 Documentos inseridos.\n");
    } else {
        printf("[ERRO] Falha na insercao.\n");
    }

    // Remover do meio (Indice 1 -> Doc2)
    removerDocumentoPorIndice(&lista, 1);

    if (lista.num_documentos == 2) {
        printf("[SUCESSO] Contador atualizado apos remocao.\n");
    }

    TK_TF_NODE *doc_idx_1 = obterDocumentoPorIndice(lista, 1);
    // Agora o indice 1 deve ser o Doc3 (o antigo indice 2)
    if (doc_idx_1 != NULL && strcmp(doc_idx_1->titulo, "Doc3") == 0) {
        printf("[SUCESSO] Remocao correta (lista religada, Doc3 passou para idx 1).\n");
    } else {
        printf("[ERRO] Falha ao religar a lista apos remocao.\n");
    }
    libertarListaDocumentos(&lista);
    libertarTokens(v);
}

void testar_R2_3_e_R2_4_IO() {
    printf("\n [R2.3 & R2.4] Teste Unitario: I/O Ficheiros \n");

    // 1. Criar dados
    MatrizTokens v = criarMatrizTokens(2);
    adicionarToken(&v, "teste");
    LL_TK_TF lista = criarListaDocumentos(&v);
    adicionarDocumentoLista(&lista, "DocIO", "teste");

    // 2. Testar Binario
    const char *f_bin = "teste_r24.bin";
    guardarColecaoBinaria(f_bin, lista);

    LL_TK_TF lista_lida = carregarColecaoBinaria(f_bin);

    if (lista_lida.num_documentos == 1) {
        printf("[SUCESSO] Leitura Binaria: Numero Docs correto.\n");
    } else {
        printf("[ERRO] Leitura Binaria falhou.\n");
    }

    if(lista_lida.inicio != NULL) {
        if (strcmp(lista_lida.inicio->titulo, "DocIO") == 0) {
             printf("[SUCESSO] Leitura Binaria: Titulo preservado.\n");
        }
        if (lista_lida.inicio->ids_tokens != NULL) {
             printf("[SUCESSO] Leitura Binaria: IDs carregados/recalculados.\n");
        }
    }

    // Limpezas
    libertarListaDocumentos(&lista);
    // Lista lida tem o seu proprio vocabulario alocado no load
    if(lista_lida.vocabulario) {
        libertarTokens(*lista_lida.vocabulario);
        free(lista_lida.vocabulario);
    }
    libertarListaDocumentos(&lista_lida);
    libertarTokens(v);
}

void teste_integracao_final() {
    printf("CENARIO DE INTEGRACAO (Pipeline Completa)\n");

    // 1. Treinar BPE com frases variadas (Universal: pode ser qualquer texto)
    MatrizTexto corpus = criarMatrizTexto(5);
    adicionarLinha(&corpus, "o gato comeu o peixe");
    adicionarLinha(&corpus, "o cao ladra alto");
    adicionarLinha(&corpus, "o gato dorme");

    MatrizTokens vocab = criarMatrizTokens(20);
    // Vamos gerar 15 tokens para dar tempo de juntar palavras inteiras como "gato"
    calcularAlfabetoTokens(&corpus, &vocab, 40);

    printf("Vocabulario Aprendido: ");
    for(int i=0; i<vocab.usados; i++) printf("['%s'] ", vocab.tokens[i]);
    printf("\n");

    // 2. Criar Base de Documentos usando o vocabulario gerado
    LL_TK_TF bd = criarListaDocumentos(&vocab);
    adicionarDocumentoLista(&bd, "Noticia1", "o gato comeu");
    adicionarDocumentoLista(&bd, "Noticia2", "o cao ladra");
    adicionarDocumentoLista(&bd, "Noticia3", "peixe fresco");

    // 3. Simular Persistencia (Binario)
    guardarColecaoBinaria("db_integracao.bin", bd);

    // Limpar memoria para garantir que o load funciona mesmo e nao le da RAM
    libertarListaDocumentos(&bd);

    // 4. Carregar Base de Dados do disco
    LL_TK_TF bd_carregada = carregarColecaoBinaria("db_integracao.bin");

    if (bd_carregada.num_documentos == 3) {
        printf("[SUCESSO] Integracao: Base de dados carregada corretamente do disco.\n");
    } else {
        printf("[ERRO] Integracao: Falha ao carregar DB.\n");
    }

    // 5. Motor de Busca (Query)
    // Aqui testamos se o sistema funciona com inputs novos
    char *query = "gato";
    printf("\n>> Pesquisando por: '%s'\n", query);

    int n;
    int *ids_q = tokenizarFrase(query, *bd_carregada.vocabulario, &n);
    int *tf_q = calcularTfParaFrase(ids_q, n, bd_carregada.vocabulario->usados);

    TK_TF_NODE *atual = bd_carregada.inicio;
    while(atual) {
        double s = calcularSimilaridadeCosseno(tf_q, atual->vetor_tf, bd_carregada.vocabulario->usados);
        if(s > 0.0) {
            printf("   Documento '%s': Similaridade %.3f\n", atual->titulo, s);
        }
        atual = atual->proximo;
    }

    // Limpeza final
    free(ids_q); free(tf_q);
    libertarMatriz(corpus);

    // Limpar vocabulario e lista carregada
    if(bd_carregada.vocabulario) {
        libertarTokens(*bd_carregada.vocabulario);
        free(bd_carregada.vocabulario);
    }
    libertarListaDocumentos(&bd_carregada);
    // Limpar vocab inicial (bd original ja foi limpa antes)
    libertarTokens(vocab);
    printf("\n INTEGRACAO CONCLUIDA COM SUCESSO \n");
}


int main() {
    testar_R1_1_e_R1_2();
    testar_R1_3_BPE_Manual();
    testar_R1_4_Tokenizacao();
    testar_R1_5_e_R1_6();
    testar_R2_1_ListaLigada();
    testar_R2_3_e_R2_4_IO();
    teste_integracao_final();

    return 0;
}
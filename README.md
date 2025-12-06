# Projeto: Sistema de Análise de Texto (LP1 / AED1)

Descrição

Implementação de estruturas dinâmicas para armazenar texto e tokens, com operações básicas de inserção, remoção e pesquisa. Inclui funções de vetorização e cálculo de similaridade. O código está organizado por módulos (ver *.c / *.h).
Estado geral (mapa rápido dos requisitos)

R1.1 — Estruturas dinâmicas para texto e tokens: Implementado (manipulacao_matrizes.h / manipulacao_matrizes.c).
R1.2 — Inserir / remover / pesquisar: Implementado (adicionarLinha, removerLinha, pesquisarTexto, etc.).
R1.3 — Cálculo do alfabeto de tokens (BPE): Parcial — ficheiros bpe.h e possivelmente bpe.c presentes; validar comportamento com exemplos.
R1.4 — Tokenização greedy (longest-match): Parcial — ficheiros vetorizacao.* presentes; validar cobertura de casos.
R1.5 — Vetorização TF: Parcial — funções previstas em vetorizacao.*.
R1.6 — Similaridade (produto interno, cosseno): Implementado (cal_similaridade.c).
R2.1 — LL_TK_TF (lista ligada de vetorização): Não verificado / possivelmente não implementado.
R2.2 — MATRIX_STR / MATRIX_INT / DOC: Parcial — manipulacao_matrizes cobre MATRIX_STR; MATRIX_INT/DOC a confirmar.
R2.3 / R2.4 — I/O em ficheiros texto / binário: Parcial — ficheiros files.c/files.h existem; validar formatos.
R2.5 — Documentação e README: Parcial — comentários Doxygen iniciados; README adicionado agora.

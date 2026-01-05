Smart Text Analyzer

Sobre o Projeto
Este trabalho foi desenvolvido no âmbito das unidades curriculares de Algoritmos e Estruturas de Dados 1 e Laboratório de Programação 1. O objetivo principal foi criar um sistema em C capaz de analisar, armazenar e comparar textos de forma eficiente.
O programa funciona como um motor de busca simples: ele lê um conjunto de textos, "aprende" um vocabulário próprio usando o algoritmo BPE (Byte Pair Encoding) e, de seguida, permite comparar frases através de vetores numéricos e similaridade de cosseno.

Organização do Código
Para manter o projeto organizado, dividimos a lógica em módulos específicos. O núcleo do sistema baseia-se na gestão de matrizes dinâmicas (manipulacao_matrizes) que suportam tanto o armazenamento de texto como dos tokens.
Temos um módulo dedicado à aprendizagem (bpe), que analisa o corpus e funde os pares de caracteres mais frequentes, e outro para a vetorização (vetorizacao), que converte as frases em IDs e calcula as frequências (TF). A gestão da coleção de documentos é feita através de uma lista ligada (gestao_documentos), e os cálculos matemáticos de similaridade estão isolados no seu próprio ficheiro. Por fim, garantimos a persistência dos dados (leitura e escrita) através do módulo de ficheiros.

Estado dos Requisitos
Relativamente aos objetivos propostos no enunciado, considerámos que todos os requisitos foram implementados com sucesso.
No que toca à primeira fase, as estruturas dinâmicas e a sua manipulação (R1.1 e R1.2) estão funcionais. O algoritmo de aprendizagem de vocabulário BPE (R1.3) gera os tokens corretamente, e a tokenização segue a lógica "greedy" exigida (R1.4). A criação dos vetores de frequência (R1.5) e o cálculo da similaridade entre frases (R1.6) também estão concluídos.
Quanto à segunda fase, implementámos a lista ligada LL_TK_TF para gerir os documentos (R2.1) e adaptámos as estruturas agregadoras pedidas no R2.2 (usando TK_TF_NODE). A leitura e escrita de ficheiros (R2.3 e R2.4) funciona tanto para formato de texto como para formato binário. O código encontra-se comentado para geração de documentação Doxygen.


Pedro Rodrigues- 2024114929
Guilherme Taipa- 2024118263
Pedro Ramos- 2024110748

cmake_minimum_required(VERSION 3.29)
project(ProgetoAEDLP1 C)

set(CMAKE_C_STANDARD 17)

add_executable(ProgetoAEDLP1 main.c
        manipulacao_matrizes.c
        manipulacao_matrizes.h
        bpe.h
        bpe.c
        vetorizacao.h
        vetorizacao.c
        cal_similaridade.h
        cal_similaridade.c
)

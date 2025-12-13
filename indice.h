#ifndef INDICE_H
#define INDICE_H

#include <stdio.h>
#include <stdlib.h>
#include "geral.h"

#define TAMANHO_CABECALHO_INDICE 12
#define TAMANHO_REGISTRO_INDICE 12

// ----------------- Estruturas de Índice -----------------

typedef struct {
    char status;    // 1 byte      
    char lixo[11];  // 11 bytes
} CabecalhoIndice;  // 12 bytes 

typedef struct {
    int id;                 // 4 bytes
    long long byteOffSet;   // 8 bytes
} RegistroIndice;           // 12 bytes

// ----------------- Funções de Índice -----------------

void lerCabecalhoIndice(FILE *fpIndice, CabecalhoIndice *headerIndice);
void escreverCabecalhoIndice(FILE *fpIndice, CabecalhoIndice headerIndice);
void lerRegistroIndice(FILE *fpIndice, RegistroIndice *registroIndice);
void escreveRegistroIndice(FILE *fpIndice, RegistroIndice novoRegistro);

// Funções de manipulação e ordenação do índice
int compararIndices(const void *a, const void *b);
void atualizarEOrdenarArquivoIndice(FILE *fpIndice, char *nameFileIndice, RegistroIndice *novosRegistros, int numRegistrosNovos);
void atualizarIndiceComAdicoes(FILE *fpIndice, char *nomeArquivoIndice, RegistroIndice *novosRegistros, int numRegistrosNovos);

// Funções de reescrita complexa (remoções e atualizações)
void reescreverIndiceComRemocoes(FILE *fpIndice, char *nomeArquivoIndice, CabecalhoIndice headerIndice, int *idsParaRemover, int numRemovidos);
void reescreverIndiceComAtualizacoes(FILE *fpIndice, char *nomeArquivoIndice, int *idsParaRemover, int numRemovidos, RegistroIndice* registrosParaAdicionar, int numAdicionados);

// Funções em RAM
RegistroIndice* carregarIndiceRAM(FILE *fpIndice, int *numRegistros);
long long buscaBinariaIndiceRAM(RegistroIndice *indiceEmRAM, int numRegistros, int idBuscado);

#endif
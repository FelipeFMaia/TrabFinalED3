#ifndef PESSOA_H
#define PESSOA_H

#include <stdio.h>
#include <stdlib.h>
#include "geral.h"
#include "indice.h" 

#define TAMANHO_CABECALHO_PESSOA 17

// ----------------- Estruturas de Pessoa -----------------

typedef struct {
    char status;                // 1 byte 
    int qtdPessoas;             // 4 bytes
    int qtdRemovidos;           // 4 bytes
    long long proxByteOffSet;   // 8 bytes
} CabecalhoPessoa;              // 17 bytes

typedef struct {
    char removido;                          // 1 bytes
    int tamanhoRegistro;                    // 4 bytes
    int idPessoa;                           // 4 bytes
    int idadePessoa;                        // 4 bytes
    int tamanhoNomePessoa;                  // 4 bytes
    char nomePessoa[MAX_STRING_TAMANHO];    // MAX_STRING_TAMANHO bytes (definido em geral.h)
    int tamanhoNomeUsuario;                 // 4 bytes
    char nomeUsuario[MAX_STRING_TAMANHO];   // MAX_STRING_TAMANHO bytes
} RegistroPessoa;

// ----------------- Funções de Pessoa -----------------

// Leitura, escrita em arquivo e impressão
void lerCabecalhoPessoa(FILE *fpPessoa, CabecalhoPessoa *headerPessoa);
void escreveCabecalhoPessoa(FILE *fpPessoa, CabecalhoPessoa headerPessoa);
int lerRegistroPessoa(FILE *fpPessoa, RegistroPessoa *pessoa);
void escreverRegistroPessoa(FILE *fpPessoa, RegistroPessoa pessoa, int tamanhoDisponivel);
void imprimePessoa(RegistroPessoa pessoa);

// Manipulação de Registros
void removerRegistroPessoa(FILE* fpPessoa);
long long inserirPessoa(FILE* fpPessoa, RegistroPessoa novoRegistro, CabecalhoPessoa *headerPessoa);
int tamanhoUtilRegistro(RegistroPessoa pessoa);

// Buscas e Critérios
void lerCriteriosDeBusca(char nomeCampo[MAX_STRING_TAMANHO], char valorCampo[MAX_STRING_TAMANHO]);
int correspondeRegistro(char nomeCampo[MAX_STRING_TAMANHO], char valorCampo[MAX_STRING_TAMANHO], RegistroPessoa pessoa);

// Atualizações
void lerCriteriosDeAtualizacao(char *campoBusca, char *valorBusca, char *campoAtualiza, char *valorAtualiza);
void aplicarAtualizacao(RegistroPessoa *pessoa, char *campoAtualiza, char *valorAtualiza);

// Busca Complexa (Integração com Índice)
int buscarPessoas(FILE *fpPessoa, RegistroIndice *indiceEmRAM, int numRegIndice, CabecalhoPessoa *headerPessoa, char *nomeCampo, char *valorCampo, long long *vetorOffsets);


#endif
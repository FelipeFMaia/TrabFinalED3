#ifndef SEGUE_H
#define SEGUE_H

#include <stdio.h>
#include <stdlib.h>
#include "geral.h"

#define TAMANHO_CABECALHO_SEGUE 9
#define TAMANHO_REGISTRO_SEGUE 30

// ----------------- Estruturas de Segue -----------------

typedef struct {
    char status;            // 1 byte
    int quantidadePessoas;  // 4 bytes
    int proxRRN;            // 4 bytes
} CabecalhoSegue;           // 9 bytes

typedef struct {
    char removido;                  // 1 byte
    int idPessoaQueSegue;           // 4 bytes
    int idPessoaQueESeguida;        // 4 bytes
    char dataInicioQueSegue[10];    // 10 bytes
    char dataFimQueSegue[10];       // 10 bytes
    char grauAmizade;               // 1 byte
} RegistroSegue;                    // 30 bytes

// ----------------- Funções de Segue -----------------

void lerCabecalhoSegue(FILE *fpSegue, CabecalhoSegue *headerSegue);
void escreverCabecalhoSegue(FILE *fpSegue, CabecalhoSegue headerSegue);
void lerRegistroSegue(FILE *fpSegue, RegistroSegue *registroSegue);
void escreverRegistroSegue(FILE *fpSegue, RegistroSegue regSegue);

#endif
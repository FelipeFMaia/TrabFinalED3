#ifndef GERAL_H
#define GERAL_H

#include <stdio.h>
#include <stdlib.h>

#define MAX_STRING_TAMANHO 100

// Funções de verificação e exibição genérica
void binarioNaTela(char *nomeArquivoBinario);
void scan_quote_string(char *str);
int verificaArquivo(FILE *arquivo);
int verificaConsistenciaArquivo(FILE *arquivo, int arqID);
void atualizarConsistencia(FILE *arquivo, char consistencia);
void preencheStringCifrao(char *string);

#endif
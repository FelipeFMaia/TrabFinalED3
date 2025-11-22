#ifndef UTILIDADES_H
#define UTILIDADES_H

#define MAX_STRING_TAMANHO 100
#define TAMANHO_CABECALHO_INDICE 12
#define TAMANHO_REGISTRO_INDICE 12
#define TAMANHO_CABECALHO_PESSOA 17
#define TAMANHO_CABECALHO_SEGUE 9
#define TAMANHO_REGISTRO_SEGUE 30

#include <stdio.h>
#include <stdlib.h>


// ----------------- Arquivo de Índice -----------------
typedef struct{
    char status;    // 1 byte      
    char lixo[11];  // 11 bytes
} CabecalhoIndice;  // 12 bytes 

typedef struct{
    int id;                 // 4 bytes
    long long byteOffSet;   // 8 bytes
} RegistroIndice;           // 12 bytes


// ----------------- Arquivo de Dados -----------------

typedef struct{
    char status;                // 1 byte 
    int qtdPessoas;             // 4 bytes
    int qtdRemovidos;           // 4 bytes
    long long proxByteOffSet;   // 8 bytes
} CabecalhoPessoa;              // 17 bytes

// Estrutura para os registros de dados em memória (pessoa.bin)
typedef struct {
    char removido;                          // 1 bytes
    int tamanhoRegistro;                    // 4 bytes
    int idPessoa;                           // 4 bytes
    int idadePessoa;                        // 4 bytes
    int tamanhoNomePessoa;                  // 4 bytes
    char nomePessoa[MAX_STRING_TAMANHO];    // MAX_STRING_TAMANHO bytes
    int tamanhoNomeUsuario;                 // 4 bytes
    char nomeUsuario[MAX_STRING_TAMANHO];   // MAX_STRING_TAMANHO bytes
} RegistroPessoa;


// ----------------- Arquivo de Segue -----------------

typedef struct{
    char status;            // 1 byte
    int quantidadePessoas;  // 4 bytes
    int proxRRN;            // 4 bytes
} CabecalhoSegue;           // 9 bytes

typedef struct{
    char removido;                  // 1 byte
    int idPessoaQueSegue;           // 4 bytes
    int idPessoaQueESeguida;        // 4 bytes
    char dataInicioQueSegue[10];    // 10 bytes
    char dataFimQueSegue[10];       // 10 bytes
    char grauAmizade;               // 1 byte
} RegistroSegue;                    // 30 bytes

// ----------------- Funções Auxiliares -----------------

void binarioNaTela(char *nomeArquivoBinario);

void scan_quote_string(char *str);

void imprimePessoa(RegistroPessoa pessoa);

int verificaArquivo(FILE *arquivo);

int verificaConsistenciaArquivo(FILE *arquivo, int arqID);

void lerCabecalhoIndice(FILE *fpIndice, CabecalhoIndice *headerIndice);

void escreverCabecalhoIndice(FILE *fpIndice, CabecalhoIndice headerIndice);

void lerCabecalhoPessoa(FILE *fpPessoa, CabecalhoPessoa *headerPessoa);

void lerCabecalhoSegue(FILE *fpSegue, CabecalhoSegue *headerSegue);

void lerRegistroIndice(FILE *fpIndice, RegistroIndice *registroIndice);

int lerRegistroPessoa(FILE *fpPessoa, RegistroPessoa *pessoa);

void lerRegistroSegue(FILE *fpSegue, RegistroSegue *registroSegue);

void preencheStringCifrao(char *string);

void atualizarConsistencia(FILE *arquivo, char consistencia);

void lerCriteriosDeBusca (char nomeCampo[MAX_STRING_TAMANHO], char valorCampo[MAX_STRING_TAMANHO]);

int correspondeRegistro (char nomeCampo[MAX_STRING_TAMANHO], char valorCampo[MAX_STRING_TAMANHO], RegistroPessoa pessoa);

int compararIndices(const void *a, const void *b);

int tamanhoUtilRegistro (RegistroPessoa pessoa);

void escreveCabecalhoPessoa(FILE *fpPessoa, CabecalhoPessoa headerPessoa);

void removerRegistroPessoa(FILE* fpPessoa);

void escreveRegistroIndice(FILE *fpIndice, RegistroIndice novoRegistro);

void lerRegistroSegue(FILE *fpSegue, RegistroSegue *registroSegue);

void escreverRegistroSegue(FILE *fpSegue, RegistroSegue regSegue);

void escreverCabecalhoSegue(FILE *fpSegue, CabecalhoSegue headerSegue);

void aplicarAtualizacao (RegistroPessoa *pessoa, char *campoAtualiza, char *valorAtualiza);

void lerCriteriosDeAtualizacao (char *campoBusca, char *valorBusca, char *campoAtualiza, char *valorAtualiza);


void escreverRegistroPessoa(FILE *fpPessoa, RegistroPessoa pessoa, int tamanhoDisponivel);

long long inserirPessoa (FILE* fpPessoa, RegistroPessoa novoRegistro, CabecalhoPessoa *headerPessoa);

int buscarPessoasPorCriterio(FILE *fpPessoa, FILE *fpIndice, CabecalhoPessoa *headerPessoa, 
                             char *nomeCampo, char *valorCampo, long long **vetorOffsets);

// 
void atualizarIndiceComAdicoes(FILE *fpIndice, char *nomeArquivoIndice, 
                             RegistroIndice *novosRegistros, int numRegistrosNovos);


// Função para reescrever o índice, removendo IDs específicos
void reescreverIndiceComRemocoes(FILE *fpIndice, char *nomeArquivoIndice, CabecalhoIndice headerIndice,
                                 int *idsParaRemover, int numRemovidos);

// Função para reescrever o índice, removendo E adicionando IDs
void reescreverIndiceComAtualizacoes(FILE *fpIndice, char *nomeArquivoIndice, 
                                     int *idsParaRemover, int numRemovidos, 
                                     RegistroIndice* registrosParaAdicionar, int numAdicionados);

void atualizarEOrdenarArquivoIndice (FILE *fpIndice, char *nameFileIndice, RegistroIndice *novosRegistros, int numRegistrosNovos);

// Função para carregar o arquivo de índice na RAM
RegistroIndice* carregarIndiceRAM(FILE *fpIndice, int *numRegistros);

// Função para fazer a busca em RAM através de busca binária
long long buscaBinariaIndiceRAM(RegistroIndice *indiceEmRAM, int numRegistros, int idBuscado);

// Função para busca
int buscarPessoas(FILE *fpPessoa, RegistroIndice *indiceEmRAM, int numRegIndice, 
                  CabecalhoPessoa *headerPessoa, char *nomeCampo, 
                  char *valorCampo, long long *vetorOffsets);



#endif  

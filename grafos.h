#ifndef GRAFOS_H
#define GRAFOS_H

#include <stdio.h>
#include <stdlib.h>
#include "utilidades.h"
// ----------------- Estruturas para o Grafo -----------------

// Nó da lista encadeada de arestas (quem a pessoa segue)
typedef struct Aresta {
    int idDestino;              // idPessoa de quem é seguido
    char nomeDestino[MAX_STRING_TAMANHO]; // Guardamos o nome para facilitar a exibição/ordenação
    char grauAmizade;           // '0', '1', '2' ou '$'
    char dataInicio[11];        // DD/MM/AAAA\0
    char dataFim[11];           // DD/MM/AAAA\0
    struct Aresta *prox;        // Ponteiro para o próximo vizinho
} Aresta;

// Vértice do grafo (uma Pessoa)
typedef struct {
    int idPessoa;                           // ID da pessoa
    char nomePessoa[MAX_STRING_TAMANHO];    // Nome real 
    char nomeUsuario[MAX_STRING_TAMANHO];   // Chave de ordenação do vetor principal
    Aresta *inicioLista;                    // Cabeça da lista de adjacências
} Vertice;

// O Grafo em si
typedef struct {
    Vertice *listaVertices;     // Vetor dinâmico de vértices
    int numVertices;            // Quantidade total de pessoas (vértices)
} Grafo;

// ----------------- Estrutura para Fila (BFS) -----------------
typedef struct {
    int *itens;
    int inicio, fim;
    int capacidade;
    int tamanho;
} Fila;

// ----------------- Funções Auxiliares de Grafos -----------------


Grafo* criarGrafo(int numVertices);
void liberarGrafo(Grafo *grafo);
void adicionarAresta(Grafo *grafo, int indiceOrigem, int idDestino, char *nomeDestino, char *grau, char *dtInicio, char *dtFim);
int buscaBinariaPorIdNoGrafo(Grafo *grafo, int idProcurado);
int compararVerticesPorNomeUsuario(const void *a, const void *b);
void carregarVerticesDoArquivo(FILE *fpPessoa, Grafo *grafo);
void carregarArestasDoArquivo(FILE *fpSegue, Grafo *grafo, int modoTransposto);
void inserirArestaOrdenada(Aresta **cabeca, Aresta *novaAresta);


// Fila
Fila* criarFila(int capacidade);
void liberarFila(Fila *f);
void enfileirar(Fila *f, int item);
int desenfileirar(Fila *f);
int filaVazia(Fila *f);

#endif
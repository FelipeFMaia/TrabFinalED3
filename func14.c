#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "func.h"
#include "utilidades.h"

/**
 * Funcionalidade 14: Determinar o comprimento do ciclo (Fofoca)
 * Objetivo: Verificar se existe um caminho que começa na pessoa X e volta para X.
 * Retorno: Apenas o tamanho desse caminho (menor caminho, pois usamos BFS) ou mensagem de erro.
 * Estratégia:
 * 1. Montamos o Grafo Normal (G).
 * 2. Identificamos o índice da pessoa X.
 * 3. Iniciamos uma BFS inserindo os VIZINHOS de X na fila (distância 1).
 * 4. Se durante a BFS encontrarmos X novamente, achamos o ciclo.
 */
void func14() {
    char nameFilePessoa[MAX_STRING_TAMANHO];
    char nameFileIndice[MAX_STRING_TAMANHO];
    char nameFileSegue[MAX_STRING_TAMANHO];
    char nomeFofoqueiro[MAX_STRING_TAMANHO];

    // Leitura dos parâmetros conforme especificado
    scanf(" %s %s %s", nameFilePessoa, nameFileIndice, nameFileSegue);
    scan_quote_string(nomeFofoqueiro);

    FILE *fpPessoa = fopen(nameFilePessoa, "rb");
    FILE *fpSegue = fopen(nameFileSegue, "rb");

    // Validações de arquivo (padrão do projeto)
    if (verificaArquivo(fpPessoa) == 0) return;
    
    FILE *fpIndice = fopen(nameFileIndice, "rb");
    if (verificaArquivo(fpIndice) == 0) {
        fclose(fpPessoa);
        return;
    }
    fclose(fpIndice); // Índice não é usado na memória para grafos, apenas validado

    if (verificaArquivo(fpSegue) == 0) {
        fclose(fpPessoa);
        return;
    }

    // Leitura do cabeçalho de Pessoa
    CabecalhoPessoa headerPessoa;
    lerCabecalhoPessoa(fpPessoa, &headerPessoa);
    if (headerPessoa.status == '0') {
         printf("Falha na execução da funcionalidade.\n");
         fclose(fpPessoa); fclose(fpSegue); return;
    }

    // Criação do Grafo em Memória
    Grafo *grafo = criarGrafo(headerPessoa.qtdPessoas);
    if (grafo == NULL) {
        printf("Falha na execução da funcionalidade.\n");
        fclose(fpPessoa); fclose(fpSegue); return;
    }

    // Carregar Vértices
    carregarVerticesDoArquivo(fpPessoa, grafo);

    // Leitura do cabeçalho de Segue
    CabecalhoSegue headerSegue;
    lerCabecalhoSegue(fpSegue, &headerSegue);
    if (headerSegue.status == '0') {
         printf("Falha na execução da funcionalidade.\n");
         liberarGrafo(grafo); fclose(fpPessoa); fclose(fpSegue); return;
    }

    // Carregar Arestas no modo NORMAL (0), pois queremos saber quem segue quem
    carregarArestasDoArquivo(fpSegue, grafo, 0);

    // Encontrar o índice da pessoa que gerou a fofoca no vetor de vértices
    int indiceInicio = -1;
    // Como o vetor está ordenado por nomeUsuario (feito no carregarVertices), 
    // poderíamos usar busca binária por string, mas a busca linear é segura e simples aqui.
    for (int i = 0; i < grafo->numVertices; i++) {
        if (strcmp(grafo->listaVertices[i].nomeUsuario, nomeFofoqueiro) == 0) {
            indiceInicio = i;
            break;
        }
    }

    if (indiceInicio == -1) {
        // Se a pessoa não existe no grafo
        printf("Registro inexistente.\n"); 
        liberarGrafo(grafo); fclose(fpPessoa); fclose(fpSegue); return;
    }

    // --- Algoritmo de BFS para detecção de ciclo ---
    
    // Vetor de distâncias: -1 indica não visitado
    int *distancia = (int*)malloc(grafo->numVertices * sizeof(int));
    for(int i = 0; i < grafo->numVertices; i++) {
        distancia[i] = -1;
    }

    Fila *fila = criarFila(grafo->numVertices);
    
    // Passo crucial: Não colocamos o 'indiceInicio' na fila com distância 0.
    // Nós colocamos os VIZINHOS dele com distância 1.
    // Assim, se encontrarmos 'indiceInicio' novamente, sabemos que fechou o ciclo.

    Aresta *a = grafo->listaVertices[indiceInicio].inicioLista;
    int cicloEncontrado = 0;
    int tamanhoCiclo = 0;

    while(a != NULL) {
        // Verifica se a aresta é válida (dataFim == NULO, ou seja, '$')
        if (a->dataFim[0] == '$') {
            int v = buscaBinariaPorIdNoGrafo(grafo, a->idDestino);
            
            if (v != -1) {
                // Caso especial: A pessoa segue ela mesma (ciclo de tamanho 1)
                if (v == indiceInicio) {
                    cicloEncontrado = 1;
                    tamanhoCiclo = 1;
                    break;
                }
                
                // Adiciona vizinho na fila se ainda não foi visitado
                // Nota: Pode haver arestas duplicadas ou caminhos alternativos, 
                // só enfileiramos se distancia for -1 para garantir o menor caminho (BFS).
                if (distancia[v] == -1) {
                    distancia[v] = 1;
                    enfileirar(fila, v);
                }
            }
        }
        a = a->prox; // Próximo vizinho (já em ordem alfabética)
    }

    // Se não achou ciclo imediato (tamanho 1), roda a BFS
    if (!cicloEncontrado) {
        while(!filaVazia(fila)) {
            int u = desenfileirar(fila);
            
            // Percorre os vizinhos de u
            Aresta *vizinho = grafo->listaVertices[u].inicioLista;
            while(vizinho != NULL) {
                
                // Só considera relacionamentos ativos
                if (vizinho->dataFim[0] == '$') {
                    int w = buscaBinariaPorIdNoGrafo(grafo, vizinho->idDestino);
                    
                    if (w != -1) {
                        // VERIFICAÇÃO DO CICLO:
                        // Se o vizinho 'w' for a pessoa inicial, achamos o caminho de volta!
                        if (w == indiceInicio) {
                            cicloEncontrado = 1;
                            tamanhoCiclo = distancia[u] + 1;
                            break; // Sai do while interno
                        }
                        
                        // Se não é o inicio e não foi visitado, continua a busca
                        if (distancia[w] == -1) {
                            distancia[w] = distancia[u] + 1;
                            enfileirar(fila, w);
                        }
                    }
                }
                vizinho = vizinho->prox;
            }
            if (cicloEncontrado) break; // Sai do while da fila
        }
    }

    if (cicloEncontrado) {
        printf("%d\n", tamanhoCiclo);
    } else {
        printf("A FOFOCA NAO RETORNOU\n");
    }

    // Liberação de memória
    liberarFila(fila);
    free(distancia);
    liberarGrafo(grafo);
    fclose(fpPessoa);
    fclose(fpSegue);
}
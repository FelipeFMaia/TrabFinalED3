#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "func.h"
#include "utilidades.h"
#include "grafos.h"

/**
 * Funcionalidade 14: determinar o comprimento do ciclo de fofoca --> Verificar se existe um caminho que começa na pessoa X e volta para X
 * 1. montamos o grafo normal (G)
 * 2. identificamos o índice da pessoa X
 * 3. iniciamos uma BFS inserindo os vi\inhoa de X na fila --> distância 1
 * 4. se durante a BFS encontrarmos X novamente, achamos o ciclo
 */
void func14() {
    char nameFilePessoa[MAX_STRING_TAMANHO];
    char nameFileIndice[MAX_STRING_TAMANHO];
    char nameFileSegue[MAX_STRING_TAMANHO];
    char nomeFofoqueiro[MAX_STRING_TAMANHO];

    scanf(" %s %s %s", nameFilePessoa, nameFileIndice, nameFileSegue);
    scan_quote_string(nomeFofoqueiro); //ler nome do fofoqueiro com aspas

    // tenta abrir os arquivos de parametro binarios para leitura
    FILE *fpPessoa = fopen(nameFilePessoa, "rb");
    FILE *fpSegue = fopen(nameFileSegue, "rb");
    FILE *fpIndice = fopen(nameFileIndice, "rb");

    // verificação de segurança
    if (verificaArquivo(fpPessoa) == 0){
        return; // aborta funcionalidade
    }
    
    if (verificaArquivo(fpIndice) == 0) {
        fclose(fpPessoa);
        return; // aborta funcionalidade
    }
    

    if (verificaArquivo(fpSegue) == 0) {
        fclose(fpPessoa);
        fclose(fpIndice);
        return; // aborta funcionalidade
    }

    // leitura do cabeçalho de Pessoa
    CabecalhoPessoa headerPessoa;
    lerCabecalhoPessoa(fpPessoa, &headerPessoa);
    if (headerPessoa.status == '0') {   //caso o arquivo pessoa esteja inconsistente, aborta funcionalidade
         printf("Falha na execução da funcionalidade.\n");
         fclose(fpPessoa); fclose(fpSegue); fclose(fpIndice); return;
    }

    // criação do Grafo em Memória
    Grafo *grafo = criarGrafo(headerPessoa.qtdPessoas);
    if (grafo == NULL) {
        printf("Falha na execução da funcionalidade.\n");
        fclose(fpPessoa); fclose(fpSegue); fclose(fpIndice); return;
    }

    // carregar Vértices
    carregarVerticesDoArquivo(fpPessoa, grafo); // vertices são as pessoas do arquivo pessoas

    // leitura do cabeçalho de Segue
    CabecalhoSegue headerSegue;
    lerCabecalhoSegue(fpSegue, &headerSegue);
    if (headerSegue.status == '0') {    //caso o arquivo segue esteja inconsistente, aborta funcionalidade
         printf("Falha na execução da funcionalidade.\n");
         liberarGrafo(grafo); fclose(fpPessoa); fclose(fpSegue); return;
    }

    // carregar Arestas no modo NORMAL (0), pois queremos saber quem segue quem
    carregarArestasDoArquivo(fpSegue, grafo, 0);

    // encontrar o índice da pessoa que gerou a fofoca no vetor de vértices
    int indiceInicio = -1; // flag pra encontrar o fofoqueiro
    
    // busca linear
    for (int i = 0; i < grafo->numVertices; i++) {
        if (strcmp(grafo->listaVertices[i].nomeUsuario, nomeFofoqueiro) == 0) {
            indiceInicio = i; // salva indice do fofoqueiro
            break;
        }
    }

    if (indiceInicio == -1) {
        // se a pessoa não existe no grafo
        printf("Registro inexistente.\n"); 
        liberarGrafo(grafo); fclose(fpPessoa); fclose(fpSegue); fclose(fpIndice); return;
    }

    // algoritmo de BFS para detecção de ciclo
    
    // quando o vetor de distâncias = -1 , significa que não foi visitado
    int *distancia = (int*)malloc(grafo->numVertices * sizeof(int)); // vetor dinâmico de inteiro com a qtd de vertices do grafo
    for(int i = 0; i < grafo->numVertices; i++) {
        distancia[i] = -1;  // inicializa o vetor distancia com tudo "nao visitado"
    }

    Fila *fila = criarFila(grafo->numVertices); // fila com qtd de vertices do grafo

    // o indiceInicio na fila não tem distancia 0, mas colocamos seus vizinhos com distancia 1
    // assim se encontrarmos o indiceInicio novamente, fechamos o ciclo
    Aresta *a = grafo->listaVertices[indiceInicio].inicioLista;

    int cicloEncontrado = 0;    // flag para encontro do ciclo
    int tamanhoCiclo = 0;

    while(a != NULL) {
        // verifica se a aresta é válida --> dataFim == NULO, ou seja $
        if (a->dataFim[0] == '$') {
            int v = buscaBinariaPorIdNoGrafo(grafo, a->idDestino);
            
            if (v != -1) {
                // a pessoa segue ela mesma --> ciclo de tamanho 1
                if (v == indiceInicio) {
                    cicloEncontrado = 1;
                    tamanhoCiclo = 1;
                    break;
                }

                // pode haver arestas duplicadas ou caminhos alternativos, só enfileiramos se distancia for -1 para garantir o menor caminho (BFS)
                if (distancia[v] == -1) {
                    distancia[v] = 1;
                    enfileirar(fila, v);
                }
            }
        }
        a = a->prox; // próximo vizinho (já em ordem alfabética)
    }

    // se não achou ciclo imediato de tamanho 1, roda a BFS
    if (!cicloEncontrado) {
        while(!filaVazia(fila)) {
            int u = desenfileirar(fila);
            
            // percorre os vizinhos de u
            Aresta *vizinho = grafo->listaVertices[u].inicioLista;
            while(vizinho != NULL) {
                
                // só considera relacionamentos ativos
                if (vizinho->dataFim[0] == '$') {
                    int w = buscaBinariaPorIdNoGrafo(grafo, vizinho->idDestino);
                    
                    if (w != -1) {
                        // se o vizinho w for a pessoa inicial --> achamos o caminho de volta
                        if (w == indiceInicio) {
                            cicloEncontrado = 1;
                            tamanhoCiclo = distancia[u] + 1;
                            break; // sai do while interno
                        }
                        
                        // se não é o inicio e não foi visitado --> continua a busca
                        if (distancia[w] == -1) {
                            distancia[w] = distancia[u] + 1;
                            enfileirar(fila, w);
                        }
                    }
                }
                vizinho = vizinho->prox;
            }
            if (cicloEncontrado) break; // sai do while da fila
        }
    }

    if (cicloEncontrado) {
        printf("%d\n", tamanhoCiclo);   // encontramos o ciclo
    } else {
        printf("A FOFOCA NAO RETORNOU\n");  // o while expirou sem que encontrassemos o ciclo
    }

    // liberação de memória
    liberarFila(fila);
    free(distancia);
    liberarGrafo(grafo);
    fclose(fpPessoa);
    fclose(fpSegue);
    fclose(fpIndice);
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "func.h"
#include "geral.h"
#include "pessoa.h"
#include "segue.h"
#include "grafos.h"

/**
 * Funcionalidade 14: Determinar o comprimento do ciclo de fofoca --> BFS para encontrar ciclo
 * 1. Montar o grafo direcionado (quem segue quem)
 * 2. Encontrar o índice da pessoa que iniciou a fofoca (Origem)
 * 3. Iniciar BFS: Inserir todos os vizinhos diretos da Origem na fila com distância 1
 * 4. Executar a BFS. Se encontrarmos a pessoa Origem novamente, o ciclo mais curto foi fechado
 * 5. Imprimir o comprimento do caminho (distância acumulada)
 */
void func14() {
    char nameFilePessoa[MAX_STRING_TAMANHO];
    char nameFileIndice[MAX_STRING_TAMANHO];
    char nameFileSegue[MAX_STRING_TAMANHO];
    char nomeFofoqueiro[MAX_STRING_TAMANHO];

    // Pegar do terminal os nomes dos arquivos e nomeFofoqueiro
    scanf(" %s %s %s", nameFilePessoa, nameFileIndice, nameFileSegue);
    scan_quote_string(nomeFofoqueiro); // Ler nome da pessoa que gerou a fofoca com aspas

    // Tentar abrir os arquivos binários para leitura, em modo de leitura binária
    FILE *fpPessoa = fopen(nameFilePessoa, "rb");
    FILE *fpSegue = fopen(nameFileSegue, "rb");
    FILE *fpIndice = fopen(nameFileIndice, "rb");

    // Verificação de segurança
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

    // Ler cabeçalho do arquivo Pessoa e verificar a consistência
    CabecalhoPessoa headerPessoa;
    lerCabecalhoPessoa(fpPessoa, &headerPessoa);
    if (verificaConsistenciaArquivo(fpPessoa, 2) == 0) {   // Caso o arquivo Pessoa esteja inconsistente, abortar funcionalidade
        printf("Falha na execução da funcionalidade.\n");
        fclose(fpPessoa);
        fclose(fpSegue);
        fclose(fpIndice);
        return;
    }

    // Criar grafo
    Grafo *grafo = criarGrafo(headerPessoa.qtdPessoas);

    // Carregar vértices do grafo
    carregarVerticesDoArquivo(fpPessoa, grafo); // Vértices são as pessoas do arquivo pessoas

    // Ler cabeçalho do arquivo Segue e verificar a consistência
    CabecalhoSegue headerSegue;
    lerCabecalhoSegue(fpSegue, &headerSegue);
    if (verificaConsistenciaArquivo(fpSegue, 3) == 0) {    // Caso o arquivo Segue esteja inconsistente, abortar funcionalidade
        printf("Falha na execução da funcionalidade.\n");
        liberarGrafo(grafo);
        fclose(fpPessoa);
        fclose(fpSegue);
        fclose(fpIndice);
        return;
    }

    // Carregar arestas no modo NORMAL (parâmetro 0)
    // Grafo direcionado: A -> B significa que A segue B (A conta fofoca para B)
    carregarArestasDoArquivo(fpSegue, grafo, 0);

    // Encontrar índice da pessoa fofoqueira (Origem)
    int indiceFofoqueiro = -1; // flag pra encontrar o índice
    for (int i = 0; i < grafo->numVertices; i++) {
        if (strcmp(grafo->listaVertices[i].nomeUsuario, nomeFofoqueiro) == 0) {
            indiceFofoqueiro = i; // salva o indice
            break;
        }
    }

    // Se a pessoa não existe (indice==-1)
    if (indiceFofoqueiro == -1) {
        printf("Falha na execução da funcionalidade.\n");
        // liberar memória e fechar arquivos
        liberarGrafo(grafo);
        fclose(fpPessoa);
        fclose(fpSegue);
        fclose(fpIndice);
        return;
    }

    // ------------------- BFS (Breadth-First Search) para Ciclo -------------------

    // Alocar vetor para armazenar distâncias
    // Ele também serve como vetor de "visitados": se distancia[i] == -1, não foi visitado
    int *distancia = (int*)malloc(grafo->numVertices * sizeof(int)); 
    for(int i = 0; i < grafo->numVertices; i++) {
        distancia[i] = -1;  // inicializar com -1 (não visitado)
    }

    // Criar fila com um tamanho seguro
    Fila *fila = criarFila(grafo->numVertices);

    // Passo Diferenciado: Não colocamos o Fofoqueiro na fila com distância 0
    // Colocamos os VIZINHOS do Fofoqueiro na fila com distância 1
    // Motivo: Queremos encontrar quando a busca RETORNA ao Fofoqueiro
    
    Aresta *vizinhoInicial = grafo->listaVertices[indiceFofoqueiro].inicioLista;
    int cicloEncontrado = 0;    // flag de sucesso
    int tamanhoCiclo = 0;

    while(vizinhoInicial != NULL) {
        // Filtrar as pessoas que ainda seguem (dataFim == NULO/$)
        if (vizinhoInicial->dataFim[0] == '$') {
            int indiceVizinho = buscaIndicePorIdNoGrafo(grafo, vizinhoInicial->idDestino);
            
            if (indiceVizinho != -1) {
                // Caso especial: Auto-loop (a pessoa segue ela mesma)
                if (indiceVizinho == indiceFofoqueiro) {
                    cicloEncontrado = 1;
                    tamanhoCiclo = 1;
                    break;
                }

                // Se ainda não foi visitado, marca distância 1 e enfileira
                if (distancia[indiceVizinho] == -1) {
                    distancia[indiceVizinho] = 1;
                    enfileirar(fila, indiceVizinho);
                }
            }
        }
        // Ir para o próximo vizinho (respeitando ordem alfabética da inserção)
        vizinhoInicial = vizinhoInicial->prox;
    }

    // Loop principal da BFS (se não achou auto-loop logo de cara)
    if (!cicloEncontrado) {
        while(!filaVazia(fila)) {
            // Desenfileirar o nó atual
            int noAtual = desenfileirar(fila);
            
            // Percorrer os vizinhos do noAtual
            Aresta *vizinho = grafo->listaVertices[noAtual].inicioLista;
            while(vizinho != NULL) {
                
                // Verificar se a relação ainda existe (ativo)
                if (vizinho->dataFim[0] == '$') {
                    int indiceVizinho = buscaIndicePorIdNoGrafo(grafo, vizinho->idDestino);
                    
                    if (indiceVizinho != -1) {
                        // VERIFICAÇÃO DO CICLO: Se o vizinho for o próprio Fofoqueiro
                        if (indiceVizinho == indiceFofoqueiro) {
                            cicloEncontrado = 1;
                            tamanhoCiclo = distancia[noAtual] + 1; // Distância até atual + 1 salto para voltar à origem
                            break; // Quebra o while interno
                        }
                        
                        // Se não é a origem e não foi visitado, continua a propagação
                        if (distancia[indiceVizinho] == -1) {
                            distancia[indiceVizinho] = distancia[noAtual] + 1;
                            enfileirar(fila, indiceVizinho);
                        }
                    }
                }
                vizinho = vizinho->prox;
            }
            if (cicloEncontrado) break; // Quebra o while da fila
        }
    }

    // --------------------------- Resultado ---------------------------
    
    if (cicloEncontrado) {
        printf("%d\n", tamanhoCiclo);
    } else {
        printf("A FOFOCA NAO RETORNOU\n");
    }

    // Liberar memória e fechar arquivos
    liberarFila(fila);
    free(distancia);
    liberarGrafo(grafo);
    fclose(fpPessoa);
    fclose(fpSegue);
    fclose(fpIndice);
}
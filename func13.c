#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "func.h"
#include "geral.h"
#include "pessoa.h"
#include "segue.h"
#include "grafos.h"

/**
 * Funcionalidade 13: Caminho mais curto até a celebridade --> BFS no grafo transposto
 * 1. Montar o grafo transposto (direções invertidas)
 * 2. Rodar BFS partindo do nó Celebridade
 * 3. A BFS preenche o vetor pai. Se pai[i] != -1 para um nó 'i', existe o caminho mais curto de 'i' para a celebridade no grafo original
 * 4. Imprime os caminhos ativos (onde dataFim é NULO, representado por '$') na ordem correta
 */
void func13() {
    char nameFilePessoa[MAX_STRING_TAMANHO];
    char nameFileIndice[MAX_STRING_TAMANHO];
    char nameFileSegue[MAX_STRING_TAMANHO];
    char nomeCelebridade[MAX_STRING_TAMANHO];

    // Pegar do terminal os nomes dos arquivos e nomeCelebridade
    scanf(" %s %s %s", nameFilePessoa, nameFileIndice, nameFileSegue);
    scan_quote_string(nomeCelebridade); // Ler nome da celebridade com aspas

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

    // carregar arestas no modo TRANSPOSTO (parâmetro 1)
    // BFS saindo do destino no grafo transposto encontra todos as origens
    carregarArestasDoArquivo(fpSegue, grafo, 1);

    // Encontrar índice da celebridade
    int indiceCeleb = -1; // flag pra encontrar o índice
    for (int i = 0; i < grafo->numVertices; i++) {
        if (strcmp(grafo->listaVertices[i].nomeUsuario, nomeCelebridade) == 0) {
            indiceCeleb = i; // salva o indice
            break;
        }
    }

    // Se a celebridade não existe (indice==-1)
    if (indiceCeleb == -1) {
        printf("Falha na execução da funcionalidade.\n");
        // liberar memória e fechar arquivos
        liberarGrafo(grafo);
        fclose(fpPessoa);
        fclose(fpSegue);
        fclose(fpIndice);
        return;
    }

    // ------------------- BFS (Breadth-First Search) -------------------

    // Alocar vetores para armazenar pai's e visitado's para o algoritmo com um tamanho de quantidade de vértices
    int *pai = (int*)malloc(grafo->numVertices * sizeof(int)); // vetor que armazena o índice do nó (no grafo) visitado imediatamente anterior ao nó do índice atual
    int *visitado = (int*)malloc(grafo->numVertices * sizeof(int)); // vetor que indica se o nó de índice i foi visitado
    for(int i=0; i<grafo->numVertices; i++) {   // inicializar os vetores
        pai[i] = -1; // inicializar com -1 
        visitado[i] = 0; // inicializar com 0 (não visitado)
    }

    // Criar fila com o tamanho do número de vértices do grafo
    Fila *fila = criarFila(grafo->numVertices);
    enfileirar(fila, indiceCeleb); // Primeiro da fila é a celebridade, por usarmos a lógica transposta
    visitado[indiceCeleb] = 1; // Celebridade foi visitado   
    pai[indiceCeleb] = indiceCeleb; // Pai dele mesmo (marcar raíz)

    // Loop principal do algoritmo
    while(!filaVazia(fila)) {
        // Desenfileirar o primeiro nó da fila do algoritmo
        int noAtual = desenfileirar(fila);
        
        // Pegar a lista das arestas vizinhas do nó desenfileirado
        Aresta *vizinho = grafo->listaVertices[noAtual].inicioLista;

        // Percorrer os vizinhos do noAtual na lista de adjacências, respeitando a ordem alfabética para desempate
        while(vizinho != NULL) {
            // Filtrar as pessoas que ainda seguem, ou seja, cuja dataFim é nulo ($)
            if (vizinho->dataFim[0] == '$') { 
                // Usando buscaIndicePorIdNoGrafo para pegar o índice do vizinho
                // nos vetores (pai e visitados) e atualizá-los
                // No grafo transposto, 'vizinho->idDestino' é a pessoa que segue 'noAtual' (no original)
                int indiceVizinho = buscaIndicePorIdNoGrafo(grafo, vizinho->idDestino);
                
                if (indiceVizinho != -1 && visitado[indiceVizinho] == 0) {
                    visitado[indiceVizinho] = 1; // marcar como visitado de uma vez, evitando que o pai seja sobrescrito e garantindo o caminho mínimo
                    pai[indiceVizinho] = noAtual; // o pai do vizinho é noAtual
                    enfileirar(fila, indiceVizinho);
                }
            }
            // Ir para o próximo vizinho (ou aresta) da lista
            vizinho = vizinho->prox;
        }
    }

    // --------------------------- Resultado da BFS ---------------------------
    // Iterando sobre o grafo pegando/reconstuindo o caminho de quem segue quem até chegar na celebridade
    // E printando conforme especificado

    // Loop principal, iterando sobre todos os vertices, exceto a própria celebridade
    for (int i = 0; i < grafo->numVertices; i++) {
        if (i == indiceCeleb) continue; // pular o indice da celebridade

        if (pai[i] == -1) {  // Nós que não tem pai não seguem ninguém, logo não seguem a celebridade (pensando no transposto)
            printf("NAO SEGUE A CELEBRIDADE\n");
        } else {
            // Reconstruir caminho: i -> pai[i] -> pai[pai[i]] ... -> Celebridade
            int atual = i;
            while (atual != indiceCeleb) {
                int proximo = pai[atual];
                
                // É necessário recuperar os dados da aresta (atual -> proximo) no grafo original
                // No grafo transposto, tem-se a aresta (proximo -> atual)
                // Deve-se procurar o atual na lista de adjacência de proximo
                
                Aresta *arestaTransposta = grafo->listaVertices[proximo].inicioLista;
                while (arestaTransposta != NULL) { // Iterar sobre todas as arestas da lista
                    // Condição para encontrar o nó "atual" na lista de adjacência de "proximo"
                    // (que no transposto é a aresta que descobre "atual")
                    if (arestaTransposta->idDestino == grafo->listaVertices[atual].idPessoa) {
                        // imprimir conforme especificado
                        printf("%s, %s, %s, ", 
                            grafo->listaVertices[atual].nomeUsuario, 
                            grafo->listaVertices[proximo].nomeUsuario, 
                            arestaTransposta->dataInicio);
                            
                        if(arestaTransposta->dataFim[0] == '$'){
                            printf("%s, ", "NULO");
                        }else {
                            printf("%s, ", arestaTransposta->dataFim);
                        }

                        if (arestaTransposta->grauAmizade == '$'){
                            printf("NULO\n");
                        }else{
                            printf("%c\n", arestaTransposta->grauAmizade);
                        }

                        break;
                    }
                    arestaTransposta = arestaTransposta->prox;
                }
                
                atual = proximo;
            }
        }
        // Pula linha entre usuários, se não for o último
         printf("\n");
    }

    // Liberar memória e fechar arquivos
    liberarFila(fila);
    free(pai);
    free(visitado);
    liberarGrafo(grafo);
    fclose(fpPessoa);
    fclose(fpSegue);
    fclose(fpIndice);
}
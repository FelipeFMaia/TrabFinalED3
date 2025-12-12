#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "func.h"
#include "utilidades.h"
#include "grafos.h"

/**
 * Funcionalidade 13: Caminho mais curto (BFS no Grafo Transposto)
 * 1. Monta o Grafo Transposto (direções invertidas).
 * 2. Roda BFS saindo da Celebridade.
 * 3. A BFS preenche um vetor 'pai'. Se pai[u] existe, tem caminho de u para celebridade.
 * 4. Imprime os caminhos ativos (dataFim == NULO).
 */
void func13() {
    char nameFilePessoa[MAX_STRING_TAMANHO];
    char nameFileIndice[MAX_STRING_TAMANHO];
    char nameFileSegue[MAX_STRING_TAMANHO];
    char nomeCelebridade[MAX_STRING_TAMANHO];

    scanf(" %s %s %s", nameFilePessoa, nameFileIndice, nameFileSegue);
    scan_quote_string(nomeCelebridade); // Ler nome da celebridade com aspas

    // tenta abrir os arquivos de parametro binarios para leitura
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

    CabecalhoPessoa headerPessoa;
    lerCabecalhoPessoa(fpPessoa, &headerPessoa);
    if (headerPessoa.status == '0') {   //caso o arquivo pessoa esteja inconsistente, aborta funcionalidade
         printf("Falha na execução da funcionalidade.\n");
         fclose(fpPessoa); fclose(fpSegue); fclose(fpIndice); return;
    }

    // criar grafo
    Grafo *grafo = criarGrafo(headerPessoa.qtdPessoas);

    carregarVerticesDoArquivo(fpPessoa, grafo); // vertices são as pessoas do arquivo pessoas

    CabecalhoSegue headerSegue;
    lerCabecalhoSegue(fpSegue, &headerSegue);
    if (headerSegue.status == '0') {    //caso o arquivo segue esteja inconsistente, aborta funcionalidade
         printf("Falha na execução da funcionalidade.\n");
         liberarGrafo(grafo); fclose(fpPessoa); fclose(fpSegue); fclose(fpIndice); return;
    }

    // carregar arestas no modo TRANSPOSTO (1)
    // BFS saindo do destino no grafo transposto encontra todos as origens
    carregarArestasDoArquivo(fpSegue, grafo, 1);

    // Encontrar índice da celebridade
    int indiceCeleb = -1; //flag pra encontrar o indice
    for (int i = 0; i < grafo->numVertices; i++) {
        if (strcmp(grafo->listaVertices[i].nomeUsuario, nomeCelebridade) == 0) {
            indiceCeleb = i; //salva o indice
            break;
        }
    }

    if (indiceCeleb == -1) {
        printf("Celebridade inexistente.\n"); // tratamento pra caso tenha alguma falha
        liberarGrafo(grafo); fclose(fpPessoa); fclose(fpSegue); fclose(fpIndice); return;
    }

    // BFS
    int *pai = (int*)malloc(grafo->numVertices * sizeof(int)); // vetor com alocação dinâmica com elementos inteiros em qtd de vértices
    int *visitado = (int*)malloc(grafo->numVertices * sizeof(int)); // outro vetor com alocação dinâmica com elementos inteiros em qtd de vértices
    for(int i=0; i<grafo->numVertices; i++) {   // inicializa o  vetor dinâmico com -1 (pai) e 0 (visitado)
        pai[i] = -1;
        visitado[i] = 0;
    }

    Fila *fila = criarFila(grafo->numVertices); //cria fila do tamanho de vertices do grafo
    enfileirar(fila, indiceCeleb);  // add item: indiceCeleb
    visitado[indiceCeleb] = 1;      
    pai[indiceCeleb] = indiceCeleb; // Pai dele mesmo pra marcar raiz

    while(!filaVazia(fila)) {
        int u = desenfileirar(fila);
        
        // Percorrer vizinhos (que estão ordenados alfabeticamente)
        Aresta *a = grafo->listaVertices[u].inicioLista;
        while(a != NULL) {
            // para validar as pessoas que ainda seguem, a data fim deve ser nulo --> $
            if (a->dataFim[0] == '$') { 
                // precisamos achar o índice do vizinho no vetor
                // o vizinho 'a' tem o idDestino. Mas precisamos do índice dele no vetor
                // como carregamos o grafo, podemos usar buscaBinariaPorIdNoGrafo (ou sequencial otimizada)
                
                // no grafo transposto, 'a->idDestino' é a pessoa que segue 'u' (no original)
                int v = buscaBinariaPorIdNoGrafo(grafo, a->idDestino);
                
                if (v != -1 && visitado[v] == 0) {
                    visitado[v] = 1;
                    pai[v] = u; // O pai de v é u (quem descobriu v)
                    enfileirar(fila, v);
                }
            }
            a = a->prox;
        }
    }

    // exibir todas as pessoas, menos a celebridade
    for (int i = 0; i < grafo->numVertices; i++) {
        if (i == indiceCeleb) continue; // pula o indice da celebridade

        if (pai[i] == -1) {
            // não tem caminho
            printf("NAO SEGUE A CELEBRIDADE\n");
        } else {
            // reconstruir caminho: i -> pai[i] -> pai[pai[i]] ... -> Celebridade
            int atual = i;
            while (atual != indiceCeleb) {
                int proximo = pai[atual];
                
                // precisamos recuperar os dados da aresta (atual -> proximo) no grafo original
                // no grafo transposto, temos a aresta (proximo -> atual)
                // vamos procurar o atual na lista de adjacência de proximo
                
                Aresta *arestaTransposta = grafo->listaVertices[proximo].inicioLista;
                while (arestaTransposta != NULL) {
                    if (arestaTransposta->idDestino == grafo->listaVertices[atual].idPessoa) {
                        // achamos a aresta correspondente que queriamos

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

    liberarFila(fila);
    free(pai);
    free(visitado);
    liberarGrafo(grafo);
    fclose(fpPessoa);
    fclose(fpSegue);
    fclose(fpIndice);
}
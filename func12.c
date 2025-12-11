#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "func.h"
#include "utilidades.h"
#include "grafos.h"

/**
 * Funcionalidade 12: Grafo Transposto
 */
void func12() {
    char nameFilePessoa[MAX_STRING_TAMANHO];
    char nameFileIndice[MAX_STRING_TAMANHO];
    char nameFileSegue[MAX_STRING_TAMANHO];

    scanf(" %s %s %s", nameFilePessoa, nameFileIndice, nameFileSegue);

    FILE *fpPessoa = fopen(nameFilePessoa, "rb");
    FILE *fpSegue = fopen(nameFileSegue, "rb");

    if (verificaArquivo(fpPessoa) == 0) return;
    
    FILE *fpIndice = fopen(nameFileIndice, "rb");
    if (verificaArquivo(fpIndice) == 0) {
        fclose(fpPessoa);
        return;
    }
    fclose(fpIndice);

    if (verificaArquivo(fpSegue) == 0) {
        fclose(fpPessoa);
        return;
    }

    CabecalhoPessoa headerPessoa;
    lerCabecalhoPessoa(fpPessoa, &headerPessoa);
    if (headerPessoa.status == '0') {
         printf("Falha na execução da funcionalidade.\n");
         fclose(fpPessoa); fclose(fpSegue); return;
    }

    Grafo *grafo = criarGrafo(headerPessoa.qtdPessoas);
    
    carregarVerticesDoArquivo(fpPessoa, grafo);

    CabecalhoSegue headerSegue;
    lerCabecalhoSegue(fpSegue, &headerSegue);
    if (headerSegue.status == '0') {
         printf("Falha na execução da funcionalidade.\n");
         liberarGrafo(grafo); fclose(fpPessoa); fclose(fpSegue); return;
    }

    // Carregar arestas com MODO TRANSPOSTO = 1
    carregarArestasDoArquivo(fpSegue, grafo, 1);

    for (int i = 0; i < grafo->numVertices; i++) {
        Vertice *v = &grafo->listaVertices[i];
        Aresta *a = v->inicioLista;
        
        int imprimiuAlgum = 0;
        
        while (a != NULL) {
            printf("%s, %s, %s, %s, ", 
                   v->nomeUsuario, 
                   a->nomeDestino, 
                   a->dataInicio, 
                   (a->dataFim[0] == '$') ? "NULO" : a->dataFim);

            if (a->grauAmizade == '$') {
                printf("NULO\n");
            } else {
                printf("%c\n", a->grauAmizade);
            }
            
            a = a->prox;
            imprimiuAlgum = 1;
        }
        
        if (imprimiuAlgum) {
             printf("\n");
        }
    }

    liberarGrafo(grafo);
    fclose(fpPessoa);
    fclose(fpSegue);
}
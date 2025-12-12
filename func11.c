#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "func.h"
#include "utilidades.h"
#include "grafos.h"

/**
 * Funcionalidade 11: criação e exibição do grafo de quem segue quem
 */
void func11() {
    char nameFilePessoa[MAX_STRING_TAMANHO];
    char nameFileIndice[MAX_STRING_TAMANHO];
    char nameFileSegue[MAX_STRING_TAMANHO];

    scanf(" %s %s %s", nameFilePessoa, nameFileIndice, nameFileSegue);

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

    CabecalhoPessoa headerPessoa;
    lerCabecalhoPessoa(fpPessoa, &headerPessoa);
    if (headerPessoa.status == '0') {   //caso o arquivo pessoa esteja inconsistente, aborta funcionalidade
         printf("Falha na execução da funcionalidade.\n");
         fclose(fpPessoa); fclose(fpSegue); fclose(fpIndice); return;
    }

    // criar grafo
    Grafo *grafo = criarGrafo(headerPessoa.qtdPessoas);
    
    // carregar vértices
    carregarVerticesDoArquivo(fpPessoa, grafo); // vertices são as pessoas do arquivo pessoas

    CabecalhoSegue headerSegue;
    lerCabecalhoSegue(fpSegue, &headerSegue);
    if (headerSegue.status == '0') {    //caso o arquivo segue esteja inconsistente, aborta funcionalidade
         printf("Falha na execução da funcionalidade.\n");
         liberarGrafo(grafo); fclose(fpPessoa); fclose(fpSegue); fclose(fpIndice); return;
    }

    // carregar arestas (modo normal = 0)
    carregarArestasDoArquivo(fpSegue, grafo, 0);    // arestas são as relações de quem segue quem

    // exibir o grafo de lista de adjacências
    for (int i = 0; i < grafo->numVertices; i++) {
        Vertice *v = &grafo->listaVertices[i];
        Aresta *a = v->inicioLista;
        
        int imprimiuAlgum = 0; // flag pra saber quando pular linha
        
        while (a != NULL) {
            
            printf("%s, %s, %s, ", 
                   v->nomeUsuario, 
                   a->nomeDestino, 
                   a->dataInicio);

            // tratamento para o char de data final
            if (a->dataFim[0] == '$') {
                printf("NULO, ");
            } else {
                printf("%s, ", a->dataFim);
            }   
            // tratamento para o char do grauAmizade
            if (a->grauAmizade == '$') {
                printf("NULO\n");
            } else {
                printf("%c\n", a->grauAmizade);
            }
            
            a = a->prox;
            imprimiuAlgum = 1;
        }
        
        // pular linha entre vértices se houve impressão
        if (imprimiuAlgum) {
             printf("\n");
        }
    }

    liberarGrafo(grafo);
    fclose(fpPessoa);
    fclose(fpSegue);
    fclose(fpIndice);
}
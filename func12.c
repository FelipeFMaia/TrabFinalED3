#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "func.h"
#include "utilidades.h"
#include "grafos.h"

/**
 * Funcionalidade 12: grafo transposto da funcionalidade 11
 */
void func12() {
    char nameFilePessoa[MAX_STRING_TAMANHO];
    char nameFileIndice[MAX_STRING_TAMANHO];
    char nameFileSegue[MAX_STRING_TAMANHO];

    // Pegar do terminal os nomes dos arquivos
    scanf(" %s %s %s", nameFilePessoa, nameFileIndice, nameFileSegue);

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

    // Criar o grafo
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

    // Carregar arestas no modo TRANSPOSTO (parâmetro 1 na função abaixo)
    carregarArestasDoArquivo(fpSegue, grafo, 1);    // Arestas são as relações de quem segue quem

    // Exibir o grafo transposto
    for (int i = 0; i < grafo->numVertices; i++) {
        Vertice *v = &grafo->listaVertices[i];
        Aresta *a = v->inicioLista;
        
        int imprimiuAlgum = 0;  // flag pra saber quando pular linha
        
        // percorre a lista de adjacência do vértice e imprime cada aresta (origem, destino, datas e grau)
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

    // Liberar memória e fechar arquivos
    liberarGrafo(grafo);
    fclose(fpPessoa);
    fclose(fpSegue);
    fclose(fpIndice);
}
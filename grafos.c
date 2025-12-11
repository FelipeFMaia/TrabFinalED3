#include <stdio.h>
#include <stdlib.h> 
#include <string.h>  
#include <ctype.h>
#include "grafos.h"

// ----------------- Funções de Manipulação de Grafos -----------------

// Cria a estrutura inicial do grafo alocando o vetor de vértices
Grafo* criarGrafo(int numVertices) {
    if (numVertices < 0) return NULL;
    
    Grafo *grafo = (Grafo*)malloc(sizeof(Grafo));
    if (grafo == NULL) return NULL;

    grafo->numVertices = numVertices;
    grafo->listaVertices = (Vertice*)malloc(numVertices * sizeof(Vertice));
    
    if (grafo->listaVertices == NULL) {
        free(grafo);
        return NULL;
    }

    // Inicializar listas como NULL para segurança
    for (int i = 0; i < numVertices; i++) {
        grafo->listaVertices[i].inicioLista = NULL;
    }

    return grafo;
}

// Libera toda a memória do grafo (listas encadeadas e vetor principal)
void liberarGrafo(Grafo *grafo) {
    if (grafo == NULL) return;

    for (int i = 0; i < grafo->numVertices; i++) {
        Aresta *atual = grafo->listaVertices[i].inicioLista;
        while (atual != NULL) {
            Aresta *temp = atual;
            atual = atual->prox;
            free(temp);
        }
    }
    free(grafo->listaVertices);
    free(grafo);
}

// Comparador para o qsort dos vértices (pelo nomeUsuario)
int compararVerticesPorNomeUsuario(const void *a, const void *b) {
    Vertice *v1 = (Vertice*)a;
    Vertice *v2 = (Vertice*)b;
    return strcmp(v1->nomeUsuario, v2->nomeUsuario);
}

// Inserir aresta na lista encadeada mantendo a ordem alfabética do nomeDestino
void inserirArestaOrdenada(Aresta **cabeca, Aresta *novaAresta) {
    Aresta *atual;

    // Caso 1: Inserção no início (lista vazia ou novo é menor que o primeiro)
    if (*cabeca == NULL || strcmp((*cabeca)->nomeDestino, novaAresta->nomeDestino) >= 0) {
        novaAresta->prox = *cabeca;
        *cabeca = novaAresta;
    } else {
        // Caso 2: Percorrer para achar a posição
        atual = *cabeca;
        while (atual->prox != NULL && strcmp(atual->prox->nomeDestino, novaAresta->nomeDestino) < 0) {
            atual = atual->prox;
        }
        novaAresta->prox = atual->prox;
        atual->prox = novaAresta;
    }
}

// Busca Sequencial para achar o ÍNDICE no vetor do grafo dado um idPessoa.
// (O arquivo segue tem IDs, mas nosso vetor está ordenado por NOME, então não dá pra fazer busca binária simples por ID no vetor principal)
int buscaBinariaPorIdNoGrafo(Grafo *grafo, int idProcurado) {
    // Nota: Como o grafo não está ordenado por ID, a busca é sequencial O(N).
    // Para datasets muito grandes, criaríamos um índice auxiliar, mas para este trabalho isso é suficiente.
    for(int i = 0; i < grafo->numVertices; i++) {
        if (grafo->listaVertices[i].idPessoa == idProcurado) {
            return i;
        }
    }
    return -1; 
}

// Carrega vértices e ordena por nomeUsuario
void carregarVerticesDoArquivo(FILE *fpPessoa, Grafo *grafo) {
    RegistroPessoa reg;
    int i = 0;
    
    fseek(fpPessoa, TAMANHO_CABECALHO_PESSOA, SEEK_SET);

    while (lerRegistroPessoa(fpPessoa, &reg) != 0) {
        if (reg.removido == '0') {
            grafo->listaVertices[i].idPessoa = reg.idPessoa;
            strcpy(grafo->listaVertices[i].nomePessoa, reg.nomePessoa);
            strcpy(grafo->listaVertices[i].nomeUsuario, reg.nomeUsuario);
            grafo->listaVertices[i].inicioLista = NULL;
            i++;
        }
    }
    
    // Atualiza número real de vértices (caso haja removidos que não entraram)
    grafo->numVertices = i;

    // Ordenação fundamental para a saída correta
    qsort(grafo->listaVertices, grafo->numVertices, sizeof(Vertice), compararVerticesPorNomeUsuario);
}

// Carrega arestas. Se modoTransposto == 1, inverte origem e destino.
void carregarArestasDoArquivo(FILE *fpSegue, Grafo *grafo, int modoTransposto) {
    RegistroSegue reg;
    fseek(fpSegue, TAMANHO_CABECALHO_SEGUE, SEEK_SET);

    // Como o arquivo segueOrdenado tem registros de tamanho fixo, podemos usar fread direto
    // Mas precisamos pular os bytes se ele estiver removido.
    // Sua função lerRegistroSegue já lê tudo. Vamos usá-la.
    
    // Nota: lerRegistroSegue lê removido + dados.
    while (fread(&reg.removido, sizeof(char), 1, fpSegue) == 1) {
        fread(&reg.idPessoaQueSegue, sizeof(int), 1, fpSegue);
        fread(&reg.idPessoaQueESeguida, sizeof(int), 1, fpSegue);
        fread(reg.dataInicioQueSegue, sizeof(char), 10, fpSegue);
        fread(reg.dataFimQueSegue, sizeof(char), 10, fpSegue);
        fread(&reg.grauAmizade, sizeof(char), 1, fpSegue);

        if (reg.removido == '1') continue;

        int idOrigem, idDestino;

        if (modoTransposto == 0) {
            idOrigem = reg.idPessoaQueSegue;
            idDestino = reg.idPessoaQueESeguida;
        } else {
            // Inversão para a Funcionalidade 12
            idOrigem = reg.idPessoaQueESeguida; 
            idDestino = reg.idPessoaQueSegue;
        }

        // Achar índices no vetor (mapeamento ID -> Índice)
        int indexOrigem = buscaBinariaPorIdNoGrafo(grafo, idOrigem);
        int indexDestino = buscaBinariaPorIdNoGrafo(grafo, idDestino);

        if (indexOrigem != -1 && indexDestino != -1) {
            Aresta *nova = (Aresta*)malloc(sizeof(Aresta));
            nova->idDestino = idDestino;
            // Copia o nome do destino para a aresta (facilita o print e ordenação da lista)
            strcpy(nova->nomeDestino, grafo->listaVertices[indexDestino].nomeUsuario);
            
            nova->grauAmizade = reg.grauAmizade;
            
            // Tratamento de strings de data
            strncpy(nova->dataInicio, reg.dataInicioQueSegue, 10); nova->dataInicio[10] = '\0';
            strncpy(nova->dataFim, reg.dataFimQueSegue, 10); nova->dataFim[10] = '\0';
            
            inserirArestaOrdenada(&(grafo->listaVertices[indexOrigem].inicioLista), nova);
        }
    }
}

// ----------------- Funções de Fila (para BFS) -----------------

Fila* criarFila(int capacidade) {
    Fila *f = (Fila*)malloc(sizeof(Fila));
    f->capacidade = capacidade;
    f->itens = (int*)malloc(capacidade * sizeof(int));
    f->inicio = 0;
    f->fim = -1;
    f->tamanho = 0;
    return f;
}

void liberarFila(Fila *f) {
    if (f != NULL) {
        free(f->itens);
        free(f);
    }
}

void enfileirar(Fila *f, int item) {
    if (f->tamanho == f->capacidade) return;
    f->fim = (f->fim + 1) % f->capacidade;
    f->itens[f->fim] = item;
    f->tamanho++;
}

int desenfileirar(Fila *f) {
    if (f->tamanho == 0) return -1;
    int item = f->itens[f->inicio];
    f->inicio = (f->inicio + 1) % f->capacidade;
    f->tamanho--;
    return item;
}

int filaVazia(Fila *f) {
    return (f->tamanho == 0);
}
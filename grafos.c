#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 
#include "grafos.h"

// ----------------- Funções Auxiliares Locais -----------------

// Função auxiliar para comparar datas (DD/MM/AAAA)
// Retorna < 0 se dataA < dataB, 0 se igual, > 0 se dataA > dataB
// Trata nulos ('$') conforme especificação: valor não nulo vem antes do nulo
int compararDatasGrafos(const char *dataA, const char *dataB) {
    // Trata os casos nulos (identificados por '$')
    if (dataA[0] == '$') {  
        if (dataB[0] == '$') return 0; // Ambos nulos
        return 1; // A é nulo, B não (A > B, pois nulo fica no fim/depois)
    }
    if (dataB[0] == '$') return -1; // B é nulo, A não (A < B)

    // Ambos não são nulos: comparar Ano (pos 6), Mês (pos 3), Dia (pos 0)
    int cmp = strncmp(dataA + 6, dataB + 6, 4); // Ano
    if (cmp != 0) return cmp;

    cmp = strncmp(dataA + 3, dataB + 3, 2); // Mês
    if (cmp != 0) return cmp;

    return strncmp(dataA, dataB, 2); // Dia
}

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
// Em caso de empate no nome, usa a dataInicio como critério de desempate
void inserirArestaOrdenada(Aresta **cabeca, Aresta *novaAresta) {
    Aresta *atual;
    
    // Verifica se deve inserir na cabeça da lista
    // Critério: Lista vazia OU Novo < Cabeça
    // Novo < Cabeça se: (NomeNovo < NomeCabeca) OU (NomeNovo == NomeCabeca E DataNovo < DataCabeca)
    int cmpNomeHead = 0;
    int deveInserirNoInicio = 0;

    if (*cabeca == NULL) {
        deveInserirNoInicio = 1;
    } else {
        cmpNomeHead = strcmp(novaAresta->nomeDestino, (*cabeca)->nomeDestino);
        if (cmpNomeHead < 0) {
            deveInserirNoInicio = 1;
        } else if (cmpNomeHead == 0) {
            // Empate no nome, verificar data
            if (compararDatasGrafos(novaAresta->dataInicio, (*cabeca)->dataInicio) < 0) {
                deveInserirNoInicio = 1;
            }
        }
    }

    if (deveInserirNoInicio) {
        novaAresta->prox = *cabeca;
        *cabeca = novaAresta;
    } else {
        // Percorrer para achar a posição
        atual = *cabeca;
        while (atual->prox != NULL) {
            int cmpNomeProx = strcmp(atual->prox->nomeDestino, novaAresta->nomeDestino);
            
            // Se o próximo tem nome maior, paramos (o novo entra antes dele)
            if (cmpNomeProx > 0) {
                break;
            }
            // Se o próximo tem nome igual, verificamos a data
            if (cmpNomeProx == 0) {
                // Se a data do próximo for maior que a do novo, o novo entra antes dele
                if (compararDatasGrafos(atual->prox->dataInicio, novaAresta->dataInicio) > 0) {
                    break;
                }
            }
            // Caso contrário (nome menor ou nome igual com data menor/igual), continuamos
            atual = atual->prox;
        }
        novaAresta->prox = atual->prox;
        atual->prox = novaAresta;
    }
}

// Busca Sequencial para achar o índice no vetor do grafo dado um idPessoa
int buscaIndicePorIdNoGrafo(Grafo *grafo, int idProcurado) {
    // Loop para a busca
    for(int i = 0; i < grafo->numVertices; i++) {
        if (grafo->listaVertices[i].idPessoa == idProcurado) {
            return i;
        }
    }
    // Se não encontrado, retornar -1
    return -1; 
}

// Carrega vértices e ordena por nomeUsuario
void carregarVerticesDoArquivo(FILE *fpPessoa, Grafo *grafo) {
    RegistroPessoa registro;
    int i = 0;
    // pular cabeçalho
    fseek(fpPessoa, TAMANHO_CABECALHO_PESSOA, SEEK_SET);
    // loop para ler registro do arquivo Pessoa e salvar num vértice do grafo 
    while (lerRegistroPessoa(fpPessoa, &registro) != 0) {
        if (registro.removido == '0') {
            grafo->listaVertices[i].idPessoa = registro.idPessoa;
            strcpy(grafo->listaVertices[i].nomePessoa, registro.nomePessoa);
            strcpy(grafo->listaVertices[i].nomeUsuario, registro.nomeUsuario);
            grafo->listaVertices[i].inicioLista = NULL;
            i++;
        }
    }
    
    // Atualiza número real de vértices (caso haja removidos que não entraram)
    grafo->numVertices = i;

    // Ordenação pelo nomeUsuario
    qsort(grafo->listaVertices, grafo->numVertices, sizeof(Vertice), compararVerticesPorNomeUsuario);
}

// Carrega as arestas do arquivo segue.bin para o Grafo
// Se modoTransposto==1, inverte origem e destino
void carregarArestasDoArquivo(FILE *fpSegue, Grafo *grafo, int modoTransposto) {
    RegistroSegue reg;
    CabecalhoSegue header;

    // Ler cabeçalho para obter quantidade de registros
    fseek(fpSegue, 0, SEEK_SET);
    lerCabecalhoSegue(fpSegue, &header);

    // Loop para ler a quantidade de registros
    for (int i = 0; i < header.quantidadePessoas; i++) {
        lerRegistroSegue(fpSegue, &reg);

        // Se removido, pular
        if (reg.removido == '1') {
            continue;
        }
        
        int idOrigem, idDestino;

        // Definir origem e destino conforme o modo (Normal ou Transposto)
        if (modoTransposto == 0) {
            idOrigem = reg.idPessoaQueSegue;
            idDestino = reg.idPessoaQueESeguida;
        } else {
            idOrigem = reg.idPessoaQueESeguida;
            idDestino = reg.idPessoaQueSegue;
        }

        // Buscar indices no grafo
        int indexOrigem = buscaIndicePorIdNoGrafo(grafo, idOrigem);
        int indexDestino = buscaIndicePorIdNoGrafo(grafo, idDestino);

        // Se ambos existirem, adicionar aresta
        if (indexOrigem != -1 && indexDestino != -1) {
            Aresta *nova = (Aresta*)malloc(sizeof(Aresta));
            nova->idDestino = idDestino;
            strcpy(nova->nomeDestino, grafo->listaVertices[indexDestino].nomeUsuario);
            nova->grauAmizade = reg.grauAmizade;
            
            strncpy(nova->dataInicio, reg.dataInicioQueSegue, 10);
            nova->dataInicio[10] = '\0';
            
            strncpy(nova->dataFim, reg.dataFimQueSegue, 10);
            nova->dataFim[10] = '\0';
            
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
    // Se a fila estiver vazia
    if (f->tamanho == 0) {
        return -1;
    }
    // Se a fila não estiver vazia, tirar o primeio da fila (desenfileirar)
    int item = f->itens[f->inicio];
    f->inicio = (f->inicio + 1) % f->capacidade;
    f->tamanho--;
    return item;
}

int filaVazia(Fila *f) {
    return (f->tamanho == 0);
}
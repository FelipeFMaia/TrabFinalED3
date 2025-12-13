#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "indice.h"

void lerCabecalhoIndice(FILE *fpIndice, CabecalhoIndice *headerIndice) {
	fread(&headerIndice->status, sizeof(char), 1, fpIndice);
    fread(headerIndice->lixo, sizeof(char), 11, fpIndice);
}

void escreverCabecalhoIndice(FILE *fpIndice, CabecalhoIndice headerIndice) {
    // Garantir escrever no início do arquivo
    fseek(fpIndice, 0, SEEK_SET);
    // Escrever cabecalho índice
    fwrite(&headerIndice.status, sizeof(headerIndice.status), 1, fpIndice);
    fwrite(headerIndice.lixo, sizeof(headerIndice.lixo), 1, fpIndice);
}

void lerRegistroIndice(FILE *fpIndice, RegistroIndice *registroIndice) {
	fread(&registroIndice->id, sizeof(int), 1, fpIndice);
	fread(&registroIndice->byteOffSet, sizeof(long long), 1, fpIndice);
}

void escreveRegistroIndice(FILE *fpIndice, RegistroIndice novoRegistro) {
    fwrite(&novoRegistro.id, sizeof(int), 1, fpIndice);
	fwrite(&novoRegistro.byteOffSet, sizeof(long long), 1, fpIndice);
}

// Para o QuickSort
int compararIndices(const void *a, const void *b) {
    RegistroIndice *regA = (RegistroIndice *)a;
    RegistroIndice *regB = (RegistroIndice *)b;
    return (regA->id - regB->id);
}

int compararIndicesPeloId(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

// Função que vai inserir os novos registros e ordenar o arquivo de índice
void atualizarEOrdenarArquivoIndice (FILE *fpIndice, char *nameFileIndice, RegistroIndice *novosRegistros, int numRegistrosNovos) {
	// Descobrir o número de registros antigos
    fseek(fpIndice, 0, SEEK_END);
    long long tamanhoBytesArquivo = ftell(fpIndice);
    int numRegistrosAntigos = (tamanhoBytesArquivo - TAMANHO_CABECALHO_INDICE) / TAMANHO_REGISTRO_INDICE;

	// Número total de registros após inserção
    int numeroRegistros = numRegistrosAntigos + numRegistrosNovos;

    // Alocar um vetor de ponteiros da struct de registro de índice
    // para fazer o sorting na memória e depois a escrita
    RegistroIndice *registrosIndiceMemoria = (RegistroIndice*)malloc(numeroRegistros * sizeof(RegistroIndice));

    // Ler o arquivo de índice na memória (registros antigos)
    fseek(fpIndice, TAMANHO_CABECALHO_INDICE, SEEK_SET); // pular cabecalho
    for (int i = 0; i < numRegistrosAntigos; i++) { // Ler todos os registros antigos
        lerRegistroIndice(fpIndice, &registrosIndiceMemoria[i]);
    }

    // Adicionar os novos registros no vetor de índices
    for (int i = 0; i < numRegistrosNovos; i++) {
        registrosIndiceMemoria[i + numRegistrosAntigos] = novosRegistros[i];
    }

    // Ordenar o vetor usando QuickSort
    qsort(registrosIndiceMemoria, numeroRegistros, sizeof(RegistroIndice), compararIndices);

	// Reabrir o arquivo em "wb" e reescrever tudo
    fclose(fpIndice);
    fpIndice = fopen(nameFileIndice, "wb");
    if (verificaArquivo(fpIndice) == 0) {
        return;
    } 

    // Escreve o cabeçalho do arquivo de índice já consistente para fechar
    CabecalhoIndice headerIndice = {'1', "$$$$$$$$$$$"};
    fwrite(&headerIndice.status, sizeof(char), 1, fpIndice);
    fwrite(headerIndice.lixo, sizeof(char), 11, fpIndice);

    // Escrever o vetor ordenado no arquivo (já aberto para escrita e leitura)
    fseek(fpIndice, TAMANHO_CABECALHO_INDICE, SEEK_SET); // Pular cabeçalho
    for (int i = 0; i < numeroRegistros; i++) {
        escreveRegistroIndice(fpIndice, registrosIndiceMemoria[i]);
    }

    // Liberar memória
    free(registrosIndiceMemoria);

	// fechar o arquivo e reabrir em "r+b" para o resto do programa
    fclose(fpIndice);
    fpIndice = fopen(nameFileIndice, "r+b");
    if (verificaArquivo(fpIndice) == 0) {
		return; // abortar
	}

}

// Função auxiliar para verificar (binariamente) se um ID existe em um vetor
int idEstaNaLista(int idBuscado, int *listaIds, int tamanhoLista) {
	int inicio = 0, meio, fim = tamanhoLista - 1;
	int encontrou = 0;

    while (inicio <= fim) {
        // Calcula o índice do registro do meio
        meio = inicio + (fim - inicio) / 2;

        // Comparar se é o id buscado
        if (listaIds[meio] == idBuscado) {
            encontrou = 1;
            break; // Encontrou. Sai do loop
        }
        else if (listaIds[meio] < idBuscado) {
            // O ID que lemos é menor, então o que buscamos está na metade direita
            inicio = meio + 1;
        }
        else { // listaIds[meio] > idBuscado
            // O ID que lemos é maior, então o que buscamos está na metade esquerda
            fim = meio - 1;
        }
    }

	if (encontrou == 0) {
		return 0;
	}
	else {
		return 1;
	}

}

// Função auxiliar para a func5, para atualizar o arquivo de índice após remoção
void reescreverIndiceComRemocoes(FILE *fpIndice, char *nomeArquivoIndice, CabecalhoIndice headerIndice, int *idsParaRemover, int numRemovidos) {
    // Se não houver remoções, não fazer nada
    if (numRemovidos == 0) {
		return; // Abortar
	}

    // Descobrir o número de registros antigos
    fseek(fpIndice, 0, SEEK_END);
    long long tamanhoBytesArquivo = ftell(fpIndice);
    int numRegistrosAntigos = (tamanhoBytesArquivo - TAMANHO_CABECALHO_INDICE) / TAMANHO_REGISTRO_INDICE;

    // Alocar vetor para ler os registros antigos
    RegistroIndice *vetorAntigo = (RegistroIndice*)malloc(numRegistrosAntigos * sizeof(RegistroIndice));
    fseek(fpIndice, TAMANHO_CABECALHO_INDICE, SEEK_SET);
    for (int i = 0; i < numRegistrosAntigos; i++) {
        lerRegistroIndice(fpIndice, &vetorAntigo[i]);
    }

    // Alocar vetor para os novos registros
    RegistroIndice *vetorNovo = (RegistroIndice*)malloc(numRegistrosAntigos * sizeof(RegistroIndice));
    int contagemVetorNovo = 0;

    // ordenar para busca binária
    qsort(idsParaRemover, numRemovidos, sizeof(int), compararIndicesPeloId);

    // Fazer um filtro, ou seja, copiar do antigo para o novo se não tiver na lista de remoção
    for (int i = 0; i < numRegistrosAntigos; i++) {
        // Usando busca binária para isso
        if (idEstaNaLista(vetorAntigo[i].id, idsParaRemover, numRemovidos) == 0) {
            // ID não está na lista de remoção, então fazer a cópia
            vetorNovo[contagemVetorNovo] = vetorAntigo[i];
            contagemVetorNovo++;
        }
    }

    // Reabrir o arquivo no modo "wb" e reescrever tudo
    fclose(fpIndice); // Fecha o ponteiro antigo
    fpIndice = fopen(nomeArquivoIndice, "wb"); // Cria um novo ponteiro
    if (verificaArquivo(fpIndice) == 0) {
        // Libera memória antes de sair em caso de erro
        free(vetorAntigo);
        free(vetorNovo);
        return; // Retorna para indicar falha
    }

    // Atualizar consistência
    headerIndice.status = '1';

	// Cabeçalho para o novo arquivo de índice, já consistente
    escreverCabecalhoIndice(fpIndice, headerIndice);

    // Escreve os registros (já estão ordenados)
    for (int i = 0; i < contagemVetorNovo; i++) {
        escreveRegistroIndice(fpIndice, vetorNovo[i]);
    }

	// liberar memória
    free(vetorAntigo);
    free(vetorNovo);
    
    // Fechar e reabrir o arquivo de índica no modo "r+b"
    fclose(fpIndice);
    fpIndice = fopen(nomeArquivoIndice, "r+b"); // Cria o ponteiro final
    if (verificaArquivo(fpIndice) == 0) {
        return;// Retorna para indicar falha
    } 

    return;
}

// Função auxiliar para a func7, para atualizar o arquivo de índica após as modificações
void reescreverIndiceComAtualizacoes(FILE *fpIndice, char *nomeArquivoIndice, 
                                     int *idsParaRemover, int numRemovidos, 
                                     RegistroIndice* registrosParaAdicionar, int numAdicionados) {
	// se não houver modificações, não fazer nada
    if (numRemovidos == 0 && numAdicionados == 0) {
		return; // abortar
	}

    // Ler o número de registos antes das modificações
    fseek(fpIndice, 0, SEEK_END);
    long long tamanhoBytesArquivo = ftell(fpIndice);
    int numRegistrosAntigos = (tamanhoBytesArquivo - TAMANHO_CABECALHO_INDICE) / TAMANHO_REGISTRO_INDICE;

	// Alocar um vetor de RegistroIndice para os registros de índice antes da modificação
    RegistroIndice *vetorAntigo = (RegistroIndice*)malloc(numRegistrosAntigos * sizeof(RegistroIndice));
    fseek(fpIndice, TAMANHO_CABECALHO_INDICE, SEEK_SET);
    for (int i = 0; i < numRegistrosAntigos; i++) {
        lerRegistroIndice(fpIndice, &vetorAntigo[i]);
    }

    // Alocar um vetor de RegistroIndice para os registros após as modificações
    int numRegistrosNovo = numRegistrosAntigos - numRemovidos + numAdicionados;
    RegistroIndice *vetorNovo = (RegistroIndice*)malloc(numRegistrosNovo * sizeof(RegistroIndice));
    int contagemVetorNovo = 0;

    // ordenar para busca binária
    qsort(idsParaRemover, numRemovidos, sizeof(int), compararIndicesPeloId);

    // Fazer um filtro, copiando do antigo para o novo se não tiver removido
    for (int i = 0; i < numRegistrosAntigos; i++) {
        if (idEstaNaLista(vetorAntigo[i].id, idsParaRemover, numRemovidos) == 0) {
            vetorNovo[contagemVetorNovo] = vetorAntigo[i];
            contagemVetorNovo++;
        }
    }

    // Adicionar os novos registros no final do vetor filtrado
    for (int i = 0; i < numAdicionados; i++) {
        vetorNovo[contagemVetorNovo] = registrosParaAdicionar[i];
        contagemVetorNovo++;
    }

    // Ordenar o vetor combinado (filtrado + adicionados), usando QuickSort
    qsort(vetorNovo, contagemVetorNovo, sizeof(RegistroIndice), compararIndices);

    // Reabrir o arquivo no modo "wb" e reescrever tudo
    fclose(fpIndice);
    fpIndice = fopen(nomeArquivoIndice, "wb");
    if (verificaArquivo(fpIndice) == 0) { // verificar arquivo
        return;
    } 

	// Cabeçaho para o novo arquivo de índice
    CabecalhoIndice headerIndice = {'0', "$$$$$$$$$$$"};
    fwrite(&headerIndice.status, sizeof(char), 1, fpIndice);
    fwrite(headerIndice.lixo, sizeof(char), 11, fpIndice);
    
    // Escreve os registros ordenados
    for (int i = 0; i < contagemVetorNovo; i++) {
        escreveRegistroIndice(fpIndice, vetorNovo[i]);
    }

    free(vetorAntigo);
    free(vetorNovo);
    
    // Fechar e reabrir em "r+b"
    fclose(fpIndice);
    fpIndice = fopen(nomeArquivoIndice, "r+b");
    if (verificaArquivo(fpIndice) == 0) {
        return;
    } 
}

//Carrega todos os registros do arquivo de índice para um vetor na RAM.
//Retorna o ponteiro para o vetor alocado e atualiza numRegistros.
//Devolve o numero de registro no arquivo de índice pela variavel numRegistros
RegistroIndice* carregarIndiceRAM(FILE *fpIndice, int *numRegistros) {
    // Descobrir o número de registros
    fseek(fpIndice, 0, SEEK_END);
    long long tamanhoBytesArquivo = ftell(fpIndice);
    *numRegistros = (tamanhoBytesArquivo - TAMANHO_CABECALHO_INDICE) / TAMANHO_REGISTRO_INDICE;

    // Se não há registros, retorna NULL
    if (*numRegistros <= 0) {
        return NULL;
    }

    // Alocar o vetor na RAM
    RegistroIndice *vetorRAM = (RegistroIndice*)malloc(*numRegistros * sizeof(RegistroIndice));
    if (vetorRAM == NULL) {
        printf("Falha ao alocar memória para o índice.\n");
        *numRegistros = 0; // Garante que o número de registros é 0 em caso de falha
        return NULL;
    }

    fseek(fpIndice, TAMANHO_CABECALHO_INDICE, SEEK_SET); // Pula cabeçalho
    // Ler os registros de índice e guardar na memoria RAM
    for (int i = 0; i < *numRegistros; i++) {
        lerRegistroIndice(fpIndice, &vetorRAM[i]);
    }
    
    return vetorRAM;
}

//Função que busca (binária) o ID procurado no VETOR DE ÍNDICE (RAM).
//retorna -1 se não encontrado 
//retorna o byteOffset do registro no arquivo Pessoa
long long buscaBinariaIndiceRAM(RegistroIndice *indiceEmRAM, int numRegistros, int idBuscado) {
    
    // Se o vetor é nulo ou não há registros, cancela a operação
    if (indiceEmRAM == NULL || numRegistros == 0) {
        return -1;
    }

    // incialização de variáveis para a busca binária
	int inicio = 0;
	int fim = numRegistros - 1;
	int meio;
    
	// loop da busca binária na memoria RAM
    while (inicio <= fim) {
        meio = inicio + (fim - inicio) / 2;

        // verifica se é maior, menor ou igual que o idBuscado
        if (indiceEmRAM[meio].id == idBuscado) {
            return indiceEmRAM[meio].byteOffSet; // Encontrou. Retorna o offset.
        }
        else if (indiceEmRAM[meio].id < idBuscado) {
            // o id lido é menor, então o que buscamos está na metade direita
            inicio = meio + 1;
        }
        else { // indiceEmRAM[meio].id > idBuscado
            // o id lido é maior, então o que buscamos está na metade esquerda
            fim = meio - 1;
        }
    }

    return -1; // ID não encontrado
}
#define MAX_STRING_TAMANHO 100
#define CABECALHO_INDICE_TAMANHO 12
#define CABECALHO_PESSOA_TAMANHO 17

#include <stdio.h>
#include <stdlib.h>
#include "utilidades.h"
#include "func.h"

/**
 * Funcionalidade 5: buscas e remoção de registros
 * Esta função lê o nome de um arquivo de dados de pessoas e um arquivo de índice.
 * Em seguida, ela executa 'n' operações de remoção, onde cada remoção é especificada
 * por um campo (ex: "idPessoa") e um valor.
 * A estratégia de busca muda com base no campo:
 * 1.  Se o campo for "idPessoa", a busca e remoção é OTIMIZADA, usando o arquivo de índice
 * para encontrar rapidamente a posição do registro no arquivo de dados e fazendo a remoção.
 * 2.  Para qualquer outro campo ("nomePessoa", "idadePessoa", etc.), a busca é SEQUENCIAL,
 * lendo o arquivo de dados registro por registro do início ao fim e fazendo a remoção quando corresponder.
 * O código foi elaborado para que dentro de um loop fosse feita a verificação de critério e busca de pessoas,
 * depois dentro desse loop haverá novamente outro loop para as devidas remoções.
 */

void func5 () {
    
    int n; // Número de buscas que serão realizadas
    char nameFileIndice[MAX_STRING_TAMANHO], nameFilePessoa[MAX_STRING_TAMANHO];
    
    // Lê os nomes dos arquivos de dados e de índice
    scanf(" %s %s", nameFilePessoa, nameFileIndice);
    // Lê a quantidade de remocões que o usuário quer fazer
    scanf("%d", &n);

    FILE *fpIndice, *fpPessoa;

    // Tenta abrir os dois arquivos em modo de leitura binária e escrita
    fpPessoa = fopen(nameFilePessoa, "r+b");
    fpIndice = fopen(nameFileIndice, "r+b");

    // Verificação de segurança
    if (verificaArquivo(fpIndice) == 0) {
        return; // Aborta funcionalidade
    }
    if (verificaArquivo(fpPessoa) == 0) {
        fclose(fpIndice);
        return; // Aborta funcionalidade
    }


    // Lendo o cabeçalho do arquivo de ÍNDICE
    CabecalhoIndice headerIndice;
    lerCabecalhoIndice(fpIndice, &headerIndice);

    // Lendo o cabeçalho do arquivo pessoa
    CabecalhoPessoa headerPessoa;
    lerCabecalhoPessoa(fpPessoa, &headerPessoa);

    // Verificando a consistência dos arquivos
    int consistencia = 1;
    consistencia = verificaConsistenciaArquivo(fpIndice, 1);
     if(consistencia == 0) {
         return;
     }
     consistencia = verificaConsistenciaArquivo(fpPessoa, 2);
     if(consistencia == 0) {
        return;
    }

    // Carregar o arquivo de índice inteiro para a RAM
    int numRegIndice;  // variavel que seŕa inclusa na funcao e retornará com o valor correto de registros indice
    RegistroIndice *indiceEmRAM = carregarIndiceRAM(fpIndice, &numRegIndice);
    if (indiceEmRAM == NULL && numRegIndice > 0) { // falha na alocação de memória
        fclose(fpPessoa);
        fclose(fpIndice);
        return;
    }

    // modificar a consistência dos arquivos, pois estão abertos para escrita
    // inconsistente
    headerIndice.status = '0';
    atualizarConsistencia(fpIndice, headerIndice.status);
    // inconsistente
    headerPessoa.status = '0';
    atualizarConsistencia(fpPessoa, headerPessoa.status);


    // alocando vetores auxiliares
    int maxRegistros = headerPessoa.qtdPessoas + headerPessoa.qtdRemovidos; // Tamanho seguro
    
    // Aloca vetor para guardar os offsets encontrados pela busca
    long long *offsetsEncontrados = (long long*)malloc(maxRegistros * sizeof(long long));
    // Aloca vetor para guardar os IDs que precisam ser removidos do índice
    int *idsParaRemoverDoIndice = (int*)malloc(maxRegistros * sizeof(int));
    int totalRemovidosDoIndice = 0;

    // Este loop vai rodar 'n' vezes, uma para cada busca solicitada
    for(int i = 0; i < n; i++) {
        int temp; // Variável temporária
        
        scanf("%d", &temp); // Consumir do terminal o número da linha
        
        char nomeCampo[MAX_STRING_TAMANHO];
        char valorCampo[MAX_STRING_TAMANHO];

        lerCriteriosDeBusca(nomeCampo, valorCampo);

        // Número de registro encontrados de acordo com os critérios de busca
        // Os offsets dos registros encontrados são armazenados no vetor offsetsEncontrados 
        int numEncontrados = buscarPessoas(fpPessoa, indiceEmRAM, numRegIndice, &headerPessoa, 
                                           nomeCampo, valorCampo, offsetsEncontrados);

        // Loop para deletar todos os registros encontrados de acordo com os critérios de busca
        if (numEncontrados > 0) {
            for (int j = 0; j < numEncontrados; j++) {
                long long offsetAtual = offsetsEncontrados[j];
                
                // Ler o id antes de remover
                int idParaRemover;
                fseek(fpPessoa, offsetAtual + 1 + 4, SEEK_SET); // Pular removido e tamanho
                fread(&idParaRemover, sizeof(int), 1, fpPessoa); // Ler o idPessoa do registro a ser removido

                // Adicionar o id ao vetor de ids para remover no arquivo de índice
                idsParaRemoverDoIndice[totalRemovidosDoIndice] = idParaRemover;
                totalRemovidosDoIndice++;

                // Fazer a remoção lógica no arquivo de Pessoas
                fseek(fpPessoa, offsetAtual, SEEK_SET);
                removerRegistroPessoa(fpPessoa);

                // Atualizar o cabeçalho em memória
                headerPessoa.qtdPessoas--;
                headerPessoa.qtdRemovidos++;        
            }
        }
    } 

    // Atualizar o arquivo de índice (sem os removidos)
    reescreverIndiceComRemocoes(fpIndice, nameFileIndice, headerIndice, idsParaRemoverDoIndice, totalRemovidosDoIndice);

    // adicionar verificação de segurança
    if (fpIndice == NULL) {
        printf("Falha no processamento do arquivo.\n");
        headerPessoa.status = '1';
        atualizarConsistencia(fpPessoa, headerPessoa.status);
        fclose(fpPessoa); // Fechar o outro arquivo
        free(offsetsEncontrados);
        free(idsParaRemoverDoIndice);
        
        return; // Abortar
    }

    // Atualizaro o cabeçalho do arquivo de Pessoas no disco
    escreveCabecalhoPessoa(fpPessoa, headerPessoa);

    // modificar a consistência dos arquivos, pois vamos fechar
    // consistente
    headerIndice.status = '1';
    atualizarConsistencia(fpIndice, headerIndice.status);
    // consistente
    headerPessoa.status = '1';
    atualizarConsistencia(fpPessoa, headerPessoa.status);

    free(indiceEmRAM);
    free(offsetsEncontrados);
    free(idsParaRemoverDoIndice);
    fclose(fpIndice);
    fclose(fpPessoa);
    binarioNaTela(nameFilePessoa);
    binarioNaTela(nameFileIndice);
}


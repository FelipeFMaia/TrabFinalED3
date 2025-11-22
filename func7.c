#define MAX_STRING_TAMANHO 100
#define CABECALHO_INDICE_TAMANHO 12
#define CABECALHO_PESSOA_TAMANHO 17

#include <stdio.h>
#include <stdlib.h>
#include "utilidades.h"
#include "func.h"


/**
 * Funcionalidade 7:
    Lê dois campos e seus valores associados do terminal, por linha.
    O primeiro campo e o primeiro valor indicam o registro a ser procurado.
    O segundo campo e o segundo valor indicam o que vai ser atualizado no registro encontrado.
    Decisão:
        1) Se o valorCampo couber naquele campo daquele registro, então atualizar.
        2) Se não couber, então remover logicamente o registro e buscar o primeiro registro 
removido logicamente que caiba e não achar nenhum, inserir no final.
    Fazer isso para todas as atualizações requisitadas no terminal 

    Como fizemos:
    Optamos por pegar todas as atualizacoes e armazenar em um vetor para que dentro do loop principal, a qual servirá para
    todas as atualizações, sejam feitas as devidas incrementações de registro seja no caso em que caiba a atualização,
    seja no caso em que devemos procurar um novo registro com tamanho maior.
 */
void func7 () {
    int numAtualizacoes; // Número de atualizacoes que serão realizadas
    char nameFileIndice[MAX_STRING_TAMANHO], nameFilePessoa[MAX_STRING_TAMANHO];
    
    // Lê os nomes dos arquivos de dados e de índice
    scanf(" %s %s", nameFilePessoa, nameFileIndice);
    // Lê a quantidade de remocões que o usuário quer fazer
    scanf("%d", &numAtualizacoes);

    FILE *fpIndice, *fpPessoa;

    // Tenta abrir os dois arquivos, o de índice vai ser no modo de escrita
    // e o arquivo de pessoa vai ser no modo de leitura
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


    // --- LEITURA E VERIFICAÇÃO DOS CABEÇALHOS ---

    // Lendo o cabeçalho do arquivo de ÍNDICE
    CabecalhoIndice headerIndice;
    lerCabecalhoIndice(fpIndice, &headerIndice);

    // Lendo o cabeçalho do arquivo de DADOS (Pessoas)
    CabecalhoPessoa headerPessoa;
    lerCabecalhoPessoa(fpPessoa, &headerPessoa);

    // Verificando a consistência dos arquivos
     int consistencia;
     consistencia = verificaConsistenciaArquivo(fpIndice, 1);
     if(consistencia == 0) {
         return;
     }
     consistencia = verificaConsistenciaArquivo(fpPessoa, 2);
     if(consistencia == 0) {
         return;
     }

    // Carregar o arquivo de índice inteiro para a RAM
    int numRegIndice;
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

    // Tamanho seguro para fazer alocação do vetor
    int maxRegistros = headerPessoa.qtdPessoas + numAtualizacoes; // Tamanho seguro
    
    // Alocar vetor de long long para armazenar os offsets encontrados para fazer a modificação/inserção 
    long long *offsetsEncontrados = (long long*)malloc(maxRegistros * sizeof(long long));
    // Alocar vetor de int para os ids que forem removidos ao longo das atualizações (tamanho insuficiente)
    int *idsParaRemoverDoIndice = (int*)malloc(maxRegistros * sizeof(int));
    // Alocar vetor de RegistroIndice para armazenar os registros a serem adicionados (pela modificação ou inserção)
    RegistroIndice *registrosParaAdicionarNoIndice = (RegistroIndice*)malloc(maxRegistros * sizeof(RegistroIndice));

    // Variaveis para o numero de indices adicionados ou removidos
    int numAdicionados = 0;
    int numRemovidos = 0;

    // _________ Loop Principal ________

    // Este loop vai rodar 'numAtualizacoes' vezes, uma para cada busca solicitada
    for(int i = 0; i < numAtualizacoes; i++) {
        int temp; // Variável temporária
        
        scanf("%d", &temp); // Consumir do terminal o número da linha
        
        // Instanciar variveis para ler do terminal os critérios
        char campoBusca[MAX_STRING_TAMANHO], valorBusca[MAX_STRING_TAMANHO];
        char campoAtualiza[MAX_STRING_TAMANHO], valorAtualiza[MAX_STRING_TAMANHO]; 

        // Ler do terminal 
        lerCriteriosDeAtualizacao(campoBusca, valorBusca, campoAtualiza, valorAtualiza);

        // Número de registro encontrados de acordo com os critérios de busca
        // Os offsets dos registros encontrados são armazenados no vetor offsetsEncontrados 
        int numEncontrados = buscarPessoas(fpPessoa, indiceEmRAM, numRegIndice, &headerPessoa, 
                                           campoBusca, valorBusca, offsetsEncontrados);

        // Loop para modificar todos os registros encontrados
        for (int j = 0; j < numEncontrados; j++) {
            long long offsetAtual = offsetsEncontrados[j];
            
            // Instanciar registro de Pessoa e salvar o registro do offsetAtual para as comparações
            RegistroPessoa pessoa;
            fseek(fpPessoa, offsetAtual, SEEK_SET);
            lerRegistroPessoa(fpPessoa, &pessoa);

            int idAntigo = pessoa.idPessoa;
            int tamanhoAntigo = pessoa.tamanhoRegistro;

            // Aplica a atualização (na memória)
            aplicarAtualizacao(&pessoa, campoAtualiza, valorAtualiza);

            // Pegar o tamanho do registro atualizado
            int tamanhoNovo = tamanhoUtilRegistro(pessoa);

            // --- Escrita ---
            
            // CASO 1: Cabe no registro antigo
            if (tamanhoNovo <= tamanhoAntigo) {
                // escrever o registro atualizado no registro antigo
                fseek(fpPessoa, offsetAtual, SEEK_SET);
                escreverRegistroPessoa(fpPessoa, pessoa, tamanhoAntigo);

                // Se o ID mudou,atualizar o índice
                if (pessoa.idPessoa != idAntigo) {
                    // adicionar no vetor para remoção futura no arquivo de índice
                    idsParaRemoverDoIndice[numRemovidos] = idAntigo;
                    numRemovidos++;
                    // adicionar no vetor para adição futura no arquivo de índice
                    registrosParaAdicionarNoIndice[numAdicionados].id = pessoa.idPessoa;
                    registrosParaAdicionarNoIndice[numAdicionados].byteOffSet = offsetAtual;
                    numAdicionados++;
                }
            }
            // CASO 2: Não cabe, remover o antigo e inserir um novo no final do arquivo de pessoas
            else {
                // Remover logicamente o antigo
                fseek(fpPessoa, offsetAtual, SEEK_SET);
                removerRegistroPessoa(fpPessoa);
                headerPessoa.qtdPessoas--; // inserirPessoa vai incrementar logo abaixo
                headerPessoa.qtdRemovidos++;
                
                // Adicionar o ID antigo para remoção do índice
                idsParaRemoverDoIndice[numRemovidos] = idAntigo;
                numRemovidos++;

                // Inserir o registro atualizado
                long long novoByteOffset = inserirPessoa(fpPessoa, pessoa, &headerPessoa);
                
                // Adicionar o novo registro para inserção no índice
                registrosParaAdicionarNoIndice[numAdicionados].id = pessoa.idPessoa;
                registrosParaAdicionarNoIndice[numAdicionados].byteOffSet = novoByteOffset;
                numAdicionados++;
            } 
        }
    }

    // Atualizar o arquivo de índice com todas as remoções e adições (armazenadas nos vetores auxiliares)
    reescreverIndiceComAtualizacoes(fpIndice, nameFileIndice, 
                                    idsParaRemoverDoIndice, numRemovidos, 
                                    registrosParaAdicionarNoIndice, numAdicionados);

    // Atualiza o cabeçalho do arquivo de pessoas
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
    free(registrosParaAdicionarNoIndice);
    fclose(fpIndice);
    fclose(fpPessoa);
    binarioNaTela(nameFilePessoa);
    binarioNaTela(nameFileIndice);

}

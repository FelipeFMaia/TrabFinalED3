#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "func.h"
#include "geral.h"
#include "pessoa.h"
#include "indice.h"

/**
 * Funcionalidade 4: buscas em arquivos de dados e de índice.
 * Esta função lê o nome de um arquivo de dados de pessoas e um arquivo de índice.
 * Em seguida, ela executa 'n' operações de busca, onde cada busca é especificada
 * por um campo (ex: "idPessoa") e um valor.
 * A estratégia de busca muda com base no campo:
 * 1.  Se o campo for "idPessoa", a busca é OTIMIZADA, usando o arquivo de índice
 * para encontrar rapidamente a posição do registro no arquivo de dados.
 * 2.  Para qualquer outro campo ("nomePessoa", "idadePessoa", etc.), a busca é SEQUENCIAL,
 * lendo o arquivo de dados registro por registro do início ao fim.
 * * (NOTA: Tivemos que atualizar ela pra usar o índice em RAM, 
 * senão não compilava mais com a 'buscarPessoas' nova que usa RAM kkkkkk)
 */
void func4 () {
    // --- PREPARAÇÃO E LEITURA INICIAL ---
    int n; // Número de buscas que serão realizadas
    char nameFileIndice[MAX_STRING_TAMANHO], nameFilePessoa[MAX_STRING_TAMANHO];
    
    // Ler do terminal os nomes do arquivo e o número de buscas a serem realizadas
    scanf(" %s %s", nameFilePessoa, nameFileIndice);
    scanf("%d", &n);

    // Abre os arquivos para leitura binária ("rb")
    FILE *fpPessoa = fopen(nameFilePessoa, "rb");
    FILE *fpIndice = fopen(nameFileIndice, "rb");

    // Verificar os arquivos
    if (verificaArquivo(fpIndice) == 0) {
        return;
    } 
    if (verificaArquivo(fpPessoa) == 0) {
        fclose(fpIndice);
        return;
    }

    // Leitura e verificação dos cabeçalhos
    // (Isso aqui continua igualzinho ao original)

    // Lendo o cabeçalho do arquivo de índice
    CabecalhoIndice headerIndice;
    lerCabecalhoIndice(fpIndice, &headerIndice);
    // Lendo o cabeçalho do arquivo de pessoas
    CabecalhoPessoa headerPessoa;
    lerCabecalhoPessoa(fpPessoa, &headerPessoa);

    // Verificando a consistência dos arquivos
    if (verificaConsistenciaArquivo(fpIndice, 1) == 0) {
        return;
    }
    if (verificaConsistenciaArquivo(fpPessoa, 2) == 0) {
        return;
    }

    // --- INÍCIO DA MUDANÇA NECESSÁRIA ---
    // Tivemos que adicionar isso aqui pra func4 funcionar com a 
    // nova buscarPessoas (que agora pede o índice em RAM)
    
    int numRegIndice; // Quantidade de registros no índice
    RegistroIndice *indiceEmRAM = carregarIndiceRAM(fpIndice, &numRegIndice);
    
    // Verificar se a alocação do índice em RAM deu certo
    if (indiceEmRAM == NULL && numRegIndice > 0) { // falha na alocação de memória
        printf("Falha ao alocar memória para o índice.\n");
        fclose(fpPessoa);
        fclose(fpIndice);
        return;
    }
    // --- FIM DA MUDANÇA NECESSÁRIA ---


    // Alocando vetor de offsets
    // Alocando um vetor que será utilizado em todas as 'n' buscas
    // O tamanho máximo de resultados de uma busca sequencial é qtdPessoas
    long long *offsetsEncontrados = (long long*)malloc(headerPessoa.qtdPessoas * sizeof(long long));
    if (offsetsEncontrados == NULL && headerPessoa.qtdPessoas > 0) {
        printf("Falha ao alocar memória para busca.\n");
        
        // Opa, se alocamos o indiceEmRAM, temos que liberar ele aqui também
        free(indiceEmRAM); // <-- Tivemos que adicionar isso
        
        fclose(fpIndice);
        fclose(fpPessoa);
        return;
    }

    // --- LOOP PRINCIPAL DE BUSCAS ---
    for(int i = 0; i < n; i++) {
        int temp; 
        scanf("%d", &temp); // Consumir do terminal o número da linha
        
        char nomeCampo[MAX_STRING_TAMANHO];
        char valorCampo[MAX_STRING_TAMANHO];
        lerCriteriosDeBusca(nomeCampo, valorCampo);
        
        int numEncontrados;

        // --- CHAMADA DA FUNÇÃO DE BUSCA CENTRALIZADA ---
        // Agora a chamada tem 7 argumentos, passando o índice em RAM
        // (essa era a causa do erro de compilação)
        numEncontrados = buscarPessoas(fpPessoa, indiceEmRAM, numRegIndice, &headerPessoa, 
                                       nomeCampo, valorCampo, offsetsEncontrados);

        // --- PROCESSAMENTO DOS RESULTADOS ---
        if (numEncontrados == 0) {
            printf("Registro inexistente.\n\n");
        } 
        else {
            // Itera sobre os offsets encontrados, lê e imprime cada registro
            for (int j = 0; j < numEncontrados; j++) {
                // Posiciona o ponteiro no offset encontrado
                fseek(fpPessoa, offsetsEncontrados[j], SEEK_SET);
                
                RegistroPessoa pessoa;
                
                // Aqui usamos a nova lerRegistroPessoa que retorna 0 (EOF) 
                // ou >0 (bytes lidos)
                int bytesLidos = lerRegistroPessoa(fpPessoa, &pessoa); 

                // Se a leitura deu certo E o registro não tá removido
                if (bytesLidos > 0 && pessoa.removido == '0') {
                    imprimePessoa(pessoa); // Imprime formatado
                }
            }
        }
    }

    // --- FINALIZAÇÃO ---
    
    // Temos que liberar a memória que alocamos lá em cima
    free(indiceEmRAM); // <-- Tivemos que adicionar isso

    free(offsetsEncontrados); // Libera o vetor de offsets

    fclose(fpIndice);
    fclose(fpPessoa);
}
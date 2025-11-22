#include "func.h"
#include "utilidades.h"
#include <stdio.h>

/**
 * Funcionalidade 3: Recupera e exibe todos os registros de um arquivo de dados.
 * A função abre o arquivo de pessoas, verifica sua integridade e percorre
 * os registros, imprimindo na tela todos que não estão logicamente removidos.
 */
void func3() {
    // Leitura do nome do arquivo de entrada
    char nameFilePessoa[MAX_STRING_TAMANHO];
    scanf(" %s", nameFilePessoa);

    // Abre o arquivo de dados para leitura binária
    FILE *fpPessoa = fopen(nameFilePessoa, "rb");

    // Validação da abertura do arquivo
    if (verificaArquivo(fpPessoa) == 0) {
        return; 
    }

    // Leitura do cabeçalho do arquivo de dados
    CabecalhoPessoa headerPessoa;
    lerCabecalhoPessoa(fpPessoa, &headerPessoa);

    // Verificação da consistência do arquivo (status)
    if (headerPessoa.status == '0') {
        printf("Falha no processamento do arquivo.\n");
        fclose(fpPessoa);
        return;
    }

    // Caso não existam registros no arquivo
    if (headerPessoa.qtdPessoas == 0) {
        printf("Registro inexistente.\n");
        fclose(fpPessoa);
        return;
    }
    
    // Loop para percorrer os registros de dados
    while (ftell(fpPessoa) < headerPessoa.proxByteOffSet) {
        RegistroPessoa pessoa;

        // Lê um registro completo, já tratando o caso de registros removidos
        int removido = lerRegistroPessoa(fpPessoa, &pessoa);

        // Se o registro lido não estiver marcado como removido, exibe seus dados
        if (removido == 0) {
            imprimePessoa(pessoa);
        }
    }

    // Fechamento do arquivo
    fclose(fpPessoa);
}
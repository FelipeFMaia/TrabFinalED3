#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "func.h"
#include "geral.h"
#include "segue.h"

/*
Funcionalidade 9
Ordenar o arquivo
Nesse código usamos funções auxiliares como compareInts e compareDatas para uma função geral funcaoDeComparacao.
No fim, buscamos usar a  função qsort da biblioteca do c stdlib.h (qsort), usando essa função geral que criamos.
*/

// Função auxiliar para comparar inteiros (trata nulos) --> usada na função de comparação
int compareInts(int a, int b) {
    // Trata os casos nulos primeiro
    if (a == -1) {          // A é nulo
        if (b == -1) 
            return 0;       // A e B são nulos
        else 
            return 1;       // A é nulo e B não
    }

    if (b == -1)          // B é nulo e A não
        return -1;          // se A nao é nulo, ele vem antes
    
    //ambos valores não nulos --> subtração
    return a - b;   // Retorna < 0 se a < b, 0 se a == b, > 0 se a > b
}

// Função auxiliar para comparar datas (trata nulos e formato) --> usada na função de comparação
int compareDatas(const char *dataA, const char *dataB) {
    // Trata os casos nulos (identificados por '$')
    if (dataA[0] == '$') {  // data A é nula
        
        if (dataB[0] == '$') {
            return 0;       // data B também é nula, são consideradas iguais
        }else 
            return 1;       // A é nulo, B não 
    }

    if (dataB[0] == '$')    // Data B é nula
        return -1;          // A é menor

    // Ambos não são nulos --> comparar ano, depois mês, depois dia
    
    int comparacaoAno = strncmp(dataA + 6, dataB + 6, 4);  // comparar ano --> pula 6 char para analisar o ano e compara os 4 chars de ano
    if (comparacaoAno != 0) return comparacaoAno;

    // Comparar Mês (pos 3, 2 chars)
    int comparacaoMes = strncmp(dataA + 3, dataB + 3, 2);  //  comparar mes -->  pula 3 chars para analisar o mes e comparar os 2 chars de mes
    if (comparacaoMes != 0) return comparacaoMes;

    // Comparar Dia (pos 0, 2 chars)
    return strncmp(dataA, dataB, 2);                // comparar dia --> começa no inicio da string e analisa os 2 chars de dia
}


int funcaoDeComparacao(const void *a, const void *b) {
    RegistroSegue *regA = (RegistroSegue *)a;
    RegistroSegue *regB = (RegistroSegue *)b;

    // chamar os comparadores auxiliares na ordem de prioridade 
    // comparacao pode ser -1, 0 e 1 --> -1 regA é menor, 0 são iguais, 1 regB é menor
    
    // Critério 1: idPessoaQueSegue
    int comparacao = compareInts(regA->idPessoaQueSegue, regB->idPessoaQueSegue);
    if (comparacao != 0) return comparacao;       //se der zero, segue pro próximo critério de comparação

    // Critério 2: idPessoaQueESeguida (desempate)
    comparacao = compareInts(regA->idPessoaQueESeguida, regB->idPessoaQueESeguida);
    if (comparacao != 0) return comparacao;       //se der zero, segue pro próximo critério de comparação

    // Critério 3: dataInicioQueSegue (desempate)
    comparacao = compareDatas(regA->dataInicioQueSegue, regB->dataInicioQueSegue);
    if (comparacao != 0) return comparacao;       //se der zero, segue pro próximo critério de comparação

    // Critério 4: dataFimQueSegue (desempate final)
    return compareDatas(regA->dataFimQueSegue, regB->dataFimQueSegue);
}


void func9() {
    // nomes dos arquivos
    char nameFileDesordenado[MAX_STRING_TAMANHO];
    char nameFileOrdenado[MAX_STRING_TAMANHO];

    // Ler nomes dos arquivos do terminal
    scanf(" %s %s", nameFileDesordenado, nameFileOrdenado);

    // Abrir arquivo desordenado
    FILE *fpDesordenado;
    fpDesordenado = fopen(nameFileDesordenado, "rb");
    // verificar arquivo desordenado
    if (verificaArquivo(fpDesordenado) == 0) {
        return; // aborta funcionalidade
    }
    // Verificar consistência do arquivo desordenado
    int consistencia = 1;
    consistencia = verificaConsistenciaArquivo(fpDesordenado, 3);
     if (consistencia == 0) { // se o arquivo estiver inconsistente
         fclose(fpDesordenado);
         return; // abortar funcionalidade
     }

    // Criar o arquivo ordenado
    FILE *fpOrdenado;
    // verificar arquivo ordenado
    fpOrdenado = fopen(nameFileOrdenado, "wb");
    if (verificaArquivo(fpOrdenado) == 0) {
    fclose(fpDesordenado);
    return; // aborta funcionalidade
    }

    // Ler cabeçalho do arquivo desordenado, que será o mesmo para os dois
    CabecalhoSegue headerSegue;
    lerCabecalhoSegue(fpDesordenado, &headerSegue);

    // Escrever o cabecalho no Ordenado
    escreverCabecalhoSegue(fpOrdenado, headerSegue);

    // Se o arquivo não tiver pessoa, não há nada a ser ordenado
    // Basta atualizar a consistência, fechar os arquivos e abortar a funcionalidade
    if (headerSegue.quantidadePessoas == 0) {
        consistencia = 1;
        atualizarConsistencia(fpOrdenado, consistencia);
        fclose(fpOrdenado);
        fclose(fpDesordenado);
        binarioNaTela(nameFileOrdenado);
        return;
    }
    // se tiver pessoas, continuar

    // Alocar um vetor de registros de Segue
    // para colocar o arquivo desordenado na memória e ordenar
    RegistroSegue *registros = (RegistroSegue*)malloc(headerSegue.quantidadePessoas * sizeof(RegistroSegue));
    if (registros == NULL) {
        printf("Falha no processamento do arquivo.\n");
        fclose(fpDesordenado);
        fclose(fpOrdenado);
        return;
    }

    // Ler todos os registros de Segue do arquivo desordenado e salva num vetor para ordenar
    for (int i = 0; i < headerSegue.quantidadePessoas; i++) {
        lerRegistroSegue(fpDesordenado, &registros[i]);
    }

    // Uma vez que os registros estejam na RAM, podemos fechar o arquivo Desordenado
    fclose(fpDesordenado);

    // Ordenar o vetor da memória
    qsort(registros, headerSegue.quantidadePessoas, sizeof(RegistroSegue), funcaoDeComparacao);

    // Reposicionar o cursor de fpOrdenado depois do cabecalho
    fseek(fpOrdenado, TAMANHO_CABECALHO_SEGUE, SEEK_SET);

    // Uma vez ordenado, basta escrever esse vetor ordenado de registro no arquivo Ordenado
    for (int i = 0; i < headerSegue.quantidadePessoas; i++) {
        escreverRegistroSegue(fpOrdenado, registros[i]);     
    }

    // Atualizar a consistência e fechar o arquivo Ordenado
    atualizarConsistencia(fpOrdenado, '1');
    
    fclose(fpOrdenado);
    free(registros);
    binarioNaTela(nameFileOrdenado);
}

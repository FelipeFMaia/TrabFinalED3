#include <stdio.h>
#include <stdlib.h>     //para o uso do quicksort (qsort)
#include <string.h>
#include "func.h"
#include "utilidades.h"



void func2() {
    char nome_arqCSV[50], nome_arqIndice[50], nome_arqPessoa[50];
    char buffer[100];


    RegistroPessoa RAM[1000];             //memoria em ram para a leitura do CSV
    RegistroIndice vetorIndice[1000];     //memoria em ram para a leitura do indice
    
    scanf(" %s",nome_arqCSV); //nome do arquivo csv
    FILE *fpCsv= fopen(nome_arqCSV, "r");        //cria arquivo com o nome_arq
    if (verificaArquivo(fpCsv) == 0) {
        return; // Para a execução da func2
    }
    
    scanf(" %s",nome_arqPessoa); //nome do arquivo pessoa
    FILE *fpPessoa = fopen(nome_arqPessoa, "wb");        //cria arquivo com o nome_arq
    if (verificaArquivo(fpPessoa) == 0) {
        fclose(fpCsv);
        return; // Para a execução da func2
    }
    
    scanf(" %s",nome_arqIndice); //nome do arquivo indice primario
    FILE *fpIndice = fopen(nome_arqIndice, "rb+");        //cria arquivo com o nome_arq
    if (verificaArquivo(fpIndice) == 0) {
        fclose(fpCsv);
        fclose(fpPessoa);
        return; // Para a execução da func2
    }
    
        CabecalhoIndice cabecalhoIndice;
        CabecalhoPessoa cabecalhoPessoa;
        cabecalhoPessoa.status = '0'; //inconsistente
        cabecalhoPessoa.qtdPessoas = 0;
        cabecalhoPessoa.qtdRemovidos = 0;
        cabecalhoPessoa.proxByteOffSet = 17;

        escreveCabecalhoPessoa(fpPessoa, cabecalhoPessoa);

        atualizarConsistencia(fpIndice, '0');     //torna o status do indice com inscosistente
        
        fseek(fpIndice, 0, SEEK_END);     //move o ponteiro pro fim do arquivo
        int tamanhoIndice = ftell(fpIndice);            //nos conta o tamnho do arquivo devolvendo a posicao do ponteiro manipiulado pelo fseek
        
        int numeroIndice = 0;
        if(tamanhoIndice > 12){
            numeroIndice = (tamanhoIndice - 12) / sizeof(RegistroIndice);     //pega o numero de registros presente no Indice
        }

        fseek(fpIndice, 12, SEEK_SET); // Pula o cabeçalho e vai para o início dos dados

        if(numeroIndice>0){
            fread(vetorIndice, sizeof(RegistroIndice), numeroIndice, fpIndice);    //le o arquivo de indice ja existente e salva num vetor
        }
    
        fgets(buffer, 100, fpCsv);   //le a primeira linha do csv que nao sera copiada (cabecalho)

        char *tok;
        int i = 0, contReg = 0; //contador de registros do arquivo CSV
        char *Temp;

        while (fgets(buffer, 100, fpCsv) != NULL){      //le o CSV tudo na memoria RAM
            buffer[strcspn(buffer, "\r\n")] = '\0';      //remove o /n adicionado no fgets
            Temp = buffer;

            tok = strsep(&Temp, ",");       //le o id da linha pega pelo fgets
            if(tok != ""){
                RAM[i].idPessoa = atoi(tok);
            }else
                RAM[i].idPessoa = -1;

            tok = strsep(&Temp, ",");       //le o nomePessoa da linha pega pelo fgets
            if(tok != ""){
                strncpy(RAM[i].nomePessoa, tok, sizeof(RAM[i].nomePessoa));
            }else
                strncpy(RAM[i].nomePessoa, "", sizeof(RAM[i].nomePessoa));

            tok = strsep(&Temp, ",");       //le a idade da linha pega pelo fgets
            if(tok != ""){
                RAM[i].idadePessoa = atoi(tok);
            }else
                RAM[i].idadePessoa = 0;

            tok = strsep(&Temp, ",");       //le o nomeUsuario da linha pega pelo fgets
            if(tok != ""){
                strncpy(RAM[i].nomeUsuario, tok, sizeof(RAM[i].nomeUsuario));
            }else 
                strncpy(RAM[i].nomeUsuario, "", sizeof(RAM[i].nomeUsuario));

            i++;            //proximo indice da RAM
            contReg++;      //conta o numero de registros do csv que serao passados ao arquivo pessoa
        }

        int tamNomePessoa, tamNomeUsuario, tamRegistro;
        int j = numeroIndice;
        for(i = 0; i<contReg; i++){         //passa da memoria RAM para o arquivo, campo por campo
            tamNomePessoa = strlen(RAM[i].nomePessoa);
            tamNomeUsuario = strlen(RAM[i].nomeUsuario);
            tamRegistro = (4*sizeof(int) + tamNomePessoa + tamNomeUsuario);// tamanho do registro sera de 4 inteiros e os tamanho das strings variaveis
            
            vetorIndice[j].byteOffSet = ftell(fpPessoa);             //salva info do byteoffset na ram para indice
            
            char removido = '0';
            fwrite(&removido,sizeof(char),1,fpPessoa);                                //campo do logicamente removido
            fwrite(&tamRegistro,sizeof(int),1,fpPessoa);                              //campo do tamanho de registro
            fwrite(&RAM[i].idPessoa,sizeof(int),1,fpPessoa);                          //campo de id
            
            vetorIndice[j].id = RAM[i].idPessoa;                                       //salva a informacao de id na ram para indice
            j++;

            if(RAM[i].idadePessoa == 0){
                RAM[i].idadePessoa = -1;
            }                       //troca o valor nulo por -1
            fwrite(&RAM[i].idadePessoa,sizeof(int),1,fpPessoa);                   //campo da idade
        
            fwrite(&tamNomePessoa, sizeof(int), 1, fpPessoa);                        //campo tamNomePessoa
            if (tamNomePessoa > 0) {
                fwrite(RAM[i].nomePessoa, sizeof(char), tamNomePessoa, fpPessoa);     //escreve o nome se o tamanho nao for zero
            }                                                                          //o tamanho escrito e igual ao strlen(nome)

            fwrite(&tamNomeUsuario, sizeof(int), 1, fpPessoa);                        //campo tam NomeUsuario
            if (tamNomeUsuario > 0) {
            fwrite(RAM[i].nomeUsuario, sizeof(char), tamNomeUsuario, fpPessoa);       //escreve o nome se o tamanho nao for zero
            }                                                                          //o tamanho escrito e igual ao strlen(nome)
            }

        numeroIndice = j;       //numeroIndice atualizado pois houve insercoes no vetorIndice

        cabecalhoPessoa.status = '1';                           //status = consistente
        cabecalhoPessoa.qtdPessoas = contReg;                   //qtdP = contador de registros
        cabecalhoPessoa.qtdRemovidos = 0;                       //continua sem removidos
        cabecalhoPessoa.proxByteOffSet = ftell(fpPessoa);      //proxbyteoffset = proximo byte livre no arquivo

        escreveCabecalhoPessoa(fpPessoa, cabecalhoPessoa);

        qsort(vetorIndice, numeroIndice, sizeof(RegistroIndice), compararIndices); //ordena de forma crescente os indices do vetor 
        
        fseek(fpIndice,12,SEEK_SET);        //retorna ponteiro pro inicio do arquivo indice depois do cabecalho

        for(i=0; i<numeroIndice; i++){      //adiciona de forma crescente os indices, campo a campo, no arquivo indice
            fwrite(&vetorIndice[i].id,sizeof(int),1,fpIndice); //sobrescreve de maneira ordenada
            fwrite(&vetorIndice[i].byteOffSet,sizeof(long long),1,fpIndice); //sobrescreve de maneira ordenada
        }
        
        atualizarConsistencia(fpIndice, '1'); // status do indice = consistente
        fclose(fpIndice);
        fclose(fpPessoa);
        fclose(fpCsv);

        binarioNaTela(nome_arqPessoa);
        binarioNaTela(nome_arqIndice);

        return;     //termina o programa
    }
#include <stdio.h>
#include "func.h"
#include "utilidades.h"

void func1(){
        char nome_arq[50];
        char lixo[] = {'$','$','$','$','$','$','$','$','$','$','$'};
        
        scanf(" %s", nome_arq);
        
        FILE *fpIndice = fopen(nome_arq, "wb");        //cria arquivo com o nome_arq

        verificaArquivo (fpIndice);

        CabecalhoIndice cabecalho;
        cabecalho.status = '1'; //consistente

        for(int i = 0; i<11; i++){
            cabecalho.lixo[i] = lixo[i];    //escreve o lixo "$" no  resto do cabecario
        }
       
        //escreve o cabecario no indice primario
        fwrite(&cabecalho.status,sizeof(char),1,fpIndice);
        fwrite(&cabecalho.lixo,sizeof(char),11,fpIndice);
        
        fclose(fpIndice);

        binarioNaTela(nome_arq);     //exibe o arquivo de indice primario recem criado

        return;
}

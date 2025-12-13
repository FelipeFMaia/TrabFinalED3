#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "geral.h"
#include "indice.h"
#include "pessoa.h"
#include "segue.h"

// -------------------- Funcões disponibilizadas pela disciplina --------------------
void binarioNaTela(char *nomeArquivoBinario) { 

	/* Use essa função para comparação no run.codes. Lembre-se de ter fechado (fclose) o arquivo anteriormente.
	*  Ela vai abrir de novo para leitura e depois fechar (você não vai perder pontos por isso se usar ela). */

	unsigned long i, cs;
	unsigned char *mb;
	size_t fl;
	FILE *fs;
	if(nomeArquivoBinario == NULL || !(fs = fopen(nomeArquivoBinario, "rb"))) {
		fprintf(stderr, "ERRO AO ESCREVER O BINARIO NA TELA (função binarioNaTela): não foi possível abrir o arquivo que me passou para leitura. Ele existe e você tá passando o nome certo? Você lembrou de fechar ele com fclose depois de usar?\n");
		return;
	}
	fseek(fs, 0, SEEK_END);
	fl = ftell(fs);
	fseek(fs, 0, SEEK_SET);
	mb = (unsigned char *) malloc(fl);
	fread(mb, 1, fl, fs);

	cs = 0;
	for(i = 0; i < fl; i++) {
		cs += (unsigned long) mb[i];
	}
	printf("%lf\n", (cs / (double) 100));
	free(mb);
	fclose(fs);
}

void scan_quote_string(char *str) {

	/*
	*	Use essa função para ler um campo string delimitado entre aspas (").
	*	Chame ela na hora que for ler tal campo. Por exemplo:
	*
	*	A entrada está da seguinte forma:
	*		nomeDoCampo "MARIA DA SILVA"
	*
	*	Para ler isso para as strings já alocadas str1 e str2 do seu programa, você faz:
	*		scanf("%s", str1); // Vai salvar nomeDoCampo em str1
	*		scan_quote_string(str2); // Vai salvar MARIA DA SILVA em str2 (sem as aspas)
	*
	*/

	char R;

	while((R = getchar()) != EOF && isspace(R)); // ignorar espaços, \r, \n...

	if(R == 'N' || R == 'n') { // campo NULO
		getchar(); getchar(); getchar(); // ignorar o "ULO" de NULO.
		strcpy(str, ""); // copia string vazia
	} else if(R == '\"') {
		if(scanf("%[^\"]", str) != 1) { // ler até o fechamento das aspas
			strcpy(str, "");
		}
		getchar(); // ignorar aspas fechando
	} else if(R != EOF){ // vc tá tentando ler uma string que não tá entre aspas! Fazer leitura normal %s então, pois deve ser algum inteiro ou algo assim...
		str[0] = R;
		scanf("%s", &str[1]);
	} else { // EOF
		strcpy(str, "");
	}
}


// -------------------- Funções de verificação e impressão --------------------
int verificaArquivo (FILE *arquivo) {
	if (arquivo == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return 0; // ERRO
	}
	else {
		return 1; // OK
	}
}

// Função para verificar a consistência
// arqID: 1) Índice 2) Pessoa 3) Segue
// Se consistente, retorna 1, senão retorna 0
int verificaConsistenciaArquivo(FILE *arquivo, int arqID) {
	// arqID == 1 indica que é para o arquivo de índice
	if (arqID == 1) {
		CabecalhoIndice headerIndice;
        fseek(arquivo, 0, SEEK_SET);
		lerCabecalhoIndice(arquivo, &headerIndice);
		if (headerIndice.status == '0') {
			printf("Falha no processamento do arquivo.\n");
			return 0; // retornar a inconsistência
		}
		else {
			fseek(arquivo, 0, SEEK_SET);
			return 1; // retornar a consistência
		}
	}

	// arqID == 2 indica que é para o arquivo de Pessoas
	else if (arqID == 2) {
		CabecalhoPessoa headerPessoa;
        fseek(arquivo, 0, SEEK_SET);
		lerCabecalhoPessoa(arquivo, &headerPessoa);
		if (headerPessoa.status == '0') {
			printf("Falha no processamento do arquivo.\n");
			return 0; // retornar a inconsistência
		}
		else {
			fseek(arquivo, 0, SEEK_SET);
			return 1; // retornar a consistência
		}
	}

	// arqID == 3 indica que é para o arquivo Segue
	else if (arqID == 3) {
		CabecalhoSegue headerSegue;
        fseek(arquivo, 0, SEEK_SET);
		lerCabecalhoSegue(arquivo, &headerSegue);
		if (headerSegue.status == '0') {
			printf("Falha no processamento do arquivo.\n");
			return 0; // retornar a inconsistência
		}
		else {
			fseek(arquivo, 0, SEEK_SET);
			return 1; // retornar a consistência
		}
	}

	else {
		return 0;
	}
}

void atualizarConsistencia(FILE *arquivo, char consistencia) {
	// cursor no inicio do arquivo
	fseek(arquivo, 0, SEEK_SET);
	// atualizando a consistencia
	fwrite(&consistencia, sizeof(char), 1, arquivo);
	// cursor no inicio do arquivo novamente
	fseek(arquivo, 0, SEEK_SET);
}

void preencheStringCifrao (char *string) {
    for (int i = 0; i < 10; i++) {
		string[i] = '$';
	}
}
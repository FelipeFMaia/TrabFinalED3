#include <stdio.h>
#include <stdlib.h> 
#include <string.h>  
#include <ctype.h>
#include "utilidades.h"

// O nome das funções sao bem sugestivas ao que cada função pretende realizar

//---------------------------------------------Funcões disponibilizadas pela disciplina------------------------------------------------
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

//---------------------------------------------Funções de verificação e impressão------------------------------------------------------
void imprimePessoa(RegistroPessoa pessoa) {
    printf("Dados da pessoa de codigo %d\n", pessoa.idPessoa);
    // Tratamento para campos que podem ser nulos ou vazios

    if (pessoa.tamanhoNomePessoa == 0) {
        printf("Nome: -\n");
    } else {
        printf("Nome: %s\n", pessoa.nomePessoa);
    }
    
    if (pessoa.idadePessoa == -1) {
        printf("Idade: -\n");
    } else {
        printf("Idade: %d\n", pessoa.idadePessoa);
    }

    if (pessoa.tamanhoNomeUsuario == 0) {
        printf("Usuario: -\n");
    } else {
        printf("Usuario: %s\n", pessoa.nomeUsuario);
    }

    printf("\n"); // Deixa uma linha em branco para separar as saídas
}

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

// ---------------------------------------------------Operações com o arquivo Indice--------------------------------------------------

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

// ---------------------------------------------------Operações com o arquivo Pessoa------------------------------------------------------

void lerCabecalhoPessoa(FILE *fpPessoa, CabecalhoPessoa *headerPessoa) {
    fread(&headerPessoa->status, sizeof(char), 1, fpPessoa);
    fread(&headerPessoa->qtdPessoas, sizeof(int), 1, fpPessoa);
    fread(&headerPessoa->qtdRemovidos, sizeof(int), 1, fpPessoa);
    fread(&headerPessoa->proxByteOffSet, sizeof(long long), 1, fpPessoa);
}

void escreveCabecalhoPessoa(FILE *fpPessoa, CabecalhoPessoa headerPessoa) {
    // Vai para o início do arquivo
	fseek(fpPessoa, 0, SEEK_SET);
    // Escreve
	fwrite(&headerPessoa.status, sizeof(char), 1, fpPessoa);
    fwrite(&headerPessoa.qtdPessoas, sizeof(int), 1, fpPessoa);
    fwrite(&headerPessoa.qtdRemovidos, sizeof(int), 1, fpPessoa);
    fwrite(&headerPessoa.proxByteOffSet, sizeof(long long), 1, fpPessoa);
}

int lerRegistroPessoa(FILE *fpPessoa, RegistroPessoa *pessoa) {

    // ler o campo removido. Caso não consiga ler nada, é fim de arquivo
    if (fread(&pessoa->removido, sizeof(char), 1, fpPessoa) < 1) {
        return 0; // 0 significa EOF
    }

    // ler o campo tamanhoRegistro:
    // para pular o registro, caso removido ou para retornar os bytes totais lidos
    if (fread(&pessoa->tamanhoRegistro, sizeof(int), 1, fpPessoa) < 1) {
        return 0; // 0 significa EOF
    }

    // calculando os bytes totais do registro 
    int bytesTotaisDoRegistro = sizeof(char) + sizeof(int) + pessoa->tamanhoRegistro;
    
	// Mesmo encontrando no índice, o registro pode ter sido removido logicamente
	if (pessoa->removido == '1') {
        int tamanhoRegistro = pessoa->tamanhoRegistro;
		fseek(fpPessoa, tamanhoRegistro, SEEK_CUR); // pular o registro removido
		return bytesTotaisDoRegistro; // retornar os  bytes totais lidos, evitando ftell
	}
	// caso o registro não tenha sido removido, continua a leitura
	// Se não foi removido, lemos o resto dos dados do registro
    
	fread(&pessoa->idPessoa, sizeof(int), 1, fpPessoa);
	fread(&pessoa->idadePessoa, sizeof(int), 1, fpPessoa);
    
	// Para campos de tamanho variável (strings), primeiro lemos o tamanho,
	// depois lemos a quantidade exata de bytes correspondente
	fread(&pessoa->tamanhoNomePessoa, sizeof(int), 1, fpPessoa);
	fread(pessoa->nomePessoa, sizeof(char), pessoa->tamanhoNomePessoa, fpPessoa);
	pessoa->nomePessoa[pessoa->tamanhoNomePessoa] = '\0'; // Adicionar o terminador de string
    
	fread(&pessoa->tamanhoNomeUsuario, sizeof(int), 1, fpPessoa);
	fread(pessoa->nomeUsuario, sizeof(char), pessoa->tamanhoNomeUsuario, fpPessoa);
	pessoa->nomeUsuario[pessoa->tamanhoNomeUsuario] = '\0'; // Adicionar o terminador de string
	
	// Calcular os bytes uteis lidos dos fread's
	int bytesUteisLidos = tamanhoUtilRegistro(*pessoa);
    
	// Calcular o tamanho do lixo no final do registro, caso tenha
	int tamanhoLixo = pessoa->tamanhoRegistro - bytesUteisLidos;
    
	// Pular lixo
	if (tamanhoLixo > 0) {
		fseek(fpPessoa, tamanhoLixo, SEEK_CUR);
	}
    
    // retornar o total de bytes lidos, para evitar ftell
	return bytesTotaisDoRegistro; // Não foi removido logicamente
}

// Atualiza o status de removido, fazendo a remoção lógica 
void removerRegistroPessoa(FILE* fpPessoa) {
  fwrite("1", sizeof(char), 1, fpPessoa);
}

// Função auxiliar para escrever um registro novo, já verificado o registro logicamente removido e que há espaço suficiente
// tamanhoDisponivel não contabiliza os 5 primeiros bytes (1 byte para removido e 4 bytes para tamanhoRegistro)
// essa função já preenche o fim do registro, caso haja espaço sobrando
void escreverRegistroPessoa(FILE *fpPessoa, RegistroPessoa pessoa, int tamanhoDisponivel) {
    // removido == 0, registro não removido
    fwrite("0", sizeof(char), 1, fpPessoa);
    // Tamanho do registro
    fwrite(&tamanhoDisponivel, sizeof(int), 1, fpPessoa);
    // idPessoa
    fwrite(&pessoa.idPessoa, sizeof(int), 1, fpPessoa);
    // idadePessoa
    fwrite(&pessoa.idadePessoa, sizeof(int), 1, fpPessoa);
    // tamanhoNomePessoa
    fwrite(&pessoa.tamanhoNomePessoa, sizeof(int), 1, fpPessoa);
    // nomePessoa[MAX_STRING_TAMANHO]
    fwrite(pessoa.nomePessoa, sizeof(char), pessoa.tamanhoNomePessoa, fpPessoa);
    // tamanhoNomeUsuario
    fwrite(&pessoa.tamanhoNomeUsuario, sizeof(int), 1, fpPessoa);
    // nomeUsuario[MAX_STRING_TAMANHO]
    fwrite(pessoa.nomeUsuario, sizeof(char), pessoa.tamanhoNomeUsuario, fpPessoa);

    // Preencher o restante de bytes do registro com lixo ($)
    int tamanhoNecessario = tamanhoUtilRegistro(pessoa);
    int tamanhoLixo = tamanhoDisponivel - tamanhoNecessario;
    for (int i = 0; i < tamanhoLixo; i++) {
        fwrite("$", sizeof(char), 1, fpPessoa);
    }

}

// Função responsável pela inserção no final do arquivo pessoa
long long inserirPessoa (FILE* fpPessoa, RegistroPessoa novoRegistro, CabecalhoPessoa *headerPessoa) {
    int tamanhoNecessario = tamanhoUtilRegistro(novoRegistro);
    long long posAtual = ftell(fpPessoa);

    // inserir no final
    long long byteOffset = headerPessoa->proxByteOffSet;
    fseek(fpPessoa, byteOffset, SEEK_SET);

    // Atualizar o byteOffset e mover o cursor para ele
    byteOffset = headerPessoa->proxByteOffSet;
    fseek(fpPessoa, byteOffset, SEEK_SET);

    // No fim, o tamanho disponível é exatamente o necessário
    escreverRegistroPessoa(fpPessoa, novoRegistro, tamanhoNecessario); 

    // Atualizar o cabeçalho na memória
    headerPessoa->qtdPessoas++;
    headerPessoa->proxByteOffSet += (sizeof(char) + sizeof(int) + tamanhoNecessario);

    return byteOffset;
}

// ----------------------------------------------------Operações com arquivo Segue-----------------------------------------------------

void lerCabecalhoSegue(FILE *fpSegue, CabecalhoSegue *headerSegue) {
	fread(&headerSegue->status, sizeof(char), 1, fpSegue);
    fread(&headerSegue->quantidadePessoas, sizeof(int), 1, fpSegue);
    fread(&headerSegue->proxRRN, sizeof(int), 1, fpSegue);
}

void lerRegistroSegue(FILE *fpSegue, RegistroSegue *registroSegue) {
    // Lê campo a campo, na ordem exata em que foi escrito
    fread(&registroSegue->removido, sizeof(char), 1, fpSegue);
    fread(&registroSegue->idPessoaQueSegue, sizeof(int), 1, fpSegue);
    fread(&registroSegue->idPessoaQueESeguida, sizeof(int), 1, fpSegue);
    fread(registroSegue->dataInicioQueSegue, sizeof(char), 10, fpSegue);
    fread(registroSegue->dataFimQueSegue, sizeof(char), 10, fpSegue);
    fread(&registroSegue->grauAmizade, sizeof(char), 1, fpSegue);
}

void escreverRegistroSegue(FILE *fpSegue, RegistroSegue regSegue) {
  // 
  fwrite(&regSegue.removido, sizeof(char), 1, fpSegue);
  //
  fwrite(&regSegue.idPessoaQueSegue, sizeof(int), 1, fpSegue);
  //
  fwrite(&regSegue.idPessoaQueESeguida, sizeof(int), 1, fpSegue);
  //
  fwrite(regSegue.dataInicioQueSegue, sizeof(char), 10, fpSegue);
  //
  fwrite(regSegue.dataFimQueSegue, sizeof(char), 10, fpSegue);
  //
  fwrite(&regSegue.grauAmizade, sizeof(char), 1, fpSegue);
}

void escreverCabecalhoSegue(FILE *fpSegue, CabecalhoSegue headerSegue) {
    // Vai para o início do arquivo
	fseek(fpSegue, 0, SEEK_SET);
    // Escreve
	fwrite(&headerSegue.status, sizeof(char), 1, fpSegue);
    fwrite(&headerSegue.quantidadePessoas, sizeof(int), 1, fpSegue);
    fwrite(&headerSegue.proxRRN, sizeof(int), 1, fpSegue);
}

// ---------------------------------Funções auxiliares de atualização de registro por campo e valor-----------------------------------

void lerCriteriosDeBusca (char nomeCampo[MAX_STRING_TAMANHO], char valorCampo[MAX_STRING_TAMANHO]) {
    // Ler o nome do campo a ser buscado (até encontrar o caractere '=')
    scanf(" %49[^=]%*c", nomeCampo);
    
    // Lendo o valor da busca a depender se é uma string ou não
    // Para campo de string
    if (strcmp(nomeCampo, "nomePessoa") == 0 || strcmp(nomeCampo, "nomeUsuario") == 0) {
        // Usar a funcao scan_quote_string para armazenar na string sem as aspas
        scan_quote_string(valorCampo); 
    } else {
        // Para campo numérico. Ler como string e depois usar atoi
        scanf("%s", valorCampo); 
    }
}

void lerCriteriosDeAtualizacao (char *campoBusca, char *valorBusca, char *campoAtualiza, char *valorAtualiza) {
    // Leitura de campo e valor de campo que vai ser atualizado
		// Ler o nome do campo a ser buscado (até encontrar o caractere '=')
    scanf(" %49[^=]%*c", campoBusca);
    
    // Lendo o valor do campo a ser atualizado a depender se é uma string ou não
    // Para campo de string
    if (strcmp(campoBusca, "nomePessoa") == 0 || strcmp(campoBusca, "nomeUsuario") == 0) {
        // Usar a funcao scan_quote_string para armazenar na string sem as aspas
        scan_quote_string(valorBusca); 
    } else {
        // Para campo numérico. Ler como string e depois usar atoi
        scanf("%s", valorBusca); 
    }

	// Leitura de campo e valor de campo que vai atualizar
    // Ler o nome do campo a ser buscado (até encontrar o caractere '=')
    scanf(" %49[^=]%*c", campoAtualiza);
    
    // Lendo o valor do campo que vai atualizar a depender se é uma string ou não
    // Para campo de string
    if (strcmp(campoAtualiza, "nomePessoa") == 0 || strcmp(campoAtualiza, "nomeUsuario") == 0) {
        // Usar a funcao scan_quote_string para armazenar na string sem as aspas
        scan_quote_string(valorAtualiza); 
    } else {
        // Para campo numérico. Ler como string e depois usar atoi
        scanf("%s", valorAtualiza); 
    }
}


void aplicarAtualizacao (RegistroPessoa *pessoa, char *campoAtualiza, char *valorAtualiza) {
    // atualizar o valor no campo de atualizacao
    
    // Se o campo é idPessoa
    if (strcmp(campoAtualiza, "idPessoa") == 0) {
        pessoa->idPessoa = atoi(valorAtualiza);
    
    // Se o campo é idadePessoa
    } else if (strcmp(campoAtualiza, "idadePessoa") == 0) {
        if (strcmp(valorAtualiza, "NULO") == 0) {
            pessoa->idadePessoa = -1;
        } else {
            pessoa->idadePessoa = atoi(valorAtualiza);
        }
    
    // Se o campo é nomePessoa
    } else if (strcmp(campoAtualiza, "nomePessoa") == 0) {
        if (strcmp(valorAtualiza, "NULO") == 0) {
            pessoa->tamanhoNomePessoa = 0;
            pessoa->nomePessoa[0] = '\0';
        } else {
            strcpy(pessoa->nomePessoa, valorAtualiza);
            pessoa->tamanhoNomePessoa = strlen(valorAtualiza);
        }
    
    // Se o campo é nomeUsuario
    } else if (strcmp(campoAtualiza, "nomeUsuario") == 0) {
        if (strcmp(valorAtualiza, "NULO") == 0) {
            pessoa->tamanhoNomeUsuario = 0;
            pessoa->nomeUsuario[0] = '\0';
        } else {
            strcpy(pessoa->nomeUsuario, valorAtualiza);
            pessoa->tamanhoNomeUsuario = strlen(valorAtualiza);
        }
    }
}

// -----------------------------------------------Funções auxiliares diversas----------------------------------------------------------

void preencheStringCifrao (char *string) {
    for (int i = 0; i < 10; i++) {
		string[i] = '$';
	}
}

// verifica a correspondência entre o registro procurado e o campo requisitado
int correspondeRegistro (char nomeCampo[MAX_STRING_TAMANHO], char valorCampo[MAX_STRING_TAMANHO], RegistroPessoa pessoa) {
        // Agora, verificamos se este é o registro que procuramos
        int correspondeu = 0;
        
        if (strcmp(nomeCampo, "idadePessoa") == 0) {
            if (strcmp(valorCampo, "NULO") == 0) {
                if (pessoa.idadePessoa == -1) {
                    correspondeu = 1;
                }
            } else {
                if (pessoa.idadePessoa == atoi(valorCampo)) {
                    correspondeu = 1;
                }
            }
        } 
        else if (strcmp(nomeCampo, "nomePessoa") == 0) {
            if (strcmp(valorCampo, "NULO") == 0) {
                if (pessoa.tamanhoNomePessoa == 0) {
                    correspondeu = 1;
                }
            } else {
                if (strcmp(pessoa.nomePessoa, valorCampo) == 0) {
                    correspondeu = 1;
                }
            }
        } 
        else if (strcmp(nomeCampo, "nomeUsuario") == 0) {
            if (strcmp(valorCampo, "NULO") == 0) {
                if (pessoa.tamanhoNomeUsuario == 0) {
                    correspondeu = 1;
                } 
            } else {
                if (strcmp(pessoa.nomeUsuario, valorCampo) == 0) {
                    correspondeu = 1;
                } 
            }
        }

	// Se correspondeu, retorna 1
	if (correspondeu == 1) {
		return 1;
	}
	// Se não correspondeu, retorna 0
	else {
		return 0;
	}

}

// retorna a quantidade de bytes uteis (bytes de dado) de um registro de pessoa
int tamanhoUtilRegistro (RegistroPessoa pessoa) {
    // 4 * 4: 4 ints que possuem 4 bytes cada
    return 4 * 4 + pessoa.tamanhoNomePessoa + pessoa.tamanhoNomeUsuario;
}

// Para o QuickSort
int compararIndices(const void *a, const void *b) {
    RegistroIndice *regA = (RegistroIndice *)a;
    RegistroIndice *regB = (RegistroIndice *)b;
    return (regA->id - regB->id);
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

int compararIndicesPeloId(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
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


//---------------------------------------------------- Funções e ram e super função de busca---------------------------------------------------------

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


// superfunção de buscar pessoas
// retorna o numero de registros encontrados
int buscarPessoas(FILE *fpPessoa, RegistroIndice *indiceEmRAM, int numRegIndice, CabecalhoPessoa *headerPessoa, char *nomeCampo, char *valorCampo, long long *vetorOffsets) {
    
    int numEncontrados = 0;

    // busca por indice em memoria RAM
    if (strcmp(nomeCampo, "idPessoa") == 0) { // o campo escolhido permite utilizar o arquivo indice para uma busca binaria mais eficiente
        
        long long offset = buscaBinariaIndiceRAM(indiceEmRAM, numRegIndice, atoi(valorCampo));

        if (offset != -1) {
            // Encontrou no índice, agora verificar se não está removido no arq. de dados
            fseek(fpPessoa, offset, SEEK_SET);
            char removido;
            fread(&removido, sizeof(char), 1, fpPessoa);

            if (removido == '0') {
                vetorOffsets[0] = offset; // Armazena no vetor
                numEncontrados = 1;
            }
        }
    }

    //  busca sequencial para outros tipos de campos solicitados
    else {
        // pula o cabeçalho do arquivo de dados
        fseek(fpPessoa, TAMANHO_CABECALHO_PESSOA, SEEK_SET);

        RegistroPessoa pessoa;
        long long posAtual = TAMANHO_CABECALHO_PESSOA;

        // Loop principal da busca sequencial
        while (posAtual < headerPessoa->proxByteOffSet) {

            // Salvando o byteOffset antes de ler o registro
            long long offsetDesteRegistro = posAtual;

            // pegar o total de bytes lidos para verificar se é fim de arquivo
            int bytesLidos = lerRegistroPessoa(fpPessoa, &pessoa);

            // Se nenhum byte for lido, então é fim de arquivo
            if (bytesLidos == 0) {
                break;
            }

            // Se o registro é válido (não removido)
            if (pessoa.removido == '0') {
                // Verifica se corresponde ao critério de busca
                if (correspondeRegistro(nomeCampo, valorCampo, pessoa) == 1) {
                    // Adiciona o offset ao vetor e incrementa o contador
                    vetorOffsets[numEncontrados] = posAtual;
                    numEncontrados++;
                    // Se o campo é único (nomeUsuario), podemos parar a busca.
                    if (strcmp(nomeCampo, "nomeUsuario") == 0) {
                        break;
                    }
                }
            }
            // atualizando a posição atual no arquivo sem usar ftell
            posAtual += bytesLidos;
        }
    }

    return numEncontrados; // Retorna o número de Registros encontrados
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
void inserirArestaOrdenada(Aresta **cabeca, Aresta *novaAresta) {
    Aresta *atual;

    // Caso 1: Inserção no início (lista vazia ou novo é menor que o primeiro)
    if (*cabeca == NULL || strcmp((*cabeca)->nomeDestino, novaAresta->nomeDestino) >= 0) {
        novaAresta->prox = *cabeca;
        *cabeca = novaAresta;
    } else {
        // Caso 2: Percorrer para achar a posição
        atual = *cabeca;
        while (atual->prox != NULL && strcmp(atual->prox->nomeDestino, novaAresta->nomeDestino) < 0) {
            atual = atual->prox;
        }
        novaAresta->prox = atual->prox;
        atual->prox = novaAresta;
    }
}

// Busca Sequencial para achar o ÍNDICE no vetor do grafo dado um idPessoa.
// (O arquivo segue tem IDs, mas nosso vetor está ordenado por NOME, então não dá pra fazer busca binária simples por ID no vetor principal)
int buscaBinariaPorIdNoGrafo(Grafo *grafo, int idProcurado) {
    // Nota: Como o grafo não está ordenado por ID, a busca é sequencial O(N).
    // Para datasets muito grandes, criaríamos um índice auxiliar, mas para este trabalho isso é suficiente.
    for(int i = 0; i < grafo->numVertices; i++) {
        if (grafo->listaVertices[i].idPessoa == idProcurado) {
            return i;
        }
    }
    return -1; 
}

// Carrega vértices e ordena por nomeUsuario
void carregarVerticesDoArquivo(FILE *fpPessoa, Grafo *grafo) {
    RegistroPessoa reg;
    int i = 0;
    
    fseek(fpPessoa, TAMANHO_CABECALHO_PESSOA, SEEK_SET);

    while (lerRegistroPessoa(fpPessoa, &reg) != 0) {
        if (reg.removido == '0') {
            grafo->listaVertices[i].idPessoa = reg.idPessoa;
            strcpy(grafo->listaVertices[i].nomePessoa, reg.nomePessoa);
            strcpy(grafo->listaVertices[i].nomeUsuario, reg.nomeUsuario);
            grafo->listaVertices[i].inicioLista = NULL;
            i++;
        }
    }
    
    // Atualiza número real de vértices (caso haja removidos que não entraram)
    grafo->numVertices = i;

    // Ordenação fundamental para a saída correta
    qsort(grafo->listaVertices, grafo->numVertices, sizeof(Vertice), compararVerticesPorNomeUsuario);
}

// Carrega arestas. Se modoTransposto == 1, inverte origem e destino.
void carregarArestasDoArquivo(FILE *fpSegue, Grafo *grafo, int modoTransposto) {
    RegistroSegue reg;
    fseek(fpSegue, TAMANHO_CABECALHO_SEGUE, SEEK_SET);

    // Como o arquivo segueOrdenado tem registros de tamanho fixo, podemos usar fread direto
    // Mas precisamos pular os bytes se ele estiver removido.
    // Sua função lerRegistroSegue já lê tudo. Vamos usá-la.
    
    // Nota: lerRegistroSegue lê removido + dados.
    while (fread(&reg.removido, sizeof(char), 1, fpSegue) == 1) {
        fread(&reg.idPessoaQueSegue, sizeof(int), 1, fpSegue);
        fread(&reg.idPessoaQueESeguida, sizeof(int), 1, fpSegue);
        fread(reg.dataInicioQueSegue, sizeof(char), 10, fpSegue);
        fread(reg.dataFimQueSegue, sizeof(char), 10, fpSegue);
        fread(&reg.grauAmizade, sizeof(char), 1, fpSegue);

        if (reg.removido == '1') continue;

        int idOrigem, idDestino;

        if (modoTransposto == 0) {
            idOrigem = reg.idPessoaQueSegue;
            idDestino = reg.idPessoaQueESeguida;
        } else {
            // Inversão para a Funcionalidade 12
            idOrigem = reg.idPessoaQueESeguida; 
            idDestino = reg.idPessoaQueSegue;
        }

        // Achar índices no vetor (mapeamento ID -> Índice)
        int indexOrigem = buscaBinariaPorIdNoGrafo(grafo, idOrigem);
        int indexDestino = buscaBinariaPorIdNoGrafo(grafo, idDestino);

        if (indexOrigem != -1 && indexDestino != -1) {
            Aresta *nova = (Aresta*)malloc(sizeof(Aresta));
            nova->idDestino = idDestino;
            // Copia o nome do destino para a aresta (facilita o print e ordenação da lista)
            strcpy(nova->nomeDestino, grafo->listaVertices[indexDestino].nomeUsuario);
            
            nova->grauAmizade = reg.grauAmizade;
            
            // Tratamento de strings de data
            strncpy(nova->dataInicio, reg.dataInicioQueSegue, 10); nova->dataInicio[10] = '\0';
            strncpy(nova->dataFim, reg.dataFimQueSegue, 10); nova->dataFim[10] = '\0';
            
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
    if (f->tamanho == 0) return -1;
    int item = f->itens[f->inicio];
    f->inicio = (f->inicio + 1) % f->capacidade;
    f->tamanho--;
    return item;
}

int filaVazia(Fila *f) {
    return (f->tamanho == 0);
}



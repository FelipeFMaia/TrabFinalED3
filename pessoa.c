#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pessoa.h" 


// - lerCriteriosDeAtualizacao, aplicarAtualizacao
// - buscarPessoas

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

// Atualiza o status de removido, fazendo a remoção lógica 
void removerRegistroPessoa(FILE* fpPessoa) {
  fwrite("1", sizeof(char), 1, fpPessoa);
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

// retorna a quantidade de bytes uteis (bytes de dado) de um registro de pessoa
int tamanhoUtilRegistro (RegistroPessoa pessoa) {
    // 4 * 4: 4 ints que possuem 4 bytes cada
    return 4 * 4 + pessoa.tamanhoNomePessoa + pessoa.tamanhoNomeUsuario;
}

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
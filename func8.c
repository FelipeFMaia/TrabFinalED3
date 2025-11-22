#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utilidades.h"
#include "func.h"


/*
Funcionalidade 8
Criar o arquivo segue.bin lendo do csv
Nosso código basicamente verifica consistências e, caso não haja, procede com a criação de novos arquivos
a partir de um arquivo csv disponibilizado pela disciplina.
*/

int lerLinhaCSV (FILE *fpCSV, RegistroSegue *regSegue) {  
  // Ler a linha no buffer
  char buffer[MAX_STRING_TAMANHO];
  char *verificacao = fgets(buffer, sizeof(buffer), fpCSV);
  
  if (verificacao == NULL) {
    return 0; // fim de arquivo ou falha de leitura
  }

  // eliminar o \n e/ou \r da linha do CSV
  buffer[strcspn(buffer, "\n\r")] = '\0';

  // agora vamos ler campo a campo do registro que esta no buffer
  // campo vai ser o ponteiro para o campo
  char *campo;
  char *ponteiroBuffer = buffer;

  // removido, pela especificicação é sempre '0'
  regSegue->removido = '0';

  // idPessoaQueSegue
  campo = strsep(&ponteiroBuffer, ",");
  if (campo == NULL || strlen(campo) == 0) { // Verificar se o campo é vazio
    regSegue->idPessoaQueSegue = -1;
  }
  else {
    regSegue->idPessoaQueSegue = atoi(campo);
  }

  // idPessoaQueESeguida
  campo = strsep(&ponteiroBuffer, ",");
  if (campo == NULL || strlen(campo) == 0) { // Verificar se o campo é vazio
    regSegue->idPessoaQueESeguida = -1;
  }
  else {
    regSegue->idPessoaQueESeguida = atoi(campo);
  }

  // dataInicioQueSegue
  campo = strsep(&ponteiroBuffer, ",");
  if (campo == NULL || strlen(campo) == 0) { // Verificar se o campo é vazio
    preencheStringCifrao(regSegue->dataInicioQueSegue); 
  }
  else {
    preencheStringCifrao(regSegue->dataInicioQueSegue); 
    strncpy(regSegue->dataInicioQueSegue, campo, strlen(campo));
  }

  // dataFimQueSegue
  campo = strsep(&ponteiroBuffer, ",");
  if (campo == NULL || strlen(campo) == 0) { // Verificar se o campo é vazio
    preencheStringCifrao(regSegue->dataFimQueSegue);
  }
  else {
    preencheStringCifrao(regSegue->dataFimQueSegue);
    strncpy(regSegue->dataFimQueSegue, campo, strlen(campo));
  }

  // grauAmizade
  campo = strsep(&ponteiroBuffer, ",");
  if (campo == NULL || strlen(campo) == 0) { // Verificar se o campo é vazio
    regSegue->grauAmizade = '$'; // TESTE -------------------------------------------------------------------------------------------------------------   
  }
  else {
    regSegue->grauAmizade = campo[0];
  }

  return 1; // Não é fim de arquivo ou foi bem sucedido a leitura
}

void func8() {
  // nomes dos arquivos
  char nameFileCSV[MAX_STRING_TAMANHO];
  char nameFileBIN[MAX_STRING_TAMANHO];

  // instanciar e inicializar cabecalho do arquivo Segue
  CabecalhoSegue headerSegue;
  headerSegue.status = '0'; // inconsistente
  headerSegue.quantidadePessoas = 0;
  headerSegue.proxRRN = 0;

  // Ler nomes dos arquivos do terminal
  scanf(" %s %s", nameFileCSV, nameFileBIN);

  // Abrir CSV
  FILE *fpCSV;
  fpCSV = fopen(nameFileCSV, "r");
  // verificar arquivo CSV
  if (verificaArquivo(fpCSV) == 0) {
    return; // aborta funcionalidade
  }
  
  // Criar segue.bin
  FILE *fpSegue;
  // verificar arquivo binario
  fpSegue = fopen(nameFileBIN, "wb");
  if (verificaArquivo(fpSegue) == 0) {
    fclose(fpCSV);
    return; // aborta funcionalidade
  }

  // preencher o cabecalho do arquivo binario, campo a campo
  escreverCabecalhoSegue(fpSegue, headerSegue);

  // consumir primeira linha do CSV (inutil)
  char buffer[MAX_STRING_TAMANHO];
  fgets(buffer, sizeof(buffer), fpCSV);

  // instanciar registro e variavel auxiliar
  RegistroSegue regSegue;
  int sucessoLeitura;

  while (1) {
    sucessoLeitura = lerLinhaCSV(fpCSV, &regSegue);
    if (sucessoLeitura == 1) {
      escreverRegistroSegue(fpSegue, regSegue);
      headerSegue.quantidadePessoas += 1;
      headerSegue.proxRRN += 1;
    }
    else {
      break;
    }

  }
  
  // atualizar status do arquivo binario, pois vamos parar a escrita e fechar
  headerSegue.status = '1';

  // atualizar o cabecalho do binario
  escreverCabecalhoSegue(fpSegue, headerSegue);
  fclose(fpCSV);
  fclose(fpSegue);
  binarioNaTela(nameFileBIN);
}

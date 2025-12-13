#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "segue.h"

// Coloque aqui as funções:
// - lerCabecalhoSegue, escreverCabecalhoSegue
// - lerRegistroSegue, escreverRegistroSegue

void lerCabecalhoSegue(FILE *fpSegue, CabecalhoSegue *headerSegue) {
	fread(&headerSegue->status, sizeof(char), 1, fpSegue);
    fread(&headerSegue->quantidadePessoas, sizeof(int), 1, fpSegue);
    fread(&headerSegue->proxRRN, sizeof(int), 1, fpSegue);
}

void escreverCabecalhoSegue(FILE *fpSegue, CabecalhoSegue headerSegue) {
    // Vai para o início do arquivo
	fseek(fpSegue, 0, SEEK_SET);
    // Escreve
	fwrite(&headerSegue.status, sizeof(char), 1, fpSegue);
    fwrite(&headerSegue.quantidadePessoas, sizeof(int), 1, fpSegue);
    fwrite(&headerSegue.proxRRN, sizeof(int), 1, fpSegue);
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
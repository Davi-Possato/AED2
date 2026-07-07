#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "insere_atualiza.h"
#include "structs.h"
#include "escreve_arq.h"
#include "ler_bin.h"


extern void ScanQuoteString(char *str);

// função para ler do stdin os dados de uma nova estação p preencher o reg (estrutura definida que será, posteriormente, inserido no arquivo binário)

void ler_entrada_registro(Reg *r) {
    r->removido = '0';
    r->proximo  = -1;

    char buf[100];
    scanf("%s", buf);
    r->codEstacao = (strcmp(buf, "NULO") == 0) ? -1 : atoi(buf);

    // p campos de Strings (nomEst e NomeLinha)

    char nomeEst[100];
    ScanQuoteString(nomeEst); 

    // verifica se o registro a ser lido é nulo
    if (nomeEst[0] == '\0') { 
        r->tamNomeEstacao = 0;
        r->nomeEstacao = NULL;
    // senão 
    } else {
        r->tamNomeEstacao = (int)strlen(nomeEst); //armazena o tamanho da string
        r->nomeEstacao = strdup(nomeEst); // faz malloc + copia do conteudo p um ponteiro do registro
    }

    scanf("%s", buf);
    r->codLinha = (strcmp(buf, "NULO") == 0) ? -1 : atoi(buf);

    char nomeLin[100];
    ScanQuoteString(nomeLin);
    if (nomeLin[0] == '\0') {
        r->tamNomeLinha = 0;
        r->nomeLinha    = NULL;
    } else {
        r->tamNomeLinha = (int)strlen(nomeLin);
        r->nomeLinha    = strdup(nomeLin);
    }

    // Para campos numéricos (cod's e distProxEstacao)

    scanf("%s", buf); // le o valor como string temporária
    r->codProxEstacao = (strcmp(buf, "NULO") == 0) ? -1 : atoi(buf); // verifica se é nulo (marca -1), senão converte p inteiro usando atoi

    scanf("%s", buf);
    r->distProxEstacao = (strcmp(buf, "NULO") == 0) ? -1 : atoi(buf);

    scanf("%s", buf);
    r->codLinhaIntegra = (strcmp(buf, "NULO") == 0) ? -1 : atoi(buf);

    scanf("%s", buf);
    r->codEstIntegra = (strcmp(buf, "NULO") == 0) ? -1 : atoi(buf);
}

// função pra inserir um registro (RegIndice) no vetor de índices que está carregado na memória
void insere_no_indice(RegIndice *vetor, int *qtd, RegIndice nova) {

    int i = *qtd - 1; /* Começa no último elemento existente */

    while (i >= 0 && vetor[i].codEstacao > nova.codEstacao) {
        vetor[i + 1] = vetor[i]; /* Move uma posição à direita */
        i--;
    }

    vetor[i + 1] = nova;
    (*qtd)++;   
}

// função para remover registro do vetor de índices
void remove_do_indice(RegIndice *vetor, int *qtd, int chave) {
    int pos = -1; /* Posição da entrada a remover */

    for (int i = 0; i < *qtd; i++) {
        if (vetor[i].codEstacao == chave) {
            pos = i;
            break;
        }
    }

    if (pos == -1) return; 
    for (int i = pos; i < *qtd - 1; i++) {
        vetor[i] = vetor[i + 1];
    }

    (*qtd)--; 
}

// função pra salvar o vetor de índices no disco
int salva_indice_no_disco(const char *arqIndice, RegIndice *vetor, int qtd) {
    FILE *indice = fopen(arqIndice, "wb");
    if (indice == NULL) return 0; 
    char status = '0';
    fwrite(&status, sizeof(char), 1, indice);

    for (int i = 0; i < qtd; i++) {
        fwrite(&vetor[i].codEstacao, sizeof(int), 1, indice);
        fwrite(&vetor[i].rrn,        sizeof(int), 1, indice);
    }

    status = '1';
    fseek(indice, 0, SEEK_SET);
    fwrite(&status, sizeof(char), 1, indice);

    fclose(indice);
    return 1; /* Sucesso */
}

// função para atualizar registro com base nos campos e valores solicitados
void atualiza_campo_reg(Reg *r, const char *nomeCampo, const char *valor) {

    int valorInt = (strcmp(valor, "NULO") == 0) ? -1 : atoi(valor);


    if (strcmp(nomeCampo, "codEstacao") == 0) {
        r->codEstacao = valorInt;

    } else if (strcmp(nomeCampo, "codLinha") == 0) {
        r->codLinha = valorInt;

    } else if (strcmp(nomeCampo, "codProxEstacao") == 0) {
        r->codProxEstacao = valorInt;

    } else if (strcmp(nomeCampo, "distProxEstacao") == 0) {
        r->distProxEstacao = valorInt;

    } else if (strcmp(nomeCampo, "codLinhaIntegra") == 0) {
        r->codLinhaIntegra = valorInt;

    } else if (strcmp(nomeCampo, "codEstIntegra") == 0) {
        r->codEstIntegra = valorInt;


    } else if (strcmp(nomeCampo, "nomeEstacao") == 0) {
        if (r->nomeEstacao != NULL) {
            free(r->nomeEstacao);
            r->nomeEstacao = NULL;
        }
        if (valor[0] == '\0' || strcmp(valor, "NULO") == 0) {
            /* Novo valor é NULO: tamanho 0, ponteiro NULL */
            r->tamNomeEstacao = 0;
            r->nomeEstacao    = NULL;
        } else {
            r->nomeEstacao    = strdup(valor);
            r->tamNomeEstacao = (int)strlen(valor);
        }

    } else if (strcmp(nomeCampo, "nomeLinha") == 0) {
        if (r->nomeLinha != NULL) {
            free(r->nomeLinha);
            r->nomeLinha = NULL;
        }
        if (valor[0] == '\0' || strcmp(valor, "NULO") == 0) {
            r->tamNomeLinha = 0;
            r->nomeLinha    = NULL;
        } else {
            r->nomeLinha    = strdup(valor);
            r->tamNomeLinha = (int)strlen(valor);
        }
    }
}

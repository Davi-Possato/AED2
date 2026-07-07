/*
 * insere_atualiza.h
 *
 * Declarações das funções auxiliares exclusivas das Funcionalidades [8] e [9]:
 *   - Inserção de novos registros com reaproveitamento da pilha de removidos
 *   - Atualização de campos de registros existentes
 *
 * Ambas as funcionalidades também mantêm o arquivo de índice primário
 * (indexaEstacao) sincronizado e ordenado pelo campo codEstacao.
 *
 * Padrão de nomenclatura: segue o mesmo estilo do restante do projeto.
 */

#pragma once
#include <stdio.h>
#include "structs.h"

/*
 * @brief Lê da stdin os 8 campos de um novo registro a inserir.
 *
 * Campos lidos (nesta ordem, conforme especificação):
 *   codEstacao nomeEstacao codLinha nomeLinha codProxEstacao
 *   distProxEstacao codLinhaIntegra codEstIntegra
 *
 * Strings (nomeEstacao, nomeLinha) são lidas com ScanQuoteString para
 * suportar valores entre aspas duplas e o token especial NULO.
 * Campos inteiros NULO são armazenados como -1.
 *
 * @param r Ponteiro para a struct Reg que receberá os dados lidos.
 */
void ler_entrada_registro(Reg *r);

/*
 * @brief Insere uma nova entrada no vetor de índice primário em memória,
 *        mantendo-o ordenado crescentemente por codEstacao.
 *
 * Abre espaço deslocando os elementos à direita do ponto de inserção,
 * exatamente como um passo de "insertion sort" pontual.
 *
 * @param vetor      Vetor de RegIndice (deve ter espaço para qtd+1 elementos).
 * @param qtd        Ponteiro para a quantidade atual de entradas; é incrementado.
 * @param nova        Nova entrada a inserir.
 */
void insere_no_indice(RegIndice *vetor, int *qtd, RegIndice nova);

/*
 * @brief Remove do vetor em memória a entrada cujo codEstacao == chave.
 *
 * Desloca todos os elementos posteriores uma posição à esquerda e
 * decrementa o contador de entradas. Operação O(n) no pior caso.
 *
 * @param vetor  Vetor de RegIndice.
 * @param qtd    Ponteiro para a quantidade atual; é decrementado se removeu.
 * @param chave  codEstacao a remover.
 */
void remove_do_indice(RegIndice *vetor, int *qtd, int chave);

/*
 * @brief Persiste o vetor de índice em memória para o arquivo em disco.
 *
 * Grava: 1 byte de status ('1' = consistente) seguido de todos os pares
 * (codEstacao, rrn) de 8 bytes cada, na ordem do vetor (já ordenada).
 *
 * @param arqIndice  Nome do arquivo de índice a (re)criar.
 * @param vetor      Vetor de RegIndice a gravar.
 * @param qtd        Quantidade de entradas no vetor.
 * @return           1 em sucesso, 0 em falha de abertura.
 */
int salva_indice_no_disco(const char *arqIndice, RegIndice *vetor, int qtd);

/*
 * @brief Aplica um par (nomeCampo, valorCampo) sobre um registro em memória.
 *
 * Suporta todos os campos da struct Reg. Strings são duplicadas via strdup
 * e o campo de tamanho correspondente é atualizado. Valores "NULO" em
 * campos de string zeram o tamanho e liberam o ponteiro anterior.
 * Valores "NULO" em campos inteiros gravam -1.
 *
 * @param r         Registro a modificar.
 * @param nomeCampo Nome do campo (ex: "nomeEstacao", "codLinha").
 * @param valor     Valor como string (ex: "Luz", "42", "NULO").
 */
void atualiza_campo_reg(Reg *r, const char *nomeCampo, const char *valor);

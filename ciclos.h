/*
 * ciclos.h
 *
 * Módulo NOVO responsável pela funcionalidade [13]: contar quantos ciclos
 * simples existem no grafo direcionado original a partir de uma estação de
 * origem (um ciclo simples não repete nenhum vértice, exceto o primeiro e
 * o último, que são a própria origem).
 */

#pragma once
#include "grafo.h"

/*!
 * @brief Conta os ciclos simples que partem de "origem" e retornam a ela,
 *        percorrendo o grafo direcionado original (o mesmo montado pela
 *        funcionalidade [10]/constroi_grafo_arquivo), via busca em
 *        profundidade com backtracking.
 *
 * @param g      Grafo direcionado já construído.
 * @param origem Nome da estação de origem.
 * @return       Quantidade de ciclos simples encontrados, ou -1 se não for
 *               possível voltar à estação de origem (nenhum ciclo existe)
 *               ou se a estação não existir no grafo.
 */
int conta_ciclos_simples(Grafo* g, char* origem);

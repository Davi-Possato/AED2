/*
 * grafo_io.h
 *
 * Módulo NOVO, criado para a implementação das funcionalidades [12] e [13].
 *
 * Objetivo: evitar duplicar pela terceira/quarta vez, dentro do main.c, o
 * mesmo bloco de código já usado nas funcionalidades [10] e [11] para
 * montar o grafo (dicionário codEstacao->nomeEstacao, criação dos vértices
 * ordenados e criação das arestas normais/integração). Nenhuma das funções
 * ou blocos já existentes em main.c foi alterada: este arquivo apenas
 * concentra, para o código NOVO, a mesma lógica de construção do grafo,
 * reaproveitando as structs e funções já existentes em grafo.h/structs.h/
 * ler_bin.h.
 */

#pragma once
#include <stdio.h>
#include "structs.h"
#include "ler_bin.h"
#include "grafo.h"

/*!
 * @brief Constrói o grafo direcionado ponderado (vértices ordenados
 *        alfabeticamente + listas de adjacência) a partir de um arquivo de
 *        dados binário, seguindo exatamente os mesmos critérios de inserção
 *        de vértices e arestas definidos na especificação da funcionalidade
 *        [10]: aresta (codEstacao, codProxEstacao) usando o nome/distância/
 *        linha do próprio registro, e aresta de integração
 *        (codEstacao, codEstacaoIntegra) com distância 0 e linha
 *        "Integração", somente quando os nomes das estações diferem.
 *
 * @param bin Arquivo binário já aberto para leitura, com o byte de status já
 *            validado pelo chamador. A função reposiciona o cursor
 *            internamente (fseek para o início dos registros de dados), não
 *            sendo necessário que o chamador já esteja posicionado lá.
 * @param g   Grafo a ser preenchido. A função aloca e organiza o vetor de
 *            vértices (o chamador é responsável por liberar com
 *            libera_grafo, função já existente em grafo.c).
 */
void constroi_grafo_arquivo(FILE* bin, Grafo* g);

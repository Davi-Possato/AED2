/*
 * arvore_geradora.h
 *
 * Módulo NOVO responsável pela funcionalidade [12]:
 *   - construir a árvore geradora mínima (AGM) do grafo, tratando as linhas
 *     como bidirecionais (ida e volta), conforme especificado;
 *   - percorrer a AGM em profundidade a partir de uma estação origem,
 *     imprimindo cada aresta (pai, filho, distância) na ordem em que é
 *     visitada.
 *
 * Desempates seguidos (conforme especificação):
 *   (i)  na escolha de vértices, prevalece o de menor nome;
 *   (ii) em empate de peso entre arestas (u1,v1) e (u2,v2), prevalece a
 *        aresta de menor u; em novo empate, a de menor v.
 * Como o vetor de vértices já chega ordenado alfabeticamente (compara_vertices,
 * já existente em grafo.c), varrer os vértices em ordem crescente de índice e
 * usar comparação estrita (<) já implementa naturalmente o desempate (i).
 */

#pragma once
#include "grafo.h"

/*!
 * @brief Constrói a árvore geradora mínima de g, tratando toda aresta
 *        existente como bidirecional (u->v e v->u com o mesmo peso),
 *        usando o algoritmo de Prim com os critérios de desempate exigidos
 *        pela especificação.
 *
 * @param g      Grafo direcionado original (já construído, ex.: via
 *               constroi_grafo_arquivo).
 * @param arvore Grafo de saída, preenchido com os vértices de g (mesma
 *               ordem alfabética) e as arestas escolhidas pela AGM,
 *               inseridas nos dois sentidos para permitir a busca em
 *               profundidade a partir de qualquer vértice. Deve ser
 *               liberado pelo chamador com libera_grafo (já existente).
 */
void arvore_geradora_minima(Grafo* g, Grafo* arvore);

/*!
 * @brief Percorre a árvore geradora mínima em profundidade a partir da
 *        estação origem, imprimindo em cada chamada recursiva o nome da
 *        estação atual, o nome da estação filha e a distância entre elas,
 *        no formato "nomeAtual, nomeFilho, distancia". Quando um vértice
 *        tem mais de um filho, eles são visitados em ordem alfabética
 *        (garantida pela própria adiciona_aresta, já existente, que insere
 *        cada aresta em ordem crescente pelo nome do destino).
 *
 * @param arvore Árvore geradora mínima já construída (arvore_geradora_minima).
 * @param origem Nome da estação a partir da qual a busca em profundidade
 *               deve começar.
 */
void dfs_arvore(Grafo* arvore, char* origem);

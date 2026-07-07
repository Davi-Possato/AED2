#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arvore_geradora.h"

/* Constante para representar "infinito" nas chaves do Prim (mesmo valor já
 * usado em grafo.c para o Dijkstra). */
#define INFINITO 2147483647

/* busca_indice_vertice já existe em grafo.c (não é static), mas não está
 * declarada em grafo.h. Para não precisar alterar nenhum arquivo já
 * existente, ela é redeclarada aqui via extern. */
extern int busca_indice_vertice(Grafo* g, char* nome);

/* Protótipo interno (uso exclusivo deste arquivo) */
static void dfs_arvore_rec(Grafo* arvore, int atual, int* visitado);

/*
 * adiciona_aresta_menor
 *
 * Ao espelhar o grafo (ver espelha_grafo abaixo), a mesma aresta (origem,
 * destino) pode acabar sendo inserida mais de uma vez em "g" — por exemplo,
 * se o arquivo já contém as duas direções (u->v e v->u) para o mesmo par de
 * estações, possivelmente com pesos ligeiramente diferentes. A função
 * adiciona_aresta (já existente) só mescla o nome da linha quando o destino
 * já existe, sem nunca atualizar a distância — o que manteria sempre a
 * primeira inserida, mesmo que não seja a menor.
 *
 * Esta função corrige isso: se já existe uma aresta para o mesmo destino,
 * apenas atualiza sua distância caso a nova seja menor (mantendo o menor
 * peso, que é a interpretação correta de "a mesma ligação física" ao tratar
 * o grafo como não-direcionado). Caso não exista ainda, delega para
 * adiciona_aresta (já existente) para criar a aresta normalmente.
 */
static void adiciona_aresta_menor(Grafo* g, char* origem, char* destino, int distancia, char* linha) {
    int idx = busca_indice_vertice(g, origem);
    if (idx == -1) return;

    Aresta* atual = g->vertices[idx].inicio;
    while (atual != NULL && strcmp(atual->destino, destino) != 0) {
        atual = atual->prox;
    }

    if (atual != NULL) {
        if (distancia < atual->distancia) {
            atual->distancia = distancia;
        }
    } else {
        adiciona_aresta(g, origem, destino, distancia, linha);
    }
}

/*
 * espelha_grafo
 *
 * Cria uma versão não-direcionada de g: para cada aresta u->v (e cada nome
 * de linha associado a ela) já existente em g, insere tanto u->v quanto
 * v->u no grafo de saída "nd". Isso implementa literalmente a orientação da
 * especificação da funcionalidade [12]: "as linhas podem ser de ida e
 * volta [...] se existe uma linha entre uma estação 1 e uma estação 2, as
 * pessoas podem ir da estação 1 para a estação 2 e vice-versa".
 *
 * Usa adiciona_aresta_menor (acima) em vez de adiciona_aresta diretamente,
 * para garantir que, se a mesma ligação acabar sendo espelhada mais de uma
 * vez, prevaleça sempre o menor peso.
 */
static void espelha_grafo(Grafo* g, Grafo* nd) {
    int n = g->qtd_vertices;

    nd->capacidade = n > 0 ? n : 1;
    nd->qtd_vertices = n;
    nd->vertices = malloc(nd->capacidade * sizeof(Vertice));

    for (int i = 0; i < n; i++) {
        strcpy(nd->vertices[i].origem, g->vertices[i].origem);
        nd->vertices[i].inicio = NULL;
    }

    for (int i = 0; i < n; i++) {
        Aresta* a = g->vertices[i].inicio;
        while (a != NULL) {
            for (int k = 0; k < a->linhas.qtd_linhas; k++) {
                adiciona_aresta_menor(nd, g->vertices[i].origem, a->destino, a->distancia, a->linhas.linhas[k]);
                adiciona_aresta_menor(nd, a->destino, g->vertices[i].origem, a->distancia, a->linhas.linhas[k]);
            }
            a = a->prox;
        }
    }
}

void arvore_geradora_minima(Grafo* g, Grafo* arvore) {
    Grafo nd; /* versão não-direcionada de g, usada apenas internamente */
    espelha_grafo(g, &nd);

    int n = nd.qtd_vertices;

    arvore->capacidade = n > 0 ? n : 1;
    arvore->qtd_vertices = n;
    arvore->vertices = malloc(arvore->capacidade * sizeof(Vertice));
    for (int i = 0; i < n; i++) {
        strcpy(arvore->vertices[i].origem, nd.vertices[i].origem);
        arvore->vertices[i].inicio = NULL;
    }

    int* chave = malloc(n * sizeof(int));
    int* visitado = malloc(n * sizeof(int));
    int* pai = malloc(n * sizeof(int));

    for (int i = 0; i < n; i++) {
        chave[i] = INFINITO;
        visitado[i] = 0;
        pai[i] = -1;
    }
    /* Vértice inicial da árvore: por convenção do algoritmo de Prim, o
     * primeiro vértice inserido não tem aresta de entrada, então sua chave
     * é zerada para inicializar a busca. Escolhe-se o vértice de índice 0
     * (menor nome, já que o vetor está ordenado alfabeticamente),
     * implementando o desempate (i) da especificação também para o início
     * do algoritmo. */
    if (n > 0) chave[0] = 0;

    /* Algoritmo de Prim: a cada iteração, escolhe entre os vértices ainda
     * não inseridos aquele de menor chave; a comparação estrita "<" na
     * varredura em ordem crescente de índice garante que, em caso de
     * empate, prevaleça o de menor nome (desempate (i)). */
    for (int cont = 0; cont < n; cont++) {
        int u = -1, minimo = INFINITO;
        for (int i = 0; i < n; i++) {
            if (!visitado[i] && chave[i] < minimo) {
                minimo = chave[i];
                u = i;
            }
        }
        if (u == -1) break; /* componente esgotado (grafo desconexo) */

        visitado[u] = 1;

        if (pai[u] != -1) {
            adiciona_aresta(arvore, nd.vertices[pai[u]].origem, nd.vertices[u].origem, chave[u], "MST");
            adiciona_aresta(arvore, nd.vertices[u].origem, nd.vertices[pai[u]].origem, chave[u], "MST");
        }

        /* relaxamento dos vizinhos de u */
        Aresta* a = nd.vertices[u].inicio;
        while (a != NULL) {
            int v = busca_indice_vertice(&nd, a->destino);
            if (v != -1 && !visitado[v]) {
                /* desempate (ii): entre pesos iguais, prevalece a aresta
                 * cujo valor de u seja o menor (nome da estação). Como v é
                 * o mesmo vértice em ambos os lados do empate, não é
                 * necessário desempatar por v separadamente aqui. */
                if (a->distancia < chave[v] ||
                    (a->distancia == chave[v] && pai[v] != -1 &&
                     strcmp(nd.vertices[u].origem, nd.vertices[pai[v]].origem) < 0)) {
                    chave[v] = a->distancia;
                    pai[v] = u;
                }
            }
            a = a->prox;
        }
    }

    free(chave);
    free(visitado);
    free(pai);
    libera_grafo(&nd);
}

/* Percorre recursivamente a árvore em pré-ordem, imprimindo cada aresta
 * (atual, filho, distância) antes de descer para o filho, exatamente na
 * ordem exigida pela especificação. Os filhos já estão em ordem alfabética
 * porque adiciona_aresta (já existente) insere cada aresta ordenadamente. */
static void dfs_arvore_rec(Grafo* arvore, int atual, int* visitado) {
    visitado[atual] = 1;

    Aresta* a = arvore->vertices[atual].inicio;
    while (a != NULL) {
        int filho = busca_indice_vertice(arvore, a->destino);
        if (filho != -1 && !visitado[filho]) {
            printf("%s, %s, %d\n", arvore->vertices[atual].origem, a->destino, a->distancia);
            dfs_arvore_rec(arvore, filho, visitado);
        }
        a = a->prox;
    }
}

void dfs_arvore(Grafo* arvore, char* origem) {
    int n = arvore->qtd_vertices;
    int* visitado = calloc(n, sizeof(int));

    int idx = busca_indice_vertice(arvore, origem);
    if (idx != -1) {
        dfs_arvore_rec(arvore, idx, visitado);
    }

    free(visitado);
}

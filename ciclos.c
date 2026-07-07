#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ciclos.h"

/* busca_indice_vertice já existe em grafo.c (não é static), mas não está
 * declarada em grafo.h. Para não precisar alterar nenhum arquivo já
 * existente, ela é redeclarada aqui via extern. */
extern int busca_indice_vertice(Grafo* g, char* nome);

/*
 * conta_ciclos_rec
 *
 * Busca em profundidade com backtracking clássica para contar ciclos
 * simples que passam por um vértice fixo (origem_idx). A cada aresta
 * (atual -> v) explorada:
 *   - se v é a própria origem, um ciclo simples foi fechado (contador++);
 *   - se v ainda não foi visitado nesse caminho, marca como visitado,
 *     desce recursivamente e depois desmarca (backtracking), permitindo
 *     que v participe de outros caminhos/ciclos diferentes.
 * O vértice de origem permanece marcado como visitado durante toda a busca
 * (feito em conta_ciclos_simples) para impedir que um caminho intermediário
 * passe por ele novamente antes de fechar o ciclo.
 */
static void conta_ciclos_rec(Grafo* g, int origem_idx, int atual, int* visitado, int* contador) {
    Aresta* a = g->vertices[atual].inicio;
    while (a != NULL) {
        int v = busca_indice_vertice(g, a->destino);
        if (v == origem_idx) {
            (*contador)++;
        } else if (v != -1 && !visitado[v]) {
            visitado[v] = 1;
            conta_ciclos_rec(g, origem_idx, v, visitado, contador);
            visitado[v] = 0;
        }
        a = a->prox;
    }
}

int conta_ciclos_simples(Grafo* g, char* origem) {
    int idx = busca_indice_vertice(g, origem);
    if (idx == -1) return -1;

    int n = g->qtd_vertices;
    int* visitado = calloc(n, sizeof(int));
    visitado[idx] = 1;

    int contador = 0;
    conta_ciclos_rec(g, idx, idx, visitado, &contador);

    free(visitado);

    return contador > 0 ? contador : -1;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "grafo_io.h"

/*
 * constroi_grafo_arquivo
 *
 * Reproduz, de forma modularizada, os 3 passos já usados nos blocos das
 * funcionalidades [10] e [11] em main.c:
 *   1) monta um dicionário local (codEstacao -> nomeEstacao) apenas com os
 *      registros ativos (não removidos);
 *   2) cria um vértice para cada nome de estação distinto que aparece como
 *      origem, próxima estação ou estação de integração, e ordena o vetor
 *      de vértices em ordem alfabética (compara_vertices, já existente);
 *   3) cria as arestas (normal e de integração), reutilizando a função
 *      adiciona_aresta já existente em grafo.c, que também cuida de manter
 *      cada lista linear ordenada e de mesclar nomes de linha quando duas
 *      estações já compartilham uma aresta.
 */
void constroi_grafo_arquivo(FILE* bin, Grafo* g) {
    MapaCodNome mapa[5000];
    int qtd_mapa = 0;
    Reg reg;

    g->capacidade = 100;
    g->qtd_vertices = 0;
    g->vertices = malloc(g->capacidade * sizeof(Vertice));

    /* 1) dicionário codEstacao -> nomeEstacao, apenas registros ativos */
    fseek(bin, 17, SEEK_SET);
    while (ler_reg_bin(bin, &reg)) {
        if (reg.removido == '0' && reg.codEstacao != -1 &&
            reg.tamNomeEstacao > 0 && reg.nomeEstacao != NULL) {
            int ja_tem_mapa = 0;
            for (int i = 0; i < qtd_mapa; i++) {
                if (mapa[i].cod == reg.codEstacao) { ja_tem_mapa = 1; break; }
            }
            if (!ja_tem_mapa) {
                mapa[qtd_mapa].cod = reg.codEstacao;
                strcpy(mapa[qtd_mapa].nome, reg.nomeEstacao);
                qtd_mapa++;
            }
        }
        free_reg(&reg);
    }

    /* 2) cria os vértices (origem, próxima estação e estação de integração) */
    fseek(bin, 17, SEEK_SET);
    while (ler_reg_bin(bin, &reg)) {
        if (reg.removido == '0') {
            char* nomes_v[3] = {NULL, NULL, NULL};

            if (reg.tamNomeEstacao > 0 && reg.nomeEstacao != NULL) nomes_v[0] = reg.nomeEstacao;
            if (reg.codProxEstacao != -1) nomes_v[1] = lookup_nome(mapa, qtd_mapa, reg.codProxEstacao);
            if (reg.codEstIntegra != -1) nomes_v[2] = lookup_nome(mapa, qtd_mapa, reg.codEstIntegra);

            for (int v = 0; v < 3; v++) {
                if (nomes_v[v] != NULL) {
                    int ja_tem = 0;
                    for (int i = 0; i < g->qtd_vertices; i++) {
                        if (strcmp(g->vertices[i].origem, nomes_v[v]) == 0) { ja_tem = 1; break; }
                    }
                    if (!ja_tem) {
                        if (g->qtd_vertices == g->capacidade) {
                            g->capacidade *= 2;
                            g->vertices = realloc(g->vertices, g->capacidade * sizeof(Vertice));
                        }
                        strcpy(g->vertices[g->qtd_vertices].origem, nomes_v[v]);
                        g->vertices[g->qtd_vertices].inicio = NULL;
                        g->qtd_vertices++;
                    }
                }
            }
        }
        free_reg(&reg);
    }

    qsort(g->vertices, g->qtd_vertices, sizeof(Vertice), compara_vertices);

    /* 3) cria as arestas normais e de integração */
    fseek(bin, 17, SEEK_SET);
    while (ler_reg_bin(bin, &reg)) {
        if (reg.removido == '0' && reg.tamNomeEstacao > 0 && reg.nomeEstacao != NULL) {
            char* origem = reg.nomeEstacao;

            if (reg.codProxEstacao != -1) {
                char* destino = lookup_nome(mapa, qtd_mapa, reg.codProxEstacao);
                if (destino != NULL && reg.tamNomeLinha > 0 && reg.nomeLinha != NULL) {
                    adiciona_aresta(g, origem, destino, reg.distProxEstacao, reg.nomeLinha);
                }
            }
            if (reg.codEstIntegra != -1) {
                char* destino_integra = lookup_nome(mapa, qtd_mapa, reg.codEstIntegra);
                if (destino_integra != NULL && strcmp(origem, destino_integra) != 0) {
                    adiciona_aresta(g, origem, destino_integra, 0, "Integração");
                }
            }
        }
        free_reg(&reg);
    }
}

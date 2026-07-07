#include "grafo.h"
// Constante para representar o "Infinito" nas distâncias
#define INFINITO 2147483647
char* lookup_nome(MapaCodNome* mapa, int qtd_mapa, int cod) {
    for (int i = 0; i < qtd_mapa; i++) {
        if (mapa[i].cod == cod) {
            return mapa[i].nome;
        }
    }
    return NULL;
}

int compara_vertices(const void* a, const void* b) {
    Vertice* va = (Vertice*)a;
    Vertice* vb = (Vertice*)b;
    return strcmp(va->origem, vb->origem);
}

void adiciona_linha(Aresta* aresta, char* nome_linha) {
    for (int i = 0; i < aresta->linhas.qtd_linhas; i++) {
        if (strcmp(aresta->linhas.linhas[i], nome_linha) == 0) return;
    }
    int i = aresta->linhas.qtd_linhas - 1;
    while (i >= 0 && strcmp(aresta->linhas.linhas[i], nome_linha) > 0) {
        strcpy(aresta->linhas.linhas[i + 1], aresta->linhas.linhas[i]);
        i--;
    }
    strcpy(aresta->linhas.linhas[i + 1], nome_linha);
    aresta->linhas.qtd_linhas++;
}

void adiciona_aresta(Grafo* g, char* origem, char* destino, int distancia, char* linha) {
    if (!origem || !destino) return;

    Vertice* u = NULL;
    for (int i = 0; i < g->qtd_vertices; i++) {
        if (strcmp(g->vertices[i].origem, origem) == 0) {
            u = &g->vertices[i];
            break;
        }
    }
    if (!u) return;

    Aresta* atual = u->inicio;
    Aresta* ant = NULL;

    while (atual != NULL && strcmp(atual->destino, destino) < 0) {
        ant = atual;
        atual = atual->prox;
    }

    if (atual != NULL && strcmp(atual->destino, destino) == 0) {
        adiciona_linha(atual, linha);
    } else {
        Aresta* nova = malloc(sizeof(Aresta));
        strcpy(nova->destino, destino);
        nova->distancia = distancia;
        nova->linhas.qtd_linhas = 0;
        adiciona_linha(nova, linha);
        nova->prox = atual;

        if (ant == NULL) u->inicio = nova;
        else ant->prox = nova;
    }
}

void libera_grafo(Grafo* g) {
    for (int i = 0; i < g->qtd_vertices; i++) {
        Aresta* atual = g->vertices[i].inicio;
        while (atual != NULL) {
            Aresta* prox = atual->prox;
            free(atual);
            atual = prox;
        }
    }
    free(g->vertices);
}

// Função auxiliar para achar o índice de um vértice pelo nome
int busca_indice_vertice(Grafo* g, char* nome) {
    for (int i = 0; i < g->qtd_vertices; i++) {
        if (strcmp(g->vertices[i].origem, nome) == 0) return i;
    }
    return -1;
}

void dijkstra(Grafo* g, char* nome_origem, char* nome_destino) {
    int id_origem = busca_indice_vertice(g, nome_origem);
    int id_destino = busca_indice_vertice(g, nome_destino);

    if (id_origem == -1 || id_destino == -1) {
        printf("Não existe caminho entre as estações solicitadas.\n");
        return;
    }

    // Vetores do Dijkstra
    int* distancias = malloc(g->qtd_vertices * sizeof(int));
    int* visitados = malloc(g->qtd_vertices * sizeof(int));
    int* antecessores = malloc(g->qtd_vertices * sizeof(int));

    // Inicialização
    for (int i = 0; i < g->qtd_vertices; i++) {
        distancias[i] = INFINITO;
        visitados[i] = 0;
        antecessores[i] = -1;
    }
    distancias[id_origem] = 0;

    // Loop principal do Dijkstra
    for (int count = 0; count < g->qtd_vertices; count++) {
        // 1. Pega o vértice não visitado com a menor distância
        int min = INFINITO;
        int u = -1;
        for (int i = 0; i < g->qtd_vertices; i++) {
            if (!visitados[i] && distancias[i] < min) {
                min = distancias[i];
                u = i;
            }
        }

        // Se não achou ninguém alcançável, para
        if (u == -1) break; 
        visitados[u] = 1;

        // Se chegamos no destino, podemos parar antes de varrer tudo!
        if (u == id_destino) break;

        // 2. Atualiza a distância dos vizinhos
        Aresta* a = g->vertices[u].inicio;
        while (a != NULL) {
            int v = busca_indice_vertice(g, a->destino);
            if (v != -1 && !visitados[v] && distancias[u] != INFINITO) {
                // Se a nova distância for MENOR, a gente atualiza
                if (distancias[u] + a->distancia < distancias[v]) {
                    distancias[v] = distancias[u] + a->distancia;
                    antecessores[v] = u;
                }
            }
            a = a->prox;
        }
    }

    // --- MONTAGEM DA SAÍDA ---
    if (distancias[id_destino] == INFINITO) {
        printf("Não existe caminho entre as estações solicitadas.\n");
    } else {
        // Como o caminho nos antecessores é de "trás pra frente", usamos um array temporário para inverter
        int caminho[5000]; 
        int passos = 0;
        int atual = id_destino;
        
        while (atual != -1) {
            caminho[passos++] = atual;
            atual = antecessores[atual];
        }

        printf("Numero de estacoes que serao percorridas: %d\n", passos - 1);
        printf("Distancia que sera percorrida: %d\n", distancias[id_destino]);
        
        // Imprime de trás pra frente (da Origem pro Destino)
        for (int i = passos - 1; i >= 0; i--) {
            printf("%s", g->vertices[caminho[i]].origem);
            if (i > 0) printf(", ");
        }
        printf("\n");
    }

    free(distancias);
    free(visitados);
    free(antecessores);
}
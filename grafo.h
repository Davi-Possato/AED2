#ifndef GRAFO_H
#define GRAFO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Estrutura para ajudar a encontrar o nome da estação pelo código
typedef struct {
    int cod;
    char nome[50];
} MapaCodNome;

// Estrutura para armazenar múltiplas linhas em uma mesma aresta
typedef struct {
    char linhas[10][50]; 
    int qtd_linhas;
} LinhasAresta;

// Estrutura da Aresta (Lista Encadeada)
typedef struct Aresta {
    char destino[50];
    int distancia;
    LinhasAresta linhas;
    struct Aresta* prox;
} Aresta;

// Estrutura do Vértice
typedef struct {
    char origem[50];
    Aresta* inicio;
} Vertice;

// O Grafo completo
typedef struct {
    Vertice* vertices;
    int qtd_vertices;
    int capacidade;
} Grafo;

// Protótipos das funções
char* lookup_nome(MapaCodNome* mapa, int qtd_mapa, int cod);
int compara_vertices(const void* a, const void* b);
void adiciona_linha(Aresta* aresta, char* nome_linha);
void adiciona_aresta(Grafo* g, char* origem, char* destino, int distancia, char* linha);
void libera_grafo(Grafo* g);
void dijkstra(Grafo* g, char* origem, char* destino);

#endif
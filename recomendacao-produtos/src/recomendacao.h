#ifndef RECOMENDACAO_H
#define RECOMENDACAO_H

#include <vector>
#include "lista_compras.h"
#include "similaridade.h"

/*
 * Estrutura que guarda um produto e seu valor de ranking
 */
typedef struct {
    int    indice;
    double ranking;
} ItemRanking;

/*
 * Calcula o vetor de ranqueamento (ja ordenado) de todos os produtos para
 * o cliente idx_cliente, seguindo o algoritmo da Secao 2.3. Usa apenas os
 * dados da matriz de similaridade, por isso serve tanto ao testador C++
 * quanto ao binding Python.
 */
std::vector<ItemRanking> recomendacao_ranking(Similaridade *sim, int idx_cliente);

/* Gera e exibe os k produtos recomendados para o cliente idx_cliente */
void recomendacao_exibe(ListaCompras *lc, Similaridade *sim,
                        int idx_cliente, int k);

#endif

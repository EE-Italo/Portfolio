#include <stdio.h>
#include "similaridade.h"

/* ------------------------------------------------------------------ */
/* Algoritmo PADRAO                                                    */
/* Constroi explicitamente a transposta A^T (m x n) e depois faz a     */
/* multiplicacao I = A x A^T. Esta versao gasta memoria extra com a    */
/* copia transposta e calcula todas as n x n celulas.                  */
/* ------------------------------------------------------------------ */
static void multiplicaPadrao(Similaridade *sim, int n, int m,
                             std::vector<std::vector<int>> *I) {
    /* Cria a transposta A^T: At[k][i] = A[i][k] */
    std::vector<std::vector<int>> At(m, std::vector<int>(n, 0));
    for (int i = 0; i < n; i++)
        for (int k = 0; k < m; k++)
            At[k][i] = sim->matriz_compras[i][k];

    /* I[i][j] = soma de A[i][k] * At[k][j] */
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int soma = 0;
            for (int k = 0; k < m; k++)
                soma += sim->matriz_compras[i][k] * At[k][j];
            (*I)[i][j] = soma;
        }
    }
}

/* ------------------------------------------------------------------ */
/* Algoritmo ADAPTADO (Atividade 4)                                    */
/* Nao constroi a transposta. Como I e simetrica (I[i][j] = I[j][i]),  */
/* calcula apenas a metade superior (j >= i) e copia para a inferior,  */
/* reduzindo quase pela metade o numero de multiplicacoes.             */
/* ------------------------------------------------------------------ */
static void multiplicaAdaptado(Similaridade *sim, int n, int m,
                               std::vector<std::vector<int>> *I) {
    for (int i = 0; i < n; i++) {
        for (int j = i; j < n; j++) {
            int soma = 0;
            for (int k = 0; k < m; k++)
                soma += sim->matriz_compras[i][k] * sim->matriz_compras[j][k];
            (*I)[i][j] = soma;
            (*I)[j][i] = soma;
        }
    }
}

/* ------------------------------------------------------------------ */
/* Constroi a matriz de compras A a partir das compras e calcula S     */
/* usando o algoritmo de multiplicacao escolhido.                      */
/* ------------------------------------------------------------------ */
void similaridade_constroiDeCompras(Similaridade *sim,
                                    const std::vector<std::vector<int>> &compras,
                                    int n_produtos, int algoritmo) {
    int n = (int)compras.size();
    int m = n_produtos;

    sim->n_clientes = n;
    sim->n_produtos = m;

    /* Inicializa a matriz de compras A[n][m] com zeros */
    sim->matriz_compras.assign(n, std::vector<int>(m, 0));

    /* Preenche A: A[i][p] = 1 se cliente i comprou produto p */
    for (int i = 0; i < n; i++)
        for (int p : compras[i])
            sim->matriz_compras[i][p] = 1;

    /* Calcula a matriz de intersecao I = A x A^T usando o algoritmo pedido */
    std::vector<std::vector<int>> I(n, std::vector<int>(n, 0));

    if (algoritmo == ALG_PADRAO)
        multiplicaPadrao(sim, n, m, &I);
    else
        multiplicaAdaptado(sim, n, m, &I);

    /* ----------------------------------------------------------------
     * Calcula S usando a distancia de Jaccard: S[i][j] = 1 - I[i][j] / |Pi|
     * O tamanho |Pi| (quantos produtos o cliente i comprou) e igual a
     * I[i][i], pois A[i] . A[i] conta os 1's da linha i.
     * ---------------------------------------------------------------- */
    sim->S.assign(n, std::vector<double>(n, 1.0));

    for (int i = 0; i < n; i++) {
        int tam_pi = I[i][i];
        if (tam_pi == 0) continue;

        for (int j = 0; j < n; j++)
            sim->S[i][j] = 1.0 - (double)I[i][j] / (double)tam_pi;
    }
}

/* ------------------------------------------------------------------ */
/* Versao que recebe a estrutura ListaCompras (usada pelo testador C++) */
/* Converte as listas de compras em vetores e delega para a versao     */
/* principal acima.                                                     */
/* ------------------------------------------------------------------ */
void similaridade_constroi(Similaridade *sim, ListaCompras *lc, int algoritmo) {
    int n = (int)lc->clientes.size();
    int m = (int)lc->produtos.size();

    std::vector<std::vector<int>> compras(n);
    for (int i = 0; i < n; i++)
        for (int p : lc->compras[i])
            compras[i].push_back(p);

    similaridade_constroiDeCompras(sim, compras, m, algoritmo);
}

/* ------------------------------------------------------------------ */
/* Retorna o indice do cliente mais similar ao cliente idx             */
/* Ignora o proprio cliente e clientes com similaridade == 1.0        */
/* ------------------------------------------------------------------ */
int similaridade_maisProximo(Similaridade *sim, int idx) {
    int    melhor     = -1;
    double menor_dist = 2.0;

    for (int j = 0; j < sim->n_clientes; j++) {
        if (j == idx) continue;
        if (sim->S[idx][j] >= 1.0) continue;

        if (sim->S[idx][j] < menor_dist) {
            menor_dist = sim->S[idx][j];
            melhor     = j;
        }
    }
    return melhor;
}

/* ------------------------------------------------------------------ */
/* Libera a memoria da estrutura                                        */
/* ------------------------------------------------------------------ */
void similaridade_libera(Similaridade *sim) {
    sim->matriz_compras.clear();
    sim->S.clear();
}

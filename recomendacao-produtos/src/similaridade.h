#ifndef SIMILARIDADE_H
#define SIMILARIDADE_H

#include <vector>
#include "lista_compras.h"

/*
 * Modulo Similaridade - Atividades 2 e 4
 *
 * matriz_compras : A[n_clientes][n_produtos] — 1 se comprou, 0 se nao
 * S              : S[n_clientes][n_clientes] — similaridade de Jaccard
 */
typedef struct {
    std::vector<std::vector<int>>    matriz_compras;
    std::vector<std::vector<double>> S;
    int n_clientes;
    int n_produtos;
} Similaridade;

/*
 * Selecao do algoritmo de multiplicacao usado na Atividade 4:
 *  ALG_PADRAO   -> constroi a transposta A^T e multiplica A x A^T
 *  ALG_ADAPTADO -> usa a simetria da matriz de intersecao e calcula
 *                  apenas a metade superior (i <= j)
 */
#define ALG_PADRAO   0
#define ALG_ADAPTADO 1

/* Constroi a matriz de compras e calcula S usando o algoritmo escolhido */
void similaridade_constroi(Similaridade *sim, ListaCompras *lc, int algoritmo);

/* Versao que recebe as compras direto (cada cliente = lista de indices de
 * produtos). Usada pela integracao com Python (Atividade 5). */
void similaridade_constroiDeCompras(Similaridade *sim,
                                    const std::vector<std::vector<int>> &compras,
                                    int n_produtos, int algoritmo);

/* Retorna o indice do cliente mais similar a idx (excluindo ele mesmo) */
int similaridade_maisProximo(Similaridade *sim, int idx);

/* Libera a memoria da estrutura */
void similaridade_libera(Similaridade *sim);

#endif

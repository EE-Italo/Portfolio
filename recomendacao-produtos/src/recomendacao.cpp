#include <stdio.h>
#include <math.h>
#include "recomendacao.h"

/* Valor minimo usado no lugar de 0.0 antes de tirar o log, ja que
 * log(0) nao e definido */
#define RANKING_EPSILON 1e-300

/* ------------------------------------------------------------------ */
/* Ordena o vetor de ranking em ordem crescente usando bubble sort     */
/* Bubble sort foi escolhido por ser simples e facil de entender       */
/* ------------------------------------------------------------------ */
static void ordenaRanking(std::vector<ItemRanking> *R) {
    int n = (int)R->size();

    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if ((*R)[j].ranking > (*R)[j+1].ranking) {
                ItemRanking temp = (*R)[j];
                (*R)[j]         = (*R)[j+1];
                (*R)[j+1]       = temp;
            }
        }
    }
}

/* ------------------------------------------------------------------ */
/* Algoritmo de recomendacao - Secao 2.3 do documento                  */
/*                                                                      */
/* Passo 1: identifica vizinhos L (clientes com S[c][s] < 1.0)        */
/* Passo 2: inicializa R[p] = 1.0 para todos os produtos              */
/* Passo 3: para cada vizinho s e produto p comprado por s             */
/*          e nao comprado por c: R[p] *= S[c][s]                      */
/* Passo 4: ordena R (nao decrescente) e devolve o vetor              */
/*                                                                      */
/* Usa a matriz de compras densa que ja esta dentro de Similaridade,   */
/* logo nao depende da ListaCompras.                                   */
/* ------------------------------------------------------------------ */
std::vector<ItemRanking> recomendacao_ranking(Similaridade *sim, int idx_cliente) {

    int n_produtos = sim->n_produtos;
    int n_clientes = sim->n_clientes;

    /* Passo 1: identifica vizinhos */
    std::vector<int> vizinhos;
    for (int s = 0; s < n_clientes; s++) {
        if (s == idx_cliente) continue;
        if (sim->S[idx_cliente][s] < 1.0)
            vizinhos.push_back(s);
    }

    /* Passo 2: inicializa vetor de ranking com 1.0 */
    std::vector<ItemRanking> R(n_produtos);
    for (int p = 0; p < n_produtos; p++) {
        R[p].indice  = p;
        R[p].ranking = 1.0;
    }

    /* Produtos que o cliente ja comprou (linha da matriz de compras) */
    const std::vector<int> &ja_comprou = sim->matriz_compras[idx_cliente];

    /* Passo 3: atualiza o ranking multiplicando pela similaridade */
    for (int s : vizinhos) {
        double sim_cs = sim->S[idx_cliente][s];
        const std::vector<int> &compras_s = sim->matriz_compras[s];
        for (int p = 0; p < n_produtos; p++) {
            if (compras_s[p] && !ja_comprou[p])
                R[p].ranking *= sim_cs;
        }
    }

    /* Produtos ja comprados recebem ranking 2.0 para ficarem no final */
    for (int p = 0; p < n_produtos; p++) {
        if (ja_comprou[p])
            R[p].ranking = 2.0;
    }

    /* Passo 4: ordena pelo ranking */
    ordenaRanking(&R);
    return R;
}

/* ------------------------------------------------------------------ */
/* Gera e exibe os k produtos recomendados para o cliente idx_cliente  */
/* ------------------------------------------------------------------ */
void recomendacao_exibe(ListaCompras *lc, Similaridade *sim,
                        int idx_cliente, int k) {

    int n_produtos = sim->n_produtos;
    std::vector<ItemRanking> R = recomendacao_ranking(sim, idx_cliente);

    /* Menor e maior log10(ranking) entre os produtos elegiveis, usados
     * para normalizar o valor exibido numa escala de compatibilidade de
     * 0 a 100%. Escala logaritmica e usada porque o ranking e um produto
     * de similaridades e pode variar por muitas ordens de grandeza. */
    double menor_log = 0.0, maior_log = 0.0;
    bool tem_candidato = false;
    for (int p = 0; p < n_produtos; p++) {
        if (R[p].ranking >= 2.0) continue;

        double valor = R[p].ranking;
        if (valor < RANKING_EPSILON) valor = RANKING_EPSILON;
        double logv = log10(valor);

        if (!tem_candidato || logv < menor_log) menor_log = logv;
        if (!tem_candidato || logv > maior_log) maior_log = logv;
        tem_candidato = true;
    }

    printf("Top %d recomendacoes para cliente %s:\n",
           k, lc->clientes[idx_cliente].codigo);

    int exibidos = 0;
    for (int i = 0; i < n_produtos && exibidos < k; i++) {
        if (R[i].ranking < 2.0) {
            double valor = R[i].ranking;
            if (valor < RANKING_EPSILON) valor = RANKING_EPSILON;
            double logv = log10(valor);

            double compatibilidade = 100.0;
            if (maior_log > menor_log)
                compatibilidade = 100.0 * (1.0 - (logv - menor_log) /
                                                  (maior_log - menor_log));

            printf("  %d. [%s] %s  (compatibilidade: %.1f%%)\n",
                   exibidos + 1,
                   lc->produtos[R[i].indice].codigo,
                   lc->produtos[R[i].indice].nome,
                   compatibilidade);
            exibidos++;
        }
    }

    if (exibidos == 0)
        printf("  Nenhuma recomendacao disponivel.\n");

    printf("\n");
}

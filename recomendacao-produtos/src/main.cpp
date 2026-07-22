#include <stdio.h>
#include <time.h>
#include "lista_compras.h"
#include "similaridade.h"
#include "recomendacao.h"

int main() {

    /* -------------------------------------------------- */
    /* Pede o numero do cluster e monta o caminho do CSV  */
    /* -------------------------------------------------- */
    int num_cluster;
    printf("Digite o numero do cluster (0-20): ");
    scanf("%d", &num_cluster);

    char caminho_csv[100];
    sprintf(caminho_csv, "dados/dados_venda_cluster_%d.csv", num_cluster);
    printf("\nArquivo: %s\n\n", caminho_csv);

    /* -------------------------------------------------- */
    /* Pede o numero de recomendacoes k                   */
    /* -------------------------------------------------- */
    int k;
    printf("Digite o numero de recomendacoes (k): ");
    scanf("%d", &k);

    /* ================================================== */
    /* A1 - Carrega ListaCompras                          */
    /* ================================================== */
    printf("\n========================================\n");
    printf("  A1 - Carregando ListaCompras\n");
    printf("========================================\n");

    ListaCompras lc;
    if (!listaCompras_carrega(&lc, caminho_csv)) {
        fprintf(stderr, "Erro ao abrir o arquivo: %s\n", caminho_csv);
        return 1;
    }

    printf("Clientes carregados : %d\n", (int)lc.clientes.size());
    printf("Produtos carregados : %d\n\n", (int)lc.produtos.size());

    /* Usa os 3 primeiros clientes do arquivo como exemplo */
    int n_testes = (int)lc.clientes.size();
    if (n_testes > 3) n_testes = 3;

    printf("--- Produtos por cliente ---\n");
    for (int i = 0; i < n_testes; i++)
        listaCompras_exibeProdutos(&lc, i);

    /* ================================================== */
    /* A2 - Calcula Similaridade                          */
    /* ================================================== */
    printf("========================================\n");
    printf("  A2 - Calculando Similaridade\n");
    printf("========================================\n");

    Similaridade sim;
    similaridade_constroi(&sim, &lc, ALG_ADAPTADO);

    printf("Matriz calculada (%dx%d)\n\n", sim.n_clientes, sim.n_clientes);

    /* ================================================== */
    /* A4 - Compara os dois algoritmos de multiplicacao   */
    /* ================================================== */
    printf("========================================\n");
    printf("  A4 - Comparando algoritmos de multiplicacao\n");
    printf("========================================\n");

    /* Mede o tempo de construir a matriz com o algoritmo padrao */
    Similaridade sim_padrao;
    clock_t ini_padrao = clock();
    similaridade_constroi(&sim_padrao, &lc, ALG_PADRAO);
    clock_t fim_padrao = clock();

    /* Mede o tempo de construir a matriz com o algoritmo adaptado */
    Similaridade sim_adaptado;
    clock_t ini_adaptado = clock();
    similaridade_constroi(&sim_adaptado, &lc, ALG_ADAPTADO);
    clock_t fim_adaptado = clock();

    double tempo_padrao   = (double)(fim_padrao - ini_padrao) / CLOCKS_PER_SEC;
    double tempo_adaptado = (double)(fim_adaptado - ini_adaptado) / CLOCKS_PER_SEC;

    printf("Tamanho da matriz  : %d clientes x %d produtos\n",
           sim.n_clientes, sim.n_produtos);
    printf("Algoritmo padrao   : %.4f s\n", tempo_padrao);
    printf("Algoritmo adaptado : %.4f s\n", tempo_adaptado);
    if (tempo_adaptado > 0.0)
        printf("Aceleracao (speedup): %.2fx\n", tempo_padrao / tempo_adaptado);
    printf("\n");

    similaridade_libera(&sim_padrao);
    similaridade_libera(&sim_adaptado);

    printf("--- Cliente mais similar ---\n");
    for (int i = 0; i < n_testes; i++) {
        int prox = similaridade_maisProximo(&sim, i);
        if (prox == -1) {
            printf("Cliente %s: nenhum vizinho encontrado.\n\n",
                   lc.clientes[i].codigo);
        } else {
            printf("Cliente %s -> mais similar: %s | S = %.4f\n\n",
                   lc.clientes[i].codigo,
                   lc.clientes[prox].codigo,
                   sim.S[i][prox]);
        }
    }

    /* ================================================== */
    /* A3 - Recomendacoes                                 */
    /* ================================================== */
    printf("========================================\n");
    printf("  A3 - Recomendacoes (k=%d)\n", k);
    printf("========================================\n\n");

    for (int i = 0; i < n_testes; i++)
        recomendacao_exibe(&lc, &sim, i, k);

    /* -------------------------------------------------- */
    /* Libera memoria                                     */
    /* -------------------------------------------------- */
    similaridade_libera(&sim);
    listaCompras_libera(&lc);

    printf("Pressione Enter para sair...\n");
    getchar();
    getchar();

    return 0;
}

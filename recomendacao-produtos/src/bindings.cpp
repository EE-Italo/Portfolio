/* ------------------------------------------------------------------ */
/* Atividade 5 - Integracao Python <-> C++ com pybind11                 */
/*                                                                      */
/* Este arquivo e a "ponte" (binding) entre o Python e os modulos de    */
/* recomendacao escritos em C++ (Atividades 2, 3 e 4). Ele apenas       */
/* recebe os dados vindos do Python, chama as funcoes ja existentes e   */
/* devolve o resultado. Toda a logica continua nos modulos C++ (STL).   */
/*                                                                      */
/* O pybind11 converte automaticamente:                                 */
/*   list de list de int  <-> std::vector<std::vector<int>>             */
/*   list de tuple        <-> std::vector<std::pair<int,double>>        */
/* ------------------------------------------------------------------ */
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <vector>
#include <utility>
#include <ctime>

#include "similaridade.h"
#include "recomendacao.h"

namespace py = pybind11;

/* ------------------------------------------------------------------ */
/* Retorna o indice do cliente mais similar ao cliente idx.            */
/* compras[i] = lista de indices de produtos comprados pelo cliente i. */
/* ------------------------------------------------------------------ */
int cliente_mais_similar(const std::vector<std::vector<int>> &compras,
                         int n_produtos, int idx, int algoritmo) {
    Similaridade sim;
    similaridade_constroiDeCompras(&sim, compras, n_produtos, algoritmo);
    int prox = similaridade_maisProximo(&sim, idx);
    similaridade_libera(&sim);
    return prox;
}

/* ------------------------------------------------------------------ */
/* Retorna os k produtos mais bem ranqueados para o cliente idx.       */
/* Cada item da lista e uma tupla (indice_produto, ranking).           */
/* ------------------------------------------------------------------ */
std::vector<std::pair<int, double>> recomenda(
        const std::vector<std::vector<int>> &compras,
        int n_produtos, int idx, int k, int algoritmo) {

    Similaridade sim;
    similaridade_constroiDeCompras(&sim, compras, n_produtos, algoritmo);

    std::vector<ItemRanking> R = recomendacao_ranking(&sim, idx);
    similaridade_libera(&sim);

    std::vector<std::pair<int, double>> resultado;
    for (int i = 0; i < (int)R.size() && (int)resultado.size() < k; i++) {
        /* ranking 2.0 marca produto ja comprado, que nao e recomendado */
        if (R[i].ranking < 2.0)
            resultado.push_back(std::make_pair(R[i].indice, R[i].ranking));
    }
    return resultado;
}

/* ------------------------------------------------------------------ */
/* Mede (em segundos) o tempo de construir a matriz de similaridade    */
/* usando o algoritmo escolhido. Usado para comparar padrao x adaptado.*/
/* ------------------------------------------------------------------ */
double tempo_similaridade(const std::vector<std::vector<int>> &compras,
                          int n_produtos, int algoritmo) {
    Similaridade sim;
    clock_t ini = clock();
    similaridade_constroiDeCompras(&sim, compras, n_produtos, algoritmo);
    clock_t fim = clock();
    similaridade_libera(&sim);
    return (double)(fim - ini) / CLOCKS_PER_SEC;
}

/* ------------------------------------------------------------------ */
/* Definicao do modulo Python. O nome "recomendador" e o que sera      */
/* usado com "import recomendador" no Python.                          */
/* ------------------------------------------------------------------ */
PYBIND11_MODULE(recomendador, m) {
    m.doc() = "Modulo de recomendacao (C++) exposto ao Python via pybind11";

    m.def("cliente_mais_similar", &cliente_mais_similar,
          py::arg("compras"), py::arg("n_produtos"),
          py::arg("idx"), py::arg("algoritmo") = ALG_ADAPTADO,
          "Indice do cliente mais similar ao cliente idx");

    m.def("recomenda", &recomenda,
          py::arg("compras"), py::arg("n_produtos"), py::arg("idx"),
          py::arg("k"), py::arg("algoritmo") = ALG_ADAPTADO,
          "Lista de (indice_produto, ranking) dos k melhores produtos");

    m.def("tempo_similaridade", &tempo_similaridade,
          py::arg("compras"), py::arg("n_produtos"),
          py::arg("algoritmo") = ALG_ADAPTADO,
          "Tempo (s) para construir a matriz de similaridade");

    /* Constantes de selecao de algoritmo, iguais as do C++ */
    m.attr("ALG_PADRAO")   = (int)ALG_PADRAO;
    m.attr("ALG_ADAPTADO") = (int)ALG_ADAPTADO;
}

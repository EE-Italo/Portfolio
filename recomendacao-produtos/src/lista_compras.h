#ifndef LISTA_COMPRAS_H
#define LISTA_COMPRAS_H

#include <vector>
#include <list>

/*
 * Estrutura que representa um cliente
 */
typedef struct {
    char codigo[32];
} Cliente;

/*
 * Estrutura que representa um produto
 */
typedef struct {
    char codigo[32];
    char nome[256];
} Produto;

/*
 * Par codigo -> indice, usado como entrada de mapa simples
 */
typedef struct {
    char codigo[32];
    int  indice;
} EntradaMapa;

/*
 * Modulo ListaCompras - Atividade 1
 *
 * clientes      : vetor com todos os clientes carregados do CSV
 * produtos      : vetor com todos os produtos carregados do CSV
 * mapa_clientes : vetor de pares (codigo -> indice) para busca de clientes
 * mapa_produtos : vetor de pares (codigo -> indice) para busca de produtos
 * compras       : vetor onde compras[i] e a lista de indices dos
 *                 produtos comprados pelo cliente i
 */
typedef struct {
    std::vector<Cliente>        clientes;
    std::vector<Produto>        produtos;
    std::vector<EntradaMapa>    mapa_clientes;
    std::vector<EntradaMapa>    mapa_produtos;
    std::vector<std::list<int>> compras;
} ListaCompras;

/* Busca um codigo no mapa e retorna o indice, ou -1 se nao encontrado */
int listaCompras_buscaMapa(std::vector<EntradaMapa> *mapa, const char *codigo);

/* Le o CSV e preenche a estrutura. Retorna 1 em sucesso, 0 em falha */
int listaCompras_carrega(ListaCompras *lc, const char *caminho);

/* Exibe os produtos comprados pelo cliente de indice idx */
void listaCompras_exibeProdutos(ListaCompras *lc, int idx_cliente);

/* Libera a memoria da estrutura */
void listaCompras_libera(ListaCompras *lc);

#endif

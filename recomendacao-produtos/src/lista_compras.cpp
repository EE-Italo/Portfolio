#include <stdio.h>
#include <string.h>
#include "lista_compras.h"

/* ------------------------------------------------------------------ */
/* Busca um codigo no mapa e retorna o indice interno, ou -1           */
/* ------------------------------------------------------------------ */
int listaCompras_buscaMapa(std::vector<EntradaMapa> *mapa, const char *codigo) {
    for (int i = 0; i < (int)mapa->size(); i++) {
        if (strcmp((*mapa)[i].codigo, codigo) == 0)
            return (*mapa)[i].indice;
    }
    return -1;
}

/* ------------------------------------------------------------------ */
/* Remove espacos e \r\n do final de uma string                        */
/* ------------------------------------------------------------------ */
static void trim(char *s) {
    int len = strlen(s);
    while (len > 0 && (s[len-1] == ' ' || s[len-1] == '\r' || s[len-1] == '\n'))
        s[--len] = '\0';
}

/* ------------------------------------------------------------------ */
/* Detecta o separador do arquivo (primeira virgula ou ponto-e-virgula)*/
/* ------------------------------------------------------------------ */
static char detectaSeparador(const char *caminho) {
    FILE *f = fopen(caminho, "r");
    if (!f) return ',';

    char linha[512];
    fgets(linha, sizeof(linha), f);
    fclose(f);

    for (int i = 0; linha[i] != '\0'; i++) {
        if (linha[i] == ';') return ';';
        if (linha[i] == ',') return ',';
    }
    return ',';
}

/* ------------------------------------------------------------------ */
/* Carrega o CSV em duas passagens                                      */
/* Passagem 1: registra clientes e produtos unicos                     */
/* Passagem 2: monta a lista de compras de cada cliente                */
/* ------------------------------------------------------------------ */
int listaCompras_carrega(ListaCompras *lc, const char *caminho) {
    FILE *f = fopen(caminho, "r");
    if (!f) {
        fprintf(stderr, "Erro ao abrir o arquivo: %s\n", caminho);
        return 0;
    }

    char sep = detectaSeparador(caminho);

    char linha[512];
    char data[16], cod_cli[32], cod_prod[32], nome_prod[256];

    /* Pula o cabecalho */
    fgets(linha, sizeof(linha), f);

    /* ---- 1a passagem: registra clientes e produtos unicos ---- */
    while (fgets(linha, sizeof(linha), f)) {

        /* Substitui o separador por tabulacao para facilitar o sscanf */
        for (int i = 0; linha[i]; i++)
            if (linha[i] == sep) linha[i] = '\t';

        if (sscanf(linha, "%15s\t%31s\t%31s\t%255[^\n\r]",
                   data, cod_cli, cod_prod, nome_prod) < 4)
            continue;

        trim(cod_cli);
        trim(cod_prod);
        trim(nome_prod);

        /* Adiciona cliente se ainda nao existir */
        if (listaCompras_buscaMapa(&lc->mapa_clientes, cod_cli) == -1) {
            int novo_indice = (int)lc->clientes.size();

            Cliente c;
            strncpy(c.codigo, cod_cli, sizeof(c.codigo));
            lc->clientes.push_back(c);

            EntradaMapa entrada;
            strncpy(entrada.codigo, cod_cli, sizeof(entrada.codigo));
            entrada.indice = novo_indice;
            lc->mapa_clientes.push_back(entrada);

            /* Cria lista de compras vazia para esse cliente */
            lc->compras.push_back(std::list<int>());
        }

        /* Adiciona produto se ainda nao existir */
        if (listaCompras_buscaMapa(&lc->mapa_produtos, cod_prod) == -1) {
            int novo_indice = (int)lc->produtos.size();

            Produto p;
            strncpy(p.codigo, cod_prod, sizeof(p.codigo));
            strncpy(p.nome,   nome_prod, sizeof(p.nome));
            lc->produtos.push_back(p);

            EntradaMapa entrada;
            strncpy(entrada.codigo, cod_prod, sizeof(entrada.codigo));
            entrada.indice = novo_indice;
            lc->mapa_produtos.push_back(entrada);
        }
    }

    /* ---- 2a passagem: monta lista de compras ---- */
    rewind(f);
    fgets(linha, sizeof(linha), f); /* pula cabecalho */

    while (fgets(linha, sizeof(linha), f)) {

        for (int i = 0; linha[i]; i++)
            if (linha[i] == sep) linha[i] = '\t';

        if (sscanf(linha, "%15s\t%31s\t%31s\t%255[^\n\r]",
                   data, cod_cli, cod_prod, nome_prod) < 4)
            continue;

        trim(cod_cli);
        trim(cod_prod);

        int idx_cli  = listaCompras_buscaMapa(&lc->mapa_clientes, cod_cli);
        int idx_prod = listaCompras_buscaMapa(&lc->mapa_produtos, cod_prod);

        if (idx_cli == -1 || idx_prod == -1) continue;

        /* Evita adicionar o mesmo produto duas vezes para o mesmo cliente */
        bool ja_comprou = false;
        for (int p : lc->compras[idx_cli]) {
            if (p == idx_prod) { ja_comprou = true; break; }
        }

        if (!ja_comprou)
            lc->compras[idx_cli].push_back(idx_prod);
    }

    fclose(f);
    return 1;
}

/* ------------------------------------------------------------------ */
/* Exibe os produtos comprados pelo cliente de indice idx_cliente      */
/* ------------------------------------------------------------------ */
void listaCompras_exibeProdutos(ListaCompras *lc, int idx_cliente) {
    printf("Produtos comprados pelo cliente %s:\n",
           lc->clientes[idx_cliente].codigo);

    int n = 1;
    for (int idx_prod : lc->compras[idx_cliente]) {
        printf("  %d. [%s] %s\n", n++,
               lc->produtos[idx_prod].codigo,
               lc->produtos[idx_prod].nome);
    }
    printf("\n");
}

/* ------------------------------------------------------------------ */
/* Libera a memoria da estrutura                                        */
/* ------------------------------------------------------------------ */
void listaCompras_libera(ListaCompras *lc) {
    lc->clientes.clear();
    lc->produtos.clear();
    lc->mapa_clientes.clear();
    lc->mapa_produtos.clear();
    lc->compras.clear();
}

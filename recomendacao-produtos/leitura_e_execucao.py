# ------------------------------------------------------------------
# Atividade 5 - Testador em Python
#
# Reproduz a Atividade 1 (leitura do CSV e montagem da ListaCompras)
# diretamente em Python e usa o modulo C++ "recomendador" (Atividades
# 2, 3 e 4) para calcular similaridade e recomendacoes.
#
# Antes de rodar, compile o modulo C++ com:
#     python setup.py build_ext --inplace
# ------------------------------------------------------------------
import recomendador


# ------------------------------------------------------------------
# Atividade 1 em Python: le o CSV e monta as estruturas de dados
# ------------------------------------------------------------------
def detecta_separador(caminho):
    """Retorna ';' ou ',' conforme o primeiro separador da 1a linha."""
    with open(caminho, "r", encoding="latin-1") as f:
        primeira = f.readline()
    for ch in primeira:
        if ch == ";":
            return ";"
        if ch == ",":
            return ","
    return ","


def carrega_lista_compras(caminho):
    """
    Le o CSV e devolve:
      clientes      : lista com os codigos dos clientes
      produtos      : lista de (codigo, nome) dos produtos
      mapa_clientes : dicionario codigo -> indice interno
      mapa_produtos : dicionario codigo -> indice interno
      compras       : lista onde compras[i] e a lista de indices de
                      produtos comprados pelo cliente i
    """
    sep = detecta_separador(caminho)

    clientes = []
    produtos = []
    mapa_clientes = {}
    mapa_produtos = {}
    compras = []

    with open(caminho, "r", encoding="latin-1") as f:
        f.readline()  # pula o cabecalho

        for linha in f:
            # separa em no maximo 4 campos; o nome fica com o "resto"
            campos = linha.split(sep, 3)
            if len(campos) < 4:
                continue

            cod_cli = campos[1].strip()
            cod_prod = campos[2].strip()
            nome_prod = campos[3].strip()

            # registra o cliente se ainda nao existir
            if cod_cli not in mapa_clientes:
                mapa_clientes[cod_cli] = len(clientes)
                clientes.append(cod_cli)
                compras.append([])

            # registra o produto se ainda nao existir
            if cod_prod not in mapa_produtos:
                mapa_produtos[cod_prod] = len(produtos)
                produtos.append((cod_prod, nome_prod))

            idx_cli = mapa_clientes[cod_cli]
            idx_prod = mapa_produtos[cod_prod]

            # evita repetir o mesmo produto para o mesmo cliente
            if idx_prod not in compras[idx_cli]:
                compras[idx_cli].append(idx_prod)

    return clientes, produtos, mapa_clientes, mapa_produtos, compras


def main():
    num_cluster = int(input("Digite o numero do cluster (0-20): "))
    caminho = "dados/dados_venda_cluster_%d.csv" % num_cluster
    print("\nArquivo: %s\n" % caminho)

    k = int(input("Digite o numero de recomendacoes (k): "))

    # -------------------- A1 --------------------
    print("\n========================================")
    print("  A1 - Carregando ListaCompras (Python)")
    print("========================================")

    (clientes, produtos, mapa_clientes,
     mapa_produtos, compras) = carrega_lista_compras(caminho)

    n_produtos = len(produtos)
    print("Clientes carregados : %d" % len(clientes))
    print("Produtos carregados : %d\n" % n_produtos)

    n_testes = min(3, len(clientes))

    print("--- Produtos por cliente ---")
    for i in range(n_testes):
        print("Produtos comprados pelo cliente %s:" % clientes[i])
        for n, idx_prod in enumerate(compras[i], start=1):
            cod, nome = produtos[idx_prod]
            print("  %d. [%s] %s" % (n, cod, nome))
        print()

    # -------------------- A2 (C++) --------------------
    print("========================================")
    print("  A2 - Cliente mais similar (modulo C++)")
    print("========================================")
    for i in range(n_testes):
        prox = recomendador.cliente_mais_similar(compras, n_produtos, i)
        if prox == -1:
            print("Cliente %s: nenhum vizinho encontrado.\n" % clientes[i])
        else:
            print("Cliente %s -> mais similar: %s\n"
                  % (clientes[i], clientes[prox]))

    # -------------------- A4 (C++) --------------------
    print("========================================")
    print("  A4 - Comparando algoritmos (modulo C++)")
    print("========================================")
    t_padrao = recomendador.tempo_similaridade(
        compras, n_produtos, recomendador.ALG_PADRAO)
    t_adaptado = recomendador.tempo_similaridade(
        compras, n_produtos, recomendador.ALG_ADAPTADO)
    print("Tamanho da matriz  : %d clientes x %d produtos"
          % (len(clientes), n_produtos))
    print("Algoritmo padrao   : %.4f s" % t_padrao)
    print("Algoritmo adaptado : %.4f s" % t_adaptado)
    if t_adaptado > 0.0:
        print("Aceleracao (speedup): %.2fx" % (t_padrao / t_adaptado))
    print()

    # -------------------- A3 (C++) --------------------
    print("========================================")
    print("  A3 - Recomendacoes (k=%d) (modulo C++)" % k)
    print("========================================\n")
    for i in range(n_testes):
        itens = recomendador.recomenda(compras, n_produtos, i, k)
        print("Top %d recomendacoes para cliente %s:" % (k, clientes[i]))
        if not itens:
            print("  Nenhuma recomendacao disponivel.")
        for pos, (idx_prod, ranking) in enumerate(itens, start=1):
            cod, nome = produtos[idx_prod]
            print("  %d. [%s] %s  (ranking: %.6e)" % (pos, cod, nome, ranking))
        print()


if __name__ == "__main__":
    main()

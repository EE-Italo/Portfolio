"""
Extrai a série histórica completa do IPCA (tabela SIDRA 1737, nível Brasil)
via API de Agregados do IBGE, limpa os dados e salva em CSV.
"""

import requests
import pandas as pd

TABELA = 1737
LOCALIDADE = "N1[1]"  # Brasil

VARIAVEIS = {
    "2266": "numero_indice",
    "63": "variacao_mensal",
    "2263": "variacao_acum_3m",
    "2264": "variacao_acum_6m",
    "69": "variacao_acum_ano",
    "2265": "variacao_acum_12m",
}

URL = (
    f"https://servicodados.ibge.gov.br/api/v3/agregados/{TABELA}"
    f"/periodos/all/variaveis/{'|'.join(VARIAVEIS)}"
)


def extrair():
    resp = requests.get(URL, params={"localidades": LOCALIDADE}, timeout=30)
    resp.raise_for_status()
    return resp.json()


def limpar(dados):
    colunas = {}
    for bloco in dados:
        nome_coluna = VARIAVEIS[bloco["id"]]
        serie = bloco["resultados"][0]["series"][0]["serie"]
        colunas[nome_coluna] = pd.Series(serie)

    df = pd.DataFrame(colunas)
    df.index.name = "periodo"
    df = df.reset_index()

    df["ano"] = df["periodo"].str[:4].astype(int)
    df["mes"] = df["periodo"].str[4:6].astype(int)

    for col in VARIAVEIS.values():
        df[col] = pd.to_numeric(df[col], errors="coerce")

    df = df.sort_values("periodo").reset_index(drop=True)
    colunas_finais = ["periodo", "ano", "mes"] + list(VARIAVEIS.values())
    return df[colunas_finais]


def salvar(df, caminho):
    df.to_csv(caminho, index=False, encoding="utf-8-sig")


if __name__ == "__main__":
    dados = extrair()
    df = limpar(dados)
    salvar(df, "ipca.csv")
    print(f"{len(df)} registros salvos em ipca.csv")
    print(df.tail())

"""
Mantém um log de previsões reais (fora da amostra) da variação mensal do
IPCA: a cada execução, preenche o valor real de previsões antigas cujo mês
já foi divulgado pelo IBGE, e — se o próximo mês ainda não tiver previsão
registrada — gera uma nova usando só os dados disponíveis até agora.

Diferente do backtest em prever_ipca.py (que simula previsões passadas),
aqui a previsão é gravada ANTES de o IBGE divulgar o dado real, então o
log mede precisão de verdade ao longo do tempo, sem viés de retrospecto.
Uma previsão, uma vez registrada, nunca é reescrita.
"""

import os

import pandas as pd

from modelos_ipca import carregar, construir_features, prever_naive, prever_sarima, prever_ml

LOG = "previsoes_log.csv"


def carregar_log():
    if os.path.exists(LOG):
        return pd.read_csv(LOG, parse_dates=["mes_previsto"])
    return pd.DataFrame(columns=["mes_previsto", "data_geracao", "naive", "sarima", "ml", "real"])


def reconciliar(log, serie):
    pendentes = log["real"].isna()
    valores_reais = log.loc[pendentes, "mes_previsto"].map(serie)
    log.loc[pendentes, "real"] = valores_reais
    return log


def gerar_previsao(df, proximo_mes):
    serie = df["variacao_mensal"]
    feats = construir_features(serie)

    nova_linha = pd.DataFrame({"mes": [proximo_mes.month]}, index=[proximo_mes])
    for lag in (1, 2, 3, 6, 12):
        nova_linha[f"lag_{lag}"] = serie.iloc[-lag]
    nova_linha["media_movel_3"] = serie.iloc[-3:].mean()
    nova_linha["media_movel_12"] = serie.iloc[-12:].mean()

    colunas = feats.drop(columns="y").columns
    x_treino, y_treino = feats[colunas], feats["y"]
    x_alvo = nova_linha[colunas]

    return {
        "mes_previsto": proximo_mes,
        "data_geracao": pd.Timestamp.utcnow().tz_localize(None),
        "naive": prever_naive(serie),
        "sarima": prever_sarima(serie),
        "ml": prever_ml(x_treino, y_treino, x_alvo),
        "real": float("nan"),
    }


def atualizar():
    df = carregar()
    serie = df["variacao_mensal"]
    log = carregar_log()

    log = reconciliar(log, serie)

    proximo_mes = serie.index[-1] + pd.DateOffset(months=1)
    ja_registrado = (log["mes_previsto"] == proximo_mes).any()

    if not ja_registrado:
        nova = gerar_previsao(df, proximo_mes)
        nova_linha = pd.DataFrame([nova])
        log = nova_linha if log.empty else pd.concat([log, nova_linha], ignore_index=True)
        print(f"Previsão registrada para {proximo_mes:%Y-%m}: "
              f"naive={nova['naive']:.2f}  sarima={nova['sarima']:.2f}  ml={nova['ml']:.2f}")
    else:
        print(f"{proximo_mes:%Y-%m} já tem previsão registrada — nada a fazer.")

    log = log.sort_values("mes_previsto").reset_index(drop=True)
    log.to_csv(LOG, index=False)
    return log


if __name__ == "__main__":
    atualizar()

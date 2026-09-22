"""
Funções de modelagem compartilhadas entre o backtest (prever_ipca.py) e o
registro de previsões reais (registrar_previsao.py): um baseline ingênuo,
SARIMA e um modelo de ML (gradient boosting com features de defasagem).
"""

import pandas as pd
from sklearn.ensemble import HistGradientBoostingRegressor
from statsmodels.tsa.statespace.sarimax import SARIMAX

# A partir do Plano Real (jul/1994) — antes disso é outro regime de inflação
# (hiperinflação), cujo histórico não ajuda a prever o cenário atual.
INICIO_SERIE_MODERNA = "1994-07-01"

LAGS = [1, 2, 3, 6, 12]


def carregar(caminho="ipca.csv"):
    df = pd.read_csv(caminho)
    df["data"] = pd.to_datetime(df["periodo"], format="%Y%m")
    df = df.set_index("data").asfreq("MS")
    return df.loc[INICIO_SERIE_MODERNA:]


def construir_features(serie):
    df = pd.DataFrame({"y": serie})
    for lag in LAGS:
        df[f"lag_{lag}"] = serie.shift(lag)
    df["media_movel_3"] = serie.shift(1).rolling(3).mean()
    df["media_movel_12"] = serie.shift(1).rolling(12).mean()
    df["mes"] = df.index.month
    return df.dropna()


def prever_naive(historico):
    return historico.iloc[-1]


def prever_sarima(historico):
    modelo = SARIMAX(
        historico, order=(1, 0, 1), seasonal_order=(1, 0, 1, 12),
        enforce_stationarity=False, enforce_invertibility=False,
    )
    resultado = modelo.fit(disp=False)
    return resultado.forecast(1).iloc[0]


def prever_ml(x_treino, y_treino, x_alvo):
    modelo = HistGradientBoostingRegressor(max_depth=3, random_state=0)
    modelo.fit(x_treino, y_treino)
    return modelo.predict(x_alvo)[0]

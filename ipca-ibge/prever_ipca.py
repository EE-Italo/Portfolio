"""
Compara métodos de previsão da variação mensal do IPCA (1 mês à frente)
com validação walk-forward (expanding window): um baseline ingênuo,
SARIMA e um modelo de ML (gradient boosting com features de defasagem).
"""

import pandas as pd
from sklearn.ensemble import HistGradientBoostingRegressor
from sklearn.metrics import mean_absolute_error, mean_squared_error
from statsmodels.tsa.statespace.sarimax import SARIMAX

JANELA_TESTE = 60  # últimos 5 anos, previsão de 1 mês à frente em cada um
LAGS = [1, 2, 3, 6, 12]


def carregar():
    df = pd.read_csv("ipca.csv")
    df["data"] = pd.to_datetime(df["periodo"], format="%Y%m")
    return df.set_index("data").asfreq("MS")


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


def backtest(df):
    serie = df["variacao_mensal"]
    feats = construir_features(serie)

    linhas = []
    for i in range(len(feats) - JANELA_TESTE, len(feats)):
        data_alvo = feats.index[i]
        y_real = feats["y"].iloc[i]

        historico = serie.loc[:data_alvo].iloc[:-1]
        x_treino = feats.drop(columns="y").iloc[:i]
        y_treino = feats["y"].iloc[:i]
        x_alvo = feats.drop(columns="y").iloc[[i]]

        linhas.append({
            "data": data_alvo,
            "real": y_real,
            "naive": prever_naive(historico),
            "sarima": prever_sarima(historico),
            "ml": prever_ml(x_treino, y_treino, x_alvo),
        })

    return pd.DataFrame(linhas).set_index("data")


def avaliar(resultados):
    linhas = []
    for modelo in ("naive", "sarima", "ml"):
        mae = mean_absolute_error(resultados["real"], resultados[modelo])
        rmse = mean_squared_error(resultados["real"], resultados[modelo]) ** 0.5
        linhas.append({"modelo": modelo, "mae": mae, "rmse": rmse})
    return pd.DataFrame(linhas)


if __name__ == "__main__":
    df = carregar()
    resultados = backtest(df)
    resultados.to_csv("backtest_previsao.csv")

    metricas = avaliar(resultados)
    metricas.to_csv("metricas_previsao.csv", index=False)
    print(f"Backtest com {JANELA_TESTE} meses (1 passo à frente, expanding window):\n")
    print(metricas.to_string(index=False))

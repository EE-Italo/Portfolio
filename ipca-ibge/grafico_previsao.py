"""
Gera um gráfico comparando o IPCA real com as previsões de 1 mês à frente
dos três métodos testados em prever_ipca.py, ao longo da janela de backtest.
"""

import pandas as pd
import matplotlib.pyplot as plt

SURFACE = "#fcfcfb"
INK_PRIMARY = "#0b0b0b"
INK_SECONDARY = "#52514e"
INK_MUTED = "#898781"
GRIDLINE = "#e1e0d9"
BASELINE = "#c3c2b7"

COR_REAL = "#2a78d6"
COR_NAIVE = "#eb6834"
COR_SARIMA = "#1baf7a"
COR_ML = "#eda100"


def carregar():
    df = pd.read_csv("backtest_previsao.csv")
    df["data"] = pd.to_datetime(df["data"])
    return df


def estilizar_eixo(ax):
    ax.set_facecolor(SURFACE)
    for spine in ("top", "right", "left"):
        ax.spines[spine].set_visible(False)
    ax.spines["bottom"].set_color(BASELINE)
    ax.spines["bottom"].set_linewidth(1)
    ax.grid(axis="y", color=GRIDLINE, linewidth=1)
    ax.set_axisbelow(True)
    ax.tick_params(colors=INK_MUTED, labelsize=9)
    ax.yaxis.set_major_formatter(lambda v, _: f"{v:.1f}%")


def plotar(df, caminho="previsao_ipca.png"):
    fig, ax = plt.subplots(figsize=(10, 5), facecolor=SURFACE)
    fig.suptitle("IPCA — real vs. previsão de 1 mês à frente (backtest)",
                  color=INK_PRIMARY, fontsize=14, fontweight="bold", x=0.06, ha="left")
    ax.set_title(f"{len(df)} meses, validação walk-forward (expanding window)",
                  color=INK_SECONDARY, fontsize=10, loc="left")

    ax.plot(df["data"], df["real"], color=COR_REAL, linewidth=2.5, label="Real", zorder=4)
    ax.plot(df["data"], df["naive"], color=COR_NAIVE, linewidth=1.5, label="Ingênuo (repete último mês)", alpha=0.9)
    ax.plot(df["data"], df["sarima"], color=COR_SARIMA, linewidth=1.5, label="SARIMA", alpha=0.9)
    ax.plot(df["data"], df["ml"], color=COR_ML, linewidth=1.5, label="Gradient Boosting", alpha=0.9)

    estilizar_eixo(ax)
    ax.legend(
        loc="upper center", bbox_to_anchor=(0.5, -0.12), ncol=4,
        frameon=False, fontsize=9, labelcolor=INK_SECONDARY, handlelength=1.5,
    )

    fig.tight_layout(rect=(0, 0.06, 1, 0.93))
    fig.savefig(caminho, dpi=150, facecolor=SURFACE)
    print(f"Gráfico salvo em {caminho}")


if __name__ == "__main__":
    df = carregar()
    plotar(df)

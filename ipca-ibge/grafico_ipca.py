"""
Gera um gráfico da variação mensal do IPCA a partir de ipca.csv:
um painel com a série completa (mostra a hiperinflação pré-Real) e
um painel com o recorte pós-Plano Real (jul/1994), onde a escala
moderna fica legível.
"""

import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.dates as mdates

SURFACE = "#fcfcfb"
INK_PRIMARY = "#0b0b0b"
INK_SECONDARY = "#52514e"
INK_MUTED = "#898781"
GRIDLINE = "#e1e0d9"
BASELINE = "#c3c2b7"
SERIES = "#2a78d6"


def carregar():
    df = pd.read_csv("ipca.csv")
    df["data"] = pd.to_datetime(df["periodo"], format="%Y%m")
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
    ax.yaxis.set_major_formatter(lambda v, _: f"{v:.0f}%")


def rotular_ultimo_ponto(ax, df):
    ultimo = df.iloc[-1]
    ax.scatter([ultimo["data"]], [ultimo["variacao_mensal"]], s=36, color=SERIES,
               edgecolor=SURFACE, linewidth=2, zorder=5)
    ax.annotate(
        f"{ultimo['variacao_mensal']:.2f}%",
        (ultimo["data"], ultimo["variacao_mensal"]),
        textcoords="offset points", xytext=(6, 6),
        color=INK_PRIMARY, fontsize=9, fontweight="bold",
    )


def plotar(df, caminho="ipca_variacao_mensal.png"):
    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 8), facecolor=SURFACE)
    fig.suptitle("IPCA — Variação mensal (%)", color=INK_PRIMARY, fontsize=14, fontweight="bold", x=0.06, ha="left")

    ax1.plot(df["data"], df["variacao_mensal"], color=SERIES, linewidth=2)
    ax1.set_title(f"Série completa — dez/{df['ano'].min()} a {df['data'].max():%b/%Y}",
                   color=INK_SECONDARY, fontsize=10, loc="left")
    estilizar_eixo(ax1)
    rotular_ultimo_ponto(ax1, df)

    recente = df[df["data"] >= "1994-07-01"]
    ax2.plot(recente["data"], recente["variacao_mensal"], color=SERIES, linewidth=2)
    ax2.set_title("Desde o Plano Real (jul/1994) — recorte de escala",
                   color=INK_SECONDARY, fontsize=10, loc="left")
    estilizar_eixo(ax2)
    ax2.xaxis.set_major_locator(mdates.YearLocator(5))
    ax2.xaxis.set_major_formatter(mdates.DateFormatter("%Y"))
    rotular_ultimo_ponto(ax2, recente)

    fig.tight_layout(rect=(0, 0, 1, 0.95))
    fig.savefig(caminho, dpi=150, facecolor=SURFACE)
    print(f"Gráfico salvo em {caminho}")


if __name__ == "__main__":
    df = carregar()
    plotar(df)

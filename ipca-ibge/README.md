# IPCA — Extração e Visualização

Pipeline que extrai a série histórica completa do IPCA (Índice Nacional de Preços ao Consumidor Amplo) diretamente da API de Agregados do IBGE (tabela SIDRA 1737, nível Brasil), limpa os dados e gera um gráfico da variação mensal desde 1979.

Os dados são atualizados automaticamente todo mês via GitHub Actions, então o CSV e o gráfico neste repositório refletem sempre a divulgação mais recente do IBGE.

## Como funciona

| Etapa | Arquivo | Responsabilidade |
|-------|---------|-------------------|
| **Extração** | `extrair_ipca.py` | Consulta a API SIDRA do IBGE e salva a série completa em `ipca.csv`. |
| **Visualização** | `grafico_ipca.py` | Lê `ipca.csv` e gera `ipca_variacao_mensal.png`, com um painel da série completa (mostrando a hiperinflação pré-Real) e outro recortado a partir do Plano Real (jul/1994), onde a escala moderna fica legível. |
| **Automação** | `../.github/workflows/atualizar-ipca.yml` | Roda os dois scripts todo mês e commita os arquivos atualizados. |

### Variáveis extraídas

- Número-índice
- Variação mensal (%)
- Variação acumulada em 3, 6 e 12 meses (%)
- Variação acumulada no ano (%)

## Estrutura do projeto

```
.
├── extrair_ipca.py            # Extração + limpeza dos dados
├── grafico_ipca.py             # Geração do gráfico
├── ipca.csv                    # Série histórica atualizada
├── ipca_variacao_mensal.png    # Gráfico atualizado
└── requirements.txt
```

## Como executar

```bash
pip install -r requirements.txt
python extrair_ipca.py
python grafico_ipca.py
```

## Tecnologias

- **Python 3**
- **requests** — consumo da API do IBGE
- **pandas** — limpeza e estruturação dos dados
- **matplotlib** — visualização
- **GitHub Actions** — atualização mensal automática

## Autor

Ítalo Mendonça de Souza — Bacharelado em Ciência de Dados e IA (UFPB)

# Sistema de Recomendação de Produtos

Sistema de recomendação de produtos baseado em **similaridade entre clientes**, implementado em **C++** e também exposto ao **Python** via [pybind11](https://pybind11.readthedocs.io/).

A partir do histórico de compras de um conjunto de clientes, o sistema calcula quais clientes têm perfis parecidos (similaridade de Jaccard) e recomenda a cada cliente os produtos que seus vizinhos mais próximos compraram e ele ainda não.

## Como funciona

O projeto está dividido em módulos, cada um responsável por uma etapa do pipeline:

| Módulo | Arquivos | Responsabilidade |
|--------|----------|------------------|
| **Lista de Compras** | `lista_compras.{cpp,h}` | Lê o CSV de vendas e monta as estruturas de dados: clientes, produtos e a lista de compras de cada cliente. |
| **Similaridade** | `similaridade.{cpp,h}` | Monta a matriz de compras (cliente × produto) e calcula a matriz de similaridade de Jaccard entre todos os clientes. |
| **Recomendação** | `recomendacao.{cpp,h}` | Gera o ranking de produtos recomendados para um cliente com base nos clientes mais similares. |
| **Programa principal** | `main.cpp` | Interface de linha de comando que executa todo o fluxo e imprime os resultados. |
| **Binding Python** | `bindings.cpp`, `setup.py`, `leitura_e_execucao.py` | Expõe os módulos de similaridade e recomendação ao Python. |

### Algoritmo de similaridade

A similaridade entre dois clientes usa o **índice de Jaccard**: o número de produtos comprados em comum dividido pelo número de produtos comprados por pelo menos um dos dois.

O módulo implementa **duas versões** do cálculo e compara o desempenho das duas:

- **Padrão** — constrói a matriz transposta e multiplica A × Aᵀ.
- **Adaptado** — aproveita a simetria da matriz de interseção e calcula apenas a metade superior, reduzindo o número de operações.

## Estrutura do projeto

```
.
├── src/                      # Código-fonte C++
│   ├── main.cpp              # Programa principal (CLI)
│   ├── lista_compras.{cpp,h} # Leitura do CSV e estruturas de dados
│   ├── similaridade.{cpp,h}  # Matriz de compras e similaridade de Jaccard
│   ├── recomendacao.{cpp,h}  # Geração das recomendações
│   └── bindings.cpp          # Ponte C++ → Python (pybind11)
├── dados/                    # Bases de vendas por cluster (CSV)
├── setup.py                  # Compila o módulo Python
├── leitura_e_execucao.py     # Testador em Python
└── README.md
```

## Formato dos dados

Cada arquivo em `dados/` é um CSV com histórico de vendas de um *cluster* de clientes, separado por `;`:

```
DATA_COMPRA;COD_CLIENTE;COD_PRODUTO;NOME_PRODUTO
20200515;99CL9Y01;26630;TABLET NB316 M7S 16GB QUAD CORE PRETO
```

## Como executar

### Versão C++ (linha de comando)

Compile os fontes (o `bindings.cpp` é ignorado aqui, pois só é usado na versão Python):

```bash
g++ -std=c++17 src/main.cpp src/lista_compras.cpp src/similaridade.cpp src/recomendacao.cpp -o programa
```

Rode a partir da raiz do projeto (para que o caminho `dados/` seja encontrado):

```bash
./programa
```

O programa pergunta o número do cluster (0–20) e a quantidade de recomendações `k`, e então exibe: os produtos de cada cliente (A1), a matriz de similaridade (A2), a comparação de desempenho dos dois algoritmos (A4) e as recomendações finais (A3).

### Versão Python (módulo compilado)

Requer `pybind11` instalado (`pip install pybind11`). Compile o módulo:

```bash
python setup.py build_ext --inplace
```

Isso gera um módulo `recomendador` que pode ser importado no Python. Em seguida, rode o testador:

```bash
python leitura_e_execucao.py
```

## Tecnologias

- **C++17** — lógica central e estruturas de dados
- **pybind11** — integração C++ ↔ Python
- **Python 3** — script de teste e leitura de dados

## Autor

Ítalo Mendonça de Souza — Bacharelado em Ciência de Dados e IA (UFPB)

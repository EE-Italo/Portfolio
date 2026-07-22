# Porcentagem de Sucesso em Processos Seletivos: Estudantes do CI-UFPB

Este projeto faz parte da disciplina de **Introdução à Ciência de Dados (ICD)** e tem como objetivo analisar o desempenho e a taxa de aprovação de estudantes do Centro de Informática da Universidade Federal da Paraíba (CI-UFPB) em processos seletivos do setor de tecnologia.

## Equipe
* **Ítalo Mendonça**
* **Julia Rosa**
* **Maria Nathalia**

---

## Objetivo do Projeto
Investigar e quantificar o sucesso de estudantes das graduações do Centro de Informática (Ciência da Computação, Engenharia de Computação e Ciência de Dados e IA) em seleções do mercado de trabalho.

Além da métrica geral, o projeto busca identificar:
* **Recorte de Gênero:** Existe diferença na taxa de aprovação ou no tempo de inserção no mercado entre estudantes de diferentes gêneros?
* **Diferença entre Cursos:** Há variação de sucesso entre as três graduações do Centro?

---

## Abordagem de Coleta de Dados
A metodologia de coleta foi estruturada para garantir a integridade dos dados oficiais:

* **Dados Acadêmicos (CI):** Extração de listas de bolsistas e voluntários de projetos de extensão e PROBEX através dos portais institucionais e documentos do Centro de Informática.
* **Dados de Mercado (Externo):** Mineração de nomes e cargos em editais de convocação para residências tecnológicas, concursos e estágios publicados no Diário Oficial.
* **Enriquecimento via API:** Para a análise de gênero, utilizamos a API de nomes do IBGE, que realiza a predição probabilística do gênero com base no primeiro nome dos candidatos.

---

## Dicionário de Dados

Abaixo estão as descrições das colunas que compõem o conjunto de dados final após o processamento:

| Nome da Coluna | Descrição da Coluna | Exemplo |
| :--- | :--- | :--- |
| **`origem_dado`** | Indica a fonte do registro: se provém do ecossistema interno da UFPB ou de publicações externas no Diário Oficial. | "Mercado (DOU)" ou "CI-UFPB" |
| **`curso`** | Graduação vinculada ao registro (extraída das siglas das abas da planilha do CI ou identificada no edital). | "Ciência de Dados e IA", "Engenharia da Computação" ou "Ciência da Computação" |
| **`genero_predito`** | Identificação de gênero (Masculino/Feminino) baseada no primeiro nome, validada pela API do IBGE e revisada na limpeza de dados. | "Feminino" |
| **`tipo_vaga`** | Categoria da oportunidade ou programa acadêmico relacionado ao registro. | "Monitoria", "PIBIC", "PROBEX" ou "Estágio" |
| **`ano_referencia`** | O ano em que a nomeação foi publicada ou em que o projeto de extensão/monitoria ocorreu. | 2024 |
| **`nome_candidato`** | Nome completo do indivíduo (utilizado para a classificação de gênero e remoção de duplicatas). | "MARIA DA SILVA" |

> **Nota:** As colunas `curso` e `tipo_vaga` são extraídas dinamicamente durante o pré-processamento no Jupyter Notebook a partir dos metadados das fontes originais e nomes das abas das planilhas.

---

## Disponibilização dos Dados
Os dados brutos e processados utilizados nesta análise estão disponíveis no serviço de armazenamento abaixo:

* https://drive.google.com/drive/folders/1MWO9to1v4gEM1xJj9Q-J0RR65T29lCxn?usp=sharing

---
## Como Executar o Projeto
1. Clone o repositório.
2. Instale as dependências: `pip install -r requirements.txt`.
3. Abra o notebook `projeto_ICD.ipynb` em seu ambiente Jupyter ou VS Code e execute todas as células.
---
*Projeto desenvolvido para a disciplina de ICD - UFPB (2025.2).*

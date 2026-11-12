# Artigo final do POC I (modelo IEEE)

Trabalho de cunho científico: segue o modelo de artigo da IEEE (classe `IEEEtran`, versão
conferência, em português), com a estrutura sugerida nas normas da disciplina: resumo e
abstract, introdução, referencial teórico e trabalhos correlatos, desenvolvimento
(contribuição), resultados, conclusões e trabalhos futuros, referências e apêndices.

## Arquivos

| Arquivo | Conteúdo |
|---|---|
| `artigo.tex` | texto do artigo |
| `referencias.bib` | referências (BibTeX, estilo `IEEEtran`) |
| `figs/*.pdf` | figuras vetoriais, geradas por `scripts/paper_figs.py` |
| `tabelas/*.tex` | tabelas, geradas por `scripts/paper_tables.py` a partir dos resultados |
| `artigo_overleaf.zip` | tudo acima num único arquivo, para enviar ao Overleaf |

## Compilar no Overleaf

1. *New Project → Upload Project* e escolha `artigo_overleaf.zip`.
2. Em *Menu*, confira que o compilador é **pdfLaTeX** e o documento principal é `artigo.tex`.
3. Clique em *Recompile*. O Overleaf roda o BibTeX sozinho.

Localmente (com TeX Live ou MiKTeX): `latexmk -pdf artigo.tex`.

## Antes de entregar

- Revise o texto: ele é seu e deve refletir o que você apresentará.
- Confira no Moodle se há exigências de formatação além do modelo IEEE (capa, limite de
  páginas, idioma do resumo).
- Veja o comentário antes da bibliografia em `artigo.tex` sobre declarar o uso de ferramentas
  de IA, conforme as regras da disciplina.
- Se mudar algum resultado, regenere as figuras e tabelas com
  `python scripts/paper_figs.py` e `python scripts/paper_tables.py` em vez de editar à mão.

"""Pôster do POC I no modelo do banner da Mostra UFMG (60 × 90 cm, retrato).

A partir do modelo (docs/poster/modelo_banner_mostraUFMG.pptx), gera:
  docs/poster/poster_POC_I.pptx   o pôster: título, subtítulo, linha vermelha, moldura e
                                  rodapé (QR code, @dcc.ufmg, logos DCC/UFMG) do modelo,
                                  mais o conteúdo do trabalho
  docs/poster/figs/*.png          figuras do pôster (300 dpi)
  docs/poster/previa.png          prévia desenhada a partir da MESMA especificação de layout,
                                  com as fontes do modelo, para conferir quebras de linha e
                                  transbordamento (não há PowerPoint/LibreOffice no ambiente)

O modelo traz dois slides quase iguais; usa-se o segundo, em que o endereço dcc.ufmg.br fica
sob o QR code (no primeiro ele fica por cima do código), e o primeiro é removido.

Fontes (Tomorrow e Titillium Web, Google Fonts, licença OFL): as embutidas no modelo estão
comprimidas (MicroType Express) e não servem para medir texto; passe com --fonts uma pasta
com Tomorrow-Bold.ttf, TitilliumWeb-Regular.ttf, TitilliumWeb-Bold.ttf e
TitilliumWeb-Italic.ttf. O pôster deve ser aberto no Google Slides, que tem as duas fontes.

Uso (a partir da pasta POC-I):
    python scripts/poster.py --fonts <pasta das fontes>
"""
from __future__ import annotations

import argparse
import re
import sys
import zipfile
from dataclasses import dataclass, field
from pathlib import Path
from xml.sax.saxutils import escape

ROOT = Path(__file__).resolve().parents[1]
sys.path[:0] = [str(ROOT), str(ROOT / "scripts")]

import matplotlib                                   # noqa: E402

matplotlib.use("Agg")
import matplotlib.pyplot as plt                     # noqa: E402
import pandas as pd                                 # noqa: E402
from matplotlib import font_manager                 # noqa: E402
from matplotlib.colors import ListedColormap        # noqa: E402
from PIL import Image, ImageDraw, ImageFont         # noqa: E402

OUT = ROOT / "docs" / "poster"
EMU_CM = 360000
PT_CM = 2.54 / 72
RED, INK, BODY, MUTED = "C90C0F", "1A1A1A", "2B2B2B", "555555"
TINT_RED, TINT_GRAY = "FBECEC", "F3F3F3"
HCOLOR = {"dsatur": "#2a78d6", "rlf": "#eb6834"}
TITI, TOMO = "Titillium Web", "Tomorrow"
# altura natural da linha = (ascendente + descendente) / UPM da tabela hhea
NATURAL = {TITI: 1.521, TOMO: 1.2}
ASCENT = {TITI: 1133 / 1521, TOMO: 1000 / 1200}
FONT_FILES = {(TITI, False, False): "TitilliumWeb-Regular.ttf",
              (TITI, True, False): "TitilliumWeb-Bold.ttf",
              (TITI, False, True): "TitilliumWeb-Italic.ttf",
              (TOMO, True, False): "Tomorrow-Bold.ttf"}


# --------------------------------------------------------------------------- #
# Especificação do layout (centímetros)
# --------------------------------------------------------------------------- #
@dataclass
class Run:
    text: str
    size: float = 28
    bold: bool = False
    italic: bool = False
    color: str = BODY
    font: str = TITI


@dataclass
class Para:
    runs: list[Run]
    align: str = "l"            # "l" ou "ctr"
    spacing: float = 0.85       # múltiplo da altura natural da linha
    after: float = 10           # pt depois do parágrafo
    bullet: bool = False
    br_before: list[int] = field(default_factory=list)   # quebras forçadas (índice do run)


@dataclass
class Box:
    name: str
    x: float
    y: float
    w: float
    h: float
    paras: list[Para] = field(default_factory=list)
    fill: str | None = None
    line: str | None = None
    radius: float = 0.0          # 0 = retângulo; >0 = cantos arredondados (fração)
    inset: tuple = (0, 0, 0, 0)  # esquerda, topo, direita, base
    anchor: str = "t"
    shape: str = "rect"          # rect | rightArrow


@dataclass
class Pic:
    name: str
    x: float
    y: float
    w: float
    h: float
    path: Path
    alt: str


def P(*runs, **kw) -> Para:
    return Para(list(runs), **kw)


def body(text, size=28, **kw) -> list[Run]:
    """Texto com trechos em **negrito**."""
    out = []
    for i, part in enumerate(re.split(r"\*\*", text)):
        if part:
            out.append(Run(part, size=size, bold=(i % 2 == 1), **kw))
    return out


X0, W = 3.2, 53.6
G = 1.6
CW = (W - G) / 2
XL, XR = X0, X0 + CW + G


HDR_H = 1.8      # altura do cabeçalho de seção (quadrado vermelho + título)
HDR_GAP = 0.45   # espaço entre o cabeçalho e o conteúdo
ROW_GAP = 0.9    # espaço entre faixas
FIG_H = 9.5      # altura das figuras
SLACK = 0.3      # folga vertical em cada caixa medida


def header(num: int, title: str, x: float, y: float, w: float) -> list:
    tile = Box(f"Seção {num}", x, y, HDR_H, HDR_H,
               [P(Run(str(num), 40, True, color="FFFFFF", font=TOMO), align="ctr",
                  spacing=1.0, after=0)], fill=RED, anchor="ctr")
    text = Box(f"Título da seção {num}", x + HDR_H + 0.5, y, w - HDR_H - 0.5, HDR_H,
               [P(Run(title, 40, True, color=INK, font=TOMO), spacing=1.0, after=0)],
               anchor="ctr")
    return [tile, text]


def card(name, x, y, w, paras, fill=TINT_GRAY, inset=0.6, anchor="t"):
    return Box(name, x, y, w, 0, paras, fill=fill, radius=0.06,
               inset=(inset, inset, inset, inset), anchor=anchor)


def fit(box: Box, fonts, slack: float = SLACK) -> Box:
    """Altura da caixa = altura medida do texto + margens internas + folga."""
    _, h, _ = layout(box, fonts)
    box.h = round(h + box.inset[1] + box.inset[3] + slack, 2)
    return box


def same_height(boxes: list[Box]) -> None:
    m = max(b.h for b in boxes)
    for b in boxes:
        b.h = m


def build_spec(figs: dict[str, Path], fonts) -> list:
    S: list = []
    # ------------------------------------------------ autores
    aut = fit(Box("Autores", X0, 15.45, W, 0, [
        P(Run("Lucas Affonso Pires", 30, True, color=INK),
          Run("   ·   Orientador: Prof. Marcio Costa Santos", 30, color=INK),
          align="ctr", spacing=1.0, after=2),
        P(Run("Departamento de Ciência da Computação  ·  Universidade Federal de Minas Gerais",
              24, color=MUTED), align="ctr", spacing=1.0, after=0)]), fonts, slack=0)
    S.append(aut)

    # ------------------------------------------------ 1 e 2: problema e pergunta
    y = aut.y + aut.h + 0.9
    c = y + HDR_H + HDR_GAP
    S += header(1, "O PROBLEMA", XL, y, CW)
    left = fit(Box("Texto: o problema", XL, c, CW, 0, [
        P(*body("Colorir um grafo é dar cores aos vértices sem que vizinhos fiquem iguais, "
                "usando o mínimo de cores. Algoritmos **gulosos** colorem um vértice por vez "
                "com a menor cor permitida; o resultado depende da **regra que escolhe o "
                "próximo vértice**.")),
        P(*body("**Welsh-Powell** (maior grau), **DSATUR** (mais cores na vizinhança) e "
                "**RLF** (uma cor de cada vez) usam regras diferentes, e nenhuma vence "
                "sempre."), after=0)]), fonts)
    S.append(left)
    S += header(2, "PERGUNTA E OBJETIVO", XR, y, CW)
    q = fit(card("Pergunta do trabalho", XR, c, CW, [
        P(*body("Um modelo de aprendizado de máquina consegue escolher, a cada passo, o "
                "próximo vértice e usar no máximo tantas cores quanto as heurísticas "
                "clássicas?", size=30, color=INK), after=0)],
        fill=TINT_RED, inset=0.7, anchor="ctr"), fonts, slack=0.4)
    obj = fit(Box("Texto: objetivo", XR, q.y + q.h + 0.5, CW, 0, [
        P(*body("**No POC I:** heurísticas, base de dados, análise exploratória e prova de "
                "conceito. As melhorias do modelo ficam para o POC II."), after=0)]), fonts)
    S += [q, obj]
    y = max(left.y + left.h, obj.y + obj.h) + ROW_GAP

    # ------------------------------------------------ 3: o que construímos
    c = y + HDR_H + HDR_GAP
    S += header(3, "O QUE CONSTRUÍMOS", XL, y, W)
    gap4 = 1.6
    cw4 = (W - 3 * gap4) / 4
    items = [("4", "heurísticas:", " aleatória, Welsh-Powell, DSATUR e RLF; 43 testes"),
             ("2.818", "grafos:", " Erdős–Rényi e Barabási–Albert; 7,3 milhões de passos"),
             ("858.790", "candidatos rotulados", " por rollout com DSATUR e RLF"),
             ("127", "instâncias DIMACS:", " teste final, usado uma única vez")]
    cards = [fit(card(f"Etapa {i + 1}", X0 + i * (cw4 + gap4), c, cw4, [
        P(Run(num, 56, True, color=RED, font=TOMO), spacing=1.0, after=2),
        P(Run(lab, 26, True, color=INK), Run(desc, 26, color=BODY), after=0)]), fonts)
        for i, (num, lab, desc) in enumerate(items)]
    same_height(cards)
    S += cards
    for i, cd in enumerate(cards[:-1]):
        S.append(Box(f"Seta {i + 1}", cd.x + cw4 + 0.3, cd.y + cd.h / 2 - 0.5, 1.0, 1.0,
                     fill=RED, shape="rightArrow"))
    y = cards[0].y + cards[0].h + ROW_GAP

    # ------------------------------------------------ 4 e 5: rótulos e dados
    c = y + HDR_H + HDR_GAP
    S += header(4, "ROTULANDO CADA PASSO", XL, y, CW)
    S.append(Pic("Figura: rótulo por passo", XL, c, CW, FIG_H, figs["rotulo"],
                 "Ao longo dos 150 passos de um grafo aleatório, o valor do melhor candidato "
                 "cai de 57 para 53 cores; o RLF sozinho usa 57 e o DSATUR, 61."))
    cap1 = fit(Box("Legenda: rótulo", XL, c + FIG_H + 0.3, CW, 0, [
        P(*body("A cada passo, as 4 heurísticas sugerem um vértice, e cada sugestão é "
                "completada com DSATUR e com RLF. Seguir sempre a melhor (**oráculo**) "
                "nunca usa mais cores que elas e usa menos em **55%** dos grafos.",
                size=24), after=0)]), fonts)
    S += header(5, "O QUE OS DADOS MOSTRAM", XR, y, CW)
    S.append(Pic("Figura: heurística vencedora", XR, c, CW, FIG_H, figs["vencedora"],
                 "Mapas da heurística vencedora: RLF em quase todos os grafos Erdős–Rényi, "
                 "DSATUR em quase todos os Barabási–Albert."))
    cap2 = fit(Box("Legenda: dados", XR, c + FIG_H + 0.3, CW, 0, [
        P(*body("A disputa é só entre RLF e DSATUR, e a **família do grafo** quase decide a "
                "vencedora. Só **59%** dos passos têm escolha errada possível (82% no ER, "
                "6% no BA).", size=24), after=0)]), fonts)
    same_height([cap1, cap2])
    S += [cap1, cap2]
    y = cap1.y + cap1.h + ROW_GAP

    # ------------------------------------------------ 6: resultados
    c = y + HDR_H + HDR_GAP
    S += header(6, "RESULTADOS", XL, y, W)
    stats = [("97,2%", "de acerto por passo (seguir o RLF: 89,3%)"),
             ("96%", "de empates com a melhor heurística na validação"),
             ("90%", "de empates no DIMACS (1\u00a0melhor, 9%\u00a0pior)"),
             ("24%", "do DIMACS (n\u00a0≤\u00a0500) em que o oráculo vence")]
    sgap = 1.0
    sw = (W - 3 * sgap) / 4
    st = []
    for i, (num, txt) in enumerate(stats):
        b = fit(card(f"Resultado {i + 1}", X0 + i * (sw + sgap), c, sw, [
            P(Run(num, 66, True, color=RED, font=TOMO), spacing=1.0, after=2),
            P(Run(txt, 24, color=BODY), after=0)], fill="FFFFFF"), fonts)
        b.line = "DDDDDD"
        st.append(b)
    same_height(st)
    S += st
    yb = c + st[0].h + 0.8
    why = fit(card("Por que ainda não supera", XL, yb, CW, [
        P(Run("Por que ainda não supera?", 28, True, color=INK), spacing=1.0, after=4),
        P(*body("O modelo aprendeu a seguir o **RLF no ER** e o **DSATUR no BA**. Com ~195 "
                "passos decisivos por grafo, 2,8% de erro gera ~5 escolhas ruins, mais que o "
                "ganho possível (~1,2 cor).", size=24), after=0)]), fonts)
    nxt = fit(card("Próximos passos", XR, yb, CW, [
        P(Run("Próximos passos: POC II", 28, True, color=INK), spacing=1.0, after=4),
        P(Run("pesar mais os passos decisivos", 24), bullet=True, after=0),
        P(Run("rotular os estados da própria política (DAgger)", 24), bullet=True, after=0),
        P(Run("otimizar o número final de cores", 24), bullet=True, after=0),
        P(Run("treinar com novos tipos de grafo", 24), bullet=True, after=0)],
        fill=TINT_RED), fonts)
    same_height([why, nxt])
    S += [why, nxt]
    return S


# --------------------------------------------------------------------------- #
# Medição e quebra de linhas (mesma regra na prévia e na verificação)
# --------------------------------------------------------------------------- #
class Fonts:
    def __init__(self, folder: Path):
        self.folder = folder
        self.cache: dict = {}

    def get(self, run: Run, px: int):
        key = (run.font, run.bold, run.italic, px)
        if key not in self.cache:
            fname = FONT_FILES.get((run.font, run.bold, run.italic)) or \
                FONT_FILES[(run.font, run.bold, False)]
            self.cache[key] = ImageFont.truetype(str(self.folder / fname), px)
        return self.cache[key]

    def width_pt(self, text: str, run: Run) -> float:
        return self.get(run, 200).getlength(text) / 200 * run.size


def wrap(para: Para, width_pt: float, fonts: Fonts):
    """Linhas do parágrafo: lista de [(texto, run, x_pt)], largura em pt de cada uma."""
    toks = [(t, r) for r in para.runs for t in re.findall(r"[^ ]+| +", r.text)]   # espaço não separável não quebra
    lines, cur, x = [], [], 0.0
    for t, r in toks:
        w = fonts.width_pt(t, r)
        if t == " " * len(t):
            if cur:
                cur.append((t, r, x))
                x += w
            continue
        if cur and x + w > width_pt:
            while cur and cur[-1][0].strip(" ") == "":
                x = cur[-1][2]
                cur.pop()
            lines.append((cur, x))
            cur, x = [], 0.0
        cur.append((t, r, x))
        x += w
    while cur and cur[-1][0].strip(" ") == "":
        x = cur[-1][2]
        cur.pop()
    lines.append((cur, x))
    return lines


BULLET_IND = 0.9   # cm


def layout(box: Box, fonts: Fonts):
    """(linhas posicionadas, altura do texto em cm, palavras maiores que a largura)."""
    li, ti, ri, bi = box.inset
    avail = (box.w - li - ri) / PT_CM
    out, y, too_wide = [], 0.0, []
    for k, para in enumerate(box.paras):
        ind = BULLET_IND / PT_CM if para.bullet else 0.0
        size = max(r.size for r in para.runs)
        fam = para.runs[0].font
        lh = NATURAL[fam] * size * para.spacing
        for j, (words, wpt) in enumerate(wrap(para, avail - ind, fonts)):
            if wpt > avail - ind + 0.5:
                too_wide.append(words[0][0])
            out.append((para, words, wpt, y, lh, ind, j == 0))
            y += lh
        y += para.after if k < len(box.paras) - 1 else 0
    return out, y * PT_CM, too_wide


# --------------------------------------------------------------------------- #
# Figuras do pôster
# --------------------------------------------------------------------------- #
def make_figs(fonts_dir: Path | None) -> dict[str, Path]:
    import eda
    figdir = OUT / "figs"
    figdir.mkdir(parents=True, exist_ok=True)
    family = "DejaVu Sans"
    if fonts_dir:
        for f in fonts_dir.glob("TitilliumWeb-*.ttf"):
            font_manager.fontManager.addfont(str(f))
        family = TITI
    plt.rcParams.update({"font.family": family, "font.size": 20, "axes.titlesize": 22,
                         "axes.labelsize": 20, "xtick.labelsize": 18, "ytick.labelsize": 18,
                         "legend.fontsize": 19, "axes.spines.top": False,
                         "axes.spines.right": False, "axes.edgecolor": "#888888",
                         "xtick.color": "#333333", "ytick.color": "#333333",
                         "axes.labelcolor": "#222222", "figure.facecolor": "white",
                         "axes.facecolor": "white", "savefig.facecolor": "white",
                         "axes.grid": True, "grid.color": "#e6e6e6", "grid.linewidth": 0.8})
    size = (CW / 2.54, FIG_H / 2.54)

    # rótulo por passo (mesmo grafo do pitch e do artigo)
    gid = "er_n150_p0.90_s0"
    R = pd.read_parquet(ROOT / "data" / "B" / "rollouts", filters=[("graph_id", "==", gid)])
    o = pd.read_parquet(ROOT / "data" / "B" / "oracle").set_index("graph_id").loc[gid]
    best = R.groupby("step").rollout_colors.min()
    inf = R.groupby("step").label.min() == 0
    fig, ax = plt.subplots(figsize=size)
    for t in inf[inf].index:
        ax.axvspan(t - 0.5, t + 0.5, color="#efece6", linewidth=0)
    for h, yv, lab in (("dsatur", o.colors_dsatur, "DSATUR"), ("rlf", o.colors_rlf, "RLF")):
        ax.axhline(yv, color=HCOLOR[h], linestyle=":", linewidth=3)
        ax.text(len(best) - 1, yv + 0.25, f"{lab} sozinho: {yv}", color=HCOLOR[h],
                ha="right", va="bottom", fontsize=20, fontweight="bold")
    s = R[R.suggested_by.str.contains("rlf", regex=False)].set_index("step").rollout_colors
    ax.plot(s.index, s, color=HCOLOR["rlf"], linewidth=2.2, alpha=0.8,
            label="candidato do RLF")
    ax.plot(best.index, best, color="#1a1a1a", linewidth=4, label="melhor candidato")
    ax.text(len(best) - 1, o.oracle_colors - 0.35, f"oráculo: {o.oracle_colors}",
            ha="right", va="top", fontsize=20, fontweight="bold", color="#1a1a1a")
    ax.set_ylim(o.oracle_colors - 1.9, o.colors_dsatur + 1.4)
    ax.set_xlim(-1, len(best))
    ax.set(xlabel="passo (faixas cinza: passos informativos)", ylabel="cores ao completar")
    ax.legend(loc="center", bbox_to_anchor=(0.39, 0.345), ncol=2, frameon=False,
              columnspacing=1.2, handlelength=1.6)
    fig.tight_layout(pad=0.3)
    p_rot = figdir / "rotulo.png"
    fig.savefig(p_rot, dpi=300)
    plt.close(fig)

    # heurística vencedora por região da camada A
    A = eda.load_tier(ROOT / "data", "A")
    present = [c for c in eda.WCATS if (A.winner == c).any()]
    codes = {c: i for i, c in enumerate(present)}
    cmap = ListedColormap([eda.WCOLOR[c] for c in present])
    A["wcode"] = A.winner.map(codes).astype(int)
    mode = lambda s: s.value_counts().idxmax()      # noqa: E731
    fig, axes = plt.subplots(1, 2, figsize=size)
    er = A[A.model == "er"].groupby(["param", "n"]).wcode.agg(mode).unstack("n")
    axes[0].imshow(er.to_numpy(), origin="lower", aspect="auto", cmap=cmap, vmin=-0.5,
                   vmax=len(present) - 0.5, interpolation="nearest")
    axes[0].set_xticks(range(len(er.columns))[1::6], [str(c) for c in er.columns][1::6])
    axes[0].set_yticks(range(len(er.index))[1::6],
                       [f"{p:.1f}".replace(".", ",") for p in er.index][1::6])
    axes[0].set(xlabel="n", ylabel="p", title="Erdős–Rényi")
    axes[0].grid(False)
    ba = A[A.model == "ba"].groupby(["n", "param"]).wcode.agg(mode).reset_index()
    axes[1].scatter(ba.n, ba.param, c=ba.wcode, cmap=cmap, vmin=-0.5,
                    vmax=len(present) - 0.5, s=40, marker="s", linewidths=0)
    axes[1].set_yscale("log")
    axes[1].set(xlabel="n", ylabel="m", title="Barabási–Albert")
    labels = {"DSATUR": "DSATUR vence", "RLF": "RLF vence"}
    fig.legend([plt.Rectangle((0, 0), 1, 1, color=eda.WCOLOR[c]) for c in present],
               [labels.get(c, "empate") for c in present], loc="lower center",
               ncol=len(present), frameon=False, bbox_to_anchor=(0.5, 0.0))
    fig.tight_layout(pad=0.3, rect=(0, 0.13, 1, 1))
    p_ven = figdir / "vencedora.png"
    fig.savefig(p_ven, dpi=300)
    plt.close(fig)
    return {"rotulo": p_rot, "vencedora": p_ven}


# --------------------------------------------------------------------------- #
# XML
# --------------------------------------------------------------------------- #
def emu(cm: float) -> int:
    return int(round(cm * EMU_CM))


def rpr(r: Run) -> str:
    return (f'<a:rPr lang="pt-BR" sz="{int(round(r.size * 100))}" b="{int(r.bold)}" '
            f'i="{int(r.italic)}" dirty="0"><a:solidFill><a:srgbClr val="{r.color}"/>'
            f'</a:solidFill><a:latin typeface="{r.font}"/><a:ea typeface="{r.font}"/>'
            f'<a:cs typeface="{r.font}"/><a:sym typeface="{r.font}"/></a:rPr>')


def para_xml(p: Para) -> str:
    mar = (f' marL="{emu(BULLET_IND)}" indent="{-emu(BULLET_IND)}"' if p.bullet
           else ' marL="0" indent="0"')
    bu = ('<a:buClr><a:srgbClr val="C90C0F"/></a:buClr><a:buSzPct val="100000"/>'
          '<a:buFont typeface="Arial"/><a:buChar char="&#8226;"/>' if p.bullet
          else "<a:buNone/>")
    ppr = (f'<a:pPr{mar} algn="{p.align}" rtl="0"><a:lnSpc><a:spcPct '
           f'val="{int(round(p.spacing * 100000))}"/></a:lnSpc><a:spcBef><a:spcPts val="0"/>'
           f'</a:spcBef><a:spcAft><a:spcPts val="{int(round(p.after * 100))}"/></a:spcAft>'
           f'{bu}</a:pPr>')
    runs = "".join(f"<a:r>{rpr(r)}<a:t>{escape(r.text)}</a:t></a:r>" for r in p.runs)
    return f"<a:p>{ppr}{runs}<a:endParaRPr lang=\"pt-BR\" sz=\"{int(p.runs[-1].size * 100)}\"/></a:p>"


def box_xml(b: Box, sid: int) -> str:
    if b.shape == "rightArrow":
        geom = '<a:prstGeom prst="rightArrow"><a:avLst/></a:prstGeom>'
    elif b.radius:
        geom = (f'<a:prstGeom prst="roundRect"><a:avLst><a:gd name="adj" '
                f'fmla="val {int(b.radius * 100000)}"/></a:avLst></a:prstGeom>')
    else:
        geom = '<a:prstGeom prst="rect"><a:avLst/></a:prstGeom>'
    fill = (f'<a:solidFill><a:srgbClr val="{b.fill}"/></a:solidFill>' if b.fill
            else "<a:noFill/>")
    line = (f'<a:ln w="19050"><a:solidFill><a:srgbClr val="{b.line}"/></a:solidFill></a:ln>'
            if b.line else "<a:ln><a:noFill/></a:ln>")
    li, ti, ri, bi = (emu(v) for v in b.inset)
    paras = "".join(para_xml(p) for p in b.paras) or "<a:p><a:endParaRPr lang=\"pt-BR\"/></a:p>"
    tx = "1" if b.paras and not b.fill else "0"
    return (f'<p:sp><p:nvSpPr><p:cNvPr id="{sid}" name="{escape(b.name)}"/>'
            f'<p:cNvSpPr{" txBox=\"1\"" if tx == "1" else ""}/><p:nvPr/></p:nvSpPr>'
            f'<p:spPr><a:xfrm><a:off x="{emu(b.x)}" y="{emu(b.y)}"/><a:ext cx="{emu(b.w)}" '
            f'cy="{emu(b.h)}"/></a:xfrm>{geom}{fill}{line}</p:spPr>'
            f'<p:txBody><a:bodyPr wrap="square" lIns="{li}" tIns="{ti}" rIns="{ri}" '
            f'bIns="{bi}" anchor="{b.anchor}" anchorCtr="0" rtlCol="0"><a:noAutofit/>'
            f'</a:bodyPr><a:lstStyle/>{paras}</p:txBody></p:sp>')


def pic_xml(p: Pic, sid: int, rid: str) -> str:
    return (f'<p:pic><p:nvPicPr><p:cNvPr id="{sid}" name="{escape(p.name)}" '
            f'descr="{escape(p.alt)}"/><p:cNvPicPr><a:picLocks noChangeAspect="1"/>'
            f'</p:cNvPicPr><p:nvPr/></p:nvPicPr><p:blipFill><a:blip r:embed="{rid}"/>'
            f'<a:stretch><a:fillRect/></a:stretch></p:blipFill><p:spPr><a:xfrm>'
            f'<a:off x="{emu(p.x)}" y="{emu(p.y)}"/><a:ext cx="{emu(p.w)}" cy="{emu(p.h)}"/>'
            f'</a:xfrm><a:prstGeom prst="rect"><a:avLst/></a:prstGeom></p:spPr></p:pic>')


TITLE_LINES = ("APRENDIZADO DE MÁQUINA PARA ESCOLHAS", "GULOSAS EM COLORAÇÃO DE GRAFOS")
TITLE_PT = 60
TITLE_Y = 3.6        # cm: centraliza as duas linhas entre a borda e a linha vermelha


def build_pptx(template: Path, out: Path, spec: list) -> None:
    zin = zipfile.ZipFile(template)
    files = {n: zin.read(n) for n in zin.namelist()}
    drop = {"ppt/slides/slide1.xml", "ppt/slides/_rels/slide1.xml.rels",
            "ppt/notesSlides/notesSlide1.xml", "ppt/notesSlides/_rels/notesSlide1.xml.rels"}

    def sub(part, old, new, count=1):
        s = files[part].decode("utf-8")
        assert s.count(old) == count, (part, old[:60], s.count(old))
        files[part] = s.replace(old, new).encode("utf-8")

    # remove o slide 1 (variante com o endereço sobre o QR code)
    sub("ppt/presentation.xml", '<p:sldId id="256" r:id="rId6"/>', "")
    sub("ppt/_rels/presentation.xml.rels",
        '<Relationship Id="rId6" Type="http://schemas.openxmlformats.org/officeDocument/2006/'
        'relationships/slide" Target="slides/slide1.xml"/>', "")
    for part in ("/ppt/slides/slide1.xml", "/ppt/notesSlides/notesSlide1.xml"):
        ct = files["[Content_Types].xml"].decode("utf-8")
        files["[Content_Types].xml"] = re.sub(
            rf'<Override ContentType="[^"]+" PartName="{re.escape(part)}"/>', "", ct
        ).encode("utf-8")

    slide = "ppt/slides/slide2.xml"
    # título em duas linhas, 60 pt, centralizado verticalmente acima da linha vermelha
    old_run = ('<a:r><a:rPr b="1" lang="en-US" sz="8733"><a:latin typeface="Tomorrow"/>'
               '<a:ea typeface="Tomorrow"/><a:cs typeface="Tomorrow"/><a:sym typeface="Tomorrow"/>'
               '</a:rPr><a:t>TÍTULO (COLOCAR EM DUAS LINHAS)</a:t></a:r>')
    rp = (f'<a:rPr b="1" lang="pt-BR" sz="{TITLE_PT * 100}"><a:latin typeface="Tomorrow"/>'
          '<a:ea typeface="Tomorrow"/><a:cs typeface="Tomorrow"/><a:sym typeface="Tomorrow"/>'
          '</a:rPr>')
    sub(slide, old_run, f"<a:r>{rp}<a:t>{TITLE_LINES[0]}</a:t></a:r><a:br>{rp}</a:br>"
                        f"<a:r>{rp}<a:t>{TITLE_LINES[1]}</a:t></a:r>")
    sub(slide, '<a:off x="1146122" y="874869"/><a:ext cx="19307700" cy="3226200"/>',
        f'<a:off x="1146122" y="{emu(TITLE_Y)}"/><a:ext cx="19307700" cy="{emu(6.3)}"/>')
    sub(slide, "<a:t>PROJETO ORIENTADO EM COMPUTAÇÃO</a:t>",
        "<a:t>PROJETO ORIENTADO EM COMPUTAÇÃO I</a:t>")

    # conteúdo
    rels = files["ppt/slides/_rels/slide2.xml.rels"].decode("utf-8")
    shapes, sid, nrel = [], 300, 10
    for el in spec:
        sid += 1
        if isinstance(el, Pic):
            rid = f"rId{nrel}"
            nrel += 1
            media = f"ppt/media/poster_{el.path.stem}.png"
            files[media] = el.path.read_bytes()
            rels = rels.replace("</Relationships>",
                                f'<Relationship Id="{rid}" Type="http://schemas.openxmlformats'
                                f'.org/officeDocument/2006/relationships/image" '
                                f'Target="../media/{Path(media).name}"/></Relationships>')
            shapes.append(pic_xml(el, sid, rid))
        else:
            shapes.append(box_xml(el, sid))
    files["ppt/slides/_rels/slide2.xml.rels"] = rels.encode("utf-8")
    sub(slide, "</p:spTree>", "".join(shapes) + "</p:spTree>")

    out.parent.mkdir(parents=True, exist_ok=True)
    with zipfile.ZipFile(out, "w", zipfile.ZIP_DEFLATED) as z:
        for n in zin.namelist():                     # ordem original ([Content_Types] etc.)
            if n not in drop:
                z.writestr(n, files[n])
        for n in files:
            if n not in zin.namelist():
                z.writestr(n, files[n])


# --------------------------------------------------------------------------- #
# Prévia
# --------------------------------------------------------------------------- #
PX = 50  # pixels por cm


def draw_text(img: Image.Image, d: ImageDraw.ImageDraw, box: Box, fonts: Fonts) -> None:
    li, ti, ri, bi = box.inset
    lines, h, _ = layout(box, fonts)
    inner_h = box.h - ti - bi
    oy = box.y + ti + ((inner_h - h) / 2 if box.anchor == "ctr" else 0)
    avail = (box.w - li - ri) / PT_CM
    for para, words, wpt, ytop, lh, ind, first in lines:
        fam = para.runs[0].font
        base_pt = ytop + lh * ASCENT[fam] * 0.9 + (lh - lh * 0.9) / 2
        xoff = (avail - ind - wpt) / 2 if para.align == "ctr" else 0
        if para.bullet and first:
            r = para.runs[0]
            d.text(((box.x + li) * PX, (oy + base_pt * PT_CM) * PX), "•",
                   font=fonts.get(r, int(r.size * PT_CM * PX)), fill="#" + RED, anchor="ls")
        for t, r, x in words:
            px = (box.x + li + (ind + xoff + x) * PT_CM) * PX
            d.text((px, (oy + base_pt * PT_CM) * PX), t,
                   font=fonts.get(r, max(1, int(round(r.size * PT_CM * PX)))),
                   fill="#" + r.color, anchor="ls")


def render(template: Path, spec: list, fonts: Fonts, out: Path) -> None:
    W_, H_ = 60.0075, 89.993
    img = Image.new("RGB", (int(W_ * PX), int(H_ * PX)), "white")
    d = ImageDraw.Draw(img)
    z = zipfile.ZipFile(template)

    def rect(x, y, w, h, fill=None, outline=None, width=1, radius=0):
        xy = [x * PX, y * PX, (x + w) * PX, (y + h) * PX]
        if radius:
            d.rounded_rectangle(xy, radius=radius * PX, fill=fill, outline=outline, width=width)
        else:
            d.rectangle(xy, fill=fill, outline=outline, width=width)

    # elementos do modelo (slide 2), nas posições do arquivo
    rect(647063 / EMU_CM, 517441 / EMU_CM, 20305873 / EMU_CM, 31003175 / EMU_CM,
         outline="#" + RED, width=max(1, int(0.079 * PX)))
    title = Box("Título", 1146122 / EMU_CM, TITLE_Y, 19307700 / EMU_CM, 6.3, [
        P(Run(TITLE_LINES[0], TITLE_PT, True, color="000000", font=TOMO), align="ctr",
          spacing=1.0, after=0),
        P(Run(TITLE_LINES[1], TITLE_PT, True, color="000000", font=TOMO), align="ctr",
          spacing=1.4, after=0)])
    draw_text(img, d, title, fonts)
    y = 4433530 / EMU_CM
    d.line([(6.0 * PX, y * PX), (54.0 * PX, y * PX)], fill="#" + RED,
           width=int(142875 / EMU_CM * PX))
    sub_ = Box("Subtítulo", 647100 / EMU_CM, 4669732 / EMU_CM, 20305800 / EMU_CM, 2.22,
               [P(Run("PROJETO ORIENTADO EM COMPUTAÇÃO I", 40, True, color="000000",
                      font=TOMO), align="ctr", spacing=1.4, after=0)],
               inset=(0.254, 0.254, 0.254, 0.254))
    draw_text(img, d, sub_, fonts)

    def paste(member, x, y, w, h, crop_tb=None):
        im = Image.open(z.open(member)).convert("RGBA")
        if crop_tb:                                   # fillRect com t/b negativos
            t, b = crop_tb
            full_h = h * (1 + t + b)
            im = im.resize((int(w * PX), int(full_h * PX)))
            top = int(t * h * PX)
            im = im.crop((0, top, int(w * PX), top + int(h * PX)))
        else:
            im = im.resize((int(w * PX), int(h * PX)))
        img.paste(im, (int(x * PX), int(y * PX)), im)

    paste("ppt/media/image2.png", 882105 / EMU_CM, 28199859 / EMU_CM, 3168199 / EMU_CM,
          3168199 / EMU_CM)
    paste("ppt/media/image3.png", 13136757 / EMU_CM, 29395576 / EMU_CM, 7317122 / EMU_CM,
          1777299 / EMU_CM, crop_tb=(0.53394, 0.48056))
    gx, gy, sc = 4086877 / EMU_CM, 30473553 / EMU_CM, 2133524 / 2844698
    paste("ppt/media/image4.png", gx, gy, 603698 * sc / EMU_CM, 606041 * sc / EMU_CM)
    draw_text(img, d, Box("insta", gx + 603698 * sc / EMU_CM, gy, 2241000 * sc / EMU_CM, 1.2,
                          [P(Run("@dcc.ufmg", 23.83, True, color="000000"), align="ctr",
                             spacing=1.0, after=0)]), fonts)
    draw_text(img, d, Box("url", 1620542 / EMU_CM, 31021829 / EMU_CM, 1691400 / EMU_CM, 0.7,
                          [P(Run("https://dcc.ufmg.br", 15.41, True, color="000000"),
                             align="ctr", spacing=1.0, after=0)]), fonts)

    for el in spec:
        if isinstance(el, Pic):
            im = Image.open(el.path).convert("RGB").resize((int(el.w * PX), int(el.h * PX)))
            img.paste(im, (int(el.x * PX), int(el.y * PX)))
            continue
        if el.shape == "rightArrow":
            x, y, w, h = el.x * PX, el.y * PX, el.w * PX, el.h * PX
            d.polygon([(x, y + h * 0.25), (x + w * 0.5, y + h * 0.25), (x + w * 0.5, y),
                       (x + w, y + h / 2), (x + w * 0.5, y + h), (x + w * 0.5, y + h * 0.75),
                       (x, y + h * 0.75)], fill="#" + el.fill)
            continue
        if el.fill or el.line:
            rect(el.x, el.y, el.w, el.h, fill=("#" + el.fill) if el.fill else None,
                 outline=("#" + el.line) if el.line else None, width=2,
                 radius=el.radius * min(el.w, el.h) if el.radius else 0)
        draw_text(img, d, el, fonts)
    out.parent.mkdir(parents=True, exist_ok=True)
    img.save(out)


def check(spec: list, fonts: Fonts) -> list[str]:
    """Texto que transborda a caixa, palavras mais largas que a caixa e sobreposições."""
    problems = []
    for el in spec:
        if isinstance(el, Box) and el.paras:
            _, h, wide = layout(el, fonts)
            inner = el.h - el.inset[1] - el.inset[3]
            if h > inner + 1e-6:
                problems.append(f"{el.name}: texto com {h:.2f} cm em {inner:.2f} cm")
            if wide:
                problems.append(f"{el.name}: palavra larga demais: {wide}")
    rects = [el for el in spec if isinstance(el, (Box, Pic)) and not
             (isinstance(el, Box) and el.name.startswith(("Título da seção", "Seção")))]
    for i, a in enumerate(rects):
        for b in rects[i + 1:]:
            if (a.x < b.x + b.w - 0.01 and b.x < a.x + a.w - 0.01 and
                    a.y < b.y + b.h - 0.01 and b.y < a.y + a.h - 0.01):
                problems.append(f"sobreposição: {a.name} × {b.name}")
    bottom = max(el.y + el.h for el in spec)
    if bottom > 77.6:
        problems.append(f"conteúdo termina em {bottom:.1f} cm (rodapé começa em 78,3 cm)")
    return problems


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description="Pôster do POC I (banner Mostra UFMG).")
    ap.add_argument("--fonts", type=Path, required=True,
                    help="pasta com as fontes Tomorrow e Titillium Web (.ttf)")
    ap.add_argument("--modelo", type=Path, default=OUT / "modelo_banner_mostraUFMG.pptx")
    ap.add_argument("--out", type=Path, default=OUT / "poster_POC_I.pptx")
    ap.add_argument("--previa", type=Path, default=OUT / "previa.png")
    args = ap.parse_args(argv)
    if hasattr(sys.stdout, "reconfigure"):
        sys.stdout.reconfigure(encoding="utf-8", errors="replace")
    fonts = Fonts(args.fonts)
    figs = make_figs(args.fonts)
    spec = build_spec(figs, fonts)
    problems = check(spec, fonts)
    for el in spec:
        if isinstance(el, Box) and el.paras:
            _, h, _ = layout(el, fonts)
            print(f"  {el.name:32s} texto {h:5.2f} cm / caixa {el.h - el.inset[1] - el.inset[3]:5.2f} cm")
    build_pptx(args.modelo, args.out, spec)
    render(args.modelo, spec, fonts, args.previa)
    print("problemas:", problems or "nenhum")
    print("pôster:", args.out, "| prévia:", args.previa)
    return 1 if problems else 0


if __name__ == "__main__":
    sys.exit(main())

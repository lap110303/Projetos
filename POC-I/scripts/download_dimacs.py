"""Baixa as instâncias DIMACS de coloração e monta o catálogo (conjunto de TESTE final).

Conjunto usado (127 instâncias):
  * as 79 instâncias da página original do desafio (M. Trick):
        https://mat.tepper.cmu.edu/COLOR/instances.html
    baixadas em texto (.col) do diretório do COLOR02/03/04 quando existem lá, e no
    formato binário original (.col.b) caso contrário (só as famílias flat);
  * as instâncias de coloração acrescentadas no COLOR02/03/04
        https://mat.tepper.cmu.edu/COLOR04/INSTANCES/
    k-Insertions, k-FullIns, *GPIA, mug, qg.order, wap0?a e C2000.5 / C4000.5.
Ficam de fora as GEOM* (coloração com banda/multicoloração), as wap0? sem "a" e os
demais .clq (clique máxima).

O número cromático conhecido (χ) e a sigla da fonte vêm das próprias páginas do
desafio; "?" ou ausente = desconhecido na fonte. Nada é preenchido à mão.

Saídas em data/dimacs/: instancias/ (arquivos baixados), fontes/ (cópia das páginas
usadas) e catalogo.csv (nome, url, χ, fonte, família, bytes, sha256). Arquivos já
baixados são mantidos (retomada).

Uso (a partir da pasta POC-I):
    python scripts/download_dimacs.py
"""
from __future__ import annotations

import hashlib
import re
import sys
import time
import urllib.request
from pathlib import Path

import pandas as pd

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / "data" / "dimacs"
BASE_ORIG = "https://mat.tepper.cmu.edu/COLOR/"
BASE_04 = "https://mat.tepper.cmu.edu/COLOR04/"
PAGES = {"COLOR_instances.html": BASE_ORIG + "instances.html",
         "COLOR04.html": BASE_04,
         "COLOR04_INSTANCES.html": BASE_04 + "INSTANCES/"}
EXTRA_04 = [r"^\d-(FullIns|Insertions)_\d\.col$", r"GPIA\.col$", r"^mug\d+_\d+\.col$",
            r"^qg\.order\d+\.col$", r"^wap0\da\.col$", r"^C(2000|4000)\.5\.clq$"]
FAMILIES = [  # (expressão no nome, família)
    (r"^DSJC", "DSJC (aleatório)"), (r"^DSJR", "DSJR (geométrico)"),
    (r"^flat", "flat (Culberson)"), (r"^le450", "le450 (Leighton)"),
    (r"^queen", "rainhas (SGB)"), (r"^myciel", "Mycielski"),
    (r"^(mulsol|zeroin|fpsol2|inithx)", "alocação de registradores"),
    (r"^school", "horário escolar"), (r"^(anna|david|homer|huck|jean)\.", "livros (SGB)"),
    (r"^games", "jogos (SGB)"), (r"^miles", "milhas (SGB)"),
    (r"^latin", "quadrado latino"), (r"(Insertions|FullIns)", "Insertions/FullIns"),
    (r"GPIA", "GPIA"), (r"^mug", "mug"), (r"^qg\.", "quasigrupos"),
    (r"^wap", "frequências (wap)"), (r"^C\d+\.5", "C (aleatório)"),
]
UA = {"User-Agent": "poc-coloracao-ufmg/1.0 (download de instancias de benchmark)"}


def fetch(url: str) -> bytes:
    req = urllib.request.Request(url, headers=UA)
    for attempt in range(4):
        try:
            with urllib.request.urlopen(req, timeout=120) as r:
                return r.read()
        except Exception as e:                       # rede instável: tenta de novo
            if attempt == 3:
                raise RuntimeError(f"falha ao baixar {url}: {e}") from e
            time.sleep(2 * (attempt + 1))
    raise AssertionError


def flat_text(html: str) -> str:
    return re.sub(r"\s+", " ", re.sub(r"<[^>]*>", " ", html))


def parse_chi(text: str) -> dict[str, tuple[float, str]]:
    """{nome do arquivo (sem .b): (χ ou NaN, sigla da fonte)} a partir do texto da página."""
    pat = re.compile(r"([A-Za-z0-9_.\-]+?\.(?:col(?:\.b)?|clq))\s*\(?\s*(\d+)\s*,\s*(\d+)\s*\)?"
                     r"\s*,\s*(\d+|\?)\s*,?\s*([A-Z]{2,4})?")
    out = {}
    for name, _n, _m, chi, tag in pat.findall(text):
        name = name.replace("mugg", "mug").removesuffix(".b")
        out.setdefault(name, (float(chi) if chi != "?" else float("nan"), tag or ""))
    return out


def family(name: str) -> str:
    return next((f for pat, f in FAMILIES if re.search(pat, name)), "outra")


def main() -> int:
    if hasattr(sys.stdout, "reconfigure"):
        sys.stdout.reconfigure(encoding="utf-8", errors="replace")
    (OUT / "instancias").mkdir(parents=True, exist_ok=True)
    (OUT / "fontes").mkdir(parents=True, exist_ok=True)
    html = {}
    for fname, url in PAGES.items():
        html[fname] = fetch(url).decode("latin-1")
        (OUT / "fontes" / fname).write_text(html[fname], encoding="utf-8")

    # sem exigir 'HREF="' antes: a página tem uma âncora malformada (linha do
    # latin_square_10) que engole o início do link do le450_15a
    orig = re.findall(r'instances/([A-Za-z0-9_.\-]+\.col(?:\.b)?)"', html["COLOR_instances.html"],
                      flags=re.I)
    listing = set(re.findall(r'href="([^"?/][^"]*)"', html["COLOR04_INSTANCES.html"]))
    items = []                                       # (nome, url)
    for f in dict.fromkeys(orig):
        name = f.removesuffix(".b")
        if name in listing:
            items.append((name, BASE_04 + "INSTANCES/" + name))
        else:
            items.append((f, BASE_ORIG + "instances/" + f))
    have = {n.removesuffix(".b") for n, _ in items}
    for f in sorted(listing):
        if f not in have and any(re.search(p, f) for p in EXTRA_04):
            items.append((f, BASE_04 + "INSTANCES/" + f))

    chi = parse_chi(flat_text(html["COLOR_instances.html"]))
    for k, v in parse_chi(flat_text(html["COLOR04.html"])).items():
        if k not in chi or (pd.isna(chi[k][0]) and not pd.isna(v[0])):
            chi[k] = v

    rows = []
    for i, (fname, url) in enumerate(items, 1):
        path = OUT / "instancias" / fname
        if not path.exists():
            data = fetch(url)
            tmp = path.with_name(path.name + ".tmp")
            tmp.write_bytes(data)
            tmp.replace(path)
            time.sleep(0.3)                          # gentileza com o servidor
            print(f"  {i:3d}/{len(items)} {fname} ({len(data) / 1e6:.2f} MB)", flush=True)
        data = path.read_bytes()
        key = fname.removesuffix(".b")
        c, tag = chi.get(key, (float("nan"), ""))
        rows.append({"instancia": key.rsplit(".", 1)[0], "arquivo": fname, "url": url,
                     "familia": family(key), "fonte": tag, "chi_conhecido": c,
                     "bytes": len(data), "sha256": hashlib.sha256(data).hexdigest()})
    cat = pd.DataFrame(rows).sort_values("instancia").reset_index(drop=True)
    cat.to_csv(OUT / "catalogo.csv", index=False, encoding="utf-8")
    print(f"{len(cat)} instâncias, {cat.bytes.sum() / 1e6:.1f} MB; χ conhecido em "
          f"{cat.chi_conhecido.notna().sum()}; catálogo em {OUT / 'catalogo.csv'}")
    return 0


if __name__ == "__main__":
    sys.exit(main())

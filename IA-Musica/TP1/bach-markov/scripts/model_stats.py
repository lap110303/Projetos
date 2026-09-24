#!/usr/bin/env python3
"""Estatísticas do corpus e da esparsidade dos modelos por ordem (Seção 3 do artigo)."""
import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))
from bachmarkov.corpus import load_corpus  # noqa: E402
from bachmarkov.markov import HarmonyMarkov, MelodyMarkov  # noqa: E402

for mode in ["major", "minor"]:
    d = load_corpus(mode)
    h = HarmonyMarkov().fit(d)
    print(f"{mode}: {len(d)} corais, {sum(len(c['events']) for c in d)} eventos, "
          f"{len(h.vocab)} verticais distintas, {h.C.nnz} transições verticais distintas")
    for n in range(1, 7):
        tab = MelodyMarkov(n).fit(d).tables[n]
        tot = sum(sum(c.values()) for c in tab.values())
        det = sum(sum(c.values()) for c in tab.values() if len(c) == 1) / tot
        br = sum(len(c) * sum(c.values()) for c in tab.values()) / tot
        print(f"  n={n}: {len(tab):5d} contextos | sucessores médios {br:5.2f} | "
              f"contextos determinísticos {det:.0%}")

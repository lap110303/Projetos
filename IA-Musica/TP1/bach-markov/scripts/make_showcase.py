#!/usr/bin/env python3
"""Gera as peças de demonstração referenciadas no artigo (MIDI, MusicXML, JSON e MP3)
e uma tabela com as métricas de cada uma (outputs/showcase.csv)."""
import csv
import json
import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))
from bachmarkov.corpus import load_corpus  # noqa: E402
from bachmarkov.evaluate import CorpusIndex, parallels_rate, stylistic_distance  # noqa: E402
from bachmarkov.export import to_audio, to_midi, to_musicxml  # noqa: E402
from bachmarkov.generate import generate_chorale  # noqa: E402

OUT = os.path.join(os.path.dirname(__file__), "..", "outputs")

# (id, ordem, tau, alpha, modo, tonalidade, semente, descrição)
SHOWCASE = [
    ("coral01", 1, 1.0, 0.1, "major", "C", 7, "ordem baixa: frases irregulares"),
    ("coral02", 3, 1.0, 0.1, "major", "C", 7, "configuração principal"),
    ("coral03", 3, 1.0, 0.1, "major", "C", 8, "mesma configuração, outra semente"),
    ("coral04", 2, 1.5, 0.1, "minor", "D", 3, "modo menor, temperatura alta"),
    ("coral05", 6, 1.0, 0.1, "major", "C", 7, "ordem alta: memorização"),
    ("coral06", 3, 1.0, 100.0, "major", "C", 7, "harmonia quase sem memória (alpha alto)"),
]


def main():
    for sub in ["midi", "musicxml", "json", "audio"]:
        os.makedirs(os.path.join(OUT, sub), exist_ok=True)
    idx = {m: CorpusIndex(load_corpus(m)) for m in ["major", "minor"]}
    table = []
    for cid, n, tau, al, mode, key, seed, desc in SHOWCASE:
        ev, meta = generate_chorale(n, tau, al, mode, seed)
        L, src = idx[mode].longest_copy(ev)
        st = stylistic_distance([ev], [c["events"] for c in load_corpus(mode)])
        row = dict(id=cid, order=n, tau=tau, alpha=al, mode=mode, key=key, seed=seed,
                   n_notes=len(ev), longest_copy=L, copy_source=src,
                   copy_ratio=round(L / len(ev), 3), phrases=sum(e["fer"] for e in ev),
                   phrase_len_sd=round(st["phrase_len_sd"], 2),
                   parallels=round(parallels_rate([ev]), 3), description=desc)
        table.append(row)
        mid = os.path.join(OUT, "midi", cid + ".mid")
        pm = to_midi(ev, mid, mode, key)
        to_musicxml(ev, os.path.join(OUT, "musicxml", cid + ".musicxml"), mode, key, cid)
        with open(os.path.join(OUT, "json", cid + ".json"), "w") as f:
            json.dump({"meta": {**meta, "key": key, "description": desc}, "events": ev}, f, indent=1)
        audio = to_audio(mid, os.path.join(OUT, "audio", cid + ".mp3"))
        row["duration_s"] = round(pm.get_end_time(), 1)
        print(f"{cid}: n={n} tau={tau} alpha={al} {mode}/{key} seed={seed} | {len(ev)} notas, "
              f"{row['phrases']} frases, cópia máx={L} ({src}), paralelas={row['parallels']} -> {audio}")
    with open(os.path.join(OUT, "showcase.csv"), "w", newline="") as f:
        w = csv.DictWriter(f, fieldnames=list(table[0].keys()))
        w.writeheader()
        w.writerows(table)


if __name__ == "__main__":
    main()

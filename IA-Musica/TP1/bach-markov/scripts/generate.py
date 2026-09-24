#!/usr/bin/env python3
"""Gera um ou mais corais e salva .mid, .musicxml, .json e (opcional) áudio.

Exemplos:
  python scripts/generate.py --order 3 --seeds 1 2 3
  python scripts/generate.py --order 2 --tau 1.5 --alpha 1.0 --mode minor --key D --audio
"""
import argparse
import json
import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))
from bachmarkov.export import to_audio, to_midi, to_musicxml  # noqa: E402
from bachmarkov.generate import generate_chorale  # noqa: E402


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--order", type=int, default=3, help="ordem n da cadeia melódica")
    ap.add_argument("--tau", type=float, default=1.0, help="temperatura de amostragem")
    ap.add_argument("--alpha", type=float, default=0.1, help="suavização da cadeia harmônica")
    ap.add_argument("--mode", choices=["major", "minor"], default="major")
    ap.add_argument("--key", default=None, help="tonalidade de saída (ex.: G, Bb). Padrão: C / A")
    ap.add_argument("--seeds", type=int, nargs="+", default=[0])
    ap.add_argument("--min-len", type=int, default=32)
    ap.add_argument("--max-len", type=int, default=80)
    ap.add_argument("--bpm", type=int, default=72)
    ap.add_argument("--out", default="outputs")
    ap.add_argument("--name", default=None, help="prefixo dos arquivos")
    ap.add_argument("--audio", action="store_true", help="renderiza .mp3 com FluidSynth")
    a = ap.parse_args()

    for seed in a.seeds:
        ev, meta = generate_chorale(a.order, a.tau, a.alpha, a.mode, seed, a.min_len, a.max_len)
        name = a.name or f"coral_n{a.order}_t{a.tau}_a{a.alpha}_{a.mode}"
        if len(a.seeds) > 1 or not a.name:
            name += f"_s{seed}"
        meta["key"] = a.key or ("C" if a.mode == "major" else "A")
        base = os.path.join(a.out, name)
        for sub in ["midi", "musicxml", "json", "audio"]:
            os.makedirs(os.path.join(a.out, sub), exist_ok=True)
        mid = os.path.join(a.out, "midi", name + ".mid")
        to_midi(ev, mid, a.mode, a.key, a.bpm)
        to_musicxml(ev, os.path.join(a.out, "musicxml", name + ".musicxml"), a.mode, a.key, name)
        with open(os.path.join(a.out, "json", name + ".json"), "w") as f:
            json.dump({"meta": meta, "events": ev}, f, indent=1)
        msg = f"[ok] {name}: {meta['n_notes']} notas"
        if a.audio:
            msg += " -> " + to_audio(mid, os.path.join(a.out, "audio", name + ".mp3"))
        print(msg)


if __name__ == "__main__":
    main()

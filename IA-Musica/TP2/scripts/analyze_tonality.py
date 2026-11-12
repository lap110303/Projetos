"""Apoio à análise qualitativa: estabilidade tonal, cadência final e nº de vozes.

Para cada peça estima a tonalidade (Krumhansl–Schmuckler, histograma de classes de
altura ponderado pela duração) da peça inteira, dos 4 primeiros e dos 4 últimos
compassos; verifica se o último baixo é a tônica e mede a fração de ataques em que
soam exatamente 4 notas. Saída: results/tonality.json
"""
import json
import sys
from pathlib import Path

import numpy as np
import pretty_midi

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from chorales.data import ROOT, TEMPO_BPM  # noqa: E402

MAJOR = np.array([6.35, 2.23, 3.48, 2.33, 4.38, 4.09, 2.52, 5.19, 2.39, 3.66, 2.29, 2.88])
MINOR = np.array([6.33, 2.68, 3.52, 5.38, 2.60, 3.53, 2.54, 4.75, 3.98, 2.69, 3.34, 3.17])
BAR_S = 4 * 60.0 / TEMPO_BPM
NAMES = ["C", "C#", "D", "Eb", "E", "F", "F#", "G", "Ab", "A", "Bb", "B"]


def key_of(notes, t0=None, t1=None):
    h = np.zeros(12)
    for n in notes:
        s, e = n.start, n.end
        if t0 is not None:
            s, e = max(s, t0), min(e, t1)
        if e > s:
            h[n.pitch % 12] += e - s
    if h.sum() == 0:
        return None
    best = max(((np.corrcoef(h, np.roll(prof, k))[0, 1], k, mode)
                for prof, mode in ((MAJOR, "maior"), (MINOR, "menor")) for k in range(12)))
    return best[1], best[2]


def analyze(path):
    notes = pretty_midi.PrettyMIDI(str(path)).instruments[0].notes
    end = max(n.end for n in notes)
    k_all = key_of(notes)
    k_first = key_of(notes, 0, 4 * BAR_S + BAR_S)          # inclui a anacruse
    k_last = key_of(notes, end - 4 * BAR_S, end)
    last_on = max(n.start for n in notes)
    final = [n.pitch for n in notes if n.start >= last_on - 1e-6 or n.end >= end - 1e-6]
    bass_pc = min(final) % 12
    onsets = {}
    for n in notes:
        onsets.setdefault(round(n.start, 4), 0)
    for n in notes:                                        # nº de notas soando em cada ataque
        for t in onsets:
            if n.start <= t < n.end:
                onsets[t] += 1
    four = np.mean([v == 4 for v in onsets.values()])
    return {"key": f"{NAMES[k_all[0]]} {k_all[1]}", "stable": k_first == k_last,
            "first": f"{NAMES[k_first[0]]} {k_first[1]}", "last": f"{NAMES[k_last[0]]} {k_last[1]}",
            "final_bass_is_tonic_of_last": bass_pc == k_last[0],
            "final_bass_is_tonic_of_first": bass_pc == k_first[0],
            "frac_onsets_4_voices": float(four)}


def main():
    sets = {"test": sorted((ROOT / "data/midi/test_ref").glob("*.mid")),
            "transformer": sorted((ROOT / "samples/transformer/eval").glob("*.mid")),
            "markov": sorted((ROOT / "samples/markov/eval").glob("*.mid"))}
    out = {"summary": {}, "showcase": {}}
    for name, files in sets.items():
        res = [analyze(f) for f in files]
        out["summary"][name] = {
            "n": len(res),
            "same_key_first_last": float(np.mean([r["stable"] for r in res])),
            "ends_on_tonic_of_opening_key": float(np.mean([r["final_bass_is_tonic_of_first"] for r in res])),
            "ends_on_tonic_of_final_key": float(np.mean([r["final_bass_is_tonic_of_last"] for r in res])),
            "frac_onsets_4_voices": float(np.mean([r["frac_onsets_4_voices"] for r in res])),
        }
        print(name, out["summary"][name])
    for d in ["transformer", "markov", "human"]:
        for f in sorted((ROOT / "samples" / d / "showcase").glob("*.mid")):
            out["showcase"][f.name] = analyze(f)
            print(f.name, out["showcase"][f.name])
    (ROOT / "results/tonality.json").write_text(json.dumps(out, indent=2))


if __name__ == "__main__":
    main()

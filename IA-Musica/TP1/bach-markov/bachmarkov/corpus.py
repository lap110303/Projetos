"""Extração e cache do corpus de corais de Bach (music21.corpus).

Cada coral em 4/4 com quatro vozes é transposto para Dó maior (modo maior)
ou Lá menor (modo menor) e convertido em uma sequência de eventos, um por
nota do soprano:

    evento = {"s": altura MIDI do soprano,
              "dur": duração em semínimas,
              "beat": posição métrica no compasso (0.0 .. 3.75),
              "fer": 1 se a nota tem fermata (fim de frase), senão 0,
              "atb": [contralto, tenor, baixo] soando no ataque da nota}
"""
from __future__ import annotations

import json
import os
from fractions import Fraction

from music21 import corpus, converter, expressions, interval, pitch

CACHE = os.path.join(os.path.dirname(__file__), "..", "data", "chorales.json")
VALID_DURS = {0.25, 0.5, 0.75, 1.0, 1.5, 2.0, 3.0, 4.0}


def _q(x) -> float:
    return float(Fraction(x).limit_denominator(12))


def _notes_with_offsets(part):
    part = part.stripTies()
    out = []
    for n in part.recurse().notesAndRests:
        out.append((_q(n.getOffsetInHierarchy(part)), _q(n.quarterLength), n))
    return out


def _sounding(notes, t):
    for off, dur, n in notes:
        if off <= t < off + dur:
            return n.pitches[-1].midi if (n.isNote or n.isChord) else None
    return None


def parse_chorale(path):
    s = converter.parse(path)
    ts = s.recurse().getElementsByClass("TimeSignature")
    if not ts or ts[0].ratioString != "4/4" or len(s.parts) != 4:
        return None
    k = s.analyze("key")
    target = pitch.Pitch("C") if k.mode == "major" else pitch.Pitch("A")
    semis = (target.pitchClass - k.tonic.pitchClass) % 12
    if semis > 6:
        semis -= 12
    s = s.transpose(interval.Interval(semis))
    voices = [_notes_with_offsets(p) for p in s.parts]  # S, A, T, B

    events = []
    for off, dur, n in voices[0]:
        if not n.isNote:  # ignora pausas do soprano
            continue
        if dur not in VALID_DURS:
            return None
        beat = _q(n.beat - 1) if n.beat is not None else 0.0
        fer = int(any(isinstance(e, expressions.Fermata) for e in n.expressions))
        atb = [_sounding(v, off) for v in voices[1:]]
        if None in atb:
            return None
        events.append({"s": n.pitch.midi, "dur": dur, "beat": beat,
                       "fer": fer, "atb": atb})
    if len(events) < 16 or not any(e["fer"] for e in events):
        return None
    events[-1]["fer"] = 1  # a última nota sempre encerra frase
    return {"mode": k.mode, "events": events}


def build_corpus(verbose=True):
    paths = corpus.getComposer("bach")
    data, seen = [], set()
    for i, p in enumerate(paths):
        try:
            ch = parse_chorale(p)
        except Exception:
            ch = None
        if ch is None:
            continue
        key = tuple((e["s"], e["dur"]) for e in ch["events"])
        if key in seen:  # remove harmonizações duplicadas da mesma melodia
            continue
        seen.add(key)
        ch["name"] = os.path.basename(str(p))
        data.append(ch)
        if verbose and len(data) % 50 == 0:
            print(f"  {len(data)} corais extraídos ({i+1}/{len(paths)} arquivos)")
    return data


def load_corpus(mode=None, rebuild=False):
    if rebuild or not os.path.exists(CACHE):
        print("Extraindo corpus de corais de Bach (music21)...")
        data = build_corpus()
        os.makedirs(os.path.dirname(CACHE), exist_ok=True)
        with open(CACHE, "w") as f:
            json.dump(data, f)
    with open(CACHE) as f:
        data = json.load(f)
    if mode:
        data = [c for c in data if c["mode"] == mode]
    return data


if __name__ == "__main__":
    d = load_corpus(rebuild=True)
    maj = sum(c["mode"] == "major" for c in d)
    print(f"Total: {len(d)} corais ({maj} maiores, {len(d)-maj} menores)")

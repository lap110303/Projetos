"""Pipeline de geração: melodia (ordem n) -> harmonização (ordem 1) -> arquivos."""
from __future__ import annotations

import numpy as np

from .corpus import load_corpus
from .markov import HarmonyMarkov, MelodyMarkov

_CACHE = {}


def get_models(order, alpha, mode):
    key = (order, alpha, mode)
    if key not in _CACHE:
        data = load_corpus(mode)
        _CACHE[key] = (MelodyMarkov(order).fit(data), HarmonyMarkov(alpha).fit(data), data)
    return _CACHE[key]


def generate_chorale(order=2, tau=1.0, alpha=0.1, mode="major", seed=0,
                     min_len=32, max_len=80, harmonize=True):
    rng = np.random.default_rng(seed)
    mel_model, har_model, _ = get_models(order, alpha, mode)
    melody, tries = mel_model.generate(rng, tau, min_len, max_len, mode)
    events = []
    verticals = har_model.harmonize(melody, rng, mode) if harmonize else None
    for i, (s, dur, beat, fer) in enumerate(melody):
        atb = list(verticals[i][1:4]) if verticals else []
        events.append({"s": s, "dur": dur, "beat": beat, "fer": fer, "atb": atb})
    meta = dict(order=order, tau=tau, alpha=alpha, mode=mode, seed=seed,
                n_notes=len(events), rejection_tries=tries)
    return events, meta

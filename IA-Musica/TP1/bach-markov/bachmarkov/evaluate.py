"""Métricas de avaliação objetiva.

Originalidade / memorização
  * longest_copy: maior trecho contíguo de notas (altura, duração) da peça
    gerada que aparece literalmente em algum coral do corpus.
  * copy_ratio: longest_copy / nº de notas.
  * novelty_k: fração dos k-gramas (k=8) da peça que não existem no corpus.
Fidelidade estilística (peças agrupadas vs. corpus)
  * Divergência de Jensen-Shannon (bits) entre histogramas de classe de altura,
    intervalo melódico e duração.
  * Comprimento médio de frase (notas entre fermatas).
Diversidade
  * Distância de edição normalizada média entre pares de peças da mesma configuração.
Harmonia
  * Taxa de quintas/oitavas paralelas por transição entre verticais.
"""
from __future__ import annotations

import itertools
from collections import Counter

import numpy as np
from scipy.spatial.distance import jensenshannon


def md_tokens(events):
    return [(e["s"], e["dur"]) for e in events]


class CorpusIndex:
    def __init__(self, chorales):
        self.names = [c["name"] for c in chorales]
        self.vocab = {}
        ids, owner = [], []
        for ci, c in enumerate(chorales):
            for t in md_tokens(c["events"]):
                ids.append(self.vocab.setdefault(t, len(self.vocab)))
                owner.append(ci)
            ids.append(-1 - ci)  # separador único
            owner.append(ci)
        self.C = np.array(ids)
        self.owner = np.array(owner)
        self.ngrams = {}

    def _encode(self, events):
        return np.array([self.vocab.get(t, -10**6) for t in md_tokens(events)])

    def longest_copy(self, events):
        g = self._encode(events)
        prev = np.zeros(len(self.C), dtype=int)
        best, where = 0, None
        for gi in g:
            cur = np.zeros_like(prev)
            m = self.C == gi
            cur[1:][m[1:]] = prev[:-1][m[1:]] + 1
            cur[0] = int(m[0])
            j = int(cur.argmax())
            if cur[j] > best:
                best, where = int(cur[j]), self.names[self.owner[j]]
            prev = cur
        return best, where

    def novelty(self, events, k=8):
        if k not in self.ngrams:
            s = set()
            C = self.C.tolist()
            for i in range(len(C) - k + 1):
                w = C[i:i + k]
                if min(w) >= 0:
                    s.add(tuple(w))
            self.ngrams[k] = s
        g = self._encode(events).tolist()
        grams = [tuple(g[i:i + k]) for i in range(len(g) - k + 1)]
        return 1.0 - sum(x in self.ngrams[k] for x in grams) / max(1, len(grams))


def _hist(values, bins):
    c = Counter(values)
    h = np.array([c.get(b, 0) for b in bins], float) + 1e-9
    return h / h.sum()


def features(pieces):
    pcs, ints, durs, phr = [], [], [], []
    for ev in pieces:
        s = [e["s"] for e in ev]
        pcs += [p % 12 for p in s]
        ints += [int(np.clip(b - a, -12, 12)) for a, b in zip(s, s[1:])]
        durs += [e["dur"] for e in ev]
        n = 0
        for e in ev:
            n += 1
            if e["fer"]:
                phr.append(n)
                n = 0
    return pcs, ints, durs, phr


DUR_BINS = [0.25, 0.5, 0.75, 1.0, 1.5, 2.0, 3.0, 4.0]


def stylistic_distance(gen, ref):
    g, r = features(gen), features(ref)
    js = lambda a, b, bins: float(jensenshannon(_hist(a, bins), _hist(b, bins), base=2) ** 2)
    return {
        "js_pitchclass": js(g[0], r[0], range(12)),
        "js_interval": js(g[1], r[1], range(-12, 13)),
        "js_duration": js(g[2], r[2], DUR_BINS),
        "phrase_len": float(np.mean(g[3])),
        "phrase_len_sd": float(np.std(g[3])),
        "stepwise": float(np.mean([abs(i) <= 2 for i in g[1]])),
    }


def edit_distance(a, b):
    prev = list(range(len(b) + 1))
    for i, x in enumerate(a, 1):
        cur = [i]
        for j, y in enumerate(b, 1):
            cur.append(min(prev[j] + 1, cur[j - 1] + 1, prev[j - 1] + (x != y)))
        prev = cur
    return prev[-1] / max(len(a), len(b))


def diversity(pieces):
    toks = [md_tokens(p) for p in pieces]
    return float(np.mean([edit_distance(a, b) for a, b in itertools.combinations(toks, 2)]))


def parallels_rate(pieces):
    bad = tot = 0
    for ev in pieces:
        vs = [[e["s"], *e["atb"]] for e in ev]
        for a, b in zip(vs, vs[1:]):
            tot += 1
            for i, j in itertools.combinations(range(4), 2):
                i1, i2 = (a[i] - a[j]) % 12, (b[i] - b[j]) % 12
                if i1 == i2 and i1 in (0, 7) and a[i] != b[i] and a[j] != b[j] \
                        and np.sign(b[i] - a[i]) == np.sign(b[j] - a[j]):
                    bad += 1
                    break
    return bad / max(1, tot)

"""Modelos de Markov para geração de corais.

1. MelodyMarkov: cadeia de Markov de ordem n sobre eventos do soprano
   x_t = (altura, duração, posição métrica, fermata).
       P(x_t | x_{t-n}, ..., x_{t-1}) = c(contexto, x_t) / c(contexto)
   Probabilidades estimadas por máxima verossimilhança (contagens) no corpus,
   com temperatura tau: p_i ∝ c_i^(1/tau), e back-off para contextos não vistos.

2. HarmonyMarkov: cadeia de ordem 1 sobre sonoridades verticais
   v_t = (S, A, T, B, fermata). Dada a melodia gerada, amostra-se a sequência
   de verticais *condicionada* a que a voz S de cada v_t seja igual à nota do
   soprano (restrições unárias, cf. Pachet & Roy, 2011). A amostragem é exata
   via forward-filtering / backward-sampling:
       P(v'|v) = (c(v,v') + alpha * P0(v')) / (c(v) + alpha)
   onde P0 é a distribuição unigrama e alpha controla a suavização.
"""
from __future__ import annotations

from collections import Counter, defaultdict

import numpy as np
from scipy import sparse

START, END = ("<s>",), ("</s>",)
TONIC_PC = {"major": 0, "minor": 9}


def melody_token(e):
    return (e["s"], e["dur"], e["beat"], e["fer"])


class MelodyMarkov:
    def __init__(self, order: int = 2):
        if order < 1:
            raise ValueError("ordem deve ser >= 1")
        self.order = order
        # tabelas para todas as ordens k <= n (usadas no back-off)
        self.tables = {k: defaultdict(Counter) for k in range(0, order + 1)}

    def fit(self, chorales):
        n = self.order
        for ch in chorales:
            seq = [START] * n + [melody_token(e) for e in ch["events"]] + [END]
            for i in range(n, len(seq)):
                for k in range(0, n + 1):
                    ctx = tuple(seq[i - k:i])
                    self.tables[k][ctx][seq[i]] += 1
        return self

    def distribution(self, context):
        for k in range(self.order, -1, -1):  # back-off: n, n-1, ..., 0
            ctx = tuple(context[len(context) - k:]) if k else ()
            if ctx in self.tables[k]:
                return self.tables[k][ctx], k
        raise RuntimeError("modelo vazio")

    def sample(self, rng, tau=1.0, max_len=120):
        context = [START] * self.order
        out, backoffs = [], 0
        while len(out) < max_len:
            counts, k = self.distribution(context)
            backoffs += k < self.order
            toks = list(counts.keys())
            w = np.array([counts[t] for t in toks], dtype=float) ** (1.0 / tau)
            tok = toks[rng.choice(len(toks), p=w / w.sum())]
            if tok == END:
                return out, backoffs
            out.append(tok)
            context.append(tok)
        return None, backoffs  # não terminou

    def generate(self, rng, tau=1.0, min_len=32, max_len=80, mode="major",
                 max_tries=5000):
        """Amostragem por rejeição: aceita a melodia se o comprimento estiver
        em [min_len, max_len] e a última nota for a tônica."""
        tonic = TONIC_PC[mode]
        for attempt in range(1, max_tries + 1):
            seq, _ = self.sample(rng, tau, max_len)
            if seq and min_len <= len(seq) and seq[-1][0] % 12 == tonic:
                return seq, attempt
        raise RuntimeError("nenhuma melodia satisfez as restrições")


class HarmonyMarkov:
    def __init__(self, alpha: float = 0.1):
        self.alpha = alpha

    @staticmethod
    def vertical(e):
        return (e["s"], *e["atb"], e["fer"])

    def fit(self, chorales):
        vocab, rows, cols, first = {}, [], [], Counter()
        idx = lambda v: vocab.setdefault(v, len(vocab))
        for ch in chorales:
            vs = [idx(self.vertical(e)) for e in ch["events"]]
            first[vs[0]] += 1
            rows += vs[:-1]
            cols += vs[1:]
        V = len(vocab)
        self.vocab = vocab
        self.inv = list(vocab.keys())
        self.C = sparse.csr_matrix((np.ones(len(rows)), (rows, cols)), shape=(V, V))
        uni = np.bincount(rows + cols, minlength=V).astype(float)
        self.P0 = uni / uni.sum()
        self.row_tot = np.asarray(self.C.sum(axis=1)).ravel()
        self.first = np.array([first[i] for i in range(V)], float)
        self.by_sfer = defaultdict(list)
        self.by_s = defaultdict(list)
        for v, i in vocab.items():
            self.by_sfer[(v[0], v[4])].append(i)
            self.by_s[v[0]].append(i)
        return self

    def _trans(self, I, J):
        """Matriz |I|x|J| de P(j|i) suavizada."""
        C = self.C[I][:, J].toarray()
        return (C + self.alpha * self.P0[J][None, :]) / (self.row_tot[I][:, None] + self.alpha)

    def harmonize(self, melody, rng, mode="major"):
        tonic = TONIC_PC[mode]
        allowed = []
        for s, _, _, fer in melody:
            cand = self.by_sfer.get((s, fer)) or self.by_s.get(s)
            if not cand:
                raise ValueError(f"altura {s} nunca aparece no soprano do corpus")
            allowed.append(np.array(cand))
        last = [i for i in allowed[-1] if self.inv[i][3] % 12 == tonic]
        if last:  # cadência final: baixo na tônica
            allowed[-1] = np.array(last)

        # forward filtering
        a0 = self.first[allowed[0]] + self.alpha * self.P0[allowed[0]]
        alphas, mats = [a0 / a0.sum()], []
        for t in range(1, len(allowed)):
            M = self._trans(allowed[t - 1], allowed[t])
            a = alphas[-1] @ M
            alphas.append(a / a.sum())
            mats.append(M)
        # backward sampling
        j = rng.choice(len(allowed[-1]), p=alphas[-1])
        path = [allowed[-1][j]]
        for t in range(len(allowed) - 2, -1, -1):
            w = alphas[t] * mats[t][:, j]
            j = rng.choice(len(w), p=w / w.sum())
            path.append(allowed[t][j])
        path.reverse()
        return [self.inv[i] for i in path]

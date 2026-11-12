"""Baseline: cadeia de Markov de ordem n sobre os mesmos tokens REMI.

Estende a cadeia do TP1 (máxima verossimilhança + back-off) para uma distribuição
própria em todo o vocabulário, via interpolação de Witten-Bell:

    P_k(w | h_k) = (c(h_k, w) + T(h_k) P_{k-1}(w | h_{k-1})) / (c(h_k) + T(h_k)),

em que T(h) é o nº de tokens distintos observados após o contexto h e P_0 é o
unigrama com suavização de Laplace. Contextos nunca vistos herdam P_{k-1}.
As tabelas de contagem são indexadas por hash de 64 bits para caber na memória.
"""
from __future__ import annotations

import numpy as np

_P1 = np.uint64(1_000_003)
_P2 = np.uint64(0x9E3779B97F4A7C15)


def _ctx_hash(arr: np.ndarray, pos: np.ndarray, k: int) -> np.ndarray:
    """Hash dos k tokens que antecedem cada posição ``pos`` em ``arr``."""
    h = np.zeros(len(pos), dtype=np.uint64)
    with np.errstate(over="ignore"):
        for j in range(k, 0, -1):
            h = h * _P1 + (arr[pos - j].astype(np.uint64) + np.uint64(1))
    return h


def _pair_hash(ctx: np.ndarray, w: np.ndarray) -> np.ndarray:
    with np.errstate(over="ignore"):
        return (ctx ^ (ctx >> np.uint64(29))) * _P2 + w.astype(np.uint64)


class _Table:
    def __init__(self, ctx: np.ndarray, w: np.ndarray):
        pair = _pair_hash(ctx, w)
        self.pair_keys, self.pair_c = np.unique(pair, return_counts=True)
        self.ctx_keys, self.ctx_c = np.unique(ctx, return_counts=True)
        # nº de tipos por contexto = nº de pares distintos com aquele contexto
        uniq_pairs = np.unique(np.stack([ctx, w.astype(np.uint64)]), axis=1)
        _, self.ctx_T = np.unique(uniq_pairs[0], return_counts=True)

    @staticmethod
    def _lookup(keys, vals, q):
        i = np.searchsorted(keys, q)
        i = np.minimum(i, len(keys) - 1)
        hit = keys[i] == q
        return np.where(hit, vals[i], 0)

    def ctx_stats(self, q):
        i = np.minimum(np.searchsorted(self.ctx_keys, q), len(self.ctx_keys) - 1)
        hit = self.ctx_keys[i] == q
        return np.where(hit, self.ctx_c[i], 0), np.where(hit, self.ctx_T[i], 0)

    def pair_count(self, ctx, w):
        return self._lookup(self.pair_keys, self.pair_c, _pair_hash(ctx, w))


class NGramLM:
    def __init__(self, order: int, vocab_size: int, pad_id: int):
        self.order, self.V, self.pad = order, vocab_size, pad_id
        self.tables: list[_Table] = []

    # ------------------------------------------------------------------ dados
    def _flatten(self, seqs):
        """Concatena as sequências com ``order`` PADs à esquerda de cada uma."""
        parts, targets = [], []
        offset = 0
        for s in seqs:
            s = np.asarray(s, dtype=np.int64)
            parts.append(np.full(self.order, self.pad, dtype=np.int64))
            parts.append(s)
            start = offset + self.order
            targets.append(np.arange(start + 1, start + len(s)))   # prevê s[1:]
            offset += self.order + len(s)
        return np.concatenate(parts), np.concatenate(targets)

    def fit(self, seqs):
        arr, pos = self._flatten(seqs)
        w = arr[pos]
        self.unigram = (np.bincount(w, minlength=self.V) + 1.0)
        self.unigram /= self.unigram.sum()
        self.tables = [_Table(_ctx_hash(arr, pos, k), w) for k in range(1, self.order + 1)]
        return self

    # ------------------------------------------------------------ avaliação
    def token_logprobs(self, seqs, order: int | None = None) -> np.ndarray:
        """log P de cada token-alvo (s[1:]) usando as ordens 0..order."""
        order = self.order if order is None else order
        arr, pos = self._flatten(seqs)
        w = arr[pos]
        p = self.unigram[w]
        for k in range(1, order + 1):
            ctx = _ctx_hash(arr, pos, k)
            c_h, T_h = self.tables[k - 1].ctx_stats(ctx)
            c_hw = self.tables[k - 1].pair_count(ctx, w)
            seen = c_h > 0
            p = np.where(seen, (c_hw + T_h * p) / np.maximum(c_h + T_h, 1), p)
        return np.log(p)

    # ------------------------------------------------------------ amostragem
    def next_dist(self, history: list[int]) -> np.ndarray:
        hist = [self.pad] * self.order + list(history)
        arr = np.asarray(hist, dtype=np.int64)
        all_w = np.arange(self.V)
        p = self.unigram.copy()
        for k in range(1, self.order + 1):
            ctx = _ctx_hash(arr, np.array([len(arr)]), k)
            c_h, T_h = self.tables[k - 1].ctx_stats(ctx)
            if c_h[0] == 0:
                break                       # contextos mais longos também não existem
            c_hw = self.tables[k - 1].pair_count(np.repeat(ctx, self.V), all_w)
            p = (c_hw + T_h[0] * p) / (c_h[0] + T_h[0])
        return p

    def generate(self, bos_id, eos_id, max_tokens, rng, filt, banned=()):
        seq = [bos_id]
        for _ in range(max_tokens):
            p = self.next_dist(seq)
            p[list(banned)] = 0.0
            p = filt(p / p.sum())
            nxt = int(rng.choice(self.V, p=p))
            if nxt == eos_id:
                break
            seq.append(nxt)
        return seq[1:]

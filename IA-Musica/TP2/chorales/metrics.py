"""Métricas objetivas (muspy) e comparação de distribuições (Yang & Lerch, 2020)."""
from __future__ import annotations

from pathlib import Path

import muspy
import numpy as np
from scipy.stats import gaussian_kde

METRICS = {
    "pitch_range": "Extensão (semitons)",
    "n_pitch_classes_used": "Classes de altura usadas",
    "pitch_class_entropy": "Entropia de classes de altura",
    "scale_consistency": "Consistência de escala",
    "polyphony": "Polifonia",
    "empty_beat_rate": "Taxa de tempos vazios",
    "groove_consistency": "Consistência de groove",
}


def compute_metrics(midi_path: Path) -> dict[str, float]:
    music = muspy.read_midi(str(midi_path))
    res = music.resolution
    out = {
        "pitch_range": muspy.pitch_range(music),
        "n_pitch_classes_used": muspy.n_pitch_classes_used(music),
        "pitch_class_entropy": muspy.pitch_class_entropy(music),
        "scale_consistency": muspy.scale_consistency(music),
        "polyphony": muspy.polyphony(music),
        "empty_beat_rate": muspy.empty_beat_rate(music),
        "groove_consistency": muspy.groove_consistency(music, measure_resolution=4 * res),
    }
    notes = [n for t in music.tracks for n in t.notes]
    end = max((n.time + n.duration for n in notes), default=0)
    out["n_notes"] = len(notes)
    out["n_bars"] = end / (4 * res)
    return {k: float(v) for k, v in out.items()}


# ----------------------------------------------------------- Yang & Lerch
def _pairwise(a: np.ndarray, b: np.ndarray | None = None) -> np.ndarray:
    """Distâncias absolutas intra-conjunto (b=None) ou inter-conjuntos."""
    if b is None:
        d = np.abs(a[:, None] - a[None, :])
        return d[np.triu_indices(len(a), k=1)]
    return np.abs(a[:, None] - b[None, :]).ravel()


def _kde(x: np.ndarray):
    x = x[np.isfinite(x)]
    if np.ptp(x) < 1e-9:                       # distribuição degenerada
        x = x + np.random.default_rng(0).normal(0, 1e-3, len(x))
    return gaussian_kde(x)


def overlap_and_kl(p_samples: np.ndarray, q_samples: np.ndarray, n_grid: int = 2000):
    """Área de sobreposição e KL(P||Q) entre KDEs das duas amostras."""
    lo = min(p_samples.min(), q_samples.min())
    hi = max(p_samples.max(), q_samples.max())
    pad = 0.1 * (hi - lo + 1e-9)
    grid = np.linspace(lo - pad, hi + pad, n_grid)
    p, q = _kde(p_samples)(grid), _kde(q_samples)(grid)
    dx = grid[1] - grid[0]
    p /= p.sum() * dx
    q /= q.sum() * dx
    oa = float(np.sum(np.minimum(p, q)) * dx)
    eps = 1e-10
    kl = float(np.sum(p * np.log((p + eps) / (q + eps))) * dx)
    return oa, kl


def compare_to_reference(gen: np.ndarray, ref: np.ndarray) -> dict[str, float]:
    """Protocolo de Yang & Lerch (2020) para uma métrica escalar.

    Compara a distribuição de distâncias intra-referência (humano–humano) com a
    de distâncias inter-conjuntos (gerado–humano): OA alto e KL baixo indicam
    que as peças geradas são tão "próximas" do conjunto de teste quanto as
    próprias peças humanas entre si.
    """
    gen, ref = np.asarray(gen, float), np.asarray(ref, float)
    intra_ref, inter = _pairwise(ref), _pairwise(gen, ref)
    oa, kl = overlap_and_kl(intra_ref, inter)
    oa_feat, _ = overlap_and_kl(ref, gen)
    return {
        "mean": float(gen.mean()), "std": float(gen.std()),
        "intra_gen": float(_pairwise(gen).mean()), "inter": float(inter.mean()),
        "abs_mean_diff": float(abs(gen.mean() - ref.mean())),
        "OA": oa, "KL": kl, "OA_feature": oa_feat,
    }

"""Amostragem autoregressiva: temperatura, top-k e top-p (núcleo).

A mesma função ``filter_probs`` é usada pelo Transformer e pela cadeia de Markov,
garantindo o mesmo protocolo de geração para os dois sistemas.
"""
from __future__ import annotations

import numpy as np
import torch
import torch.nn.functional as F


def filter_probs(probs: np.ndarray, temperature: float = 1.0, top_k: int = 0,
                 top_p: float = 1.0) -> np.ndarray:
    """Reaplica temperatura e corta a cauda de uma distribuição (numpy, 1-D)."""
    p = np.asarray(probs, dtype=np.float64)
    if temperature != 1.0:
        p = np.power(np.maximum(p, 1e-300), 1.0 / temperature)
    p /= p.sum()
    if top_k and top_k < len(p):
        kth = np.partition(p, -top_k)[-top_k]
        p = np.where(p >= kth, p, 0.0)
    if top_p < 1.0:
        order = np.argsort(-p)
        csum = np.cumsum(p[order])
        keep = order[: int(np.searchsorted(csum, top_p * csum[-1])) + 1]
        mask = np.zeros_like(p, dtype=bool)
        mask[keep] = True
        p = np.where(mask, p, 0.0)
    return p / p.sum()


def filter_logits(logits: torch.Tensor, temperature: float = 1.0, top_k: int = 0,
                  top_p: float = 1.0) -> torch.Tensor:
    """Versão em lote (B, V) para o Transformer; devolve probabilidades."""
    logits = logits.float() / max(temperature, 1e-6)
    if top_k and top_k < logits.size(-1):
        kth = torch.topk(logits, top_k, dim=-1).values[..., -1:]
        logits = logits.masked_fill(logits < kth, float("-inf"))
    probs = F.softmax(logits, dim=-1)
    if top_p < 1.0:
        sp, idx = torch.sort(probs, descending=True, dim=-1)
        csum = torch.cumsum(sp, dim=-1)
        drop = (csum - sp) > top_p          # mantém o menor prefixo com massa >= top_p
        sp = sp.masked_fill(drop, 0.0)
        probs = torch.zeros_like(probs).scatter(-1, idx, sp)
        probs = probs / probs.sum(-1, keepdim=True)
    return probs


@torch.no_grad()
def generate_transformer(model, bos_id: int, eos_id: int, n: int, max_tokens: int,
                         device, temperature=1.0, top_k=0, top_p=1.0, banned=(),
                         generator: torch.Generator | None = None) -> list[list[int]]:
    """Gera ``n`` sequências em lote a partir de BOS, com janela deslizante de contexto."""
    model.eval()
    L = model.cfg.max_len
    seqs = torch.full((n, 1), bos_id, dtype=torch.long, device=device)
    done = torch.zeros(n, dtype=torch.bool, device=device)
    for _ in range(max_tokens):
        ctx = seqs[:, -L:]
        with torch.autocast(device_type=device.type, dtype=torch.bfloat16,
                            enabled=device.type == "cuda"):
            logits = model(ctx)[:, -1, :]
        if banned:
            logits[:, list(banned)] = float("-inf")
        probs = filter_logits(logits, temperature, top_k, top_p)
        nxt = torch.multinomial(probs, 1, generator=generator).squeeze(1)
        nxt = torch.where(done, torch.full_like(nxt, eos_id), nxt)
        seqs = torch.cat([seqs, nxt.unsqueeze(1)], dim=1)
        done |= nxt == eos_id
        if done.all():
            break
    out = []
    for row in seqs.tolist():
        row = row[1:]
        out.append(row[: row.index(eos_id)] if eos_id in row else row)
    return out

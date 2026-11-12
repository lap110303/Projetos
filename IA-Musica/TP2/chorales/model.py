"""Transformer decoder-only (autoregressivo) em PyTorch puro."""
from __future__ import annotations

from dataclasses import asdict, dataclass

import torch
import torch.nn as nn
import torch.nn.functional as F


@dataclass
class ModelConfig:
    vocab_size: int
    max_len: int = 1024
    d_model: int = 256
    n_layers: int = 4
    n_heads: int = 4
    d_ff: int = 1024
    dropout: float = 0.1
    pad_id: int = 0


class MusicTransformer(nn.Module):
    """Pilha de ``nn.TransformerEncoderLayer`` com máscara causal (estilo GPT).

    Embedding de tokens + embedding posicional aprendido, pré-normalização,
    e projeção de saída com pesos amarrados ao embedding de entrada.
    """

    def __init__(self, cfg: ModelConfig):
        super().__init__()
        self.cfg = cfg
        self.tok_emb = nn.Embedding(cfg.vocab_size, cfg.d_model, padding_idx=cfg.pad_id)
        self.pos_emb = nn.Embedding(cfg.max_len, cfg.d_model)
        self.drop = nn.Dropout(cfg.dropout)
        layer = nn.TransformerEncoderLayer(
            d_model=cfg.d_model, nhead=cfg.n_heads, dim_feedforward=cfg.d_ff,
            dropout=cfg.dropout, activation="gelu", batch_first=True, norm_first=True)
        self.blocks = nn.TransformerEncoder(layer, num_layers=cfg.n_layers,
                                            enable_nested_tensor=False)
        self.norm = nn.LayerNorm(cfg.d_model)
        self.head = nn.Linear(cfg.d_model, cfg.vocab_size, bias=False)
        self.head.weight = self.tok_emb.weight
        self.apply(self._init)

    @staticmethod
    def _init(m):
        if isinstance(m, (nn.Linear, nn.Embedding)):
            nn.init.normal_(m.weight, std=0.02)
            if isinstance(m, nn.Linear) and m.bias is not None:
                nn.init.zeros_(m.bias)

    def forward(self, x: torch.Tensor) -> torch.Tensor:
        """x: (B, T) ids -> logits (B, T, V)."""
        B, T = x.shape
        pos = torch.arange(T, device=x.device)
        h = self.drop(self.tok_emb(x) + self.pos_emb(pos))
        mask = nn.Transformer.generate_square_subsequent_mask(T, device=x.device)
        h = self.blocks(h, mask=mask, is_causal=True)
        return self.head(self.norm(h))

    def loss(self, x: torch.Tensor, y: torch.Tensor) -> torch.Tensor:
        logits = self(x)
        return F.cross_entropy(logits.reshape(-1, logits.size(-1)).float(), y.reshape(-1),
                               ignore_index=self.cfg.pad_id)

    def n_params(self) -> int:
        return sum(p.numel() for p in self.parameters())

    def to_dict(self) -> dict:
        return asdict(self.cfg)


@torch.no_grad()
def sequence_nll(model: MusicTransformer, seq: list[int], device, stride: int | None = None):
    """Soma da log-verossimilhança negativa (nats) e nº de tokens previstos.

    Sequências maiores que o contexto são avaliadas com janela deslizante.
    """
    L = model.cfg.max_len
    stride = stride or L // 2
    ids = torch.tensor(seq, device=device)
    total, count, prev_end = 0.0, 0, 0
    for begin in range(0, max(1, len(seq) - 1), stride):
        end = min(begin + L, len(seq) - 1)
        x = ids[begin:end].unsqueeze(0)
        y = ids[begin + 1:end + 1]
        logp = F.log_softmax(model(x).float()[0], -1)
        nll = -logp.gather(1, y.unsqueeze(1)).squeeze(1)
        new = end - max(prev_end, begin)          # só os alvos ainda não contados
        total += nll[-new:].sum().item()
        count += new
        prev_end = end
        if end >= len(seq) - 1:
            break
    return total, count

"""Testes rápidos: python -m pytest tests -q  (ou python tests/test_pipeline.py)"""
import sys
from pathlib import Path

import numpy as np
import torch

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from chorales.markov import NGramLM  # noqa: E402
from chorales.model import ModelConfig, MusicTransformer  # noqa: E402
from chorales.sampling import filter_logits, filter_probs  # noqa: E402


def test_markov_is_normalised():
    rng = np.random.default_rng(0)
    seqs = [[1, *rng.integers(3, 12, 50).tolist(), 2] for _ in range(20)]
    lm = NGramLM(4, 12, 0).fit(seqs)
    for h in ([1], [1, 5, 6], seqs[0][:10], [1, 11, 11, 11, 11]):
        p = lm.next_dist(h)
        assert abs(p.sum() - 1) < 1e-9 and (p > 0).all()
    # log-probs vetorizadas == distribuição explícita
    lp = lm.token_logprobs([seqs[3]])
    explicit = [np.log(lm.next_dist(seqs[3][:t])[seqs[3][t]]) for t in range(1, len(seqs[3]))]
    assert np.allclose(lp, explicit)


def test_filters():
    p = np.array([0.5, 0.3, 0.15, 0.05])
    assert np.allclose(filter_probs(p, top_p=0.7), [0.625, 0.375, 0, 0])
    assert np.allclose(filter_probs(p, top_k=1), [1, 0, 0, 0])
    q = filter_logits(torch.log(torch.tensor([p])), top_p=0.7)[0].numpy()
    assert np.allclose(q, [0.625, 0.375, 0, 0], atol=1e-6)


def test_transformer_is_causal():
    m = MusicTransformer(ModelConfig(vocab_size=20, max_len=16, d_model=32, n_layers=2,
                                     n_heads=2, d_ff=64, dropout=0.0))
    x = torch.randint(3, 20, (1, 12))
    y = x.clone()
    y[0, 8:] = 3
    for train_mode in (True, False):          # caminho de treino e "fast path" de inferência
        m.train(train_mode)
        with torch.no_grad():
            assert torch.allclose(m(x)[0, :8], m(y)[0, :8], atol=1e-5)


if __name__ == "__main__":
    test_markov_is_normalised()
    test_filters()
    test_transformer_is_causal()
    print("ok")

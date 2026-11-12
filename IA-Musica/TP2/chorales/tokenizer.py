"""Representação REMI (miditok) para os corais.

Os quatro vozes são fundidas numa única faixa: cada nota vira ``Pitch`` +
``Duration``; ``Bar`` e ``Position`` (grade de semicolcheias) dão a métrica
explícita. Sem velocidade, andamento, acordes ou programas: o corpus não tem
essa informação.
"""
from __future__ import annotations

import json
from pathlib import Path

from miditok import REMI, TokenizerConfig, TokSequence

from .data import ROOT

TOKENIZER_PATH = ROOT / "data" / "tokenizer.json"
PITCH_RANGE = (33, 86)          # Lá1..Ré6 (inclusivo), folga para a transposição (dados: 36..81)


def build_tokenizer() -> REMI:
    cfg = TokenizerConfig(
        pitch_range=PITCH_RANGE,
        beat_res={(0, 4): 4, (4, 12): 2},     # semicolcheia até 1 compasso, colcheia além
        num_velocities=1,
        use_velocities=False,
        use_chords=False, use_rests=False, use_tempos=False,
        use_time_signatures=False, use_programs=False,
        use_pitchdrum_tokens=False,           # sem bateria: evita 62 tokens inúteis
        special_tokens=["PAD", "BOS", "EOS"],
    )
    return REMI(cfg)


def load_tokenizer() -> REMI:
    if TOKENIZER_PATH.exists():
        return REMI(params=TOKENIZER_PATH)
    return build_tokenizer()


def special_ids(tok: REMI) -> dict[str, int]:
    return {name: tok.vocab[f"{name}_None"] for name in ("PAD", "BOS", "EOS")}


def encode_score(tok: REMI, score) -> list[int]:
    """Score -> ids, com BOS/EOS nas pontas."""
    res = tok.encode(score)
    seq = res[0] if isinstance(res, list) else res
    sp = special_ids(tok)
    return [sp["BOS"], *seq.ids, sp["EOS"]]


def decode_ids(tok: REMI, ids: list[int]):
    sp = special_ids(tok)
    ids = [i for i in ids if i not in sp.values()]
    return tok.decode([TokSequence(ids=ids)])


def token_types(tok: REMI, ids: list[int]) -> list[str]:
    inv = {v: k for k, v in tok.vocab.items()}
    return [inv[i].split("_")[0] for i in ids]


def save_json(obj, path: Path):
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(obj))

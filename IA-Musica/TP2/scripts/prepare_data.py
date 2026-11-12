"""Pré-processamento: JSON -> MIDI -> tokens REMI (+ aumento por transposição no treino).

Saídas:
  data/midi/{train,valid,test}/*.mid     corais originais em MIDI
  data/midi/test_ref/*.mid               teste após ida e volta pelo tokenizador
                                         (referência humana usada na avaliação)
  data/tokens/{split}.json               sequências de ids
  data/tokenizer.json                    configuração/vocabulário do REMI
  results/dataset_stats.json
"""
import json
import sys
from pathlib import Path

import numpy as np

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from chorales.data import ROOT, SPLITS, grid_to_score, load_split, pitch_bounds, write_midi  # noqa: E402
from chorales.tokenizer import (PITCH_RANGE, TOKENIZER_PATH, build_tokenizer, decode_ids,  # noqa: E402
                                encode_score, token_types)

TRANSPOSITIONS = range(-5, 7)   # 12 tonalidades


def main():
    tok = build_tokenizer()
    TOKENIZER_PATH.parent.mkdir(parents=True, exist_ok=True)
    tok.save(TOKENIZER_PATH)
    stats = {"vocab_size": len(tok), "pitch_range_tokens": PITCH_RANGE, "splits": {}}
    tok_dir = ROOT / "data" / "tokens"
    tok_dir.mkdir(parents=True, exist_ok=True)

    for split in SPLITS:
        grids = load_split(split)
        seqs, n_aug = [], 0
        for i, g in enumerate(grids):
            write_midi(grid_to_score(g), ROOT / "data" / "midi" / split / f"{split}_{i:03d}.mid")
            shifts = TRANSPOSITIONS if split == "train" else [0]
            for s in shifts:
                lo, hi = g[g >= 0].min() + s, g[g >= 0].max() + s
                if lo < PITCH_RANGE[0] or hi >= PITCH_RANGE[1]:
                    continue
                ids = encode_score(tok, grid_to_score(g, transpose=s))
                seqs.append(ids)
                n_aug += s != 0
                if split == "test" and s == 0:
                    write_midi(decode_ids(tok, ids),
                               ROOT / "data" / "midi" / "test_ref" / f"test_{i:03d}.mid")
        (tok_dir / f"{split}.json").write_text(json.dumps(seqs))

        lens = np.array([len(s) for s in seqs])
        orig_lens = np.array([len(encode_score(tok, grid_to_score(g))) for g in grids])
        n_notes = [int(((np.diff(g, axis=0, prepend=-2) != 0) & (g >= 0)).sum()) for g in grids]
        types = {}
        for s in seqs[: len(grids)]:
            for t in token_types(tok, s):
                types[t] = types.get(t, 0) + 1
        stats["splits"][split] = {
            "pieces": len(grids),
            "sequences_after_aug": len(seqs),
            "transposed_copies": n_aug,
            "steps16_mean": float(np.mean([len(g) for g in grids])),
            "bars_mean": float(np.mean([len(g) for g in grids]) / 16),
            "notes_mean": float(np.mean(n_notes)),
            "tokens_mean_original": float(orig_lens.mean()),
            "tokens_std_original": float(orig_lens.std()),
            "tokens_min_original": int(orig_lens.min()),
            "tokens_max_original": int(orig_lens.max()),
            "tokens_total_after_aug": int(lens.sum()),
            "pitch_min_max": pitch_bounds(grids),
            "token_type_share": {k: v / sum(types.values()) for k, v in types.items()},
        }
        print(split, json.dumps(stats["splits"][split], indent=1))

    stats["vocab_types"] = sorted({k.split("_")[0] for k in tok.vocab})
    out = ROOT / "results" / "dataset_stats.json"
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text(json.dumps(stats, indent=2))
    print("vocab:", len(tok))


if __name__ == "__main__":
    main()

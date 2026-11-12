"""Geração de amostras incondicionais (a partir de BOS) pelos dois sistemas.

Mesmo protocolo para ambos: temperatura, top-p e limite de tokens idênticos.
Saídas em samples/<sistema>/eval/*.mid e samples/<sistema>/eval_ids.json.
"""
import argparse
import json
import sys
import time
from functools import partial
from pathlib import Path

import numpy as np
import torch

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from chorales.data import ROOT, write_midi  # noqa: E402
from chorales.markov import NGramLM  # noqa: E402
from chorales.model import ModelConfig, MusicTransformer  # noqa: E402
from chorales.sampling import filter_probs, generate_transformer  # noqa: E402
from chorales.tokenizer import decode_ids, load_tokenizer, special_ids  # noqa: E402


def save(tok, system, seqs, subdir, meta):
    d = ROOT / "samples" / system / subdir
    d.mkdir(parents=True, exist_ok=True)
    empty = 0
    for i, ids in enumerate(seqs):
        score = decode_ids(tok, ids)
        if sum(len(t.notes) for t in score.tracks) == 0:
            empty += 1
        write_midi(score, d / f"{system}_{i:03d}.mid")
    (ROOT / "samples" / system / f"{subdir}_ids.json").write_text(json.dumps(seqs))
    meta["empty_outputs"] = empty
    (ROOT / "samples" / system / f"{subdir}_meta.json").write_text(json.dumps(meta, indent=2))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--system", choices=["transformer", "markov", "both"], default="both")
    ap.add_argument("--n", type=int, default=200)
    ap.add_argument("--temperature", type=float, default=1.0)
    ap.add_argument("--top_k", type=int, default=0)
    ap.add_argument("--top_p", type=float, default=0.95)
    ap.add_argument("--max_tokens", type=int, default=2600)
    ap.add_argument("--batch", type=int, default=50)
    ap.add_argument("--seed", type=int, default=123)
    ap.add_argument("--subdir", default="eval")
    args = ap.parse_args()

    tok = load_tokenizer()
    sp = special_ids(tok)
    banned = (sp["PAD"], sp["BOS"])
    systems = ["transformer", "markov"] if args.system == "both" else [args.system]

    for system in systems:
        t0 = time.time()
        if system == "transformer":
            device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
            ck = torch.load(ROOT / "checkpoints/transformer.pt", map_location=device)
            model = MusicTransformer(ModelConfig(**ck["config"])).to(device)
            model.load_state_dict(ck["state_dict"])
            g = torch.Generator(device=device).manual_seed(args.seed)
            seqs = []
            for b in range(0, args.n, args.batch):
                k = min(args.batch, args.n - b)
                seqs += generate_transformer(model, sp["BOS"], sp["EOS"], k, args.max_tokens,
                                             device, args.temperature, args.top_k, args.top_p,
                                             banned=banned, generator=g)
                print(f"  transformer {len(seqs)}/{args.n}  {time.time() - t0:.0f}s")
        else:
            order = json.loads((ROOT / "results/markov_orders.json").read_text())["best_order"]
            train = json.loads((ROOT / "data/tokens/train.json").read_text())
            lm = NGramLM(order, len(tok), sp["PAD"]).fit(train)
            rng = np.random.default_rng(args.seed)
            filt = partial(filter_probs, temperature=args.temperature, top_k=args.top_k,
                           top_p=args.top_p)
            seqs = []
            for i in range(args.n):
                seqs.append(lm.generate(sp["BOS"], sp["EOS"], args.max_tokens, rng, filt, banned))
                if (i + 1) % 50 == 0:
                    print(f"  markov (n={order}) {i + 1}/{args.n}  {time.time() - t0:.0f}s")
        meta = {**vars(args), "system": system, "time_s": time.time() - t0,
                "mean_len": float(np.mean([len(s) for s in seqs])),
                "hit_max_tokens": int(sum(len(s) >= args.max_tokens for s in seqs))}
        save(tok, system, seqs, args.subdir, meta)
        print(system, json.dumps(meta))


if __name__ == "__main__":
    main()

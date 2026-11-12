"""Ajuste da cadeia de Markov e escolha da ordem n pela NLL de validação."""
import json
import math
import sys
import time
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from chorales.data import ROOT  # noqa: E402
from chorales.markov import NGramLM  # noqa: E402
from chorales.tokenizer import load_tokenizer, special_ids  # noqa: E402

MAX_ORDER = 16


def main():
    tok = load_tokenizer()
    sp = special_ids(tok)
    train = json.loads((ROOT / "data/tokens/train.json").read_text())
    valid = json.loads((ROOT / "data/tokens/valid.json").read_text())
    t0 = time.time()
    lm = NGramLM(MAX_ORDER, len(tok), sp["PAD"]).fit(train)
    print(f"ajuste até ordem {MAX_ORDER}: {time.time() - t0:.1f}s")
    curve = []
    for n in range(0, MAX_ORDER + 1):
        tr = -lm.token_logprobs(train[:300], order=n).mean()
        va = -lm.token_logprobs(valid, order=n).mean()
        curve.append({"order": n, "train_nll": float(tr), "valid_nll": float(va),
                      "valid_ppl": math.exp(va)})
        print(json.dumps(curve[-1]))
    best = min(curve, key=lambda r: r["valid_nll"])
    out = {"curve": curve, "best_order": best["order"], "best_valid_nll": best["valid_nll"],
           "n_contexts": {k + 1: int(len(t.ctx_keys)) for k, t in enumerate(lm.tables)}}
    (ROOT / "results").mkdir(exist_ok=True)
    (ROOT / "results" / "markov_orders.json").write_text(json.dumps(out, indent=2))
    print("melhor ordem:", best)


if __name__ == "__main__":
    main()

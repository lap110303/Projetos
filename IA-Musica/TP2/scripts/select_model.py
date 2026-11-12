"""Escolhe, pela NLL de validação, o melhor Transformer entre os dropouts testados."""
import json
import shutil
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from chorales.data import ROOT  # noqa: E402

runs = {}
for h in sorted((ROOT / "results").glob("transformer_history_do*.json")):
    tag = h.stem.split("_")[-1]
    runs[tag] = json.loads(h.read_text())["best_valid_nll"]
best = min(runs, key=runs.get)
shutil.copy(ROOT / f"checkpoints/transformer_{best}.pt", ROOT / "checkpoints/transformer.pt")
shutil.copy(ROOT / f"results/transformer_history_{best}.json", ROOT / "results/transformer_history.json")
(ROOT / "results/model_selection.json").write_text(json.dumps({"valid_nll": runs, "chosen": best}, indent=2))
print("NLL de validação por configuração:", runs, "-> escolhido:", best)

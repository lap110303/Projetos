"""Renderização de áudio (apenas para audição).

Usa FluidSynth + soundfont quando disponíveis (variável SOUNDFONT); caso
contrário, um sintetizador aditivo simples com timbre de órgão, sem dependências
externas.
"""
from __future__ import annotations

import os
import shutil
from pathlib import Path

import numpy as np
import pretty_midi
from scipy.io import wavfile

SR = 22050
HARMONICS = [(1, 1.0), (2, 0.45), (3, 0.25), (4, 0.12), (6, 0.05)]


def _organ(pm: pretty_midi.PrettyMIDI, sr: int = SR) -> np.ndarray:
    end = pm.get_end_time() + 1.0
    out = np.zeros(int(end * sr) + 1, dtype=np.float64)
    for inst in pm.instruments:
        for n in inst.notes:
            dur = n.end - n.start
            t = np.arange(int((dur + 0.08) * sr)) / sr
            f0 = pretty_midi.note_number_to_hz(n.pitch)
            wave = sum(a * np.sin(2 * np.pi * h * f0 * t) for h, a in HARMONICS if h * f0 < sr / 2)
            env = np.minimum(1.0, t / 0.02)                               # ataque 20 ms
            env *= np.clip((dur + 0.08 - t) / 0.08, 0.0, 1.0)             # liberação 80 ms
            s = int(n.start * sr)
            out[s:s + len(t)] += wave * env * (n.velocity / 127.0)
    out /= max(np.abs(out).max(), 1e-9)
    return 0.8 * out


def render(midi_path: Path, wav_path: Path) -> str:
    pm = pretty_midi.PrettyMIDI(str(midi_path))
    sf = os.environ.get("SOUNDFONT")
    wav_path.parent.mkdir(parents=True, exist_ok=True)
    if sf and Path(sf).exists() and shutil.which("fluidsynth"):
        audio = pm.fluidsynth(fs=SR, sf2_path=sf)
        audio = 0.8 * audio / max(np.abs(audio).max(), 1e-9)
        engine = "fluidsynth"
    else:
        audio = _organ(pm)
        engine = "aditivo"
    wavfile.write(str(wav_path), SR, (audio * 32767).astype(np.int16))
    return engine

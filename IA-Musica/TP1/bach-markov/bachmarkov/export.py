"""Exportação dos corais gerados: MIDI (pretty_midi), MusicXML (music21), áudio (FluidSynth)."""
from __future__ import annotations

import os
import shutil
import subprocess

import pretty_midi
from music21 import expressions, meter, note, stream, tempo, key as m21key

KEYS = {"C": 0, "C#": 1, "Db": 1, "D": 2, "Eb": 3, "E": 4, "F": 5, "F#": 6,
        "Gb": 6, "G": 7, "Ab": 8, "A": 9, "Bb": 10, "B": 11}
NAMES = ["Soprano", "Alto", "Tenor", "Baixo"]


def transpose_amount(mode, key_name):
    """Semitons para levar Dó maior / Lá menor à tonalidade pedida (-6..+5)."""
    base = 0 if mode == "major" else 9
    d = (KEYS[key_name] - base) % 12
    return d - 12 if d > 5 else d


def _voice_lines(events, shift):
    """Converte eventos em 4 linhas [(pitch, dur, fer)], ligando notas repetidas
    nas vozes internas (A, T, B mudam só nos ataques do soprano)."""
    lines = [[] for _ in range(4)]
    for e in events:
        pitches = [e["s"], *e["atb"]]
        for v, p in enumerate(pitches):
            p += shift
            if v > 0 and lines[v] and lines[v][-1][0] == p and not lines[v][-1][2]:
                lines[v][-1][1] += e["dur"]
                lines[v][-1][2] = e["fer"]
            else:
                lines[v].append([p, e["dur"], e["fer"]])
    return lines


def to_midi(events, path, mode="major", key_name=None, bpm=72, fermata_extra=1.0,
            program=19):
    """Escreve MIDI com 4 trilhas (SATB). Fermatas são realizadas prolongando a
    nota em `fermata_extra` semínimas, como na prática de execução dos corais."""
    shift = transpose_amount(mode, key_name) if key_name else 0
    pm = pretty_midi.PrettyMIDI(initial_tempo=bpm)
    spb = 60.0 / bpm
    for v, line in enumerate(_voice_lines(events, shift)):
        inst = pretty_midi.Instrument(program=program, name=NAMES[v])
        t = 0.0
        for p, dur, fer in line:
            d = (dur + (fermata_extra if fer else 0)) * spb
            vel = 90 if v == 0 else 72
            inst.notes.append(pretty_midi.Note(vel, int(p), t, t + d * 0.97))
            t += d
        pm.instruments.append(inst)
    pm.write(path)
    return pm


def to_musicxml(events, path, mode="major", key_name=None, title="Coral gerado"):
    shift = transpose_amount(mode, key_name) if key_name else 0
    tonic = (0 if mode == "major" else 9) + shift
    sc = stream.Score()
    sc.insert(0, __import__("music21").metadata.Metadata(title=title, composer="MelodyMarkov + HarmonyMarkov"))
    pickup = events[0]["beat"]
    for v, line in enumerate(_voice_lines(events, shift)):
        part = stream.Part(id=NAMES[v])
        part.partName = NAMES[v]
        part.append(meter.TimeSignature("4/4"))
        part.append(m21key.Key(__import__("music21").pitch.Pitch(tonic % 12).name, mode))
        if v == 0:
            part.append(tempo.MetronomeMark(number=72))
        if pickup:
            part.append(note.Rest(quarterLength=pickup))
        for p, dur, fer in line:
            n = note.Note(p, quarterLength=dur)
            if fer:
                n.expressions.append(expressions.Fermata())
            part.append(n)
        sc.insert(0, part)
    sc = sc.makeNotation()
    sc.write("musicxml", fp=path)


def find_soundfont():
    for p in [os.environ.get("SOUNDFONT", ""), "/usr/share/sounds/sf2/FluidR3_GM.sf2",
              "/usr/share/sounds/sf2/default-GM.sf2", "/usr/share/soundfonts/default.sf2",
              "/opt/homebrew/share/soundfonts/default.sf2"]:
        if p and os.path.exists(p):
            return p
    return None


def to_audio(midi_path, out_path, sample_rate=44100):
    """Renderiza áudio com FluidSynth (+ ffmpeg para .mp3). Se o FluidSynth não
    estiver instalado, usa a síntese senoidal do pretty_midi (WAV)."""
    sf = find_soundfont()
    wav = os.path.splitext(out_path)[0] + ".wav"
    if shutil.which("fluidsynth") and sf:
        subprocess.run(["fluidsynth", "-ni", "-g", "0.6", "-F", wav, "-r", str(sample_rate),
                        sf, midi_path], check=True, capture_output=True)
    else:
        import numpy as np
        from scipy.io import wavfile
        y = pretty_midi.PrettyMIDI(midi_path).synthesize(fs=sample_rate)
        wavfile.write(wav, sample_rate, (y / (abs(y).max() + 1e-9) * 0.8 * 32767).astype(np.int16))
    if out_path.endswith(".mp3") and shutil.which("ffmpeg"):
        subprocess.run(["ffmpeg", "-y", "-loglevel", "error", "-i", wav, "-b:a", "192k", out_path],
                       check=True)
        os.remove(wav)
        return out_path
    return wav

# server.py
import time
from multiprocessing import Process, Queue, get_context
from flask import Flask, render_template, request, jsonify
from flask_cors import CORS
import search  

app = Flask(__name__)
CORS(app)

ROWS, COLS = 6, 7

def fallback_move(board):
    """Escolhe a primeira coluna válida (fallback simples)."""
    for c in range(COLS):
        if board[0][c] == 0:
            return c
    return 0  # Nenhuma jogada possível (tabuleiro cheio)

def _agent_worker(board, player, config, q):
    """Roda em OUTRO processo para isolar travamentos/loops infinitos."""
    try:
        col = search.choose_move(board, player, config)
        q.put(("ok", col))
    except Exception as e:
        q.put(("err", str(e)))

def run_agent_with_timeout(board, player, config, timeout_s=5.0):
    """
    Executa choose_move em um processo separado e aplica um hard timeout.
    Retorna (col, info, flags) — onde flags indica timeout/crash.
    """
    ctx = get_context("spawn")
    q = ctx.Queue()
    p = ctx.Process(target=_agent_worker, args=(board, player, config, q))
    p.start()
    p.join(timeout_s)

    if p.is_alive():
        # Estourou tempo: mata o processo
        p.terminate()
        p.join()
        col = fallback_move(board)
        info = {"timeout": True, "method": "fallback"}
        return col, info

    # Processo terminou; coleta a resposta
    if q.empty():
        # Crash silencioso
        col = fallback_move(board)
        info = {"crash": True, "method": "fallback"}
        return col, info

    status, payload = q.get()
    if status == "ok":
        col = payload
        info = {"method": "AI"}
        return col, info
    else:
        # Exceção no código do aluno
        col = fallback_move(board)
        info = {"error": payload, "method": "fallback"}
        return col, info
    
def parse_board_str(board_str: str):
    """
    Converte '0000000;0000000;...;0000000' -> [[int,...]*7]*6
    Aceita linhas separadas por ';' (recomendado) ou '\n'.
    """
    if board_str is None:
        raise ValueError("Parâmetro 'board' ausente.")
    
    # Normaliza separadores
    raw_rows = [r for r in board_str.replace('\n', ';').split(';') if r != ""]
    if len(raw_rows) != ROWS:
        raise ValueError(f"Tabuleiro inválido: esperado {ROWS} linhas, recebido {len(raw_rows)}.")

    board = []
    for r, line in enumerate(raw_rows):
        if len(line) != COLS:
            raise ValueError(f"Linha {r} inválida: esperado {COLS} colunas, recebido {len(line)}.")
        try:
            row = [int(ch) for ch in line]
        except ValueError:
            raise ValueError("Caracteres inválidos no tabuleiro (use apenas 0,1,2).")
        if any(v not in (0,1,2) for v in row):
            raise ValueError("Valores inválidos no tabuleiro (use apenas 0,1,2).")
        board.append(row)
    return board

@app.route('/')
def index():
    return render_template('index.html')

@app.route("/ping", methods=["GET"])
def ping():
    return jsonify({"result": "success", "message": "pong"})

@app.route("/ai_move", methods=["GET"])
def ai_move():
    """
    Exemplo de chamada (GET):
      /ai_move?board=0000000;0000000;0000000;0000000;0000000;0000000&player=2&max_depth=5&max_time_ms=2000&agent=ai_minimax
    """
    # Lê parâmetros (com valores-padrão seguros)
    board_str = request.args.get("board", type=str)
    # player pode vir como "1" ou "2" (string); converte para int
    player = request.args.get("player", type=str)
    try:
        player = int(player)
    except Exception:
        # fallback seguro (se o cliente enviar algo estranho)
        player = 1

    max_time_ms = request.args.get("max_time_ms", type=int)
    if max_time_ms is None:
        max_time_ms = 2000

    max_depth = request.args.get("max_depth", type=int)
    if max_depth is None:
        max_depth = 5

    # novo: qual agente usar (ex: ai_random, ai_minimax, ai_alphabeta, ai_id)
    agent = request.args.get("agent", default="ai_random", type=str)

    # Lê tabuleiro
    board = parse_board_str(board_str)

    # Monta config que será passado ao agente (inclui 'agent')
    config = {
        "max_time_ms": max_time_ms,
        "max_depth": max_depth,
        "agent": agent
    }

    # Define hard timeout (com margem)
    hard_timeout_s = max(1.0, (max_time_ms or 2000) / 1000.0 + 0.2)  # margem de 200ms

    # Executa agente com timeout
    t0 = time.time()
    col, info = run_agent_with_timeout(board, player, config, timeout_s=hard_timeout_s)
    t1 = time.time()

    # Se info for None, normaliza para dict para poder usar info.update
    if info is None:
        info = {}

    # Calcula tempo decorrido
    elapsed_ms = int((t1 - t0) * 1000)

    # Imprime resultado do agente (útil para debug)
    print(f"AI agent={agent} chose column {col} in {elapsed_ms} ms. Info: {info}")

    info.update({
        "elapsed_ms": elapsed_ms,
        "max_time_ms": max_time_ms,
        "agent": agent,
        "max_depth": max_depth
    })

    return jsonify({"result": "success", "col": col, "info": info})

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5001, debug=True)
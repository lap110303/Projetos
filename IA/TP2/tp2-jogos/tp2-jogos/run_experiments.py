"""
Script de experimentos para Connect-4
Cria partidas automáticas entre agentes (aleatório, minimax, alfa-beta e iterative deepening)
- Executa 100 jogos por combinação (alternando quem começa)
- Coleta métricas: taxa de vitória, tempo médio por jogada (ms), média de estados visitados por jogada, e para ID: profundidade média atingida

Uso:
  python run_experiments.py

Requisitos: coloque este arquivo no mesmo diretório de `search.py` e `server.py` (fornecidos). Python 3.8+.
Saídas:
 - imprime resumo no stdout
 - salva CSV em ./experiments_results.csv

"""
from typing import List, Dict, Tuple, Optional
import time
import math
import random
import csv
import statistics
import search

# Seed para reprodutibilidade
random.seed(0)

P1, P2 = search.P1, search.P2

# ------------------------------------------------------------------
# Agent com instrumentação (conta nós/estados e, para ID, profundidade alcançada)
# ------------------------------------------------------------------
class InstrumentedAgent:
    def __init__(self, agent_type: str, max_depth: int = 4, max_time_ms: int = 1000):
        self.agent_type = agent_type  # 'random', 'minimax', 'alphabeta', 'id'
        self.max_depth = max_depth
        self.max_time_ms = max_time_ms
        # métricas por jogada (listas)
        self.node_counts: List[int] = []
        self.times_ms: List[float] = []
        self.depths_reached: List[int] = []  # só usado para ID

    # Implementações instrumentadas (baseadas no search.py)
    def minimax(self, board, depth, maximizing, player_root) -> int:
        # incrementa nó visitado ao entrar neste nó
        self._current_node_count += 1
        term, w = search.terminal(board)
        if term:
            if w == player_root:
                return 1000000
            elif w == 0:
                return 0
            else:
                return -1000000
        if depth == 0:
            return search.heuristic(board, player_root)

        moves = search.valid_moves(board)
        if maximizing:
            best = -math.inf
            for m in moves:
                nb = search.make_move(board, m, player_root)
                val = self.minimax(nb, depth - 1, False, player_root)
                if val > best:
                    best = val
            return best
        else:
            opp = search.other(player_root)
            best = math.inf
            for m in moves:
                nb = search.make_move(board, m, opp)
                val = self.minimax(nb, depth - 1, True, player_root)
                if val < best:
                    best = val
            return best

    def alphabeta(self, board, depth, alpha, beta, maximizing, player_root) -> int:
        self._current_node_count += 1
        term, w = search.terminal(board)
        if term:
            if w == player_root:
                return 1000000
            elif w == 0:
                return 0
            else:
                return -1000000
        if depth == 0:
            return search.heuristic(board, player_root)

        moves = search.valid_moves(board)
        if maximizing:
            value = -math.inf
            for m in moves:
                nb = search.make_move(board, m, player_root)
                value = max(value, self.alphabeta(nb, depth - 1, alpha, beta, False, player_root))
                alpha = max(alpha, value)
                if alpha >= beta:
                    break
            return value
        else:
            opp = search.other(player_root)
            value = math.inf
            for m in moves:
                nb = search.make_move(board, m, opp)
                value = min(value, self.alphabeta(nb, depth - 1, alpha, beta, True, player_root))
                beta = min(beta, value)
                if alpha >= beta:
                    break
            return value

    def choose_move(self, board, player: int) -> int:
        legal = search.valid_moves(board)
        if not legal:
            return 0

        # reinicia contadores da jogada
        self._current_node_count = 0
        t0 = time.perf_counter()

        # Primeiro movimento: prefere centros (como no search.py)
        if search.is_board_empty(board):
            center_choices = [c for c in search.CENTER_COLS if c in legal]
            if center_choices:
                col = random.choice(center_choices)
                self._current_node_count += 1  # conta como um nó simples (escolha imediata)
                t1 = time.perf_counter()
                self.node_counts.append(self._current_node_count)
                self.times_ms.append((t1 - t0) * 1000.0)
                # para ID, depth 0
                if self.agent_type == 'id':
                    self.depths_reached.append(0)
                return col

        if self.agent_type == 'random':
            col = random.choice(legal)
            # conta como 1 nó (escolha trivial)
            self._current_node_count += 1
            t1 = time.perf_counter()
            self.node_counts.append(self._current_node_count)
            self.times_ms.append((t1 - t0) * 1000.0)
            if self.agent_type == 'id':
                self.depths_reached.append(0)
            return col

        elif self.agent_type == 'minimax':
            best_val = -math.inf
            best_moves = []
            depth = int(self.max_depth)
            # note: search.choose_move_minimax usa depth-1 na chamada; aqui usaremos depth-1 de forma equivalente
            for m in legal:
                nb = search.make_move(board, m, player)
                val = self.minimax(nb, depth - 1, False, player)
                if val > best_val:
                    best_val = val
                    best_moves = [m]
                elif val == best_val:
                    best_moves.append(m)
            col = random.choice(best_moves)

        elif self.agent_type == 'alphabeta':
            best_val = -math.inf
            best_moves = []
            depth = int(self.max_depth)
            # ordenação leve: preferir centro semelhante ao prefer_center_order
            ordered_moves = self._prefer_center_order(legal)
            for m in ordered_moves:
                nb = search.make_move(board, m, player)
                val = self.alphabeta(nb, depth - 1, -math.inf, math.inf, False, player)
                if val > best_val:
                    best_val = val
                    best_moves = [m]
                elif val == best_val:
                    best_moves.append(m)
            col = random.choice(best_moves)

        elif self.agent_type == 'id':
            # iterative deepening usando our alphabeta counting; respeita max_time_ms
            max_time = self.max_time_ms / 1000.0
            t_start = time.time()
            time_limit = t_start + max_time
            best_move = legal[0]
            legal_sorted = self._prefer_center_order(legal)
            max_completed_depth = 0
            for depth in range(1, int(self.max_depth) + 1):
                if time.time() >= time_limit:
                    break
                cur_best_val = -math.inf
                cur_best_moves = []
                # para cada movimento na ordem preferida
                for m in legal_sorted:
                    if time.time() >= time_limit:
                        break
                    nb = search.make_move(board, m, player)
                    val = self.alphabeta(nb, depth - 1, -math.inf, math.inf, False, player)
                    if val > cur_best_val:
                        cur_best_val = val
                        cur_best_moves = [m]
                    elif val == cur_best_val:
                        cur_best_moves.append(m)
                if cur_best_moves:
                    best_move = random.choice(cur_best_moves)
                    max_completed_depth = depth
                # próximo depth se houver tempo
            col = best_move
            # registra profundidade alcançada nesta jogada
            self.depths_reached.append(max_completed_depth)

        else:
            # fallback random
            col = random.choice(legal)
            self._current_node_count += 1

        t1 = time.perf_counter()
        elapsed_ms = (t1 - t0) * 1000.0
        # grava métricas da jogada
        self.node_counts.append(getattr(self, '_current_node_count', 0))
        self.times_ms.append(elapsed_ms)
        # para agentes não-ID, depth não aplicável (registramos 0)
        if self.agent_type != 'id':
            self.depths_reached.append(0)

        return col

    def _prefer_center_order(self, legal_moves: List[int]) -> List[int]:
        center_candidates = [c for c in search.CENTER_COLS if c in legal_moves]
        other_candidates = [c for c in legal_moves if c not in center_candidates]
        random.shuffle(center_candidates)
        return center_candidates + other_candidates

# ------------------------------------------------------------------
# Função que roda N jogos entre dois agentes e coleta métricas
# ------------------------------------------------------------------

def run_series(agentA_cfg: Dict, agentB_cfg: Dict, n_games: int = 100) -> Dict:
    """Retorna dicionário com métricas agregadas."""
    # cria agentes instrumentados
    agentA = InstrumentedAgent(agentA_cfg['type'], max_depth=agentA_cfg.get('max_depth', 4), max_time_ms=agentA_cfg.get('max_time_ms', 1000))
    agentB = InstrumentedAgent(agentB_cfg['type'], max_depth=agentB_cfg.get('max_depth', 4), max_time_ms=agentB_cfg.get('max_time_ms', 1000))

    wins_A = 0
    wins_B = 0
    draws = 0

    # para estatísticas por jogada
    moves_count_A = 0
    moves_count_B = 0

    for game_i in range(n_games):
        # inicia tabuleiro vazio
        board = [[0 for _ in range(search.COLS)] for _ in range(search.ROWS)]
        # alterna quem começa: se game_i even, A começa como P1; se odd, B começa como P1
        A_is_P1 = (game_i % 2 == 0)
        current_player = P1
        # mapeia players para agentes
        def agent_for(player):
            if A_is_P1:
                return agentA if player == P1 else agentB
            else:
                return agentB if player == P1 else agentA

        # joga até terminal
        while True:
            # escolhe agente
            agent = agent_for(current_player)
            # pede jogada instrumentada (recebe coluna)
            col = agent.choose_move(board, current_player)
            # aplica jogada
            nb = search.make_move(board, col, current_player)
            if nb is None:
                # jogada inválida (proteção) -> escolhe primeiro legal
                legal = search.valid_moves(board)
                if not legal:
                    break
                col = legal[0]
                nb = search.make_move(board, col, current_player)
            board = nb

            # atualiza contadores de jogadas
            if agent is agentA:
                moves_count_A += 1
            else:
                moves_count_B += 1

            term, w = search.terminal(board)
            if term:
                if w == P1:
                    if A_is_P1:
                        wins_A += 1
                    else:
                        wins_B += 1
                elif w == P2:
                    if A_is_P1:
                        wins_B += 1
                    else:
                        wins_A += 1
                else:
                    draws += 1
                break

            # troca jogador
            current_player = P1 if current_player == P2 else P2

    # agregados:
    # tempo médio por jogada por agente
    avg_time_A = statistics.mean(agentA.times_ms) if agentA.times_ms else 0.0
    avg_time_B = statistics.mean(agentB.times_ms) if agentB.times_ms else 0.0
    avg_nodes_A = statistics.mean(agentA.node_counts) if agentA.node_counts else 0.0
    avg_nodes_B = statistics.mean(agentB.node_counts) if agentB.node_counts else 0.0
    # profundidade média alcançada (apenas relevante para ID; se não-ID, lista contém zeros)
    avg_depth_A = statistics.mean(agentA.depths_reached) if agentA.depths_reached else 0.0
    avg_depth_B = statistics.mean(agentB.depths_reached) if agentB.depths_reached else 0.0

    results = {
        'agentA_cfg': agentA_cfg,
        'agentB_cfg': agentB_cfg,
        'games': n_games,
        'wins_A': wins_A,
        'wins_B': wins_B,
        'draws': draws,
        'win_rate_A': wins_A / n_games,
        'win_rate_B': wins_B / n_games,
        'avg_time_A_ms': avg_time_A,
        'avg_time_B_ms': avg_time_B,
        'avg_nodes_A': avg_nodes_A,
        'avg_nodes_B': avg_nodes_B,
        'avg_depth_A': avg_depth_A,
        'avg_depth_B': avg_depth_B,
    }
    return results

# ------------------------------------------------------------------
# Experimentos especificados
# ------------------------------------------------------------------

def run_all_experiments(n_games_each: int = 100):
    experiments = []

    # 1) Minimax vs Aleatório: profundidades 2,3,4,5
    for depth in [2,3,4,5]:
        exp_name = f"Minimax(d={depth}) vs Random"
        print('\nRunning:', exp_name)
        agentA = {'type':'minimax', 'max_depth': depth}
        agentB = {'type':'random'}
        res = run_series(agentA, agentB, n_games=n_games_each)
        res['name'] = exp_name
        experiments.append(res)
        print_summary(res)

    # 2) Alpha-Beta vs Minimax (sem poda) : depths 2,3,4,5
    # "Minimax (sem poda)" é o agente 'minimax' (já implementado sem poda)
    for depth in [2,3,4,5]:
        exp_name = f"AlphaBeta(d={depth}) vs Minimax(d={depth})"
        print('\nRunning:', exp_name)
        agentA = {'type':'alphabeta', 'max_depth': depth}
        agentB = {'type':'minimax', 'max_depth': depth}
        res = run_series(agentA, agentB, n_games=n_games_each)
        res['name'] = exp_name
        experiments.append(res)
        print_summary(res)

    # 3) Iterative Deepening vs Alpha-Beta: limites 1s e 2s
    for t_s in [1,2]:
        exp_name = f"ID(time={t_s}s) vs AlphaBeta(d=6)"
        print('\nRunning:', exp_name)
        agentA = {'type':'id', 'max_depth': 6, 'max_time_ms': int(t_s*1000)}
        agentB = {'type':'alphabeta', 'max_depth': 6}
        res = run_series(agentA, agentB, n_games=n_games_each)
        res['name'] = exp_name
        experiments.append(res)
        print_summary(res)

    # salva em CSV
    save_csv(experiments, 'experiments_results.csv')
    print('\nAll experiments finished. Results saved to experiments_results.csv')

# ------------------------------------------------------------------
# Utilitários de saída
# ------------------------------------------------------------------

def print_summary(res: Dict):
    name = res.get('name', 'experiment')
    print(f"\n=== {name} ===")
    print(f"Games: {res['games']}")
    print(f"AgentA cfg: {res['agentA_cfg']} | AgentB cfg: {res['agentB_cfg']}")
    print(f"Wins A: {res['wins_A']} | Wins B: {res['wins_B']} | Draws: {res['draws']}")
    print(f"Win rate A: {res['win_rate_A']:.2f} | Win rate B: {res['win_rate_B']:.2f}")
    print(f"Avg time per move A: {res['avg_time_A_ms']:.1f} ms | B: {res['avg_time_B_ms']:.1f} ms")
    print(f"Avg states visited per move A: {res['avg_nodes_A']:.1f} | B: {res['avg_nodes_B']:.1f}")
    print(f"Avg depth reached A: {res['avg_depth_A']:.2f} | B: {res['avg_depth_B']:.2f}")


def save_csv(experiments: List[Dict], filename: str):
    keys = ['name','games','agentA_cfg','agentB_cfg','wins_A','wins_B','draws','win_rate_A','win_rate_B','avg_time_A_ms','avg_time_B_ms','avg_nodes_A','avg_nodes_B','avg_depth_A','avg_depth_B']
    with open(filename, 'w', newline='') as f:
        writer = csv.writer(f)
        writer.writerow(keys)
        for e in experiments:
            row = [e.get(k) for k in keys]
            writer.writerow(row)

# ------------------------------------------------------------------
# Entry point
# ------------------------------------------------------------------
if __name__ == '__main__':
    # ajuste N se quiser testes mais rápidos
    N = 100
    run_all_experiments(n_games_each=N)

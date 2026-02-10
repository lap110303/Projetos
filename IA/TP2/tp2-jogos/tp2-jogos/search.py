from typing import List, Tuple, Optional, Dict
import time
import math
import random

ROWS, COLS = 6, 7
EMPTY, P1, P2 = 0, 1, 2


# -----------------------------------------------------------------------------
# Utilidades de tabuleiro (PRONTAS)
# -----------------------------------------------------------------------------
def copy_board(board: List[List[int]]) -> List[List[int]]:
    return [row[:] for row in board]

def valid_moves(board: List[List[int]]) -> List[int]:
    """Retorna as colunas ainda jogáveis (topo vazio)."""
    return [c for c in range(COLS) if board[0][c] == EMPTY]

def make_move(board: List[List[int]], col: int, player: int) -> Optional[List[List[int]]]:
    """Retorna um novo tabuleiro aplicando a gravidade na coluna col; None se inválido."""
    if col < 0 or col >= COLS or board[0][col] != EMPTY:
        return None
    nb = copy_board(board)
    for r in reversed(range(ROWS)):
        if nb[r][col] == EMPTY:
            nb[r][col] = player
            return nb
    return None

def winner(board: List[List[int]]) -> int:
    """0 se ninguém venceu; 1 ou 2 se há 4 em linha."""
    # Horizontais
    for r in range(ROWS):
        for c in range(COLS - 3):
            x = board[r][c]
            if x != EMPTY and x == board[r][c+1] == board[r][c+2] == board[r][c+3]:
                return x
    # Verticais
    for c in range(COLS):
        for r in range(ROWS - 3):
            x = board[r][c]
            if x != EMPTY and x == board[r+1][c] == board[r+2][c] == board[r+3][c]:
                return x
    # Diag ↘
    for r in range(ROWS - 3):
        for c in range(COLS - 3):
            x = board[r][c]
            if x != EMPTY and x == board[r+1][c+1] == board[r+2][c+2] == board[r+3][c+3]:
                return x
    # Diag ↗
    for r in range(3, ROWS):
        for c in range(COLS - 3):
            x = board[r][c]
            if x != EMPTY and x == board[r-1][c+1] == board[r-2][c+2] == board[r-3][c+3]:
                return x
    return 0

def is_full(board: List[List[int]]) -> bool:
    return all(board[0][c] != EMPTY for c in range(COLS))

def terminal(board: List[List[int]]) -> Tuple[bool, int]:
    """(é_terminal, vencedor) com vencedor=0 para empate/indefinido."""
    w = winner(board)
    if w != 0:
        return True, w
    if is_full(board):
        return True, 0
    return False, 0

def other(player: int) -> int:
    return P1 if player == P2 else P2

# -----------------------------------------------------------------------------
# ÚNICO PONTO A SER IMPLEMENTADO PELOS ALUNOS
# -----------------------------------------------------------------------------

# ---------------------------------------------------------------------
# Compatibilidade com novo código-base / nomes esperados por server.py
# Adicione estes aliases para que o dicionário AI_PLAYERS do server possa
# referenciar funções como search.minimax, search.alpha_beta, etc.
# ---------------------------------------------------------------------
def choose_move_infinity(board: List[List[int]], player: int, config: Dict) -> int:
    """
    Agente dummy que entra em loop infinito para testar timeout do servidor.
    O servidor deverá interromper essa chamada quando o timeout for atingido.
    """
    # loop com sleep para não ocupar 100% CPU, ainda assim é 'infinito'
    while True:
        time.sleep(1)

# Aliases mais curtos / nomes alternativos que o server pode usar:
def choose_random(board: List[List[int]], player: int, config: Dict) -> int:
    """Alias para o agente aleatório."""
    return choose_move_random(board, player, config)

def minimax(board: List[List[int]], player: int, config: Dict) -> int:
    """Alias para o agente Minimax (profundidade limitada)."""
    return choose_move_minimax(board, player, config)

def alpha_beta(board: List[List[int]], player: int, config: Dict) -> int:
    """Alias para o agente Alfa-Beta."""
    return choose_move_alphabeta(board, player, config)

def iterative_deepening(board: List[List[int]], player: int, config: Dict) -> int:
    """Alias para Iterative Deepening (grafia correta)."""
    return choose_move_iterative_deepening(board, player, config)

def iterative_deepning(board: List[List[int]], player: int, config: Dict) -> int:
    """Alias com grafia alternativa/typo (muitos exemplos usam variações)."""
    return choose_move_iterative_deepening(board, player, config)

# --- helpers para aleatoriedade e centro ---
CENTER_COLS = [ (COLS // 2) - 1, (COLS // 2), (COLS // 2) + 1 ]  # [2,3,4] para COLS==7

def is_board_empty(board):
    """Retorna True se nenhuma peça estiver no tabuleiro."""
    for r in range(ROWS):
        for c in range(COLS):
            if board[r][c] != EMPTY:
                return False
    return True

def prefer_center_order(legal):
    """
    Retorna uma lista de movimentos com preferência por centro,
    porém embaralhando a ordem dos 3 centros entre si para gerar variedade.
    """
    # centrais possíveis na jogada (laterais podem não estar em 'legal' se já preenchidas)
    center_candidates = [c for c in CENTER_COLS if c in legal]
    other_candidates = [c for c in legal if c not in center_candidates]
    # embaralha apenas as centrais para igualar preferência entre elas
    random.shuffle(center_candidates)
    # agora junta: centrais embaralhadas primeiro, depois os outros (poderíamos também embaralhar os outros se quisermos mais aleatoriedade)
    return center_candidates + other_candidates

# ---------------------------------------------------------------------
# Implementação de dispatch para vários agentes (aleatório / minimax / alfa-beta / ID)
# ---------------------------------------------------------------------

# Heurística simples (contagem de janelas)
def evaluate_window(window: List[int], player: int) -> int:
    opp = other(player)
    score = 0
    if window.count(player) == 4:
        score += 100000
    elif window.count(player) == 3 and window.count(EMPTY) == 1:
        score += 50
    elif window.count(player) == 2 and window.count(EMPTY) == 2:
        score += 10

    if window.count(opp) == 3 and window.count(EMPTY) == 1:
        score -= 80  # evitar permitir vitória do oponente
    return score

def heuristic(board: List[List[int]], player: int) -> int:
    """Avaliação simples: centro (3 colunas) + janelas horizontais/verticais/diagonais."""
    score = 0
    # centro: agora considera as 3 colunas centrais igualmente (colunas mid-1, mid, mid+1)
    mid = COLS // 2  # para COLS==7 -> mid = 3
    center_cols = [mid-1, mid, mid+1]  # [2,3,4]
    center_count = 0
    for c in center_cols:
        for r in range(ROWS):
            if board[r][c] == player:
                center_count += 1
    # cada peça no "3-centro" vale 3 (mesmo peso que antes por peça central)
    score += center_count * 3

    # horizontal
    for r in range(ROWS):
        for c in range(COLS - 3):
            window = [board[r][c+i] for i in range(4)]
            score += evaluate_window(window, player)

    # vertical
    for c in range(COLS):
        for r in range(ROWS - 3):
            window = [board[r+i][c] for i in range(4)]
            score += evaluate_window(window, player)

    # diag ↘
    for r in range(ROWS - 3):
        for c in range(COLS - 3):
            window = [board[r+i][c+i] for i in range(4)]
            score += evaluate_window(window, player)

    # diag ↗
    for r in range(3, ROWS):
        for c in range(COLS - 3):
            window = [board[r-i][c+i] for i in range(4)]
            score += evaluate_window(window, player)

    return score

# Minimax (usando make_move que retorna nova cópia)
def minimax(board: List[List[int]], depth: int, maximizing: bool, player_root: int) -> int:
    term, w = terminal(board)
    if term:
        if w == player_root:
            return 1000000
        elif w == 0:
            return 0
        else:
            return -1000000
    if depth == 0:
        return heuristic(board, player_root)

    moves = valid_moves(board)
    if maximizing:
        best = -math.inf
        for m in moves:
            nb = make_move(board, m, player_root)
            val = minimax(nb, depth - 1, False, player_root)
            best = max(best, val)
        return best
    else:
        opp = other(player_root)
        best = math.inf
        for m in moves:
            nb = make_move(board, m, opp)
            val = minimax(nb, depth - 1, True, player_root)
            best = min(best, val)
        return best

# Alpha-Beta
def alphabeta(board: List[List[int]], depth: int, alpha: int, beta: int, maximizing: bool, player_root: int) -> int:
    term, w = terminal(board)
    if term:
        if w == player_root:
            return 1000000
        elif w == 0:
            return 0
        else:
            return -1000000
    if depth == 0:
        return heuristic(board, player_root)

    moves = valid_moves(board)
    if maximizing:
        value = -math.inf
        for m in moves:
            nb = make_move(board, m, player_root)
            value = max(value, alphabeta(nb, depth - 1, alpha, beta, False, player_root))
            alpha = max(alpha, value)
            if alpha >= beta:
                break
        return value
    else:
        opp = other(player_root)
        value = math.inf
        for m in moves:
            nb = make_move(board, m, opp)
            value = min(value, alphabeta(nb, depth - 1, alpha, beta, True, player_root))
            beta = min(beta, value)
            if alpha >= beta:
                break
        return value

# Root wrappers que retornam a coluna escolhida
def choose_move_random(board: List[List[int]], player: int, config: Dict) -> int:
    legal = valid_moves(board)
    if not legal:
        return 0
    return random.choice(legal)

def choose_move_minimax(board: List[List[int]], player: int, config: Dict) -> int:
    legal = valid_moves(board)
    if not legal:
        return 0

    # Se for primeiro movimento do jogo, escolha aleatória entre as 3 colunas centrais (quando disponíveis)
    if is_board_empty(board):
        center_choices = [c for c in CENTER_COLS if c in legal]
        if center_choices:
            return random.choice(center_choices)
        else:
            return random.choice(legal)

    best_val = -math.inf
    best_moves = []
    depth = int(config.get("max_depth", 3))
    for m in legal:
        nb = make_move(board, m, player)
        val = minimax(nb, depth - 1, False, player)
        if val > best_val:
            best_val = val
            best_moves = [m]
        elif val == best_val:
            best_moves.append(m)
    # desempate aleatório entre melhores
    return random.choice(best_moves)

def choose_move_alphabeta(board: List[List[int]], player: int, config: Dict) -> int:
    legal = valid_moves(board)
    if not legal:
        return 0

    # Se for primeiro movimento do jogo, escolha aleatória entre as 3 colunas centrais (quando disponíveis)
    if is_board_empty(board):
        center_choices = [c for c in CENTER_COLS if c in legal]
        if center_choices:
            # também embaralha centrais para diversidade
            return random.choice(center_choices)
        else:
            return random.choice(legal)

    best_val = -math.inf
    best_moves = []
    depth = int(config.get("max_depth", 4))

    # gera ordem preferencial com leve aleatoriedade nas 3 centrais
    ordered_moves = prefer_center_order(legal)

    for m in ordered_moves:
        nb = make_move(board, m, player)
        val = alphabeta(nb, depth - 1, -math.inf, math.inf, False, player)
        if val > best_val:
            best_val = val
            best_moves = [m]
        elif val == best_val:
            best_moves.append(m)
    return random.choice(best_moves)

def choose_move_iterative_deepening(board: List[List[int]], player: int, config: Dict) -> int:
    legal = valid_moves(board)
    if not legal:
        return 0

    max_time_ms = int(config.get("max_time_ms") or 0)
    max_depth = int(config.get("max_depth") or 6)

    # Se for primeiro movimento, escolha aleatória entre as 3 colunas centrais
    if is_board_empty(board):
        center_choices = [c for c in CENTER_COLS if c in legal]
        if center_choices:
            return random.choice(center_choices)
        else:
            return random.choice(legal)

    start = time.time()
    time_limit = start + (max_time_ms / 1000.0 if max_time_ms > 0 else 10.0)

    best_move = legal[0]

    # ordem inicial: centrais com embaralhamento, depois demais
    legal_sorted = prefer_center_order(legal)

    for depth in range(1, max_depth + 1):
        # checar limite de tempo antes de começar iteração
        if time.time() >= time_limit:
            break

        cur_best_val = -math.inf
        cur_best_moves = []

        for m in legal_sorted:
            if time.time() >= time_limit:
                break
            nb = make_move(board, m, player)
            val = alphabeta(nb, depth - 1, -math.inf, math.inf, False, player)
            if val > cur_best_val:
                cur_best_val = val
                cur_best_moves = [m]
            elif val == cur_best_val:
                cur_best_moves.append(m)

        if cur_best_moves:
            # desempate aleatório entre os melhores desta profundidade
            best_move = random.choice(cur_best_moves)
        # continue para próxima profundidade, se houver tempo

    return best_move


# Função principal (dispatcher)
def choose_move(board: List[List[int]], player: int, config: Dict) -> int:
    """
    Dispatcher para diferentes agentes com base em config['agent'].
    Retorna um int (coluna).
    """
    agent = config.get("agent", "ai_random")
    max_time_ms = int(config.get("max_time_ms") or 0)
    max_depth = int(config.get("max_depth") or 4)
    print(f"[search.choose_move] agent={agent}, player={player}, max_time_ms={max_time_ms}, max_depth={max_depth}")

    if agent == "ai_random":
        return choose_move_random(board, player, config)
    elif agent == "ai_minimax":
        return choose_move_minimax(board, player, config)
    elif agent == "ai_alphabeta":
        return choose_move_alphabeta(board, player, config)
    elif agent == "ai_id":
        return choose_move_iterative_deepening(board, player, config)
    else:
        # fallback
        return choose_move_random(board, player, config)
import heapq
from math import sqrt
from collections import deque

WALL = 'X'
START_STATE = 'S'
GOAL_STATE  = 'G'

def plan(map, algorithm='bfs', heuristic=None):
    """ Loads a level, searches for a path between the given waypoints, and displays the result.
    """
    print(map)
    print("Algorithm:", algorithm)
    print("Heuristic:", heuristic)

    # Load the level from the file
    level = parse_level(map)

    # Retrieve the source and destination coordinates from the level.
    start = level['start']
    goal = level['goal']

    # Search for and display the path from src to dst.
    path = []
    visited = {}

    if algorithm == 'bfs':
        path, visited = bfs(start, goal, level, transition_model)
    elif algorithm == 'dfs':
        path, visited = dfs(start, goal, level, transition_model)
    elif algorithm == 'ucs':
        path, visited = ucs(start, goal, level, transition_model)
    elif algorithm == 'greedy':
        if heuristic == 'euclidian':
            path, visited = greedy_best_first(start, goal, level, transition_model, h_euclidian)
        elif heuristic == 'manhattan':
            path, visited = greedy_best_first(start, goal, level, transition_model, h_manhattan)
    elif algorithm == 'astar':
        if heuristic == 'euclidian':
            path, visited = a_star(start, goal, level, transition_model, h_euclidian)
        elif heuristic == 'manhattan':
            path, visited = a_star(start, goal, level, transition_model, h_manhattan)

    return path, path_cost(path, level), visited

def parse_level(map):
    """ Parses a level from a string.
    """
    start = None
    goal = None
    walls = set()
    spaces = {}

    for j, line in enumerate(map.split('\n')):
        for i, char in enumerate(line):
            if char == '\n':
                continue
            elif char == WALL:
                walls.add((i, j))
            elif char == START_STATE:
                start = (i, j)
                spaces[(i, j)] = 1.
            elif char == GOAL_STATE:
                goal = (i, j)
                spaces[(i, j)] = 1.
            elif char.isnumeric():
                spaces[(i, j)] = float(char)

    level = {'walls': walls, 'spaces': spaces, 'start': start, 'goal': goal}

    return level

def path_cost(path, level):
    """ Returns the cost of the given path.
    """
    cost = 0
    for i in range(len(path) - 1):
        cost += cost_function(level, path[i], path[i + 1],
                              level['spaces'][path[i]],
                              level['spaces'][path[i + 1]])

    return cost

# =============================
# Transition Model
# =============================

def cost_function(level, state1, state2, cost1, cost2):
    """ Returns the cost of the edge joining state1 and state2.
    Uses: dist(state1,state2) * (cost1 + cost2)/2
    """
    (x1, y1) = state1
    (x2, y2) = state2
    dist = sqrt((x1 - x2) ** 2 + (y1 - y2) ** 2)
    return dist * ((cost1 + cost2) / 2.0)


def transition_model(level, state1):
    """ Provides a list of adjacent states and their respective costs from the given state.
        Allows 8-directional movement. Returns an iterable of (neighbor, cost).
    """
    (x, y) = state1
    neighbors = [(-1, -1), (0, -1), (1, -1),
                 (-1,  0),          (1,  0),
                 (-1,  1), (0,  1), (1,  1)]

    adj_states = {}
    for dx, dy in neighbors:
        nx, ny = x + dx, y + dy
        new_state = (nx, ny)
        # only allow movement to valid spaces (not walls and within bounds of map)
        if new_state in level['spaces']:
            cost = cost_function(level, state1, new_state,
                                 level['spaces'][state1],
                                 level['spaces'][new_state])
            adj_states[new_state] = cost

    return adj_states.items()

# =============================
# Helpers
# =============================

def reconstruct_path(came_from, start, goal):
    """Reconstruct path from came_from mapping. Returns list start->...->goal."""
    if goal not in came_from:
        return []
    path = [goal]
    while path[-1] != start:
        parent = came_from.get(path[-1])
        if parent is None:
            # no path
            return []
        path.append(parent)
    path.reverse()
    return path

# =============================
# Uninformed Search Algorithms
# =============================

def bfs(s, g, level, adj):
    """ Breadth-first search. visited contains expanded nodes mapped to their parent. """
    came_from = {s: None}
    visited = {}  # mapping node -> parent for expanded nodes
    frontier = deque([s])

    while frontier:
        current = frontier.popleft()
        # mark as expanded
        visited[current] = came_from[current]

        if current == g:
            path = reconstruct_path(came_from, s, g)
            return path, visited

        for neighbor, _ in adj(level, current):
            if neighbor not in came_from:
                came_from[neighbor] = current
                frontier.append(neighbor)

    return [], visited


def dfs(s, g, level, adj):
    """ Depth-first search. visited contains expanded nodes mapped to their parent. """
    came_from = {s: None}
    visited = {}
    frontier = [s]  # stack (LIFO)

    while frontier:
        current = frontier.pop()
        # mark as expanded
        if current not in visited:
            visited[current] = came_from[current]

        if current == g:
            path = reconstruct_path(came_from, s, g)
            return path, visited

        # expand neighbors: to keep behavior deterministic, convert to list
        for neighbor, _ in adj(level, current):
            if neighbor not in came_from:
                came_from[neighbor] = current
                frontier.append(neighbor)

    return [], visited


def ucs(s, g, level, adj):
    """ Uniform-cost search (Dijkstra). visited contains expanded nodes mapped to their parent.
        Frontier entries are (path_cost, node).
    """
    came_from = {s: None}
    cost_so_far = {s: 0.0}
    visited = {}
    frontier = []
    heapq.heappush(frontier, (0.0, s))

    while frontier:
        current_cost, current = heapq.heappop(frontier)

        # If this entry is outdated (we have a better cost), skip it
        if current_cost > cost_so_far.get(current, float('inf')):
            continue

        # mark as expanded
        visited[current] = came_from[current]

        if current == g:
            path = reconstruct_path(came_from, s, g)
            return path, visited

        for neighbor, move_cost in adj(level, current):
            new_cost = current_cost + move_cost
            if neighbor not in cost_so_far or new_cost < cost_so_far[neighbor]:
                cost_so_far[neighbor] = new_cost
                came_from[neighbor] = current
                heapq.heappush(frontier, (new_cost, neighbor))

    return [], visited

# ======================================
# Informed (Heuristic) Search Algorithms
# ======================================

def greedy_best_first(s, g, level, adj, h):
    """ Greedy best-first search. Priority by h(n). visited contains expanded nodes. """
    came_from = {s: None}
    visited = {}
    frontier = []
    # push (priority, unique_counter, node) to avoid tie issues; counter not strictly necessary here
    heapq.heappush(frontier, (h(s, g), 0, s))
    counter = 1

    # to avoid repeatedly adding same node without updating parent, keep discovered set
    discovered = {s}

    while frontier:
        _, _, current = heapq.heappop(frontier)

        # mark as expanded
        visited[current] = came_from[current]

        if current == g:
            path = reconstruct_path(came_from, s, g)
            return path, visited

        for neighbor, _ in adj(level, current):
            if neighbor not in discovered:
                discovered.add(neighbor)
                came_from[neighbor] = current
                heapq.heappush(frontier, (h(neighbor, g), counter, neighbor))
                counter += 1

    return [], visited


def a_star(s, g, level, adj, h):
    """ A* search. visited contains expanded nodes. Frontier entries: (priority = g + h, g_cost, node) """
    came_from = {s: None}
    cost_so_far = {s: 0.0}
    visited = {}
    frontier = []
    heapq.heappush(frontier, (h(s, g), 0.0, s))

    while frontier:
        priority, current_g, current = heapq.heappop(frontier)

        # ignore outdated entries
        if current_g > cost_so_far.get(current, float('inf')):
            continue

        # mark as expanded
        visited[current] = came_from[current]

        if current == g:
            path = reconstruct_path(came_from, s, g)
            return path, visited

        for neighbor, move_cost in adj(level, current):
            new_g = current_g + move_cost
            if neighbor not in cost_so_far or new_g < cost_so_far[neighbor]:
                cost_so_far[neighbor] = new_g
                came_from[neighbor] = current
                new_priority = new_g + h(neighbor, g)
                heapq.heappush(frontier, (new_priority, new_g, neighbor))

    return [], visited

# ======================================
# Heuristic functions
# ======================================

def h_euclidian(s, g):
    """ Euclidean distance heuristic. """
    (x1, y1) = s
    (x2, y2) = g
    return sqrt((x1 - x2) ** 2 + (y1 - y2) ** 2)


def h_manhattan(s, g):
    """ Manhattan distance heuristic. """
    (x1, y1) = s
    (x2, y2) = g
    return abs(x1 - x2) + abs(y1 - y2)

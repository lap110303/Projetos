"""POC I — Aprendizado de máquina para seleção de escolhas gulosas em coloração de grafos.

Pacote com geração de grafos, heurísticas gulosas de referência, rollouts
e extração de atributos. Todos os grafos são representados como matrizes de
adjacência densas booleanas (numpy), o que simplifica e acelera as operações
vetorizadas para os tamanhos usados no projeto (n <= 10.000).
"""
__version__ = "0.1.0"

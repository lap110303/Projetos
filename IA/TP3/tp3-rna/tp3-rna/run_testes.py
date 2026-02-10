import numpy as np
import learn

# Configuração para imprimir arrays de forma similar ao numpy padrão
np.set_printoptions(precision=8, suppress=True, linewidth=150)

def print_separator(title):
    print(f"\n{'='*20} {title} {'='*20}")

# ---------------------------------------------------------
# 1. Inicialização dos Parâmetros
# ---------------------------------------------------------
print_separator("1. TESTE DE INICIALIZAÇÃO")
np.random.seed(42) # Reset para reproduzir exatamente a criação dos pesos
W1, b1, W2, b2 = learn.initialize_parameters(784, 4, 10)

print(f"W1 shape: {W1.shape}")
print("W1 (primeiras linhas):\n", W1[:6, :4]) # Mostra pedaço para conferir
print(f"\nb1 shape: {b1.shape}")
print("b1:\n", b1)
print(f"\nW2 shape: {W2.shape}")
print("W2 (primeiras linhas):\n", W2[:4, :])
print(f"\nb2 shape: {b2.shape}")
print("b2:\n", b2)

# ---------------------------------------------------------
# 2. Funções de Ativação
# ---------------------------------------------------------
print_separator("2. TESTE DE ATIVAÇÕES")

# ReLU
a_relu = learn.relu(np.array([[-1, 0, 1], [-2, 0, 2]]))
print("ReLU result:\n", a_relu)

# Softmax
a_softmax = learn.softmax(np.array([[1, 2, 3], [4, 5, 6]]))
print("\nSoftmax result:\n", a_softmax)

# ---------------------------------------------------------
# 3. Propagação Forward
# ---------------------------------------------------------
print_separator("3. TESTE DE FORWARD")
# Para replicar o enunciado, resetamos o seed. 
# Assim os Pesos serão inicializados iguais ao passo 1, 
# e o X será gerado com a mesma aleatoriedade esperada.
np.random.seed(42) 

# Recria os parametros e o X exatamente como o professor fez na sequência
params = learn.initialize_parameters(784, 4, 10)
X = np.random.randn(2, 784) 

Z1, A1, Z2, A2 = learn.forward(X, params)

print(f"Z1 shape: {Z1.shape}")
print("Z1:\n", Z1)
print(f"\nA1 shape: {A1.shape}")
print("A1:\n", A1)
print(f"\nZ2 shape: {Z2.shape}")
print("Z2:\n", Z2)
print(f"\nA2 shape: {A2.shape}")
print("A2:\n", A2)

# ---------------------------------------------------------
# 4. Função de Perda
# ---------------------------------------------------------
print_separator("4. TESTE DE LOSS")
y_true = np.array([[0, 1, 0], [1, 0, 0]])
y_pred = np.array([[0.1, 0.6, 0.3], [0.3, 0.4, 0.3]])
loss = learn.cross_entropy_loss(y_true, y_pred)
print("Loss:", loss)

# ---------------------------------------------------------
# 5. Gradiente Descendente
# ---------------------------------------------------------
print_separator("5. TESTE DE GRADIENTE DESCENDENTE")
# Resetamos o seed novamente para garantir que X e pesos estejam no estado inicial
np.random.seed(42)

# 1. Gera pesos iniciais iguais ao enunciado
params = learn.initialize_parameters(784, 4, 10)
# 2. Gera X igual ao enunciado (o primeiro randn após seed 42 e init)
X = np.random.randn(2, 784) 
# 3. Define y fixo
y = np.array([[0, 1, 0, 0, 0, 0, 0, 0, 0, 0], [0, 0, 0, 0, 0, 0, 0, 1, 0, 0]])

# Executa o passo
new_params, loss_step = learn.gradient_descent_step(X, y, params, learning_rate=0.01)
nW1, nb1, nW2, nb2 = new_params

print("Loss após 1 passo:", loss_step)

print("\nW1 atualizado (primeiras linhas):")
print(nW1[:6, :])

print("\nb1 atualizado:")
print(nb1)

print("\nW2 atualizado (primeiras linhas):")
print(nW2[:4, :])

print("\nb2 atualizado:")
print(nb2)

# ---------------------------------------------------------
# 6. Avaliação (Acurácia)
# ---------------------------------------------------------
print_separator("6. TESTE DE ACURÁCIA")
y_true_acc = np.array([[0, 1, 0], [1, 0, 0]])
y_pred_acc = np.array([[0.1, 0.6, 0.3], [0.3, 0.4, 0.3]])
acc = learn.accuracy(y_true_acc, y_pred_acc)
print("Acurácia:", acc)
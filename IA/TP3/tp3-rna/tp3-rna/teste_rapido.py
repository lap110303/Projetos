import numpy as np
import learn

# Garante a mesma aleatoriedade do enunciado para comparar W1, W2
np.random.seed(42) 

print("--- 1. Teste de Inicialização ---")
try:
    W1, b1, W2, b2 = learn.initialize_parameters(784, 4, 10)
    print(f"W1 shape: {W1.shape} (Esperado: (784, 4))")
    print(f"b1 shape: {b1.shape} (Esperado: (1, 4))")
    print("W1 (primeiras linhas):\n", W1[:3, :])
except Exception as e:
    print("Erro na inicialização:", e)

print("\n--- 2. Teste de Ativações ---")
try:
    z_relu = np.array([[-1, 0, 1], [-2, 0, 2]])
    print("ReLU Entrada:\n", z_relu)
    print("ReLU Saída:\n", learn.relu(z_relu))
    
    z_soft = np.array([[1, 2, 3], [4, 5, 6]])
    print("\nSoftmax Entrada:\n", z_soft)
    print("Softmax Saída:\n", learn.softmax(z_soft))
except Exception as e:
    print("Erro nas ativações:", e)

print("\n--- 3. Teste de Forward ---")
try:
    X = np.random.randn(2, 784)
    # Recriamos parametros aleatorios para o teste de forward isolado
    params = learn.initialize_parameters(784, 4, 10) 
    Z1, A1, Z2, A2 = learn.forward(X, params)
    print(f"Z1 shape: {Z1.shape}")
    print(f"A2 (Output) shape: {A2.shape}")
    print("A2 (Output preview):\n", A2)
except Exception as e:
    print("Erro no Forward:", e)

print("\n--- 4. Teste de Loss e Acurácia ---")
try:
    y_true = np.array([[0, 1, 0], [1, 0, 0]])
    y_pred = np.array([[0.1, 0.6, 0.3], [0.3, 0.4, 0.3]])
    
    loss = learn.cross_entropy_loss(y_true, y_pred)
    acc = learn.accuracy(y_true, y_pred)
    
    print(f"Loss Calculada: {loss:.5f} (Esperado aprox: 0.85739)")
    print(f"Acurácia Calculada: {acc} (Esperado: 0.5)")
except Exception as e:
    print("Erro em Loss/Accuracy:", e)
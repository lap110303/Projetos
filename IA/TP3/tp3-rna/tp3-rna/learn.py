import numpy as np
np.random.seed(42)

from sklearn.datasets import fetch_openml
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import OneHotEncoder

def load_mnist():
    # Load MNIST data from sklearn
    mnist = fetch_openml('mnist_784', as_frame=False, cache=True, version=1)
    X, y = mnist["data"], mnist["target"].astype(int)

    # Normalize the data
    X = X / 255.0

    # Split the data into training and testing sets
    X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)

    # One-hot encode the labels
    encoder = OneHotEncoder(sparse_output=False) # Nota: sparse=False depreciado em versões novas, use sparse_output se der erro
    y_train = encoder.fit_transform(y_train.reshape(-1, 1))
    y_test = encoder.transform(y_test.reshape(-1, 1))

    return X_train, X_test, y_train, y_test

def initialize_parameters(input_size, hidden_size, output_size):
    """
    Inicializa os parâmetros da rede neural.
    W são inicializados com valores aleatórios pequenos (* 0.01).
    b são inicializados com zeros.
    """
    W1 = np.random.randn(input_size, hidden_size) * 0.01
    b1 = np.zeros((1, hidden_size))
    
    W2 = np.random.randn(hidden_size, output_size) * 0.01
    b2 = np.zeros((1, output_size))
    
    return W1, b1, W2, b2

# Activation functions
def relu(Z):
    """
    Implementa a função de ativação ReLU: max(0, Z)
    """
    return np.maximum(0, Z)

def relu_derivative(Z):
    return Z > 0

def softmax(Z):
    """
    Implementa a função de ativação Softmax.
    """
    # Calcula e^z
    exp_Z = np.exp(Z)
    # Divide pela soma das exponenciais ao longo das classes (axis=1)
    # keepdims=True garante que a dimensão (m, 1) seja mantida para o broadcast correto
    A = exp_Z / np.sum(exp_Z, axis=1, keepdims=True)
    return A

# Forward pass
def forward(X, parameters):
    """
    Realiza a propagação das entradas pela rede (Forward Propagation).
    """
    W1, b1, W2, b2 = parameters
    
    # Camada Escondida (Hidden Layer)
    Z1 = np.dot(X, W1) + b1
    A1 = relu(Z1)
    
    # Camada de Saída (Output Layer)
    Z2 = np.dot(A1, W2) + b2
    A2 = softmax(Z2)
    
    return Z1, A1, Z2, A2

# Loss function
def cross_entropy_loss(y_true, y_pred):
    """
    Calcula a Entropia Cruzada Categórica.
    """
    m = y_true.shape[0] # número de exemplos
    
    # Adicionamos um epsilon minúsculo para evitar log(0) caso a rede preveja 0 exato
    epsilon = 1e-15 
    
    # Fórmula: -1/m * sum(sum(y_true * log(y_pred)))
    loss = -np.sum(y_true * np.log(y_pred + epsilon)) / m
    
    return loss

# Backward pass
def backward(X, y, W2, Z1, A1, A2):
    n_samples = X.shape[0]
    
    dZ2 = A2 - y
    dW2 = np.dot(A1.T, dZ2) / n_samples
    db2 = np.sum(dZ2, axis=0, keepdims=True) / n_samples
    dA1 = np.dot(dZ2, W2.T)
    
    dZ1 = dA1 * relu_derivative(Z1)
    dW1 = np.dot(X.T, dZ1) / n_samples
    db1 = np.sum(dZ1, axis=0, keepdims=True) / n_samples
    
    return dW1, db1, dW2, db2

def gradient_descent_step(X, y, parameters, learning_rate=0.01):
    """
    Executa um passo de atualização dos pesos usando Gradiente Descendente.
    """
    W1, b1, W2, b2 = parameters
    
    # 1. Forward propagation
    Z1, A1, Z2, A2 = forward(X, parameters)
    
    # 2. Calcular a perda (Loss)
    loss = cross_entropy_loss(y, A2)
    
    # 3. Backward propagation (cálculo dos gradientes)
    dW1, db1, dW2, db2 = backward(X, y, W2, Z1, A1, A2)
    
    # 4. Atualização dos parâmetros
    W1 = W1 - learning_rate * dW1
    b1 = b1 - learning_rate * db1
    W2 = W2 - learning_rate * dW2
    b2 = b2 - learning_rate * db2
    
    new_parameters = (W1, b1, W2, b2)
    
    return new_parameters, loss

def accuracy(y_true, y_pred):
    """
    Calcula a taxa de acerto (acurácia) da rede.
    """
    # np.argmax retorna o índice do maior valor (a classe prevista)
    # axis=1 opera ao longo das colunas (para cada exemplo)
    predictions = np.argmax(y_pred, axis=1)
    labels = np.argmax(y_true, axis=1)
    
    # Compara as previsões com os rótulos reais e calcula a média
    acc = np.mean(predictions == labels)
    
    return acc
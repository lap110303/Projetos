import numpy as np
import matplotlib
matplotlib.use('Agg')  # Use the Agg backend for non-interactive plotting
import matplotlib.pyplot as plt

import learn
from flask import Flask, render_template, request, jsonify
from flask_socketio import SocketIO, emit
from flask_cors import CORS

app = Flask(__name__)
CORS(app)

socketio = SocketIO(app, async_mode='threading', cors_allowed_origins="*")

# RNN hiper-pamaraters
epoch = 0
max_epochs = 1000
learning_rate = 0.01
parameters = {}

# Dataset variables
X_train, X_test = [], []
y_train, y_test = [], []

# Learning curve
losses = []
accuracies = []

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/start_learn', methods=['GET'])
def start_learn():
    # Get hipermarameters
    hidden_size = int(request.args.get('hidden_size'))

    global max_epochs
    max_epochs = int(request.args.get('max_epochs'))

    global learning_rate
    learning_rate = float(request.args.get('learning_rate'))

    global epoch
    epoch = 0
    
    init_parameters = request.args.get('init_parameters') == 'true'
    print(f"-> Hipermarameters: max_epochs={max_epochs}, learning_rate={learning_rate}, hidden_size={hidden_size}, init_parameters={init_parameters}")

    global losses
    global accuracies
    global parameters
    if init_parameters:
        # Initialize parameters
        parameters = learn.initialize_parameters(784, hidden_size, 10)
        print("-> Parameters initialized")

        losses = []
        accuracies = []
    else:
        W1 = np.load('model/W1.npy')
        b1 = np.load('model/b1.npy')
        W2 = np.load('model/W2.npy')
        b2 = np.load('model/b2.npy')
        parameters = (W1, b1, W2, b2)

        losses = np.loadtxt('model/losses.csv', delimiter=',').tolist()
        accuracies = np.loadtxt('model/accuracies.csv', delimiter=',').tolist()

        print("-> Parameters loaded from disk")
        print(parameters)

    # Load dataset
    print("-> Loading MNIST dataset")
     
    global X_train, X_test, y_train, y_test
    X_train, X_test, y_train, y_test = learn.load_mnist()

    # Example usage
    save_mnist_image(X_train, 0, 'mnist_digit_0.png')

    # Perform search algorithm logic here with map_name and algorithm_name
    # For demonstration purposes, just returning a simple response
    response = jsonify({'result': 'success'})
    response.headers.add("Access-Control-Allow-Origin", "*")

    return response

@app.route('/save_results', methods=['GET'])
def save_results():
    try:
        W1, b1, W2, b2 = parameters

        # Save weights to a npy file
        np.save('model/W1.npy', W1)
        np.save('model/b1.npy', b1)
        np.save('model/W2.npy', W2)
        np.save('model/b2.npy', b2)

        # Save losses and accuracies to a csv file
        np.savetxt('model/losses.csv', losses, delimiter=',')
        np.savetxt('model/accuracies.csv', accuracies, delimiter=',')

        response = jsonify({'result': 'success'})
        response.headers.add("Access-Control-Allow-Origin", "*")

        return response

    except Exception as e:
        return jsonify({'result': 'error', 'error_details': str(e)}), 500
    
@app.route('/load_results', methods=['GET'])
def load_results():
    try:
        W1 = np.load('model/W1.npy')
        b1 = np.load('model/b1.npy')
        W2 = np.load('model/W2.npy')
        b2 = np.load('model/b2.npy')

        global parameters
        parameters = [W1, b1, W2, b2]

        response = jsonify({'result': 'success', 'parameters': serialize_parameters(parameters)})
        response.headers.add("Access-Control-Allow-Origin", "*")

        return response

    except Exception as e:
        return jsonify({'result': 'error', 'error_details': str(e)}), 500

@socketio.on('message_from_client')
def handle_message(data):
    msg_type = data['type']
    if msg_type == 'step':
        if epoch < max_epochs:
            # Perform a step of gradient descent
            loss, accuracy = gradient_descent_step()

            response = {'type': 'step', 'epoch': epoch, 'loss': loss, 'accuracy': accuracy, 'parameters': serialize_parameters(parameters)}
            emit('message_from_server', response)

            if epoch % 100 == 0:
                losses.append(loss)
                accuracies.append(accuracy)
        
                # Save losses and accuracies to a csv file
                save_results()

        else:
            response = {'type': 'finish'}
            emit('message_from_server', response)

def gradient_descent_step():
    global epoch
    global parameters

    # Update pamaraters with gradient descent
    parameters, loss = learn.gradient_descent_step(X_train, y_train, parameters, learning_rate)
    
    # Compute accuracy
    y_hat = learn.forward(X_test, parameters)[-1]
    accuracy = learn.accuracy(y_test, y_hat)
    
    # Print results
    print(f"Epoch: {epoch}, Loss: {loss}, Accuracy: {accuracy}")

    # Increses the epoch
    epoch += 1

    return loss, accuracy

def serialize_parameters(parameters):
    return [p.tolist() for p in parameters]

def save_mnist_image(X, index, filename):
    """
    Save a plot of a given example of the MNIST dataset.

    Parameters:
    - X: np.array, the dataset containing the MNIST images.
    - index: int, the index of the example to plot.
    - filename: str, the filename to save the plot.
    """
    image = X[index].reshape(28, 28)
    plt.figure(figsize=(4, 4))
    plt.imshow(image, cmap='gray')
    plt.axis('off')
    plt.title(f'MNIST Digit at Index {index}')
    plt.savefig(filename)
    plt.close()

if __name__ == '__main__':
    # Use the SSL context for HTTPS
    socketio.run(app, host="0.0.0.0", port="5001", debug=True)

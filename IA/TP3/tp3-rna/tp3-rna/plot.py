import matplotlib.pyplot as plt
import numpy as np

def load_data(filename):
    """Load the data from the CSV file.

    Args:
        filename (str): Name of the CSV file to load.

    Returns:
        data (np.ndarray): Data loaded from the CSV file.
    """
    # Load the data from the CSV file
    data = []
    with open(filename, 'r') as file:
        for line in file:
            data.append([float(x) for x in line.strip().split(',')])

    # Convert data to a NumPy array for easy manipulation
    data_array = np.array(data)

    return data_array

def plot_curve(data, color, linestyle, filename, xlabel, ylabel):
    """Plot the curve with the specified color and linestyle.

    Args:
        data (np.ndarray): Data to plot.
        color (str): Color of the curve.
        linestyle (str): Linestyle of the curve.
        filename (str): Name of the file to save the plot.
    """
    # Create a new figure
    plt.figure()

    # Plot the data with the specified color and linestyle
    plt.plot(data, color=color, linestyle=linestyle)

    # Add a grid with dashed lines
    plt.grid(True, linestyle='--')

    # Add titles and labels for clarity
    # plt.title('Averages of Each Experiment')
    plt.xlabel(xlabel)
    plt.ylabel(ylabel)

    # Save the plot as a PNG file
    plt.savefig(filename)

# Load losses data from csv
losses = load_data('model/losses.csv')

# Load accuracies data from csv
accuracies = load_data('model/accuracies.csv')

# Plot the losses curve
plot_curve(losses, 'red', 'solid', 'losses.png', 'Número de Épocas (x100)', 'Perda')

# Plot the accuracies curve
plot_curve(accuracies, 'blue', 'solid', 'accuracies.png', 'Número de Épocas (x100)', 'Acurácia')

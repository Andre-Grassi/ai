import numpy as np
import matplotlib.pyplot as plt

MAX_CAPTURED_DOGS = 14
MAX_JAGUAR_MOBILITY = 8


def plot_jaguar_score(cw, mw):
    max_score = (MAX_CAPTURED_DOGS * cw) + (MAX_JAGUAR_MOBILITY * mw)

    # 1. Define ranges for your variables
    # captured_dogs: 0 to 14
    dogs = np.linspace(0, 14, 100)
    # jaguar_mobility: 0 to 8
    mobility = np.linspace(0, 8, 100)

    # 2. Create a grid of values
    X, Y = np.meshgrid(dogs, mobility)

    # 3. Apply your formula
    # X = captured_dogs, Y = jaguar_mobility
    Z = 1 - 2 * ((X * cw + Y * mw) / max_score)

    # 4. Plot
    plt.figure(figsize=(10, 8))
    # Create a filled contour plot (Heatmap style)
    cp = plt.contourf(X, Y, Z, 20, cmap="viridis")
    plt.colorbar(cp, label="Resulting Score")

    plt.title(f"Score Visualization\ncw={cw}, mw={mw}, max={max_score}")
    plt.xlabel("Captured Dogs")
    plt.ylabel("Jaguar Mobility")
    plt.show()


# Example usage with constants
plot_jaguar_score(cw=5.0, mw=1.0)

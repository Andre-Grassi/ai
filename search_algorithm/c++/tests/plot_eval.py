import numpy as np
import matplotlib.pyplot as plt
from matplotlib.widgets import Slider

MAX_CAPTURED_DOGS = 5
MAX_JAGUAR_MOBILITY = 8


def calculate_max_score(cw, mw):
    return (MAX_CAPTURED_DOGS * cw) + (MAX_JAGUAR_MOBILITY * mw)


def calculate_score(cw, mw):
    return 1 - 2 * ((X * cw + Y * mw) / max_jaguar_score)


# 1. Setup Data
dogs = np.linspace(0, MAX_CAPTURED_DOGS, 100)  # Range for captured_dogs
mobility = np.linspace(0, MAX_JAGUAR_MOBILITY, 100)  # Range for jaguar_mobility
X, Y = np.meshgrid(dogs, mobility)

# Initial values
init_cw = 5.0
init_mw = 1.0
max_jaguar_score = calculate_max_score(init_cw, init_mw)


# 2. Create the Figure and Initial Plot
fig, ax = plt.subplots(figsize=(10, 8))
plt.subplots_adjust(bottom=0.25)  # Make room for sliders at the bottom

Z = calculate_score(init_cw, init_mw)
# We use contourf for the heatmap
contour = ax.contourf(X, Y, Z, 20, cmap="viridis")
cbar = fig.colorbar(contour, ax=ax)
cbar.set_label("Score (1 to -1)")

ax.set_xlabel("Captured Dogs")
ax.set_ylabel("Jaguar Mobility")
ax.set_title(f"Score Visualization\ncw={init_cw}, mw={init_mw}")

# 3. Add Sliders
# Define axes for the sliders [left, bottom, width, height]
ax_cw = plt.axes([0.25, 0.1, 0.65, 0.03])
ax_mw = plt.axes([0.25, 0.05, 0.65, 0.03])

slider_cw = Slider(ax_cw, "Weight (cw)", 0.0, 20.0, valinit=init_cw)
slider_mw = Slider(ax_mw, "Mobility (mw)", 0.0, 10.0, valinit=init_mw)


# 4. Define Update Function
def update(val):
    cw = slider_cw.val
    mw = slider_mw.val

    # Recalculate Z
    new_Z = calculate_score(cw, mw)

    # Clear and redraw the plot (fastest way for contour plots)
    ax.clear()
    ax.contourf(X, Y, new_Z, 20, cmap="viridis")
    ax.set_xlabel("Captured Dogs")
    ax.set_ylabel("Jaguar Mobility")
    ax.set_title(f"Score Visualization\ncw={cw:.2f}, mw={mw:.2f}")

    # Redraw the canvas
    fig.canvas.draw_idle()


# Attach the update function to the sliders
slider_cw.on_changed(update)
slider_mw.on_changed(update)

plt.show()

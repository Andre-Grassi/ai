import numpy as np
import matplotlib.pyplot as plt
from matplotlib.widgets import Slider

MAX_CAPTURED_DOGS = 5
MAX_JAGUAR_MOBILITY = 8


def calculate_max_score(cw, mw):
    return (MAX_CAPTURED_DOGS * cw) + (MAX_JAGUAR_MOBILITY * mw)


# MODIFIED: Now accepts current_max as an argument
def calculate_score(cw, mw, current_max):
    # Avoid division by zero if sliders are both 0
    if current_max == 0:
        return np.ones_like(X)
    return 1 - 2 * ((X * cw + Y * mw) / current_max)


# 1. Setup Data
dogs = np.linspace(0, MAX_CAPTURED_DOGS, 100)
mobility = np.linspace(0, MAX_JAGUAR_MOBILITY, 100)
X, Y = np.meshgrid(dogs, mobility)

# Initial values
init_cw = 5.0
init_mw = 1.0
init_max = calculate_max_score(init_cw, init_mw)

# 2. Create the Figure
fig, ax = plt.subplots(figsize=(10, 8))
plt.subplots_adjust(bottom=0.25)

Z = calculate_score(init_cw, init_mw, init_max)

# Plot
# We lock vmin and vmax to -1 and 1 so the colors always mean the same thing
mesh = ax.pcolormesh(X, Y, Z, shading="gouraud", cmap="viridis", vmin=-1, vmax=1)
cbar = fig.colorbar(mesh, ax=ax)
cbar.set_label("Score")

ax.set_xlabel("Captured Dogs")
ax.set_ylabel("Jaguar Mobility")
ax.set_title(f"Score Visualization\ncw={init_cw}, mw={init_mw}")

# 3. Add Sliders
ax_cw = plt.axes([0.25, 0.1, 0.65, 0.03])
ax_mw = plt.axes([0.25, 0.05, 0.65, 0.03])

slider_cw = Slider(ax_cw, "Weight (cw)", 0.0, 100.0, valinit=init_cw)
slider_mw = Slider(ax_mw, "Mobility (mw)", 0.0, 100.0, valinit=init_mw)


# 4. Define Update Function
def update(val):
    cw = slider_cw.val
    mw = slider_mw.val

    # Recalculate the max score based on NEW slider values
    current_max = calculate_max_score(cw, mw)

    # Pass the new max to the score calculation
    new_Z = calculate_score(cw, mw, current_max)

    mesh.set_array(new_Z.ravel())

    ax.set_title(f"Score Visualization\ncw={cw:.2f}, mw={mw:.2f}")
    fig.canvas.draw_idle()


slider_cw.on_changed(update)
slider_mw.on_changed(update)

plt.show()

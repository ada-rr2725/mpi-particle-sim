import glob
import os
import re
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation

DATA_DIR = os.path.dirname(os.path.abspath(__file__))
X_MAX    = 5.0
Y_MAX    = 5.0

# data loading functions

def discover_runs(data_dir):
    files = glob.glob(os.path.join(data_dir, "particles_p*_t*.dat"))
    procs, steps = set(), set()
    for f in files:
        m = re.search(r"particles_p(\d+)_t(\d+)\.dat", f)
        if m:
            procs.add(int(m.group(1)))
            steps.add(int(m.group(2)))
    return sorted(procs), sorted(steps)

def load_step(data_dir, procs, step):
    """Return arrays (x, y, rank) for all particles at a given timestep."""
    xs, ys, ranks = [], [], []
    for pid in procs:
        path = os.path.join(data_dir, f"particles_p{pid}_t{step}.dat")
        if not os.path.exists(path):
            continue
        data = np.loadtxt(path)
        if data.ndim == 1:
            data = data[np.newaxis, :]
        if data.size == 0:
            continue
        xs.append(data[:, 0])
        ys.append(data[:, 1])
        ranks.append(np.full(len(data), pid))
    if not xs:
        return np.array([]), np.array([]), np.array([])
    return np.concatenate(xs), np.concatenate(ys), np.concatenate(ranks)

# build colour map and discover available data

procs, steps = discover_runs(DATA_DIR)
n_procs = len(procs)

cmap   = plt.cm.get_cmap("tab10", n_procs)
colors = {pid: cmap(i) for i, pid in enumerate(procs)}

# setup the figure and axes

fig = plt.figure(figsize=(9, 6))
ax  = fig.add_axes([0.08, 0.08, 0.62, 0.88])   # [left, bottom, width, height]

ax.set_xlim(0, X_MAX)
ax.set_ylim(0, Y_MAX)
ax.set_xlabel("x")
ax.set_ylabel("y")
ax.set_aspect("equal")
ax.set_title("Particle simulation")

# vertical domain boundaries for each MPI rank, shown as dashed lines
stripe_w = X_MAX / n_procs
for i in range(1, n_procs):
    ax.axvline(i * stripe_w, color="grey", linewidth=0.8, linestyle="--", alpha=0.4)

# one scatter plot per MPI rank, initially empty
scatters = {}
for pid in procs:
    sc = ax.scatter([], [], s=2, color=colors[pid],
                    label=f"rank {pid}", rasterized=True)
    scatters[pid] = sc

# legend in the right part of the figure
legend_ax = fig.add_axes([0.73, 0.08, 0.24, 0.88])
legend_ax.axis("off")
handles = [plt.Line2D([0], [0], marker="o", color="w",
                      markerfacecolor=colors[pid], markersize=8,
                      label=f"rank {pid}") for pid in procs]
legend_ax.legend(handles=handles, loc="upper left",
                 title="MPI rank", title_fontsize=9,
                 fontsize=9, frameon=True)

# timestep counter — large text in the bottom-right corner of the plot
counter = ax.text(0.98, 0.03, "", transform=ax.transAxes,
                  ha="right", va="bottom", fontsize=13,
                  fontweight="bold",
                  bbox=dict(boxstyle="round,pad=0.3", fc="white", alpha=0.7))

# particle counter — smaller text above the timestep counter
pcount = ax.text(0.98, 0.10, "", transform=ax.transAxes,
                 ha="right", va="bottom", fontsize=9, color="dimgrey")

# animation update

def update(frame_idx):
    step = steps[frame_idx]
    x, y, rank = load_step(DATA_DIR, procs, step)

    for pid in procs:
        mask = rank == pid
        if mask.any():
            scatters[pid].set_offsets(np.column_stack([x[mask], y[mask]]))
        else:
            scatters[pid].set_offsets(np.empty((0, 2)))

    counter.set_text(f"step {step}")
    pcount.set_text(f"{len(x):,} particles")
    return list(scatters.values()) + [counter, pcount]

ani = animation.FuncAnimation(
    fig,
    update,
    frames=len(steps),
    interval=500,
    blit=True,
)

plt.show()

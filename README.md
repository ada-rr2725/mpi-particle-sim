# mpi-particle-sim

![demo](demo.gif)

MPI-decomposed 2D particle simulation with gravity, elastic wall bouncing, and per-timestep rank migration via MPI_Alltoall.

## build

```bash
make
```

or manually:

```bash
mpicxx -O2 -o mpi-particle-sim main.cpp particle.cpp domain.cpp
```

## run

```bash
mpiexec -n 4 ./mpi-particle-sim
```

use `--oversubscribe` if running more ranks than physical cores:

```bash
mpiexec -n 8 --oversubscribe ./mpi-particle-sim
```

## visualise

requires python with numpy and matplotlib:

```bash
python3 visualise.py
```

particles are coloured by owning rank. dashed vertical lines show domain boundaries. auto-detects number of processes and timesteps from output files.

## parameters

| parameter | default | description |
|---|---|---|
| `x_max` | 5.0 | domain width |
| `y_max` | 5.0 | domain height |
| `gravity` | 9.81 | gravitational acceleration (m/s^2) |
| `max_particles` | 10000 | total particle count |
| `dt` | 0.005 | timestep size |
| `num_steps` | 2000 | number of simulation steps |
| `write_interval` | 50 | steps between file output |

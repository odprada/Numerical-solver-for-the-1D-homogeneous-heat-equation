# Parallel numerical solver for the 1D heat equation (C, MPI)

Parallel solver for the one-dimensional homogeneous heat equation

$$\frac{\partial u}{\partial t} = k\frac{\partial^2 u}{\partial x^2}, \qquad x \in [0, 1]$$

with initial condition `u(x, 0) = u0 = 1` and fixed zero Dirichlet boundary conditions `u(0, t) = u(1, t) = 0`.

## Method

**Numerical scheme.** Explicit finite differences, forward in time and centered in space (FTCS):

```
u_i^{n+1} = u_i^n + (k * tau / h^2) * (u_{i+1}^n - 2 u_i^n + u_{i-1}^n)
```

- Accuracy: O(tau + h^2)
- Stability (Courant condition): `k * tau / h^2 <= 1/2`

**Parallelization (geometric parallelism).**

- The rod is split into contiguous blocks of points, one block per MPI process. There is no central process: every rank computes its own block boundaries from `N`, `size` and `myrank`.
- If `N` is not divisible by `p`, the remainder `N mod p` is distributed one extra point each to the first ranks, so block sizes differ by at most one.
- Each rank stores its block plus two ghost cells, which hold the neighbours' edge values.
- Ghost cells are refreshed every time step by a blocking odd/even exchange (4 phases):
  1. odd ranks send left, even ranks receive from the right
  2. odd ranks receive from the left, even ranks send to the right
  3. odd ranks receive from the right, even ranks send to the left
  4. odd ranks send to the right, even ranks receive from the left

  Every send has a matching receive at the same moment, so it cannot deadlock, and the communication time per step does not grow with `p`.
- Two arrays (current and new time level) are swapped by pointer after each step, so there is no copy.
- The same code handles `p = 1` with no messages sent.

## Validation

The numerical solution is compared with the exact Fourier-series solution

$$u(x,t) = \sum_{m=0}^{\infty} \frac{4}{(2m+1)\pi}\,\sin\big((2m+1)\pi x\big)\,e^{-k(2m+1)^2\pi^2 t}$$

at the 11 points `x = 0, 0.1, ..., 1.0` at `T = 0.1` (with `k = 1`, `h = 0.02`, `tau = 0.0002`).

- Maximum absolute error is about 9e-4 (values up to about 0.47 at the centre).
- Results for `p = 1, 2, 3` are identical.
- The solution is symmetric about `x = 0.5`, and the end points are exactly 0.

## Test Environment

- **CPU:** Dual-socket Intel Xeon 5160 @ 3.00 GHz (2 cores per socket, 4 physical cores per node, no hyper-threading)
- **Cache:** 32 KiB L1d / 32 KiB L1i per core; 4 MiB L2 per socket (8 MiB total per node)
- **Cluster Configuration:** 3 compute nodes (12 cores total), Slurm workload manager
- **Compilation:** First run: `mpicc solver.c -lm` (N = 2000, 10000, 50000). Second run: `mpicc -O3  -march=core2 solver.c -lm` (N = 2000, 10000, 50000, 250000, 1000000)

## Conclusions

1. **Accuracy and Stability.** As long as the Courant stability condition (`k * tau / h^2 <= 1/2`) holds, the numerical solver closely matches the exact analytical solution (within ~`9e-4` at `T = 0.1`) and gives identical results regardless of how many processes (`p = 1, 2, 3`) are used.

2. **Single-Node vs. Multi-Node Scaling (`p <= 4` vs. `p >= 5`).** Each compute node has 4 physical CPU cores (dual-socket Intel Xeon 5160). Up to `p = 4`, all processes stay on one machine and communicate through fast shared memory, giving strong speedup on larger grids (`N = 50,000`). Once `p >= 5`, MPI spreads processes across multiple nodes over the network, adding communication latency at every time step that noticeably slows down smaller runs (`N = 2,000`).

3. **Impact of Grid Size (`N`) and Complexity.** Because the explicit stability criterion forces `tau = O(1 / N^2)`, the number of time steps grows as `O(N^2)` and total grid updates scale as `O(N^3)`, while each process still only exchanges two numbers (`double`) per step (`O(1)` per step). Moving from `N = 2,000` to `N = 50,000` gives each core vastly more computation relative to communication overhead, resulting in much better parallel efficiency.

4. **Naive (`O(p)`) vs. Odd-Even (`O(1)`) Exchange.** Pairing odd and even processes guarantees there are no deadlocks and keeps communication time per step constant (`O(1)`) as `p` grows. For small 8-byte messages (`double`), standard `MPI_Send` can partially hide the `O(p)` slowdown of the naive approach because MPI buffers tiny messages internally.

## Notes

1. To guarantee reasonable computing times, T was reduced every time N was increased.

2. During the second run, for simplicity, `p = 1` was only run once using the optimized communication mode, hence the point (1,1) is missing from the naive communication speedup graph.

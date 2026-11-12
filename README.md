# Parallel numerical solver for the 1D heat equation (C, MPI)

Parallel solver for the one-dimensional homogeneous heat equation

$$\frac{\partial u}{\partial t} = k\,\frac{\partial^2 u}{\partial x^2}, \qquad x \in [0, 1]$$

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
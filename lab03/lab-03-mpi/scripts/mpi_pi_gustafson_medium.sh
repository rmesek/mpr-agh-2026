#!/bin/bash
# Weak Scaling (Gustafson's Law)
# Medium Problem Size (1e+08 per proc.)
# Est. Time: 0m 18s

mpiexec -np 1 ./mpi_pi_benchmark 1.000000e+08
mpiexec -np 2 ./mpi_pi_benchmark 1.000000e+08 --no-header
mpiexec -np 3 ./mpi_pi_benchmark 1.000000e+08 --no-header
mpiexec -np 4 ./mpi_pi_benchmark 1.000000e+08 --no-header
mpiexec -np 5 ./mpi_pi_benchmark 1.000000e+08 --no-header
mpiexec -np 6 ./mpi_pi_benchmark 1.000000e+08 --no-header
mpiexec -np 7 ./mpi_pi_benchmark 1.000000e+08 --no-header
mpiexec -np 8 ./mpi_pi_benchmark 1.000000e+08 --no-header
mpiexec -np 9 ./mpi_pi_benchmark 1.000000e+08 --no-header
mpiexec -np 10 ./mpi_pi_benchmark 1.000000e+08 --no-header
mpiexec -np 11 ./mpi_pi_benchmark 1.000000e+08 --no-header
mpiexec -np 12 ./mpi_pi_benchmark 1.000000e+08 --no-header

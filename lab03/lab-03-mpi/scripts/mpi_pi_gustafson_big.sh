#!/bin/bash
# Weak Scaling (Gustafson's Law)
# Big Problem Size (1e+10 per proc.)
# Est. Time: 30m 0s

mpiexec -np 1 ./mpi_pi_benchmark 1.000000e+10
mpiexec -np 2 ./mpi_pi_benchmark 1.000000e+10 --no-header
mpiexec -np 3 ./mpi_pi_benchmark 1.000000e+10 --no-header
mpiexec -np 4 ./mpi_pi_benchmark 1.000000e+10 --no-header
mpiexec -np 5 ./mpi_pi_benchmark 1.000000e+10 --no-header
mpiexec -np 6 ./mpi_pi_benchmark 1.000000e+10 --no-header
mpiexec -np 7 ./mpi_pi_benchmark 1.000000e+10 --no-header
mpiexec -np 8 ./mpi_pi_benchmark 1.000000e+10 --no-header
mpiexec -np 9 ./mpi_pi_benchmark 1.000000e+10 --no-header
mpiexec -np 10 ./mpi_pi_benchmark 1.000000e+10 --no-header
mpiexec -np 11 ./mpi_pi_benchmark 1.000000e+10 --no-header
mpiexec -np 12 ./mpi_pi_benchmark 1.000000e+10 --no-header

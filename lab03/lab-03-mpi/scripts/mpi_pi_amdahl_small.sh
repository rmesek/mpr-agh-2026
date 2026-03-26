#!/bin/bash
# Strong Scaling (Amdahl's Law)
# Small Problem Size (1.2e+07 const.)
# Est. Time: 0m 0s

mpiexec -np 1 ./mpi_pi_benchmark 1.200000e+07
mpiexec -np 2 ./mpi_pi_benchmark 6.000000e+06 --no-header
mpiexec -np 3 ./mpi_pi_benchmark 4.000000e+06 --no-header
mpiexec -np 4 ./mpi_pi_benchmark 3.000000e+06 --no-header
mpiexec -np 5 ./mpi_pi_benchmark 2.400000e+06 --no-header
mpiexec -np 6 ./mpi_pi_benchmark 2.000000e+06 --no-header
mpiexec -np 7 ./mpi_pi_benchmark 1.714286e+06 --no-header
mpiexec -np 8 ./mpi_pi_benchmark 1.500000e+06 --no-header
mpiexec -np 9 ./mpi_pi_benchmark 1.333333e+06 --no-header
mpiexec -np 10 ./mpi_pi_benchmark 1.200000e+06 --no-header
mpiexec -np 11 ./mpi_pi_benchmark 1.090909e+06 --no-header
mpiexec -np 12 ./mpi_pi_benchmark 1.000000e+06 --no-header

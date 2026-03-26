#!/bin/bash
#SBATCH --job-name=mpi_pi_amdahl_big
#SBATCH --output=results/mpi_pi_amdahl_big_%A_%a.out
#SBATCH --nodes=1
#SBATCH --ntasks=12
#SBATCH --time=02:00:00
#SBATCH --partition=plgrid
#SBATCH --account=plgmpr26-cpu
#SBATCH --array=1-10

ml gcc/13.2.0 openmpi/4.1.6-gcc-13.2.0

mpicc -O3 -o ./mpi_pi_benchmark mpi_pi_benchmark.c

mkdir -p results

bash ./scripts/mpi_pi_amdahl_big.sh > results/raw_mpi_pi_amdahl_big_${SLURM_ARRAY_TASK_ID}.out
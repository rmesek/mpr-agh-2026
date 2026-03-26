#!/bin/bash
#SBATCH --job-name=mpi_pi_small
#SBATCH --output=results/mpi_pi_gustafson_small_%A_%a.out
#SBATCH --nodes=1
#SBATCH --ntasks=12
#SBATCH --time=00:01:00
#SBATCH --partition=plgrid
#SBATCH --account=plgmpr26-cpu
#SBATCH --array=1-10

ml gcc/13.2.0 openmpi/4.1.6-gcc-13.2.0

mkdir -p results

mpicc -O3 -o $SCRATCH/mpi_pi_benchmark_gustafson_small_${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID} mpi_pi_benchmark.c

# Weak Scaling (Gustafson's Law)
# Small Problem Size (100000 per proc.)
# Est. Time: 0m 0s

mpiexec -np 1 $SCRATCH/mpi_pi_benchmark_gustafson_small_${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID} 1.000000e+05 > results/raw_mpi_pi_gustafson_small_${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID}.out
mpiexec -np 2 $SCRATCH/mpi_pi_benchmark_gustafson_small_${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID} 1.000000e+05 --no-header >> results/raw_mpi_pi_gustafson_small_${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID}.out
mpiexec -np 3 $SCRATCH/mpi_pi_benchmark_gustafson_small_${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID} 1.000000e+05 --no-header >> results/raw_mpi_pi_gustafson_small_${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID}.out
mpiexec -np 4 $SCRATCH/mpi_pi_benchmark_gustafson_small_${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID} 1.000000e+05 --no-header >> results/raw_mpi_pi_gustafson_small_${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID}.out
mpiexec -np 5 $SCRATCH/mpi_pi_benchmark_gustafson_small_${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID} 1.000000e+05 --no-header >> results/raw_mpi_pi_gustafson_small_${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID}.out
mpiexec -np 6 $SCRATCH/mpi_pi_benchmark_gustafson_small_${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID} 1.000000e+05 --no-header >> results/raw_mpi_pi_gustafson_small_${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID}.out
mpiexec -np 7 $SCRATCH/mpi_pi_benchmark_gustafson_small_${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID} 1.000000e+05 --no-header >> results/raw_mpi_pi_gustafson_small_${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID}.out
mpiexec -np 8 $SCRATCH/mpi_pi_benchmark_gustafson_small_${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID} 1.000000e+05 --no-header >> results/raw_mpi_pi_gustafson_small_${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID}.out
mpiexec -np 9 $SCRATCH/mpi_pi_benchmark_gustafson_small_${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID} 1.000000e+05 --no-header >> results/raw_mpi_pi_gustafson_small_${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID}.out
mpiexec -np 10 $SCRATCH/mpi_pi_benchmark_gustafson_small_${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID} 1.000000e+05 --no-header >> results/raw_mpi_pi_gustafson_small_${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID}.out
mpiexec -np 11 $SCRATCH/mpi_pi_benchmark_gustafson_small_${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID} 1.000000e+05 --no-header >> results/raw_mpi_pi_gustafson_small_${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID}.out
mpiexec -np 12 $SCRATCH/mpi_pi_benchmark_gustafson_small_${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID} 1.000000e+05 --no-header >> results/raw_mpi_pi_gustafson_small_${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID}.out

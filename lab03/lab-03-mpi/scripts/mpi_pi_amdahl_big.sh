#!/bin/bash
#SBATCH --job-name=mpi_pi_big
#SBATCH --output=results/mpi_pi_amdahl_big_%A_%a.out
#SBATCH --nodes=1
#SBATCH --ntasks=12
#SBATCH --time=00:23:53
#SBATCH --partition=plgrid
#SBATCH --account=plgmpr26-cpu
#SBATCH --array=1-10

ml gcc/13.2.0 openmpi/4.1.6-gcc-13.2.0

mkdir -p results

mpicc -O3 -o $SCRATCH/mpi_pi_benchmark_amdahl_big_${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID} mpi_pi_benchmark.c

# Strong Scaling (Amdahl's Law)
# Big Problem Size (1.2e+10 const.)
# Est. Time: 21m 43s

mpiexec -np 1 $SCRATCH/mpi_pi_benchmark_amdahl_big_${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID} 1.200000e+10 > results/raw_mpi_pi_amdahl_big_${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID}.out
mpiexec -np 2 $SCRATCH/mpi_pi_benchmark_amdahl_big_${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID} 6.000000e+09 --no-header >> results/raw_mpi_pi_amdahl_big_${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID}.out
mpiexec -np 3 $SCRATCH/mpi_pi_benchmark_amdahl_big_${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID} 4.000000e+09 --no-header >> results/raw_mpi_pi_amdahl_big_${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID}.out
mpiexec -np 4 $SCRATCH/mpi_pi_benchmark_amdahl_big_${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID} 3.000000e+09 --no-header >> results/raw_mpi_pi_amdahl_big_${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID}.out
mpiexec -np 5 $SCRATCH/mpi_pi_benchmark_amdahl_big_${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID} 2.400000e+09 --no-header >> results/raw_mpi_pi_amdahl_big_${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID}.out
mpiexec -np 6 $SCRATCH/mpi_pi_benchmark_amdahl_big_${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID} 2.000000e+09 --no-header >> results/raw_mpi_pi_amdahl_big_${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID}.out
mpiexec -np 7 $SCRATCH/mpi_pi_benchmark_amdahl_big_${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID} 1.714286e+09 --no-header >> results/raw_mpi_pi_amdahl_big_${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID}.out
mpiexec -np 8 $SCRATCH/mpi_pi_benchmark_amdahl_big_${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID} 1.500000e+09 --no-header >> results/raw_mpi_pi_amdahl_big_${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID}.out
mpiexec -np 9 $SCRATCH/mpi_pi_benchmark_amdahl_big_${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID} 1.333333e+09 --no-header >> results/raw_mpi_pi_amdahl_big_${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID}.out
mpiexec -np 10 $SCRATCH/mpi_pi_benchmark_amdahl_big_${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID} 1.200000e+09 --no-header >> results/raw_mpi_pi_amdahl_big_${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID}.out
mpiexec -np 11 $SCRATCH/mpi_pi_benchmark_amdahl_big_${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID} 1.090909e+09 --no-header >> results/raw_mpi_pi_amdahl_big_${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID}.out
mpiexec -np 12 $SCRATCH/mpi_pi_benchmark_amdahl_big_${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID} 1.000000e+09 --no-header >> results/raw_mpi_pi_amdahl_big_${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID}.out

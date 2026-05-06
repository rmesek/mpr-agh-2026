#!/bin/bash
#SBATCH --job-name=omp_bucketsort
#SBATCH --output=logs/omp_bucketsort_%A_%a.out
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=32
#SBATCH --time=02:24:00
#SBATCH --partition=plgrid
#SBATCH --account=plgmpr26-cpu
#SBATCH --array=1-10

ml gcc/14.3.0

mkdir -p results logs

gcc -lm -fopenmp omp_bucketsort_v3.c -o $SCRATCH/omp_bucketsort_${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID}

for max_threads in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32; do
    for array_size in 1000 300000 100000000; do
        for num_buckets in 100 10000 1000000; do
            OMP_NUM_THREADS=$max_threads $SCRATCH/omp_bucketsort_${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID} $SLURM_ARRAY_TASK_ID $array_size $num_buckets > results/omp_bucketsort_${max_threads}_${array_size}_${num_buckets}_${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID}.out
        done
    done
done

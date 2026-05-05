from pathlib import Path

DIR_NAME = "scripts"

PARAMS = {
    "seed": "$SLURM_ARRAY_TASK_ID",
    "max_threads": list(range(1, 33)),
    "array_size": [1_000, 300_000, 100_000_000],
    "num_buckets": [100, 10_000, 1_000_000],
}

EST_TIME = (
    len(PARAMS["max_threads"])
    * len(PARAMS["array_size"])
    * len(PARAMS["num_buckets"])
    * 30  # estimated worst-case
)  # [s]

HEADER_STR = f"""\
#!/bin/bash
#SBATCH --job-name=omp_bucketsort
#SBATCH --output=logs/omp_bucketsort_%A_%a.out
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task={max(PARAMS["max_threads"])}
#SBATCH --time={int(EST_TIME // 3600):02d}:{int((EST_TIME % 3600) // 60):02d}:{int(EST_TIME % 60):02d}
#SBATCH --partition=plgrid
#SBATCH --account=plgmpr26-cpu
#SBATCH --array=1-10
"""

SETUP_STR = """\
ml gcc/14.3.0

mkdir -p results logs

gcc -lm -fopenmp omp_bucketsort.c -o $SCRATCH/omp_bucketsort_${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID}
"""

# OMP_NUM_THREADS=4 $SCRATCH/omp_bucketsort 42 100000000 100
RUN_LOOP_STR = f"""\
for max_threads in {" ".join(map(str, PARAMS["max_threads"]))}; do
    for array_size in {" ".join(map(str, PARAMS["array_size"]))}; do
        for num_buckets in {" ".join(map(str, PARAMS["num_buckets"]))}; do
            OMP_NUM_THREADS=$max_threads $SCRATCH/omp_bucketsort_${{SLURM_ARRAY_JOB_ID}}_${{SLURM_ARRAY_TASK_ID}} {PARAMS["seed"]} $array_size $num_buckets > results/omp_bucketsort_${{max_threads}}_${{array_size}}_${{num_buckets}}_${{SLURM_ARRAY_JOB_ID}}_${{SLURM_ARRAY_TASK_ID}}.out
        done
    done
done
"""


def write_cmd(f):
    f.write(HEADER_STR)
    f.write("\n")
    f.write(SETUP_STR)
    f.write("\n")
    f.write(RUN_LOOP_STR)


if __name__ == "__main__":
    Path(DIR_NAME).mkdir(exist_ok=True)
    with open(Path(DIR_NAME) / "omp_bucketsort.sh", "w") as f:
        write_cmd(f)

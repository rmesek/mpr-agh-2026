from pathlib import Path


DIR_NAME = "scripts"
SIZES = {
    "SMALL": {"N_POINTS": 1e5, "EST_TIME": 0.0035},
    "MEDIUM": {"N_POINTS": 1e7, "EST_TIME": 0.35},
    "BIG": {"N_POINTS": 1e9, "EST_TIME": 35.0},
}  # EST_TIME [s] for N_POINTS * max(PROCESSORS) on 12 processors
PROCESSORS = range(1, 13)


def write_header(f, scaling, size, est_time):
    est_time = max(1.1 * est_time, 60)
    time_str = f"{int(est_time // 3600):02d}:{int((est_time % 3600) // 60):02d}:{int(est_time % 60):02d}"

    f.write("#!/bin/bash\n")
    f.write(f"#SBATCH --job-name=mpi_pi_{size.lower()}\n")
    f.write(f"#SBATCH --output=results/mpi_pi_{scaling}_{size.lower()}_%A_%a.out\n")
    f.write("#SBATCH --nodes=1\n")
    f.write(f"#SBATCH --ntasks={max(PROCESSORS)}\n")
    f.write(f"#SBATCH --time={time_str}\n")
    f.write("#SBATCH --partition=plgrid\n")
    f.write("#SBATCH --account=plgmpr26-cpu\n")
    f.write("#SBATCH --array=1-10\n\n")

    f.write("ml gcc/13.2.0 openmpi/4.1.6-gcc-13.2.0\n\n")

    f.write("mkdir -p results\n\n")

    exe_path = (
        f"$SCRATCH/mpi_pi_benchmark_{scaling}_{size.lower()}_"
        "${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID}"
    )
    f.write(f"mpicc -O3 -o {exe_path} mpi_pi_benchmark.c\n\n")

    return exe_path


def write_cmd(f, scaling, size, proc, n_points_per_proc, exe_path, include_header=True):
    header = "" if include_header else " --no-header"
    redirect = ">" if include_header else ">>"
    cmd = (
        f"mpiexec -np {proc} {exe_path} {n_points_per_proc:e}{header} "
        f"{redirect} results/raw_mpi_pi_{scaling}_{size.lower()}_"
        "${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID}.out\n"
    )
    f.write(cmd)


def write_amdahl(f, size):
    max_proc = max(PROCESSORS)
    n_points = SIZES[size]["N_POINTS"] * max_proc
    base_time = SIZES[size]["EST_TIME"]
    est_time = sum(base_time * max_proc / proc for proc in PROCESSORS)

    exe_path = write_header(f, "amdahl", size, est_time)
    f.write("# Strong Scaling (Amdahl's Law)\n")
    f.write(f"# {size.capitalize()} Problem Size ({n_points:g} const.)\n")
    f.write(f"# Est. Time: {int(est_time // 60)}m {int(est_time % 60)}s\n\n")

    for i, proc in enumerate(PROCESSORS):
        include_header = i == 0
        write_cmd(f, "amdahl", size, proc, n_points / proc, exe_path, include_header)


def write_gustafson(f, size):
    n_points = SIZES[size]["N_POINTS"]
    base_time = SIZES[size]["EST_TIME"]
    # For weak scaling, each run keeps work-per-process constant.
    est_time = sum(base_time for _ in PROCESSORS)

    exe_path = write_header(f, "gustafson", size, est_time)
    f.write("# Weak Scaling (Gustafson's Law)\n")
    f.write(f"# {size.capitalize()} Problem Size ({n_points:g} per proc.)\n")
    f.write(f"# Est. Time: {int(est_time // 60)}m {int(est_time % 60)}s\n\n")

    for i, proc in enumerate(PROCESSORS):
        include_header = i == 0
        write_cmd(f, "gustafson", size, proc, n_points, exe_path, include_header)


def main():
    Path(DIR_NAME).mkdir(exist_ok=True)

    for size in SIZES.keys():
        with open(Path(DIR_NAME) / f"mpi_pi_amdahl_{size.lower()}.sh", "w") as f:
            write_amdahl(f, size)

        with open(Path(DIR_NAME) / f"mpi_pi_gustafson_{size.lower()}.sh", "w") as f:
            write_gustafson(f, size)


if __name__ == "__main__":
    main()

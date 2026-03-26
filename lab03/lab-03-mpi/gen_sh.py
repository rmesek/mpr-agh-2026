from pathlib import Path


DIR_NAME = "scripts"
SIZES = {
    "SMALL": {"N_POINTS": 1e6, "EST_TIME": 0.015},
    "MEDIUM": {"N_POINTS": 1e8, "EST_TIME": 1.5},
    "BIG": {"N_POINTS": 1e10, "EST_TIME": 150.0},
}  # EST_TIME [s] for N_POINTS * max(PROCESSORS) on 12 processors
PROCESSORS = range(1, 13)


def write_cmd(f, proc, n_points_per_proc, include_header=True):
    header = "" if include_header else " --no-header"
    cmd = f"mpiexec -np {proc} ./mpi_pi_benchmark {n_points_per_proc:e}{header}\n"
    f.write(cmd)


def write_amdahl(f, size):
    max_proc = max(PROCESSORS)
    n_points = SIZES[size]["N_POINTS"] * max_proc
    base_time = SIZES[size]["EST_TIME"]
    est_time = sum(base_time * max_proc / proc for proc in PROCESSORS)

    f.write("#!/bin/bash\n")
    f.write("# Strong Scaling (Amdahl's Law)\n")
    f.write(f"# {size.capitalize()} Problem Size ({n_points:g} const.)\n")
    f.write(f"# Est. Time: {int(est_time // 60)}m {int(est_time % 60)}s\n\n")

    for i, proc in enumerate(PROCESSORS):
        include_header = i == 0
        write_cmd(f, proc, n_points / proc, include_header)


def write_gustafson(f, size):
    n_points = SIZES[size]["N_POINTS"]
    base_time = SIZES[size]["EST_TIME"]
    # For weak scaling, each run keeps work-per-process constant.
    est_time = sum(base_time for _ in PROCESSORS)

    f.write("#!/bin/bash\n")
    f.write("# Weak Scaling (Gustafson's Law)\n")
    f.write(f"# {size.capitalize()} Problem Size ({n_points:g} per proc.)\n")
    f.write(f"# Est. Time: {int(est_time // 60)}m {int(est_time % 60)}s\n\n")

    for i, proc in enumerate(PROCESSORS):
        include_header = i == 0
        write_cmd(f, proc, n_points, include_header)


def main():
    Path(DIR_NAME).mkdir(exist_ok=True)

    for size in SIZES.keys():
        with open(Path(DIR_NAME) / f"mpi_pi_amdahl_{size.lower()}.sh", "w") as f:
            write_amdahl(f, size)

        with open(Path(DIR_NAME) / f"mpi_pi_gustafson_{size.lower()}.sh", "w") as f:
            write_gustafson(f, size)


if __name__ == "__main__":
    main()

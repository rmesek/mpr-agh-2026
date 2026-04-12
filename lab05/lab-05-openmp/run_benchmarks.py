import os
import subprocess
import statistics
from pathlib import Path

CONFIGS = [
    {"array_size": 100, "chunk_size": 1, "max_threads": 12, "repetitions": 10},
    {"array_size": 100, "chunk_size": 8, "max_threads": 12, "repetitions": 10},
    {"array_size": 300_000, "chunk_size": 1, "max_threads": 12, "repetitions": 10},
    {"array_size": 300_000, "chunk_size": 25_000, "max_threads": 12, "repetitions": 10},
    {"array_size": 300_000, "chunk_size": 50_000, "max_threads": 12, "repetitions": 10},
    {
        "array_size": 1_000_000_000,
        "chunk_size": 1,
        "max_threads": 12,
        "repetitions": 10,
    },
    {
        "array_size": 1_000_000_000,
        "chunk_size": 41_666_666,
        "max_threads": 12,
        "repetitions": 10,
    },
    {
        "array_size": 1_000_000_000,
        "chunk_size": 83_333_333,
        "max_threads": 12,
        "repetitions": 10,
    },
    {
        "array_size": 1_000_000_000,
        "chunk_size": 166_666_666,
        "max_threads": 12,
        "repetitions": 10,
    },
]


def main():
    root = Path(__file__).parent
    src = root / "omp_random_benchmark.c"
    bin = root / "omp_random_benchmark"

    print(f">>> Compiling {src.name}")
    subprocess.run(["gcc", "-O3", "-fopenmp", str(src), "-o", str(bin)], check=True)

    for cfg in CONFIGS:
        array_size = cfg["array_size"]
        chunk_size = cfg["chunk_size"]
        max_threads = cfg["max_threads"]
        repetitions = cfg["repetitions"]

        print(
            "\n>>> Running: ",
            f"{array_size=:_}, ",
            f"{chunk_size=:_}, ",
            f"{max_threads=}, ",
            f"{repetitions=}",
        )

        results = {}
        for _ in range(repetitions):
            proc = subprocess.run(
                [str(bin), str(array_size), str(chunk_size)],
                capture_output=True,
                text=True,
                env={**os.environ, "OMP_NUM_THREADS": str(max_threads)},
                check=True,
            )

            # parse tab-separated output: "function_name\tVALUE ms"
            for line in proc.stdout.strip().splitlines():
                if "\t" in line and not line.startswith("function"):
                    f_name, val_str = line.split("\t")
                    val = float(val_str.replace(" ms", ""))
                    results.setdefault(f_name, []).append(val)

        # print statistics table
        print(
            f"{'function':<25} |",
            f"{'avg [ms]':>10} |",
            f"{'min [ms]':>10} |",
            f"{'max [ms]':>10} |",
            f"{'stddev [ms]':>10}",
        )
        print(
            f"{'-' * 25} |",
            f"{'-' * 10} |",
            f"{'-' * 10} |",
            f"{'-' * 10} |",
            f"{'-' * 10}",
        )

        for f_name, times in results.items():
            avg = statistics.mean(times)
            min_v = min(times)
            max_v = max(times)
            std = statistics.stdev(times) if len(times) > 1 else 0.0
            print(
                f"{f_name:<25} |",
                f"{avg:>10.6f} |",
                f"{min_v:>10.6f} |",
                f"{max_v:>10.6f} |",
                f"{std:>10.6f}",
            )


if __name__ == "__main__":
    main()

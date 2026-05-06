# /// script
# dependencies = [
#     "matplotlib",
#     "pandas",
# ]
# ///

from pathlib import Path
import pandas as pd
import matplotlib.pyplot as plt

OUTPUT_DIR = Path("plots")

RUN_REPEATS = 10
ARRAY_SIZE = 100_000_000
NUM_BUCKETS = 10_000

# Columns in df_v1: Index(['max_threads', 'array_size', 'num_buckets', 'job_id',
#        'omp_auto_xorshift32.min', 'omp_auto_xorshift32.max',
#        'omp_auto_xorshift32.avg', 'omp_auto_xorshift32.stddev',
#        'bucketsort_v1.init.min', 'bucketsort_v1.init.max',
#        'bucketsort_v1.init.avg', 'bucketsort_v1.init.stddev',
#        'bucketsort_v1.distribution.min', 'bucketsort_v1.distribution.max',
#        'bucketsort_v1.distribution.avg', 'bucketsort_v1.distribution.stddev',
#        'bucketsort_v1.calc_offsets.min', 'bucketsort_v1.calc_offsets.max',
#        'bucketsort_v1.calc_offsets.avg', 'bucketsort_v1.calc_offsets.stddev',
#        'bucketsort_v1.sort.min', 'bucketsort_v1.sort.max',
#        'bucketsort_v1.sort.avg', 'bucketsort_v1.sort.stddev',
#        'bucketsort_v1.copy_and_merge.min', 'bucketsort_v1.copy_and_merge.max',
#        'bucketsort_v1.copy_and_merge.avg',
#        'bucketsort_v1.copy_and_merge.stddev', 'bucketsort_v1.cleanup.min',
#        'bucketsort_v1.cleanup.max', 'bucketsort_v1.cleanup.avg',
#        'bucketsort_v1.cleanup.stddev', 'bucketsort_v1.min',
#        'bucketsort_v1.max', 'bucketsort_v1.avg', 'bucketsort_v1.stddev'],
#       dtype='str')

# Columns in df_v3: Index(['max_threads', 'array_size', 'num_buckets', 'job_id',
#        'omp_auto_xorshift32.min', 'omp_auto_xorshift32.max',
#        'omp_auto_xorshift32.avg', 'omp_auto_xorshift32.stddev',
#        'bucketsort_v3.init.min', 'bucketsort_v3.init.max',
#        'bucketsort_v3.init.avg', 'bucketsort_v3.init.stddev',
#        'bucketsort_v3.distribution.min', 'bucketsort_v3.distribution.max',
#        'bucketsort_v3.distribution.avg', 'bucketsort_v3.distribution.stddev',
#        'bucketsort_v3.calc_offsets.min', 'bucketsort_v3.calc_offsets.max',
#        'bucketsort_v3.calc_offsets.avg', 'bucketsort_v3.calc_offsets.stddev',
#        'bucketsort_v3.copy_and_merge.min', 'bucketsort_v3.copy_and_merge.max',
#        'bucketsort_v3.copy_and_merge.avg',
#        'bucketsort_v3.copy_and_merge.stddev', 'bucketsort_v3.sort.min',
#        'bucketsort_v3.sort.max', 'bucketsort_v3.sort.avg',
#        'bucketsort_v3.sort.stddev', 'bucketsort_v3.min_elem.min',
#        'bucketsort_v3.min_elem.max', 'bucketsort_v3.min_elem.avg',
#        'bucketsort_v3.min_elem.stddev', 'bucketsort_v3.max_elem.min',
#        'bucketsort_v3.max_elem.max', 'bucketsort_v3.max_elem.avg',
#        'bucketsort_v3.max_elem.stddev', 'bucketsort_v3.avg_elem.min',
#        'bucketsort_v3.avg_elem.max', 'bucketsort_v3.avg_elem.avg',
#        'bucketsort_v3.avg_elem.stddev', 'bucketsort_v3.stddev_elem.min',
#        'bucketsort_v3.stddev_elem.max', 'bucketsort_v3.stddev_elem.avg',
#        'bucketsort_v3.stddev_elem.stddev', 'bucketsort_v3.cleanup.min',
#        'bucketsort_v3.cleanup.max', 'bucketsort_v3.cleanup.avg',
#        'bucketsort_v3.cleanup.stddev', 'bucketsort_v3.min',
#        'bucketsort_v3.max', 'bucketsort_v3.avg', 'bucketsort_v3.stddev'],
#       dtype='str')


LABEL_MAPPING = {
    "omp_auto_xorshift32.avg": "Czas losowania",
    "bucketsort_v1.init.avg": "Czas inicjalizacji",
    "bucketsort_v1.distribution.avg": "Czas dystrybucji",
    "bucketsort_v1.calc_offsets.avg": "Czas obliczania offsetów",
    "bucketsort_v1.sort.avg": "Czas sortowania",
    "bucketsort_v1.copy_and_merge.avg": "Czas przepisywania/łączenia",
    "bucketsort_v1.cleanup.avg": "Czas sprzątania",
    "bucketsort_v1.avg": "Całkowity czas działania",
    "bucketsort_v3.init.avg": "Czas inicjalizacji",
    "bucketsort_v3.distribution.avg": "Czas dystrybucji",
    "bucketsort_v3.calc_offsets.avg": "Czas obliczania offsetów",
    "bucketsort_v3.copy_and_merge.avg": "Czas przepisywania/łączenia",
    "bucketsort_v3.sort.avg": "Czas sortowania",
    "bucketsort_v3.cleanup.avg": "Czas sprzątania",
    "bucketsort_v3.avg": "Całkowity czas działania",
}

ERR_MAPPING = {
    "omp_auto_xorshift32.avg": "omp_auto_xorshift32.stddev",
    "bucketsort_v1.init.avg": "bucketsort_v1.init.stddev",
    "bucketsort_v1.distribution.avg": "bucketsort_v1.distribution.stddev",
    "bucketsort_v1.calc_offsets.avg": "bucketsort_v1.calc_offsets.stddev",
    "bucketsort_v1.sort.avg": "bucketsort_v1.sort.stddev",
    "bucketsort_v1.copy_and_merge.avg": "bucketsort_v1.copy_and_merge.stddev",
    "bucketsort_v1.cleanup.avg": "bucketsort_v1.cleanup.stddev",
    "bucketsort_v1.avg": "bucketsort_v1.stddev",
    "bucketsort_v3.init.avg": "bucketsort_v3.init.stddev",
    "bucketsort_v3.distribution.avg": "bucketsort_v3.distribution.stddev",
    "bucketsort_v3.calc_offsets.avg": "bucketsort_v3.calc_offsets.stddev",
    "bucketsort_v3.copy_and_merge.avg": "bucketsort_v3.copy_and_merge.stddev",
    "bucketsort_v3.sort.avg": "bucketsort_v3.sort.stddev",
    "bucketsort_v3.cleanup.avg": "bucketsort_v3.cleanup.stddev",
    "bucketsort_v3.avg": "bucketsort_v3.stddev",
}


MARKERS = ["o", "s", "^", "D", "v", "<", ">", "p", "*", "h"]


def plot_runtime(df, columns, title, filename):
    df = df.sort_values("max_threads")
    x = df["max_threads"]

    for i, col in enumerate(columns):
        label = LABEL_MAPPING.get(col, col)
        err_col = ERR_MAPPING.get(col)
        marker = MARKERS[i % len(MARKERS)]

        if err_col and err_col in df.columns:
            plt.errorbar(x, df[col], yerr=df[err_col], marker=marker, label=label)
        else:
            plt.plot(x, df[col], marker=marker, label=label)

    plt.title(title)
    plt.xlabel("Liczba wątków")
    plt.ylabel("Czas [ms]")
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    plt.savefig(OUTPUT_DIR / filename)
    plt.close()


def plot_speedup(max_threads, columns, title):
    pass


def main():
    Path(OUTPUT_DIR).mkdir(exist_ok=True)

    df_v1 = pd.read_csv("results_v1.csv")
    df_v3 = pd.read_csv("results_v3.csv")

    plot_runtime(
        df_v1[
            (df_v1["array_size"] == ARRAY_SIZE) & (df_v1["num_buckets"] == NUM_BUCKETS)
        ],
        [
            "omp_auto_xorshift32.avg",
            "bucketsort_v1.init.avg",
            "bucketsort_v1.distribution.avg",
            "bucketsort_v1.calc_offsets.avg",
            "bucketsort_v1.sort.avg",
            "bucketsort_v1.copy_and_merge.avg",
            "bucketsort_v1.cleanup.avg",
            "bucketsort_v1.avg",
        ],
        f"Sortowanie kubełkowe v1 - {ARRAY_SIZE:g} elementów, {NUM_BUCKETS:g} kubełków",
        "runtime_v1.png",
    )

    plot_runtime(
        df_v3[
            (df_v3["array_size"] == ARRAY_SIZE) & (df_v3["num_buckets"] == NUM_BUCKETS)
        ],
        [
            "omp_auto_xorshift32.avg",
            "bucketsort_v3.init.avg",
            "bucketsort_v3.distribution.avg",
            "bucketsort_v3.calc_offsets.avg",
            "bucketsort_v3.copy_and_merge.avg",
            "bucketsort_v3.sort.avg",
            "bucketsort_v3.cleanup.avg",
            "bucketsort_v3.avg",
        ],
        f"Sortowanie kubełkowe v3 - {ARRAY_SIZE:g} elementów, {NUM_BUCKETS:g} kubełków",
        "runtime_v3.png",
    )


if __name__ == "__main__":
    main()

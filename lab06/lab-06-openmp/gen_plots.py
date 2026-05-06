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

ARRAY_SIZE = [1_000, 300_000, 100_000_000]
NUM_BUCKETS = [100, 10_000, 1_000_000]
RUN_REPEATS = 10

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
COLORS = [
    "#1f77b4",
    "#ff7f0e",
    "#2ca02c",
    "#d62728",
    "#9467bd",
    "#8c564b",
    "#e377c2",
    "#7f7f7f",
    "#bcbd22",
    "#17becf",
]

UNIQUE_LABELS = [
    "Czas losowania",
    "Czas inicjalizacji",
    "Czas dystrybucji",
    "Czas obliczania offsetów",
    "Czas sortowania",
    "Czas przepisywania/łączenia",
    "Czas sprzątania",
    "Całkowity czas działania",
]

STYLE_MAPPING = {
    label: {"color": COLORS[i % len(COLORS)], "marker": MARKERS[i % len(MARKERS)]}
    for i, label in enumerate(UNIQUE_LABELS)
}


def plot_runtime(df, columns, title, filename):
    df = df.sort_values("max_threads")
    x = df["max_threads"]

    for col in columns:
        label = LABEL_MAPPING.get(col, col)
        err_col = ERR_MAPPING.get(col)
        style = STYLE_MAPPING.get(label, {"marker": "o", "color": "black"})

        if err_col and err_col in df.columns:
            plt.errorbar(
                x,
                df[col],
                yerr=df[err_col],
                marker=style["marker"],
                color=style["color"],
                label=label,
            )
        else:
            plt.plot(
                x,
                df[col],
                marker=style["marker"],
                color=style["color"],
                label=label,
            )

    plt.title(title)
    plt.xlabel("Liczba wątków")
    plt.ylabel("Czas [ms]")
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    plt.savefig(OUTPUT_DIR / filename)
    plt.close()


def plot_speedup(df, columns, title, filename):
    df = df.sort_values("max_threads")
    x = df["max_threads"]

    df_single = df[df["max_threads"] == 1]
    if df_single.empty:
        print(f"Brak danych dla 1 wątku do obliczenia przyspieszenia: {title}")
        return

    for col in columns:
        label = LABEL_MAPPING.get(col, col)
        style = STYLE_MAPPING.get(label, {"marker": "o", "color": "black"})

        t_1 = df_single[col].values[0]
        speedup = t_1 / df[col]

        plt.plot(x, speedup, marker=style["marker"], color=style["color"], label=label)

    plt.plot(
        x, x, linestyle="--", color="black", alpha=0.5, label="Idealne przyspieszenie"
    )

    plt.title(title)
    plt.xlabel("Liczba wątków")
    plt.ylabel("Przyspieszenie względne")
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    plt.savefig(OUTPUT_DIR / filename)
    plt.close()


def main():
    Path(OUTPUT_DIR).mkdir(exist_ok=True)

    # load data
    df_v1 = pd.read_csv("results_v1.csv")
    df_v3 = pd.read_csv("results_v3.csv")

    # filter for specific array size and number of buckets
    array_size = ARRAY_SIZE[-1]
    for num_buckets in NUM_BUCKETS:
        # plot runtime for fixed array size and num buckets
        plot_runtime(
            df_v1[
                (df_v1["array_size"] == array_size)
                & (df_v1["num_buckets"] == num_buckets)
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
            f"Sortowanie kubełkowe v1 - {array_size:g} elementów, {num_buckets:g} kubełków",
            f"runtime_v1_{num_buckets}.png",
        )

        plot_runtime(
            df_v3[
                (df_v3["array_size"] == array_size)
                & (df_v3["num_buckets"] == num_buckets)
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
            f"Sortowanie kubełkowe v3 - {array_size:g} elementów, {num_buckets:g} kubełków",
            f"runtime_v3_{num_buckets}.png",
        )

        # plot speedup compared to single-threaded version (max_threads=1)
        plot_speedup(
            df_v1[
                (df_v1["array_size"] == array_size)
                & (df_v1["num_buckets"] == num_buckets)
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
            f"Przyspieszenie v1 - {array_size:g} elementów, {num_buckets:g} kubełków",
            f"speedup_v1_{num_buckets}.png",
        )

        plot_speedup(
            df_v3[
                (df_v3["array_size"] == array_size)
                & (df_v3["num_buckets"] == num_buckets)
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
            f"Przyspieszenie v3 - {array_size:g} elementów, {num_buckets:g} kubełków",
            f"speedup_v3_{num_buckets}.png",
        )


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

if __name__ == "__main__":
    main()

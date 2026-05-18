# /// script
# requires-python = ">=3.10"
# dependencies = [
#     "pandas",
#     "matplotlib",
# ]
# ///

import matplotlib.pyplot as plt
import pandas as pd


def main():
    df = pd.read_csv("results.csv")

    df["time"] = pd.to_numeric(df["time"], errors="coerce")

    size_mapping = {"1G": 1, "10G": 10, "20G": 20}
    df["dataSizeNum"] = df["dataSize"].map(size_mapping)

    avg_df = (
        df.groupby(["nCores", "confId", "dataSize", "dataSizeNum"])["time"]
        .mean()
        .reset_index()
    )
    avg_df = avg_df.sort_values("dataSizeNum")

    plt.figure(figsize=(10, 6))

    line_styles = ["-", "--", "-.", ":"]
    markers = ["o", "s", "^", "D"]

    for i, config in enumerate(avg_df["confId"].unique()):
        subset = avg_df[avg_df["confId"] == config]
        subset = subset.dropna(subset=["time"])

        plt.plot(
            subset["dataSizeNum"],
            subset["time"],
            linestyle=line_styles[i],
            marker=markers[i],
            linewidth=2,
            label=f"{config}",
        )

    plt.title("Średni czas obliczeń w zależności od rozmiaru danych")
    plt.xlabel("Rozmiar danych wejściowych [GB]")
    plt.ylabel("Średni czas obliczeń [s]")
    plt.xticks([1, 10, 20], ["1G", "10G", "20G"])
    plt.legend()
    plt.grid(True, linestyle="--")
    plt.tight_layout()

    output_file = "wykres.png"
    plt.savefig(output_file)


if __name__ == "__main__":
    main()

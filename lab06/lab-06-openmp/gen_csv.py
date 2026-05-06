from pathlib import Path
import csv
import statistics
from collections import defaultdict

INPUT_DIR = Path("results")
OUTPUT_FILE = Path("results.csv")


def parse_filename(filename):
    # omp_bucketsort_[max_threads]_[array_size]_[num_buckets]_[job_id]_[task_id].out
    parts = filename.stem.split("_")
    return {
        "max_threads": int(parts[2]),
        "array_size": int(parts[3]),
        "num_buckets": int(parts[4]),
        "job_id": parts[5],
        "task_id": parts[6],
    }


def parse_output_file(filepath):
    # omp_auto_xorshift32          0.014271 ms
    # bucketsort_v3.init           0.006092 ms
    # bucketsort_v3.distribution   0.010276 ms
    # bucketsort_v3.calc_offsets   0.001177 ms
    # bucketsort_v3.copy_and_merge 0.001952 ms
    # bucketsort_v3.sort           0.035432 ms
    # bucketsort_v3.min_elem       5
    # bucketsort_v3.max_elem       17
    # bucketsort_v3.avg_elem       10.00
    # bucketsort_v3.stddev_elem    3.09
    # bucketsort_v3.cleanup        0.005092 ms
    # bucketsort_v3                0.065972 ms
    data = {}
    with open(filepath, "r") as f:
        for line in f:
            parts = line.split()
            if len(parts) >= 2:
                key = parts[0]
                value = float(parts[1])
                data[key] = value
    return data


def group_results_by_config(input_dir):
    grouped_data = defaultdict(lambda: defaultdict(list))

    for filepath in input_dir.glob("*.out"):
        params = parse_filename(filepath)
        # group by everything except task_id to aggregate over repeated runs
        group_key = (
            params["max_threads"],
            params["array_size"],
            params["num_buckets"],
            params["job_id"],
        )

        file_data = parse_output_file(filepath)
        for metric, value in file_data.items():
            grouped_data[group_key][metric].append(value)

    return grouped_data


def calculate_statistics(grouped_data):
    results = []
    for group_key, metrics in grouped_data.items():
        max_threads, array_size, num_buckets, job_id = group_key
        row = {
            "max_threads": max_threads,
            "array_size": array_size,
            "num_buckets": num_buckets,
            "job_id": job_id,
        }

        for metric_name, values in metrics.items():
            row[f"{metric_name}.min"] = min(values)
            row[f"{metric_name}.max"] = max(values)
            row[f"{metric_name}.avg"] = statistics.mean(values)
            # standard deviation requires at least 2 data points
            row[f"{metric_name}.stddev"] = (
                statistics.stdev(values) if len(values) > 1 else 0.0
            )

        results.append(row)
    return results


def write_csv(output_file, results):
    if not results:
        print("No data to write.")
        return

    headers = list(results[0].keys())
    with open(output_file, "w", newline="") as f:
        writer = csv.DictWriter(f, fieldnames=headers)
        writer.writeheader()
        writer.writerows(results)


def main():
    if not INPUT_DIR.exists():
        print(f"Directory {INPUT_DIR} does not exist.")
        return

    grouped_data = group_results_by_config(INPUT_DIR)
    stats_results = calculate_statistics(grouped_data)
    write_csv(OUTPUT_FILE, stats_results)
    print(
        f"Successfully processed {len(stats_results)} configurations into {OUTPUT_FILE}"
    )


if __name__ == "__main__":
    main()

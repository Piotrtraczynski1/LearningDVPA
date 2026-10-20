#!/usr/bin/env bash
# Run from any directory. Builds and benchmarks execute sequentially; no CSV merging.
set -euo pipefail

repo_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$repo_dir"
mkdir -p src/benchmark/results
batch_dir="$(mktemp -d "$repo_dir/src/benchmark/results/missing-srs-XXXXXXXX")"
printf 'Results and logs: %s\n' "$batch_dir"

bash build.sh -mixedEQ > "$batch_dir/build-mixed.log" 2>&1
bash build.sh -randomEQ > "$batch_dir/build-random.log" 2>&1

run_scenario() {
    local executable="$1" scenario="$2" expected_rows="$3"
    local source_csv="$repo_dir/src/benchmark/results/$scenario/runs.csv"
    # BenchmarkRunner overwrites runs.csv. Preserve any earlier output first.
    if [[ -f "$source_csv" ]]; then
        mkdir -p "$batch_dir/previous"
        cp -- "$source_csv" "$batch_dir/previous/$scenario.csv"
    fi
    printf 'Starting %s (%s measurements)\n' "$scenario" "$expected_rows"
    "$executable" bench "$scenario" > "$batch_dir/$scenario.log" 2>&1
    local actual_rows
    actual_rows="$(awk 'END {print NR - 1}' "$source_csv")"
    if [[ "$actual_rows" != "$expected_rows" ]]; then
        printf 'Incomplete output: %s, expected %s rows, got %s\n' \
            "$scenario" "$expected_rows" "$actual_rows" >&2
        return 1
    fi
    cp -- "$source_csv" "$batch_dir/$scenario.csv"
    printf 'Completed %s: %s rows\n' "$scenario" "$actual_rows"
}

run_scenario ./build-mixed/run cancellation-srs-mixed-exact-subset 500
run_scenario ./build-mixed/run commutation-srs-mixed-exact-subset 1050
run_scenario ./build-mixed/run idempotency-srs-mixed-exact-subset 400
run_scenario ./build-random/run probabilistic-srs-tuning-replay 16800
printf 'All scenarios completed. New CSV files: %s\n' "$batch_dir"

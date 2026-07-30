# Learning Deterministic Visibly Pushdown Automata Under Accessible Stack

## Description

This repository implements an active learning algorithm for Deterministic Visibly Pushdown Automata (DVPAs) with accessible stack, as described in [Learning Deterministic Visibly Pushdown Automata Under Accessible Stack](https://drops.dagstuhl.de/storage/00lipics/lipics-vol241-mfcs2022/LIPIcs.MFCS.2022.74/LIPIcs.MFCS.2022.74.pdf). Additionally, the implementation extends the algorithm with an advice mechanism based on SRS, automaton generators, and benchmarking mechanisms.

## Requirements

- CMake 3.11 or newer
- a C++ compiler with C++20 support
- Git and an Internet connection when building the unit tests for the first time

## Building

Make the build script executable once after cloning the repository:

```bash
chmod +x build.sh
```

Build the project from the repository root:

```bash
./build.sh
```

The executable is created at `build/run`.

Additional build modes are available:

```bash
./build.sh -d
./build.sh -randomEQ
./build.sh -d -randomEQ
```

The `-d` option enables additional diagnostic checks and creates `build-debug/run`. The `-randomEQ` option enables random equivalence queries and creates `build-random/run`. When both options are used, the executable is created at `build-debug-random/run`.

## Running

Run the executable from the repository root:

```bash
./build/run <generator> <seed> <numOfTests>
./build/run bench <scenario>
./build/run custom
./build/run --help
```

Available generators are `random`, `cda`, `sevpa`, `mevpa`, `ecda`, `xml`, `commutative`, `cancel`, and `idempotency`.

Examples:

```bash
./build/run random 42 100
./build/run cda 123 50
./build/run bench increasing-number-of-states-base
./build/run custom
```

Use the executable from the appropriate build directory for diagnostic or random-equivalence-query builds. The available benchmark scenarios are defined in [`src/benchmark/scenario/Scenarios.hpp`](src/benchmark/scenario/Scenarios.hpp).

## Tests

Build and run all unit tests:

```bash
./build.sh test
```

Run the tests with diagnostic checks enabled:

```bash
./build.sh -d test
```

Build and run one unit test target:

```bash
./build.sh test -t <TEST_NAME>
./build.sh -d test -t <TEST_NAME>
```

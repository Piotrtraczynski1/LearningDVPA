# Learning Visibly Pushdown Languages under Rewriting Rules

## Description

The baseline implements the algorithm described in [this paper](https://drops.dagstuhl.de/storage/00lipics/lipics-vol241-mfcs2022/LIPIcs.MFCS.2022.74/LIPIcs.MFCS.2022.74.pdf). This algorithm is extended with the advice mechanism described in the AAAI submission.

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

## Experiments

### Parameters

To change the number of membership queries used to approximate equivalence queries change `numOfRandomWords` in `src/utils/EquivalenceCheckConfig.hpp`. In the same file parameter `maxLengthRandomWord` is the bound on the length of these words.

To change various parameters of test edit `src/TesterParameters.hpp`. Most parameter names are self-explaining: 
  - minNumOfStates, maxNumOfStates: bounds on the number of states of randomly generated automata
  - minNumOfCalls, maxNumOfCalls, minNumOfLocals, maxNumOfLocals, minNumOfReturns, maxNumOfReturns: bounds on the size of randomly generated pushdown alphabet
  - minNumOfStackSymbols, maxNumOfStackSymbols: bound on randomly generated stack alphabet
  - numOfRandomTestingWords: number of words used to approximate consistency tests
  - maxTestingWordLength: maximal length of words used to approximate consistency test
  - density: probability with with each randomly generated transition does not lead to the sink state
  - useSrs: Boolean value whether to use the advice mechanism
  - useEquivalenceCheckToValidateOutput: Boolean value whether the perform the exact equivalence test on the learned automaton. Relevant only in the probabilistic setting. 
  - generator-specifc parameters: numOfModules, minSecondDvpaNumOfStates, maxSecondDvpaNumOfStates
  - additional debugging and supervision options: savePassedTestData, supervisedMode, supervisedTestMaxDuration

Note: Parameter sets are grouped by generator. Separate parameter sets are defined for builds with and without `-randomEQ`.

### Benchmarks

The committed benchmark scenarios are registered in [`src/benchmark/scenario/Scenarios.hpp`](src/benchmark/scenario/Scenarios.hpp). The `*-increasing-number-of-states` scenarios provide the data for the exact equivalence query analysis, while the `*-accuracy` scenarios provide the data for the probabilistic equivalence query analysis and require a build with `-randomEQ`.

Run a benchmark with:

```bash
./build/run bench cancellation-increasing-number-of-states
./build-random/run bench cancellation-accuracy
```

Each run writes its output to `src/benchmark/results/<scenario>/runs.csv`. Some data sets used in the notebooks were collected in several batches with different seeds.

The mixed equivalence query data in the notebooks was produced with local scenarios that are not part of the provided source package. The corresponding processed data files are available in `results/data/mixedEq/`, but reproducing or extending the mixed experiments requires changes to the source code.

### XML

1. Fix `numOfRandomWords` in `src/utils/EquivalenceCheckConfig.hpp`. In the probabilistic setting 30 000 000 is sufficient to learn a correct automaton, while 25000000 is not (with the seed 123). In the exact setting, the value 3000 000 000 is insufficient to learn a correct automaton. The information whether the automaton passed the exact test is displayed in the last line.

2. Fix parameters of the test in `src/TesterParameters.hpp`.

3. Run `build/run xml 123 1` or `build-random/run xml 123 1` depending on the setting you plan to test.
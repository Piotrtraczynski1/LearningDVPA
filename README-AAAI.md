# Learning Deterministic Visibly Pushdown Automata Under Accessible Stack

## Description

Implementation of the algorithm described in [this paper](https://drops.dagstuhl.de/storage/00lipics/lipics-vol241-mfcs2022/LIPIcs.MFCS.2022.74/LIPIcs.MFCS.2022.74.pdf).


## Installation

### Dependencies


1. **Google Test**

   - Install GTest and required build tools:
     ```bash
     sudo apt install gtest
     sudo apt install build-essential
     sudo apt install libgtest-dev
     ```
   - Clone the GTest repository, build and install it:
     ```bash
     git clone https://github.com/google/googletest.git
     cd googletest/
     cmake .
     make
     sudo make install
     ```

2. **Clang-Tidy**

   - Install Clang and Clang-Tidy:
     ```bash
     sudo apt install clang
     sudo apt install clang-tidy
     ```
   - To enable Clang-Tidy static analysis in Visual Studio Code, add the following configuration to your `settings.json`:
     ```json
     {
       "C_Cpp.codeAnalysis.clangTidy.enabled": true,
       "C_Cpp.codeAnalysis.clangTidy.useBuildPath": true,
       "C_Cpp.codeAnalysis.clangTidy.path": "/usr/bin/clang-tidy",
       "C_Cpp.codeAnalysis.clangTidy.args": [
         "--config-file=.clang-tidy"
       ]
     }
     ```

3. **JSON for Modern C++**
   - Install the `nlohmann-json` library for JSON handling:
     ```bash
     sudo apt install nlohmann-json3-dev
     ```

### `build.sh` script
- Add execution privileges to script:
    ```bash
    chmod +x build.sh
    ```

1. **Project build**
    - To build the project run
    ```bash
    ./build.sh
    ```
    The executable file `run` will be placed in the `./build` directory.

    - To build the project run with the probabilistic test
    ```bash
    ./build.sh -randomEQ
    ```
    The executable file `run` will be placed in the `./build-random` directory.

    - To build and run UTs
    ```bash
    ./build.sh test
    ```
    - To build and a run single UT
    ```bash
    ./build.sh test -t [TEST_NAME]
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
  - savePassedTestData  ??
  - supervisedMode ??
  - supervisedTestMaxDuration ??

### Benchmarks

??

### XML

1. Fix `numOfRandomWords` in `src/utils/EquivalenceCheckConfig.hpp`. In the probabilistic setting 30 000 000 is sufficient to learn a correct automaton, while 25000000 is not (with the seed 123). In the exact setting, the value 3000 000 000 is insufficient to learn a correct automaton. The information whether the automaton passed the exact test is displayed in the last line.

2. Fix parameters of the test in `src/TesterParameters.hpp`.

3. Run `build/run xml 123 1` or `build-random/run xml 123 1` depending on the setting you plan to test.
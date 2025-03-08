# Learning Deterministic Visibly Pushdown Automata Under Accessible Stack

## Description

Implementation of the algorithm described in [this paper](https://drops.dagstuhl.de/storage/00lipics/lipics-vol241-mfcs2022/LIPIcs.MFCS.2022.74/LIPIcs.MFCS.2022.74.pdf).


## Dependencies

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

## `build.sh` script
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

    - To build and run UTs
    ```bash
    ./build.sh test
    ```
    - To build and a run single UT
    ```bash
    ./build.sh test -t [TEST_NAME]
    ```


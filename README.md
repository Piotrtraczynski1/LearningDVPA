## REQUIREMENTS

To Run UT:
- sudo apt install gtest
- sudo apt install build-essential
- sudo apt install libgtest-dev
- git clone https://github.com/google/googletest.git
- cd googletest/
- cmake .
- make
- sudo make install

- chmod +x build.sh

Clang-tidy:
- sudo apt install clang
- sudo apt install clang-tidy
- settings.json:
{
    "C_Cpp.codeAnalysis.clangTidy.enabled": true,
    "C_Cpp.codeAnalysis.clangTidy.useBuildPath": true,
    "C_Cpp.codeAnalysis.clangTidy.path": "/usr/bin/clang-tidy",
    "C_Cpp.codeAnalysis.clangTidy.args": [
        "--config-file=.clang-tidy"
    ]
}

Parser:
- sudo apt install nlohmann-json3-dev

add_subdirectory(scenario)

add_library(benchmark STATIC
    BenchmarkRunner.cpp
)

target_link_libraries(benchmark PRIVATE utils common generator learner teacher scenario)

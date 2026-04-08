add_subdirectory(increasingNumOfStates)

add_library(scenario INTERFACE)

target_link_libraries(scenario INTERFACE increasingNumOfStates)

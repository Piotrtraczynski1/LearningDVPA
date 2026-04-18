add_subdirectory(increasingNumOfStates)
add_subdirectory(srs)

add_library(scenario INTERFACE)

target_link_libraries(scenario INTERFACE increasingNumOfStates srsScenarios)

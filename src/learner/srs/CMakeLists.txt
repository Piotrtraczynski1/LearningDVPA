if(BUILD_TESTS)
    add_subdirectory(ut)
endif()

add_library(srs STATIC
    SpecialSymbolAdder.cpp
    SrsChecker.cpp
)

target_link_libraries(srs PRIVATE utils common teacher)

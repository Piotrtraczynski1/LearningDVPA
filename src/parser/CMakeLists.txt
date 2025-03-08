if(BUILD_TESTS)
    add_subdirectory(ut)
endif()

add_library(parser STATIC
    Parser.cpp
)

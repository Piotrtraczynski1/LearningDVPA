if(BUILD_TESTS)
    add_subdirectory(ut)
endif()

add_library(transition STATIC
    Transition.cpp
)

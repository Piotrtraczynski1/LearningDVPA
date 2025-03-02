add_library(transition STATIC
    Transition.cpp
)

if(BUILD_TESTS)
    add_subdirectory(ut)
endif()

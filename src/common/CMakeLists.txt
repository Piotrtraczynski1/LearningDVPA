add_subdirectory(transition)

add_library(common STATIC
    Stack.cpp
    VPA.cpp
    Word.cpp
)

target_link_libraries(common PRIVATE transition)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(ut)
endif()
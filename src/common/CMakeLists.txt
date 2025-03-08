if(BUILD_TESTS)
    add_subdirectory(ut)
endif()

add_subdirectory(transition)

add_library(common STATIC
    Stack.cpp
    VPA.cpp
    Word.cpp
)

target_include_directories(common PRIVATE ${CMAKE_SOURCE_DIR})

target_link_libraries(common PRIVATE transition)
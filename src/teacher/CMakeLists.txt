add_library(teacher STATIC
    Teacher.cpp
)

target_link_libraries(teacher PRIVATE common)

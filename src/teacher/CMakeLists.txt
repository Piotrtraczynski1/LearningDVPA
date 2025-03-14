if(BUILD_TESTS)
    add_subdirectory(ut)
endif()

add_subdirectory(cfg)

add_library(teacher STATIC
    Teacher.cpp
    Converter.cpp
)

target_link_libraries(teacher PRIVATE common cfg)

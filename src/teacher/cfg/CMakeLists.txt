if(BUILD_TESTS)
    add_subdirectory(ut)
endif()

add_library(cfg STATIC
    Cfg.cpp
    Calculator.cpp
)
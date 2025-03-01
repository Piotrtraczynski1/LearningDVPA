macro(add_test_target test_name test_file)
    # Kompilowanie testu z podanego pliku
    add_executable(${test_name} ${test_file})

    # Łączenie z GTest
    target_link_libraries(${test_name} PRIVATE GTest::GTest GTest::Main)

    # Dodanie testu do CMake
    add_test(NAME ${test_name} COMMAND ${test_name})
endmacro()
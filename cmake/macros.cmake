macro(add_test_target test_name test_file)
    add_executable(${test_name} ${test_file})

    target_link_libraries(${test_name} PRIVATE GTest::GTest GTest::Main)

    add_test(NAME ${test_name} COMMAND ${test_name})
endmacro()
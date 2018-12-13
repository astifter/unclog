include_directories(${unclog_INCLUDES})
add_executable(test1 test1.c)
target_link_libraries(test1 unclog)
add_test(NAME test1 COMMAND ./test1)

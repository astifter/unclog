include_directories(${unclog_INCLUDES})
add_executable(test1 test1.c)
target_link_libraries(test1 unclog)
add_custom_target(check echo "ran checks" DEPENDS run_test1)
add_custom_target(run_test1 
	COMMAND ./test1
	DEPENDS test1
	WORKING_DIRECTORY ${CMAKE_CURRENT_BUILD_DIR}
)

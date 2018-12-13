cmake_minimum_required(VERSION 3.5)
enable_testing()

project(unclog C)

set(CMAKE_CFLAGS "--std=c90 -Wall -Wextra -pedantic -fPIC -g $ENV{CFLAGS}")

add_subdirectory(inih)
add_subdirectory(src)
add_subdirectory(tests)

file(GLOB_RECURSE sources RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.c")
file(GLOB_RECURSE headers RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.h")
add_custom_target(format 
	COMMAND clang-format-3.8 -i -style=file ${sources} ${headers} 
	DEPENDS ${sources} ${headers}
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

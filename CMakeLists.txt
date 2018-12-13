cmake_minimum_required(VERSION 3.5)
enable_testing()

project(unclog C)

set(CMAKE_CFLAGS "--std=c90 -Wall -Wextra -pedantic -fPIC -g $ENV{CFLAGS}")

add_subdirectory(inih)
add_subdirectory(src)
add_subdirectory(tests)

#clang-format-3.8 -i -style=file $$(find src tests -name '*.c' -o -name '*.h')

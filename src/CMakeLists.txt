include_directories(${inih_INCLUDES})
add_library(unclog SHARED
	unclog.c
	unclog_global.c
  	unclog_init.c
	unclog_levels.c
	unclog_source.c

	unclog.h
	unclog_int.h
)
target_link_libraries(unclog pthread inih)
set(unclog_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}" CACHE STRING "" FORCE)

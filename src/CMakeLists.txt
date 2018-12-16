include_directories(${inih_INCLUDES})

add_library(unclog SHARED
	unclog.c
	unclog_global.c
  	unclog_config.c
	unclog_levels.c
	unclog_source.c
	unclog_sink.c
	unclog.h
	unclog_int.h
)
target_link_libraries(unclog pthread inih)

install(TARGETS unclog DESTINATION lib COMPONENT Main)
install(FILES unclog.h DESTINATION include COMPONENT Main)

set(unclog_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}" CACHE STRING "" FORCE)

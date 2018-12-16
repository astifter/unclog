include_directories(${unclog_INCLUDES})

add_executable(test1 test1.c)
target_link_libraries(test1 unclog)

install(TARGETS test1 DESTINATION tests COMPONENT Test)
install(FILES unclog.ini DESTINATION tests COMPONENT Test)

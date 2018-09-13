cmake_minimum_required(VERSION 3.5)
include_directories(speedtest)

file(GLOB SOURCES "speedtest/*.c")
add_library(speedtest STATIC ${SOURCES})
install(TARGETS speedtest DESTINATION .)

set ( PROJECT_LINK_LIBS libspeedtest.so curl )
link_directories( . )
add_executable(speedtestapp main.c)
target_link_libraries(speedtestapp ${PROJECT_LINK_LIBS} )
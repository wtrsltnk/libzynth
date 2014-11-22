cmake_minimum_required(VERSION 2.8)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/")
project(libzynth)
set(VERSION "1.0.0")

add_subdirectory(src)

install(FILES LICENSE README.md
	DESTINATION share/doc/libzynth
	)

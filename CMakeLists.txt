### Project source

cmake_minimum_required(VERSION 3.12)

## Project setup
project(sgl

	C
	CXX
)

## Include cmake scripts
include(cmake/version.cmake)
include(cmake/platform.cmake)
include(cmake/compiler.cmake)
include(cmake/target.cmake)

## Add source code
add_subdirectory(src)

## Add test code
add_subdirectory(test)
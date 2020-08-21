### Project source

cmake_minimum_required(VERSION 3.12)

## Project setup
project(korin

	C
	CXX
)

## Include cmake scripts
option(KORIN_USE_EXTERNAL_CMAKE_SETUP "If true, skip local CMake setup" OFF)

if(NOT KORIN_USE_EXTERNAL_CMAKE_SETUP)

	include(cmake/version.cmake)
	include(cmake/platform.cmake)
	include(cmake/compiler.cmake)
	include(cmake/target.cmake)
	
endif(NOT KORIN_USE_EXTERNAL_CMAKE_SETUP)

## Add external projects
add_subdirectory(external)

## Add source code
add_subdirectory(src)

## Add test code
add_subdirectory(test)
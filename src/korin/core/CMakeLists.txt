### Setup net module
set(MODULE_NAME ${PROJECT_NAME}-core)

## Collect source and header files
file(GLOB_RECURSE PRIVATE_FILES

	"private/*.c"
	"private/*.cpp"
	"private/*.cxx"
	"private/*.h"
	"private/*.hpp"
	"private/*.tpp"
)

file(GLOB_RECURSE PUBLIC_FILES

	"private/*.c"
	"private/*.cpp"
	"private/*.cxx"
	"private/*.h"
	"private/*.hpp"
	"private/*.tpp"
)

## Create library target
add_library(${MODULE_NAME} OBJECT

	${PRIVATE_FILES}
	${PUBLIC_FILES}
)

## Setup module dependencies
set(DEPENDENCIES)
foreach(DEPENDENCY ${DEPENDENCIES})

	## Add dependency to list
	list(APPEND DEPENDENCY_LIBRARIES "${PROJECT_NAME}-${DEPENDENCY}")

endforeach()

target_link_libraries(${MODULE_NAME} ${DEPENDENCY_LIBRARIES})

## Include directories
target_include_directories(${MODULE_NAME}

	PUBLIC
		"public"
	PRIVATE
		"private"
)

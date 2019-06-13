### Setup SGL library target
set(MODULE_NAME ${PROJECT_NAME}-core)

## Collect source and header files
file(GLOB_RECURSE PRIVATE_FILES

	"private/*.c"
	"private/*.cpp"
	"private/*.cxx"
)

file(GLOB_RECURSE PUBLIC_FILES

	"public/*.h"
	"public/*.hpp"
	"public/*.tpp"
)

## Create library target
add_library(${MODULE_NAME} OBJECT

	${PRIVATE_FILES}
	${PUBLIC_FILES}
)

## Include directories
target_include_directories(${MODULE_NAME}

	PUBLIC
		"public"
)
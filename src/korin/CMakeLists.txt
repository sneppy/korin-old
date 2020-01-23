### Project source setup

## List of modules
set(MODULES

	"core"
)

foreach(MODULE ${MODULES})

	## Get module directory
	get_filename_component(MODULE_PATH "./${MODULE}" ABSOLUTE)

	## Create folders if necessary
	if(NOT EXISTS "${MODULE_PATH}")
		
		## Create sub directories
		file(MAKE_DIRECTORY "${MODULE_PATH}")
		file(MAKE_DIRECTORY "${MODULE_PATH}/public")
		file(MAKE_DIRECTORY "${MODULE_PATH}/private")

		## Dummy file (avoids no source error)
		file(WRITE "${MODULE_PATH}/private/main.cpp"
		
			"/**\n"
			" * This file serves the only purpose of\n"
			" * having some initial *.cpp file for\n"
			" * CMake configuration. This is a library,\n"
			" * there's not program entry point.\n"
			" */\n"
		)

		## CMake source configuration
		file(WRITE "${MODULE_PATH}/CMakeLists.txt"
		
			"### Setup ${MODULE} module\n"
			"set(MODULE_NAME \${PROJECT_NAME}-${MODULE})\n"
			"\n"
			"## Collect source and header files\n"
			"file(GLOB_RECURSE PRIVATE_FILES\n"
			"\n"
			"\t\"private/*.c\"\n"
			"\t\"private/*.cpp\"\n"
			"\t\"private/*.cxx\"\n"
			")\n"
			"\n"
			"file(GLOB_RECURSE PUBLIC_FILES\n"
			"\n"
			"\t\"public/*.h\"\n"
			"\t\"public/*.hpp\"\n"
			"\t\"public/*.tpp\"\n"
			")\n"
			"\n"
			"## Create library target\n"
			"add_library(\${MODULE_NAME} OBJECT\n"
			"\n"
			"\t\${PRIVATE_FILES}\n"
			"\t\${PUBLIC_FILES}\n"
			")\n"
			"\n"
			"## Setup module dependencies"
			"\n"
			"set(DEPENDENCIES)\n"
			"foreach(DEPENDENCY \${DEPENDENCIES})\n"
			"\n"
			"\t## Add dependency to list\n"
			"\tlist(APPEND DEPENDENCY_LIBRARIES \"\${PROJECT_NAME}-\${DEPENDENCY}\")\n"
			"\n"
			"endforeach()\n"
			"\n"
			"target_link_libraries(\${MODULE_NAME} \${DEPENDENCY_LIBRARIES})\n"
			"\n"
			"## Include directories\n"
			"target_include_directories(\${MODULE_NAME}\n"
			"\n"
			"\tPUBLIC\n"
			"\t\t\"public\"\n"
			")\n"
		)

	endif(NOT EXISTS "${MODULE_PATH}")
	
	## Process module
	add_subdirectory("${MODULE_PATH}")

	## Add target objects to list
	list(APPEND MODULE_LIBRARIES "${PROJECT_NAME}-${MODULE}")
	
endforeach()

## Create global library
add_library(${PROJECT_NAME})
set_target_properties(${PROJECT_NAME}

	PROPERTIES
		OUTPUT_NAME ${PROJECT_NAME}
)
	
## Link all dependency libraries
target_link_libraries(${PROJECT_NAME} ${MODULE_LIBRARIES})

## Create global application
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")
	
	file(WRITE "./main.cpp"
	
		"#include \"core_types.h\"\n"
		"\n"
		"#include <stdio.h>\n"
		"\n"
		"int32 main()\n"
		"{\n"
		"	printf(\"hello ${PROJECT_NAME}!\\n\");\n"

		"	return 0;\n"
		"}\n"
	)

endif(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")

add_executable(${PROJECT_NAME}_test

	"./main.cpp"
)

target_link_libraries(${PROJECT_NAME}_test

	${PROJECT_NAME}
)

## Set target properties
set(OUTPUT_DIR ${PROJECT_SOURCE_DIR}/bin)
set_target_properties(${PROJECT_NAME}_test

	PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR}
)

## Create run target
add_custom_target(run

	COMMAND				$<TARGET_FILE:${PROJECT_NAME}_test>
	DEPENDS				${PROJECT_NAME}
	WORKING_DIRECTORY	${PROJECT_SOURCE_DIR}
)
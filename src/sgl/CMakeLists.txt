### SGL library setup

add_subdirectory(core)

## Create global library
add_library(${PROJECT_NAME}

	$<TARGET_OBJECTS:${PROJECT_NAME}-core>
)

## Populate include directiories interface
target_include_directories(${PROJECT_NAME}

	PUBLIC
		$<TARGET_PROPERTY:${PROJECT_NAME}-core,INTERFACE_INCLUDE_DIRECTORIES>
)
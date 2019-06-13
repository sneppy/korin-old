### Setup Run target

option(CREATE_RUN_TARGET "If true create and build the run target" ON)

if(CREATE_RUN_TARGET)

	## Create executable
	set(TARGET_NAME ${PROJECT_NAME}-run)
	add_executable(${TARGET_NAME}
	
		"run.cpp"
	)

	set(TARGET_OUTPUT_DIR "${PROJECT_SOURCE_DIR}/bin")
	set_target_properties(${TARGET_NAME}
	
		PROPERTIES
			RUNTIME_OUTPUT_DIRECTORY ${TARGET_OUTPUT_DIR}
	)

	## Link SGL library
	target_link_libraries(${TARGET_NAME}
	
		${PROJECT_NAME}
	)

	## Create make target
	add_custom_target(run
	
		COMMAND				"${TARGET_OUTPUT_DIR}/${TARGET_NAME}"
		DEPENDS				${TARGET_NAME}
		WORKING_DIRECTORY	${PROJECT_SOURCE_DIR}
	)

endif(CREATE_RUN_TARGET)

### Include unit tests

option(CREATE_TEST_TARGET "If true create and build the test target" ON)

if(CREATE_TEST_TARGET)

	add_subdirectory(units)

endif(CREATE_TEST_TARGET)
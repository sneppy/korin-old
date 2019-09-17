### Include unit tests

option(CREATE_TEST_TARGET "If true create and build the test target" ON)

if(CREATE_TEST_TARGET)

	add_subdirectory(units)

endif()

### Include benchmarks

option(BUILD_BENCHMARKS "If true build all benchmark targets" ON)

if(BUILD_BENCHMARKS)

	add_subdirectory(benches)

endif()
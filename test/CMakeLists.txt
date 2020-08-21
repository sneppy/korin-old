### Include unit tests

option(KORIN_BUILD_UNITS "If true create and build the test target" ON)

if(KORIN_BUILD_UNITS)

	add_subdirectory(units)

endif(KORIN_BUILD_UNITS)

### Include benchmarks

option(KORIN_BUILD_BENCHES "If true build all benchmark targets" ON)

if(KORIN_BUILD_BENCHES)

	add_subdirectory(benches)

endif()
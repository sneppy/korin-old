### Bunch of test and benchmark suites powered by Google

## This one is for C++ unit tests
add_subdirectory(googletest)

## This one is for C++ code benchmark
# Suppress tests
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Suppress benchmark's tests" FORCE)
add_subdirectory(benchmark)
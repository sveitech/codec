project(codec LANGUAGES CXX)
cmake_minimum_required(VERSION 3.0)

add_subdirectory(lib/googletest)

set(sources

)

set(test_sources
	src/playground_TEST.cpp
)

add_executable(codec_test ${sources} ${test_sources})
target_link_libraries(codec_test gmock gtest_main)
target_include_directories(codec_test PRIVATE include)
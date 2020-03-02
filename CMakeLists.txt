project(codec LANGUAGES CXX)
cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 11)

add_subdirectory(lib/googletest)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

set(sources

)

set(test_sources
	#src/binary_codec_TEST.cpp
	#src/json_codec_TEST.cpp
	src/templates_TEST.cpp
)

add_executable(codec_test ${sources} ${test_sources})
target_link_libraries(codec_test gmock gtest_main)
target_include_directories(codec_test 
	PRIVATE include
	PRIVATE lib/json/include
)
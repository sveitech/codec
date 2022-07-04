cmake_minimum_required(VERSION 3.0)
project(codec LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(GTest)
find_package(cereal)

add_executable(codec src/main.cpp)

add_executable(codec_test
    src/codec_test.cpp
)
target_include_directories(codec_test PRIVATE
    include/
)
target_link_libraries(codec_test
    cereal::cereal
    GTest::gtest
    GTest::gmock
    GTest::gtest_main
)

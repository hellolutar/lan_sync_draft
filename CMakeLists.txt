CMAKE_MINIMUM_REQUIRED(VERSION 3.15 FATAL_ERROR)

PROJECT(demo LANGUAGES CXX)

include(test/CMakeLists.txt)

add_custom_target(line
    COMMAND git ls-files | xargs cat | wc -l
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/src
)

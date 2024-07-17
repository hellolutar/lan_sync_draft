CMAKE_MINIMUM_REQUIRED(VERSION 3.15 FATAL_ERROR)

PROJECT(demo LANGUAGES CXX)

include(${PROJECT_SOURCE_DIR}/etc/config.cmake)
include(${PROJECT_SOURCE_DIR}/etc/common.cmake)

include(test/test.cmake)
include(example/example.cmake)

add_custom_target(line
    COMMAND git ls-files | xargs cat | wc -l
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/src
)

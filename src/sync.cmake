
add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/src/main.cc)

target_link_libraries(
    ${PROJECT_NAME}
    libevent::core
    libevent::pthreads
    $<TARGET_OBJECTS:lansync_common_obj>
)
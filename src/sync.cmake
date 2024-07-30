
add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/src/main.cc)

target_link_libraries(
    ${PROJECT_NAME}
    libevent::core
    libevent::pthreads
    OpenSSL::Crypto
    OpenSSL::SSL
    ${gflags_TARGET}
    $<TARGET_OBJECTS:lansync_common_obj>
)

file(COPY ${PROJECT_SOURCE_DIR}/resources/properties.properties DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

macro(compile_example name)
    add_executable( ${name} ${PROJECT_SOURCE_DIR}/example/${name}.cc)

    target_link_libraries( ${name}
        libevent::core
        libevent::pthreads
        $<TARGET_OBJECTS:lansync_common_obj>
    )
    message("compile example: " ${name})
endmacro(compile_example)


compile_example(tcp_cli)
compile_example(tcp_srv)
include_directories(
  ${PROJECT_SOURCE_DIR}/test
)

add_library(test_env_obj OBJECT
  ${PROJECT_SOURCE_DIR}/test/uint-test/dep/framework_engine_for_test.cc
  ${PROJECT_SOURCE_DIR}/test/uint-test/dep/test_common.cc
  ${PROJECT_SOURCE_DIR}/test/uint-test/dep/setup_road_test.cc

)

find_package(GTest REQUIRED)

include(CTest)

enable_testing()

include(GoogleTest)

macro(ttest name fpath)
  add_executable(${name} ${fpath})
  target_link_libraries(
    ${name}
    GTest::gtest_main
    libevent::core
    libevent::pthreads
    OpenSSL::Crypto
    OpenSSL::SSL
    ${gflags_TARGET}
    $<TARGET_OBJECTS:lansync_common_obj>
    $<TARGET_OBJECTS:test_env_obj>
  )
  # message("compile test: " ${name})
  gtest_discover_tests(${name})
endmacro(ttest)

ttest(net_framework_engine_test
  ${PROJECT_SOURCE_DIR}/test/uint-test/net/net_framework_engine_test.cc
)
ttest(me_tcpcli
  ${PROJECT_SOURCE_DIR}/test/uint-test/endpoint/me_tcpcli.cc
)
ttest(me_tcpsrv
  ${PROJECT_SOURCE_DIR}/test/uint-test/endpoint/me_tcpsrv.cc
)

ttest(resource_manager_test
  ${PROJECT_SOURCE_DIR}/test/uint-test/resource_manager_test.cc
)
ttest(resource_manager_base_filesystem_test
  ${PROJECT_SOURCE_DIR}/test/uint-test/resource_manager_base_filesystem_test.cc
)

ttest(task_test
  ${PROJECT_SOURCE_DIR}/test/uint-test/task/task_test.cc
)
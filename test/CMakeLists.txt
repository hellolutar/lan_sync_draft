include(${PROJECT_SOURCE_DIR}/etc/config.cmake)
include(${PROJECT_SOURCE_DIR}/etc/common.cmake)

include_directories(
  ${PROJECT_SOURCE_DIR}/test
)

find_package(GTest REQUIRED)

include(CTest)

enable_testing()

include(GoogleTest)

macro(ttest name fpath dependenceds_path)
  add_executable(${name} ${fpath} ${dependenceds_path})
  target_link_libraries(
    ${name}
    GTest::gtest_main
    libevent::core
    libevent::pthreads
    lansync_common_obj
  )
  message("testname " ${name})
  gtest_discover_tests(${name})
  add_test(${name} ${name})
endmacro(ttest)

ttest(net_framework_engine_test
  ${PROJECT_SOURCE_DIR}/test/net/net_framework_engine_test.cc
  ${PROJECT_SOURCE_DIR}/test/net_framework_engine_for_test.cc
)
ttest(setup_road_test
  ${PROJECT_SOURCE_DIR}/test/endpoint/setup_road_test.cc
  ${PROJECT_SOURCE_DIR}/test/net_framework_engine_for_test.cc
)
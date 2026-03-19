## unit tests

find_package(gtest REQUIRED)

enable_testing()

add_executable(tests_units main.cc)

target_sources(tests_units PRIVATE
  file.t.cpp
)

target_link_libraries(tests_units PRIVATE wndx::mqlqd::src)
target_link_libraries(tests_units PRIVATE wndx::mqlqd::dev)

if(TARGET code_coverage)
  target_link_libraries(tests_units PRIVATE code_coverage)
endif()

target_link_libraries(tests_units PRIVATE gtest::gtest)

## copy data dir so that tests have required tests data.
file(COPY "${CMAKE_CURRENT_LIST_DIR}/data"
  DESTINATION "${CMAKE_CURRENT_BINARY_DIR}"
)

## enable CMake’s test runner to discover the tests included in the binary (for ctest)
include(GoogleTest)
gtest_discover_tests(tests_units
  ## explicitly state where should lay Testing/Temporary/ etc.
  WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)

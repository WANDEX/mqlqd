# unit tests

file(GLOB_RECURSE units_sources LIST_DIRECTORIES false *.cc *.cpp)

add_executable(tests_units ${units_sources})
# target_link_libraries(tests_units INTERFACE wandex::mqlqd::dev)
target_link_libraries(tests_units PRIVATE mqlqd_src)

if(TARGET code_coverage)
  target_link_libraries(tests_units PRIVATE code_coverage)
endif()

# target_link_libraries(tests_units PRIVATE GTest::gtest_main)

# target_sources(tests_deps PUBLIC ${units_sources})
# target_sources(tests_deps PRIVATE ${units_sources})
# target_link_libraries(tests_units PRIVATE tests_deps)

target_link_libraries(tests_units PRIVATE GTest::gtest_main)
# target_link_libraries(tests_units ${GTEST_LDFLAGS})
# target_compile_options(tests_units PUBLIC ${GTEST_CFLAGS})

# discover tests included in the gtest binary (for ctest)
gtest_discover_tests(tests_units)


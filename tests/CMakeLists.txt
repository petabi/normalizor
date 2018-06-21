enable_testing()
include(GoogleTest)

add_executable(test_normalizor test_normalizor.cpp)
target_compile_options(test_normalizor PRIVATE -Wno-global-constructors)
target_include_directories(test_normalizor PUBLIC ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_BINARY_DIR}/src)
target_link_libraries(test_normalizor normalizor GTest::GTest GTest::Main)
gtest_discover_tests(test_normalizor)

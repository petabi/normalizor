include(CheckCXXCompilerFlag)
check_cxx_compiler_flag(-Wno-missing-prototypes HAVE_CXX_NO_MISSING_PROTOTYPES)

include(FindPkgConfig)
pkg_check_modules(libhs REQUIRED IMPORTED_TARGET libhs)

add_library(normalizor normalizor.cpp)
target_link_libraries(normalizor PUBLIC PkgConfig::libhs)
target_link_libraries(normalizor PRIVATE Boost::filesystem)

add_library(py_normalizor MODULE py_normalizor.cpp normalizor.cpp)
set_target_properties(py_normalizor PROPERTIES
  OUTPUT_NAME "normalizor")
if(HAVE_CXX_NO_MISSING_PROTOTYPES)
  target_compile_options(py_normalizor PRIVATE -Wno-missing-prototypes)
endif()
set_target_properties(py_normalizor PROPERTIES PREFIX "")
target_link_libraries(py_normalizor PRIVATE
  Python3::Python
  Boost::python${Python3_VERSION_MAJOR}${Python3_VERSION_MINOR}
  Boost::filesystem PkgConfig::libhs)

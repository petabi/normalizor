include(CheckCXXCompilerFlag)
check_cxx_compiler_flag(-Wno-missing-prototypes HAVE_CXX_NO_MISSING_PROTOTYPES)

find_library(HYPERSCAN_LIB hs ${CMAKE_INSTALL_PREFIX}/lib)

add_library(normalizor normalizor.cpp)
target_link_libraries(normalizor PUBLIC ${HYPERSCAN_LIB})
target_link_libraries(normalizor PRIVATE Boost::filesystem)

add_library(py_normalizor MODULE py_normalizor.cpp normalizor.cpp)
if(HAVE_CXX_NO_MISSING_PROTOTYPES)
  target_compile_options(py_normalizor PRIVATE -Wno-missing-prototypes)
endif()
set_target_properties(py_normalizor PROPERTIES PREFIX "")
target_link_libraries(py_normalizor
  PRIVATE Python3::Python Boost::python36 Boost::filesystem ${HYPERSCAN_LIB})

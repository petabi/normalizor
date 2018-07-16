include(CheckCXXCompilerFlag)
check_cxx_compiler_flag(-Wno-missing-prototypes HAVE_CXX_NO_MISSING_PROTOTYPES)
check_cxx_compiler_flag(-Wno-zero-as-null-pointer-constant
  HAVE_CXX_NO_ZERO_AS_NULL_POINTER_CONSTANT)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-missing-prototypes")

find_library(HYPERSCAN_LIB hs ${CMAKE_INSTALL_PREFIX}/lib)

add_library(normalizor normalizor.cpp)
target_link_libraries(normalizor PUBLIC ${HYPERSCAN_LIB})
target_link_libraries(normalizor PRIVATE Boost::filesystem)

add_library(py_normalizor MODULE py_normalizor.cpp normalizor.cpp)
set_target_properties(py_normalizor PROPERTIES PREFIX "")
target_link_libraries(py_normalizor
  PRIVATE Python3::Python Boost::python36 Boost::filesystem ${HYPERSCAN_LIB})

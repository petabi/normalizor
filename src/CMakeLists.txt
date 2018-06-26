include(CheckCXXCompilerFlag)
check_cxx_compiler_flag(-Wno-missing-prototypes HAVE_CXX_NO_MISSING_PROTOTYPES)
check_cxx_compiler_flag(-Wno-zero-as-null-pointer-constant
  HAVE_CXX_NO_ZERO_AS_NULL_POINTER_CONSTANT)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-missing-prototypes")

find_library(HYPERSCAN_LIB hs ${CMAKE_INSTALL_PREFIX}/lib)
include_directories(${Boost_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS})
add_library(normalizor normalizor.cpp)
target_link_libraries(normalizor Boost::filesystem ${HYPERSCAN_LIB})

add_library(pynormalizor SHARED py_normalizor.cpp)
target_link_libraries(pynormalizor normalizor ${PYTHON_LIBRARY} Boost::python36)

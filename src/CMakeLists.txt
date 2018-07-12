include(CheckCXXCompilerFlag)
check_cxx_compiler_flag(-Wno-missing-prototypes HAVE_CXX_NO_MISSING_PROTOTYPES)
check_cxx_compiler_flag(-Wno-zero-as-null-pointer-constant
  HAVE_CXX_NO_ZERO_AS_NULL_POINTER_CONSTANT)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-missing-prototypes")

find_library(HYPERSCAN_LIB hs ${CMAKE_INSTALL_PREFIX}/lib)

add_library(normalizor normalizor.cpp)
target_include_directories(normalizor PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(normalizor Boost::filesystem ${HYPERSCAN_LIB})

add_library(py_normalizor MODULE py_normalizor.cpp normalizor.cpp)
set_target_properties(py_normalizor PROPERTIES PREFIX "")
target_include_directories(py_normalizor PRIVATE  ${Boost_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS})
target_link_libraries(py_normalizor ${PYTHON_LIBRARY} Boost::python36 Boost::filesystem ${HYPERSCAN_LIB})

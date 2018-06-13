include(CheckCXXCompilerFlag)
check_cxx_compiler_flag(-Wno-missing-prototypes HAVE_CXX_NO_MISSING_PROTOTYPES)
check_cxx_compiler_flag(-Wno-zero-as-null-pointer-constant
  HAVE_CXX_NO_ZERO_AS_NULL_POINTER_CONSTANT)
find_package(Boost REQUIRED COMPONENTS filesystem program_options)
find_library(HYPERSCAN_LIB hs ${CMAKE_INSTALL_PREFIX}/lib)

add_library(normalizor normalizor.cpp)
target_link_libraries(normalizor Boost::filesystem ${HYPERSCAN_LIB})

include(CheckCXXCompilerFlag)
check_cxx_compiler_flag(-Wno-missing-prototypes HAVE_CXX_NO_MISSING_PROTOTYPES)
check_cxx_compiler_flag(-Wno-zero-as-null-pointer-constant
  HAVE_CXX_NO_ZERO_AS_NULL_POINTER_CONSTANT)

add_executable(testor testor.cpp)
target_link_libraries(testor normalizor ${Boost_LIBRARIES})

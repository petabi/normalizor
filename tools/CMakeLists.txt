include(CheckCXXCompilerFlag)
check_cxx_compiler_flag(-Wno-missing-prototypes HAVE_CXX_NO_MISSING_PROTOTYPES)
check_cxx_compiler_flag(-Wno-zero-as-null-pointer-constant
  HAVE_CXX_NO_ZERO_AS_NULL_POINTER_CONSTANT)
find_library(GOOGLE_PROFILER_LIBRARY NAMES profiler PATHS "/opt/pkg")

add_executable(testor testor.cpp)
target_link_libraries(testor
  normalizor Boost::filesystem Boost::program_options
  ${GOOGLE_PROFILER_LIBRARY})

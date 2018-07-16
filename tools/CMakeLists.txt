find_library(GOOGLE_PROFILER_LIBRARY NAMES profiler PATHS "/opt/pkg")

add_executable(testor testor.cpp)
target_link_libraries(testor
  normalizor Boost::filesystem Boost::program_options
  ${GOOGLE_PROFILER_LIBRARY})

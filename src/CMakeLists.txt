set(CMAKE_CXX_STANDARD 14)
find_package(Boost REQUIRED COMPONENTS filesystem program_options)
find_library(HYPERSCAN_LIB hs ${CMAKE_INSTALL_PREFIX}/lib)


add_executable(normalizor normalizor.cpp)
target_link_libraries(normalizor Boost::filesystem ${HYPERSCAN_LIB})

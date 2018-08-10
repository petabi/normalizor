cmake_minimum_required(VERSION 3.12)
project(normalizor VERSION 0.2.0 LANGUAGES CXX)

set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_VENDOR "Petabi")
set(CPACK_GENERATOR "TBZ2")
set(CPACK_SOURCE_GENERATOR "TBZ2")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${PROJECT_VERSION}")
set(CPACK_SOURCE_IGNORE_FILES
  "/build(-.*)?/;/.git/;/.vscode/;.DS_Store;~$;${CPACK_SOURCE_IGNORE_FILES}")
include(CPack)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weverything")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-c++98-compat")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-c++98-compat-pedantic")
endif()
if(NOT CMAKE_CXX_FLAGS_DEBUG MATCHES "-O")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
endif()
if(NOT CMAKE_CXX_FLAGS_RELEASE MATCHES "-O")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
endif()

include(GNUInstallDirs)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
find_package(Python3 COMPONENTS Development REQUIRED)
find_package(Boost REQUIRED COMPONENTS filesystem program_options system
  python${Python3_VERSION_MAJOR}${Python3_VERSION_MINOR})
find_package(GTest)

# Workaround for https://bugs.llvm.org/show_bug.cgi?id=33771
set_target_properties(
  Boost::python${Python3_VERSION_MAJOR}${Python3_VERSION_MINOR} PROPERTIES
  INTERFACE_COMPILE_OPTIONS -Wno-zero-as-null-pointer-constant)

add_subdirectory(src)
if(GTEST_FOUND)
  # Workaround for https://bugs.llvm.org/show_bug.cgi?id=33771
  set_target_properties(GTest::GTest PROPERTIES
    INTERFACE_COMPILE_OPTIONS -Wno-zero-as-null-pointer-constant)

  enable_testing()
  add_subdirectory(tests)
endif()
add_subdirectory(tools)

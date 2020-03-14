file(TO_CMAKE_PATH "CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
    message(FATAL_ERROR
      "You cannot build in a source directory (or any directory with a CMakeLists.txt file)."
      "Please make a build subdirectory. Feel free to remove CMakeCache.txt and CMakeFiles."
    )
endif()

# cmake version to be used
cmake_minimum_required(VERSION 3.1...3.14)
if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

include(CheckIPOSupported)
check_ipo_supported(RESULT result)
if(result)
  set_target_properties(PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

project(maxy_diceroll_root)

set(CMAKE_CXX_STANDARD 11)

add_subdirectory(src)
add_subdirectory(tests)

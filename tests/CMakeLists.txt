project(run_test
  DESCRIPTION "Maxy Diceroll tests"
  LANGUAGES CXX
  VERSION 0.1.3
)

include(ExternalProject)
find_package(Threads)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

# Download and install google test
ExternalProject_Add(
  gtest
  URL https://github.com/google/googletest/archive/v1.10.x.zip
  CMAKE_COMMAND cmake -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
  PREFIX ${CMAKE_CURRENT_BINARY_DIR}/gtest
  # disable install step
  INSTALL_COMMAND ""
)

# get gtest source and binary directories from CMake project
ExternalProject_Get_Property(gtest source_dir binary_dir)

# create a libgtest target to be used as a dependency by the test program
add_library(libgtest IMPORTED STATIC GLOBAL)
add_dependencies(libgtest gtest)

# set libgtest properties
set_target_properties(libgtest PROPERTIES
  "IMPORTED_LOCATION" "${binary_dir}/lib/libgtest.a"
  "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
)

# create a libgmock target to be used as a dependency by the test program
add_library(libgmock IMPORTED STATIC GLOBAL)
add_dependencies(libgmock gtest)

# set libgmock properties
set_target_properties(libgmock PROPERTIES
  "IMPORTED_LOCATION" "${binary_dir}/lib/libgmock.a"
  "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
)

# I couldn't make it work with INTERFACE_INCLUDE_DIRECTORIES
include_directories(
  "${source_dir}/googletest/include"
  "${source_dir}/googlemock/include"
  "${CMAKE_SOURCE_DIR}/src/"
)

# debug or not
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_definitions(-DDEBUG)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wpedantic")
endif()

file(GLOB TEST_SRCS *.cpp
  ${CMAKE_SOURCE_DIR}/src/*.cpp
)
list(REMOVE_ITEM TEST_SRCS ${CMAKE_SOURCE_DIR}/src/main.cpp)

add_executable(${PROJECT_NAME} ${TEST_SRCS})
target_link_libraries(${PROJECT_NAME}
  libgtest
  libgmock
  ${CMAKE_THREAD_LIBS_INIT}
)

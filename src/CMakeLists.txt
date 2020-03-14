project(maxy_diceroll
  DESCRIPTION "Roll the bones"
  LANGUAGES CXX
  VERSION 0.1.0
)

file(TO_CMAKE_PATH "CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
    message(FATAL_ERROR
      "You cannot build in a source directory (or any directory with a CMakeLists.txt file)."
      "Please make a build subdirectory. Feel free to remove CMakeCache.txt and CMakeFiles."
    )
endif()

set(CMAKE_CXX_STANDARD 11)

# compiler flags
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_FLAGS_RELEASE "-g -Os")
set(CMAKE_CXX_FLAGS_DEBUG "-g")

# packages
find_package(Threads REQUIRED)

# list of source files
file(GLOB SRCS *.cpp
  *.c
)

# debug or not
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_definitions(-DDEBUG)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wpedantic")
endif()

# defines for mongoose
add_definitions(-DMG_ENABLE_HTTP_STREAMING_MULTIPART)
add_definitions(-DMG_ENABLE_HTTP_WEBSOCKET)

# glue it all together into our executable
add_executable(${PROJECT_NAME} ${SRCS})
target_link_libraries(${PROJECT_NAME}
  ${CMAKE_THREAD_LIBS_INIT}
)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)

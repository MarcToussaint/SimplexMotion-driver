cmake_minimum_required(VERSION 3.4)

project(SimplexMotionDriver)

set(CMAKE_CXX_STANDARD 14 CACHE STRING "C++ version selection")

option(USE_HIDRAW "use low-level hidraw linux kernel" ON)

add_compile_options(
  -g -Wall -fPIC -std=c++14
  )

################################################################################

add_definitions(
  )

################################################################################

include_directories(
  src/
  )

link_directories(
  )

add_library(SimplexMotion SHARED
  src/SimplexMotion.cpp
  src/SimplexMotion-hidraw.cpp
  )

target_link_libraries(SimplexMotion
  hidapi-hidraw
  )

################################################################################

add_executable(simplexMotionTest test/test.cpp)
target_link_libraries(simplexMotionTest SimplexMotion)



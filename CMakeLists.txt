cmake_minimum_required(VERSION 3.4)

project(SimplexMotionDriver)

set(CMAKE_CXX_STANDARD 14 CACHE STRING "C++ version selection")

option(USE_HIDRAW "use low-level hidraw linux kernel" OFF)
option(USE_HIDAPI "use cross-platform hidapi library" ON)

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

if(USE_HIDRAW)
  set(SRCS
    src/SimplexMotion.cpp
    src/SimplexMotion_ControlThread.cpp
    src/SimplexMotion-com.cpp
    src/SimplexMotion-hidraw.cpp)
endif()

if(USE_HIDAPI)
  set(SRCS
    src/SimplexMotion.cpp
    src/SimplexMotion_ControlThread.cpp
    src/SimplexMotion-com.cpp
    src/SimplexMotion-hidapi.cpp)
endif()

################################################################################
  
add_library(SimplexMotion SHARED ${SRCS} )

target_link_libraries(SimplexMotion
  hidapi-hidraw
  )

################################################################################

add_executable(simplexMotionTest test/test.cpp)
target_link_libraries(simplexMotionTest SimplexMotion)



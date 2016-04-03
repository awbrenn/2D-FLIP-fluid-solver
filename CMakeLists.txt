cmake_minimum_required(VERSION 3.3)
project(flip_simulation)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
add_compile_options(-std=c++11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")

include_directories("/usr/include")
find_library(OIIO "OpenImageIO" "/usr/lib")
find_library(GLUT "glut")
find_library(GL "GL")
find_library(GLU "GLU")

set(SOURCE_FILES main.cpp FLIPParticle.cpp FLIPParticle.h FLIPForce.cpp FLIPForce.h FLIPSolver.cpp FLIPSolver.h vector2.cpp vector2.h FLIPOccupancyVolume.cpp FLIPOccupancyVolume.h FLIPVelocityGridPoint.cpp FLIPVelocityGridPoint.h)
add_executable(flip_simulation ${SOURCE_FILES})
target_link_libraries(flip_simulation ${GLUT} ${GL} ${GLU} ${OIIO})

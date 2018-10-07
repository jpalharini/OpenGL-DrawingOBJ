cmake_minimum_required(VERSION 3.10)
project(OpenGL_CG)

set(CMAKE_CXX_STANDARD 11)

include_directories(.)

add_executable(OpenGL_CG
        CMakeLists.txt
        src/Includes.h
        src/Face.cpp
        src/Face.hpp
        src/main.cpp
        src/Group.cpp
        src/Group.h
        src/Mesh.cpp
        src/Mesh.h
        src/math_funcs.cpp
        src/math_funcs.h
        src/Material.cpp
        src/Material.h src/Utils.cpp src/Utils.h)

find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
find_package(GLEW REQUIRED)
find_package (OpenGL REQUIRED)

target_link_libraries(OpenGL_CG ${GLFW_LIBRARIES} ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES})
target_include_directories(OpenGL_CG PRIVATE ${GLFW_INCLUDE_DIRS} ${GLEW_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR} /usr/local/include/glm)

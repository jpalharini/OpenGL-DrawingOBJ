cmake_minimum_required(VERSION 3.10)
project(OpenGL_CG)

set(CMAKE_CXX_STANDARD 11)

include_directories(.)

add_executable(OpenGL_CG
        CMakeLists.txt
        Includes.h
        Rectangle.cpp
        Rectangle.hpp
        main.cpp
        )

find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
find_package(GLEW REQUIRED)
find_package (OpenGL REQUIRED)

target_link_libraries(OpenGL_Colors ${GLFW_LIBRARIES} ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES})
target_include_directories(OpenGL_Colors PRIVATE ${GLFW_INCLUDE_DIRS} ${GLEW_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR} /usr/local/include/glm)

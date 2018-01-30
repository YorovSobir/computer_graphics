project(deferred)
cmake_minimum_required(VERSION 2.8)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

add_subdirectory(lib)

include_directories(
    ${CMAKE_SOURCE_DIR}/include/
#    ${CMAKE_SOURCE_DIR}/lib/glew-2.1.0/include/
    ${CMAKE_SOURCE_DIR}/lib/glm-0.9.7.1/
    ${CMAKE_SOURCE_DIR}/lib/AntTweakBar-1.16/include/
)

set(ALL_LIBS
        ${OPENGL_LIBRARY}
        ${GLUT_LIBRARIES}
        ANTTWEAKBAR_116
        glfw
        ${CMAKE_SOURCE_DIR}/lib/assimp/libassimp.so.3
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")

file(GLOB SRC_FILES ${CMAKE_SOURCE_DIR}/src/*)

add_executable(${PROJECT_NAME} ${SRC_FILES} src/glad.c src/quad.cpp)
target_link_libraries(
    ${PROJECT_NAME}
    ${ALL_LIBS}
    X11)

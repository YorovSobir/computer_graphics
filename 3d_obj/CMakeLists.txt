project(3d_obj)
cmake_minimum_required(VERSION 2.8)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

add_subdirectory(lib/glfw-3.2.1)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(OPENGL_LIBRARY
            ${OPENGL_LIBRARY}
            -lGL -lGLU -lXrandr -lXext -lX11 -lrt
            ${CMAKE_DL_LIBS}
            ${GLFW_LIBRARIES}
            )
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(OPENGL_LIBRARY
            ${OPENGL_LIBRARY}
            ${CMAKE_DL_LIBS}
            ${GLFW_LIBRARIES}
            )
endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

include_directories(
        ${CMAKE_SOURCE_DIR}/include/
)

set(ALL_LIBS
        ${OPENGL_LIBRARY}
        ${GLUT_LIBRARIES}
        glfw
        ${CMAKE_SOURCE_DIR}/lib/assimp/libassimp.so
)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

file(GLOB SRC_FILES ${CMAKE_SOURCE_DIR}/src/*)

add_executable(${PROJECT_NAME} ${SRC_FILES} include/camera.h)
target_link_libraries(
    ${PROJECT_NAME}
    ${ALL_LIBS}
    X11)

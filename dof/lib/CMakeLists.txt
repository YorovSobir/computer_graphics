
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
set(OPENGL_LIBRARY
        ${OPENGL_LIBRARY}
        -lGL -lGLU -lXrandr -lXext -lX11 -lrt
)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

add_subdirectory(glfw-3.2.1)

# AntTweakBar

set(ANTTWEAKBAR_SOURCE
        ${CMAKE_CURRENT_SOURCE_DIR}/AntTweakBar-1.16/src/LoadOGL.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/AntTweakBar-1.16/src/LoadOGLCore.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/AntTweakBar-1.16/src/TwColors.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/AntTweakBar-1.16/src/TwBar.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/AntTweakBar-1.16/src/TwEventGLFW.c
        ${CMAKE_CURRENT_SOURCE_DIR}/AntTweakBar-1.16/src/TwFonts.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/AntTweakBar-1.16/src/TwMgr.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/AntTweakBar-1.16/src/TwOpenGL.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/AntTweakBar-1.16/src/TwOpenGLCore.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/AntTweakBar-1.16/src/TwPrecomp.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/AntTweakBar-1.16/src/TwEventGLUT.c
)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
add_definitions(
        -D_UNIX
)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
add_definitions(
        -D_WINDOWS
)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")


set(ANTTWEAKBAR_HEADERS
        ${CMAKE_CURRENT_SOURCE_DIR}/AntTweakBar-1.16/src/AntPerfTimer.h
        ${CMAKE_CURRENT_SOURCE_DIR}/AntTweakBar-1.16/src/LoadOGL.h
        ${CMAKE_CURRENT_SOURCE_DIR}/AntTweakBar-1.16/src/LoadOGLCore.h
        ${CMAKE_CURRENT_SOURCE_DIR}/AntTweakBar-1.16/src/MiniGLFW.h
        ${CMAKE_CURRENT_SOURCE_DIR}/AntTweakBar-1.16/src/resource.h
        ${CMAKE_CURRENT_SOURCE_DIR}/AntTweakBar-1.16/src/TwBar.h
        ${CMAKE_CURRENT_SOURCE_DIR}/AntTweakBar-1.16/src/TwColors.h
        ${CMAKE_CURRENT_SOURCE_DIR}/AntTweakBar-1.16/src/TwFonts.h
        ${CMAKE_CURRENT_SOURCE_DIR}/AntTweakBar-1.16/src/TwGraph.h
        ${CMAKE_CURRENT_SOURCE_DIR}/AntTweakBar-1.16/src/TwMgr.h
        ${CMAKE_CURRENT_SOURCE_DIR}/AntTweakBar-1.16/src/TwOpenGL.h
        ${CMAKE_CURRENT_SOURCE_DIR}/AntTweakBar-1.16/src/TwOpenGLCore.h
        ${CMAKE_CURRENT_SOURCE_DIR}/AntTweakBar-1.16/src/TwPrecomp.h
)

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/AntTweakBar-1.16/include/
)

add_library( ANTTWEAKBAR_116 STATIC
        ${ANTTWEAKBAR_SOURCE}
        ${ANTTWEAKBAR_HEADERS}
)

target_link_libraries(ANTTWEAKBAR_116
        ${OPENGL_LIBRARY}
        ${EXTRA_LIBS}
)

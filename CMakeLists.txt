﻿cmake_minimum_required(VERSION 3.15)
project(test)

set(CMAKE_BUILD_TYPE "Debug")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/bin/Release)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/bin/Debug)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(FINAL_OUTPUT_BIN_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG})
else()
    set(FINAL_OUTPUT_BIN_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE})
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# FOR IGNORE WARNING
set(CMAKE_EXE_LINKER_FLAGS /NODEFAULTLIB:"Libcmtd.lib")
set(CMAKE_EXE_LINKER_FLAGS /NODEFAULTLIB:"Libcmt.lib")

# 3RDPARTY
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/glm)

# ASSIMP
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/assimp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/assimp/include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty)
link_libraries(assimp-vc143-mt)

# OPENGL GLFW
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/glfw/include/GLFW)

if(MSVC_VERSION LESS 1910)
    link_directories(${CMAKE_CURRENT_SOURCE_DIR}/glfw/lib-vc2019)
else()
    link_directories(${CMAKE_CURRENT_SOURCE_DIR}/glfw/lib-vc2022)
endif()

link_libraries(glfw3_mt glfw3 glfw3dll)

# OPENGL GLAD
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/glad/include/)
file(GLOB glad_src ${CMAKE_CURRENT_SOURCE_DIR}/glad/src/*.c)

# MY BASIC SHAPE
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/glpractice/include/)
file(GLOB gl_shape_src ${CMAKE_CURRENT_SOURCE_DIR}/glpractice/*.cpp)

# target_link_libraries(glfw3)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)

# include imgui
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui/backends)
file(GLOB imgui_src ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui/*.cpp)
file(GLOB imgui_backend_src ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui/backends/*.cpp)

# output exectuables
file(GLOB app_src ${CMAKE_CURRENT_SOURCE_DIR}/learning_content/*.cpp)

foreach(_var ${app_src})
    get_filename_component(app_name ${_var} NAME_WE)
    add_executable(${app_name} ${glad_src} ${gl_shape_src} ${_var} ${imgui_src} ${imgui_backend_src})
    message("output exectuable:${app_name}")
endforeach()

# copy resources
add_custom_target(copy_resources ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/resource
    ${FINAL_OUTPUT_BIN_PATH}/resource
)

add_custom_target(
    CopyAssimpDll ALL
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/assimp/assimp-vc143-mt.dll
    ${FINAL_OUTPUT_BIN_PATH}/assimp-vc143-mt.dll
    COMMENT "Copying assimp-vc143-mt.dll to bin/Debug/3rdparty/"
)
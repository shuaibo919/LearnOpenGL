cmake_minimum_required(VERSION 3.15)
project(test)

set(CMAKE_BUILD_TYPE "Debug")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/)
set(CXX_STANDARD 17)
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
# if(${MSVC_VERSION} LESS 1930)
# link_directories(${CMAKE_CURRENT_SOURCE_DIR}/glfw/lib-vc2019)
# else()
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/glfw/lib-vc2022)
# endif()
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
    add_executable(${app_name}  ${glad_src} ${gl_shape_src} ${_var} ${imgui_src} ${imgui_backend_src})
	message("output exectuable:${app_name}")
endforeach()
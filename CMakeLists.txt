cmake_minimum_required(VERSION 3.15)
project(test)
SET(CMAKE_BUILD_TYPE "Debug")
file(GLOB src ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/)

# FOR IGNORE WARNING
set(CMAKE_EXE_LINKER_FLAGS /NODEFAULTLIB:"Libcmtd.lib")

# OPENGL GLFW
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/glfw/include/GLFW)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/glfw/lib-vc2022)
link_libraries(glfw3_mt glfw3 glfw3dll)
# OPENGL GLAD
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/glad/include/)
file(GLOB glad_src ${CMAKE_CURRENT_SOURCE_DIR}/glad/src/*.c)
# target_link_libraries(glfw3)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)
add_executable(test  ${glad_src} ${src})
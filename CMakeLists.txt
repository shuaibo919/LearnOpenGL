cmake_minimum_required(VERSION 3.15)
project(amateur)
file(GLOB src ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/)

# OPENGL GLFW
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/glfw/include/GLFW)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/glfw/lib-vc2022)
link_libraries(glfw3_mt glfw3 glfw3dll)
# OPENGL GLAD
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/glad/include/)
file(GLOB glad_src ${CMAKE_CURRENT_SOURCE_DIR}/glad/src/*.cpp)
link_libraries(glfw3_mt glfw3 glfw3dll)
# target_link_libraries(glfw3)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)
add_executable(test ${src} ${glad_src})
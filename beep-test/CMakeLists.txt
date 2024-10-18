cmake_minimum_required(VERSION 3.5.0)

get_filename_component(DIR_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
message(STATUS "Current directory name: ${DIR_NAME}")

project(${DIR_NAME} VERSION 0.1.0)

FILE(GLOB_RECURSE src_source *.c)
add_executable(${PROJECT_NAME} ${src_source})
target_link_libraries(${PROJECT_NAME} PRIVATE pthread)

include(${CMAKE_SOURCE_DIR}/CopyExecutable.cmake)
copy_executable(${PROJECT_NAME})
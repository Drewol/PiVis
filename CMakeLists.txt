cmake_minimum_required(VERSION 3.8)

project(PIVIS)
find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(PkgConfig REQUIRED)


option(USE_SYSTEM_GRPC "Use system installed gRPC" OFF)
if(USE_SYSTEM_GRPC)
  # Find system-installed gRPC
  find_package(gRPC CONFIG REQUIRED)
else()
  # Build gRPC using FetchContent or add_subdirectory
  add_subdirectory(grpc)
endif()

file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/interfaces)
file(GLOB protofiles "proto/*.proto")
execute_process(COMMAND which grpc_cpp_plugin
                OUTPUT_STRIP_TRAILING_WHITESPACE
                OUTPUT_VARIABLE GRPC_PLUGIN)
message(STATUS "GRPC_PLUGIN: ${GRPC_PLUGIN}")
foreach(file ${protofiles})
    get_filename_component(filename ${file} NAME)
    message(STATUS "Adding: ${filename}")
    execute_process(COMMAND protoc --plugin=protoc-gen-grpc=${GRPC_PLUGIN} --proto_path=${CMAKE_CURRENT_SOURCE_DIR}/proto --grpc_out=${CMAKE_CURRENT_SOURCE_DIR}/interfaces ${file})
    execute_process(COMMAND protoc --proto_path=${CMAKE_CURRENT_SOURCE_DIR}/proto --cpp_out=${CMAKE_CURRENT_SOURCE_DIR}/interfaces ${file})
endforeach()

file(GLOB interface_sources "interfaces/*")
add_library(interfaces ${interface_sources})
set_target_properties(interfaces PROPERTIES LINKER_LANGUAGE CXX)

add_library(nanovg nanovg/src/nanovg.c)
target_include_directories(nanovg PRIVATE nanovg/src)
set_target_properties(nanovg PROPERTIES LINKER_LANGUAGE C)


file(GLOB sources "servers/src/*.cpp")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/interfaces servers/include nanovg/src /opt/vc/include)
add_executable(pivis ${sources})
message(STATUS "OPENGL_INCLUDE_DIR: ${OPENGL_INCLUDE_DIR}")
target_include_directories(pivis PRIVATE ${OPENGL_INCLUDE_DIR})
target_link_libraries(pivis glfw)
target_link_libraries(pivis GLEW)
target_link_libraries(pivis OpenGL::GL)
target_link_libraries(pivis interfaces)
target_link_libraries(pivis grpc++)
target_link_libraries(pivis nanovg)

cmake_minimum_required(VERSION 3.8)

project(PIVIS)
find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)
if(NOT MSVC)
    find_package(PkgConfig REQUIRED)
endif(NOT MSVC)

option(USE_SYSTEM_GRPC "Use system installed gRPC" OFF)
if(USE_SYSTEM_GRPC)
  # Find system-installed gRPC
  find_package(gRPC CONFIG REQUIRED)
  find_package(Protobuf REQUIRED)
else()
  # Build gRPC using FetchContent or add_subdirectory
  add_subdirectory(grpc)
endif()

file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/interfaces)
file(GLOB protofiles "proto/*.proto")
if(MSVC)
  set(GRPC_PLUGIN "C:\\protoc\\bin\\grpc_cpp_plugin.exe")
  #GLEW
  add_library(GLEW
      ${CMAKE_CURRENT_SOURCE_DIR}/glew/include/GL/glew.h
      ${CMAKE_CURRENT_SOURCE_DIR}/glew/include/GL/glxew.h
      ${CMAKE_CURRENT_SOURCE_DIR}/glew/include/GL/wglew.h
      ${CMAKE_CURRENT_SOURCE_DIR}/glew/src/glew.c
  )
  # GLEW is included statically and also doesn't need GLU(Which doesn't even exist on linux)
  target_compile_definitions(GLEW PUBLIC -DGLEW_NO_GLU -DGLEW_STATIC)
  target_include_directories(GLEW PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/glew/include)
else()
  execute_process(COMMAND which grpc_cpp_plugin
                  OUTPUT_STRIP_TRAILING_WHITESPACE
                  OUTPUT_VARIABLE GRPC_PLUGIN)
endif(MSVC)

message(STATUS "GRPC_PLUGIN: ${GRPC_PLUGIN}")
foreach(file ${protofiles})
    get_filename_component(filename ${file} NAME)
    message(STATUS "Adding: ${filename}")
    execute_process(COMMAND protoc --plugin=protoc-gen-grpc=${GRPC_PLUGIN} --proto_path=${CMAKE_CURRENT_SOURCE_DIR}/proto --grpc_out=${CMAKE_CURRENT_SOURCE_DIR}/interfaces ${file})
    execute_process(COMMAND protoc --proto_path=${CMAKE_CURRENT_SOURCE_DIR}/proto --cpp_out=${CMAKE_CURRENT_SOURCE_DIR}/interfaces ${file})
endforeach()

file(GLOB interface_sources "interfaces/*")
add_library(interfaces ${interface_sources})
target_link_libraries(interfaces grpc++)
set_target_properties(interfaces PROPERTIES LINKER_LANGUAGE CXX)
target_compile_features(interfaces PUBLIC cxx_std_17)

add_library(nanovg nanovg/src/nanovg.c)
target_include_directories(nanovg PRIVATE nanovg/src)
set_target_properties(nanovg PROPERTIES LINKER_LANGUAGE C)


file(GLOB_RECURSE sources services/*.cpp services/*.hpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/interfaces services/include nanovg/src /opt/vc/include)
add_executable(pivis ${sources})
target_compile_features(pivis PUBLIC cxx_std_17)
message(STATUS "OPENGL_INCLUDE_DIR: ${OPENGL_INCLUDE_DIR}")
target_include_directories(pivis PRIVATE ${OPENGL_INCLUDE_DIR})
target_link_libraries(pivis glfw)
target_link_libraries(pivis GLEW)
target_link_libraries(pivis OpenGL::GL)
target_link_libraries(pivis interfaces)
target_link_libraries(pivis grpc++)
target_link_libraries(pivis nanovg)

if(MSVC)
  set_target_properties(pivis PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")
  set_directory_properties(PROPERTY VS_STARTUP_PROJECT pivis)
endif(MSVC)
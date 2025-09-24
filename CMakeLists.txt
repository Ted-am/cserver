cmake_minimum_required(VERSION 3.20)

project(cserver LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(USE_SOEM "Build with SOEM EtherCAT master" OFF)

add_compile_definitions($<$<BOOL:${USE_SOEM}>:USE_SOEM>)

if(MSVC)
  add_compile_options(/W4 /permissive-)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_executable(cserver ${SRC_FILES})

if(WIN32)
  # Link Ws2_32 for sockets if needed later
  target_link_libraries(cserver PRIVATE ws2_32)
endif()

install(TARGETS cserver RUNTIME DESTINATION bin)



cmake_minimum_required(VERSION 3.0)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_FLAGS "-Wall -Wextra --std=c++14 -stdlib=libc++ -I/usr/include/libcxxabi")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

file(GLOB SOURCES src/*.cc src/*.h)
FIND_PACKAGE(Boost COMPONENTS
    program_options
    exception
    REQUIRED)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS}) 
    add_executable(main ${SOURCES})
    target_link_libraries(main ${Boost_LIBRARIES})
endif()

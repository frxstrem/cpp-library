cmake_minimum_required(VERSION 3.3)
project(cpp-library)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS -fconcepts)

set(Boost_USE_STATIC_LIBS        ON) # only find static libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)

find_package(Boost 1.64.0
  REQUIRED context)

message("Boost include path: ${Boost_INCLUDE_DIR}")
message("Boost library path: ${Boost_LIBRARY_DIR}")

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

include_directories(include)
add_executable(main src/main.cc)
target_link_libraries(main ${Boost_LIBRARIES})

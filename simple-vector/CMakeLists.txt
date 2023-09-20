cmake_minimum_required(VERSION 3.11)

project(Simple_vector CXX)

set(CMAKE_CXX_STANDARD 17)

if (CMAKE_SYSTEM_NAME MATCHES "^MINGW")
    set(SYSTEM_LIBS -lstdc++)
else()
    set(SYSTEM_LIBS)
endif()

add_executable(simple_vector main.cpp array_ptr.h simple_vector.h)
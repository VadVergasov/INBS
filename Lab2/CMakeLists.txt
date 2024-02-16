cmake_minimum_required(VERSION 3.16)

project("Lab2" CXX)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE SRC "src/*.h" "src/*.cpp")

add_library(Libs ${SRC})
link_libraries(Libs)

file(GLOB APP_SOURCES "apps/*.cpp")
foreach(sourcefile ${APP_SOURCES})
    string(REGEX REPLACE ".+/(.+)\\..*" "\\1" name ${sourcefile})
    add_executable(${name} MACOSX_BUNDLE ${sourcefile})
    target_include_directories(${name} PUBLIC src)
endforeach(sourcefile ${APP_SOURCES})

cmake_minimum_required(VERSION 3.12)
project(syn_flood)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

file(GLOB APP_SOURCES "*.cpp")

foreach(sourcefile ${APP_SOURCES})
    string(REGEX REPLACE ".+/(.+)\\..*" "\\1" name ${sourcefile})
    add_executable(${name} ${sourcefile})
    target_include_directories(${name} PUBLIC src)
    TARGET_LINK_LIBRARIES( ${name} LINK_PUBLIC ${Boost_LIBRARIES} )
endforeach(sourcefile ${APP_SOURCES})

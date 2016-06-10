cmake_minimum_required(VERSION 3.3 FATAL_ERROR)

project(ckjson VERSION 0.1.0 LANGUAGES C CXX)

# All subprojects
file(GLOB_RECURSE RAPIDJSON_INCLUDES
     RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
     rapidjson/*.h
     )

set(PROJECT_SOURCES
    "ckjson/json.cpp"
    )

set(PROJECT_INCLUDES
    "ckjson/jsontypes.hpp"
    "ckjson/json.hpp"
    "ckjson/jsonstreambuf.hpp"
    ${RAPIDJSON_INCLUDES}
    )

get_directory_property(HAS_PARENT_DIRECTORY PARENT_DIRECTORY)

if(HAS_PARENT_DIRECTORY)
    set(ckjson_SOURCES ${PROJECT_SOURCES} PARENT_SCOPE)
    set(ckjson_INCLUDES ${PROJECT_INCLUDES} PARENT_SCOPE)
endif(HAS_PARENT_DIRECTORY)


cmake_minimum_required(VERSION 3.3 FATAL_ERROR)

project(ckentity VERSION 0.1.0 LANGUAGES CXX)

# All subprojects
set(PROJECT_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/entitystore.cpp"
   )

set(PROJECT_INCLUDES
    "${CMAKE_CURRENT_SOURCE_DIR}/entity.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/entitystore.hpp"
   )

if(NOT DISABLE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif(NOT DISABLE_TESTS)

get_directory_property(HAS_PARENT_DIRECTORY PARENT_DIRECTORY)

if(HAS_PARENT_DIRECTORY)
    set(ckentity_SOURCES ${PROJECT_SOURCES} PARENT_SCOPE)
    set(ckentity_INCLUDES ${PROJECT_INCLUDES} PARENT_SCOPE)
endif(HAS_PARENT_DIRECTORY)



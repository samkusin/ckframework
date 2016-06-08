cmake_minimum_required(VERSION 3.3 FATAL_ERROR)

project(ckio VERSION 0.1.0 LANGUAGES C CXX)

# All subprojects
set(PROJECT_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/file.c"
   )

set(PROJECT_INCLUDES
    "${CMAKE_CURRENT_SOURCE_DIR}/file.h"
   )

if(NOT DISABLE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif(NOT DISABLE_TESTS)

get_directory_property(HAS_PARENT_DIRECTORY PARENT_DIRECTORY)

if(HAS_PARENT_DIRECTORY)
    set(ckio_SOURCES ${PROJECT_SOURCES} PARENT_SCOPE)
    set(ckio_INCLUDES ${PROJECT_INCLUDES} PARENT_SCOPE)
endif(HAS_PARENT_DIRECTORY)



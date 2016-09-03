cmake_minimum_required(VERSION 3.3 FATAL_ERROR)

project(cktask VERSION 0.1.0 LANGUAGES C CXX)

# All subprojects
set(PROJECT_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/task.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/taskscheduler.cpp"
   )

set(PROJECT_INCLUDES
    "${CMAKE_CURRENT_SOURCE_DIR}/task.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/taskscheduler.hpp"
   )


get_directory_property(HAS_PARENT_DIRECTORY PARENT_DIRECTORY)

if(HAS_PARENT_DIRECTORY)
    set(cktask_SOURCES ${PROJECT_SOURCES} PARENT_SCOPE)
    set(cktask_INCLUDES ${PROJECT_INCLUDES} PARENT_SCOPE)
endif(HAS_PARENT_DIRECTORY)




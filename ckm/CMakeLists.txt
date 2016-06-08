cmake_minimum_required(VERSION 3.3 FATAL_ERROR)

project(cinek VERSION 0.1.0 LANGUAGES C CXX)

# All subprojects
set(PROJECT_SOURCES )

set(PROJECT_INCLUDES
    "${CMAKE_CURRENT_SOURCE_DIR}/aabb.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/mathtypes.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/mathtypes.inl"
    "${CMAKE_CURRENT_SOURCE_DIR}/math.inl"
    "${CMAKE_CURRENT_SOURCE_DIR}/equation.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/equation.inl"
    "${CMAKE_CURRENT_SOURCE_DIR}/keyframe.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/keyframe.inl"
    "${CMAKE_CURRENT_SOURCE_DIR}/sequence.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/sequence.inl"
   )

if(NOT DISABLE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif(NOT DISABLE_TESTS)

get_directory_property(HAS_PARENT_DIRECTORY PARENT_DIRECTORY)

if(HAS_PARENT_DIRECTORY)
    set(ckm_SOURCES ${PROJECT_SOURCES} PARENT_SCOPE)
    set(ckm_INCLUDES ${PROJECT_INCLUDES} PARENT_SCOPE)
endif(HAS_PARENT_DIRECTORY)




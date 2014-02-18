cmake_minimum_required(VERSION 2.8.8)

project( framework )

# setup options and needed variables.
if( DEFINED CINEKSDK_DIR )
    set( CMAKE_MODULE_PATH ${CINEKSDK_DIR}/cmake ${CMAKE_MODULE_PATH} )
endif( )

include( CineKSDKBuild )

include_directories( ${CINEKSDK_ROOT} )

# files.
set( PROJECT_INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}/defs.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/allocator.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/string.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vector.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/map.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/errorreporter.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/memorypool.hpp
)

set( PROJECT_SOURCES
)

# Targets
# Define project sources and includes for the parent
CineKSDK_Module_Target( framework "${PROJECT_INCLUDES}" "${PROJECT_SOURCES}")

# Tests
set( TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/framework/ckjobtest.cpp
)
CineKSDK_Test_Target( framework "${TEST_SOURCES}" )

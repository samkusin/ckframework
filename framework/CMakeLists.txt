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
    ${CMAKE_CURRENT_SOURCE_DIR}/ckdefs.h
    ${CMAKE_CURRENT_SOURCE_DIR}/types.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/debug.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/allocator.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/string.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vector.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/map.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/memorypool.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/job.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/jobqueue.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/jobscheduler.hpp  
)

set( PROJECT_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/debug.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/allocator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/string.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/jobqueue.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/jobscheduler.cpp  
)

# Targets
# Define project sources and includes for the parent
CineKSDK_Module_Target( framework "${PROJECT_INCLUDES}" "${PROJECT_SOURCES}")

# Tests
set( TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/framework/ckjobtest.cpp
)
CineKSDK_Test_Target( framework "${TEST_SOURCES}" )

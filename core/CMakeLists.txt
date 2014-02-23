cmake_minimum_required(VERSION 2.8.8)

project( core )

# setup options and needed variables.
if( DEFINED CINEKSDK_DIR )
    set( CMAKE_MODULE_PATH ${CINEKSDK_DIR}/cmake ${CMAKE_MODULE_PATH} )
endif( )

include( CineKSDKBuild )

include_directories( ${CINEKSDK_ROOT} )

# files.
set( PROJECT_INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}/cktypes.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ckdefs.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ckalloc.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ckdebug.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ckfspath.h
)

set( PROJECT_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/ckalloc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ckdebug.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ckfspath.c
)

# Targets
# Define project sources and includes for the parent
CineKSDK_Module_Target( core "${PROJECT_INCLUDES}" "${PROJECT_SOURCES}")

# Tests
#set( TEST_SOURCES
#    ${CMAKE_CURRENT_SOURCE_DIR}/test/ckpooltest.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/test/cktimertest.cpp
#)
#CineKSDK_Test_Target( core "${TEST_SOURCES}" )

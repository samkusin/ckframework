cmake_minimum_required(VERSION 2.8.8)

project( overview )

# require CINEKSDK_DIR 
if( DEFINED CINEKSDK_DIR )
    set( CMAKE_MODULE_PATH ${CINEKSDK_DIR}/cmake )
endif( )

include( CineKSDKBuild )

set( PROJECT_INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}/types.h
    ${CMAKE_CURRENT_SOURCE_DIR}/alloc.h
    ${CMAKE_CURRENT_SOURCE_DIR}/maptypes.h
    ${CMAKE_CURRENT_SOURCE_DIR}/debug.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/stage.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/map.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/grid.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/viewpoint.hpp
)
set( PROJECT_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/stage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/map.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/viewpoint.cpp
)

# Targets
CineKSDK_Module_Target( overview "${PROJECT_INCLUDES}" "${PROJECT_SOURCES}" )

# Projects
set( TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/test/stagetest.cpp
)
CineKSDK_Test_Target( overview "${TEST_SOURCES}" )
